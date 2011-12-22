#ifndef __LINUX26__SOUND__CONTROL_H__
#define __LINUX26__SOUND__CONTROL_H__

#include_next <sound/control.h>

/*
 * Snippets from Linux 2.6.27
 */

/*
 * Frequently used control callbacks
 */
int snd_ctl_boolean_mono_info(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_info *uinfo);
int snd_ctl_boolean_stereo_info(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_info *uinfo);

/*
 * virtual master control
 */
struct snd_kcontrol *snd_ctl_make_virtual_master(char *name,
						 const unsigned int *tlv);
int snd_ctl_add_slave(struct snd_kcontrol *master, struct snd_kcontrol *slave);

#endif /* __LINUX26__SOUND__CONTROL_H__ */
