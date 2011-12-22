/*
 * \brief  Dummy stubs for 'drm_crtc.h' as relied on by i915 GEM code
 * \author Norman Feske
 * \date   2010-08-03
 */

#ifndef _DRM__DRM_CRTC_H_
#define _DRM__DRM_CRTC_H_

#include <drmP.h>

struct drm_mode_config {
	resource_size_t fb_base;         /* needed for i915_dma.c */
	int             num_connector;   /* needed for i915_irq */
	struct list_head connector_list; /* needed for i915_irq */
};

struct drm_connector {
	struct list_head head; /* needed for i915_irq */
	void *helper_private;
};

#endif /* _DRM__DRM_CRTC_H_ */
