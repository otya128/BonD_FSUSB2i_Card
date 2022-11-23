/* fsusb2i   (c) 2015 trinity19683
  message (MS-Windows)
  message.h
  2015-12-30
*/
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
void u_debugMessage(const unsigned int flags, const char* FuncName, const unsigned int Line, const int retCode, const char* fmt, ...);
void dumpHex(char* const buf, const unsigned buflen, const int addr, const void* const dptr, unsigned dsize);
#ifdef __cplusplus
}
#endif


#define warn_msg(errCode, ...) u_debugMessage(1, 0, 0, errCode, __VA_ARGS__)
#define warn_info(errCode, ...) u_debugMessage(1, __FUNCTION__, __LINE__, errCode, __VA_ARGS__)

#ifdef _DEBUG
#define dmsg(...) u_debugMessage(1, 0, 0, 0, __VA_ARGS__)
#define dmsgn(...) u_debugMessage(0, 0, 0, 0, __VA_ARGS__)
#else
#define dmsg(...)
#define dmsgn(...)
#endif

/*EOF*/