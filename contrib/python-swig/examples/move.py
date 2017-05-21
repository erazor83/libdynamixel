#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
sys.path=['../']+sys.path

import dynamixel
import time

DYN_CFG_SERVO_ID=2

#ctx=dynamixel.dynamixel_ctx()

ctx=dynamixel.dynamixel_new_rtu('/dev/ttyACM0', 1000000, 'E', 8, 1)
con=dynamixel.dynamixel_connect(ctx)
dynamixel.dynamixel_set_debug(ctx,True)

if con==0:

	while 1:
		print(dynamixel.dynamixel_read_data(
			ctx,
			DYN_CFG_SERVO_ID,
			dynamixel.DYNAMIXEL_R_PRESENT_POSITION_L,
			2
		))
		
		print(dynamixel.dynamixel_write_data(
			ctx,
			DYN_CFG_SERVO_ID,
			dynamixel.DYNAMIXEL_R_GOAL_POSITION_L,
			[180,1]
		))
		time.sleep(1)
		print(dynamixel.dynamixel_write_data(
			ctx,
			DYN_CFG_SERVO_ID,
			dynamixel.DYNAMIXEL_R_GOAL_POSITION_L,
			[190,1]
		))
		time.sleep(1)
