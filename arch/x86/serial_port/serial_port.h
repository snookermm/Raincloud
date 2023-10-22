#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_

#include <types.h>

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
int serial_port_init(void);

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
uint64_t serial_port_write(uint8_t *buffer, uint64_t number_of_bytes);

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
uint64_t serial_port_read(uint8_t *buffer, uint64_t number_of_bytes);

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
int serial_port_poll(void);

#endif
