#include <stdio.h>

#include "dynamixel.h"

uint8_t *found_ids;
uint8_t id_count;
int main(void) {
	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu("/dev/ttyUSB0", 1000000,'N',8,1);

	dynamixel_set_debug(dyn,true);
	if (dynamixel_connect(dyn)==0) {
		id_count=dynamixel_search(dyn, 1,30,&found_ids);
		printf("%i Dynamixels found\n",id_count);
		while (id_count--) {
			printf(
				"  * Dynamixels #% 3i found @ % 2i\n",
				id_count,
				found_ids[id_count]
			);
			id_count--;
		}
		dynamixel_close(dyn);
	}
	dynamixel_free(dyn);
}