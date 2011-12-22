/*
 * \brief  Probe for supported Intel GPU
 * \author Norman Feske
 * \date   2010-07-20
 *
 * Device info taken from drivers/gpu/drm/i915/i915_drv.c
 */

/* DRM includes */
#include <drmP.h>           /* for 'struct pci_device_id' */
#include <i915/i915_drv.h>  /* for 'struct intel_device_info' */


#define INTEL_VGA_DEVICE(id, info) {		\
	.class = PCI_CLASS_DISPLAY_VGA << 8,	\
	.class_mask = 0xffff00,			\
	.vendor = 0x8086,			\
	.device = id,				\
	.subvendor = PCI_ANY_ID,		\
	.subdevice = PCI_ANY_ID,		\
	.driver_data = (unsigned long) info }

const static struct intel_device_info intel_i830_info = {
	.is_i8xx = 1, .is_mobile = 1, .cursor_needs_physical = 1,
};

const static struct intel_device_info intel_845g_info = {
	.is_i8xx = 1,
};

const static struct intel_device_info intel_i85x_info = {
	.is_i8xx = 1, .is_i85x = 1, .is_mobile = 1,
	.cursor_needs_physical = 1,
};

const static struct intel_device_info intel_i865g_info = {
	.is_i8xx = 1,
};

const static struct intel_device_info intel_i915g_info = {
	.is_i915g = 1, .is_i9xx = 1, .cursor_needs_physical = 1,
};
const static struct intel_device_info intel_i915gm_info = {
	.is_i9xx = 1,  .is_mobile = 1,
	.cursor_needs_physical = 1,
};
const static struct intel_device_info intel_i945g_info = {
	.is_i9xx = 1, .has_hotplug = 1, .cursor_needs_physical = 1,
};
const static struct intel_device_info intel_i945gm_info = {
	.is_i945gm = 1, .is_i9xx = 1, .is_mobile = 1,
	.has_hotplug = 1, .cursor_needs_physical = 1,
};

const static struct intel_device_info intel_i965g_info = {
	.is_i965g = 1, .is_i9xx = 1, .has_hotplug = 1,
};

const static struct intel_device_info intel_i965gm_info = {
	.is_i965g = 1, .is_mobile = 1, .is_i965gm = 1, .is_i9xx = 1,
	.is_mobile = 1, .has_fbc = 1, .has_rc6 = 1,
	.has_hotplug = 1,
};

const static struct intel_device_info intel_g33_info = {
	.is_g33 = 1, .is_i9xx = 1, .need_gfx_hws = 1,
	.has_hotplug = 1,
};

const static struct intel_device_info intel_g45_info = {
	.is_i965g = 1, .is_g4x = 1, .is_i9xx = 1, .need_gfx_hws = 1,
	.has_pipe_cxsr = 1,
	.has_hotplug = 1,
};

const static struct intel_device_info intel_gm45_info = {
	.is_i965g = 1, .is_mobile = 1, .is_g4x = 1, .is_i9xx = 1,
	.is_mobile = 1, .need_gfx_hws = 1, .has_fbc = 1, .has_rc6 = 1,
	.has_pipe_cxsr = 1,
	.has_hotplug = 1,
};

const static struct intel_device_info intel_pineview_info = {
	.is_g33 = 1, .is_pineview = 1, .is_mobile = 1, .is_i9xx = 1,
	.need_gfx_hws = 1,
	.has_hotplug = 1,
};

const static struct intel_device_info intel_ironlake_d_info = {
	.is_ironlake = 1, .is_i965g = 1, .is_i9xx = 1, .need_gfx_hws = 1,
	.has_pipe_cxsr = 1,
	.has_hotplug = 1,
};

const static struct intel_device_info intel_ironlake_m_info = {
	.is_ironlake = 1, .is_mobile = 1, .is_i965g = 1, .is_i9xx = 1,
	.need_gfx_hws = 1, .has_rc6 = 1,
	.has_hotplug = 1,
};

const static struct intel_device_info intel_sandybridge_d_info = {
	.is_i965g = 1, .is_i9xx = 1, .need_gfx_hws = 1,
	.has_hotplug = 1, .is_gen6 = 1,
};

const static struct intel_device_info intel_sandybridge_m_info = {
	.is_i965g = 1, .is_mobile = 1, .is_i9xx = 1, .need_gfx_hws = 1,
	.has_hotplug = 1, .is_gen6 = 1,
};

const static struct pci_device_id pciidlist[] = {
	INTEL_VGA_DEVICE(0x3577, &intel_i830_info),
	INTEL_VGA_DEVICE(0x2562, &intel_845g_info),
	INTEL_VGA_DEVICE(0x3582, &intel_i85x_info),
	INTEL_VGA_DEVICE(0x358e, &intel_i85x_info),
	INTEL_VGA_DEVICE(0x2572, &intel_i865g_info),
	INTEL_VGA_DEVICE(0x2582, &intel_i915g_info),
	INTEL_VGA_DEVICE(0x258a, &intel_i915g_info),
	INTEL_VGA_DEVICE(0x2592, &intel_i915gm_info),
	INTEL_VGA_DEVICE(0x2772, &intel_i945g_info),
	INTEL_VGA_DEVICE(0x27a2, &intel_i945gm_info),
	INTEL_VGA_DEVICE(0x27ae, &intel_i945gm_info),
	INTEL_VGA_DEVICE(0x2972, &intel_i965g_info),
	INTEL_VGA_DEVICE(0x2982, &intel_i965g_info),
	INTEL_VGA_DEVICE(0x2992, &intel_i965g_info),
	INTEL_VGA_DEVICE(0x29a2, &intel_i965g_info),
	INTEL_VGA_DEVICE(0x29b2, &intel_g33_info),
	INTEL_VGA_DEVICE(0x29c2, &intel_g33_info),
	INTEL_VGA_DEVICE(0x29d2, &intel_g33_info),
	INTEL_VGA_DEVICE(0x2a02, &intel_i965gm_info),
	INTEL_VGA_DEVICE(0x2a12, &intel_i965gm_info),
	INTEL_VGA_DEVICE(0x2a42, &intel_gm45_info),
	INTEL_VGA_DEVICE(0x2e02, &intel_g45_info),
	INTEL_VGA_DEVICE(0x2e12, &intel_g45_info),
	INTEL_VGA_DEVICE(0x2e22, &intel_g45_info),
	INTEL_VGA_DEVICE(0x2e32, &intel_g45_info),
	INTEL_VGA_DEVICE(0x2e42, &intel_g45_info),
	INTEL_VGA_DEVICE(0xa001, &intel_pineview_info),
	INTEL_VGA_DEVICE(0xa011, &intel_pineview_info),
	INTEL_VGA_DEVICE(0x0042, &intel_ironlake_d_info),
	INTEL_VGA_DEVICE(0x0046, &intel_ironlake_m_info),
	INTEL_VGA_DEVICE(0x0102, &intel_sandybridge_d_info),
	INTEL_VGA_DEVICE(0x0106, &intel_sandybridge_m_info),
	{0, 0, 0}
};


struct intel_device_info *probe_device_info(unsigned vendor, unsigned device)
{
	unsigned i = 0;
	for (; pciidlist[i].vendor; i++)
		if (pciidlist[i].vendor == vendor
		 && pciidlist[i].device == device)
			return (struct intel_device_info *)pciidlist[i].driver_data;

	return 0;
}

