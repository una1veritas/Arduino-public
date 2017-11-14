/*
 * OC1AClock.h
 *
 *  Created on: 2017/11/13
 *      Author: sin
 */

#ifndef OC1ACLOCK_H_
#define OC1ACLOCK_H_

#include <inttypes.h>

struct OCA1Clock {
	uint8_t prescaler;
    uint16_t topvalue;
};

void OC1AClock_setup(uint8_t presc, uint16_t top);
void OC1AClock_reset(uint8_t presc, uint16_t top);
void OC1AClock_start(void);
void OC1AClock_restart(void);
void OC1AClock_stop();

#endif /* OC1ACLOCK_H_ */
