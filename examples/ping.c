/*
 * ping one servo
 * 
 */


#include <stdio.h>


#include "dynamixel.h"

#include "config.h"
#define DYN_CFG_SERVO_ID	1


int main(void) {
	dynamixel_t *dyn;
	
	int8_t ping;

	dyn = dynamixel_new_rtu(DYN_CFG_PORT, DYN_CFG_SPEED, _DYNAMIXEL_SERIAL_DEFAULTS);

	dynamixel_set_debug(dyn,DYN_CFG_DEBUG);
	
	if (dynamixel_connect(dyn)==0) {
		ping=dynamixel_ping(dyn,DYN_CFG_SERVO_ID);

		if (ping) {
			printf("Servo @%i reachable!\n",DYN_CFG_SERVO_ID);
		} else {
			printf("Servo @%i timed out!\n",DYN_CFG_SERVO_ID);
		}
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}