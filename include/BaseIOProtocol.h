/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: BaseIOProtocol.h
 * *
 * *  Description: Base IO protocol code & data structures
 * *
 * *  Copyright:        Copyright (c) 2011-2012, 
 * *                    Rosemount Analytical 
 * *                    All Rights Reserved.
 * *
 * *  Operating System:  None.
 * *  Language:          'C++'
 * *  Target:            Gas Chromatograph Model GC700XP
 * *
 * *  Revision History:
 * *  $Id: BaseIOProtocol.h,v 1.12 2015/04/30 09:32:30 EMRSN\Dkad Exp $
 * *  $Log: BaseIOProtocol.h,v $
 * *  Revision 1.12  2015/04/30 09:32:30  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.11  2014/11/20 23:20:07  emrsn\rrthomas
 * *  Added enums/union memebers for supporting Base IO G2 features (Loop Power Selection, Serial Driver Selection).
 * *
 * *  Revision 1.10  2014/06/26 10:01:09  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.9  2013/10/08 15:09:48  emrsn\rrthomas
 * *  EPC Pressure Sensor Temperature correction made configurable using MON2020. Here is the correction formula (implemented in Analyzer IO firmware) -
 * *
 * *  Corrected_Carrier_Pressure = Carrier_Pressure  (Ambient_temperature  base_temperature) * pressure_correction
 * *
 * *  "Ambient_temperature" is read from RTD2 which is mounted on the IMB Board in close proximity to the Pressure Sensor.
 * *  "base_temperature" is available in EPC_CONFIG table.
 * *  "pressure_correction" is available in EPC_CONFIG table.
 * *
 * *  Revision 1.8  2012/03/21 06:49:03  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.7  2011/06/01 08:19:14  emrsn\rsaha
 * *  Extended HAL EPC interfaces for 370XA.
 * *
 * *  Revision 1.6  2009/12/17 14:06:23  emrsn\rsaha
 * *  Added support for FPD in BaseIO card.
 * *
 * *  Revision 1.5  2009/04/15 13:24:15  epm-na\rsaha
 * *  Revised with support for FID Ignition and Gain control.
 * *
 * *  Revision 1.4  2007/04/05 13:14:51  epm-na\rthomas
 * *  LTLOI: Added HAL function to access the LTLOI interface on the base IO
 * *  board.
 * *
 * *  Revision 1.3  2006/12/13 22:21:14  epm-na\DPrabhakar
 * *  Code clean up
 * *    - Removed un-used enum in BaseIOProtocol.h
 * *
 * *  Revision 1.2  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



#ifndef _BASEIO_PROTOCOL_H
#define _BASEIO_PROTOCOL_H

#include "DevProtocol.h"

#define MAX_SERIALIO_PKT_LEN  6


/************************************************************************************/
// Common structures
/************************************************************************************/
//Serial IO baud rate setting
typedef enum {
  MIN_BAUD_RATE,
  BAUD_110 = MIN_BAUD_RATE,
  BAUD_300,
  BAUD_600,
  BAUD_1200,
  BAUD_2400,
  BAUD_4800,
  BAUD_9600,
  BAUD_14400,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200,
  MAX_BAUD_RATE,
} BASEIO_SER_BAUD_ENUM;

typedef enum {
  MIN_SER_DATA_LEN,
  DATA_LEN_5_BITS = MIN_SER_DATA_LEN,
  DATA_LEN_6_BITS,
  DATA_LEN_7_BITS,
  DATA_LEN_8_BITS,
  MAX_SER_DATA_LEN,
} BASEIO_SER_DATA_LEN_ENUM;

//Serial IO parity configuration
typedef enum {
  MIN_PARITY_SETTING,
  PARITY_EVEN = MIN_PARITY_SETTING,
  PARITY_ODD,
  PARITY_SPACE,
  PARITY_MARK,
  PARITY_NONE,
  PARITY_MULTIDROP,
  MAX_PARITY_SETTING,
} BASEIO_SER_PARITY_ENUM;

//Serial IO stop bit configuration
typedef enum {
  MIN_STOP_BITS,
  STOP_BIT_1 = MIN_STOP_BITS,
  STOP_BIT_1_5,
  STOP_BIT_2,
  MAX_STOP_BITS,
} BASEIO_SER_STOP_BITS_ENUM;

