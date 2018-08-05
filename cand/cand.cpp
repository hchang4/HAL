/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: cand.cpp
 * *
 * *  Description: CAN dameon. All communication to the CAN driver is 
 * *               routed through this daemon, using IPC channels.
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
 * *  $Id: cand.cpp,v 1.24 2015/05/04 14:32:10 EMRSN\sboyidi Exp $
 * *  $Log: cand.cpp,v $
 * *  Revision 1.24  2015/05/04 14:32:10  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.23  2014/06/26 09:59:39  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.22  2013/07/11 19:56:39  emrsn\rrthomas
 * *  Changed how CAND stores and searches "device registration" information. Earlier, we had a std::list of registration entries. Changed it into a 3D array. The std::list is memory efficient, but routing an incoming CAN packet to the correct receipient (routing) requires a linear search in the std::list. Amount of time spent looking up routing entries depends upon where the registration entry is present in the std::list. For example, the Preamp Streaming entries are added last, so each time a packet comes in (which is once every 20 ms), we have to search through a list of around 50 entries to find the routing information. With a 3D array, we can use Slot ID, Fn Type and Fn Count to find the routing information without a linear search. Quick testing indicates that the new scheme uses 98% less time doing routing lookup.
 * *
 * *  Also took out all the BCI CAN Driver code (that we used for the 700XA Alphas on AMD based Linux Boxes); we don't use it anymore and it just clutters the code.
 * *
 * *  Revision 1.21  2012/09/18 13:22:39  emrsn\rsaha
 * *  Revised to not process CPU intensive the debug log messages if no verbosity.
 * *
 * *  Revision 1.20  2012/03/21 06:48:40  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.19  2008/11/11 01:18:00  epm-na\rsaha
 * *  Revised not to send acknowledgements to FFB board remote request packets. This requests will be acknowledged by application itself.
 * *
 * *  Revision 1.18  2008/03/08 00:48:08  epm-na\rthomas
 * *  Minor changes while printing debug info...
 * *
 * *  Revision 1.17  2008/03/07 16:40:17  epm-na\rthomas
 * *  Includes the following changes -
 * *
 * *  1. Fix for frequent timeout errors in HAL when you execute a HAL command (like turn ON / OFF solenoid, start / stop broadcast). Cause of the problem - CAND tries to SysLog errors such as unsolicited device responses (from HW boards) and HAL layer trying to talk to devices that were not registered with cand.  All SysLog's require a write to the GC Database. Sys Log messages are stored in the flash, and hence the SysLog function call can cause CAND to get blocked and not return for several seconds (or at the least several 100s of milliseconds)! This can cause any concurrent requests from HAL to timeout!!!
 * *  TODO: Need to have a mechanism to log these messages! CAND is the gateway to the H/W and an error message from cand needs to get logged and analyzed
 * *  2. Added some code to profile the amount of time it takes to fire a solenoid. This needs to be turned off after we are done profiling.
 * *
 * *  Revision 1.16  2008/02/29 00:45:33  epm-na\rthomas
 * *  Added more debug information for SysLog text when CAN packets are received from hardware boards, and no HAL object has registered with CAND to receive these messages.
 * *
 * *  Revision 1.15  2008/01/10 22:12:57  epm-na\lratner
 * *  Modified for updated version of class CIPC.
 * *
 * *  Revision 1.14  2008/01/08 19:51:38  epm-na\lratner
 * *  Moved class CIPC from /platform/HAL/ipc to /platform/ipc.
 * *
 * *  Revision 1.13  2007/08/13 04:28:15  EPM-NA\ssatpute
 * *  Added code to run cand with priority.
 * *
 * *  Revision 1.12  2007/01/30 13:39:26  EPM-NA\ssatpute
 * *  Added missing global.h
 * *
 * *  Revision 1.11  2006/12/14 00:11:50  epm-na\DPrabhakar
 * *  Fixed a bug when handling the -v option (the app needs to ignore that
 * *  option; was not doing it correctly, which was causing the app to exit
 * *  when using the -v option)
 * *
 * *  Revision 1.10  2006/12/13 22:36:28  epm-na\DPrabhakar
 * *  - Modified code to use the SysLog() API (logLib) to log error messsges
 * *  - Modified code to use the DEBUGx() API to print error messages
 * *  - Will now unregister the device if the write to that device channel's
 * *    FIFO fails (streaming or command/response).
 * *
 * *  Revision 1.9  2006/12/08 21:24:40  epm-na\DPrabhakar
 * *  Added Daniel file headers.
 * *  Added README file containing application execution information.
 * *
 * *
 * *************************************************************************/




