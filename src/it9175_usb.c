/* fsusb2i   (c) 2015-2016 trinity19683
  IT9175 USB commands (MS-Windows)
  it9175_usb.c
  2016-01-05
*/

#include "stdafx.h"
#include <errno.h>
#include <WinUSB.h>

#include "osdepend.h"
#include "it9175_usb.h"
#include "it9175_priv.h"


static unsigned int  it9175_checksum(uint8_t* const buf, const int mode)
{
	int len, i;
	unsigned checksum = 0;
	len = buf[0] - 1;
	for(i = 1; i < len; i++) {
		checksum += (i & 1) ? buf[i] << 8 : buf[i];
	}
	checksum = ~checksum & 0xFFFF;
	if(0 != mode) {
		//# verify checksum
		return (buf[len] << 8 | buf[len +1]) ^ checksum;
	}
	//# add checksum
	buf[len] = checksum >> 8;
	buf[len +1] = checksum & 0xFF;
	return 0;
}

int it9175_ctrl_msg(void* const pst, const uint8_t cmd, const uint8_t mbox, const uint8_t wrlen, const uint8_t rdlen)
{
	struct state_st* const st = pst;
	int ret;
	DWORD wlen, rlen, alen;

	wlen = REQ_HDR_LEN + wrlen + CHECKSUM_LEN;
	if(CMD_FW_DL == cmd) {
		//# no ACK for these packets
		rlen = 0;
	}else{
		rlen = ACK_HDR_LEN + rdlen + CHECKSUM_LEN;
		if(MAX_XFER_SIZE < rlen) {
			warn_info(0,"buffer overflow");
			return -EINVAL;
		}
	}
	st->buf[0] = (wlen - 1) & 0xFF;
	st->buf[1] = mbox;
	st->buf[2] = cmd;

	//# mutex lock
	if((ret = uthread_mutex_lock(st->pmutex))) {
		warn_info(ret,"mutex_lock failed");
		return -EINVAL;
	}
	st->buf[3] = st->seq++;
	it9175_checksum(st->buf, 0);    //# calc and add checksum
	//# send cmd
	if(WinUsb_WritePipe(st->fd, EP_CTRLBULK, st->buf, wlen, &alen, NULL) == FALSE) {
		ret = GetLastError();
		warn_msg(ret,"CTRLcmd=%02X",cmd);
		goto exit_err0;
	}
	if(alen != wlen) {
		ret = -EIO;
		warn_msg(alen,"CTRLcmd=%02X wlen=%d",cmd,wlen);
		goto exit_err0;
	}

	if(0 < rlen) {
		//# read response
		if(WinUsb_ReadPipe(st->fd, EP_CTRLBULKRES, st->buf, rlen, &alen, NULL) == FALSE) {
			ret = GetLastError();
			warn_msg(ret,"CTRLcmd=%02X_R",cmd);
			goto exit_err0;
		}
		if(alen != rlen) {
			ret = -EIO;
			warn_msg(alen,"CTRLcmd=%02X rlen=%d",cmd,rlen);
			goto exit_err0;
		}
	}

	//# mutex unlock
	if((ret = uthread_mutex_unlock(st->pmutex))) {
		warn_info(ret,"mutex_unlock failed");
		return -EINVAL;
	}

	if(it9175_checksum(st->buf, 1) != 0) {
		//# verify checksum, mismatch
		warn_msg(0,"CTRLcmd=%02X checksum mismatch",cmd);
		return -EIO;
	}

	if(st->buf[2]) {
		//# check status
		warn_msg(st->buf[2],"CTRLcmd=%02X",cmd);
		return -EIO;
	}

	return 0;
exit_err0:
	//# mutex unlock
	if((alen = uthread_mutex_unlock(st->pmutex))) {
		warn_info(alen,"mutex_unlock failed");
		return -EINVAL;
	}
	return ret;
}

int it9175_usbSetTimeout(void* const pst)
{
	struct state_st* const s = pst;
	ULONG dwValue;
	dwValue = USB_TIMEOUT;
	WinUsb_SetPipePolicy(s->fd, EP_CTRLBULK, PIPE_TRANSFER_TIMEOUT, sizeof(dwValue), &dwValue);
	WinUsb_SetPipePolicy(s->fd, EP_CTRLBULKRES, PIPE_TRANSFER_TIMEOUT, sizeof(dwValue), &dwValue);
	return 0;
}

/*EOF*/