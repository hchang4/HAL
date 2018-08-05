/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: BaseIOProtocol.h
 * *
 * *  Description: Preamp protocol code & data structures
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
 * *  $Id: PreampProtocol.h,v 1.19 2017/02/13 16:46:06 emrsn\jbeatty Exp $
 * *  $Log: PreampProtocol.h,v $
 * *  Revision 1.19  2017/02/13 16:46:06  emrsn\jbeatty
 * *  Oops, forgot to clean up before checkin
 * *
 * *  Revision 1.18  2017/02/13 16:32:02  emrsn\jbeatty
 * *  Added support for G2 Preamp Hardware Inhhibit timed event
 * *
 * *  Revision 1.17  2016/01/14 21:32:02  emrsn\rrthomas
 * *  Added a device type with which we can broadcast a Cycle Clock Start Message to all the Preamp boards at one shot. This will make it possible for Preaming Streaming objects present on different physical boards to start the analysis cycle at the exact same time. If we have four detectors in a Cycle Clock and if we use individual start messages to start each streaming object, then it is possible that they will start in the order in which we sent them the start messages. If for some reason one or more of these messages are delayed, we will end up with different detectors that start a different times.
 * *
 * *  For this added the following -
 * *  1. Added a way to associate each Preamp Streaming Object (or detector) with a Cycle Clock. Added a function to set the Cycle Clock for each detector.
 * *  2. New Function Type called Cycle Clock Sync. This will reside in the Preamp Boards (G1 and G2) and can be accessible as CAN address "1". All preamps will receive this message with a Cycle Clock number (1/2/3/4). When this message is recevied all the detectors associated with that particular cycle clock will start streaming data to the main CPU.
 * *
 * *  Revision 1.16  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.15  2014/06/26 10:01:17  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.14  2012/09/24 03:55:07  emrsn\rsaha
 * *  Revised to Enable/Disable 700XA Preamp IO AutoZero algorithm. Previously only Enable support was there in HAL.
 * *
 * *  Revision 1.13  2012/09/07 12:49:24  emrsn\rsaha
 * *  Revised with support to Enable preamp AutoZero and to read bridge left and right detector status.
 * *
 * *  Revision 1.12  2012/03/21 06:49:08  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.11  2009/05/07 16:39:14  epm-na\rthomas
 * *  Starts/Stops broadcast for all channels in a preamp. Added this API to prevent the start time of the two channels from being affected by communication jitters. This API should be used in Mode 1 (1 Stream, 2 Detectors, 1 Method) to prevent a lag between detector 1 and 2.
 * *  But having said that, the 2nd Preamp Streaming object needs to do some initialization for beginning/stopping streaming. For such cases, call this API with the doInitOnly flag set to "TRUE"; and this should be done preferrably before starting broadcast and after stopping broadcast using the other channel. This will ensure the object is ready for streaming but the API in itself will not communicate with the Preamp device.
 * *
 * *  Revision 1.10  2008/11/17 22:12:26  epm-na\rsaha
 * *  Updated protocol to retrive low to high sub channel scaling factor of a preamp.
 * *
 * *  Revision 1.9  2007/04/26 08:56:42  epm-na\rthomas
 * *   - Added functions to read CAL status (in progress or not) through
 * *     either premap config or streaming objects.
 * *
 * *  Revision 1.8  2007/04/25 10:59:39  epm-na\rthomas
 * *   - Added ability to enable self calibration through preamp stream and
 * *     config objects.
 * *
 * *  Revision 1.7  2007/03/27 08:51:07  epm-na\rthomas
 * *  Preamp:
 * *   - Replaced references to 2X and 20X with LOW and HIGH gain respectively.
 * *
 * *  Revision 1.6  2007/03/13 08:33:35  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *
 * *  Revision 1.5  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#ifndef _PREAMP_PROTOCOL_H
#define _PREAMP_PROTOCOL_H

#include "DevProtocol.h"

/************************************************************************************/
// Common structures
/************************************************************************************/
// For the Preamp Bridge gain selection
typedef enum
{
  MIN_PREAMP_BRIDGE_GAIN = 0,
  PREAMP_BRIDGE_GAIN_LOW = MIN_PREAMP_BRIDGE_GAIN,
  PREAMP_BRIDGE_GAIN_HIGH,
  PREAMP_BRIDGE_GAIN_AUTO,
  MAX_PREAMP_BRIDGE_GAIN,
} PREAMP_BRIDGE_GAIN_ENUM;

// For the Preamp Sampling rate selection
typedef enum
{
  MIN_PREAMP_SAMPLING_RATE = 0,
  PREAMP_SAMPLING_RATE_6HZ875 = MIN_PREAMP_SAMPLING_RATE,
  PREAMP_SAMPLING_RATE_13HZ75,
  PREAMP_SAMPLING_RATE_27HZ5,
  PREAMP_SAMPLING_RATE_55HZ,
  MAX_PREAMP_SAMPLING_RATE,
} PREAMP_SAMPLING_RATE_ENUM;

