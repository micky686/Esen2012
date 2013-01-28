START: clr reg_str_0
getservice temp
sendmsg reg_str_0, 0, 1
wait 500
sendmsg reg_str_0, 0, 2
wait 500
ldl reg_0, 1
jmpgr START