#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <getopt.h>
#include <global.h>
#include <runAsRTTask.h>
#include "FixEndian.h"
#include "cand.h"
#include "loglib.h"
#include "debug.h"

using namespace::std;

//#define CAND_DEBUG_EN

#ifdef CAND_DEBUG_EN
#define DEBUG_CAND  DEBUG3
#else
#define DEBUG_CAND(...)
#endif


//Define this macro to test for failures...
//  (i.e. defining this macro will make the code
//  discard every 'n'th packet that it receives.
//  This will test the retry mechanism on the
//  device side.)
//#define TEST_FAILURE

#ifdef TEST_FAILURE

#define TX_RAND_FAIL  500
#define RX_RAND_FAIL  500

#include <signal.h>
unsigned int g_unExitApp = 0;

unsigned long g_ulTxDiscPkts = 0;
unsigned long g_ulRxDiscPkts = 0;

//Signal handler for SIGINT
void SIGINTHandler(int sigNum)
{
  g_unExitApp = 1;
}

#endif



#ifdef CANDLOG_EN

#include <signal.h>
unsigned int g_ucDumpLog = 0;

//Signal handler for SIGUSR
void SIGUSR1Handler(int sigNum)
{
  g_ucDumpLog = 1;
}

#endif
  


  
///// CONSTRUCTERS/DESTRUCTORS/////

//Default constructer
CCAND::CCAND()
{   
  m_fdCANDrvRx = 0;
  m_fdCANDrvTx = 0;
  memset (ltRegList, 0, sizeof (ltRegList));
}

//Default destructor
CCAND::~CCAND()
{       
}       
    
///// PUBLIC FUNCTIONS /////
    
//The CAND handler - the main function
int CCAND::CANDHandler(char *pDevPath)
{           
  int nRetVal = 0;
  fd_set readFDs;
  struct timeval tvSelect;
  int selectN = 0;
  
  DEBUG_CAND("**** %s ****", __FUNCTION__);

  if (InitDriver(pDevPath) < 0)
  {
    LogError(CAND_ERR_DRV_OPEN, __LINE__);
    CANDClose();
    return -1;
  }

  if (InitCmdIPC() < 0)
  {
    LogError(CAND_ERR_IPC_CMD_RX, __LINE__);
    CANDClose();
    return -1;
  }


  FD_ZERO(&readFDs);
  
  int fdCmdRx = m_ipcCmdRx.GetFd();
  //Set this to the largest of m_fdCANDrvRx and the command IPC fd, plus 1
  selectN = m_fdCANDrvRx > fdCmdRx ? m_fdCANDrvRx : fdCmdRx;
  selectN++;

  DEBUG_CAND("Entering while(1)...");
  while (1)
  {
    FD_SET(m_fdCANDrvRx, &readFDs);
    FD_SET(fdCmdRx, &readFDs);
    tvSelect.tv_sec = SELECT_TIMEOUT_SEC;
    tvSelect.tv_usec = SELECT_TIMEOUT_USEC;

    //Do a select to check if there's data on either of the FDs...
    //TODO - decide whether we want to wait indefinitely for at least
    //  one of the FDs to be active...

    if (select(selectN, &readFDs, NULL, NULL, NULL) < 0)
    {
      LogError(CAND_ERR_SELECT, __LINE__);
    }
    else
    {
      //Check if the CAN driver receive FD is 'active'
      if (FD_ISSET(m_fdCANDrvRx, &readFDs))
      {
        //Receive and process the data
        HandleCANReceive();
      }

      //Check if the command IPC FD is 'active'
      if (FD_ISSET(m_ipcCmdRx.GetFd(), &readFDs))
      {
        //Receive and process the command
        HandleTopLevelCmds();
      }
    }

#ifdef TEST_FAILURE
    if (g_unExitApp)
    {
      nRetVal = 0;
      break;
    }
#endif

  }



  return nRetVal;
}

//Release resources and close driver & all IPC channels
int CCAND::CANDClose()
{
  int nRetVal = 0;

  DEBUG_CAND("**** %s ****", __FUNCTION__);

  for (unsigned char SlotID = 0; SlotID <= 0x1F; ++SlotID)
    for (unsigned char FnType = 0; FnType <= 0x1F; ++FnType)
      for (unsigned char FnCount = 0; FnCount <= 0xF; ++FnCount)
        if ( AlreadyRegistered (SlotID, FnType, FnCount) )
          DeRegister(SlotID, FnType, FnCount);

  close(m_fdCANDrvRx);
  close(m_fdCANDrvTx);

  return nRetVal;
}