// For AC bridge ON/OFF status selection
typedef enum
{
  MIN_PREAMP_STATE = 0,
  PREAMP_STATE_OFF = MIN_PREAMP_STATE,
  PREAMP_STATE_ON,
  MAX_PREAMP_STATE,
} PREAMP_STATE_ENUM;

typedef enum
{
  PREAMP_HWINHIBIT_OFF = 0,
  PREAMP_HWINHIBIT_ON,
} PREAMP_HWINHIBIT_STATE;

// Offset type enum
typedef enum
{
  MIN_PREAMP_OFFSET_TYPE = 0,
  BRIDGE_LEFT_COARSE = MIN_PREAMP_OFFSET_TYPE,
  BRIDGE_RIGHT_COARSE,
  BRIDGE_RIGHT_FINE,
  MAX_PREAMP_OFFSET_TYPE,
} PREAMP_OFFSET_TYPE;

// Preamp bridge adjustment enum
typedef enum
{
  BRIDGE_ADJ_0000,
  BRIDGE_ADJ_0001,
  BRIDGE_ADJ_0010,
  BRIDGE_ADJ_0011,
  BRIDGE_ADJ_0100,
  BRIDGE_ADJ_0101,
  BRIDGE_ADJ_0110,
  BRIDGE_ADJ_0111,
  BRIDGE_ADJ_1000,
  BRIDGE_ADJ_1001,
  BRIDGE_ADJ_1010,
  BRIDGE_ADJ_1011,
  BRIDGE_ADJ_1100,
  BRIDGE_ADJ_1101,
  BRIDGE_ADJ_1110,
  BRIDGE_ADJ_1111,
  NUM_BRIDGE_ADJ_ENUM

} PREAMP_BRIDGE_ADJ_ENUM;

typedef enum
{
	CYCLE_CLOCK_1 = 1, 
  CYCLE_CLOCK_2 = 2, 
  CYCLE_CLOCK_3 = 3, 
  CYCLE_CLOCK_4 = 4, 
  CYCLE_CLOCK_5 = 5, 
  CYCLE_CLOCK_6 = 6, 
	MAX_CYCLE_CLOCK,
} CYCLE_CLOCK_ENUM;


typedef struct
{
  unsigned int   unBridgeData;
  unsigned short usTimeStamp;
} __attribute (( packed )) PREAMP_STREAM_STRUCT;

typedef struct
{
  union
  {
    PREAMP_BRIDGE_GAIN_ENUM Gain;           // Preamp gain
    PREAMP_SAMPLING_RATE_ENUM SamplingRate; // Preamp sampling rate
    PREAMP_STATE_ENUM ACBridgeStatus;       // AC bridge ON/OFF status
    PREAMP_STATE_ENUM FilterStatus;         // Filter ON/OFF
    PREAMP_STATE_ENUM MovAvgStatus;         // Moving average ON/OFF
    PREAMP_STATE_ENUM CalInProgress;        // Calibration status
    PREAMP_STATE_ENUM DetectorStatus;       // Detector status
    PREAMP_STATE_ENUM AutoZero;             // AutoZero ON/OFF
    PREAMP_BRIDGE_ADJ_ENUM OffsetValue;     // Preamp bridge offset config value
    CYCLE_CLOCK_ENUM CycleClock;            // Cycle Clock associated with this Detector
    long OnBdTempmDegC;                     // Onboard temperature
    long PreampValue;                       // Current preamp value
    float Low2HighGainScaleFactor;          // Computed scale factor between low to high gain sub channels.
    PREAMP_HWINHIBIT_STATE HwInhibit;       // Hardware inhibit ON/OFF
  } __attribute (( packed )) preampData;
} __attribute (( packed )) PREAMP_DATA_STRUCT;

// Structure to be used for getting board information
typedef struct
{
  CmdAckUnion               stCmd;  // Command
  DEVICE_SYSTEM_INFO_STRUCT stData; // Data
} __attribute (( packed )) CAN_CMD_PREAMP_SYSINFO_STRUCT;

/************************************************************************************/
// Preamp Function - Commands & Data Structures
/************************************************************************************/

// Cycle Clock Synchronization
typedef enum
{
	// Set commands
	CMD_CYCLE_CLOCK_START = 0,		// Start Cycle Clock 
  CMD_CYCLE_CLOCK_STOP = 1,			// Stop/Finish Cycle Clock 
  
  // Get commands
} CYCLE_CLOCK_CMD_ENUM;

