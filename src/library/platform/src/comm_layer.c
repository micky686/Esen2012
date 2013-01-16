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
	//uint8_t framebuf_size = sizeof(*recv_frames) / sizeof(frame_t);

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
			platform.agents[agent_id].rec_msg_content = (char*)realloc(platform.agents[agent_id].rec_msg_content, frame_size);
			memcpy(platform.agents[agent_id].rec_msg_content, msg_body+START_PACKET_LEN, frame_size);
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
					platform.agents[current->dst_agent].rec_msg_content = (char*)realloc(platform.agents[agent_id].rec_msg_content, current->frame_length);
					memcpy(platform.agents[current->dst_agent].rec_msg_content, current->data, current->frame_length);

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
				}
			} else {
				prev = current;
				current = prev->next_frame->next_frame;
			}
		}
	}

}