///// PRIVATE FUNCTIONS /////

//Initialize driver
int CCAND::InitDriver(char *pDevPath)
{
  DEBUG_CAND("**** %s ****", __FUNCTION__);

  int nRetVal = 0;

  //Return with error if the device path is not specified
  if (NULL == pDevPath)
  {
    nRetVal = -1;
  }

  //Open driver in READ mode
  if (0 == nRetVal)
  {
    //For Receive: Open the CAN device in READ only, Non-blocking mode
    m_fdCANDrvRx = open(pDevPath, O_RDONLY | O_NONBLOCK);

    //Return with error if open fails...
    if (m_fdCANDrvRx < 0)
    {
      nRetVal = -1;
    }
  }

  //Open driver in WRITE mode
  if (0 == nRetVal)
  {
    //For Transmit: Open the CAN device in WRITE only, blocking mode
    m_fdCANDrvTx = open(pDevPath, O_WRONLY);

    //Return with error if open fails...
    if (m_fdCANDrvTx < 0)
    {
      nRetVal = -1;
    }
  }

  //If something failed above, close any open file descriptors
  //  and return with error
  if (nRetVal < 0)
  {
    //Assuming that a device file descriptor will not be zero - TODO - check
    if (m_fdCANDrvRx > 0)
    {
      close(m_fdCANDrvRx);
      m_fdCANDrvRx = 0;
    }

    if (m_fdCANDrvTx > 0)
    {
      close(m_fdCANDrvTx);
      m_fdCANDrvTx = 0;
    }
  }

  return nRetVal;
}

//Initialize the command IPC - RX only
int CCAND::InitCmdIPC()
{
  int nRetVal = 0;

  DEBUG_CAND("**** %s ****", __FUNCTION__);

  if (m_ipcCmdRx.IPC_InitIPC(CMD_TX_PIPE_TASK_ID,
                             CMD_TX_PIPE_MAILBOX_ID, IPC_RECV) < 0)
  {
    nRetVal = -1;
  }

  return nRetVal;
}

