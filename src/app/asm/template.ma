mv reg_0, reg_2
clone
add acc, 55
compare acc, 12
compare reg_0, reg_1

move temp

move led_matrix

move bargraph

pullmsg acc

pullmsg reg_2

pullmsg reg_str_0

wait 0x25

wait 64

wait 0b11

priority 1

priority 0

priority high

getservice temp

getservice bargraph

getservice add
