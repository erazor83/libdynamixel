/*
 * searches for all dynamixels and toggles their LED
 * 
 */


#include <stdio.h>
#include <time.h>
#include <string.h>
#include "dynamixel.h"

#include "config.h"
#define DYN_CFG_SERVO_ID	1
#define DYN_CFG_REGISTER	DYNAMIXEL_R_LED


int main(void) {
	uint8_t *data;
	uint8_t state;

	uint8_t req_data[1]={0};

	uint8_t *found_ids;
	uint8_t found_ids_local[30];
	uint8_t id_count;
	uint8_t cid;
	uint8_t cidx;
	
	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu(DYN_CFG_PORT, DYN_CFG_SPEED, _DYNAMIXEL_SERIAL_DEFAULTS);
	dynamixel_set_debug(dyn,false);
	
	if (dynamixel_connect(dyn)==0) {

		id_count=dynamixel_search(dyn, 1,30,&found_ids);
		if (id_count<=0) {
			return(0);
		}
		
		/* we need to copy the result, cos it's getting overritten with the next response*/
		memcpy(found_ids_local,found_ids,id_count);
		
		while (1) {
			for (cidx=0;cidx<id_count;cidx++) {
				cid=found_ids_local[cidx];

				state=dynamixel_read_data(dyn,cid,DYN_CFG_REGISTER,1,&data);
				if (state>0) {
					req_data[0]=(~req_data[0])&0x01;
					dynamixel_write_data(dyn,cid,DYN_CFG_REGISTER,1,req_data);
					printf("Register value: %i\n",data[0]);
				}
				usleep(5000);
			}
			req_data[0]=~req_data[0];
		}
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}

