/*
 * exe_layer.c
 *
 * Created on: Dec 10, 2012
 * Author: igor
 */
#include "exe_layer.h"

uint8_t execute_agent(agent_t *agent, uint8_t opcode_size) {

	uint8_t opcodes_done = 0;

	while (opcodes_done < opcode_size) {
		//1. fetch next opcode
		uint16_t opcode = agent->code[agent->pc];

		//2. decode opcode
		opcode_t dec_opcode = decode_opcode(opcode);

		//3. execute opcode
		execute_opcode(agent, dec_opcode);

		//4. increase program counter
		if (agent->status != blocked) {
			if (agent->pc < agent->code_len - 1) {
				agent->pc += 1;
			} else {
				agent->status = stopped;
				break;
			}
			opcodes_done += 1;
		} else {
			return opcodes_done;
		}

	}

	return opcodes_done;
}

opcode_t decode_opcode(uint16_t opcode) {

	opcode_t result;

	uint8_t nibble1 = NIBBLE1(opcode);
	uint8_t nibble2 = NIBBLE2(opcode);

	switch (nibble1) {
	//0000
	case 0:
		switch (nibble2) {

		//add reg_d, reg_r
		//0000 0011 dddd rrrr
		case 3:
			 result.id = ADD_R;
			 result.reg1 = NIBBLE3(opcode);
			 result.reg2 = NIBBLE4(opcode);
			break;

			//wait delay_ms
			//0000 0101 dddd dddd
		case 5:
			break;

			//sub reg_m, reg_s
			//0000 0110 mmmm ssss
		case 6:
			break;

			//getservice
			//0000 0111 dddd dddd
		case 7:

			break;

			//priority value
			//0000 1000 pppp pppp
		case 8:
			break;

			//div reg_d, reg_r
			//0000 1001 dddd rrrr
		case 9:
			break;

			//compare reg_d, reg_r
			//0000 1010 dddd rrrr
		case 10:
			break;

			//mul reg_d, reg_r
			//0000 1100 dddd rrrr
		case 12:
			break;

			//mv reg_d, reg_r
			//0000 1101 dddd rrrr
		case 13:
			break;

		}
		break;
		//1111
	case 15:
		switch (nibble2) {
		//move service
		//1111 0001 ssss ssss
		case 1:
			break;

			//clone
			//1111 0010 0000 0000
		case 2:
			break;

			//jmpgr offset
			//1111 0011 vvvv vvvv
		case 3:
			result.id = JMP_O;
			result.value = BYTE2(opcode);
			break;

			//die
			//1111 0100 0000 0000
		case 4:
			break;

			//jmpeq offset
			//1111 0110 vvvv vvvv
		case 6:
			break;

			//sendmsg reg_d, node, agent
			//1111 rrrr nn aa
		case 8:
			break;

			//pullmsg
			//1111 1010 0000 0000
		case 10:
			break;

			//jmpls offset
			//1111 1100 vvvv vvvv
		case 12:
			break;

		}
		break;

		//add reg_r, value
		//0011 rrrr vvvv vvvv
	case 3:
		result.id = ADD_V;
		result.reg1 = NIBBLE2(opcode);
		result.value = BYTE2(opcode);
		break;

		//sub reg_m, value
		//0110 mmmm vvvv vvvv
	case 6:
		break;

		//setservice service, reg
		//0111 rrrr ssss ssss
	case 7:
		result.id = SETSERVICE;
		result.reg1 = NIBBLE2(opcode);
		result.value = BYTE2(opcode);
		break;

		//div reg_d, value
		//1001 dddd vvvv vvvv
	case 9:
		break;

		//storecr reg_str, char
		//1011 rrrr vvvv vvvv
	case 11:
		break;

		//mul reg1, value
		//1100 dddd vvvv vvvv
	case 12:
		break;

		//store reg_d, value
		//1101 dddd vvvv vvvv
	case 13:
		result.id = STORE;
		result.reg1 = NIBBLE2(opcode);
		result.value = BYTE2(opcode);
		break;

	default:
		break;
	}

	return result;
}

void execute_opcode(agent_t *agent, opcode_t opcode) {

	switch (opcode.id) {
	case SETSERVICE:
		switch (opcode.value) {

		case SERVICE_BARGRAPH:
			if (platform.drivers.set_bargraph != NULL) {
				platform.drivers.set_bargraph(agent->regs[opcode.reg1]);
			} else {
				agent->error = ERROR_NO_SERVICE_PRESENT;
			}
			break;

		default:
			break;

		}
		break;

	case STORE:
		agent->regs[opcode.reg1] = opcode.value;
		break;

	case ADD_R:
		agent->regs[0] = agent->regs[opcode.reg1] + agent->regs[opcode.reg2];
		break;

	case ADD_V:
		agent->regs[0] = agent->regs[opcode.reg1] + opcode.value;
		break;

	case JMP_O:
		agent->pc = agent->pc + opcode.value;
		break;

	default:
		break;

	}

}
