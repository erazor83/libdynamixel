#include "dynamixel.h"

int main(void)
{
		
	dynamixel_t *dyn;

	dyn = dynamixel_new_rtu("/dev/ttyUSB0", 1000000,'N',8,1);
	dynamixel_connect(dyn);

	/* Read 5 registers from the address 0 */
	dynamixel_ping(dyn, 1);

	dynamixel_close(dyn);
	dynamixel_free(dyn);
}