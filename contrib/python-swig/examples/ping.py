#!/usr/bin/env python
# -*- coding: utf-8 -*-
# not tested!

import sys
sys.path=['../']+sys.path

import dynamixel
import time

DYN_CFG_SERVO_ID=1

ctx=dynamixel.dynamixel_ctx()

dynamixel.dynamixel_new_rtu(ctx, '/dev/ttyUSB0', 1000000, 'even', 8, 1)
dynamixel.dynamixel_connect(ctx)

#values=libmodbus.int_Array(10)

while 1:
	ping=dynamixel.dynamixel_ping(ctx,DYN_CFG_SERVO_ID);
	print(ping)
	time.sleep(1)
#print values[0]
#print retval
