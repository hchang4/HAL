#ifndef _VCAN_DRV_H
#define _VCAN_DRV_H_

#include <linux/cdev.h>
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

#include "vcan_enums.h"

//IMPORTANT: If this value is changed, do not forget to update the 
//  initialization of the vcan_device data structure in the .c file
#define NR_VCAN_MODULES   7

#define VCAN_MAJOR    91
#define VCAN_MINOR    0

//Indicates the max. number of packets to store
#define DATA_BUFFER_QUEUE_SIZE    1500//256

//Default message boxes
#define TX_MSG_BOX      MSG_BOX_1
#define RX_MSG_BOX      MSG_BOX_2

// Use 'z' as magic number 
//  (corresponds to CAN bus card in Documentation/ioctl-number.txt)
#define VCAN_IOC_MAGIC  'z'


/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 */

//Receive mailbox configuration related ioctls. 
//IMPORTANT: Changing any of the settings below does not actually
// change the configuration on the chip. After setting up everything,
// use the ML9620_IOC_T_RX_CONF ioctl to actually put the configuration
// on to the chip.
#define VCAN_IOC_S_RXMBOX1      _IOW(VCAN_IOC_MAGIC, 0, int)
#define VCAN_IOC_S_RXMBOX2      _IOW(VCAN_IOC_MAGIC, 1, int)
#define VCAN_IOC_S_RXMASK       _IOW(VCAN_IOC_MAGIC, 2, int)
#define VCAN_IOC_S_RXADDR       _IOW(VCAN_IOC_MAGIC, 3, int)
#define VCAN_IOC_T_RX_MASK_EN   _IO(VCAN_IOC_MAGIC, 4)
#define VCAN_IOC_T_RX_MASK_DIS  _IO(VCAN_IOC_MAGIC, 5)

//Invoke this to put the RX configuration on to the chip.
#define VCAN_IOC_T_RX_CONF    _IO(VCAN_IOC_MAGIC, 6)

//Baud rate
#define VCAN_IOC_S_CAN_BAUD   _IOW(VCAN_IOC_MAGIC, 7, int)

//Chip modes
#define VCAN_IOC_T_CAN_INIT   _IO(VCAN_IOC_MAGIC, 8)
#define VCAN_IOC_T_CAN_NORM   _IO(VCAN_IOC_MAGIC, 9)

//Get the last chip status information
#define VCAN_IOC_G_CAN_STAT     _IOR(VCAN_IOC_MAGIC, 10, SCANStatus)
#define VCAN_IOC_T_CLR_MSG_LOST _IO(VCAN_IOC_MAGIC, 11)

#define VCAN_IOC_MAXNR      11  //UPDATE THIS based on ioctls above

#endif //_VCAN_DRV_H_

