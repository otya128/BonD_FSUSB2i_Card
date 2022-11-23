/* fsusb2i   (c) 2015 trinity19683
  message (MS-Windows)
  message.c
  2015-12-30
*/

#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define BUFF_LEN 160

static void secure_strcpy(char *pDst, const char* const pOrg, int* const pIndex, const unsigned int buffLen)
{
	int idx;
	const int maxLen = buffLen - 1 - (*pIndex);
	pDst += *pIndex;
	for(idx = 0; idx < maxLen && pOrg[idx]; idx++) {
		pDst[idx] = pOrg[idx];
	}
	if(maxLen == idx || 0 == pOrg[idx]) {
		pDst[idx] = 0;
	}
	*pIndex += idx;
}

void u_debugMessage(const unsigned int flags, const char* FuncName, const unsigned int Line, const int retCode, const char* fmt, ...)
{
	va_list ap;
	char d_buff[BUFF_LEN];
	int idx = 0, ret;
	const char* const str_bufOverFlow = "debugMessage():BufferOverFlow\n";

	if(0 != FuncName) {
		d_buff[0] = '@';
		idx = 1;
		secure_strcpy(d_buff, FuncName, &idx, BUFF_LEN - 1);
		if(BUFF_LEN - 1 > idx) {
			d_buff[idx] = ' ';
			idx++;
		}else{
			OutputDebugStringA(str_bufOverFlow);
			return;
		}
	}

	if(0 != Line) {
		ret = _snprintf_s(&d_buff[idx], BUFF_LEN - 2 - idx, _TRUNCATE, "L%u ", Line);
		if(0 <= ret) {
			idx += ret;
		}else{
			OutputDebugStringA(str_bufOverFlow);
			return;
		}
	}

	va_start(ap, fmt);
	ret = vsnprintf_s(&d_buff[idx], BUFF_LEN - 2 - idx, _TRUNCATE, fmt, ap);
	va_end(ap);
	if(0 <= ret) {
		idx += ret;
	}else{
		OutputDebugStringA(str_bufOverFlow);
		return;
	}

	if(0 != retCode) {
		ret = _snprintf_s(&d_buff[idx], BUFF_LEN - 2 - idx, _TRUNCATE, ", ERR=%d", retCode);
		if(0 <= ret) {
			idx += ret;
		}else{
			OutputDebugStringA(str_bufOverFlow);
			return;
		}
	}
	if(flags & 0x1U) {
		secure_strcpy(d_buff, "\n", &idx, BUFF_LEN);
	}
	OutputDebugStringA(d_buff);
}

void dumpHex(char* const buf, const unsigned buflen, const int addr, const void* const dptr, unsigned dsize)
{
	int idx = 0, ret;
	const unsigned char* ptr = dptr;
	if(0 <= addr) {
		ret = _snprintf_s(buf, buflen - idx, _TRUNCATE, "%04X: ", addr);
		idx += ret;
	}
	if(dsize * 3 > buflen - idx) {
		warn_info(0,"buffer overflow");
		return;
	}
	while(dsize) {
		ret = _snprintf_s(&buf[idx], buflen - idx, _TRUNCATE, "%02X", *ptr);
		idx += ret;
		ptr++;
		dsize--;
		if(dsize) {
			buf[idx] = ' ';
			idx++;
		}
	}
	buf[idx] = 0;
}


/*EOF*/