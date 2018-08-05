/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Reliability.h
 * *
 * *  Description: CAN communication reliability layer
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
 * *  $Id: Reliability.h,v 1.14 2016/01/14 21:32:02 emrsn\rrthomas Exp $
 * *  $Log: Reliability.h,v $
 * *  Revision 1.14  2016/01/14 21:32:02  emrsn\rrthomas
 * *  Added a device type with which we can broadcast a Cycle Clock Start Message to all the Preamp boards at one shot. This will make it possible for Preaming Streaming objects present on different physical boards to start the analysis cycle at the exact same time. If we have four detectors in a Cycle Clock and if we use individual start messages to start each streaming object, then it is possible that they will start in the order in which we sent them the start messages. If for some reason one or more of these messages are delayed, we will end up with different detectors that start a different times.
 * *
 * *  For this added the following -
 * *  1. Added a way to associate each Preamp Streaming Object (or detector) with a Cycle Clock. Added a function to set the Cycle Clock for each detector.
 * *  2. New Function Type called Cycle Clock Sync. This will reside in the Preamp Boards (G1 and G2) and can be accessible as CAN address "1". All preamps will receive this message with a Cycle Clock number (1/2/3/4). When this message is recevied all the detectors associated with that particular cycle clock will start streaming data to the main CPU.
 * *
 * *  Revision 1.13  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.12  2014/06/26 10:01:18  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.11  2012/03/21 06:49:08  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.10  2009/11/09 13:10:02  epm-na\rsaha
 * *  Updated with API to return non-elapsed timeout interval.
 * *
 * *  Revision 1.9  2009/07/18 09:01:11  epm-na\rsaha
 * *  Enabled code to count number of retries made in Reliability layer.
 * *
 * *  Revision 1.8  2009/05/13 22:39:32  epm-na\rsaha
 * *  Revised GetRemoteResp prototype definition to receive more than 256 bytes of fragmented packets.
 * *
 * *  Revision 1.7  2008/10/22 22:30:03  epm-na\rsaha
 * *  Changed argument "byNumBytesCmd" of type unsigned char to "unNumBytesCmd" of unsigned int type in GetRemoteResp() method to support fragmented data transfer of more than 256 bytes. Added a public member variable to access the number of attempts made to transfer the data. This is for debugging purpose only. Uncomment "RELIABILITY_DEBUG" macro definition in "Reliability.h" file to get this enabled.
 * *
 * *  Revision 1.6  2008/05/22 05:46:29  EPM-NA\kdarji
 * *  - Added timeout parameter in "GetRemoteResp" method, which tells the amount
 * *  of time to wait for response from remote device.
 * *  [Note: Default timeout time is as it was previously i.e. HAL_DFLT_TIMEOUT
 * *  (300 MilliSeconds)]
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



// Reliability.h - header file for CReliability
//

#ifndef _RELIABILITY_H
#define _RELIABILITY_H

#include "Definitions.h"  // For common definitions and structures.
#include "CANComm.h"


// Maximum number of retries before the reliability layer should give up.
#define MAX_NO_RETRIES  3

class CReliability {
private: 
  int m_nRetryCount;

  CCANComm *m_pobCANComm;

public:

  CReliability(CCANComm *pobCANComm = NULL); // Default Constructor
  ~CReliability(); // Default Destructor

  int SetMaxRetries(int nMaxRetries);

  // Send a command to the remote device and get a response back from it.
  // For Response wait up to requested time-out
  int GetRemoteResp (unsigned char* pbyCmd,       // Data to form command packet
                     unsigned int unNumBytesCmd,  // Number of bytes in the command packet
                     unsigned char *pbyRespData,  // Pointer to write device response to
                     unsigned int unNumBytesResp, // Number of bytes expected from remote device
                     BOOL bStreamingTx = FALSE,                 // Indicates if we are transmitting streaming data
                     unsigned int unTimeOut = HAL_DFLT_TIMEOUT);// Time to wait for response from remote device

  int m_nRetryAttempts;

  // Returns part of the timeout interval that was not used.
  int GetRemTimeOut();

  // Send a command and expect no response
  int TxCmd (unsigned char* pbyCmd,       // Data to form command packet
             unsigned int unNumBytesCmd,  // Number of bytes in the command packet
             BOOL bStreamingTx = FALSE);  // Indicates if we are transmitting streaming data

};
#endif // #ifndef _RELIABILITY_H

