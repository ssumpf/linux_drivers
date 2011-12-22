/*
 * \brief   DDE Linux 2.6 audio API
 * \author  Christian Helmuth
 * \date    2009-12-19
 *
 * The DDE Linux 2.6 audio API utilizes the SALSA library, which provides the
 * programming interface for the initialized devices.
 */

#ifndef _DDE_LINUX26__AUDIO_H_
#define _DDE_LINUX26__AUDIO_H_

/**
 * Initialize audio subsystem
 *
 * \return number of pcm devices
 */
extern int dde_linux26_audio_init(void);

extern int dde_linux26_audio_init_devices(void);

extern void dde_linux26_audio_adopt_myself(void);
extern int dde_linux26_audio_play(void *data, int frame_cnt);
extern void dde_linux26_audio_stop(void);
extern void dde_linux26_audio_start(void);

/**
 * Return hardware-audio buffer for given file handle
 */
extern char *dde_linux26_get_hwbuf_by_fd(int fd);

#endif /* _DDE_LINUX26__AUDIO_H_ */
