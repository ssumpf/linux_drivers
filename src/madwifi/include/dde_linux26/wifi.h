/**
 * \brief  Linux DDE Kit wifi interface
 * \author Stefan Kalkowski
 * \date   2010-08-09
 */

#ifndef _DDE__WIFI_H_
#define _DDE__WIFI_H_

int  dde_linux26_wifi_atheros_idx(void);
void dde_linux26_wifi_set_essid(int idx, const char *essid);

#endif /* _DDE__WIFI_H_ */
