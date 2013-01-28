move temp
compare reg_0, 0
jmpeq START1
die
START1: ldl reg_1, 25
mul reg_1, 8
mv reg_1, reg_0			//desired
ldl reg_2, 100			//kp
ldl reg_3, 15			//kd
START:getservice temp
sub reg_0, reg_1		//desired - current = e
setservice bargraph, reg_0
mv reg_5, reg_0			//temporary e
add reg_0, reg_4		//esum + e
mv reg_4, reg_0
mul reg_5, reg_2		//kp * e
mv reg_7, reg_0
sub reg_6, reg_5		//e - ealt
mul reg_0, reg_3		//kd (e  - ealt)
add reg_0, reg_7		// y
mv reg_8, reg_0			
mv reg_6, reg_5			//ealt
compare reg_8, 0
jmpgr S
ldl reg_8, 0
S: compare reg_8, 100
jmpls S1
ldl reg_8, 100
S1:setservice fan, reg_8
ldl reg_0, 1
jmpgr START

