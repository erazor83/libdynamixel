#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
sys.path=['../']+sys.path

import yaml

import dynamixel
	
def writeRegister(ctx,sid,addr,value):
	return dynamixel.dynamixel_read_data(
		ctx,
		sid,
		dynamixel.DYNAMIXEL_R_PRESENT_POSITION_L,
		2
	)
	

ctx=dynamixel.dynamixel_new_rtu('/dev/ttyACM0', 1000000, 'E', 8, 1)
con=dynamixel.dynamixel_connect(ctx)

if con==0:
	config=yaml.load(open(sys.argv[1]))
	for cItem in config:
		for cServo in cItem['servos']:
			for cRegister in cItem['registers']:
				val=cItem['registers'][cRegister]
				print(cServo,cRegister,val,writeRegister(ctx,cServo,cRegister,val))
	