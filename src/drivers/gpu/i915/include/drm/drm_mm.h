/*
 * \brief  Interface to DRM memory manager
 * \author Norman Feske
 * \date   2010-07-27
 */

#ifndef _DRM__DRM_MM_H_
#define _DRM__DRM_MM_H_

struct drm_gem_object;

struct drm_mm_node {
	unsigned long start;
#ifdef __cplusplus
	struct drm_gem_object *priv;
#else
	struct drm_gem_object *private;  /* pointer to gem object, assigned by 'i915_gem_object_bind_to_gtt' */
#endif /* __cplusplus */
};


struct drm_mm_priv;
struct drm_mm {
	struct drm_mm_priv *priv;  /* custom for Genode port */
};


int drm_mm_init(struct drm_mm *mm, unsigned long start, unsigned long size);
struct drm_mm_node *drm_mm_search_free(const struct drm_mm *mm,
                                       unsigned long size,
                                       unsigned alignment,
                                       int best_match);
struct drm_mm_node *drm_mm_get_block(struct drm_mm_node *parent,
                                     unsigned long size,
                                     unsigned alignment);
void drm_mm_put_block(struct drm_mm_node *cur);
void drm_mm_takedown(struct drm_mm *mm);

#endif /* _DRM__DRM_MM_H_ */
