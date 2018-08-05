/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: CANComm.h 
 * *
 * *  Description: CAN communication layer. All communication to the
 * *               CAN Daemon is routed through this class.
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
 * *  $Id: CANComm.h,v 1.15 2016/11/16 20:48:01 emrsn\rrthomas Exp $
 * *  $Log: CANComm.h,v $
 * *  Revision 1.15  2016/11/16 20:48:01  emrsn\rrthomas
 * *  Includes the following changes/fixes -
 * *  1. Start Rx Pipe (for CAND to communicate with HAL object) naming/numbering starting after 32,768 (or max pid) to avoid conflicts with any other task that used PID number for pipe names.
 * *  2. Fix for potential duplicate Pipe Names when using higher order Function Types and Slot IDs
 * *  3. Use only one TX Pipe fd (for HAL object to send requests/CAN packets to CAND) for all the HAL objects in a process. Earlier, each HAL object created an fd to CAND TX Pipe which really does not serve any purpose.
 * *  4. Clean up commented out code and warning that are not relevant.
 * *
 * *  Revision 1.14  2015/04/30 09:32:31  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.13  2014/06/26 10:01:10  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.12  2012/03/21 06:49:03  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.11  2009/11/09 13:09:45  epm-na\rsaha
 * *  Updated with API to return non-elapsed timeout interval.
 * *
 * *  Revision 1.10  2009/05/12 19:14:14  epm-na\rthomas
 * *  Added a function to flush the Stream Pipe.
 * *
 * *  Revision 1.9  2008/10/22 22:28:50  epm-na\rsaha
 * *  Updated a parameter of type unsigned short to unsigned int for few methods to support fragmented data exchange of more than 256 bytes.
 * *
 * *  Revision 1.8  2008/05/22 05:34:14  EPM-NA\kdarji
 * *  - Added timeout parameter in "CANGetRemoteResp" method, which tells the amount
 * *  of time to wait for response from remote device.
 * *  [Note: Default timeout time is as it was previously i.e. HAL_DFLT_TIMEOUT
 * *  (300 MilliSeconds)]
 * *
 * *  Revision 1.7  2008/01/08 19:52:46  epm-na\lratner
 * *  Moved class CIPC from /platform/HAL/ipc to /platform/ipc.
 * *
 * *  Revision 1.6  2007/08/09 07:13:26  EPM-NA\ssatpute
 * *  Added below functions to return receive pipe file descriptor - streaming data.
 * *  1) CANGetRxStrmFd() in CANComm.h
 * *  2) GetRxStreamingFd() in LtLoi.h
 * *  [-Kashyap]
 * *
 * *  Revision 1.5  2007/03/13 08:33:35  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *
 * *  Revision 1.4  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



// CANComm.h - Header file for CCANComm class
//

#ifndef _CANCOMM_H
#define _CANCOMM_H


#include "Definitions.h"  // For common definitions and structures.
#include "ipc.h"          // For Named Pipe Comm.
#include "DataFragment.h"


// Class for Sending / Receiving CAN Data
class CCANComm {
private:
  
  static CIPC m_obIPCCmdTx; // Transmit Pipe - Commands to remote board. Need ONLY one per process because this is a common pipe.
  CIPC m_obIPCCmdRespRx;    // Receive Pipe - Command Response / acknowledgement from remote board
  CIPC m_obIPCStreamRx;     // Receive Pipe - Streaming data from remote board

  // Does the device support a streaming interface
  BOOL m_bIsStreaming;

  // Is the CmdTx pipe open? Need ONLY one Command Pipe per process because this is a common pipe to CAND.
  // This variable keeps track of how many instances are open so that we can close the Cmd Tx Pipe when 
  // the last object destroyed
  static int m_nInstances; 

  // Is the CmdRespRx pipe open?
  BOOL m_bIsCmdRespPipeOpen;

  // Is the Stream Pipe open?
  BOOL m_bIsStreamPipeOpen;

  // Remote Device address 
  unsigned char m_bySlotID;   // Slot Address (CAN Message Identifier)
  unsigned char m_byFnType;   // Device Function Type (Analog In / Analog Out etc...)
  unsigned char m_byFnEnum;   // Device Function Enumeration

  CDataFragment m_obCmdRespFrag;    // Fragment object for storing Command Response fragments
  CDataFragment m_obStrmRespFrag;   // Fragment object for storing Stream Response fragments

  DB_INT32 m_nRemTimeOut;

  // Private Helper Functions
  int CloseRxPipes(); // Close all the open pipes.

