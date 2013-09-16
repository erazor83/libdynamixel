/*
 * read one register
 * 
 */


#include <stdio.h>


#include "dynamixel.h"

#include "config.h"
#define DYN_CFG_SERVO_ID	1
#define DYN_CFG_REGISTER	DYNAMIXEL_R_ID


int main(void) {
	uint8_t *data;
	uint8_t state;

	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu(DYN_CFG_PORT, DYN_CFG_SPEED, _DYNAMIXEL_SERIAL_DEFAULTS);

	dynamixel_set_debug(dyn,DYN_CFG_DEBUG);
	
	if (dynamixel_connect(dyn)==0) {
		state=dynamixel_read_data(dyn,DYN_CFG_SERVO_ID,DYN_CFG_REGISTER,1,&data);
		if (state>0) {
			printf("Register value: ");
			while (state--) {
				printf("<%02x>",*(data++));
			}
			printf("\n");
		}
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}

