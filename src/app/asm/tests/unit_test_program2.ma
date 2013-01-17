ldl reg_0, 32
LABEL0: ldl reg_1, 54
mv reg_2, reg_0
add reg_2, reg_1
mv reg_3, reg_0
compare reg_3, reg_1
jmpgr LABEL0
setservice bargraph, reg_1
clone
die
