/* fsusb2i   (c) 2015-2016 trinity19683
  finding USB device file (MS-Windows)
  usbdevfile.cpp
  2016-01-22
*/

#include "stdafx.h"
#include <setupapi.h>
#include <strsafe.h>
#include <WinUSB.h>

#include "usbdevfile.h"

// Driver Instance GUID
DEFINE_GUID( GUID_WINUSB_DRV,	0x77ed26ec, 0x2783, 0x7bba, 0xa8, 0x24, 0x00, 0xbc, 0xad, 0x7a, 0xcd, 0xb9 );

HANDLE usbdevfile_alloc(unsigned int * const idx)
{
	DWORD dwRet;
	ULONG length;
	HANDLE hDev = INVALID_HANDLE_VALUE;
	GUID * const pDrvID = (GUID *)&GUID_WINUSB_DRV;

	// get handle to device info.
	HDEVINFO deviceInfo = SetupDiGetClassDevs(pDrvID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if(INVALID_HANDLE_VALUE == deviceInfo) return NULL;

	SP_DEVICE_INTERFACE_DATA interfaceData;
	interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	for(; *idx < 40; (*idx)++ ) {
		//# enumerate device interfaces
		if( FALSE == SetupDiEnumDeviceInterfaces(deviceInfo, NULL, pDrvID, *idx, &interfaceData) ) {
			dwRet = GetLastError();
			//if(dwRet == ERROR_NO_MORE_ITEMS) break;
			break;
		}

		ULONG requiredLength = 0;
		SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaceData, NULL, 0, &requiredLength, NULL);
		//# allocate buffer
		requiredLength += sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + sizeof(TCHAR);
		PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)GlobalAlloc(GMEM_FIXED, requiredLength);
		detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		//#  get path name of a device
		length = requiredLength;
		if(SetupDiGetDeviceInterfaceDetail(deviceInfo, &interfaceData, detailData, length, &requiredLength, NULL) ) {
			//# success
			hDev = CreateFile(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if( hDev == INVALID_HANDLE_VALUE ) {
				//dwRet = GetLastError();
				//if (dwRet == ERROR_ACCESS_DENIED)  //# using
			}else{
				GlobalFree(detailData);
				break;
			}
		}
		GlobalFree(detailData);
	}
	SetupDiDestroyDeviceInfoList(deviceInfo);
	
	if(INVALID_HANDLE_VALUE == hDev) return NULL;
	return hDev;
}

HANDLE usbdevfile_init(HANDLE hDev)
{
	HANDLE usbHandle;
	if(FALSE == WinUsb_Initialize( hDev, &usbHandle ))
		return NULL;
	return usbHandle;
}

void usbdevfile_free(HANDLE usbHandle)
{
	WinUsb_Free( usbHandle );
}

/*EOF*/