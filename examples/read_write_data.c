/*
 * read and write register
 * 
 */


#include <stdio.h>
#include <time.h>

#include "dynamixel.h"

#include "config.h"
#define DYN_CFG_SERVO_ID	1
#define DYN_CFG_REGISTER	DYNAMIXEL_R_LED


int main(void) {
	uint8_t *data;
	uint8_t state;

	uint8_t req_data[1]={0};

	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu(DYN_CFG_PORT, DYN_CFG_SPEED, _DYNAMIXEL_SERIAL_DEFAULTS);

	dynamixel_set_debug(dyn,true);
	if (dynamixel_connect(dyn)==0) {

		
		while (1) {
			state=dynamixel_write_data(dyn,DYN_CFG_SERVO_ID,DYN_CFG_REGISTER,1,req_data);

			state=dynamixel_read_data(dyn,DYN_CFG_SERVO_ID,DYN_CFG_REGISTER,1,&data);
			if (state>0) {
				printf("Register value: ");
				while (state--) {
					printf("<%02x>",*(data++));
				}
				printf("\n");
			}
			if (req_data[0]==0) {
				req_data[0]=1;
			} else {
				req_data[0]=0;
			}
			usleep(100000);
		}
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}

