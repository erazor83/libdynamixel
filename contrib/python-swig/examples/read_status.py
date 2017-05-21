#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
sys.path=['../']+sys.path

import dynamixel

ctx=dynamixel.dynamixel_new_rtu('/dev/ttyACM0', 1000000, 'E', 8, 1)
con=dynamixel.dynamixel_connect(ctx)

if con==0:
	for i in range(1,19):
		ret=dynamixel.dynamixel_read_data(
			ctx,
			i,
			dynamixel.DYNAMIXEL_R_MODELNUMBER_L,
			4
		)
		print(i,ret)
