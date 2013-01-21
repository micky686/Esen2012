/*
 * comm_layer.c
 *
 *  Created on: 21.12.2012
 *      Author: igor
 */

#include "comm_layer.h"

frame_list_t frm_list;


uint8_t send_message(frame_t frame){

	uint8_t res = 0;
	packet_t packet;
	uint16_t packet_id = 0;
	uint8_t packet_ind =0;

	while (frame.index < frame.frame_length){
		uint8_t data_len = 0;
		memset(packet.payload, 0, PACKET_LEN);
		packet.header = 0;

		SET_HEADER_NODE(packet.header, frame.dst_node);
		SET_PAYLOAD_SRC_BOARD(packet.payload, frame.frame_id.src_board);
		SET_PAYLOAD_SRC_NODE(packet.payload, frame.frame_id.src_node);
		SET_PAYLOAD_FRAME_ID(packet.payload, frame.frame_id.id);

		if (frame.index == 0){
			//start package
			SET_PAYLOAD_TYPE(packet.payload, START_PACKET);
			SET_PAYLOAD_PACK_ID(packet.payload, packet_id);
			SET_PAYLOAD_DST_BOARD(packet.payload, frame.dst_board);
			SET_PAYLOAD_DST_AGENT(packet.payload, frame.dst_agent);
			SET_PAYLOAD_FRAME_LEN(packet.payload, frame.frame_length);

			if (frame.frame_length + START_PACKET_LEN > PACKET_LEN){
				SET_HEADER_LEN(packet.header, PACKET_LEN);
				data_len =  PACKET_LEN - START_PACKET_LEN;
			} else {
				SET_HEADER_LEN(packet.header, (START_PACKET_LEN + frame.frame_length));
				data_len =  frame.frame_length;
			}
			packet_ind = START_PACKET_LEN;

		} else {
			//data packages
			SET_PAYLOAD_TYPE(packet.payload, DATA_PACKET);
			SET_PAYLOAD_PACK_ID(packet.payload, packet_id);

			if ((frame.frame_length - frame.index) + DATA_PACKET_LEN > PACKET_LEN){
				SET_HEADER_LEN(packet.header, PACKET_LEN);
				data_len =  PACKET_LEN - DATA_PACKET_LEN;
			} else{
				SET_HEADER_LEN(packet.header,  (DATA_PACKET_LEN + (frame.frame_length - frame.index)));
				data_len =  frame.frame_length - frame.index;
			}
			packet_ind = DATA_PACKET_LEN;
		}

		memcpy((packet.payload+packet_ind), (frame.data+frame.index), data_len);
		frame.index+= data_len;

		res += send_msg(packet.header, packet.payload);
		packet_id += 1;
	}

	return res;
}



/*uint8_t send_msg(uint8_t message_header, uint8_t *msg_body){

	printf("header %x\n", message_header);
	uint8_t i = 0;
	for (i = 0; i < (message_header & 0x0f); i++){
		printf("%d: data: %x\n", i, msg_body[i]);
	}
	fflush(stdout);
	recv_handler(message_header&0x0f, msg_body);
}*/

/* |	dst_node	|	packet len	|
 * |	start_type	|  	src board	|
 * |	src_node	|	frame id	|
 * |		packet id hi			|
 * |		packet id low			|
 * |	dst board	|	dst agent	|
 * |		frame length hi			|
 * |		frame length low		|
 * |			data				|
 * |			...					|
 * |			crc					|
 *
 *
 * |	dst_node	|	packet len	|
 * |	data_type	|  	src board	|
 * |	src_node	|	frame id	|
 * |		packet id hi			|
 * |		packet id low			|
 * |			data				|
 * |			...					|
 * |			crc					|
 */
