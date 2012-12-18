ldl reg_0, 35
MET0:	ldh reg_1, 56
ldl reg_2, 120
ldf reg_3, 143

jmpls LAB3

LAB:ldl reg_0, -1
LAB2:ldh reg_4, -5

ldl reg_1, 0xFF
LAB2: ldh reg_5, 0b10101
ldl reg_6, 0b1111

LAB3:	clr reg_str_0

clr reg_str_1

clr reg_str_2


jmpeq MET0

add reg_0, reg_1