//Handle data from the CAN driver
int CCAND::HandleCANReceive()
{
  int nRetVal = 0;
  CANDRespStruct stCANData;
  DevAddrUnion stHostToDev;
  DevAddrUnion stDevToHost;
  unsigned char ucAckPacket[CAN_PKT_MAX_LEN + 2];
  CANDRegInfo *pEntry = NULL;
  unsigned char SlotID, FnType, FnCount;

  DEBUG_CAND("**** %s ****", __FUNCTION__);

  //Read data from the driver
  nRetVal = read(m_fdCANDrvRx, stCANData.stRespData.stRxData.PktData,
                 CAN_PKT_MAX_LEN);

  if (nRetVal < 0)
  {
    LogError(CAND_ERR_DRV_RX, __LINE__);
  }
  else if (0 == nRetVal)
  {
    //TODO -
  }
  else
  {

#ifdef CAND_DEBUG_EN
    if( DebugLevel > 0 )
    {
      char szMessage[60] = "RX: ";
      char szByte[5];

      for (int dCnt = 0; dCnt < nRetVal; dCnt++)
      {
        sprintf (szByte, "0x%02x ", stCANData.stRespData.stRxData.PktData[dCnt]);
        strcat (szMessage, szByte);
      }
      DEBUG2(szMessage);
    }
#endif


#ifdef TEST_FAILURE
    if ( !( (1 + rand()) % RX_RAND_FAIL) )
    {
      g_ulRxDiscPkts++;
      DEBUG_CAND("CAND: RX pkt discarded.");
      return 0;
    }
#endif


    //Populate the data length field
    stCANData.stRespData.stRxData.PktLen = nRetVal;

    //Extract packet identification information (for top layer)
    memcpy(&stDevToHost.usDevAd, stCANData.stRespData.stRxData.PktData,
           sizeof(DevAddrUnion));

    //Take care of Endianness
    FixEndian(stDevToHost.usDevAd);

    //  This information is required to check if the board is
    //  registered or not
    SlotID = GetSlotID(&stDevToHost.usDevAd);
    FnType = GetFnType(&stDevToHost.usDevAd);
    FnCount = GetFnCount(&stDevToHost.usDevAd);
    
    DEBUG_CAND("Dev SL: %d, FT: %d, FC: %d", SlotID, FnType, FnCount);

#ifdef CANDLOG_EN
    CANDLogInfo stLogInfo;
    stLogInfo.ucDir = 0;
    stLogInfo.ucSlId = SlotID;
    stLogInfo.ucFn = FnType;
    stLogInfo.ucFnCnt = FnCount;
    memcpy(stLogInfo.ucData, &stCANData.stRespData.stRxData.PktData[2], 
           stCANData.stRespData.stRxData.PktLen - 2);
    stLogInfo.ucDataLen = stCANData.stRespData.stRxData.PktLen - 2;
#endif //CANDLOG_EN


    //Do not acknowledge FFB Remote Request packets comming from FFB board.
    //Application will send response to this request.
    if( FN_FFB_COMMAND != FnType )
    {
      //Prepare and send ACK
      //The first 2 bytes should contain the CAN address (for the driver),
      //  which is the slot ID
      ucAckPacket[0] = (SlotID & 0xFF00) << 8;
      ucAckPacket[1] = (SlotID & 0x00FF);

      //For the ACK packet header
      SetSlotID(&stHostToDev.usDevAd, SlotID);
      SetFnType(&stHostToDev.usDevAd, FnType);
      SetFnCount(&stHostToDev.usDevAd, FnCount);
    
      //Response is not a fragment
      SetFragment(&stHostToDev.usDevAd, 0);

      //Set this to acknowledge the packet
      SetDatatype(&stHostToDev.usDevAd, 1); //TODO - macros for this??

      //Take care of Endianness
      FixEndian(stHostToDev.usDevAd);

      //Copy the packet header as the first 2 bytes of the data packet
      memcpy(&ucAckPacket[2], &stHostToDev,
             sizeof(DevAddrUnion));


      //This is a BLOCKING write call!
      //Return doesn't indicate a successful data transmission - just
      //  indicates that the data was queued up in the driver, pending
      //  transmission.
      nRetVal = write(m_fdCANDrvTx, ucAckPacket, CAN_ACK_PACKET_LEN);

      //Error!
      if (nRetVal < 0)
      {
        LogError(CAND_ERR_DRV_TX_ACK, __LINE__);
      }
      //Short write to the driver!
      else if (nRetVal != CAN_ACK_PACKET_LEN)
      {
        nRetVal = -1;
        LogError(CAND_ERR_DRV_TX_ACK_SHORT, __LINE__);
      }
    }

    //Extract the corresponding registration information in the list
    if ( (pEntry = GetMatchingEntry(SlotID, FnType, FnCount)) != NULL)
    {
      //Entry found - send data to upper layer!

      //TODO - macros???
      //Streaming data - use the streaming IPC
      if (1 == GetDatatype(&stDevToHost.usDevAd))
      {
        //Check if a stream IPC has been registered
        if (pEntry->m_streamRespIPC)
        {
          stCANData.RespType = STREAM_DATA;
          //Send streaming data over the streaming IPC
          if (pEntry->m_streamRespIPC->IPC_SendPacket(&stCANData,
                                                      sizeof(CANDRespStruct)) < 0)
          {
            nRetVal = -1;
            LogError(CAND_ERR_IPC_TX_STREAM, __LINE__);

            //If write to the IPC fails, un-register this device
            if (DeRegister(SlotID, FnType, FnCount) < 0)
            {
              LogError(CAND_ERR_DEREG_CHANNEL, __LINE__);
            }
          }
        }
        else
        {
          LogError(CAND_ERR_IPC_TX_STREAM_NOT_REG, __LINE__);
        }
      }
      //Response to a command - use the command response IPC
      else
      {
#ifdef CANDLOG_EN
        if (g_ucDumpLog)
        {
          g_ucDumpLog = 0;
          obCANDLog.DumpLogToFile();
        }
        
        obCANDLog.AddCmdRespToLog(stLogInfo);
#endif //CANDLOG_EN
        stCANData.RespType = RESP_PACKET;
        //Response to a command - send it through the command response IPC
        if (pEntry->m_cmdRespIPC->IPC_SendPacket(&stCANData,
                                                 sizeof(CANDRespStruct)) < 0)
        {
          nRetVal = -1;
          LogError(CAND_ERR_IPC_TX_RESP, __LINE__);
          
          //If write to the IPC fails, un-register this device
          if (DeRegister(SlotID, FnType, FnCount) < 0)
          {
            LogError(CAND_ERR_DEREG_CHANNEL, __LINE__);
          }
        }
      }
    }
    //Entry not found!
    else
    {
      //Entry not found! - not a registered board
      DEBUG1 ("CAND_ELOG: Channel has not been registered. Slot ID = %d, FnType = %d, FnCount = %d", 
              SlotID, FnType, FnCount);
    }
  }

  return nRetVal;
}


