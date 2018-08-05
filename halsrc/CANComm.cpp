/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: CANComm.cpp 
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
 * *  $Id: CANComm.cpp,v 1.23 2016/11/16 20:48:01 emrsn\rrthomas Exp $
 * *  $Log: CANComm.cpp,v $
 * *  Revision 1.23  2016/11/16 20:48:01  emrsn\rrthomas
 * *  Includes the following changes/fixes -
 * *  1. Start Rx Pipe (for CAND to communicate with HAL object) naming/numbering starting after 32,768 (or max pid) to avoid conflicts with any other task that used PID number for pipe names.
 * *  2. Fix for potential duplicate Pipe Names when using higher order Function Types and Slot IDs
 * *  3. Use only one TX Pipe fd (for HAL object to send requests/CAN packets to CAND) for all the HAL objects in a process. Earlier, each HAL object created an fd to CAND TX Pipe which really does not serve any purpose.
 * *  4. Clean up commented out code and warning that are not relevant.
 * *
 * *  Revision 1.22  2015/05/04 14:49:25  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.21  2014/06/26 10:00:04  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.20  2013/01/30 06:18:31  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.19  2012/03/21 06:48:45  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.18  2012/01/03 13:08:25  emrsn\rsaha
 * *  Added APIs to support to communicate with IMB.
 * *
 * *  Revision 1.17  2010/02/18 15:13:54  emrsn\rsaha
 * *  Revised to write fragmented packets at one shot to comm. pipe of cand. But for Foundation field bus we don't do it, since FF Card has some limitation on CAN frame receive delay.
 * *  This is done to avoid context switching between application using HAL and cand while sending fragmented packets, since each packet write on pipe invokes higher priority cand task, as it waits for packets on the pipe. The problem was encountered in Comm->cand->BaseIO communication where serial response were taking more time to reach BaseIO at times.
 * *
 * *  Revision 1.16  2009/11/09 13:10:47  epm-na\rsaha
 * *  Updated with API to return non-elapsed timeout interval.
 * *
 * *  Revision 1.15  2009/05/13 23:01:31  epm-na\rsaha
 * *  Introduces a delay of 15 msec between each CAN frames destined to FFB card, else FFB card at times fails to detect new CAN packets.
 * *
 * *  Revision 1.14  2009/05/12 19:14:15  epm-na\rthomas
 * *  Added a function to flush the Stream Pipe.
 * *
 * *  Revision 1.13  2009/04/06 21:26:32  epm-na\rsaha
 * *  Revised to consider CRC bytes while comparing received number of bytes against expected bytes, incase of fragmented packets.
 * *
 * *  Revision 1.12  2008/10/22 21:48:35  epm-na\rsaha
 * *  Added support in CANTxCmmd() method to send fragmented data. To revert back the changes comment out the macro definition of "FRAGMENT_PACKET_H2D". Changed most of the methods second argument "byDataLen" of type unsinged char to "unDataLen" of unsigned int type to support more than 256 bytes of fragmented data exchange.
 * *
 * *  Revision 1.11  2008/05/22 05:34:12  EPM-NA\kdarji
 * *  - Added timeout parameter in "CANGetRemoteResp" method, which tells the amount
 * *  of time to wait for response from remote device.
 * *  [Note: Default timeout time is as it was previously i.e. HAL_DFLT_TIMEOUT
 * *  (300 MilliSeconds)]
 * *
 * *  Revision 1.10  2008/03/06 20:47:03  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.9  2008/01/10 22:13:54  epm-na\lratner
 * *  Modified for updated version of class CIPC.
 * *
 * *  Revision 1.8  2008/01/08 19:52:26  epm-na\lratner
 * *  Moved class CIPC from /platform/HAL/ipc to /platform/ipc.
 * *
 * *  Revision 1.7  2007/08/21 21:25:00  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.6  2007/08/09 07:04:04  EPM-NA\ssatpute
 * *  Added below functions to return receive pipe file descriptor - streaming data.
 * *  1. CANGetRxStrmFd() in CANComm.cpp file
 * *  2. GetRxStreamingFd() in LtLoi.cpp file
 * *
 * *  Revision 1.5  2007/03/13 08:39:39  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *  - Fixed data union variable names used to get / set heater control
 * *    P, I and D gains.
 * *  - Fixed Makefile in halsrc to use the library path.
 * *
 * *  Revision 1.4  2006/12/08 21:32:38  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include <memory.h> // For memset
