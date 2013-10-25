/*
 * set position via sync write / a simpler version with using uint16_t array
 * 
 */

#include <stdio.h>

#include "dynamixel.h"

#include "config.h"

#define SERVO_COUNT	2
uint16_t ids[SERVO_COUNT]={1,2};
uint16_t speed[SERVO_COUNT]={100,100};
uint16_t positions[SERVO_COUNT]={512,512};
uint16_t data[SERVO_COUNT*3];

int main(void) {
	uint8_t state;
	uint8_t cServo;

	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu(DYN_CFG_PORT, DYN_CFG_SPEED, _DYNAMIXEL_SERIAL_DEFAULTS);

	dynamixel_set_debug(dyn,DYN_CFG_DEBUG);
	
	if (dynamixel_connect(dyn)==0) {
		for (cServo=0; cServo<SERVO_COUNT; cServo++) {
			data[(cServo*3)]=ids[cServo];
			data[(cServo*3)+1]=positions[cServo];
			data[(cServo*3)+2]=speed[cServo];
		}
		state=dynamixel_sync_write_words(dyn, DYNAMIXEL_R_GOAL_POSITION_L, SERVO_COUNT,2,data);
		
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}

