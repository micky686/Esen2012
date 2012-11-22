/*
  Communication interface idea

 */


#define MAX_PAYLOAD 255

typedef enum comm_msg_id 
  {COMM_REQ_MOVE_SERVICE, COMM_ACK_MOVE_SERVICE, COMM_MSG_CONTROL_DATA}
  comm_msg_id_t;


struct comm_msg_S {  
  comm_msg_id_t id;
  int dest_node;
  int dest_board;
  long int payload_len;
  char payload[MAX_PAYLOAD];
  int crc;
} comm_msg_T;

/* */
int comm_send(*comm_msg_T msg, int (callback) ) { 
  if(busy) {
    return !0
  }
  else
    // send
    return 0;
}


int comm_busy() {
  if(busy) {
    return !0
  }
  else
    // send
    return 0;
  
}

int state;

int main_agent() {


  switch (state) {

  case state:
    ///do euoue
    break;
    //...

  }

  if (! comm_busy()){

    comm_send(*comm_msg_T msg, agent_notifier);
    state = STATE_SENT;
  }



}


int agent_notifier() {

  state = STATE_ACK;
}