#include "Definitions.h"
#include "FixEndian.h"
  
#include "debug.h"
#include "CANComm.h"

#define FRAGMENT_PACKET_H2D
#ifdef FRAGMENT_PACKET_H2D
#include "crc16.h"
#endif //#ifdef FRAGMENT_PACKET_H2D

#include <unistd.h>
#define XA_WAIT_TIME_MS_BTN_CAN_FRAMES_IN_MSEC    15

CIPC CCANComm::m_obIPCCmdTx;
int CCANComm::m_nInstances = 0; 

CCANComm::CCANComm() // Default constructor
{
  // Initialize member variables
  m_bIsCmdRespPipeOpen = FALSE;
  m_bIsStreamPipeOpen = FALSE;
  m_bIsStreaming = FALSE;
  m_bySlotID = (unsigned char)-1; // Invalid Address, sure to fail
  m_byFnType = (unsigned char)-1; // Invalid Type, sure to fail
  m_byFnEnum = (unsigned char)-1; // Invalid Enum, sure to fail
  m_nRemTimeOut = 0;

  // Do for first instance ONLY - only one Cmd TX Pipe needed per process to CAND
  if (m_nInstances++ == 0)
  {
    // Open Cmd / TX Pipe
    int nRetVal = m_obIPCCmdTx.IPC_InitIPC (CMD_TX_PIPE_TASK_ID, CMD_TX_PIPE_MAILBOX_ID, IPC_SEND);
    if (nRetVal != 0)
    {
      DEBUG1("CCANComm: Error calling CIPC::IPC_InitIPC. Error code = %d", nRetVal);
    }
  }
}

CCANComm::~CCANComm() // Default destructor
{
  // Close Cmd TX Pipe - only when destroying the last instance
  if (--m_nInstances == 0)
  {
    int nRetVal = m_obIPCCmdTx.IPC_Close();
    if (nRetVal != ERR_SUCCESS)
    {
      DEBUG1("CCANComm::~CCANComm: Cmd TX Pipe - IPC_Close () failed with error code = %d!", nRetVal);
    }
  }

  // Close the pipes, just in case they are open.
  CloseRxPipes();
}