//Serial IO - port mode configuration
typedef enum  {
  MIN_SERIALIO_PORT_MODE,
  SERIALIO_PORT_RS232 = MIN_SERIALIO_PORT_MODE,
  SERIALIO_PORT_RS485, // RS485/RS422 mode in Base IO G1, RS485 in Base IO G2
  SERIALIO_PORT_HW_HSHK,
  SERIALIO_PORT_RS422, // Not supported in Base IO G1, RS422 in Base IO G2
  MAX_SERIALIO_PORT_MODE,
} BASEIO_SER_PORT_MODE_ENUM;

//Serial port configuration structure
//(Using unsigned char instead of Enums because of packet size limitation
typedef struct {
  unsigned char serialDataLen;
  unsigned char serialBaudRate;
  unsigned char serialParity;
  unsigned char serialStopBits;
  unsigned char serialPortMode;
} __attribute (( packed )) BASEIO_SER_CONFIG_INFO;

//Digital IO - ON/OFF enums
typedef enum {
  MIN_DIO_STATE,
  DIO_STATE_OFF = MIN_DIO_STATE,
  DIO_STATE_ON,
  MAX_DIO_STATE,
} BASEIO_DIO_STATE;

//LTLOI state enums
typedef enum {
  MIN_LTLOI_STATUS,
  LTLOI_STATUS_OFF = MIN_LTLOI_STATUS,
  LTLOI_STATUS_ON,
  MAX_LTLOI_STATUS,
} LTLOI_STATUS_ENUM;

//LTLOI data
typedef struct {
  unsigned char ByteVal;
  unsigned char ByteNum;
} __attribute (( packed )) BASEIO_LTLOI_DATA;

typedef enum
{
  MIN_ANALOG_OUT_PWR_OPT = 0,
  ANALOG_OUT_PWR_INTERNAL = MIN_ANALOG_OUT_PWR_OPT,
  ANALOG_OUT_PWR_EXTERNAL,
  MAX_ANALOG_OUT_PWR_OPT
} BASEIO_ANLOUT_PWR_ENUM;

//Base IO data structure
typedef struct {
  union {
    long OnBdTempmDegC;     //Onboard temperature
    BASEIO_SER_CONFIG_INFO serialConfig;  //Serial port config info.
    unsigned long SerialIOTxTimeoutMs;    //Device side transmit timeout (when the device doesn't have a full packet of data to TX)
    BASEIO_DIO_STATE DigIOState;      //Digital IO state
    unsigned long PressureMilliVolt;  //Pressure in milli volts
    unsigned long CurrentNanoAmps;    //Current in Nano amps
    unsigned long MilliVolts;         //Milli volts (Analog IN)
    BASEIO_LTLOI_DATA LtloiData;      //Ltloi Data information
    LTLOI_STATUS_ENUM LtloiScan;      //Enable/Disable LTLOI scan & write
    LTLOI_STATUS_ENUM LtloiStream;    //Enable/Disable LTLOI streaming
    unsigned char LtloiNumClkBits;    //Number of bits to clock in/out
#ifdef MODEL_370XA
    unsigned long   PWMmPct;                // PWM setting in milli %- MUST be 0 for off cmd!!
    unsigned long   PropGain;               // Proportional gain in milli
    unsigned long   IntGain;                // Integral gain in milli
    unsigned long   DiffGain;               // Differential gain (direct value)
    float           CompFactor;             // Compensation factor (base temp/correction factor)
#endif //#ifdef MODEL_370XA
    BASEIO_ANLOUT_PWR_ENUM AnlOutPower;   // Analog output power source
  } __attribute (( packed )) BaseIOData;
} __attribute (( packed )) BASEIO_DATA_STRUCT ;