// Preamp Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum
{
  // Set commands
  CMD_PREAMP_STR_FN_EN_BROADCAST = 0,       // Enable streaming data 
  CMD_PREAMP_STR_FN_DIS_BROADCAST = 1,      // Disable streaming data
  CMD_PREAMP_STR_FN_SET_CAL_ON = 2,         // Turn ON Calibration
  CMD_PREAMP_STR_FN_EN_BROADCAST_ALL = 3,   // Turn ON Broadcast for both channels
  CMD_PREAMP_STR_FN_DIS_BROADCAST_ALL = 4,  // Turn OFF Broadcast for both channels
  CMD_PREAMP_STR_FN_SET_CYCLE_CLOCK = 5,    // Set Cycle Clock associated with this detector
  // Get commands
  CMD_PREAMP_STR_FN_GET_CAL_STATUS = 40,    // Check if CAL is ON or not
} PREAMP_STR_CMD_ENUM;

// Preamp Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum
{
  // Set commands
  CMD_PREAMP_CFG_FN_SET_GAIN = 0,          // Set Gain
  CMD_PREAMP_CFG_FN_SET_SAMPLING_RATE = 1, // Set Sampling Rate
  CMD_PREAMP_CFG_FN_SET_AC_BRIDGE = 2,     // Turn ON/OFF AC Bridge
  CMD_PREAMP_CFG_FN_SET_LEFT_COARSE = 3,   // Set the bridge left coarse value
  CMD_PREAMP_CFG_FN_SET_RIGHT_COARSE = 4,  // Set the bridge right coarse value
  CMD_PREAMP_CFG_FN_SET_RIGHT_FINE = 5,    // Set the bridge right fine value
  CMD_PREAMP_CFG_FN_SET_FILTER = 6,        // Turn ON/OFF filter
  CMD_PREAMP_CFG_FN_SET_MOV_AVG = 7,       // Turn ON/OFF moving avg.
  CMD_PREAMP_CFG_FN_SET_CAL_ON = 8,        // Turn ON Calibration
  CMD_PREAMP_CFG_FN_SET_AUTOZERO = 9,      // Turn ON AutoZero

  // Get commands
  CMD_PREAMP_CFG_FN_GET_GAIN = 40,          // Get the current Gain setting
  CMD_PREAMP_CFG_FN_GET_SAMPLING_RATE = 41, // Get the current sampling rate
  CMD_PREAMP_CFG_FN_GET_AC_BRIDGE = 42,     // Get the AC Bridge ON/OFF status
  CMD_PREAMP_CFG_FN_GET_LEFT_COARSE = 43,   // Get the bridge left coarse value
  CMD_PREAMP_CFG_FN_GET_RIGHT_COARSE = 44,  // Get the bridge right coarse value
  CMD_PREAMP_CFG_FN_GET_RIGHT_FINE = 45,    // Get the bridge right fine value
  CMD_PREAMP_CFG_FN_GET_FILTER = 46,        // Get filter ON/OFF status
  CMD_PREAMP_CFG_FN_GET_MOV_AVG = 47,       // Get moving avg ON/OFF status
  CMD_PREAMP_CFG_FN_GET_ON_BD_TEMP = 48,    // Get the onboard temperature reading
  CMD_PREAMP_CFG_FN_GET_PREAMP_VALUE = 49,  // Get the current preamp value
  CMD_PREAMP_CFG_FN_GET_CAL_STATUS = 50,    // Check if CAL is ON or not
  CMD_PREAMP_CFG_FN_GET_SCALE_FACTOR = 51,  // Get low to high gain scaling factor.
  CMD_PREAMP_CFG_FN_GET_BR_LEFT_DET_STATUS = 52,  // Get Detector Bridge Left status - connected / not connected.
  CMD_PREAMP_CFG_FN_GET_BR_RIGHT_DET_STATUS = 53  // Get Detector Bridge Right status - connected / not connected.
} PREAMP_CFG_CMD_ENUM;

typedef enum
{
  CMD_PREAMP_CTRL_FN_SET_HW_INHIBIT = 0,    // Turn hardware inhibit on/off
} PREAMP_CTRL_CMD_ENUM;

// Structure to be used for configuring (Setting / Getting) Preamp settings
typedef struct
{
  CmdAckUnion           stCmd;
  PREAMP_DATA_STRUCT    stData; // Data
} __attribute (( packed )) CAN_CMD_PREAMP_DATA_STRUCT;

// Structure to be used for Sending Status / Nack Error code to the Host
typedef struct {
  CmdAckUnion     stCmd;
  STATUS_STRUCT   stData; // Status
} __attribute (( packed )) CAN_CMD_PREAMP_STATUS_STRUCT;

#endif // #ifndef _PREAMP_PROTOCOL_H
