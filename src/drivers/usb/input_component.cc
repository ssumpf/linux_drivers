/*
 * \brief   DDE Linux 2.6 Input driver for USB HID
 * \author  Christian Helmuth
 * \author  Christian Prochaska
 * \date    2011-07-15
 */

#include <base/printf.h>
#include <base/rpc_server.h>
#include <input/component.h>
#include <os/ring_buffer.h>
#include <util/xml_node.h>

extern "C" {
#include <dde_linux26/input.h>
}

using namespace Genode;


/*********************
 ** Input component **
 *********************/

typedef Ring_buffer<Input::Event, 512> Input_ring_buffer;

static Input_ring_buffer ev_queue;

namespace Input {

	void event_handling(bool enable) { }
	bool event_pending() { return !ev_queue.empty(); }
	Event get_event() { return ev_queue.get(); }

}


/**
 * Input event call-back function
 */
static void input_callback(enum dde_linux26_input_event type,
                           unsigned keycode,
                           int absolute_x, int absolute_y,
                           int relative_x, int relative_y)
{
	Input::Event::Type t = Input::Event::INVALID;
	switch (type) {
	case EVENT_TYPE_PRESS:   t = Input::Event::PRESS; break;
	case EVENT_TYPE_RELEASE: t = Input::Event::RELEASE; break;
	case EVENT_TYPE_MOTION:  t = Input::Event::MOTION; break;
	case EVENT_TYPE_WHEEL:   t = Input::Event::WHEEL; break;
	}

	try {
		ev_queue.add(Input::Event(t, keycode,
		                             absolute_x, absolute_y,
		                             relative_x, relative_y));
	} catch (Input_ring_buffer::Overflow) {
		PWRN("input ring buffer overflow");
	}
}


void start_input_service(Rpc_entrypoint *ep, Xml_node hid_subnode)
{
	static Input::Root input_root(ep, env()->heap());
	env()->parent()->announce(ep->manage(&input_root));

	dde_linux26_input_init(input_callback);
}
