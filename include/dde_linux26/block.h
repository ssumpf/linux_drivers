/*
 * \brief  Block interface
 * \author Sebastian Sumpf <sebastian.sumpf@genode-labs.com>
 * \brief  2009-11-19
 */

#ifndef _DDE_LINUX26__BLOCK_H_
#define _DDE_LINUX26__BLOCK_H_
struct scsi_device;

enum Block_errors {
	EBLK_NOMEM = 1, //Not enough space
	EBLK_NODEV = 2, //No such device 
	EBLK_FAULT = 3, //Bad address
};


/** 
 * Device registered callback
 *
 * \param usb_index device identifier
 */
typedef void (*dde_linux26_block_plugin_cb)(int usb_index);

/**
 * Register device callback
 * 
 * \param cb new callback function
 *
 * \return old callback function
 */
extern dde_linux26_block_plugin_cb dde_linux26_block_register_plugin_callback(dde_linux26_block_plugin_cb cb);

/**
 * Check if a device is present
 *
 * \param usb_index device identifier
 *
 * \return 1 if device is present; 0 otherwise
 */
extern int dde_linux26_block_present(int usb_index);

/**
 * Add a device
 *
 * \param sdev Linux device structure
 * 
 * \return 0 on success; negative value otherwise
 */
extern int dde_linux26_block_dev_add(struct scsi_device *sdev);

/**
 *  Retrieve number of blocks for given device
 *
 *  \param usb_index device identifier
 *
 *  \return number of blocks
 */
extern int dde_linux26_block_count(int usb_index);

/**
 * Retrieve block size in bytes
 *
 * \param usb_index device identifier
 *
 * \return block size
 */
extern int dde_linux26_block_size(int usb_index);

/** Read one block from device
 *
 * \param usb_index device identifier
 * \param block_nr  block number to read
 * \param buffer    request buffer for data
 *
 * \return 0 on success; negative value otherwise
 */
extern int dde_linux26_block_read(int usb_index, unsigned long block_nr, void *buffer);

/**
 * Write one block to device
 *
 * \param usb_index device identifier
 * \param block_nr  block number to write
 * \param buffer    request buffer containing the data
 *
 * \return 0 on success; negative value otherwise
 */
extern int dde_linux26_block_write(int usb_index, unsigned long block_nr, void *buffer);

/**
 * Allocate memory for block I/O
 * 
 * \param size number of bytes to allocate
 *
 * \return pointer to memory or NULL
 */
extern void * dde_linux26_block_malloc(unsigned long size);

/**
 * Free memory
 *
 * \param pointer of memory to free
 */
extern void dde_linux26_block_free(void *ptr);

#endif /*  _DDE_LINUX26__BLOCK_H_ */
