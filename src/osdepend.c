/* fsusb2i   (c) 2015 trinity19683
  OS dependent (MS-Windows)
  osdepend.c
  2015-12-12
*/

#include "stdafx.h"
#include "osdepend.h"

void miliWait(unsigned msec)
{
	Sleep(msec);
}

void* uHeapAlloc(size_t sz)
{
	return VirtualAlloc( NULL, sz, MEM_COMMIT, PAGE_READWRITE );
}

void uHeapFree(void* const ptr)
{
	VirtualFree( ptr, 0, MEM_RELEASE );
}

int uthread_mutex_init(PMUTEX *p)
{
	if(NULL == p)
		return -1;
	if(NULL == *p) {
		*p = CreateMutex(NULL, FALSE, NULL);
		if(NULL == *p) return GetLastError();
	}
	return 0;
}

int uthread_mutex_lock(PMUTEX p)
{
	const DWORD dRet = WaitForSingleObject(p, 10000);
	if(WAIT_FAILED == dRet) return GetLastError();
	return dRet;
}

int uthread_mutex_unlock(PMUTEX p)
{
	if(ReleaseMutex(p) == 0) return GetLastError();
	return 0;
}

int uthread_mutex_destroy(PMUTEX p)
{
	if(CloseHandle(p) == 0) return GetLastError();
	return 0;
}

/*EOF*/