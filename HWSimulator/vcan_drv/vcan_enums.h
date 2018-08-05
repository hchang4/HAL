//Common enumerations

#ifndef _VCAN_ENUMS_H_
#define _VCAN_ENUMS_H_

#define MAX_CAN_DATA_BYTES  8

#define MBOX_DIS_ADDR_MASK  0
#define MBOX_EN_ADDR_MASK   1


//Can status structure
typedef struct
{   
  unsigned int init_mode:1;
  unsigned int bus_off:1;
  unsigned int err_passive:1;
  unsigned int err_warning:1;
  unsigned int tx_err_cnt:8;
  unsigned int rx_err_cnt:7;
  unsigned int rx_err_cnt_passive:1;
  unsigned int msg_lost:1;    //This will be set/reset in the driver ioctl
  unsigned int error_code;
} SCANStatus;

typedef enum
{
  MSG_BOX_1 = 1,
  MSG_BOX_2,
  MSG_BOX_3,
  MSG_BOX_4,
  MSG_BOX_5,
  MSG_BOX_6,
  MSG_BOX_7,
  MSG_BOX_8,
  MSG_BOX_9,
  MSG_BOX_10,
  MSG_BOX_11,
  MSG_BOX_12,
  MSG_BOX_13,
  MSG_BOX_14,
  MSG_BOX_15,
  MSG_BOX_16,
  MSG_BOX_17,
  MSG_BOX_18,
  MSG_BOX_19,
  MSG_BOX_20,
  MSG_BOX_21,
  MSG_BOX_22,
  MSG_BOX_23,
  MSG_BOX_24,
  MSG_BOX_25,
  MSG_BOX_26,
  MSG_BOX_27,
  MSG_BOX_28,
  MSG_BOX_29,
  MSG_BOX_30,
  MSG_BOX_31,
  MSG_BOX_32,
  MAX_MSG_BOX = MSG_BOX_32,
} E_MSG_BOX;

typedef enum
{
  MSG_STATE_DIS,
  MSG_STATE_EN,
  MAX_MSG_STATE,
} E_MSG_STATE;

typedef enum
{
  MSG_DIR_TX,
  MSG_DIR_RX,
  MAX_MSG_DIR,
} E_MSG_DIR;

typedef enum
{
  BIT_OFF = 0,
  BIT_ON = 1,
} E_BIT_STATE;

#endif //CANENUMS_H_

