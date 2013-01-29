ldl reg_1, 25
START: getservice button1
add reg_1, reg_0
mv reg_1, reg_0
getservice button0
sub reg_1, reg_0
mv reg_1, reg_0
setservice bargraph, reg_1
sendmsg reg_1, 1, 3
wait 10
sendmsg reg_1, 2, 3
wait 10
convert reg_str_1, reg_1
sendmsg reg_str_1, 1, 2
wait 500
ldl reg_0, 1
jmpgr START
