/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Definitions.h
 * *
 * *  Description: Common definitions for the HAL code
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
 * *  $Id: Definitions.h,v 1.26 2016/11/01 13:03:18 emrsn\jbeatty Exp $
 * *  $Log: Definitions.h,v $
 * *  Revision 1.26  2016/11/01 13:03:18  emrsn\jbeatty
 * *  Added support for FPD G2
 * *
 * *  Revision 1.25  2016/08/30 09:57:51  emrsn\jbeatty
 * *  Mods for 1500XA R2 support
 * *
 * *  Revision 1.24  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.23  2016/01/14 21:32:02  emrsn\rrthomas
 * *  Added a device type with which we can broadcast a Cycle Clock Start Message to all the Preamp boards at one shot. This will make it possible for Preaming Streaming objects present on different physical boards to start the analysis cycle at the exact same time. If we have four detectors in a Cycle Clock and if we use individual start messages to start each streaming object, then it is possible that they will start in the order in which we sent them the start messages. If for some reason one or more of these messages are delayed, we will end up with different detectors that start a different times.
 * *
 * *  For this added the following -
 * *  1. Added a way to associate each Preamp Streaming Object (or detector) with a Cycle Clock. Added a function to set the Cycle Clock for each detector.
 * *  2. New Function Type called Cycle Clock Sync. This will reside in the Preamp Boards (G1 and G2) and can be accessible as CAN address "1". All preamps will receive this message with a Cycle Clock number (1/2/3/4). When this message is recevied all the detectors associated with that particular cycle clock will start streaming data to the main CPU.
 * *
 * *  Revision 1.22  2015/04/30 09:32:31  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.21  2014/06/26 10:01:11  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.20  2012/03/21 06:49:04  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.19  2012/01/03 13:04:15  emrsn\rsaha
 * *  Added support to communicate with IMB device.
 * *
 * *  Revision 1.18  2011/07/13 10:52:52  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.17  2010/06/10 20:10:01  emrsn\rsaha
 * *  Added HAL interfaces for Pressure read, Serial RS232/485 control support and diagnostics details of Analyzer IO card.
 * *
 * *  Revision 1.16  2010/03/31 06:50:36  emrsn\rsaha
 * *  Revised to resolve 370XA Analyzer IO slot Id to slot name.
 * *
 * *  Revision 1.15  2009/12/17 14:06:24  emrsn\rsaha
 * *  Added support for FPD in BaseIO card.
 * *
 * *  Revision 1.14  2009/04/15 13:24:15  epm-na\rsaha
 * *  Revised with support for FID Ignition and Gain control.
 * *
 * *  Revision 1.13  2008/11/25 00:14:19  epm-na\rsaha
 * *  Added new function type named DIAGNOSTIC.
 * *
 * *  Revision 1.12  2008/10/22 22:24:08  epm-na\rsaha
 * *  Updated number of supported device functions to 15 from 14.
 * *
 * *  Revision 1.11  2008/10/09 23:37:40  epm-na\rthomas
 * *  Increased the number of HW device functions from 12 to 14 to accomodate Foundation Fieldbus and Graphical LOI.
 * *
 * *  Revision 1.10  2007/08/21 21:30:14  epm-na\lratner
 * *  Removed now-obselete #define _HAL_DEBUG
 * *
 * *  Revision 1.9  2007/04/05 13:14:51  epm-na\rthomas
 * *  LTLOI: Added HAL function to access the LTLOI interface on the base IO
 * *  board.
 * *
 * *  Revision 1.8  2007/01/10 21:52:33  EPM-NA\ssatpute
 * *  Added interface for BCI Can Interface board. Tested. Working fine.
 * *
 * *  Revision 1.7  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



// Defintions / Enums / Structures for Communication between CAND (CAN Daemon) and HAL (Hardware Abstraction Layer)
// 
#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

//#include "halglobal.h"
#include <global.h>
#include "DevProtocol.h"  // Device specific protocol definitions
#include "CommTypes.h"

typedef int BOOL;

// Task ID and Mailbox ID for sending command / CAN Packets to CAND.
// This is to be used by all HAL objects for communication with CAND
#define CMD_TX_PIPE_TASK_ID     1
#define CMD_TX_PIPE_MAILBOX_ID  0


// Mailboxes for Command Respond Pipe and Stream Pipe. 
// All HAL objects to use the following Mailbox IDs, but different 
// Task IDs. Task ID to be generated from the device name. Task ID will 
// be a 3 digit number that is derived from the device name. 
// If dev name is FnType:SlotAddr:FnCnt, then the Task ID will be 
// [FnTypeEnum][SlotAddrEnum][FnCntEnum]. This will be done by the 
// CCANComm class during the open function call. 
#define CMD_RESP_PIPE_MAILBOX_ID  0
#define CMD_STRM_PIPE_MAILBOX_ID  1

#define HAL_DFLT_TIMEOUT    300   // In ms

