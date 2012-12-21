/*
 * comm_layer.c
 *
 *  Created on: 21.12.2012
 *      Author: igor
 */

#include "comm_layer.h"

uint8_t send_message(frame_t frame){

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

		send_msg(packet.header, packet.payload);
		packet_id += 1;
	}


}

void recv_handler(uint8_t msg_length, uint8_t *msg_body){

}



uint8_t send_msg(uint8_t message_header, uint8_t *msg_body){

	printf("header %x\n", message_header);
	uint8_t i = 0;
	for (i = 0; i < (message_header & 0x0f); i++){
		printf("%d: data: %x\n", i, msg_body[i]);
	}
	fflush(stdout);
}