// Open the following pipes for bi-directional communication with CAND
// (1) Command TX Pipe to send commands to CAND and to Transmit CAN packets to 
// remote devices
// (2) Command Response Pipe to read acknowledgements from remote devices
// (3) If bIsStreaming is TRUE, then open a pipe for reading streaming data
// Performs the following functionality - 
// (1) Opens Pipes (For TX, RX and Streaming data)
// (2) Sends command to CAND to register the device
int CCANComm::CANCommOpen (unsigned char bySlotId, 
                           unsigned char byFnType, 
                           unsigned char byFnEnum, 
                           BOOL bIsStreaming /*= FALSE*/)
{
  int nRetVal = ERR_SUCCESS;
  int nCount = 0;
  CANDCmdStruct stRegCmd;
  CANDRespStruct stResp;
  
  // Generate the Task ID for opening the different Pipes.
  // Function Enum - 4 bits, Function Type - 5 bits, Slot ID - 5 bits in CAN Protocol Header
  int nPipeTaskId = (bySlotId << 9) + (byFnType << 4) + byFnEnum;
  nPipeTaskId += 32768; // max_pid = 32768. Offset HAL pipe numbers to ensure that we don't use pipe names 
                        // that uses the pid numbers or enums to generate pipe names (example dbclient).
  
  // Check if the CAN Comm Channel is already open
  if (m_bIsCmdRespPipeOpen == TRUE)
  {
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CCANComm::CANCommOpen: Unexpected sequence! CAN Comm already open for this device!");
  }

  if (nRetVal == ERR_SUCCESS)
  {
    // Initialize to all zeros
    memset (&stRegCmd, 0, sizeof (CANDCmdStruct));
    memset (&stResp, 0, sizeof (CANDRespStruct));

    // Open Cmd Resp Pipe
    if (!m_obIPCCmdRespRx.IPC_InitIPC (nPipeTaskId, CMD_RESP_PIPE_MAILBOX_ID, IPC_RECV))
    {
      m_bIsCmdRespPipeOpen = TRUE;
    }
    else
    {
      DEBUG2("CCANComm::CANCommOpen: Error calling CIPC::IPC_InitIPC.");
    }

    // Open pipe for streaming 
    if (bIsStreaming)
    {
      m_bIsStreaming = bIsStreaming;
      if (!m_obIPCStreamRx.IPC_InitIPC (nPipeTaskId, CMD_STRM_PIPE_MAILBOX_ID, IPC_RECV))
      {
        m_bIsStreamPipeOpen = TRUE;
      }
      else
      {
        DEBUG2("CCANComm: Error calling CIPC::IPC_InitIPC.");
      }
    }

    if (m_bIsCmdRespPipeOpen == FALSE ||
        (bIsStreaming == TRUE && m_bIsStreamPipeOpen == FALSE))
    {
      nRetVal = ERR_INTERNAL_ERR;
    }
    else
    {
      // Format Register command structure
      stRegCmd.CmdType = REGISTER_DATA_CH;  // Command Type
      stRegCmd.CmdData.stRegCmdData.CmdRespIPCid = nPipeTaskId; // Cmd Resp Pipe ID
      if (bIsStreaming)
      {
        stRegCmd.CmdData.stRegCmdData.StreamRespIPCid = nPipeTaskId;  // Streaming Pipe ID
      }
      stRegCmd.CmdData.stRegCmdData.SlotID = bySlotId;  // Slot Address
      stRegCmd.CmdData.stRegCmdData.FnType = byFnType;  // Function Type
      stRegCmd.CmdData.stRegCmdData.FnCount = byFnEnum; // Function Enum
      
      m_bySlotID = bySlotId;
      m_byFnType = byFnType;
      m_byFnEnum = byFnEnum;
                  
      // Register the device with CAND
      nCount = sizeof (CANDCmdStruct);
      if (m_obIPCCmdTx.IPC_SendPacket ((void *) &stRegCmd, nCount) != nCount)
      {
        // Sending registration command failed!
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CCANComm::CANCommOpen: Sending registration command failed!");
      }
    }

    if (nRetVal != ERR_SUCCESS)
    {
      // Close all open pipes (if any), in the event of an error
      CloseRxPipes ();
    }
  }

  return nRetVal;
}

// Close all open pipes, release any resource/memory allocated
int CCANComm::CANCommClose ()
{
  int nRetVal = ERR_SUCCESS;
  int nCount = 0;
  int nPipeTaskId = (int)m_byFnEnum + (int)m_byFnType * 0x10 + (int)m_bySlotID * 0x100;
  CANDCmdStruct stRegCmd;
  CANDRespStruct stResp;

  // Check if the CAN Comm Channel open
  if (m_bIsCmdRespPipeOpen == TRUE)
  {
    // Initialize to all zeros
    memset (&stRegCmd, 0, sizeof (CANDCmdStruct));
    memset (&stResp, 0, sizeof (CANDRespStruct));

    // Format Un-register command structure
    stRegCmd.CmdType = UNREGISTER_DATA_CH;  // Command Type
    stRegCmd.CmdData.stRegCmdData.CmdRespIPCid = nPipeTaskId; // Cmd Resp Pipe ID
    if (m_bIsStreaming)
    {
      stRegCmd.CmdData.stRegCmdData.StreamRespIPCid = nPipeTaskId;  // Streaming Pipe ID
    }
    stRegCmd.CmdData.stRegCmdData.SlotID = m_bySlotID;  // Slot Address
    stRegCmd.CmdData.stRegCmdData.FnType = m_byFnType;  // Function Type
    stRegCmd.CmdData.stRegCmdData.FnCount = m_byFnEnum; // Function Enum

    // Un-register the device from CAND
    nCount = sizeof (CANDCmdStruct);
    if (m_obIPCCmdTx.IPC_SendPacket ((void *) &stRegCmd, nCount) == nCount)
    {
      nRetVal = ERR_SUCCESS;
    }
    else
    {
      // Sending un-register command failed!
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CCANComm::CANCommClose: Sending registration command failed!");
    }

    // Close the pipes
    CloseRxPipes();
  }
  else
  {
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CCANComm::CANCommClose: Unexpected sequence! CAN Comm not open for this device!");

  }

  return nRetVal;
}