//Handle commends from the higher level
int CCAND::HandleTopLevelCmds()
{
  int nRetVal = 0;
  CANDCmdStruct stCmdInfo;
  //2 bytes of address + max 8 bytes of data
  unsigned char ucDataPacket[CAN_PKT_MAX_LEN + 2];
  //Length of the packet for the driver write function
  int nDrvPakLen = 0;

  DEBUG_CAND("**** %s ****", __FUNCTION__);
  
  //This should not block - we came here through select!
  nRetVal = m_ipcCmdRx.IPC_RecvPacketBlocking(&stCmdInfo, sizeof(CANDCmdStruct));

  if (nRetVal < 0)
  {
    LogError(CAND_ERR_IPC_RX_CMD, __LINE__);
  }
  else
  {
    //TODO - no ack is sent back now - may be required later.
    switch (stCmdInfo.CmdType)
    {
      //Request to register a channel
    case REGISTER_DATA_CH:
      if (Register(stCmdInfo.CmdData) < 0)
      {
        nRetVal = -1;
        LogError(CAND_ERR_REG_CHANNEL, __LINE__);
      }
      break;

      //Request to de-register a channel
    case UNREGISTER_DATA_CH:
      if (DeRegister(stCmdInfo.CmdData.stRegCmdData.SlotID, 
                     stCmdInfo.CmdData.stRegCmdData.FnType, 
                     stCmdInfo.CmdData.stRegCmdData.FnCount ) < 0)
      {
        nRetVal = -1;
        LogError(CAND_ERR_DEREG_CHANNEL, __LINE__);
      }
      break;

      //Request to send data
    case TX_CAN_DATA:

#ifdef TEST_FAILURE
      if ( !( (1 + rand()) % TX_RAND_FAIL) )
      {
        g_ulTxDiscPkts++;
        DEBUG_CAND("CAND: TX pkt discarded.");
        nRetVal = 0;
        break;
      }
#endif
    
      //Extract and store the address
      ucDataPacket[0] = (stCmdInfo.CmdData.stTxData.CANId & 0xFF00) << 8;
      ucDataPacket[1] = (stCmdInfo.CmdData.stTxData.CANId & 0x00FF);

      //Copy the actual payload
      memcpy(&ucDataPacket[2], stCmdInfo.CmdData.stTxData.PktData,
             stCmdInfo.CmdData.stTxData.PktLen);

      DEBUG3("Data length: %d", stCmdInfo.CmdData.stTxData.PktLen);

      //2 address bytes + actual data payload length
      nDrvPakLen = stCmdInfo.CmdData.stTxData.PktLen + 2;

      
      // ---------temp added will remove later  
      DevAddrUnion stHostToDev1;
      //Extract packet identification information (for top layer)
      memcpy(&stHostToDev1.usDevAd, &ucDataPacket[2], sizeof(DevAddrUnion));
      
      //Take care of Endianness
      FixEndian(stHostToDev1.usDevAd);

#ifdef CAND_DEBUG_EN
      if( DebugLevel > 0 )
      {
        char szMessage[60];
        char szByte[5];

        sprintf (szMessage, "TX: ");
        for (int nCnt = 0; nCnt < nDrvPakLen; nCnt++)
        {
          sprintf (szByte, "0x%02x ", ucDataPacket[nCnt]);
          strcat (szMessage, szByte);
        }
        DEBUG2(szMessage);
      }
#endif

      //This is a BLOCKING write call!
      //Return doesn't indicate a successful data transmission - just
      //  indicates that the data was queued up in the driver, pending
      //  transmission.
      nRetVal = write(m_fdCANDrvTx, ucDataPacket, nDrvPakLen);

      //Write error
      if (nRetVal < 0)
      {
        LogError(CAND_ERR_DRV_TX_DATA, __LINE__);
      }
      //Short write!
      else if (nRetVal < nDrvPakLen)
      {
        DEBUG_CAND("nRetVal: %d, nDrvPakLen: %d", nRetVal, nDrvPakLen);
        nRetVal = -1;
        LogError(CAND_ERR_DRV_TX_DATA_SHORT, __LINE__);
      }
#ifdef CANDLOG_EN
      else
      {
        if (g_ucDumpLog)
        {
          g_ucDumpLog = 0;
          obCANDLog.DumpLogToFile();
        }
        
        CANDLogInfo stLogInfo;
        DevAddrUnion stHostToDev;
        //Extract packet identification information (for top layer)
        memcpy(&stHostToDev.usDevAd, &ucDataPacket[2], sizeof(DevAddrUnion));

        //Take care of Endianness
        FixEndian(stHostToDev.usDevAd);

        stLogInfo.ucDir = 1;
        stLogInfo.ucSlId = stCmdInfo.CmdData.stTxData.CANId;//GetSlotID(&stHostToDev.usDevAd);
        stLogInfo.ucFn = GetFnType(&stHostToDev.usDevAd);
        stLogInfo.ucFnCnt = GetFnCount(&stHostToDev.usDevAd);

        memcpy(stLogInfo.ucData, &stCmdInfo.CmdData.stTxData.PktData[2], 
               stCmdInfo.CmdData.stTxData.PktLen - 2);
        stLogInfo.ucDataLen = stCmdInfo.CmdData.stTxData.PktLen - 2;
        obCANDLog.AddCmdRespToLog(stLogInfo);
        
      }
#endif //CANDLOG_EN

      break;

      //This is not a valid request
    default:
      LogError(CAND_ERR_IPC_RX_CMD_INVALID, __LINE__);
      break;
    }
  }

  return nRetVal;
}

