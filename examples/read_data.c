#include <stdio.h>

#include "dynamixel.h"

uint8_t *data;
uint8_t state;

int main(void) {
	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu("/dev/ttyUSB0", 1000000,'N',8,1);

	dynamixel_set_debug(dyn,true);
	if (dynamixel_connect(dyn)==0) {
		state=dynamixel_read_data(dyn,1,DYNAMIXEL_R_ID,1,&data);
		
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}

