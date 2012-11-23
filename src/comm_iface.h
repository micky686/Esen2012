/*
  Communication interface idea

 */

#define MAX_PAYLOAD 255

typedef enum comm_msg_id 
  {COMM_MOVE_SERVICE, COMM_DATA_MSG}
  comm_msg_id_t;

/* Structure for sharing messages */
typedef struct comm_msg_S {
  int type;         /* Message type identifier */
  int node;         /* Destination or origin node depending on context */
  int board;        /* Destination or origin board depending on context */
  long int len;     /* Payload length */
  char *payload;    /* Payload data*/  
} comm_msg_T;

/* Internal structure for messages */
typedef struct comm_int_msg_S_ {
  long int msg_len;          /* Length of the whole message struct */
  int msg_id;                /* Unique message identifier */
  int msg_type;              /* Message type identifier */
  int dest_node;             /* Destination node */
  int dest_board;            /* Destination board */
  int orig_node;             /* Origin node */
  int orig_board;            /* Origin board */
  long int payload_len;      /* Payload length  */
  char payload[MAX_PAYLOAD]; /* Payload data */
  int crc;                   /* CRC integrity signature  */
} comm_int_msg_T_;


/** 
   Function: comm_send_msg_ - Sends message over communication interface
   Parameters: msg - Pointer to message structure; will be copied by function
               sent_cb - Callback that is called when the message has been sent
   Returns: Zero if message accepted for sending or nonzero if rejected

*/
int comm_send_msg(comm_msg_T *msg, void (*sent_cb)(int)); 
//int _comm_send_msg_(comm_msg_T *msg, void (*callback)(int)); 

/**
   Function: comm_set_recvr_ - Sets a callback to process incoming messages
   Parameters: recvr_callback - Callback function that processes received message
   Returns: Zero if successful		  
 */
int comm_set_recvr_(void (*recvr_callback)(comm_msg_T *msg));


/**
    Function: comm_register - Registers a node to the communication system.
    Returns: Zero if successful
    Parameters: board - Board id
                node - Node id inside a board
 */
int comm_register(int board, int node);

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
