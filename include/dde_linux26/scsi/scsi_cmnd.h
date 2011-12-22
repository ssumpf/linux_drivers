#ifndef _SCSI_SCSI_CMND_H
#define _SCSI_SCSI_CMND_H

#include <linux/dma-mapping.h>

struct scsi_device;
struct request;

struct scsi_cmnd {
	struct scsi_device *device;

	unsigned char cmd_len;
	enum dma_data_direction sc_data_direction;

	/* These elements define the operation we are about to perform */
#define MAX_COMMAND_SIZE	16
	unsigned char cmnd[MAX_COMMAND_SIZE];
	unsigned request_bufflen;	/* Actual request size */

	void *request_buffer;		/* Actual requested buffer */

	/* These elements define the operation we ultimately want to perform */
	unsigned short use_sg;	/* Number of pieces of scatter-gather */

	unsigned underflow;	/* Return error if less than
				   this amount is transferred */

	int resid;		/* Number of bytes requested to be
				   transferred less actual number
				   transferred (0 if not supported) */

	struct request *request;	/* The command we are
				   	   working on */ //DEBUGGING;

#define SCSI_SENSE_BUFFERSIZE 	96
	unsigned char sense_buffer[SCSI_SENSE_BUFFERSIZE];
				/* obtained by REQUEST SENSE when
				 * CHECK CONDITION is received on original
				 * command (auto-sense) */

	/* Low-level done function - can be used by low-level driver to point
	 *        to completion function.  Not used by mid/upper level code. */
	void (*scsi_done) (struct scsi_cmnd *);
	int result;		/* Status code from lower level driver */
	void *back;
};

#endif /* _SCSI_SCSI_CMND_H */
