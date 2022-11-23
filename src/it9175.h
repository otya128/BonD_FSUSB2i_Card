/* fsusb2i   (c) 2015 trinity19683
  IT9175 USB interface, tuner, demod
  it9175.h
  2015-12-09
*/
#pragma once
#include <stdint.h>
#include "types_u.h"

typedef void* it9175_state;

int it9175_create(it9175_state* const, struct usb_endpoint_st * const);
int it9175_destroy(const it9175_state);
int it9175_setFreq(const it9175_state, const unsigned int freq);
int it9175_waitTuning(const it9175_state, const int timeout);
int it9175_waitStream(const it9175_state, const int timeout);

struct TMCC_data {
	uint8_t mode[3];
	uint8_t layer[3][4];
};
int it9175_readTMCC(const it9175_state, struct TMCC_data* const);

int it9175_readStatistic(const it9175_state, uint8_t* const data);

/*EOF*/