void recv_handler(uint8_t msg_length, uint8_t *msg_body){

	uint8_t agent_id = GET_PAYLOAD_DST_AGENT(msg_body);
	if (GET_PAYLOAD_TYPE(msg_body) == START_PACKET){

		//new frame received
		uint16_t frame_size = GET_PAYLOAD_FRAME_LEN(msg_body);

		if (frame_size > (msg_length - START_PACKET_LEN)){
			//we need to buffer
			frame_t* new_frame = malloc(sizeof(frame_t));
			memset(new_frame, 0, sizeof(frame_t));
			new_frame->frame_id.id = GET_PAYLOAD_FRAME_ID(msg_body);
			new_frame->frame_id.src_node = GET_PAYLOAD_SRC_NODE(msg_body);
			new_frame->frame_id.src_board = GET_PAYLOAD_SRC_BOARD(msg_body);
			new_frame->dst_agent = agent_id;
			new_frame->frame_length = frame_size;
			new_frame->data = (char*) malloc(frame_size+1);
			memcpy(new_frame->data, msg_body+START_PACKET_LEN, msg_length - START_PACKET_LEN);
			new_frame->index = msg_length - START_PACKET_LEN;

			if (frm_list.last != NULL){
				frm_list.last->next_frame = new_frame;
				frm_list.last = new_frame;
				frm_list.size += 1;
			} else {
				frm_list.first = new_frame;
				frm_list.last = new_frame;
				frm_list.size = 1;
			}

		} else {
			//write directly to agent
			platform.agents[agent_id].rec_msg_content = (char*)realloc(platform.agents[agent_id].rec_msg_content, frame_size +1);
			memset(platform.agents[agent_id].rec_msg_content, 0, frame_size + 1);
			memcpy(platform.agents[agent_id].rec_msg_content, msg_body+START_PACKET_LEN, frame_size);
			platform.agents[agent_id].rec_msg_len = frame_size;
		}


	} else {
		//new packet received
		uint8_t id = GET_PAYLOAD_FRAME_ID(msg_body);
		uint8_t node  = GET_PAYLOAD_SRC_NODE(msg_body);
		uint8_t board = GET_PAYLOAD_SRC_BOARD(msg_body);
		uint8_t i;
		// search for frame
		frame_t* current = frm_list.first;
		frame_t* prev = frm_list.first;

		while (current != NULL){
			if (current->frame_id.id == id && current->frame_id.src_node == node && current->frame_id.src_board == board){
				//found
				memcpy(current->data+current->index, msg_body+DATA_PACKET_LEN, msg_length - DATA_PACKET_LEN);
				current->index += msg_length - DATA_PACKET_LEN;
				if (current->frame_length == current->index){
					//everything received
					platform.agents[current->dst_agent].rec_msg_content = (char*)realloc(platform.agents[current->dst_agent].rec_msg_content, current->frame_length + 1);
					memset(platform.agents[current->dst_agent].rec_msg_content, 0, current->frame_length + 1);
					memcpy(platform.agents[current->dst_agent].rec_msg_content, current->data, current->frame_length);
					platform.agents[current->dst_agent].rec_msg_len = current->frame_length;

					prev->next_frame = current->next_frame;

					//only one element
					if (frm_list.last == frm_list.first){
						frm_list.last = NULL;
						frm_list.first = NULL;
					}
					else if (current == frm_list.first){
						frm_list.first = current->next_frame;
					} else if (current == frm_list.last){
						frm_list.last = prev;
					}
					free(current->data);
					free(current);
					current = NULL;
					frm_list.size -= 1;
				} else {
					current = NULL;
				}
			} else {
				prev = current;
				current = prev->next_frame->next_frame;
			}
		}
	}

}



