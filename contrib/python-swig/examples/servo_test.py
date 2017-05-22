#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys,tty,termios,select

import sys
sys.path=['../']+sys.path

import dynamixel
import time

SERVO_IDs=[
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
]

cServoIDX=0
cServoID=SERVO_IDs[cServoIDX]
cStartPosition=0

def setPosition(ctx,sid,value):
	dynamixel.dynamixel_write_data(
		ctx,
		sid,
		dynamixel.DYNAMIXEL_R_GOAL_POSITION_L,
		[value&0xff,value>>8]
	)
	
def getCurrentPosition(ctx,sid):
	ret=dynamixel.dynamixel_read_data(
		ctx,
		sid,
		dynamixel.DYNAMIXEL_R_PRESENT_POSITION_L,
		2
	)
	if ret[0]:
		return ret[1][0]+(ret[1][1]<<8)
	
def nextServo():
	global cServoIDX,SERVO_IDs,cServoID
	if cServoIDX<(len(SERVO_IDs)-1):
		cServoIDX=cServoIDX+1
		cServoID=SERVO_IDs[cServoIDX]
	
def prevServo():
	global cServoIDX,SERVO_IDs,cServoID
	if cServoIDX>0:
		cServoIDX=cServoIDX-1
		cServoID=SERVO_IDs[cServoIDX]

def getKey():
	if select.select([sys.stdin,],[],[],0.0)[0]:
		ch_buff=''
		while 1:
			ch = sys.stdin.read(1)
			if ch:
				ch_buff=ch_buff+ch
			else:
				break
			
		return ch_buff

ctx=dynamixel.dynamixel_new_rtu('/dev/ttyACM0', 1000000, 'E', 8, 1)
con=dynamixel.dynamixel_connect(ctx)
dynamixel.dynamixel_set_debug(ctx,True)

if con==0:
	old_settings = termios.tcgetattr(sys.stdin)
	new_settings = termios.tcgetattr(sys.stdin)
	new_settings[3] = new_settings[3] & ~(termios.ECHO | termios.ICANON) # lflags
	new_settings[6][termios.VMIN] = 0  # cc
	new_settings[6][termios.VTIME] = 0 # cc
	termios.tcsetattr(sys.stdin, termios.TCSADRAIN, new_settings)
	
	try:
		cStartPosition=getCurrentPosition(ctx,cServoID)
		while 1:
			key=getKey()
			if key=='q':
				break
			elif key=='\x1b[C':
				print('Right')
				nextServo()
				
				cStartPosition=getCurrentPosition(ctx,cServoID)
			elif key=='\x1b[D':
				print('Left')
				prevServo()
				cStartPosition=getCurrentPosition(ctx,cServoID)

			setPosition(ctx,cServoID,cStartPosition-10)
			time.sleep(0.5)
			setPosition(ctx,cServoID,cStartPosition+10)
			time.sleep(0.5)
			
	finally:
		termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)