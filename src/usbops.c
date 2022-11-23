/* fsusb2i   (c) 2015 trinity19683
  USB operations (MS-Windows)
  usbops.c
  2015-12-28
*/

#include "stdafx.h"
#include <WinUSB.h>
#include "usbops.h"

int usb_reset(HANDLE fd)
{ return 0; }

int usb_claim(HANDLE fd, unsigned int interface)
{ return 0; }

int usb_release(HANDLE fd, unsigned int interface)
{ return 0; }

int usb_setconfiguration(HANDLE fd, unsigned int confignum)
{ return 0; }

int usb_setinterface(HANDLE fd, const unsigned int interface, const unsigned int altsetting)
{
	if(WinUsb_SetCurrentAlternateSetting(fd, altsetting) == 0) return GetLastError();
	return 0;
}

int usb_clearhalt(HANDLE fd, unsigned int endpoint)
{
	if(WinUsb_ResetPipe(fd, endpoint) == 0) {
		const DWORD dRet = GetLastError();
		warn_info(dRet,"EP=%02X failed", endpoint);
		return dRet;
	}
	return 0;
}

/*EOF*/