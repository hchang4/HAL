/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: ddisc.h
 * *
 * *  Description: Device discovery application. This application bypasses
 * *               CAN daemon and pings each possible CAN ID to detect
 * *               devices. When the device responds with device capability
 * *               information, this application parses that information,
 * *               generates device names and updates the HW_CONFIG table
 * *               with this information
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
 * *  $Id: ddisc.h,v 1.21 2017/03/14 16:05:06 emrsn\jbeatty Exp $
 * *  $Log: ddisc.h,v $
 * *  Revision 1.21  2017/03/14 16:05:06  emrsn\jbeatty
 * *  Support for 2 flavors of G2 heater / solenoid board, board rev 7 for 700/1500XA, board rev 8 for 1500XA R2
 * *
 * *  Revision 1.20  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.19  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.18  2014/06/26 10:01:21  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.17  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.16  2011/07/13 10:52:52  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.15  2010/10/29 13:26:06  emrsn\rsaha
 * *  Revised to build ddiscfallback ( with no DbManager interface and HAL support ).
 * *  This task is launched from Mk3upgradefallback when no firmware exists in CPU board.
 * *  This is to facilitate logging of IO firmware version details in current_software_info.txt file, so to display in MON2020.
 * *
 * *  Revision 1.14  2010/10/14 18:02:26  emrsn\rsaha
 * *  Reduced each CAN slot ping timeout interval from 3 seconds to 600 milliseconds.
 * *
 * *  Revision 1.13  2010/06/02 00:26:43  emrsn\rrthomas
 * *  Add device enumeration entries to HW_CONFIG table (Installed Hardware database) based on contents of ROC_CARD_TYPE table. This is a stop-gap fix for the buggy ROC card type detection on the XA platform hardware.
 * *
 * *  Revision 1.12  2010/05/18 22:01:12  emrsn\rsaha
 * *  Revised to ftp LOI_version_info.txt file from LOI to GC's /home/Daniel after done with pinging all the CAN slots. We do this only if LOI IR keypad is detected to be present during CAN device discovery otherwise its assumed LOI is not connected.
 * *
 * *  Revision 1.11  2010/05/06 23:24:20  EMRSN\RRTHOMAS
 * *  Added hooks to detect the cards connected to ROC expansion slots on the XA backplane. If we discover an Analog Output card, then we add enumerations for Analog Outputs in the Analog Output Sel table. Today, we don't do anything with the other card types - AI, DI, DO, Comm.
 * *
 * *  As of this writing, the ROC card detection is slightly flaky due to hardware limitations. This won't work if the second flex cable is connected to the GC backplane.
 * *
 * *  Revision 1.10  2010/04/15 12:16:51  emrsn\rsaha
 * *  Fixed bug 4136 - GC firmware doesn't start running if modbus application keep sending queries when GC boots up as those queries make ddisc to go in spin.
 * *  Fixed it via waiting for total timeout for the device discovry response instead of having constant timeout for individual packets of the response. This way even if a invalid message is received the time took to receive that message gets substracted from the total timeout and as a result, when total timeout zeros out ddisc moves on to ping the next slot, earlier due to constant timeout for individual packets it used to spin infinitely for a single slot if not the modbus client app is not stopped manually.
 * *
 * *  Total timeout is fixed at 3 seconds, ddisc pings slot 16 to 32, out of which only 3/4 slots hold the IO cards, so ping-n-wait for other slots with total timeout of 3 seconds for each will add a total of about 30 seconds boot up time delay.
 * *
 * *  Revision 1.9  2009/02/24 17:13:42  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.8  2009/02/16 21:32:32  epm-na\rsaha
 * *  Increased device discovery time-out delay to 600ms, twice of previous.
 * *
 * *  Revision 1.7  2009/02/10 21:37:19  epm-na\rsaha
 * *  Revised to ping only CAN addresses from 16 to 32. And updated to read firmware version and date embedded in device capbility packets and put them in current_software_info.txt file. The modification is done while maintaining backward compatibility with earlier protocol where firmware version and date is not part of device capability message.
 * *
 * *  Revision 1.6  2008/11/16 00:39:00  epm-na\rthomas
 * *  Added support for ROC expansion cards.
 * *
 * *  Revision 1.5  2007/01/09 02:37:18  EPM-NA\ssatpute
 * *  Interface for BCI CAN Interface card added.
 * *  Testing to be done.
 * *
 * *  Revision 1.4  2006/12/13 22:31:58  epm-na\DPrabhakar
 * *  SysLog() API related changes (goes along with the previous commit)
 * *
 * *  Revision 1.3  2006/12/08 21:32:41  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



#ifndef _DDISC_H
#define _DDISC_H

