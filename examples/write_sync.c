/*
 * set position via sync write
 * 
 */

#include <stdio.h>

#include "dynamixel.h"

#include "config.h"

#define SERVO_COUNT	2
uint16_t ids[SERVO_COUNT]={1,2};
uint16_t speed[SERVO_COUNT]={100,100};
uint16_t positions[SERVO_COUNT]={512,512};
uint8_t data[200];

int main(void) {
	uint8_t *pdata;
	uint8_t state;
	uint8_t cServo;

	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu(DYN_CFG_PORT, DYN_CFG_SPEED, _DYNAMIXEL_SERIAL_DEFAULTS);

	dynamixel_set_debug(dyn,DYN_CFG_DEBUG);
	
	if (dynamixel_connect(dyn)==0) {
		pdata=data;
		for (cServo=0; cServo<SERVO_COUNT; cServo++) {
			*(pdata++)=ids[cServo];
			*(pdata++)=positions[cServo]&0xff;
			*(pdata++)=(positions[cServo]>>8)&0xff;
			*(pdata++)=speed[cServo]&0xff;
			*(pdata++)=(speed[cServo]>>8)&0xff;
		}
		state=dynamixel_sync_write(dyn, DYNAMIXEL_R_GOAL_POSITION_L, SERVO_COUNT,4,data);
		
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}

