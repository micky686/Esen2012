clr reg_str_0
storecr reg_str_0, S
storecr reg_str_0, E
storecr reg_str_0, T
storecr reg_str_0, -
storecr reg_str_0, T
storecr reg_str_0, E
storecr reg_str_0, M
storecr reg_str_0, P
storecr reg_str_0, :
ldl reg_0, 6
setservice lcd, reg_str_0
START: clr reg_str_1
pullmsg reg_str_1
jmpls START
ldl reg_0, 8
setservice lcd, reg_str_1
wait 500
ldl reg_0, 1
jmpgr START