// Structure to be used for configuring (Setting / Getting) Base IO settings
typedef struct {
  CmdAckUnion           stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_BASEIO_DATA_STRUCT;

// Structure to be used for Sending Status / Nack Error code to the Host
typedef struct {
  CmdAckUnion     stCmd;
  STATUS_STRUCT   stData; // Status
} __attribute (( packed )) CAN_BASEIO_STATUS_STRUCT;

// Structure to be used for getting board information
typedef struct {
  CmdAckUnion         stCmd;    // Command 
  DEVICE_SYSTEM_INFO_STRUCT stData;   // Data
} __attribute (( packed )) CAN_BASEIO_SYSINFO_STRUCT;


/************************************************************************************/
// Serial IO Function - Commands & Data Structures
/************************************************************************************/

// Serial IO Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
  // Set commands
  CMD_SERIALIO_FN_SET_PORT_CONFIG = 0,  // Configure the serial port (Baud, Parity, Stop bits)
  CMD_SERIALIO_FN_SET_TX_TIMEOUT = 1,   // TX timeout value (when the device doesn't have a full packet of data).

  //Get commands
  CMD_SERIALIO_FN_GET_PORT_CONFIG = 40, // Configure the serial port (Baud, Parity, Stop bits)
  CMD_SERIALIO_FN_GET_ON_BD_TEMP = 41,  // Get the onboard temperature reading
} SERIALIO_CMD_ENUM;

//Serial IO streaming data...
typedef struct {
  unsigned char ucData[MAX_SERIALIO_PKT_LEN];
} __attribute (( packed )) SERIALIO_STREAM_STRUCT;

// Structure to be used for configuring (Setting / Getting) Serial IO settings
typedef struct {
  CmdAckUnion       stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_SERIALIO_DATA_STRUCT;

/************************************************************************************/
// Digital INput Function - Commands & Data Structures
/************************************************************************************/

// Digital INPUT Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
  // Set commands

  //Get commands
  CMD_DIGITAL_IN_FN_GET_CH_STATE = 40,    // Get the channel HIGH/LOW (ON/OFF) state
  CMD_DIGITAL_IN_FN_GET_ON_BD_TEMP = 41,  // Get the onboard temperature reading
} DIGITAL_IN_CMD_ENUM;

// Structure to be used for configuring (Setting / Getting) Digital IN settings
typedef struct {
  CmdAckUnion       stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_DIGITAL_IN_DATA_STRUCT;

/************************************************************************************/
// Digital OUTput Function - Commands & Data Structures
/************************************************************************************/

// Digital OUTPUT Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
  // Set commands
  CMD_DIGITAL_OUT_FN_SET_CH_STATE = 0,    // Set the channel state to HIGH/LOW (ON)

  //Get commands
  CMD_DIGITAL_OUT_FN_GET_ON_BD_TEMP = 41, // Get the onboard temperature reading
} DIGITAL_OUT_CMD_ENUM;

// Structure to be used for configuring (Setting / Getting) Digital OUT settings
typedef struct {
  CmdAckUnion       stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_DIGITAL_OUT_DATA_STRUCT;


/************************************************************************************/
// EPC Function - Commands & Data Structures
/************************************************************************************/

// Digital OUTPUT Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
  // Set commands
  CMD_EPC_FN_SET_PRESSURE   = 0,  // Set pressure (in millivolts)

#ifdef MODEL_370XA
  CMD_EPC_FN_SET_EPC_CTRL_ON_PID  = 1,    // Requires a setpoint in milli DegC. PID then takes over
  CMD_EPC_FN_SET_EPC_CTRL_ON_PWM  = 2,    // Requires a setpoint in milli %. NO PID CONTROL. IT IS SWITCHED OFF. Expected to be remote PID!!
  CMD_EPC_FN_SET_EPC_CTRL_OFF     = 3,    // Turns off selected heater
  CMD_EPC_FN_SET_PROP_GAIN        = 4,    // Set proportional gain in milli
  CMD_EPC_FN_SET_INT_GAIN         = 5,    // Set integral gain in milli
  CMD_EPC_FN_SET_DIFF_GAIN        = 6,    // Set differential gain (direct value)
  CMD_EPC_FN_SET_TEMP_COMP_BASE   = 7,    // Base temperature at which we don't do any pressure compensation
  CMD_EPC_FN_SET_TEMP_COMP_CORR   = 8,    // Correction factor for pressure sensor reading
#endif //MODEL_370XA

  //Get commands
  CMD_EPC_FN_GET_PRESSURE   = 41, // Get pressure (in millivolts)
  CMD_EPC_FN_GET_ON_BD_TEMP   = 42, // Get the onboard temperature reading

#ifdef MODEL_370XA
  CMD_EPC_FN_GET_EPC_PWM          = 43,   // Get the current value of the PWM on requested htr channel
  CMD_EPC_FN_GET_PROP_GAIN        = 44,   // Get proportional gain in milli
  CMD_EPC_FN_GET_INT_GAIN         = 45,   // Get integral gain in milli
  CMD_EPC_FN_GET_DIFF_GAIN        = 46,   // Get differential gain (direct value)
#endif //MODEL_370XA

} EPC_CMD_ENUM;