/*typedef struct {

	uint8_t id;				0
	uint8_t priority;		1
	agent_status_t status;	2

	uint32_t status_flag;	3456
	uint16_t pc;			78

	int16_t regs[REG_MAX];	910 1112 1314 1516 1718 1920 2122

	uint16_t code_len;		2324
	uint16_t* code;

	uint16_t regstr_len [STR_REG_MAX]; 2526 2728 2930
	char** reg_str;

	uint16_t rec_msg_len; 3132
	char* rec_msg_content;


} agent_t;*/
uint8_t* serialize_agent(agent_t agent, uint16_t* agent_len){

	*agent_len = FIXED_LEN + (agent.code_len * sizeof(uint16_t)) + agent.regstr_len[0] + agent.regstr_len[1] + agent.regstr_len[2] + agent.rec_msg_len;
	uint8_t* agent_str = (uint8_t*) malloc(agent_len);

	SET_MOBILITY_HEADER(agent_str);

	SET_AGENT_ID(agent_str, agent.id);
	SET_AGENT_PRIO(agent_str, agent.priority);
	SET_AGENT_STATUS(agent_str, agent.status);

	SET_AGENT_FLAG_REG(agent_str, agent.status_flag);
	SET_AGENT_PC(agent_str, agent.pc);

	SET_AGENT_REG(agent_str, agent, 0);
	SET_AGENT_REG(agent_str, agent, 1);
	SET_AGENT_REG(agent_str, agent, 2);
	SET_AGENT_REG(agent_str, agent, 3);
	SET_AGENT_REG(agent_str, agent, 4);
	SET_AGENT_REG(agent_str, agent, 5);
	SET_AGENT_REG(agent_str, agent, 6);

	SET_AGENT_CODE_LEN(agent_str, agent.code_len);

	SET_AGENT_REG_STR_LEN(agent_str, agent, 0);
	SET_AGENT_REG_STR_LEN(agent_str, agent, 1);
	SET_AGENT_REG_STR_LEN(agent_str, agent, 2);

	SET_AGENT_REC_MSG_LEN(agent_str, agent.rec_msg_len);

	//copy code
	memcpy(agent_str + DYNAMIC_START, agent.code, agent.code_len * sizeof(uint16_t));

	//copy reg_str
	uint32_t pos = DYNAMIC_START + (agent.code_len * sizeof (uint16_t));
	memcpy(agent_str + pos, agent.reg_str[0], agent.regstr_len[0]);

	pos += agent.regstr_len[0];
	memcpy(agent_str + pos, agent.reg_str[1], agent.regstr_len[1]);

	pos += agent.regstr_len[1];
	memcpy(agent_str + pos, agent.reg_str[2], agent.regstr_len[2]);

	pos += agent.regstr_len[2];
	memcpy(agent_str + pos, agent.rec_msg_content, agent.rec_msg_len);

	pos += agent.rec_msg_len;

	SET_MOBILITY_END(agent_str, pos);

	return agent_str;
}

agent_t deserialize_agent(uint8_t* packet){
	agent_t agent;


	agent.id = GET_AGENT_ID(packet);
	agent.priority = GET_AGENT_PRIO(packet);
	agent.status = GET_AGENT_STATUS(packet);
	agent.status_flag = GET_AGENT_FLAG_REG(packet);
	agent.pc = GET_AGENT_PC(packet);

	agent.regs[0] = GET_AGENT_REG(packet, 0);
	agent.regs[1] = GET_AGENT_REG(packet, 1);
	agent.regs[2] = GET_AGENT_REG(packet, 2);
	agent.regs[3] = GET_AGENT_REG(packet, 3);
	agent.regs[4] = GET_AGENT_REG(packet, 4);
	agent.regs[5] = GET_AGENT_REG(packet, 5);
	agent.regs[6] = GET_AGENT_REG(packet, 6);

	agent.code_len = GET_AGENT_CODE_LEN(packet);

	agent.regstr_len[0] = GET_AGENT_REG_STR_LEN(packet, 0);
	agent.regstr_len[1] = GET_AGENT_REG_STR_LEN(packet, 1);
	agent.regstr_len[2] = GET_AGENT_REG_STR_LEN(packet, 2);

	agent.rec_msg_len = GET_AGENT_REC_MSG_LEN(packet);

	agent.code = malloc(agent.code_len * sizeof(uint16_t));
	memcpy(agent.code, packet+DYNAMIC_START, agent.code_len * sizeof(uint16_t));

	agent.reg_str = (char**) malloc(STR_REG_MAX * sizeof(char*));

	uint32_t pos = DYNAMIC_START + (agent.code_len * sizeof(uint16_t));
	agent.reg_str[0] = malloc (agent.regstr_len[0]);
	memcpy(agent.reg_str[0], packet+pos, agent.regstr_len[0]);

	pos += agent.regstr_len[0];
	agent.reg_str[1] = malloc( agent.regstr_len[1]);
	memcpy(agent.reg_str[1], packet+pos, agent.regstr_len[1]);

	pos += agent.regstr_len[1];
	agent.reg_str[2] = malloc(agent.regstr_len[2]);
	memcpy(agent.reg_str[2], packet+pos, agent.regstr_len[2]);

	pos += agent.regstr_len[2];
	agent.rec_msg_content = malloc(agent.rec_msg_len);
	memcpy(agent.rec_msg_content, packet+pos, agent.rec_msg_len);

	return agent;
}

