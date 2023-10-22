#include "serial_port.h"
#include <types.h>
#include <io.h>
//---------------------------------------------
// UART Register Offsets
//---------------------------------------------
#define BAUD_LOW_OFFSET         0x00
#define BAUD_HIGH_OFFSET        0x01
#define IER_OFFSET              0x01
#define LCR_SHADOW_OFFSET       0x01
#define FCR_SHADOW_OFFSET       0x02
#define IR_CONTROL_OFFSET       0x02
#define FCR_OFFSET              0x02
#define EIR_OFFSET              0x02
#define BSR_OFFSET              0x03
#define LCR_OFFSET              0x03
#define MCR_OFFSET              0x04
#define LSR_OFFSET              0x05
#define MSR_OFFSET              0x06

//---------------------------------------------
// UART Register Bit Defines
//---------------------------------------------
#define LSR_TXRDY               0x20
#define LSR_RXDA                0x01
#define DLAB                    0x01

//---------------------------------------------
// UART Settings
//---------------------------------------------
uint16_t  gUartBase = 0x3F8;
uint32_t  gBps      = 115200;
uint8_t   gData     = 8;
uint8_t   gStop     = 1;
uint8_t   gParity   = 0;
uint8_t   gBreakSet = 0;
/**
  *Initialize the serial device hardware.
  *
  *If no initialization is required, then return RETURN_SUCCESS.
  *If the serial device was successfully initialized, then return RETURN_SUCCESS.
  *If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.
  *
  *@retval 0 : The serial device was initialized.
  *       -1 : The serial device failed to initialize.
  * 
  */
int serial_port_init(void)
{
  uint64_t Divisor;
  uint8_t  OutputData;
  uint8_t  Data;

  //
  // Map 5..8 to 0..3
  //
  Data = (uint8_t) (gData - (uint8_t) 5);

  //
  // Calculate divisor for baud generator
  //
  Divisor = 115200 / gBps;
  
  //
  // Set communications format
  //
  OutputData = (uint8_t) ((DLAB << 7) | (gBreakSet << 6) | (gParity << 3) | (gStop << 2) | Data);
  __io_write_8 ((uint64_t) (gUartBase + LCR_OFFSET), OutputData);

  //
  // Configure baud rate
  //
  __io_write_8 ((uint64_t) (gUartBase + BAUD_HIGH_OFFSET), (uint8_t) (Divisor >> 8));
  __io_write_8 ((uint64_t) (gUartBase + BAUD_LOW_OFFSET), (uint8_t) (Divisor & 0xff));

  //
  // Switch back to bank 0
  //
  OutputData = (uint8_t) ((~DLAB << 7) | (gBreakSet << 6) | (gParity << 3) | (gStop << 2) | Data);
  __io_write_8 ((uint64_t) (gUartBase + LCR_OFFSET), OutputData);

  return 0;
}

/**
  *Write data from buffer to serial device. 
  *
  *Writes NumberOfBytes data bytes from Buffer to the serial device.  
  *The number of bytes actually written to the serial device is returned.
  *If the return value is less than NumberOfBytes, then the write operation failed.
  *If Buffer is NULL, then ASSERT(). 
  *If NumberOfBytes is zero, then return 0.
  *
  *@param  Buffer           Pointer to the data buffer to be written.
  *@param  NumberOfBytes    Number of bytes to written to the serial device.
  *
  *@retval 0                NumberOfBytes is 0.
  *@retval >0               The number of bytes written to the serial device.  
  *                         If this value is less than NumberOfBytes, then the read operation failed.
  * 
  */
uint64_t serial_port_write(uint8_t *buffer, uint64_t number_of_bytes)
{
  uint64_t  Result;
  uint8_t  Data;

  if (buffer == NULL) {
    return 0;
  }

  Result = number_of_bytes;

  while ((number_of_bytes--) != 0) {
    //
    // Wait for the serail port to be ready.
    //
    do {
      Data = __io_read_8 ((uint16_t) gUartBase + LSR_OFFSET);
    } while ((Data & LSR_TXRDY) == 0);
    __io_write_8 ((uint16_t) gUartBase, *buffer++);
  }

  return Result;
}


/**
  *Read data from serial device and save the datas in buffer.
  *
  *Reads NumberOfBytes data bytes from a serial device into the buffer
  *specified by Buffer. The number of bytes actually read is returned. 
  *If the return value is less than NumberOfBytes, then the rest operation failed.
  *If Buffer is NULL, then ASSERT(). 
  *If NumberOfBytes is zero, then return 0.
  *
  *@param  Buffer           Pointer to the data buffer to store the data read from the serial device.
  *@param  NumberOfBytes    Number of bytes which will be read.
  *
  *@retval 0                Read data failed, no data is to be read.
  *@retval >0               Actual number of bytes read from serial device.
  * 
  */
uint64_t serial_port_read(uint8_t *buffer, uint64_t number_of_bytes)
{
  uint64_t  Result;
  uint8_t  Data;

  if (NULL == buffer) {
    return 0;
  }

  Result = number_of_bytes;

  while ((number_of_bytes--) != 0) {
    //
    // Wait for the serail port to be ready.
    //
    do {
      Data = __io_read_8 ((uint16_t) gUartBase + LSR_OFFSET);
    } while ((Data & LSR_RXDA) == 0);

    *buffer++ = __io_read_8 ((uint16_t) gUartBase);
  }

  return Result;
}

/**
  *Polls a serial device to see if there is any data waiting to be read.
  *
  *Polls a serial device to see if there is any data waiting to be read.
  *If there is data waiting to be read from the serial device, then TRUE is returned.
  *If there is no data waiting to be read from the serial device, then FALSE is returned.
  *
  *@retval TRUE             Data is waiting to be read from the serial device.
  *@retval FALSE            There is no data waiting to be read from the serial device.
  * 
  */
int serial_port_poll(void)
{
  uint8_t  Data;

  //
  // Read the serial port status.
  //
  Data = __io_read_8 ((uint16_t) gUartBase + LSR_OFFSET);

  return (uint8_t) ((Data & LSR_RXDA) != 0);
}


