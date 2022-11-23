/* fsusb2i   (c) 2015-2016 trinity19683
  TS USB I/O thread (MS-Windows)
  tsthread.h
  2016-01-04
*/
#pragma once
#include "types_u.h"

typedef void* tsthread_ptr;

int tsthread_create(tsthread_ptr* const, const struct usb_endpoint_st * const);
void tsthread_destroy(const tsthread_ptr);
void tsthread_start(const tsthread_ptr);
void tsthread_stop(const tsthread_ptr);
int tsthread_read(const tsthread_ptr,  void** const ptr);
int tsthread_readable(const tsthread_ptr);
int tsthread_wait(const tsthread_ptr, const int);

/*EOF*/