//Register a board with CAND
int CCAND::Register(CmdDataUnion& stCmdInfo)
{
  int nRetVal = 0;
  int nInitStep = 0;
  CANDRegInfo *pEntry = NULL;
  unsigned char SlotID, FnType, FnCount;
  
  SlotID = stCmdInfo.stRegCmdData.SlotID;
  FnType = stCmdInfo.stRegCmdData.FnType;
  FnCount = stCmdInfo.stRegCmdData.FnCount;

  // Check for valid registration
  if ( SlotID > 0x1F || FnType > 0x1F || FnCount > 0xF)
  {
    DEBUG1 ("Register: Invalid Inputs - %d, %d, %d", SlotID, FnType, FnCount);
    nRetVal = -1;
  }
  //Check for duplicate registration attempt
  else if (!AlreadyRegistered(SlotID, FnType, FnCount))
  {
    DEBUG1 ("Register: %d, %d, %d", SlotID, FnType, FnCount);
    pEntry = &ltRegList[SlotID][FnType][FnCount - 1];
      
    //Open a board specific command response IPC channel
    //IMPORTANT: CAND should always open it's transmit
    //  IPC channel in Non-blocking mode
    pEntry->m_cmdRespIPC = new CIPC;
    if (pEntry->m_cmdRespIPC->IPC_InitIPC(
          stCmdInfo.stRegCmdData.CmdRespIPCid,
          CMD_RESP_PIPE_MAILBOX_ID,
          IPC_SEND, IPC_OPEN_NONBLOCKING) < 0)
    {
      nRetVal = -1;
    }
    else
    {
      nInitStep++;
    }
      
    //If a stream IPC channel is required, open one!
    if (stCmdInfo.stRegCmdData.StreamRespIPCid)
    {
      pEntry->m_streamRespIPC = new CIPC;
      if (pEntry->m_streamRespIPC->IPC_InitIPC(
            stCmdInfo.stRegCmdData.StreamRespIPCid,
            CMD_STRM_PIPE_MAILBOX_ID,
            IPC_SEND, IPC_OPEN_NONBLOCKING) < 0)
      {
        nRetVal = -1;
      }
      else
      {
        nInitStep++;
      }
    }

    //If an error occurred, revert back and release newly created resources ...
    if (nRetVal < 0)
    {
      //Release command response IPC information
      if (pEntry->m_cmdRespIPC)
      {
        if (nInitStep >= 1)
        {
          pEntry->m_cmdRespIPC->IPC_Close();
        }
    
        delete pEntry->m_cmdRespIPC;
        pEntry->m_cmdRespIPC = NULL;
      }
      
      //Release stream IPC information
      if (pEntry->m_streamRespIPC)
      {
        if (nInitStep >= 2)
        {
          pEntry->m_streamRespIPC->IPC_Close();
        }
    
        delete pEntry->m_streamRespIPC;
        pEntry->m_streamRespIPC = NULL;
      }
    }
  }
  //Duplicate entry! Return with error
  else
  {
    DEBUG1 ("Already Registered! %d, %d, %d", SlotID, FnType, FnCount);
    nRetVal = -1;
  }
  
  return nRetVal;
}


