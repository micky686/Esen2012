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



#endif /* COMM_LAYER_H_ */
