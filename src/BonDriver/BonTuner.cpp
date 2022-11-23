/* fsusb2i   (c) 2015-2016 trinity19683
  BonTuner.DLL (MS-Windows)
  BonTuner.cpp
  2016-02-12
*/

#include "stdafx.h"
#include <tchar.h>
#include "BonTuner.h"
#include "../usbdevfile.h"

#pragma warning( disable : 4273 )
extern "C" __declspec(dllexport) IBonDriver * CreateBonDriver()
{ return (CBonTuner::m_pThis)? CBonTuner::m_pThis : ((IBonDriver *) new CBonTuner); }
#pragma warning( default : 4273 )

//# initialize static member variables
CBonTuner * CBonTuner::m_pThis = NULL;
HINSTANCE CBonTuner::m_hModule = NULL;

const TCHAR* const g_RegKey = TEXT("Software\\trinity19683\\FSUSB2i");

CBonTuner::CBonTuner()
: m_dwCurSpace(0), m_dwCurChannel(0), m_hDev(NULL), m_hUsbDev(NULL), pDev(NULL), tsthr(NULL),
 m_ChannelList(NULL)
{ m_pThis = this; }

CBonTuner::~CBonTuner()
{
	CloseTuner();

	if(m_ChannelList != NULL) ::GlobalFree(m_ChannelList);
	m_pThis = NULL;
}

const BOOL CBonTuner::OpenTuner()
{
	//# if already open, close tuner
	CloseTuner();
	if(IsTunerOpening()) return FALSE;

	try{
		//# AllocTuner
		for(unsigned int idx = 0; idx < 40;) {
			HANDLE hDev;
			if((hDev = usbdevfile_alloc(&idx) ) == NULL) {   //# not found
				throw (const DWORD)__LINE__;
			}
			//# found
			m_hDev = hDev;
			if((hDev = usbdevfile_init(m_hDev) ) == NULL) {   //# failed
				throw (const DWORD)__LINE__;
			}
			m_hUsbDev = hDev;
			break;
		}
		//# device initialize
		m_USBEP.fd = m_hUsbDev;
		if(it9175_create(&pDev, &m_USBEP) != 0) {
			throw (const DWORD)__LINE__;
		}
		if(tsthread_create(&tsthr, &m_USBEP) != 0) {
			throw (const DWORD)__LINE__;
		}

		//# device has been ready.
		LoadData();
	}
	catch (const DWORD dwErrorStep) {
		//# Error
		warn_msg(0,"BonDriver_FSUSB2i:OpenTuner dwErrorStep = %u", dwErrorStep);

		CloseTuner();
		return FALSE;
	}
	return TRUE;
}

void CBonTuner::CloseTuner()
{
	if(tsthr) {
		tsthread_stop(tsthr);
		tsthread_destroy(tsthr);
		tsthr = NULL;
	}
	if(pDev) {
		it9175_destroy(pDev);
		pDev = NULL;
	}
	if(m_hUsbDev) {
		usbdevfile_free(m_hUsbDev);
		m_hUsbDev = NULL;
	}
	if(m_hDev) {
		::CloseHandle( m_hDev );
		m_hDev = NULL;
	}
}

const BOOL CBonTuner::SetChannel(const BYTE bCh)
{
	//# compatible with IBonDriver
	if(bCh < 13 || bCh > 52) return FALSE;
	else return SetChannel(0, bCh - 13);
}

const float CBonTuner::GetSignalLevel(void)
{
	if(NULL == pDev) return 0.0f;
	uint8_t statData[44];
	if(it9175_readStatistic(pDev, statData) != 0) return 0.1f;
	return statData[3] * 1.0f;
}

const DWORD CBonTuner::WaitTsStream(const DWORD dwTimeOut)
{
	const int remainTime = (dwTimeOut < 0x10000000) ? dwTimeOut : 0x10000000;
	if(NULL == tsthr) return WAIT_FAILED;

	const int r = tsthread_wait(tsthr, remainTime);
	if(0 > r)  return WAIT_FAILED;
	else if(0 < r)  return WAIT_OBJECT_0;
	else  return WAIT_TIMEOUT;
}

const DWORD CBonTuner::GetReadyCount()
{//# number of call GetTsStream()
	if(NULL == tsthr) return 0;
	const int ret = tsthread_readable(tsthr);
	return (ret > 0) ? 1 : 0;
}

const BOOL CBonTuner::GetTsStream(BYTE *pDst, DWORD *pdwSize, DWORD *pdwRemain)
{
	BYTE *pSrc = NULL;
	if(GetTsStream(&pSrc, pdwSize, pdwRemain)){
		if(*pdwSize) ::CopyMemory(pDst, pSrc, *pdwSize);
		return TRUE;
	}
	return FALSE;
}

const BOOL CBonTuner::GetTsStream(BYTE **ppDst, DWORD *pdwSize, DWORD *pdwRemain)
{
	if(! tsthr) return FALSE;
	const int ret = tsthread_readable(tsthr);
	if(ret <= 0) {
		//# no readable data
		*pdwSize = 0;
		*pdwRemain = 0;
		return TRUE;
	}
	*pdwSize = tsthread_read(tsthr, (void**)ppDst);
	*pdwRemain = GetReadyCount();
	//dmsg("GetTsStream(%p,%u,%u)", ppDst, *pdwSize, *pdwRemain);
	return TRUE;
}

void CBonTuner::PurgeTsStream()
{
	if(! tsthr) return;
	//# purge available data in TS buffer
	tsthread_read(tsthr, NULL);
}

void CBonTuner::Release()  //# release the instance
{ delete this; }

