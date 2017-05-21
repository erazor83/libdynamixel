#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
sys.path=['../']+sys.path

import dynamixel
import time

DYN_CFG_SERVO_ID=1

#ctx=dynamixel.dynamixel_ctx()

ctx=dynamixel.dynamixel_new_rtu('/dev/ttyACM0', 1000000, 'E', 8, 1)
con=dynamixel.dynamixel_connect(ctx)

if con==0:

	while 1:
		ping=dynamixel.dynamixel_ping(ctx,DYN_CFG_SERVO_ID)
		print(ping)
		time.sleep(1)
	#print values[0]
	#print retval
