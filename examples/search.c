/*
 * use libdynamixels scanning function
 * prind all found dynamixel id's
 */


#include <stdio.h>

#include "dynamixel.h"

#include "config.h"

int main(void) {
	uint8_t *found_ids;
	uint8_t id_count;
	
	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu(DYN_CFG_PORT, DYN_CFG_SPEED, _DYNAMIXEL_SERIAL_DEFAULTS);

	dynamixel_set_debug(dyn,DYN_CFG_DEBUG);
	
	if (dynamixel_connect(dyn)==0) {
		id_count=dynamixel_search(dyn, 1,30,&found_ids);
		printf("%i Dynamixels found\n",id_count);
		while (id_count--) {
			printf(
				"  * Dynamixels #% 3i found @ % 2i\n",
				id_count,
				found_ids[id_count]
			);
		}
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}