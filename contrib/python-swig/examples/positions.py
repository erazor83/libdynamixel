#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
sys.path=['../']+sys.path

import dynamixel
import time

SERVO_NAMES=[
	['TIBIA RF', 6],
	['TIBIA LF', 5],
	
	['TIBIA RM',18],
	['TIBIA LM',17],
	
	['TIBIA RR',12],
	['TIBIA LR',11],
	
	['COXA RF',2],
	['COXA LF',1],
	
	['COXA RM',14],
	['COXA LM',13],

	['COXA RR',8],
	['COXA LR',7],

	['FEMUR RF',4],
	['FEMUR LF',3],

	['FEMUR RM',16],
	['FEMUR LM',15],

	['FEMUR RR',10],
	['FEMUR LR',9]
]
	
def getCurrentPosition(ctx,sid):
	ret=dynamixel.dynamixel_read_data(
		ctx,
		sid,
		dynamixel.DYNAMIXEL_R_PRESENT_POSITION_L,
		2
	)
	if ret[0]:
		return ret[1][0]+(ret[1][1]<<8)
	

ctx=dynamixel.dynamixel_new_rtu('/dev/ttyACM0', 1000000, 'E', 8, 1)
con=dynamixel.dynamixel_connect(ctx)

if con==0:
	while 1:
		print("========")
		for cItem in SERVO_NAMES:
			cPos=getCurrentPosition(ctx,cItem[1])
			print("% 20s: %i"%(cItem[0],cPos))
			
		time.sleep(1)
