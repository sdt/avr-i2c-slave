/* I2C Echo Example */
#include "I2CSlave.h"
#include <stdlib.h>

#define I2C_ADDR 0x10

volatile uint8_t data;

void I2C_received(uint8_t received_data)
{
  data = received_data;
}

void I2C_requested()
{
  i2c_slave_transmitByte(data);
}

void setup()
{
  // set received/requested callbacks
  i2c_slave_setCallbacks(NULL, I2C_received, /*NULL,*/ I2C_requested);

  // init I2C
  i2c_slave_init(I2C_ADDR);
}

int main()
{
  setup();

  // Main program loop
  while(1) {
      //ping 0x20
      i2c_master_init(0x20, MasterTransmit);
      i2c_master_done();
  }
}
