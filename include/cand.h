/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: cand.h
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
 * *  $Id: cand.h,v 1.9 2015/04/30 09:32:33 EMRSN\Dkad Exp $
 * *  $Log: cand.h,v $
 * *  Revision 1.9  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.8  2014/06/26 10:01:19  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.7  2013/07/11 19:56:40  emrsn\rrthomas
 * *  Changed how CAND stores and searches "device registration" information. Earlier, we had a std::list of registration entries. Changed it into a 3D array. The std::list is memory efficient, but routing an incoming CAN packet to the correct receipient (routing) requires a linear search in the std::list. Amount of time spent looking up routing entries depends upon where the registration entry is present in the std::list. For example, the Preamp Streaming entries are added last, so each time a packet comes in (which is once every 20 ms), we have to search through a list of around 50 entries to find the routing information. With a 3D array, we can use Slot ID, Fn Type and Fn Count to find the routing information without a linear search. Quick testing indicates that the new scheme uses 98% less time doing routing lookup.
 * *
 * *  Also took out all the BCI CAN Driver code (that we used for the 700XA Alphas on AMD based Linux Boxes); we don't use it anymore and it just clutters the code.
 * *
 * *  Revision 1.6  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.5  2008/01/08 19:52:46  epm-na\lratner
 * *  Moved class CIPC from /platform/HAL/ipc to /platform/ipc.
 * *
 * *  Revision 1.4  2006/12/13 22:36:28  epm-na\DPrabhakar
 * *  - Modified code to use the SysLog() API (logLib) to log error messsges
 * *  - Modified code to use the DEBUGx() API to print error messages
 * *  - Will now unregister the device if the write to that device channel's
 * *    FIFO fails (streaming or command/response).
 * *
 * *  Revision 1.3  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



#ifndef _CAND_H
#define _CAND_H

#include <list>
#include "ipc.h"
#include "Definitions.h"
#include "DevProtocol.h"


#ifdef CANDLOG_EN
#include "candlog.h"
#endif //CANDLOG_EN


using namespace::std;

//TODO - are we waiting on a select or not?
#define SELECT_TIMEOUT_SEC      0
#define SELECT_TIMEOUT_USEC     0

//Length of the acknowledge packet (2 CAN address bytes + 2 bytes of packet header)
#define CAN_ACK_PACKET_LEN      4

struct CANDRegInfo
{
  CIPC *m_cmdRespIPC;
  CIPC *m_streamRespIPC;
};

enum CAND_ERRS
{
  CAND_ERR_DRV_OPEN = 0,
  CAND_ERR_IPC_CMD_RX,
  CAND_ERR_SELECT,
  CAND_ERR_DRV_RX,
  CAND_ERR_DRV_TX_ACK,
  CAND_ERR_DRV_TX_ACK_SHORT,
  CAND_ERR_CHANNEL_NOT_REG,
  CAND_ERR_IPC_TX_RESP,
  CAND_ERR_IPC_TX_STREAM,
  CAND_ERR_IPC_TX_STREAM_NOT_REG,
  CAND_ERR_IPC_RX_CMD,
  CAND_ERR_REG_CHANNEL,
  CAND_ERR_DEREG_CHANNEL,
  CAND_ERR_DRV_TX_DATA,
  CAND_ERR_DRV_TX_DATA_SHORT,
  CAND_ERR_IPC_RX_CMD_INVALID,
  CAND_ERR_CLOSE,
  //ADD NEW ERRORS HERE
  CAND_ERR_UNKNOWN,
};


class CCAND
{
private:
//Functions...
  //Initialize driver
  int InitDriver(char *pDevPath);

  //Initialize the command IPC - RX only
  int InitCmdIPC();

  //Handle data from the CAN driver
  int HandleCANReceive();

  //Handle commends from the higher level
  int HandleTopLevelCmds();

  //Register a device with CAND
  int Register(CmdDataUnion& stRegInfo);

  //De-register a previously registered device.
  int DeRegister(unsigned char SlotID, unsigned char FnType, unsigned char FnCount);

  //Check if the device has already been registered
  int AlreadyRegistered(unsigned char SlotID, unsigned char FnType, unsigned char FnCount);

  //Get details of a matching entry in the list
  CANDRegInfo* GetMatchingEntry(unsigned char SlotID, unsigned char FnType, unsigned char FnCount);

  void LogError(CAND_ERRS eCANDError, long lLineNr);

//Variables...
  //Receive/Transmit descriptors for the CAN device driver
  int m_fdCANDrvRx;
  int m_fdCANDrvTx;

  //The command IPC channel - to receive and process commands
  //  from the upper layer
  CIPC m_ipcCmdRx;

  //List of registered device enumerations
  CANDRegInfo ltRegList[32][32][16]; // Max 32 Slots, Max 32 Fn Types Per Slot, Max 16 Enumerations Per Type
  


#ifdef CANDLOG_EN
  CCANDLog obCANDLog;
#endif //CANDLOG_EN

  
public:
  //Default constructer
  CCAND();
  //Default destructor
  ~CCAND();

  //The CAND handler - the main function
  int CANDHandler(char *pDevPath);

  //Release resources and close driver & all IPC channels
  int CANDClose();
};


#endif //_CAND_H
