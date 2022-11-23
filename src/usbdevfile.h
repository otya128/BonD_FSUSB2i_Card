/* fsusb2i   (c) 2015 trinity19683
  finding USB device file (MS-Windows)
  usbdevfile.h
  2015-12-10
*/
#pragma once

HANDLE usbdevfile_alloc(unsigned int * const idx);
HANDLE usbdevfile_init(HANDLE hDev);
void usbdevfile_free(HANDLE usbHandle);

/*EOF*/