// Transmit a CAN command to CAND to transmit over the CAN driver interface
// This function makes a CAN payload packet with remote address (Slot Address, 
// Function type and Function Enumeration) and the data.
// If the CAN Packet size exceeds 8 bytes (the max CAN packet size), then it fragments
// the packet into multiple CAN packets. [FEATURE NOT IN CURRENT IMPLEMENTATION]
// Returns the number of bytes transmitted. 
int CCANComm::CANTxCmd (unsigned char* pbyData, // Pointer to data
                        unsigned int unDataLen,           // Data length
                        BOOL bStreamingTx)
{
  int nRetVal = ERR_SUCCESS;
  int nCount = 0;
  CANDCmdStruct stRegCmd;
  DevAddrUnion DevAddr;

  // Check input pointer
  if (pbyData == NULL)
  {
    nRetVal = ERR_INVALID_ARGS;
    DEBUG2("CCANComm::CANTxCmd: Invalid arguments!");
  }
  
  // Check for invalid data size
#ifdef FRAGMENT_PACKET_H2D
#else //FRAGMENT_PACKET_H2D
  if (unDataLen > CAN_PKT_DATA_LEN)
  {
    nRetVal = ERR_NOT_IMPLEMENTED;
    DEBUG2("CCANComm::CANTxCmd: Unexpected data size! Data size = %d!", unDataLen);
  }
#endif //FRAGMENT_PACKET_H2D

  if (nRetVal == ERR_SUCCESS)
  {
    // Initialize to all zeros
    memset (&stRegCmd, 0, sizeof (CANDCmdStruct));
    memset (&DevAddr, 0, sizeof (DevAddrUnion));

    // Command Type - TX
    stRegCmd.CmdType = TX_CAN_DATA;

    // Slot ID / CAN Msg ID to send CAN Packet to.
    stRegCmd.CmdData.stTxData.CANId = m_bySlotID;

    if (TRUE == bStreamingTx)
    {
      SetSlotID(&DevAddr.usDevAd, 1);
    }

    SetFnType(&DevAddr.usDevAd, m_byFnType);
    SetFnCount(&DevAddr.usDevAd, m_byFnEnum);

#ifdef FRAGMENT_PACKET_H2D

    DevAddrUnion CopyDevAddr = DevAddr;

    //Get the CRC code if the data to be fragmented and send.
    unsigned short usCRC = 0;
    unsigned int unbyNumPkts = 1;
    if( unDataLen > CAN_PKT_DATA_LEN )
    {
      unbyNumPkts = (unDataLen + sizeof(usCRC)) / CAN_PKT_DATA_LEN ;
      if( (unDataLen + sizeof(usCRC)) % CAN_PKT_DATA_LEN )
      {
        unbyNumPkts++;
      }

      usCRC = crc16( (unsigned char*)pbyData, unDataLen );

      FixEndian( usCRC );
    }


    //Send all the Data packets.
    unsigned int unBytesCopied = 0;
    bool bCRC1Copied = false;

    unsigned char byTxData[4096]={0};
    int nTxDataIndex = 0;

    for( unsigned int unCount = 0; (unCount < unbyNumPkts) && (nRetVal != ERR_INTERNAL_ERR); unCount++ )
    {
      unsigned int unBytes2Copy = 0;
      unBytes2Copy = ((unDataLen-unBytesCopied) < CAN_PKT_DATA_LEN) ? (unDataLen-unBytesCopied) : CAN_PKT_DATA_LEN;   
      // Format the rest of the CAN Packet (data payload) - Copy the "unBytes2Copy" bytes
      memcpy ((void*)(stRegCmd.CmdData.stTxData.PktData + sizeof (DevAddrUnion)), 
              (void *)(pbyData + unCount * CAN_PKT_DATA_LEN), unBytes2Copy);

      
      unsigned int unBytesInPacket = unBytes2Copy;  

      //If its the last packet.
      if( unCount == unbyNumPkts - 1 )
      {
        DevAddr = CopyDevAddr;
        SetFragment( &DevAddr.usDevAd, 0 );
        FixEndian( DevAddr.usDevAd );

        // Format the first two bytes (address) in the Remote CAN Packet
        memcpy ((void *)stRegCmd.CmdData.stTxData.PktData, (void *)&DevAddr.usDevAd, sizeof (DevAddrUnion));

        //If it is fragmented data copy the CRC value.
        if( unbyNumPkts > 1 )
        {
          if( bCRC1Copied )
          {
            // Copy the CRC[1] byte
            memcpy ((void*)(stRegCmd.CmdData.stTxData.PktData + sizeof (DevAddrUnion) + unBytes2Copy), 
                    (unsigned char *)&usCRC+1, 1 );
            unBytesInPacket += 1;
          }
          else
          {
            // Copy the CRC
            memcpy ((void*)(stRegCmd.CmdData.stTxData.PktData + sizeof (DevAddrUnion) + unBytes2Copy), 
                    (void *)&usCRC, sizeof(usCRC) );
            unBytesInPacket += 2;
          }
        }
      }
      else
      {
        DevAddr = CopyDevAddr;
        SetFragment( &DevAddr.usDevAd, 1 );
        FixEndian( DevAddr.usDevAd );

        // Format the first two bytes (address) in the Remote CAN Packet
        memcpy ((void *)stRegCmd.CmdData.stTxData.PktData, (void *)&DevAddr.usDevAd, sizeof (DevAddrUnion));

        if( unBytes2Copy == CAN_PKT_DATA_LEN - 1 )
        {
          // Copy the CRC[0] byte
          memcpy ((void*)(stRegCmd.CmdData.stTxData.PktData + sizeof (DevAddrUnion) + unBytes2Copy), 
                  (void *)&usCRC, 1 );
          bCRC1Copied = true;
          unBytesInPacket += 1;
        }
      }
      unBytesCopied += unBytes2Copy;
      nCount = sizeof (CANDCmdStruct);
      // Length of CAN Payload
      stRegCmd.CmdData.stTxData.PktLen = unBytesInPacket + sizeof (DevAddrUnion);

      if( FN_FFB_STATUS == m_byFnType )
      {
        if (m_obIPCCmdTx.IPC_SendPacket ((void *) &stRegCmd, nCount) != nCount)
        {
          // Sending Tx command failed!
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG2("CCANComm::CANTxCmd: Error sending TX command!");
        }

        //While sending fragmented packets to FFB board, wait for 
        //"XA_WAIT_TIME_MS_BTN_CAN_FRAMES_IN_MSEC" amount of time between CAN frames.
        //This interval is to make sure FFB Board doesn't overlook a new frame.
        usleep( XA_WAIT_TIME_MS_BTN_CAN_FRAMES_IN_MSEC * 1000 );
      }
      else
      {
        memcpy( &byTxData[nTxDataIndex], &stRegCmd, nCount );
        nTxDataIndex += nCount;
      }
    }

    if( FN_FFB_STATUS != m_byFnType )
    {
      if (m_obIPCCmdTx.IPC_SendPacket ((void *) byTxData, nTxDataIndex) != nTxDataIndex)
      {
        // Sending Tx command failed!
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CCANComm::CANTxCmd: Error sending TX command!");
      }
    }

#else //FRAGMENT_PACKET_H2D

    //Take care of Endianness
    FixEndian(DevAddr.usDevAd);

    // Format the first two bytes (address) in the Remote CAN Packet
    memcpy ((void *)stRegCmd.CmdData.stTxData.PktData, (void *)&DevAddr.usDevAd, sizeof (DevAddrUnion));

    // Format the rest of the CAN Packet (data payload) - Copy the "unDataLen" bytes
    memcpy ((void*)(stRegCmd.CmdData.stTxData.PktData + sizeof (DevAddrUnion)), (void *)pbyData, unDataLen);

    // Length of CAN Payload
    stRegCmd.CmdData.stTxData.PktLen = unDataLen + sizeof (DevAddrUnion);
        
    nCount = sizeof (CANDCmdStruct);
    if (m_obIPCCmdTx.IPC_SendPacket ((void *) &stRegCmd, nCount) != nCount)
    {
      // Sending Tx command failed!
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CCANComm::CANTxCmd: Error sending TX command!");
    }

#endif //FRAGMENT_PACKET_H2D
  }

  return nRetVal;
}

