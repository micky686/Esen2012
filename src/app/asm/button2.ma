START1: clr reg_str_0
storecr reg_str_0, S
storecr reg_str_0, E
storecr reg_str_0, T
storecr reg_str_0, -
storecr reg_str_0, T
storecr reg_str_0, E
storecr reg_str_0, M
storecr reg_str_0, P
ldl reg_0, 6
setservice lcd, reg_str_0
wait 500
ldl reg_1, 25
convert reg_str_1, reg_1
ldl reg_0, 8
setservice lcd, reg_str_1
wait 500
START: mv reg_2, reg_1
getservice button1
add reg_1, reg_0
mv reg_1, reg_0
getservice button0
sub reg_1, reg_0
mv reg_1, reg_0
compare reg_2, reg_1
jmpeq START
convert reg_str_1, reg_1
ldl reg_0, 8
setservice lcd, reg_str_1
sendmsg reg_1, 1, 3
wait 50
ldl reg_0, 1
jmpgr START
