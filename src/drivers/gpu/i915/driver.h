/*
 * \brief  Intel i915 GPU driver interface
 * \author Norman Feske
 * \date   2010-07-28
 */

/* GPU driver interface */
#include <gpu/driver.h>

/* Genode includes */
#include <pci_session/connection.h>
#include <pci_device/capability.h>

struct drm_device;

class I915_gpu_driver : public Gpu_driver
{
	private:

		Pci::Connection        _pci;
		Pci::Device_capability _bridge_cap;
		Pci::Device_capability _gpu_cap;

		Pci::Device_capability _find_bridge();
		Pci::Device_capability _find_gpu();

		drm_device *_dev;

	public:

		/**
		 * Constructor
		 */
		I915_gpu_driver();

		Client *create_client();
		uint16_t device_id();
		int ioctl(Client *client, int request, void *arg);
		void *map_buffer_object(Client *client, long handle);
		void unmap_buffer_object(Client *client, long handle);
};
