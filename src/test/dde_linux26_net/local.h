/*
 * \brief   Local interfaces
 * \author  Christian Helmuth
 * \date    2009-02-09
 */

#ifndef _LOCAL_H_
#define _LOCAL_H_


/****************
 ** Driver API **
 ****************/

/**
 * Send one network packet
 *
 * \param   packet      reference to network packet buffer
 * \param   packet_len  size of packet
 *
 * The packet will be copied out of the buffer before function returns.
 */
void dev_send(const unsigned char *packet, unsigned packet_len);


/**
 * Receive one network packet
 *
 * \param   buffer      reference to receive buffer
 * \param   buffer_len  size of buffer
 *
 * \return  actual packet length, or 0 if no packet is available
 *
 * This function copies one available network packet into the given buffer.
 */
unsigned dev_recv(unsigned char *buffer, unsigned buffer_len);


/*****************
 ** Network API **
 *****************/

void uip_loop(const unsigned char *mac_addr);

#endif /* _LOCAL_H_ */
