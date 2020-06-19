#include <util/twi.h>
#include <avr/interrupt.h>

#include "I2CSlave.h"

static void (*i2c_slave_recv)(uint8_t);
static void (*i2c_slave_req)(request_t);
static void (*i2c_slave_receive_start)(void);
static void (*i2c_slave_receive_stop)(void);


void i2c_slave_setCallbacks(void (*start)(void),
                            void (*recv)(uint8_t),
                            void (*stop)(void),
                            void (*req)(request_t))
{
  i2c_slave_recv = recv;
  i2c_slave_req = req;
  i2c_slave_receive_start = start;
  i2c_slave_receive_stop = stop;
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
    case TW_SR_SLA_ACK:
      if (i2c_slave_receive_start) i2c_slave_receive_start();
      break;
    case TW_SR_STOP:
      if (i2c_slave_receive_stop) i2c_slave_receive_stop();
      break;
    case TW_SR_DATA_ACK: // data received, ACK returned
      // received data from master, call the receive callback
      if (i2c_slave_recv) i2c_slave_recv(TWDR);
      break;
    case TW_ST_SLA_ACK: // SLA+R
      // master is requesting data, call the request callback
      if (i2c_slave_req) i2c_slave_req(INITIAL);
      break;
    case TW_ST_DATA_ACK: // data transmitted, ACK received
      // master is requesting data, call the request callback
      if (i2c_slave_req) i2c_slave_req(CONTINUATION);
      break;
    case TW_ST_DATA_NACK: // last data byte transmitted, (n)ACK received
      if (i2c_slave_req) i2c_slave_req(DONE);
      break;
    case TW_BUS_ERROR:
      // some sort of erroneous state, prepare TWI to be readdressed
      TWCR = 0;
      // TODO : is this enough ?
      break;
    default:
      break;
  }
  TWCR = (1<<TWIE) | (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
} 
