wait 50
move temp
compare reg_0, 0
jmpeq TEMP
die
TEMP: ldl reg_2, 70 	//kp
ldl reg_3, 10			//kd
ldl reg_11, 2			//ki
ldl reg_12, 3			//ki 
ldl reg_10, 0			//mobility init yes
ldl reg_1, 25			//start desired
TEMP1: pullmsg reg_1
wait 10
mul reg_1, 8
mv reg_9, reg_0			//desired
getservice temp
sub reg_0, reg_9		//current - desired  = e
setservice bargraph, reg_0
mv reg_5, reg_0			//temporary e
add reg_0, reg_4		//esum + e
mv reg_4, reg_0
mul reg_5, reg_2		//kp * e
mv reg_7, reg_0
sub reg_5, reg_6		//e - ealt
mul reg_0, reg_3		//kd (e  - ealt)
add reg_0, reg_7		// y
mv reg_8, reg_0			
div reg_4, reg_11		//ki * esum
mul reg_0, reg_12
add reg_0, reg_8		// y
mv reg_8, reg_0
mv reg_6, reg_5			//ealt
compare reg_8, 0
jmpgr S
ldl reg_8, 0
S: compare reg_8, 100
jmpls S1
ldl reg_8, 100
S1: setservice fan, reg_8
wait 10
sendmsg reg_str_0, 0, 1
compare reg_10, 0
jmpgr TEMP1
wait 30
move lcd
compare reg_0, 0
jmpeq LCD
ldl reg_10, 1
jmpgr TEMP1
LCD: clr reg_str_2
storecr reg_str_2, S
storecr reg_str_2, E
storecr reg_str_2, T
storecr reg_str_2, -
storecr reg_str_2, T
storecr reg_str_2, E
storecr reg_str_2, M
storecr reg_str_2, P
storecr reg_str_2, :
ldl reg_0, 4
setservice lcd, reg_str_2
wait 2
LCD1: mv reg_2, reg_1
getservice button1
add reg_1, reg_0
mv reg_1, reg_0
getservice button0
sub reg_1, reg_0
mv reg_1, reg_0
compare reg_10, 1
jmpls LCD2
compare reg_2, reg_1
jmpeq LCD1
LCD2: clr reg_str_1
convert reg_str_1, reg_1
ldl reg_0, 6
setservice lcd, reg_str_1
wait 2
sendmsg reg_1, 0, 3
compare reg_10, 0
jmpgr LCD1
move led_matrix
compare reg_0, 0
jmpeq LED
ldl reg_10, 1
jmpgr LCD1
LED: pullmsg reg_str_0
setservice led_matrix, reg_str_0
wait 20
ldl reg_0, 1
jmpgr LED