// Read Command Response from remote device. This is a blocking call
// This functions strips away the remote devices address from the CAN payload and 
// returns only the data back to the calling function.
// Function returns the number of bytes read on success, negative error code 
// on error.
int CCANComm::CANRxCmdRespBlocking (unsigned char* pbyData, // Pointer to write data to
                                    unsigned int unDataLen)  // Number of bytes to read
{
  return RxData (pbyData, unDataLen, FALSE, NULL);
}

// Read Command Response from remote device. The user can specify a time out in 
// milli-seconds.
// This functions strips away the remote devices address from the CAN payload and 
// returns only the data back to the calling function.
// Function returns the number of bytes read on success, negative error code 
// on error.
int CCANComm::CANRxCmdRespTimeout (unsigned char* pbyData, // Pointer to write data to
                                   unsigned int unDataLen,        // Number of bytes to read  
                                   unsigned int unTimeOut/* = 1000*/)   // Timeout in milli-seconds 
{
  return RxData (pbyData, unDataLen, FALSE, &unTimeOut);
}

// Read streaming data from remote device. This is a blocking call.
// This functions strips away the remote devices address from the CAN payload and 
// returns only the data back to the calling function.
// Function returns the number of bytes read on success, negative error code 
// on error.
int CCANComm::CANRxStrmBlocking (unsigned char* pbyData,  // Pointer to write data to
                                 unsigned int unDataLen)  // Number of bytes to read
{
  return RxData (pbyData, unDataLen, TRUE, NULL);
}