//De-register a previously registered board.
int CCAND::DeRegister(unsigned char SlotID, unsigned char FnType, unsigned char FnCount)
{
  int nRetVal = 0;
  CANDRegInfo *pEntry = NULL;

  DEBUG1("Deregister: %d, %d, %d", SlotID, FnType, FnCount);

  //Get the entry for this board in the list...
  pEntry = GetMatchingEntry (SlotID, FnType, FnCount);
  
  if ( pEntry )
  {
    //Close and release the command response IPC
    if (pEntry->m_cmdRespIPC)
    {
      pEntry->m_cmdRespIPC->IPC_Close();
      delete pEntry->m_cmdRespIPC;
      pEntry->m_cmdRespIPC = NULL;
    }

    //Close and release the stream response IPC
    if (pEntry->m_streamRespIPC)
    {
      pEntry->m_streamRespIPC->IPC_Close();
      delete pEntry->m_streamRespIPC;
      pEntry->m_streamRespIPC = NULL;
    }

  }
  //If the board is not in the list, return with error
  else
  {
    nRetVal = -1;
  }

  return nRetVal;
}

//Check if the device has already been registered
int CCAND::AlreadyRegistered(unsigned char SlotID, unsigned char FnType, unsigned char FnCount)
{
  //Check if an entry exists for the given cmd information
  if (GetMatchingEntry(SlotID, FnType, FnCount))
    return 1;
  else
    return 0;
}

CANDRegInfo* CCAND::GetMatchingEntry(unsigned char SlotID, unsigned char FnType, unsigned char FnCount)
{
  if (ltRegList[SlotID][FnType][FnCount - 1].m_cmdRespIPC != NULL)
  {
    return &ltRegList[SlotID][FnType][FnCount - 1];
  }
  else
  {
    return NULL;
  }
}

//Log error messages
void CCAND::LogError(CAND_ERRS eCANDError, long lLineNr)
{
  DEBUG_CAND("**** %s ****", __FUNCTION__); 
  char *szErrString = NULL;

  switch(eCANDError)
  {
  case CAND_ERR_DRV_OPEN:
    szErrString = "CAND_ELOG: Error opening CAN driver";
    DEBUG1("CAND_ELOG: Error opening CAN driver.");
    break;
  case CAND_ERR_IPC_CMD_RX:
    szErrString = "CAND_ELOG: Error opening Command RX IPC channel";
    DEBUG1("CAND_ELOG: Error opening Command RX IPC channel.");
    break;
  case CAND_ERR_SELECT:
    szErrString = "CAND_ELOG: Select returned with error";
    DEBUG1("CAND_ELOG: Select returned with error.");
    break;
  case CAND_ERR_DRV_RX:
    szErrString = "CAND_ELOG: Error reading from the CAN driver";
    DEBUG1("CAND_ELOG: Error reading from the CAN driver.");
    break;
  case CAND_ERR_DRV_TX_ACK:
    szErrString = "CAND_ELOG: Error writing ACK packet to the CAN driver";
    DEBUG1("CAND_ELOG: Error writing ACK packet to the CAN driver.");
    break;
  case CAND_ERR_DRV_TX_ACK_SHORT:
    szErrString = "CAND_ELOG: Not all bytes of the ACK packet were written to the CAN driver";
    DEBUG1("CAND_ELOG: Not all bytes of the ACK packet were written to the CAN driver.");
    break;
  case CAND_ERR_CHANNEL_NOT_REG:
    szErrString = "CAND_ELOG: This channel has not been registered";
    DEBUG1("CAND_ELOG: This channel has not been registered.");
    break;
  case CAND_ERR_IPC_TX_RESP:
    szErrString = "CAND_ELOG: Error sending response to the upper layer";
    DEBUG1("CAND_ELOG: Error sending response to the upper layer.");
    break;
  case CAND_ERR_IPC_TX_STREAM:
    szErrString = "CAND_ELOG: Error sending streaming data to the upper layer";
    DEBUG1("CAND_ELOG: Error sending streaming data to the upper layer.");
    break;
  case CAND_ERR_IPC_TX_STREAM_NOT_REG:
    szErrString = "CAND_ELOG: Error sending streaming data to the upper layer: Transmit stream not registered";
    DEBUG1("CAND_ELOG: Error sending streaming data to the upper layer: Transmit stream not registered.");
    break;
  case CAND_ERR_IPC_RX_CMD:
    szErrString = "CAND_ELOG: Error reading command from the upper layer";
    DEBUG1("CAND_ELOG: Error reading command from the upper layer.");
    break;
  case CAND_ERR_REG_CHANNEL:
    szErrString = "CAND_ELOG: Registration command was not successful";
    DEBUG1("CAND_ELOG: Registration command was not successful.");
    break;
  case CAND_ERR_DEREG_CHANNEL:
    szErrString = "CAND_ELOG: De-registration commad was not successful";
    DEBUG1("CAND_ELOG: De-registration commad was not successful.");
    break;
  case CAND_ERR_DRV_TX_DATA:
    szErrString = "CAND_ELOG: Error writing data from upper layer to CAN driver";
    DEBUG1("CAND_ELOG: Error writing data from upper layer to CAN driver.");
    break;
  case CAND_ERR_DRV_TX_DATA_SHORT:
    szErrString = "CAND_ELOG: Not all bytes of the data packet were written to the CAN driver";
    DEBUG1("CAND_ELOG: Not all bytes of the data packet were written to the CAN driver.");
    break;
  case CAND_ERR_IPC_RX_CMD_INVALID:
    szErrString = "CAND_ELOG: Invalid command from the upper layer... discarded";
    DEBUG1("CAND_ELOG: Invalid command from the upper layer... discarded.");
    break;
  case CAND_ERR_CLOSE:
    szErrString = "CAND_ELOG: Error de-registering all channels. Not all allocated memory was de-allocated";
    DEBUG1("CAND_ELOG: Error de-registering all channels.");
    DEBUG1("Not all allocated memory was de-allocated.");
    break;

  default:
  case CAND_ERR_UNKNOWN:
    szErrString = "CAND_ELOG: Unknown error";
    DEBUG1("CAND_ELOG: Unknown error.");
    break;
  }

  // IMPORTANT NOTE: 
  // All SysLog's require a write to the GC Database. Sys Log messages are stored in the flash, 
  // and hence, the SysLog function call can cause this function to get blocked and not return 
  // for several seconds (or at the least several 100s of milliseconds)! This can cause any concurrent
  // requests from HAL to timeout!!! 

  // TODO: Need to have a mechanism to log these messages! CAND is the gateway to the H/W and an
  // error message from cand needs to get logged and analyzed.
  //SysLog(ERR_WARNING, ERR_GENERAL, __FILE__, lLineNr, szErrString);
  
}