LPCTSTR CBonTuner::GetTunerName(void)
{ return TEXT("FSUSB2i"); }

const BOOL CBonTuner::IsTunerOpening(void)
{ return m_hUsbDev ? TRUE : FALSE; }

LPCTSTR CBonTuner::EnumTuningSpace(const DWORD dwSpace)
{ return (dwSpace == 0) ? TEXT("’nƒfƒW") : NULL; }

LPCTSTR CBonTuner::EnumChannelName(const DWORD dwSpace, const DWORD dwChannel)
{
	if(dwSpace == 0) {
		if(m_ChannelList != NULL) {
			//# User-defined channels
			const DWORD dwChannelLen    = m_ChannelList[0] >> 16;
			const DWORD dwNumOfChannels = m_ChannelList[0] & 0xFFFF;
			TCHAR* const ptrStr = (TCHAR*)(m_ChannelList + dwNumOfChannels + 1);
			if(dwChannel < dwNumOfChannels)	return ptrStr + (dwChannelLen * dwChannel);
		}else if(dwChannel < 40) {
			static TCHAR buf[6];
			_sntprintf_s(buf, sizeof(buf)/sizeof(TCHAR), _TRUNCATE, TEXT("%u"), dwChannel + 13);
			return buf;    //# The caller must copy data from this buffer.
		}
	}
	return NULL;
}

const BOOL CBonTuner::SetChannel(const DWORD dwSpace, const DWORD dwChannel)
{
	if(NULL == pDev) return FALSE;
	if(NULL == tsthr) return FALSE;

	DWORD dwFreq = 0;

	if(dwSpace == 0) {
		if(m_ChannelList != NULL) {  //# User-defined channels
			const DWORD dwNumOfChannels = m_ChannelList[0] & 0xFFFF;
			if(dwChannel < dwNumOfChannels)
				dwFreq = m_ChannelList[dwChannel + 1];
		}else{  //# UHF standard channels
			if(dwChannel < 40)
				dwFreq = dwChannel * 6000 + 473143;
		}
	}else if(dwSpace == 114514) {  //# dwChannel as freq/kHz
		dwFreq = dwChannel;
	}
	if(dwFreq < 61000 || dwFreq > 874000 ) return FALSE;

	//# change channel
	tsthread_stop(tsthr);

	int ret;
	if(it9175_setFreq(pDev, dwFreq) != 0) return FALSE;
	//# set variables
	m_dwCurSpace = dwSpace;
	m_dwCurChannel = dwChannel;
	::Sleep( 80 );

	tsthread_start(tsthr);

	if((ret = it9175_waitTuning(pDev, 1500)) < 0) return FALSE;
	//# ignore check empty channel
	//# ignore check TS sync lock

	PurgeTsStream();

	return TRUE;
}

const DWORD CBonTuner::GetCurSpace(void)
{ return m_dwCurSpace; }

const DWORD CBonTuner::GetCurChannel(void)
{ return m_dwCurChannel; }


bool CBonTuner::LoadData ()
{
	HKEY hKey;

	if(ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, g_RegKey, 0, KEY_READ, &hKey)) {
		//ReadRegMode(hKey);
		ReadRegChannels(hKey);
		RegCloseKey(hKey);
	}
	if(ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER, g_RegKey, 0, KEY_READ, &hKey)) {
		//ReadRegMode(hKey);
		ReadRegChannels(hKey);
		RegCloseKey(hKey);
	}

	return true;
}

void CBonTuner::ReadRegMode (HKEY hPKey)
{ return; }

void CBonTuner::ReadRegChannels (HKEY hPKey)
{
	if(m_ChannelList != NULL) return;

	HKEY hKey;
	DWORD NumOfValues;
	TCHAR szValueName[32];
	DWORD dwValue, dwLen, dwType, dwByte, dwMaxValueName;
	if(ERROR_SUCCESS != RegOpenKeyEx( hPKey, TEXT("Channels"), 0, KEY_READ, &hKey)) {
		return;
	}
	if(ERROR_SUCCESS != RegQueryInfoKey( hKey, NULL, NULL, NULL, NULL, NULL, NULL, &NumOfValues, &dwMaxValueName, NULL, NULL, NULL)) {
		RegCloseKey(hKey);
		return;
	}
	dwMaxValueName++;
	m_ChannelList = (DWORD*) ::GlobalAlloc(GMEM_FIXED, NumOfValues * (dwMaxValueName * sizeof(TCHAR) + sizeof(DWORD)) + sizeof(DWORD) );
	m_ChannelList[0] = dwMaxValueName << 16 | NumOfValues;
	ZeroMemory( m_ChannelList + 1, sizeof(DWORD) * NumOfValues );
	TCHAR *ptrStr;
	for(DWORD dwIdx = 0; dwIdx < NumOfValues; dwIdx++ ) {
		dwLen = 32;
		dwByte = sizeof(dwValue);
		if(ERROR_SUCCESS != RegEnumValue( hKey, dwIdx, szValueName, &dwLen, NULL, &dwType, (BYTE*)&dwValue, &dwByte)
			|| dwByte != sizeof(DWORD)) {
			break;
		}
		dwByte = dwValue >> 24; //# Index
		if( dwByte >= NumOfValues ) continue;
		m_ChannelList[dwByte + 1] = dwValue & 0x00ffffff;
		ptrStr = (TCHAR*)(m_ChannelList + NumOfValues + 1);
		ptrStr += dwMaxValueName * dwByte;
		lstrcpyn( ptrStr, szValueName, dwMaxValueName );
	}
	RegCloseKey(hKey);
}

/*EOF*/