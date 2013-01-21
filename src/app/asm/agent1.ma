	move temp
	getservice temp
MET: sendmsg	 reg_0, 1, 3
move   heater
A2: setservice heater, reg_1

add reg_0, 12
sub reg_4, reg_2


LABEL0: ldl reg_0, 12
getservice bargraph
mul reg_0, reg_3
getservice bargraph

storecr reg_str_0, c
storecr reg_str_1, a
storecr reg_str_2, b

die
