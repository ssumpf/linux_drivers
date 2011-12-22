/*
 * \brief  DRM function prototypes
 * \author Norman Feske
 * \date   2010-07-23
 *
 * In the original Linux DRM code, these prototypes are part of drmP.h
 */

#ifndef _DRM__DRM_PROTO_H_
#define _DRM__DRM_PROTO_H_

/* drm_gem.c */
void drm_gem_object_handle_unreference_unlocked(struct drm_gem_object *obj);
void drm_gem_object_reference(struct drm_gem_object *obj);
void drm_gem_object_unreference(struct drm_gem_object *obj);
void drm_gem_object_unreference_unlocked(struct drm_gem_object *obj);
struct drm_gem_object *drm_gem_object_lookup(struct drm_device *dev,
                                             struct drm_file *filp,
                                             u32 handle);
struct drm_gem_object *drm_gem_object_alloc(struct drm_device *dev,
                                            size_t size);
int drm_gem_handle_create(struct drm_file *file_priv,
                          struct drm_gem_object *obj,
                          u32 *handlep);

/* drm_irq.c */
int drm_irq_install(struct drm_device *dev);
int drm_irq_uninstall(struct drm_device *dev);
int drm_vblank_init(struct drm_device *dev, int num_crtcs);
void drm_handle_vblank(struct drm_device *dev, int crtc);

/* drm_memory.c */
int drm_unbind_agp(DRM_AGP_MEM * handle);
int drm_free_agp(DRM_AGP_MEM * handle, int pages);
void drm_core_ioremap_wc(struct drm_local_map *map, struct drm_device *dev);
void drm_core_ioremapfree(struct drm_local_map *map, struct drm_device *dev);
DRM_AGP_MEM *drm_alloc_agp(struct drm_device * dev, int pages, u32 type);
int drm_free_agp(DRM_AGP_MEM * handle, int pages);
int drm_bind_agp(DRM_AGP_MEM * handle, unsigned int start);
int drm_unbind_agp(DRM_AGP_MEM * handle);

/* drm_agpsupport.c */
struct drm_agp_head *drm_agp_init(struct drm_device *dev);
void drm_agp_chipset_flush(struct drm_device *dev);
DRM_AGP_MEM *drm_agp_bind_pages(struct drm_device *dev,
                                struct page **pages,
                                unsigned long num_pages,
                                uint32_t gtt_offset,
                                uint32_t type);
DRM_AGP_MEM *drm_agp_allocate_memory(struct agp_bridge_data *bridge, size_t pages, u32 type);
int drm_agp_free_memory(DRM_AGP_MEM * handle);
int drm_agp_bind_memory(DRM_AGP_MEM * handle, off_t start);
int drm_agp_unbind_memory(DRM_AGP_MEM * handle);

/* drm_cache.c */
void drm_clflush_pages(struct page *pages[], unsigned long num_pages);

/* drm_pci.c */
drm_dma_handle_t *drm_pci_alloc(struct drm_device *dev, size_t size,
                                size_t align);
void drm_pci_free(struct drm_device *dev, drm_dma_handle_t *dmah);

/* drm_context.h */
int drm_ctxbitmap_init(struct drm_device *dev);
struct drm_local_map *drm_getsarea(struct drm_device *dev);

/* drm_sysfs.c */
void drm_sysfs_hotplug_event(struct drm_device *dev);

/* drm_hashtab.c */
int drm_ht_insert_item(struct drm_open_hash *ht, struct drm_hash_item *item);
int drm_ht_remove_item(struct drm_open_hash *ht, struct drm_hash_item *item);

/* drm_mem_utils.h */
void *drm_calloc_large(size_t nmemb, size_t size);
void  drm_free_large(void *ptr);
void *drm_malloc_ab(size_t nmemb, size_t size);

#endif /* _DRM__DRM_PROTO_H_ */
