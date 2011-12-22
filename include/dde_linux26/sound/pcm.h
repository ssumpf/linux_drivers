#ifndef __LINUX26__SOUND__PCM_H__
#define __LINUX26__SOUND__PCM_H__

#include_next <sound/pcm.h>

/*
 * Snippets from Linux 2.6.27
 */

#define snd_pcm_group_for_each_entry(s, substream) \
	list_for_each_entry(s, &substream->group->substreams, link_list)

#endif /* __LINUX26__SOUND__PCM_H__ */