#define MAX_NUM_DEV_FUNCTIONS   23  // Maximum number of device functions in GC700XP

#define MAX_NUM_SLOTS     21  // Maximum number of slots for connecting plug-in cards

#define MAX_DEV_NAME  256

// TODO: What do we need this for? Take out!
// Data structure for storing Task ID / Mailbox Number for Named Pipe Comm
struct PipeMailBoxStruct {
  int nTaskID;    
  int nMailboxNum;
};

#define CAN_PKT_MAX_LEN   8 // Maximum CAN Packet size
#define CAN_PKT_DATA_LEN  6 // Data length for CAN Payload (2 bytes are used for address, hence 6 bytes are left for data)

// Error codes
enum ERR_CODE {
  ERR_NOT_IMPLEMENTED = -100, // Not implemented yet. Need to take this out eventually. 
  ERR_OPEN_FILE = -16, // Can't open file
  ERR_CMD_FAILED = -15, // The device indicated that the command was NOT successfully executed
  ERR_DATA_PENDING = -14, // Data pending - if trying to read a fragmented packet before it's fully received
  ERR_WRONG_CRC = -13,  // Incorrect CRC in a fragmented packet
  ERR_TIMEOUT = -12,    // Time out for streaming data
  ERR_DEV_UNKNOWN = -11,    // Unrecognized error code from remote device
  ERR_DEV_INT_ERR = -10,    // Internal error in remote device
  ERR_MEMORY_ERR = -9,    // Memory allocation error
  ERR_INVALID_DEV_NAME = -8, // HAL unable to resolve the device name into SlotID, Fn Type, Fn Enum. 
  ERR_DEV_NOT_REG = -7, // Device un-registering error - Device was never registered with CAND
  ERR_DEV_IN_USE = -6,  // Device Registration error - Device is in use
  ERR_NO_RESP = -5,   // No response from remote client, Communication failure
  ERR_INTERNAL_ERR = -4,  // Internal Error (Unable to open pipes, CAND not running etc)
  ERR_PROTOCOL = -3,    // Protocol Error
  ERR_INVALID_ARGS = -2,  // Invalid arguments passed to function
  ERR_INVALID_SEQ = -1, // Invalid sequence - For eg, Read or Write or Close called without Open 
  ERR_SUCCESS = 0     // Success, not an error
};

enum CAND_COMMAND_TYPE {
  REGISTER_DATA_CH = 0, // Register a CAN Channel
  UNREGISTER_DATA_CH,   // Unregister a CAN Channel
  TX_CAN_DATA           // Transmit CAN Data
};

enum CAND_RESP_TYPE {
  REGISTER_ACK = 0, // Acknowledgement sent to Register command 
  UNREGISTER_ACK,
  RESP_PACKET,      // Response packet
  STREAM_DATA       // Stream Data
};

enum REGISTRATION_STATUS {
  CAND_DUPLICATE_REGISTRATION = 0,  // Response to Duplicate Registration, Registration failed
  CAND_NOT_REGISTERED,              // Response when un-registering, un-registering failed
  CAND_INTERNAL_ERROR,              // Internal Error
  CAND_SUCCESS                      // register / un-register successfull 
};

// Data to be passed along with the Register / Unregister command
struct RegisterCmdDataStruct {
  unsigned int  CmdRespIPCid;     // Unique IPC Id for CAND to open for routing command responses
  unsigned int  StreamRespIPCid;  // Unique Id for CAND to open for sending streaming data. If value is 
                                  // is zero, then this module is not expecting any stream data
  unsigned char SlotID;           // Remote device address (Slot : FnType : FnCount)
  unsigned char FnType;
  unsigned char FnCount;
};

// CAN Packet struct
struct RxTxDataStruct {
  unsigned char CANId;            // CAN ID to transmit data to. Ignored when CAND sends receive packets to HAL
  unsigned char PktLen;           // Length of the CAN Packet
  unsigned char PktData [CAN_PKT_MAX_LEN];  // Data to be transmitted on CAN - To be used for TXing data
};

// Data section for commands passed from HAL to CAND
union CmdDataUnion {
  RegisterCmdDataStruct stRegCmdData;     // Parameters to configure CAND and IPC with HAL
  RxTxDataStruct      stTxData;           // Data to be transmitted
};

// Command / Data section to be written to the CAND Command / TX Pipe
struct CANDCmdStruct{
  CAND_COMMAND_TYPE CmdType;  // Type of command
  CmdDataUnion CmdData;       // Data corresponding to the command
};

// Data section for response from CAND to HAL
union CANDRespUnion {
  ERR_CODE RegStatus;         // Status of Register / Un-register command
  RxTxDataStruct stRxData;    // CAN Packet
};

// Response / Data section to be written from CAND to HAL.
struct CANDRespStruct {
  CAND_RESP_TYPE RespType;    // Response type
  CANDRespUnion stRespData;   // Data (Status or CAN Packet)
};



#endif // #ifndef _DEFINITIONS_H
