#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <stdint.h>


// used as argument to call request callback :
typedef enum {
    INITIAL = 1,      // when master open communication for reading
    CONTINUATION = 2, // when master ACKs transmitted data (it expects continuation)
    DONE = 3          // when masters (n)ACKs last transmitted data (NB : should be a nACK to indicate no data longer expected)
} request_t;

void i2c_slave_init(uint8_t address);
void i2c_slave_stop(void);
void i2c_slave_setCallbacks(void (*recv)(uint8_t), void (*req)(request_t));

// last should be 0 in normal operation, execpt for the LAST byte transmitted by the slave
inline void __attribute__((always_inline)) i2c_slave_transmitByte(uint8_t data/*, uint8_t last*/)
{
//     if (last) {
//         TWCR &= ~(1 << TWEA);
//     }
    TWDR = data;
}

ISR(TWI_vect);

#endif
