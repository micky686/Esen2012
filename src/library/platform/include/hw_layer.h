/*
* hw_layer.h
*
* Created on: Dec 9, 2012
* Author: igor
*/

#ifndef HW_LAYER_H_
#define HW_LAYER_H_

struct block_dev_access_data{
uint8_t id;
struct block_dev_acess_data* next;
};

typedef struct block_dev_access_data block_dev_access_data_t;

typedef struct block_dev_access_list{
uint8_t size;
block_dev_access_data_t* first;
block_dev_access_data_t* last;
} block_dev_access_list_t;

#endif /* HW_LAYER_H_ */