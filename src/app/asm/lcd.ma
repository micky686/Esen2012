clr reg_str_0
storecr reg_str_0, C
storecr reg_str_0, U
storecr reg_str_0, R
storecr reg_str_0, R
storecr reg_str_0, E
storecr reg_str_0, N
storecr reg_str_0, T
storecr reg_str_0, T
storecr reg_str_0, E
storecr reg_str_0, M
storecr reg_str_0, P
storecr reg_str_0, E
storecr reg_str_0, R
storecr reg_str_0, A
storecr reg_str_0, T
storecr reg_str_0, U
storecr reg_str_0, R
storecr reg_str_0, E
ldl reg_0, 2
setservice lcd, reg_str_0
START: clr reg_str_1
pullmsg reg_str_1
jmpls START
ldl reg_0, 4
setservice lcd, reg_str_1
wait 500
ldl reg_0, 1
jmpgr START
