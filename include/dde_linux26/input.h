/*
 * \brief  DDE Linux 2.6 INPUT API
 * \author Christian Helmuth
 * \date   2009-04-20
 */

#ifndef _DDE_LINUX26__INPUT_H_
#define _DDE_LINUX26__INPUT_H_

/**
 * Input event type
 */
enum dde_linux26_input_event {
	EVENT_TYPE_PRESS, EVENT_TYPE_RELEASE, /* key press and release */
	EVENT_TYPE_MOTION,                    /* any type of (pointer) motion */
	EVENT_TYPE_WHEEL                      /* mouse scroll wheel */
};


/**
 * Input event callback
 *
 * \param   type        input event type
 * \param   keycode     key code if type is EVENT_TYPE_PRESS or
 *                      EVENT_TYPE_RELEASE
 * \param   absolute_x  absolute horizontal coordinate if type is
 *                      EVENT_TYPE_MOTION
 * \param   absolute_y  absolute vertical coordinate if type is
 *                      EVENT_TYPE_MOTION
 * \param   relative_x  relative horizontal coordinate if type is
 *                      EVENT_TYPE_MOTION or EVENT_TYPE_WHEEL
 * \param   relative_y  relative vertical coordinate if type is
 *                      EVENT_TYPE_MOTION or EVENT_TYPE_WHEEL
 *
 * Key codes conform to definitions in os/include/input/event.h, which is C++
 * and therefore not included here.
 *
 * Relative coordinates are only significant if absolute_x and absolute_y are
 * 0.
 */
typedef void (*dde_linux26_input_event_cb)(enum dde_linux26_input_event type,
                                           unsigned keycode,
                                           int absolute_x, int absolute_y,
                                           int relative_x, int relative_y);

/**
 * Initialize USB HID sub-system
 *
 * \param   handler  call-back function on input events
 *
 * \return  0 on success; !0 otherwise
 */
extern int dde_linux26_input_init(dde_linux26_input_event_cb handler);

/**
 * Deinitialize USB HID sub-system
 */
extern void dde_linux26_input_exit(void);

#endif /* _DDE_LINUX26__INPUT_H_ */