#define DEF_CAN_DEV_PATH  "/dev/can1"

void PrintUsage()
{
  printf("Application usage:\n");
  printf("<app_name> -d <dev_path>\n");
  printf("Eg:\n");
  printf("cand -d /dev/can1\n");
}

//TODO - does this need to take in an arg on which device to use????? (i.e. the dev path)
int main(int argc, char *argv[])
{
  int nRetVal = 0;
  int nOptVal = 0;
  char *pcDevPath = NULL;

  
  CCAND canDaemon;

  /* run task with real time priority -p <priority number> */
  SetRTTaskPriority(argc, argv);

  InstallDebug(argc, argv);
  
  
  while (argv[optind] != NULL)
  {
    nOptVal = getopt(argc, argv, "vdp:");

    switch (nOptVal)
    {
    case 'd':
      pcDevPath = optarg;
      break;

    case 'v':
      break;
    case 'p':
      break;
    default:
      printf("Invalid option.\n");
      PrintUsage();
      return -1;
    }
  }

#ifdef CANDLOG_EN
  //Install a signal handler for SIGUSR1- used to display app statistics
  //if (SIG_ERR == signal(SIGUSR1, SIGUSR1Handler))
  //{
  //printf("Failed to install signal handler (SIGUSR1).\n");
  //}
#endif

#ifdef TEST_FAILURE
  //Install a signal handler for SIGINT - used to display app statistics when the app exits
  if (SIG_ERR == signal(SIGINT, SIGINTHandler))
  {
    printf("Failed to install signal handler (SIGINT).\n");
  }
#endif

  if (NULL == pcDevPath)
  {
    pcDevPath = DEF_CAN_DEV_PATH;
  }

  canDaemon.CANDHandler(pcDevPath);

#ifdef TEST_FAILURE
  canDaemon.CANDClose();
  
  printf("CAND: Number of Tx pkts discarded: %lu\n", g_ulTxDiscPkts);
  printf("CAND: Number of Rx pkts discarded: %lu\n", g_ulRxDiscPkts);
#endif
  
  return nRetVal;
}