  // Read from CAND
  int RxData (unsigned char* pbyData,    // Pointer to write data to
              unsigned int unDataLen,    // Number of bytes to read
              BOOL bStrmPipe,            // Which pipe to read from. If bStrmPipe = TRUE -> Stream Pipe, else CmdRespPipe
              unsigned int* punTimeout); // Timeout - NULL -> Blocking Rx, *punTimeout -> Non-blocking with timeout

public:
  CCANComm(); // Default constructor
  ~CCANComm(); // Default destructor
  
  // Open the following pipes for bi-directional communication with CAND
  // (1) Command TX Pipe to send commands to CAND and to Transmit CAN packets to 
  // remote devices
  // (2) Command Response Pipe to read acknowledgements from remote devices
  // (3) If bIsStreaming is TRUE, then open a pipe for reading streaming data
  // Performs the following functionality - 
  // (1) Opens Pipes (For TX, RX and Streaming data)
  // (2) Sends command to CAND to register the device
  int CANCommOpen (unsigned char bySlotId, 
                   unsigned char byFnType, 
                   unsigned char byFnEnum, 
                   BOOL bIsStreaming = FALSE);

  // Close all open pipes, release any resource/memory allocated
  int CANCommClose ();

  // Transmit a CAN command to CAND to transmit over the CAN driver interface
  // This function makes a CAN payload packet with remote address (Slot Address, 
  // Function type and Function Enumeration) and the data.
  // If the CAN Packet size exceeds 8 bytes (the max CAN packet size), then it fragments
  // the packet into multiple CAN packets. 
  // Returns the number of bytes transmitted. 
  int CANTxCmd (unsigned char* pbyData,     // Pointer to data
                unsigned int unDataLen,     // Data length
                BOOL bStreamingTx = FALSE); // Indicates if we are transmitting streaming data

  // Read Command Response from remote device. This is a blocking call
  // This functions strips away the remote devices address from the CAN payload and 
  // returns only the data back to the calling function.
  // Function returns the number of bytes read on success, negative error code 
  // on error.
  int CANRxCmdRespBlocking (unsigned char* pbyData,   // Pointer to write data to
                            unsigned int unDataLen);  // Number of bytes to read

  // Read Command Response from remote device. The user can specify a time out in 
  // milli-seconds.
  // This functions strips away the remote devices address from the CAN payload and 
  // returns only the data back to the calling function.
  // Function returns the number of bytes read on success, negative error code 
  // on error.
  int CANRxCmdRespTimeout (unsigned char* pbyData,        // Pointer to write data to
                           unsigned int unDataLen,        // Number of bytes to read  
                           unsigned int unTimeOut = 1000);// Timeout in milli-seconds 

  // Read streaming data from remote device. This is a blocking call.
  // This functions strips away the remote devices address from the CAN payload and 
  // returns only the data back to the calling function.
  // Function returns the number of bytes read on success, negative error code 
  // on error.
  int CANRxStrmBlocking (unsigned char* pbyData,  // Pointer to write data to
                         unsigned int unDataLen); // Number of bytes to read

  // Read streaming data from remote device. The user can specify a time out in 
  // milli-seconds. 
  // This functions strips away the remote devices address from the CAN payload and 
  // returns only the data back to the calling function.
  // Function returns the number of bytes read on success, negative error code 
  // on error.
  int CANRxStrmTimeout (unsigned char* pbyData,              // Pointer to write data to
                        unsigned int unDataLen,              // Number of bytes to read
                        unsigned int unTimeOut = 1000,       // Timeout in milli-seconds
                        unsigned int *punRemTimeOut = NULL); //  Remaining time-out


  // Send a command to the remote device and get a response back from it.
  // This is a two-in-one function call (Sends and Receives)
  // Returns negative error code on failure. On success, returns the number of bytes 
  // read from remote device.
  int CANGetRemoteResp (unsigned char* pbyCmd,          // Data to form command packet
                        unsigned int unNumBytesCmd,     // Number of bytes in the command packet
                        unsigned char *pbyRespData,     // Pointer to write device response to
                        unsigned int unNumBytesResp,    // Number of bytes expected from remote device
                        BOOL bStreamingTx = FALSE,      // Indicates if we are transmitting streaming data
                        unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for reading response from device

  // Flush the command response pipe
  int CANFlushCmdRespPipe ();
  // Return Receive Pipe File Descriptor - Streaming data from remote board
  int CANGetRxStrmFd();

  int CANFlushStreamPipe ();

  //Returns part of the timeout interval that was not used.
  int GetRemTimeOut();
};

#endif // #ifndef _CANCOMM_H