// Read streaming data from remote device. The user can specify a time out in 
// milli-seconds. 
// This functions strips away the remote devices address from the CAN payload and 
// returns only the data back to the calling function.
// Function returns the number of bytes read on success, negative error code 
// on error.
int CCANComm::CANRxStrmTimeout (unsigned char* pbyData, // Pointer to write data to
                                unsigned int unDataLen,   // Number of bytes to read
                                unsigned int unTimeOut/* = 1000*/,  // Timeout in milli-seconds
                                unsigned int *punRemTimeout /* = NULL */)
{
  int nRetVal = ERR_SUCCESS;

  nRetVal = RxData (pbyData, unDataLen, TRUE, &unTimeOut);

  //Store the remaining time out of the specified timeout value
  if (punRemTimeout)
  {
    *punRemTimeout = unTimeOut;
  }

  return nRetVal;
}

int CCANComm::CANFlushCmdRespPipe ()
{
  int nRetVal = ERR_SUCCESS;
  int nErrorCode = 0;

  // Check if the Pipe is open before trying to flush it
  if (m_bIsCmdRespPipeOpen)
  {
    // Flush the pipe
    nErrorCode = m_obIPCCmdRespRx.IPC_Flush();
    if (nErrorCode < 0)
    {
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CCANComm::CANFlushCmdRespPipe: Cmd Resp Pipe - IPC_Flush () failed with error code = %d!", nErrorCode);
    }
  }
  else
  {
    // Unexpected sequence! Pipe not open for flushing!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CCANComm::CANFlushCmdRespPipe: Unexpected sequence! Pipe not open for flushing!");
  }

  return nRetVal;
}

// Close all the open pipes.
int CCANComm::CloseRxPipes()
{
  int nRetVal = ERR_SUCCESS;

  // Close Cmd Resp Pipe
  if (m_bIsCmdRespPipeOpen)
  {
    m_obIPCCmdRespRx.IPC_Close();
    if (nRetVal != ERR_SUCCESS)
    {
      DEBUG2("CCANComm::CloseRxPipes: Cmd Resp Pipe - IPC_Close () failed with error code = %d!", nRetVal);
    }
    m_bIsCmdRespPipeOpen = FALSE;
  }

  // Close Stream Pipe
  if (m_bIsStreamPipeOpen)
  {
    m_obIPCStreamRx.IPC_Close();
    if (nRetVal != ERR_SUCCESS)
    {
      DEBUG2("CCANComm::CloseRxPipes: Stream Pipe - IPC_Close () failed with error code = %d!", nRetVal);
    }
    m_bIsStreamPipeOpen = FALSE;
  }

  return ERR_SUCCESS;
}