#include "Definitions.h"
#include "DevProtocol.h"

#ifndef MAKE_FALLBACK
#include "DataFragment.h"
#include "ResolveDevName.h"
#else //#ifndef MAKE_FALLBACK
#include "HelperClasses.h"
#endif //#ifndef MAKE_FALLBACK

#include "configHWSetup.h"

#ifdef BCI_MODE
#include "bci.h"
#endif

//Per Slot Timeout period
#define DDISC_SELECT_TIMEOUT_MSEC HAL_DFLT_TIMEOUT * 2

#define DDISC_TX_PKT_HDR_OFFSET   2
#define DDISC_TX_PKT_DATA_OFFSET  (DDISC_TX_PKT_HDR_OFFSET + sizeof(DevAddrUnion))

#define FIRMWARE_VERSION_STR_SZ     11  //xxx.xxx.xxx
#define FIRMWARE_DATE_STR_SZ        10  //YYYY.MM.DD

typedef struct {
  unsigned char           szFirmwareVersion[FIRMWARE_VERSION_STR_SZ]; //"xx.xx.xx" - Every field can have 2 digits MAX. No null termination.
  unsigned char           szFirmwareDate[FIRMWARE_DATE_STR_SZ];       //"YYYY/MM/DD" - No null termination.
} __attribute (( packed )) FIRMWARE_INFO;


typedef enum
{
  DDISC_ERR_CAN_DRV_OPEN = 0,
  DDISC_ERR_CAN_DRV_TX,
  DDISC_ERR_CAN_DRV_RX,
  DDISC_ERR_ADD_FRAG,
  DDISC_ERR_SELECT,
  DDISC_ERR_DEFRAG,
  DDISC_ERR_INFO_MEM,
  DDISC_ERR_SLOT_RESP_TIMEOUT,
  DDISC_ERR_PKT_INVALID,
  DDISC_ERR_INFO_PARSE,
  DDISC_ERR_CFG_TBL_INIT,
  DDISC_ERR_CFG_TBL_UPDATE,
  DDISC_ERR_NAME_RESOLUTION,
  DDISC_ERR_CFG_WRITE,
  DDISC_ERR_FAIL_2_FTP_LOI_VERSION_FILE,
} DDISC_ERRS;

class CDdisc
{
private:
  //Array to store the device discovery information
  unsigned char *m_pbyDiscInfo;

  //Number of bytes of device discovery information
  int m_nNrDiscInfo;

  //Defragmentation layer
  CDataFragment m_obDataFragment;

  //Name generator
  CResolveDevName m_obResolveName;

  //Interface class to access the database for updating
  //  device discovery information
  configHWSetup m_obDBInterface;
  
  //CAN driver FD
  int m_fdCAN;

#ifndef MODEL_370XA
  bool m_bIs1500XA_R2;
#endif
  
#ifdef BCI_MODE
  BCI_BRD_HDL CANBoard;
  char Controller;
  char PacketData[8];
  char Value0;

  int TxCounter0;
  int RxCounter0;

  BCI_ts_BrdSts BrdStatus;

  char CAN0TransmitOK;

  BCI_ts_BrdInfo BoardInfo;
  char Status0[16], Status1[16];

#endif

  //Initalize the CAN driver
  int InitCANDrv(char *pCANDrvPath);

  // Ping the SPI bus on the expansion slots for ROC expansion cards
  int PingSPI();

#ifdef MODEL_370XA
  // Ping 370XA LOI over UDP.
  int PingUDP();
#endif //#ifdef MODEL_370XA
  
  //Ping the specified CAN ID for device info.
  int PingCANId(int nCANId);

#ifdef MODEL_700XA
  // Detect FIDs/FPDs on 1500XA R2 backplane
  void CheckBackplaneBoards();
#endif

  //Transmit the ping command
  int TxPing(int nCANId);

  //Manage fragmented data
  int ManageFragmentation(unsigned char *byAddData, unsigned int unLen,
                          int nCANId);
  
  //Verify if the received packet header is from
  //  the same device that we 'pinged'
  int VerifyHeader(DevAddrUnion *stDevHdr, int nCANId);

  //Parse the received device info
  int ParseDiscInfo(int nCANId);

  //Error log infor
  void LogError(DDISC_ERRS eDdiscError, long lLineNr);

  // Determine the type of ROC card connected to the ROC Expansion slot
  ROC_CARD_TYPE GetROCCardType (ROC_CARD_SLOT eSlot);

public:
  //Conctructor
  CDdisc();
  //Desctuctor
  ~CDdisc();
  //The main function of device discovery
  int HandleDiscovery(char *pCANDrvPath);
};

#endif //_DDISC_H

