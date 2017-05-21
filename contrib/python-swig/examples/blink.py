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
	while True:
		for v in [0,1]:
			for i in range(0,18):
				ping=dynamixel.dynamixel_write_data(
					ctx,
					i,
					dynamixel.DYNAMIXEL_R_LED,
					[v]
				)
				
				print("%i=%i"%(i,v),ping)
				time.sleep(0.01)
			#print values[0]
			#print retval