int CCANComm::RxData (unsigned char* pbyData, // Pointer to write data to
                      unsigned int unDataLen, // Number of bytes to read
                      BOOL bStrmPipe,         // Which pipe to read from. If bStrmPipe = TRUE -> Stream Pipe, else CmdRespPipe
                      unsigned int* punTimeout) // Timeout - NULL -> Blocking Rx, *punTimeout -> Non-blocking with timeout
{
  int nRetVal = ERR_SUCCESS;
  int nCount = 0;
  CANDRespStruct stResp;
  int nBytesRxd = 0;
  INT32 nRemTimeout;// = *punTimeout;//Using the same datatype as in IPC library
  m_nRemTimeOut = 0;

  CFragment *pobFragment;
  BOOL bLoopExit = FALSE;

  // Check if Comm channel is open
  if (bStrmPipe == TRUE && m_bIsStreamPipeOpen == FALSE ||
      m_bIsCmdRespPipeOpen == FALSE)
  {
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CCANComm::RxData: Unexpected sequence! CAN Comm not opened for this device!");
  }


  if (nRetVal == ERR_SUCCESS)
  {
    nCount = sizeof (CANDRespStruct);
    
    //In case of a read with timeout, use the user specified timeout
    if (punTimeout)
    {
      nRemTimeout = *punTimeout;
    }
    //In case of a blocking read, set this value to 1 so that we wait till
    //  we get at least the user requested number of bytes
    else
    {
      nRemTimeout = 1;
    }
  
    // Loop till timeout or till fragmented packet is completely received
    while ( !(bLoopExit) )
    {
      memset (&stResp, 0, nCount);
    
      if (bStrmPipe)
      {
        if (punTimeout == NULL) // Blocking receive
        {
          nBytesRxd = m_obIPCStreamRx.IPC_RecvPacketBlocking ((void*)&stResp, nCount);
        }
        else  // Receive with Timeout
        {
          nBytesRxd = m_obIPCStreamRx.IPC_RecvPacketTimeout ((void*)&stResp, nCount, nRemTimeout, &nRemTimeout);
        }
      }
      else
      {
        if (punTimeout == NULL) // Blocking receive
        {
          nBytesRxd = m_obIPCCmdRespRx.IPC_RecvPacketBlocking ((void*)&stResp, nCount);
        }
        else  // Receive with Timeout
        {
          nBytesRxd = m_obIPCCmdRespRx.IPC_RecvPacketTimeout ((void*)&stResp, nCount, nRemTimeout, &nRemTimeout);
        }
      }

      if (nBytesRxd == nCount)
      {
        switch (stResp.RespType)
        {
        case RESP_PACKET:
        case STREAM_DATA:
          //Put received data into the fragmentation layer
          if (RESP_PACKET == stResp.RespType)
          {
            pobFragment = &m_obCmdRespFrag;
          }
          else
          {
            pobFragment = &m_obStrmRespFrag;
          }
            
          if ( (nRetVal = pobFragment->AddData(stResp.stRespData.stRxData.PktData, 
                                               stResp.stRespData.stRxData.PktLen)) < 0)
          {
            DEBUG2("CDataFragment::AddData() failed with: %d", nRetVal);
            bLoopExit = TRUE;
            break;
          }
          
          // Get and check if the complete fragment has been received
          nRetVal = pobFragment->GetData(pbyData, unDataLen);
          
          //Fragmented packet not completely received....
          if (ERR_DATA_PENDING == nRetVal)
          {
            //If we received more than the user requested number
            //  of bytes, but we still haven't received the last
            //  packet of the fragment, then it's an error
            //Since collecting fragmented packets, consider the
            //CRC bytes( 2 ) also.
            if ( (unsigned int) pobFragment->GetCount() > unDataLen + 2 /*CRC Bytes*/ )
            {
              bLoopExit = TRUE;
              nRetVal = ERR_PROTOCOL;
            }
            //Check for timeout... continue if the timeout has not expired
            else if (nRemTimeout > 0)
            {
              continue;
            }
            //Return with timeout error
            else
            {
              bLoopExit = TRUE;
              nRetVal = ERR_TIMEOUT;
              break;
            }
          }
          //Error occurred
          else if (nRetVal < 0)
          {

            bLoopExit = TRUE;
            break;
          }
          //Fragment received...
          else
          {
            if ( (unsigned int) nRetVal != unDataLen)
            {
              DEBUG2("CCANComm::RxData: Expected data size: %d, Received data size: %d", unDataLen, nRetVal);
            }

            //Store the remaining time out of the specified timeout
            if (punTimeout)
            {
              *punTimeout = nRemTimeout;
            }

            bLoopExit = TRUE;
          }
        
          break;

        default:
          bLoopExit = TRUE;
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG2("CCANComm::RxData: Invalid switch case.");
          break;
        }
      }
      else
      {
        bLoopExit = TRUE;
        // Error while receiving response command!
        nRetVal = ERR_TIMEOUT;
        DEBUG2("CCANComm::RxData: Error while receiving response command!");
      }
    } // while ( !(bLoopExit) )


    m_nRemTimeOut = nRemTimeout;
  }

  return nRetVal;
}

