#include <Arduino.h>

/* ====================================================== */
/* kontrolsumma no dokumentacijas                         */
/* ====================================================== */

typedef unsigned long BYTE;

BYTE CalculateCRC(uint8_t *lpBuffer, int nSize)
{
BYTE shift_reg, sr_lsb, data_bit, v;
BYTE fb_bit;
int i, j;

  shift_reg = 0; // initialize the shift register

  for(i=0; i<nSize; i++) {
    v = (BYTE)(lpBuffer[i] & 0x0000FFFF);
    for (j=0; j<8 ; j++) { // for each bit
      data_bit = v & 0x01; // isolate least sign bit
      sr_lsb = shift_reg & 0x01;
      fb_bit = (data_bit ^ sr_lsb) & 0x01; // calculate the feed back bit
      shift_reg = shift_reg >> 1;
      if( fb_bit==1 ) shift_reg = shift_reg ^ 0x8C;
      v = v >> 1;
    }
  }

  return shift_reg;
}

/* ====================================================== */
/* DID = 5 zinojums                                       */
/* ====================================================== */

struct msg_did5_s {
  uint8_t stxa;
  uint8_t stxb;
  uint8_t rid;
  uint8_t rez;
  uint8_t did;
  uint8_t length;
  uint8_t pwm_ch;
  uint8_t pwm_low;
  uint8_t pwm_high;
  uint8_t chsum;
  uint8_t exta;
  uint8_t extb;
} PACKED;

typedef struct msg_did5_s msg_did5_t;

void  msg_did5_encode(char *buf, int channel, unsigned int pwm)
{
msg_did5_t *msg = (msg_did5_t*)buf;

  msg->stxa = 94;
  msg->stxb = 2;
  msg->rid = 1;
  msg->rez = 0;
  msg->did = 5;
  msg->length = 3;
  msg->pwm_ch = channel;
  msg->pwm_low = pwm % 256;
  msg->pwm_high = pwm / 256;
  msg->chsum = CalculateCRC(&(msg->rid), 7);
  msg->exta = 94;
  msg->extb = 13;
}

void msg_did5_print( char *buf )
{
msg_did5_t *msg = (msg_did5_t*)buf;
unsigned pwm = msg->pwm_low+256*msg->pwm_high;

  Serial.print("STX: "); Serial.print(msg->stxa); Serial.print(" "); Serial.println(msg->stxb);
  Serial.print("RID: "); Serial.println(msg->rid);
  Serial.print("Reserved: "); Serial.println(msg->rez);
  Serial.print("DID: "); Serial.println(msg->did);
  Serial.print("Length: "); Serial.println(msg->length);
  Serial.print("Data: "); Serial.print(msg->pwm_ch); Serial.print(" "); 
  Serial.print(msg->pwm_low), Serial.print(" "); Serial.print(msg->pwm_high);
  Serial.print(" => ch="); Serial.print(msg->pwm_ch); Serial.print(" pwm="); Serial.println(pwm);
  Serial.print("Checksum: "); Serial.println(msg->chsum);
  Serial.print("EXT: "); Serial.print(msg->exta); Serial.print(" "); Serial.println(msg->extb);
}

void msg_did5_println( char *buf )
{
msg_did5_t *msg = (msg_did5_t*)buf;
unsigned pwm = msg->pwm_low+256*msg->pwm_high;

  Serial.print(msg->stxa); Serial.print(" "); Serial.print(msg->stxb);
  Serial.print(" "); Serial.print(msg->rid);
  Serial.print(" "); Serial.print(msg->rez);
  Serial.print(" "); Serial.print(msg->did);
  Serial.print(" "); Serial.print(msg->length);
  Serial.print(" "); Serial.print(msg->pwm_ch); Serial.print(" "); 
  Serial.print(msg->pwm_low), Serial.print(" "); Serial.print(msg->pwm_high);
  Serial.print(" "); Serial.print(msg->chsum);
  Serial.print(" "); Serial.print(msg->exta); Serial.print(" "); Serial.println(msg->extb);
}

int msg_did5_test( void )
{
char ex1113[] = {94,2,1,0,5,3,4,160,15,86,94,13};
char buff[20];

  msg_did5_print( ex1113 );

  msg_did5_encode(buff, 4, 4000);
  msg_did5_print( buff );

  return 0;
}
