/*
 * comm_layer.h
 *
 *  Created on: 21.12.2012
 *      Author: igor
 */

#include "platform.h"
#include "protocol0.h"

#ifndef COMM_LAYER_H_
#define COMM_LAYER_H_

#define START_PACKET 0x00
#define DATA_PACKET  0x01

#define START_PACKET_LEN 0x07
#define DATA_PACKET_LEN 0x04
#define PACKET_LEN 0x0f

/*
 * |	dst_node	|	packet len	|
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
 *
 */

#define MASK 0x0f
#define SET_HEADER_NODE(header, node) (header |= (node & MASK) << 4)
#define SET_HEADER_LEN(header, len) (header |= (len & MASK))

#define SET_PAYLOAD_TYPE(payload, type) (payload[0] |= (type & MASK) << 4)
#define SET_PAYLOAD_SRC_BOARD(payload, board) (payload[0] |= (board & MASK))

#define SET_PAYLOAD_SRC_NODE(payload, node) (payload[1] |= (node & MASK) << 4)
#define SET_PAYLOAD_FRAME_ID(payload, id) (payload[1] |= (id & MASK))

#define SET_PAYLOAD_PACK_ID(payload, id) \
	payload[2] = (( id & 0xff00 ) >> 8); \
	payload[3] = ( id & 0x00ff)

#define SET_PAYLOAD_DST_BOARD(payload, board) (payload[4] |= (board & MASK) << 4)
#define SET_PAYLOAD_DST_AGENT(payload, agent) (payload[4] |= (agent & MASK))

#define SET_PAYLOAD_FRAME_LEN(payload, len) \
	payload[5] = ((len & 0xff00) >> 8); \
	payload[6] = ( len & 0x00ff)


typedef struct {
	unsigned src_board:4;
	unsigned src_node:4;
	unsigned id:4;
} frame_id_t;

typedef struct {
	uint8_t header;
	uint8_t payload[PACKET_LEN];
} packet_t;

struct frame{
	unsigned dst_node:4;
	unsigned dst_board:4;
	unsigned dst_agent:4;
	frame_id_t frame_id;
	unsigned frame_length:16;
	unsigned index:16;
	struct frame *next_frame;
	char *data;
};

typedef struct frame frame_t;

typedef struct {
	uint16_t size;
	frame_t *first;
	frame_t *last;
}frame_list_t;

extern frame_list_t frm_list;

uint8_t send_message(frame_t frame);
//uint8_t send_msg(uint8_t message_header, uint8_t *msg_body);

#define HI_MASK 0xf0
#define LO_MASK 0x0f

#define GET_DST_NODE(header) (header & HI_MASK) >> 4)
#define GET_MSG_LEN(header) ((header & LO_MASK))

#define GET_PAYLOAD_TYPE(payload) ((payload[0]& HI_MASK) >> 4)
#define GET_PAYLOAD_SRC_BOARD(payload) (payload[0] & LO_MASK)

#define GET_PAYLOAD_SRC_NODE(payload) ((payload[1] & HI_MASK) >> 4)
#define GET_PAYLOAD_FRAME_ID(payload) (payload[1] & LO_MASK)

#define GET_PAYLOAD_PACK_ID(payload) ((payload[2] << 8) | payload[3])

#define GET_PAYLOAD_DST_BOARD(payload) ((payload[4] & HI_MASK) >> 4)
#define GET_PAYLOAD_DST_AGENT(payload) (payload[4] & LO_MASK)

#define GET_PAYLOAD_FRAME_LEN(payload) ((payload[5] << 8) | payload[6])

void recv_handler(uint8_t msg_length, uint8_t *msg_body);

char* serialize_agent(agent_t agent, uint16_t* agent_len);
agent_t deserialize_agent(uint8_t* packet);

#define MOBILITY_BYTE 0x55

#define HEADER_POS 	0
#define ID_POS 		1
#define PRIO_POS	2
#define STATUS_POS	3

#define FLAG_POS1 	4
#define FLAG_POS2 	5
#define FLAG_POS3 	6
#define FLAG_POS4 	7

#define PC_POS1 	8
#define PC_POS2 	9

