move temp
getservice temp
sendmsg acc, 1, 3
move heater
store acc, 45
setservice heater, acc
add acc, 12


LABEL0: store reg_0, 
getservice bargraph
mul reg_0, acc
setservice bargraph, acc
die
