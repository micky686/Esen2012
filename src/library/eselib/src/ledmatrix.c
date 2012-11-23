/**
 * lib: ledmatrix
 *
 * this library contains the dot matrix display.
 * 
 * matnr: 0201623
 * date: 16.1.2012
 * @author kyrill winkler <kyrill.winkler@student.tuwien.ac.at>
 */

#include <avr/interrupt.h>
//#include <avr/signal.h>

#include "ledmatrix.h"
#include "global.h"


/* ====================================================================== */
/* G L O B A L  V A R I A B L E S                                         */
/* ====================================================================== */

static char scroll_buffer[SCROLL_BUFFER_LEN] = "      ";
static uint8_t scroll_enable;

static uint8_t send_buffer[7][4] = {  {0x00, 0x00, 0x00, 0x00},
                                      {0x00, 0x00, 0x00, 0x00},
                                      {0x00, 0x00, 0x00, 0x00},
                                      {0x00, 0x00, 0x00, 0x00},
                                      {0x00, 0x00, 0x00, 0x00},
                                      {0x00, 0x00, 0x00, 0x00},
                                      {0x00, 0x00, 0x00, 0x00} };
#if 0
/*
 * es muss ein Stiegl sein...
 */
static uint8_t send_buffer[7][4] = {  {0xf0, 0x00, 0x00, 0x00},
                                      {0x0f, 0x00, 0x00, 0x00},
                                      {0x00, 0xf0, 0x00, 0x00},
                                      {0x00, 0x0f, 0x00, 0x00},
                                      {0x00, 0x00, 0xf0, 0x00},
                                      {0x00, 0x00, 0x0f, 0x00},
                                      {0x00, 0x00, 0x00, 0xf0} };
