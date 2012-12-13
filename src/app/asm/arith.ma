LABEL: add acc, 5
AD2:	add reg_2, reg_5
LAB2: add reg_0, reg_5

A:    mul reg_0, reg_4
M2:  mul reg_2, 1

div reg_2, reg_5

sub reg_5, acc
sub acc, 0xFF
