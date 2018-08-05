/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Reliability.cpp
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
 * *  $Id: Reliability.cpp,v 1.16 2016/01/14 21:32:01 emrsn\rrthomas Exp $
 * *  $Log: Reliability.cpp,v $
 * *  Revision 1.16  2016/01/14 21:32:01  emrsn\rrthomas
 * *  Added a device type with which we can broadcast a Cycle Clock Start Message to all the Preamp boards at one shot. This will make it possible for Preaming Streaming objects present on different physical boards to start the analysis cycle at the exact same time. If we have four detectors in a Cycle Clock and if we use individual start messages to start each streaming object, then it is possible that they will start in the order in which we sent them the start messages. If for some reason one or more of these messages are delayed, we will end up with different detectors that start a different times.
 * *
 * *  For this added the following -
 * *  1. Added a way to associate each Preamp Streaming Object (or detector) with a Cycle Clock. Added a function to set the Cycle Clock for each detector.
 * *  2. New Function Type called Cycle Clock Sync. This will reside in the Preamp Boards (G1 and G2) and can be accessible as CAN address "1". All preamps will receive this message with a Cycle Clock number (1/2/3/4). When this message is recevied all the detectors associated with that particular cycle clock will start streaming data to the main CPU.
 * *
 * *  Revision 1.15  2015/05/04 14:49:28  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.14  2014/06/26 10:01:00  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.13  2013/01/30 06:18:50  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.12  2012/03/21 06:49:00  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.11  2009/11/09 13:10:48  epm-na\rsaha
 * *  Updated with API to return non-elapsed timeout interval.
 * *
 * *  Revision 1.10  2009/07/18 09:03:10  epm-na\rsaha
 * *  Enabled code to count number of retries made in Reliability layer.
 * *
 * *  Revision 1.9  2009/05/13 22:53:39  epm-na\rsaha
 * *  Revised GetRemoteResp() definition to receive more than 256 bytes of fragmented packets.
 * *
 * *  Revision 1.8  2008/10/22 21:53:30  epm-na\rsaha
 * *  Changed argument "byNumBytesCmd" of type unsigned char to "unNumBytesCmd" of unsigned int type in GetRemoteResp() method to support fragmented data transfer of more than 256 bytes. Added a public member variable to access the number of attempts made to transfer the data. This is for debugging purpose only. Uncomment "RELIABILITY_DEBUG" macro definition in "Reliability.h" file to get this enabled.
 * *
 * *  Revision 1.7  2008/05/22 05:46:28  EPM-NA\kdarji
 * *  - Added timeout parameter in "GetRemoteResp" method, which tells the amount
 * *  of time to wait for response from remote device.
 * *  [Note: Default timeout time is as it was previously i.e. HAL_DFLT_TIMEOUT
 * *  (300 MilliSeconds)]
 * *
 * *  Revision 1.6  2008/03/06 20:47:04  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.5  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.4  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include "Definitions.h"
  
#include "debug.h"
#include "Reliability.h"

CReliability::CReliability(CCANComm *pobCANComm) // Default Constructor
{
  m_pobCANComm = pobCANComm;
  m_nRetryCount = MAX_NO_RETRIES;

  m_nRetryAttempts = 0;
}

CReliability::~CReliability() // Default Destructor
{
}


int CReliability::SetMaxRetries(int nMaxRetries)
{
  if (nMaxRetries <= 0)
  {
    return -1;
  }
  
  m_nRetryCount = nMaxRetries;

  return 0;
}

// Send a command to the remote device and get a response back from it.
int CReliability::GetRemoteResp (unsigned char* pbyCmd, // Data to form command packet
                                 unsigned int unNumBytesCmd,  // Number of bytes in the command packet
                                 unsigned char *pbyRespData,    // Pointer to write device response to
                                 unsigned int unNumBytesResp, // Number of bytes expected from remote device
                                 BOOL bStreamingTx,   // Indicates if we are transmitting streaming data
                                 unsigned int unTimeOut)         // Time to wait for response from remote device
{
  int nRetries = m_nRetryCount;
  int nRetVal = 0;

  if (NULL == m_pobCANComm)
  {
    return ERR_MEMORY_ERR;
  }

  do
  {

    // Send a command and wait for ackowledgement from remote device
    nRetVal = m_pobCANComm->CANGetRemoteResp(pbyCmd,             // Command
                                             unNumBytesCmd,      // Size of command
                                             pbyRespData,        // Response from remote board
                                             unNumBytesResp,     // Size of expected response
                                             bStreamingTx,       // Streaming TX or not
                                             unTimeOut);         // Time to wait for response

    if (nRetVal >= 0)
    {
      break;
    }
    if (nRetries > 0)
    {
      DEBUG2("CReliability::GetRemoteResp() - timed out.");
    }
    nRetries--;
  } while (nRetries > 0);


  m_nRetryAttempts = m_nRetryCount - nRetries;

  return nRetVal;
}


//Returns part of the timeout interval that was not used.
int CReliability::GetRemTimeOut()
{
  DB_INT32 nRemTimeOut = 0;
  if( m_pobCANComm )
  {
    nRemTimeOut = m_pobCANComm->GetRemTimeOut();
  }

  return nRemTimeOut;
}

//Transmit a command (or anything else) without waiting for a response.
int CReliability::TxCmd (unsigned char* pbyCmd,         // Data to form command packet
                         unsigned int unNumBytesCmd,    // Number of bytes in the command packet
                         BOOL bStreamingTx /*= FALSE*/) // Indicates if we are transmitting streaming data
{
  if( m_pobCANComm )
  {
    return m_pobCANComm->CANTxCmd(pbyCmd, unNumBytesCmd, bStreamingTx);
  }
  else
  {
    return ERR_MEMORY_ERR;
  }
}