// Send a command to the remote device and get a response back from it.
int CCANComm::CANGetRemoteResp (unsigned char* pbyCmd,  // Data to form command packet
                                unsigned int unNumBytesCmd,     // Number of bytes in the command packet
                                unsigned char *pbyRespData,     // Pointer to write device response to
                                unsigned int unNumBytesResp,    // Number of bytes expected from remote device
                                BOOL bStreamingTx,          // Indicates if we are transmitting streaming data
                                unsigned int unTimeOut)             // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  // Check input pointers
  if (pbyCmd == NULL || 
      pbyRespData == NULL)
  {
    nRetVal = ERR_INVALID_ARGS;
    DEBUG2("CCANComm::CANTxCmd: Invalid arguments!");
  }

#ifdef FRAGMENT_PACKET_H2D
#else //FRAGMENT_PACKET_H2D
  // Check CAN command payload length
  if (unNumBytesCmd > CAN_PKT_DATA_LEN)
  {
    nRetVal = ERR_NOT_IMPLEMENTED;
    DEBUG2("CCANComm::GetRemoteResp: TX: Unexpected data size! Data size = %d!", unNumBytesCmd);
  }
#endif

#warning "FLUSHING RX PIPE BEFORE RECEIVING A RESPONSE - TESTING"
  m_obIPCCmdRespRx.IPC_Flush();

#warning "FLUSHING CMD RESP PACKET DE-FRAGMENTER"
  m_obCmdRespFrag.Flush();

  // If no errors, then send out the command
  if (!nRetVal)
  {
    nRetVal = CANTxCmd (pbyCmd, unNumBytesCmd, bStreamingTx);
  }

  // If no errors, then read the remote device's response
  if (!nRetVal)
  {
    nRetVal = CANRxCmdRespTimeout (pbyRespData, unNumBytesResp, unTimeOut);
  }

  return nRetVal;
  
}

//Return Receive Pipe File Descriptor - Streaming data from remote board
int CCANComm::CANGetRxStrmFd()
{ 
  return m_obIPCStreamRx.GetFd();
}

int CCANComm::CANFlushStreamPipe ()
{
  int nRetVal = ERR_SUCCESS;
  int nErrorCode = 0;

  // Check if the Pipe is open before trying to flush it
  if (m_bIsStreamPipeOpen )
  {
    // Flush the pipe
    nErrorCode = m_obIPCStreamRx.IPC_Flush();
    if (nErrorCode < 0)
    {
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CCANComm::CANFlushStreamPipe: Streaming Pipe - IPC_Flush () failed with error code = %d!", nErrorCode);
    }
  }
  else
  {
    // Unexpected sequence! Pipe not open for flushing!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CCANComm::CANFlushStreamPipe: Unexpected sequence! Pipe not open for flushing!");
  }

  return nRetVal;
}

//Returns part of the timeout interval that was not used.
int CCANComm::GetRemTimeOut()
{
  return m_nRemTimeOut;
}