// Structure to be used for configuring (Setting / Getting) Digital OUT settings
typedef struct {
  CmdAckUnion       stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_EPC_DATA_STRUCT;


/************************************************************************************/
// Analog INput Function - Commands & Data Structures
/************************************************************************************/

// Analog INPUT Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
  // Set commands

  //Get commands
  CMD_ANALOG_IN_FN_GET_CURRENT = 40,    // Get channel current in nano amps
  CMD_ANALOG_IN_FN_GET_VOLTAGE = 41,    // Get channel voltage in nano amps
  CMD_ANALOG_IN_FN_GET_ON_BD_TEMP = 42, // Get the onboard temperature reading
} ANALOG_IN_CMD_ENUM;

// Structure to be used for configuring (Setting / Getting) Digital IN settings
typedef struct {
  CmdAckUnion       stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_ANALOG_IN_DATA_STRUCT;

/************************************************************************************/
// Analog OUTput Function - Commands & Data Structures
/************************************************************************************/

// Analog OUTPUT Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
  // Set commands
  CMD_ANALOG_OUT_FN_SET_CURRENT = 0,    // Set channel current in nano amps
  CMD_ANALOG_OUT_FN_SEL_LOOP_POWER = 1, // Select internal or external loop power

  //Get commands
  CMD_ANALOG_OUT_FN_GET_ON_BD_TEMP = 41,  // Get the onboard temperature reading
} ANALOG_OUT_CMD_ENUM;

// Structure to be used for configuring (Setting / Getting) Analog OUT settings
typedef struct {
  CmdAckUnion       stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_ANALOG_OUT_DATA_STRUCT;


/************************************************************************************/
// LTLOI Function - Commands & Data Structures
/************************************************************************************/

// LTLOI Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
  // Set commands
  CMD_LTLOI_FN_SET_BYTE = 0,          // Set the value of a output byte
  CMD_LTLOI_FN_SET_FLASH = 1,         // Set byte toggling status
  CMD_LTLOI_FN_SET_CLK_BITS = 2,      // Set number of bits to clock-in/clock-out
  CMD_LTLOI_FN_SET_SCAN_STATUS = 3,   // Enable/disable LTLOI scan
  CMD_LTLOI_FN_SET_STREAM_STATUS = 4, // Enable/disable LTLOI data streaming

  //Get commands
  CMD_LTLOI_FN_GET_BYTE = 41,         // Get the value of an input byte
  CMD_LTLOI_FN_GET_ON_BD_TEMP = 42,   // Get the onboard temperature reading
} LTLOI_CMD_ENUM;

// Structure to be used for configuring (Setting / Getting) LTLOI
typedef struct {
  CmdAckUnion       stCmd;
  BASEIO_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_LTLOI_DATA_STRUCT;

//LTLOI streaming data...
typedef struct {
  BASEIO_LTLOI_DATA LtloiData;          //Ltloi Data information
} __attribute (( packed )) LTLOI_STREAM_STRUCT;




/************************************************************************************/
// FID Function - Commands
/************************************************************************************/

// Max command size - 7 bits
typedef enum {
  // Set commands
  CMD_FID_FN_SET_IGNITION_OFF = 0,   //Turn OFF FID Ignition.
  CMD_FID_FN_SET_IGNITION_ON = 1,    //Turn ON FID Ignition.
  CMD_FID_FN_SET_GAIN_LOW = 2,       //Turn FID Gain low.
  CMD_FID_FN_SET_GAIN_HIGH = 3       //Turn FID Gain high.
} FID_CMD_ENUM;

/************************************************************************************/
// FPD Function - Commands
/************************************************************************************/
typedef enum {
  CMD_FPD_FN_DO_AUTO_ZERO = 0    //Do FPD AutoZero.
} FPD_CMD_ENUM;
#endif // #ifndef _BASEIO_PROTOCOL_H