#define REG0_POS1 10
#define REG0_POS2 11
#define REG1_POS1 12
#define REG1_POS2 13
#define REG2_POS1 14
#define REG2_POS2 15
#define REG3_POS1 16
#define REG3_POS2 17
#define REG4_POS1 18
#define REG4_POS2 19
#define REG5_POS1 20
#define REG5_POS2 21
#define REG6_POS1 22
#define REG6_POS2 23

#define CODE_LEN_POS1 24
#define CODE_LEN_POS2 25

#define REG0_STR_LEN_POS1 26
#define REG0_STR_LEN_POS2 27
#define REG1_STR_LEN_POS1 28
#define REG1_STR_LEN_POS2 29
#define REG2_STR_LEN_POS1 30
#define REG2_STR_LEN_POS2 31

#define REC_LEN_POS1 32
#define REC_LEN_POS2 33

#define FIXED_LEN 35
#define DYNAMIC_START 34

#define SET_MOBILITY_HEADER(packet) (packet[HEADER_POS] = MOBILITY_BYTE)
#define GET_MOBILITY_HEADER(packet) (packet[HEADER_POS])

#define SET_MOBILITY_END(packet, pos) (packet[pos] = MOBILITY_BYTE)
#define GET_MOBILITY_END(packet, pos) (packet[pos])

#define SET_AGENT_ID(packet, id) (packet[ID_POS] = id)
#define GET_AGENT_ID(packet) (packet[ID_POS])

#define SET_AGENT_PRIO(packet, prio) (packet[PRIO_POS] = prio)
#define GET_AGENT_PRIO(packet) (packet[PRIO_POS])

#define SET_AGENT_STATUS(packet, status) (packet[STATUS_POS] = status)
#define GET_AGENT_STATUS(packet) (packet[STATUS_POS])

#define SET_AGENT_FLAG_REG(packet, flag) \
	packet[FLAG_POS1] = ((flag & 0xff000000) >> 24); \
	packet[FLAG_POS2] = ((flag & 0x00ff0000) >> 16);  \
	packet[FLAG_POS3] = ((flag & 0x0000ff00) >> 8);  \
	packet[FLAG_POS4] = (flag & 0x00000000) ;

#define GET_AGENT_FLAG_REG(packet) \
	(packet[FLAG_POS1] << 24) | (packet[FLAG_POS2] << 16) | (packet[FLAG_POS3] << 8) | packet[FLAG_POS4];


#define SET_AGENT_PC(packet, pc) \
		packet[PC_POS1] = ((pc & 0xff00)>>8); \
		packet[PC_POS2] = (pc & 0x00ff);

#define GET_AGENT_PC(packet) \
		(packet[PC_POS1] << 8) | packet[PC_POS2];


#define SET_AGENT_REG(packet, agent, reg) \
		packet[REG##reg##_POS1] = ((agent.regs[reg] & 0xff00) >> 8); \
		packet[REG##reg##_POS2] = (agent.regs[reg] & 0x00ff);

#define GET_AGENT_REG(packet, reg) \
		(packet[REG##reg##_POS1] << 8) | packet[REG##reg##_POS2];

#define SET_AGENT_CODE_LEN(packet, len) \
		packet[CODE_LEN_POS1] = ((len & 0xff00)>>8); \
		packet[CODE_LEN_POS2] = (len & 0x00ff);

#define GET_AGENT_CODE_LEN(packet) \
		(packet[CODE_LEN_POS1] << 8) | packet[CODE_LEN_POS2];


#define SET_AGENT_REG_STR_LEN(packet, agent, reg) \
		packet[REG##reg##_STR_LEN_POS1] = ((agent.regstr_len[reg] & 0xff00) >> 8); \
		packet[REG##reg##_STR_LEN_POS2] = (agent.regstr_len[reg] & 0x00ff);

#define GET_AGENT_REG_STR_LEN(packet, reg) \
		(packet[REG##reg##_STR_LEN_POS1] << 8)| packet[REG##reg##_STR_LEN_POS2];


#define SET_AGENT_REC_MSG_LEN(packet, len) \
		packet[REC_LEN_POS1] = ((len & 0xff00)>>8); \
		packet[REC_LEN_POS2] = (len & 0x00ff);

#define GET_AGENT_REC_MSG_LEN(packet) \
		(packet[REC_LEN_POS1] << 8) | packet[REC_LEN_POS2];


#endif /* COMM_LAYER_H_ */
