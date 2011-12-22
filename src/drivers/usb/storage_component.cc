/*
 * \brief   DDE Linux 2.6 block component for USB storage
 * \author  Christian Helmuth
 * \author  Christian Prochaska
 * \date    2009-10-12
 */

#include <base/printf.h>
#include <base/rpc_server.h>
#include <block_session/rpc_object.h>
#include <os/ring_buffer.h>
#include <root/component.h>
#include <util/xml_node.h>

extern "C" {
#include <dde_kit/lock.h>
#include <dde_kit/thread.h>
#include <dde_linux26/general.h>
#include <dde_linux26/block.h>
}


using namespace Genode;

/**
 * Startup synchronization
 */
static struct dde_kit_lock *plugin_lock;
static int current_index = -1;

static void plugin_handler(int usb_index)
{
	current_index = usb_index;
	dde_kit_lock_unlock(plugin_lock);
}


namespace Block {

	class Session_component : public Session_rpc_object
	{
		private:

			/**
			 * Thread handling the requests of an open block session.
			 */
			class Tx_thread : public Thread<8192>
			{
				private:

					Session_component *_session;  /* corresponding session object */
					int                _usb_index;

				public:

					/**
					 * Constructor
					 */
					Tx_thread(Session_component *session,
					          int                usb_index)
					: Thread<8192>("tx_thread"),
					  _session(session),
					  _usb_index(usb_index) { }

					/**
					 * Thread's entry function.
					 */
					void entry()
					{
						dde_linux26_process_add_worker("tx_thread");

						Session_component::Tx::Sink *tx_sink =
							_session->tx_sink();
						Block::Packet_descriptor packet;

						/* signal preparedness to server activation */
						_session->tx_ready();

						/* handle requests */
						while (true) {

							/* blocking-get packet from client */
							packet = tx_sink->get_packet();
							if (!packet.valid()) {
								PWRN("received invalid packet");
								continue;
							}

							/* sanity check block number */
							if ((packet.block_number() + packet.block_count()
								 > (Genode::size_t)dde_linux26_block_count(_usb_index))
								|| packet.block_number() < 0) {
								PWRN("requested blocks %zd-%zd out of range!",
								     packet.block_number(),
								     packet.block_number() + packet.block_count());
								continue;
							}

							switch (packet.operation()) {
							case Block::Packet_descriptor::READ:
								{
									size_t size = packet.block_count() * 512;
									unsigned char *dde_blocks = (unsigned char*)dde_linux26_block_malloc(size);

									if (!dde_blocks) {
										packet.succeeded(false);
										break;
									}

									for (Genode::size_t i = 0; i < packet.block_count(); i++)
										dde_linux26_block_read(_usb_index, packet.block_number() + i,
										                       &dde_blocks[i * 512]);

									/* copy block content to packet payload */
									memcpy(tx_sink->packet_content(packet),
										   dde_blocks, size);

									dde_linux26_block_free(dde_blocks);

									packet.succeeded(true);
									break;
								}
							case Block::Packet_descriptor::WRITE:
								{
									size_t size = packet.block_count() * 512;
									unsigned char *dde_blocks = (unsigned char*)dde_linux26_block_malloc(size);

									if (!dde_blocks) {
										packet.succeeded(false);
										break;
									}

									/* copy packet payload to block content */
									memcpy(dde_blocks, tx_sink->packet_content(packet), size);

									for (Genode::size_t i = 0; i < packet.block_count(); i++)
										dde_linux26_block_write(_usb_index, packet.block_number() + i,
										                        &dde_blocks[i * 512]);

									dde_linux26_block_free(dde_blocks);

									packet.succeeded(true);
									break;
								}
							default:
								PWRN("unsupported operation");
								packet.succeeded(false);
								continue;
							}

							/* acknowledge packet to the client */
							if (!tx_sink->ready_to_ack())
								PDBG("need to wait until ready-for-ack");
							tx_sink->acknowledge_packet(packet);
						}
					}

					friend class Session_component;
			};

			int       _usb_index;
			Semaphore _startup_sema;  /* thread startup sync */
			Tx_thread _tx_thread;     /* thread handling block requests */

		public:

			/**
			 * Constructor
			 *
			 * \param tx_buf_size  buffer size for tx channel
			 * \param dev_addr     address of attached file
			 * \param dev_size     size of attached file
			 */
			Session_component(Genode::Dataspace_capability tx_ds,
			                  Genode::Rpc_entrypoint *ep,
			                  int usb_index)
			: Session_rpc_object(tx_ds, *ep),
			  _usb_index(usb_index),
			  _startup_sema(0),
			  _tx_thread(this, _usb_index)
			{
				_tx_thread.start();

				/* block until thread is ready to handle requests */
				_startup_sema.down();
			}

			/**
			 * Signal indicating that transmit thread is ready
			 */
			void tx_ready() { _startup_sema.up(); }


			/*****************************
			 ** Block session interface **
			 *****************************/

			void info(size_t *blk_count, size_t *blk_size, Operations *ops)
			{
				*blk_count = dde_linux26_block_count(_usb_index);
				*blk_size  = 512;
				ops->set_operation(Block::Packet_descriptor::READ);
				ops->set_operation(Block::Packet_descriptor::WRITE);
			}
	};


	/*
	 * Shortcut for single-client root component
	 */
	typedef Root_component<Session_component, Single_client> Root_component;


	/**
	 * Root component, handling new session requests
	 */
	class Root : public Root_component
	{
		protected:

			Session_component *_create_session(const char *args)
			{
				size_t ram_quota =
					Arg_string::find_arg(args, "ram_quota"  ).ulong_value(0);
				size_t tx_buf_size =
					Arg_string::find_arg(args, "tx_buf_size").ulong_value(0);

				/*
				 * Check if donated ram quota suffices for both
				 * communication buffers. Also check both sizes separately
				 * to handle a possible overflow of the sum of both sizes.
				 */
				Genode::size_t session_size =
					max((Genode::size_t)4096, sizeof(Session_component) + sizeof(Allocator_avl));
				if (ram_quota < session_size)
					throw Root::Quota_exceeded();
				if (tx_buf_size > ram_quota - session_size) {
					PERR("insufficient 'ram_quota', got %zd, need %zd",
					     ram_quota, tx_buf_size + session_size);
					throw Root::Quota_exceeded();
				}

				dde_kit_lock_lock(plugin_lock);

				return new (md_alloc())
				       Session_component(env()->ram_session()->alloc(tx_buf_size),
				                         ep(), current_index);
			}

		public:

			/**
			 * Constructor
			 *
			 * \param session_ep  session entrypoint
			 * \param md_alloc    meta-data allocator
			 * \param usb_index
			 */
			Root(Rpc_entrypoint *session_ep, Allocator *md_alloc)
			: Root_component(session_ep, md_alloc) { }
	};
}


void start_storage_service(Rpc_entrypoint *ep, Xml_node storage_subnode)
{
	dde_kit_lock_init(&plugin_lock);
	dde_kit_lock_lock(plugin_lock);
	dde_linux26_block_register_plugin_callback(plugin_handler);

	static Block::Root blk_root(ep, env()->heap());
	env()->parent()->announce(ep->manage(&blk_root));
}
