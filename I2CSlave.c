#include <util/twi.h>
#include <avr/interrupt.h>

#include "I2CSlave.h"

static void (*i2c_slave_recv)(uint8_t);
static void (*i2c_slave_req)(request_t);

void i2c_slave_setCallbacks(void (*recv)(uint8_t), void (*req)(request_t))
{
  i2c_slave_recv = recv;
  i2c_slave_req = req;
}

void i2c_slave_init(uint8_t address)
{
  cli();
  // load address into TWI address register
  TWAR = address << 1;
  // set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt
  TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN);
  sei();
}

void i2c_slave_stop(void)
{
  // clear acknowledge and enable bits
  cli();
  TWCR = 0;
  TWAR = 0;
  sei();
}

ISR(TWI_vect)
{
  switch(TW_STATUS)
  {
    case TW_SR_DATA_ACK: // data received, ACK returned
      // received data from master, call the receive callback
      i2c_slave_recv(TWDR);
      break;
    case TW_ST_SLA_ACK: // SLA+R
      // master is requesting data, call the request callback
      i2c_slave_req(INITIAL);
      break;
    case TW_ST_DATA_ACK: // data transmitted, ACK received
      // master is requesting data, call the request callback
      i2c_slave_req(CONTINUATION);
      break;
    /*case TW_ST_LAST_DATA: // last data byte transmitted, (n)ACK received
      i2c_slave_req(DONE);
      break;*/
    case TW_BUS_ERROR:
      // some sort of erroneous state, prepare TWI to be readdressed
      TWCR = 0;
      break;
    default:
      break;
  }
  TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
} 