#endif
static const uint8_t char_0[7] = {0xf0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xf0};
static const uint8_t char_1[7] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
static const uint8_t char_2[7] = {0xf0, 0x10, 0x10, 0xf0, 0x80, 0x80, 0xf0};
static const uint8_t char_3[7] = {0xf0, 0x10, 0x10, 0xf0, 0x10, 0x10, 0xf0};
static const uint8_t char_4[7] = {0x80, 0x80, 0xa0, 0xf0, 0x20, 0x20, 0x20};
static const uint8_t char_5[7] = {0xf0, 0x80, 0x80, 0xf0, 0x10, 0x10, 0xf0};
static const uint8_t char_6[7] = {0xf0, 0x80, 0x80, 0xf0, 0x90, 0x90, 0xf0};
static const uint8_t char_7[7] = {0xf0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
static const uint8_t char_8[7] = {0xf0, 0x90, 0x90, 0xf0, 0x90, 0x90, 0xf0};
static const uint8_t char_9[7] = {0xf0, 0x90, 0x90, 0xf0, 0x10, 0x10, 0xf0};

static const uint8_t char_A[7] = {0xf0, 0x90, 0x90, 0xf0, 0x90, 0x90, 0x90};
static const uint8_t char_B[7] = {0xe0, 0x90, 0x90, 0xf0, 0x90, 0x90, 0xe0};
static const uint8_t char_C[7] = {0xf0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf0};
static const uint8_t char_D[7] = {0xe0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xe0};
static const uint8_t char_E[7] = {0xf0, 0x80, 0x80, 0xf0, 0x80, 0x80, 0xf0};
static const uint8_t char_F[7] = {0xf0, 0x80, 0x80, 0xf0, 0x80, 0x80, 0x80};
static const uint8_t char_G[7] = {0xf0, 0x80, 0x80, 0x80, 0xb0, 0x90, 0xf0};
static const uint8_t char_H[7] = {0x90, 0x90, 0x90, 0xf0, 0x90, 0x90, 0x90};
static const uint8_t char_I[7] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
static const uint8_t char_J[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_K[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_L[7] = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf0};
static const uint8_t char_M[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_N[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_O[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_P[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_Q[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_R[7] = {0xf0, 0x90, 0x90, 0xf0, 0xc0, 0xa0, 0x90};
static const uint8_t char_S[7] = {0xf0, 0x80, 0x80, 0xf0, 0x10, 0x10, 0xf0};
static const uint8_t char_T[7] = {0xf0, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};
static const uint8_t char_U[7] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xf0};
static const uint8_t char_V[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_W[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_X[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_Y[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_Z[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t char_empty[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const uint8_t char_degree_c[7] = {0xc0, 0xc0, 0x30, 0x20, 0x20, 0x20, 0x30};


static const uint8_t *char_lut[36] =
  {char_0, char_1, char_2, char_3, char_4, char_5, char_6, char_7, char_8, char_9,
   char_A, char_B, char_C, char_D, char_E, char_F, char_G, char_H, char_I, char_J,
   char_K, char_L, char_M, char_N, char_O, char_P, char_Q, char_R, char_S, char_T,
   char_U, char_V, char_W, char_X, char_Y, char_Z
   };

/* ====================================================================== */
/* I N T E R R U P T S                                                    */
/* ====================================================================== */
/**
 * The timer 0 OC interrupt is used to multiplex access to the different rows of the display.
 * Moreover, if scrolling is enabled, it controls the advancing of the scrolling text.
 *
 **/
SIGNAL(SIG_OUTPUT_COMPARE0) {
  static volatile uint8_t i;
  static volatile uint16_t scrolling_prescale_cnt;
  static volatile char* scroll_pos = scroll_buffer;
  spi_MasterTransmit(send_buffer[6-i][1]);
  spi_MasterTransmit(send_buffer[6-i][0]);
  spi_MasterTransmit(send_buffer[6-i][3]);
  spi_MasterTransmit(send_buffer[6-i][2]);
  PORTG &= ~(1<<PG2); /* slave select low */
  PORTG |= (1<<PG2);
  ROWSEL_PORT |= (i & 0x07);
  ROWSEL_PORT &= (i | (0xf8));

  i = (i<7)? (i+1) : 0;
  
  if (scroll_enable != 0) {
    scrolling_prescale_cnt++;
    if (scrolling_prescale_cnt > SCROLLING_SPEED) {
      scrolling_prescale_cnt = 0;
      dotmatrix_send((char *) scroll_pos);
      
      if (*scroll_pos != '\0') {
        scroll_pos++;
      }
      else {
        scroll_pos = scroll_buffer;
      }
    }
  }
}




/* ====================================================================== */
/* F U N C T I O N S                                                      */
/* ====================================================================== */

/**
 * Write a character, encoded for the display, to the correct position of the send buffer.
 *
 * @param chr The character code that will be written to the buffer.
 * @param pos The position of the character.
 **/
void dotmatrix_write_char(const uint8_t *chr, uint8_t pos); /**
 * Perform character table lookup
 *
 * @param chr The character for which the lookup will be performed.
 * @return: character encoded for the display from the lut, if known, code for empty char otherwise
 **/
const uint8_t *dotmatrix_get_char(uint8_t chr);

void init_dotmatrix() {
  /* Dotmatrix controller and SPI configuration */
  ROWSEL_DDR |= ((1<<ROW_EN) | (1<<SELECT0) | (1<<SELECT1) | (1<<SELECT2));
  DDRB |= (1<<BLANK);
  
  PORTB &= ~(1<<BLANK);
  ROWSEL_PORT &= ~(1<<ROW_EN);
  
  PORTG &= ~(1<<PG2); /* slave select low */
  spi_MasterInit();

  /* Timer configuration */
  OCR0 = 50;
  // prescaler = 1024 ctc t/c mode, OC0 disconnected
  TCCR0 |= ((1<<WGM01) |(1<<CS02) | (1<<CS01) );
  TCCR0 &= ~( (1<<WGM00) | (1<<COM01) | (1<<COM00)| (1<<CS00)) ;
  
  // enable OC interrupts
  TIMSK |= (1<<OCIE0);
  sei();
}

void dotmatrix_send(char *data) {
  uint8_t i = 0;
  while (*data != '\0' && i < 6) {
    dotmatrix_write_char(dotmatrix_get_char(*data), i);
    data++;
    i++;
  }
  while(i<6) {
    dotmatrix_write_char(char_empty, i);
    i++;
  }
}

void dotmatrix_write_char(const uint8_t *chr, uint8_t pos) {
  uint8_t i;
  for(i = 0; i<7; i++) {
    switch (pos) {
      case 0:
        send_buffer[i][0] |= (chr[i]>>1 & 0xff<<3);
        send_buffer[i][0] &= (chr[i]>>1 | ~(0xff<<3));
        break;
      case 1:
        send_buffer[i][0] |= ((chr[i]>>6 & 0xff>>6));
        send_buffer[i][0] &= ((chr[i]>>6 | ~(0xff>>6)));
        // next two lines switched, jtag would hangup else >/
        send_buffer[i][1] &= ((chr[i]<<2 | ~(0xd0)));
        send_buffer[i][1] |= ((chr[i]<<2 & 0xd0 ));
        break;
      case 2: 
        send_buffer[i][1] |= ((chr[i]>>3 & 0x1e ));
        send_buffer[i][1] &= ((chr[i]>>3 | ~(0x1e) ));
        break;
      case 3:
        send_buffer[i][2] |= (chr[i]>>1 & 0xff<<3);
        send_buffer[i][2] &= (chr[i]>>1 | ~(0xff<<3));
        break;
      case 4:
        send_buffer[i][2] |= ((chr[i]>>6 & 0xff>>6));
        send_buffer[i][2] &= ((chr[i]>>6 | ~(0xff>>6)));
        send_buffer[i][3] &= ((chr[i]<<2 | ~(0xd0)));
        send_buffer[i][3] |= ((chr[i]<<2 & 0xd0 ));
        break;
      case 5:
        send_buffer[i][3] |= ((chr[i]>>3 & 0x1e ));
        send_buffer[i][3] &= ((chr[i]>>3 | ~(0x1e) ));
        break;
      default:
        break;
    }
  }
}

void dotmatrix_send_char(uint8_t chr, uint8_t pos) {
  dotmatrix_write_char(dotmatrix_get_char(chr), pos);
}

const uint8_t *dotmatrix_get_char(uint8_t chr) {
  const uint8_t *retval;
  if (chr >= '0' && chr <= '9') { /* is digit? */
    retval = char_lut[chr - '0'];
  }
  else if (chr >= 'A' && chr <= 'Z') { /* is caps char? */
    retval = char_lut[chr - 'A' + 10];
  }
  else {
    retval = char_empty;
  }
  return retval;
}

void dotmatrix_send_threedigits(uint16_t integer, uint8_t pos) {
  dotmatrix_write_char(char_lut[(integer/100) % 10], pos);
  dotmatrix_write_char(char_lut[(integer/10) % 10], pos+1);
  dotmatrix_write_char(char_lut[integer % 10], pos+2);
}

void dotmatrix_send_int(uint16_t integer, uint8_t pos) {
  if ((integer / 100) > 0) { 
    dotmatrix_write_char(char_lut[(integer/100) % 10], pos);
    dotmatrix_write_char(char_lut[(integer/10) % 10], pos+1);
    dotmatrix_write_char(char_lut[integer % 10], pos+2);
  }
  else if ((integer / 10) > 0) { 
    dotmatrix_write_char(char_lut[(integer/10) % 10], pos);
    dotmatrix_write_char(char_lut[integer % 10], pos+1);
  }
  else {
    dotmatrix_write_char(char_lut[integer % 10], pos);
  }
}

void dotmatrix_send_comma(uint8_t pos) {
  /*
  if (pos < 6) {
    send_buffer[7][((pos+1) * 5 )/8] |= 1<<(8-(((pos+1) * 5 + 1 ) % 8));
  }*/
  switch (pos) {
    case 0:
      send_buffer[6][0] |= 0x04;
      break;
    case 1:
      send_buffer[6][1] |= 0x20;
      break;
    case 2:
      send_buffer[6][1] |= 0x01;
      break;
    case 3:
      send_buffer[6][2] |= 0x04;
      break;
    case 4:
      send_buffer[6][3] |= 0x20;
      break;
    case 5:
      send_buffer[6][3] |= 0x01;
      break;
    default:
      break;

  }
}

void dotmatrix_send_degree(uint8_t pos) {
  dotmatrix_write_char(char_degree_c, pos);
  dotmatrix_write_char(char_degree_c, pos);
}

void dotmatrix_send_scrolling_text(char* string) {
  uint8_t i, j;
  for (i=6; i<SCROLL_BUFFER_LEN && *string != '\0'; i++) {
    scroll_buffer[i] = *string;
    string++;
  }
  if (i >= SCROLL_BUFFER_LEN) {
    scroll_buffer[SCROLL_BUFFER_LEN - 1] = '\0';
  }
  else {
    scroll_buffer[i] = '\0';
  }
}

void dotmatrix_enable_scrolling() {
  scroll_enable = 1;
}


void dotmatrix_disable_scrolling() { 
  scroll_enable = 0;
}

void dotmatrix_clear() {
  uint8_t i, j;
  for (i = 0; i<7; i++) {
    for (j=0; j<4; j++) {
      send_buffer[i][j] = 0;
    }
  }
}
