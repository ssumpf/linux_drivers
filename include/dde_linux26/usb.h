/*
 * \brief  DDE Linux 2.6 USB API
 * \author Christian Helmuth
 * \date   2009-04-24
 */

#ifndef _DDE_LINUX26__USB_H_
#define _DDE_LINUX26__USB_H_

#include <dde_kit/types.h>


/*****************************
 ** Virtual host-controller **
 *****************************/

/**
 * URB completion
 */
extern void dde_linux26_usb_vhcd_urb_complete(void *urb_handle,
                                              dde_kit_size_t data_size,
                                              void *data);

/*
 * Call backs should be short - defer complex tasks.
 */

/**
 * Call back to discover root hub configuration
 */
extern dde_kit_uint8_t dde_linux26_usb_vhcd_ports_cb(void);

/**
 * Call back to request status of root hub port
 */
extern dde_kit_uint32_t dde_linux26_usb_vhcd_port_status_cb(unsigned port_number);

/**
 * Call back to poll for pending port changes
 *
 * \return bit mask of ports, set bits means port status changed
 */
extern dde_kit_uint32_t dde_linux26_usb_vhcd_ports_changed_cb(void);

/**
 * Call back to clear feature flags
 */
extern void dde_linux26_usb_vhcd_clear_feature_cb(unsigned port_number,
                                                  unsigned feature);

/**
 * Call back to submit control transfer packets
 */
extern void dde_linux26_usb_vhcd_submit_control_urb_cb(int port_number,
                                                       int endpoint,
                                                       int direction_input,
                                                       void *urb_handle,
                                                       dde_kit_size_t data_size,
                                                       void *data);


/***************************
 ** Virtual device driver **
 ***************************/

extern void dde_linux26_usb_vdev_submit_control_urb(void *dev_handle,
                                                    int endpoint,
                                                    int direction_input,
                                                    void *urb_handle,
                                                    dde_kit_size_t data_size,
                                                    void *data);

/*
 * Call backs should be short - defer complex tasks.
 */

/**
 * Call back on connection of USB device
 */
extern void dde_linux26_usb_vdev_connect_cb(void *dev_handle);

/**
 * Call back on disconnection of USB device
 */
extern void dde_linux26_usb_vdev_disconnect_cb(void *dev_handle);

/**
 * Call back on URB completion
 */
extern void dde_linux26_usb_vdev_urb_complete_cb(void *dev_handle,
                                                 void *urb_handle);


#endif /* _DDE_LINUX26__USB_H_ */
