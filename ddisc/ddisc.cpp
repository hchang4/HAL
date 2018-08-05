/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: ddisc.cpp
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
 * *  $Id: ddisc.cpp,v 1.45 2017/03/14 16:05:39 emrsn\jbeatty Exp $
 * *  $Log: ddisc.cpp,v $
 * *  Revision 1.45  2017/03/14 16:05:39  emrsn\jbeatty
 * *  Support for 2 flavors of G2 heater / solenoid board, board rev 7 for 700/1500XA, board rev 8 for 1500XA R2
 * *
 * *  Revision 1.44  2016/11/17 13:52:49  emrsn\jbeatty
 * *  Added retries to 1500XA R2 backplane FID and FPD detection logic
 * *
 * *  Revision 1.43  2016/11/01 13:06:10  emrsn\jbeatty
 * *  Added support for FPD G2
 * *
 * *  Revision 1.42  2016/09/28 14:14:55  emrsn\jbeatty
 * *  Made changes for auto-detection of 1500XA R2 backplane
 * *
 * *  Revision 1.41  2016/09/02 15:19:32  emrsn\jbeatty
 * *  Fixes for 1500XA R2 FIDs
 * *
 * *  Revision 1.40  2016/08/30 09:56:11  emrsn\jbeatty
 * *  Mods for 1500XA R2 support
 * *
 * *  Revision 1.39  2016/08/09 17:08:09  emrsn\jbeatty
 * *  Support for 1500XA R2 (ddiscfallback build still to be fixed)
 * *
 * *  Revision 1.38  2015/05/04 14:31:44  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.37  2014/12/01 20:46:17  emrsn\rrthomas
 * *  Generation 2 IO Board Support - Check board revision of Preamp, Heater Solenoid and Base IO Boards and report Generation and Version information in current_software_info.txt file. This information will be used in other tasks (MON, Upgrade etc) to determine which board is connected and thereby determine what features to allow, how to program flash etc.
 * *
 * *  Revision 1.36  2014/06/26 09:59:50  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.35  2012/04/25 09:21:42  emrsn\rsaha
 * *  Revised names of IMB Firmware as Module Firmware and IMB Slot as Module Slot.
 * *
 * *  Revision 1.34  2012/03/21 07:53:21  emrsn\rsaha
 * *  Revised to correctly log IMB firmware name to current_software_info.txt file.
 * *
 * *  Revision 1.33  2012/03/21 06:48:42  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.32  2012/01/03 13:10:19  emrsn\rsaha
 * *  Revised to ping and discover IMB device.
 * *
 * *  Revision 1.31  2011/09/26 14:35:14  emrsn\rsaha
 * *  Increased support for 5 ROCIO DOs and 8 ROCIO DIs from 4 DO and 4 DI respectively.
 * *
 * *  Revision 1.30  2011/08/13 10:57:25  emrsn\rsaha
 * *  Revised as per Rony's comment.
 * *  Revised sequence number to get read from /tmp/UDPCommSequenceFile so to make sure its serial order across system.
 * *  Revised to only ping selected CAN IDs where CAN IO devices are expected to be present for 700XA and 370XA. The code is placed inside conditional compilation for 700XA and 370XA.
 * *
 * *  Revision 1.29  2011/07/20 11:14:54  emrsn\rsaha
 * *  Corrected AnalyzerIO name while loggin version details to current_software_info.txt file.
 * *
 * *  Revision 1.28  2011/07/15 11:57:51  emrsn\rsaha
 * *  Instead of having 3 different copies of UDP.cpp file in mk3upgrade, ddisc and halsrc; we are now getting compiled with UDPClient.o file present in /src/common_static/modules directory.
 * *
 * *  UDP.cpp file name is changed to UDPClient.cpp otherwise contents are all same.
 * *
 * *  Revision 1.27  2011/07/13 11:05:27  emrsn\rsaha
 * *  Revised to do device capability query for 370XA LOI over UDP.
 * *
 * *  Revision 1.26  2011/01/25 05:45:50  emrsn\rsaha
 * *  Corrected DI and DO description name to Digital Input and Digital Output.
 * *
 * *  Revision 1.25  2011/01/25 05:14:01  emrsn\rsaha
 * *  Added support for ROC IO Dicrete Inputs, Discrete Outputs and Analog Inputs.
 * *
 * *  And also synced the following changes from release_2_0 to Head branch -
 * *  Added support for ROC Modems. If ddisc sees "ROC Modem" in the ROC_CARD_CONFIG table, it adds a "Serial Port" to HW_CONFIG and calls it "Serial Com Port" and labels it as "Serial I/O (Modem)". The physical naming convention of the serial port is just like another ROC RS232 or 422/485 card.
 * *
 * *  Revision 1.24  2010/10/29 13:23:26  emrsn\rsaha
 * *  Revised to build ddiscfallback ( with no DbManager interface and HAL support ).
 * *  This task is launched from Mk3upgradefallback when no firmware exists in CPU board.
 * *  This is to facilitate logging of IO firmware version details in current_software_info.txt file, so to display in MON2020.
 * *
 * *  Revision 1.23  2010/10/14 18:08:13  emrsn\rsaha
 * *  Revised to name Analyzer IO card, if found, in current_software_info.txt file. Also now returning from main, instead of exiting, to avoid file descriptor leaks from tableAPI.
 * *
 * *  Revision 1.22  2010/06/07 20:07:18  emrsn\rsaha
 * *  Bug Fix - If LOI and Preamp IO are not connected to GC then on GC bootup, device disconvery task (ddisc) keep spining infinitely. It spins around a select call. Most probably it is due to switching from pppd to slattach when pppd timed-out to make a connection with LOI. Eventhough the event is a no concern to ddisc, it seems ddisc also receives a signal, which it doesn't handle thus resulting in infinite spin.
 * *  Revised code to reset the set of fds before setting it to the CAN fd, clearing off the select returns due to unwanted signal interruptions.
 * *
 * *  Revision 1.21  2010/06/02 00:26:42  emrsn\rrthomas
 * *  Add device enumeration entries to HW_CONFIG table (Installed Hardware database) based on contents of ROC_CARD_TYPE table. This is a stop-gap fix for the buggy ROC card type detection on the XA platform hardware.
 * *
 * *  Revision 1.20  2010/05/18 22:03:10  emrsn\rsaha
 * *  Revised to ftp LOI_version_info.txt file from LOI to GC's /home/Daniel after done with pinging all the CAN slots. We do this only if LOI IR keypad is detected to be present during CAN device discovery, otherwise its assumed LOI is not connected.
 * *
 * *  Revision 1.19  2010/05/06 23:24:20  EMRSN\RRTHOMAS
 * *  Added hooks to detect the cards connected to ROC expansion slots on the XA backplane. If we discover an Analog Output card, then we add enumerations for Analog Outputs in the Analog Output Sel table. Today, we don't do anything with the other card types - AI, DI, DO, Comm.
 * *
 * *  As of this writing, the ROC card detection is slightly flaky due to hardware limitations. This won't work if the second flex cable is connected to the GC backplane.
 * *
 * *  Revision 1.18  2010/04/15 12:15:58  emrsn\rsaha
 * *  Fixed bug 4136 - GC firmware doesn't start running if modbus application keep sending queries when GC boots up as those queries make ddisc to go in spin.
 * *  Fixed it via waiting for total timeout for the device discovry response instead of having constant timeout for individual packets of the response. This way even if a invalid message is received the time took to receive that message gets substracted from the total timeout and as a result, when total timeout zeros out ddisc moves on to ping the next slot, earlier due to constant timeout for individual packets it used to spin infinitely for a single slot if not the modbus client app is not stopped manually.
 * *
 * *  Total timeout is fixed at 3 seconds, ddisc pings slot 16 to 32, out of which only 3/4 slots hold the IO cards, so ping-n-wait for other slots with total timeout of 3 seconds for each will add a total of about 30 seconds boot up time delay.
 * *
 * *  Revision 1.17  2009/09/07 11:45:10  epm-na\rsaha
 * *  Revised to flush previously received fragmented packets before pinging the next device, else it corrupts the next discovery response message.
 * *  And instead of wait timeout ( 600msec ) for the complete message now waiting the same for each packet. This is done to receive Graphical LOI discovery response message in one piece, Graphical LOI takes time to respond and earlier our 600msec for the complete message was too short, resulting in GLOI and BaseIO to go missing in Installed Hardware list.
 * *
 * *  Revision 1.16  2009/06/23 15:44:36  epm-na\rsaha
 * *  Revised to indentify LOIIRKeyPad firmware slot number.
 * *
 * *  Revision 1.15  2009/06/02 14:12:04  epm-na\rsaha
 * *  Revised to discard unknown function types of a device.
 * *
 * *  Revision 1.14  2009/05/13 23:06:36  epm-na\rsaha
 * *  Revised to log FFB firmware name and slot number correctly to current_software_info.txt file.
 * *
 * *  Revision 1.13  2009/02/24 17:26:35  epm-na\rsaha
 * *  Updated to handle revised firmware version and to log slot ids in current_software_info.txt file.
 * *  Modification made to keep compatibility with earlier versions of IO-firmwares. In such cases it won't log firmware version/date to current_software_info.txt
 * *  file but will update HW_CONFIG table.
 * *
 * *  Revision 1.12  2009/02/11 19:18:11  epm-na\rsaha
 * *  Revised to discard orphaned CAN packets (e.g. If preamp is streaming.).
 * *  Previous to this device discovery task used to end up in confusion as "CPU" reboots while preamp is in streaming mode.
 * *
 * *  Revision 1.11  2009/02/10 21:36:13  epm-na\rsaha
 * *  Revised to ping only CAN addresses from 16 to 32. And updated to read firmware version and date embedded in device capbility packets and put them in current_software_info.txt file. The modification is done while maintaining backward compatibility with earlier protocol where firmware version and date is not part of device capability message.
 * *
 * *  Revision 1.10  2008/11/16 00:44:10  epm-na\rthomas
 * *  Hack for first 700XA order for BP. Order requires a ROC SPI based Analog Ouput expansion card. We have no means now to detect the type of card plugged into the remote expansion SPI slot on the XA backplane. So, for now, Device Discovery will add four "Analog Output on SPI expansion slot 1" entries into the HW_CONFIG table.
 * *
 * *  Revision 1.9  2007/01/10 21:50:39  EPM-NA\ssatpute
 * *  Added interface for BCI Can Interface board. Tested. Working fine.
 * *
 * *  Revision 1.8  2007/01/09 02:36:38  EPM-NA\ssatpute
 * *  Interface for BCI CAN Interface card added.
 * *  Testing to be done.
 * *
 * *  Revision 1.7  2006/12/14 00:11:50  epm-na\DPrabhakar
 * *  Fixed a bug when handling the -v option (the app needs to ignore that
 * *  option; was not doing it correctly, which was causing the app to exit
 * *  when using the -v option)
 * *
 * *  Revision 1.6  2006/12/13 22:29:40  epm-na\DPrabhakar
 * *  - Suresh modified tableAPI access code - dynamic allocation for
 * *    by passing the table that is being used in the constructor.
 * *    Done for memory usage optimization.
 * *  - All error messages are now logged using the SysLog() API (logLib)
 * *  - Modified code to use the DEBUGx API
 * *
 * *  Revision 1.5  2006/12/08 21:42:51  epm-na\DPrabhakar
 * *  Code clean-up
 * *    - Removed device name prints
 * *    - Re-enabled check for total packet length
 * *
 * *  Revision 1.4  2006/12/08 21:27:35  epm-na\DPrabhakar
 * *  - Added Danile file headers.
 * *  - Added README file containing application execution information.
 * *  - Modified device discovery to update the database with board revision
 * *    information.
 * *  - Added device description field to the database containing information
 * *    provided by the device during device discovery.
 * *
 * *
 * *************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>

#include "ddisc.h"
#include "FixEndian.h"
#include "loglib.h"

#include "UDPClient.h"
#include "LOI370XAUDPCommProtocol.h"

#ifndef MAKE_FALLBACK
#include "debug.h"
#endif //#ifndef MAKE_FALLBACK

#include "cand.h"
#include "CPU_ADC_AD7908.h"

#include "hardwareHelpers.h"

#ifdef BCI_MODE
#include "bci.h"
#include "bci_int.h"

#define BCI_TX_DEL
#endif

#define GC700XP_GRAPHICAL_LOI_BD_CAN_ID     0x19

// Constructor
CDdisc::CDdisc()
{
  m_pbyDiscInfo = NULL;
  m_nNrDiscInfo = 0;
  m_fdCAN = 0;
#ifndef MODEL_370XA
  m_bIs1500XA_R2 = false;
#endif
}

// Desctuctor
CDdisc::~CDdisc()
{
  if (m_pbyDiscInfo)
    free(m_pbyDiscInfo);
}

#ifndef MODEL_370XA
void CDdisc::CheckBackplaneBoards()
{
  if (m_bIs1500XA_R2)
  {
    // Now see if FID or FPD is installed on the backplane
    float fVoltage;
    CCPU_ADC2_AD7908 oCpuADC2;
    int nTries = 10;
    while (nTries-- > 0)
    {
      int nRet = oCpuADC2.GetReading(4, fVoltage);
      if (nRet == -1)
        continue;

      if (fVoltage <= MAX_VOLT_FID)
      {
        // FID found - add it to HW_CONFIG
        m_obDBInterface.AddDevNameHWConfigTable("FID:BACKPLANE:FID_1", FN_FID, 26, 1, "Backplane FID");
        break;
      }
      if (fVoltage >= MIN_VOLT_FPD && fVoltage <= MAX_VOLT_FPD)
      {
        // FPD found - add it to HW_CONFIG
        m_obDBInterface.AddDevNameHWConfigTable("FPD:BACKPLANE:FPD_1", FN_FPD, 27, 1, "Backplane FPD");
        break;
      }
    }
  }
}
#endif  //#ifndef MODEL_370XA

// The main function of device discovery
int CDdisc::HandleDiscovery(char *pCANDrvPath)
{
  int nRetVal = ERR_SUCCESS;

#ifdef MODEL_370XA

#define NUM_CAN_IDS_TO_PING     2
  int nListCANIds2Ping[NUM_CAN_IDS_TO_PING] = {   GC370XA_ANALYZER_BD_BASE_CAN_MSG_ID,      //370XA Analyzer IO
                                                  GC370XA_IMB_BD_BASE_CAN_MSG_ID
  };


#else //#ifdef MODEL_370XA

#define NUM_CAN_IDS_TO_PING 10
  int nListCANIds2Ping[NUM_CAN_IDS_TO_PING] = { GC700XP_PREAMP_BD_CAN_BASE_MSG_ID,  //Preamp 1 - Slot 1
                                                GC700XP_PREAMP_BD_CAN_BASE_MSG_ID+1,  //Preamp 2 - Slot 3
                                                GC700XP_PREAMP_BD_CAN_BASE_MSG_ID+2,  //Preamp 3 - Slot 5
                                                GC700XP_FPD_G2_BASE_CAN_MSG_ID, // First G2 FPD
                                                GC700XP_FPD_G2_BASE_CAN_MSG_ID+1, // Second G2 FPD
                                                GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID,//Heater Solenoid 1 - Slot 2
                                                GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID+1,//Heater Solenoid 2 - Slot 4
                                                GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID,    //FF Card
                                                GC700XP_GRAPHICAL_LOI_BD_BASE_CAN_MSG_ID, //700XA LOI
                                                GC700XP_BASE_IO_BD_BASE_CAN_MSG_ID    //BaseIO 
  };
  bool b700XALOIPreset = false;
#endif //#ifdef MODEL_370XA

  if (NULL == pCANDrvPath)
  {
    return ERR_INVALID_ARGS;
  }

  // Init/Open the CAN driver
  if ( (nRetVal = InitCANDrv(pCANDrvPath)) < 0)
  {
    LogError(DDISC_ERR_CAN_DRV_OPEN, __LINE__);
  }

  if (ERR_SUCCESS == nRetVal)
  {
    //Initialize the DB interface
    if (m_obDBInterface.SetTablesPointer() < 0)
    {
      LogError(DDISC_ERR_CFG_TBL_INIT, __LINE__);
      nRetVal = ERR_INTERNAL_ERR;
    }

#ifndef MODEL_370XA
    if (Detect1500XAR2())
      m_bIs1500XA_R2 = true;
#endif

    //Ping all possible slot IDs and wait for a response.
    for (int nInd  = 0; nInd < NUM_CAN_IDS_TO_PING; nInd++)
    {
#ifdef MODEL_370XA
      PingCANId(nListCANIds2Ping[nInd]);
#else
      if (PingCANId(nListCANIds2Ping[nInd]) >= 0
      &&  nListCANIds2Ping[nInd] == GC700XP_GRAPHICAL_LOI_BD_CAN_ID)
        b700XALOIPreset = true;
#endif
    }

#ifdef MODEL_370XA

    PingUDP();

#else //#ifdef MODEL_370XA

    // Detect boards connected to SPI expansion slots 1 and 2
    PingSPI();

    // Get the LOI version information file if LOI is connected.
    if (b700XALOIPreset)
    {
      if (system("ftpget -u Daniel -p Meter2Me 10.0.0.2 /home/Daniel/LOI_version_info.txt /home/Daniel/LOI_version_info.txt") == -1)
      {
#ifndef MAKE_FALLBACK
        DEBUG1("GLOI IRKey is detected but ftpget of LOI_version_info.txt failed!!!");
#endif
        LogError(DDISC_ERR_FAIL_2_FTP_LOI_VERSION_FILE, __LINE__);
      }
      else
        printf("Retrived Graphical LOI version information file.\n");
    }

    CheckBackplaneBoards();

#endif //#ifdef MODEL_370XA

    // Commit device information to the database
    if (m_obDBInterface.WriteHWConfigTable() < 0)
    {
      LogError(DDISC_ERR_CFG_WRITE, __LINE__);
      nRetVal = ERR_INTERNAL_ERR;
    }
  }

  return nRetVal;
}

#ifndef BCI_MODE 
// Initialize the CAN driver
int CDdisc::InitCANDrv(char *pCANDrvPath)
{

  //#ifdef BC_MODE
  if (NULL == pCANDrvPath)
  {
    return ERR_INVALID_ARGS;
  }

  m_fdCAN = open(pCANDrvPath, O_RDWR | O_NONBLOCK);
  
  if (m_fdCAN < 0)
  {
    return ERR_INTERNAL_ERR;
  }

  return ERR_SUCCESS;



}
#else
// if BCI_MODE 
//Initialize the CAN driver
int CDdisc::InitCANDrv(char *pCANDrvPath)
{
  if (NULL == pCANDrvPath)
  {
    return ERR_INVALID_ARGS;
  }


  int ret;

  printf ("Open board\n");
  ret = BCI_OpenBoard (&CANBoard, pCANDrvPath);
  if (ret != BCI_OK)
  {
    printf (BCI_GetErrorString (ret));
    return -1;
  }

  printf ("Get Board Info\n");
  if ((ret = BCI_GetBoardInfo (CANBoard, &BoardInfo)) != BCI_OK)
  {
    printf (BCI_GetErrorString (ret));
    return -1;
  }

  printf ("Board info: FW version %d ContNum %d\n", BoardInfo.fw_version,
          BoardInfo.num_can);
  printf (" Controller 0: [%s]\n", BoardInfo.can_type[0]);
  printf (" Controller 1: [%s]\n", BoardInfo.can_type[1]);

  /*========================================================================*/

  printf
    ("Configure CAN0 controller and start it (11bit/1000Kbits/Poll mode):\n");
  BCI_MDelay (1500);
  Controller = 0;
  printf ("Controller [%d] Reset\n", Controller);
  BCI_ResetCan (CANBoard, Controller);
  printf ("Controller [%d] Init\n", Controller);
  BCI_InitCan (CANBoard, Controller, BCI_1000KB);
  printf ("Controller [%d] ConfigRxQueue\n", Controller);
#warning "TESTING BCI_POLL_MODE INSTEAD OF BCI_LATENCY_MODE"
  BCI_ConfigRxQueue (CANBoard, Controller, BCI_POLL_MODE);
  //BCI_ConfigRxQueue (CANBoard, Controller, BCI_LATENCY_MODE);

  printf ("Controller [%d] Register filter ID\n", Controller);
  BCI_RegisterRxId (CANBoard, Controller, BCI_MFF_11_DAT, 10);
  BCI_RegisterRxId (CANBoard, Controller, BCI_MFF_11_DAT, 11);
  BCI_RegisterRxId (CANBoard, Controller, BCI_MFF_11_DAT, 12);
  //BCI_RegisterRxId (CANBoard, Controller, BCI_MFF_29_DAT, 0x10);

  BCI_SetAccMask (CANBoard, Controller, BCI_11B_MASK, 0, BCI_ACC_ALL);
  BCI_SetAccMask (CANBoard, Controller, BCI_29B_MASK, 0, BCI_ACC_ALL);
  printf ("Controller [%d] Start\n", Controller);
  BCI_StartCan (CANBoard, Controller);

  //Save the file descriptor for using it in select
  m_fdCAN = ((CANInterface *) CANBoard)->DeviceFileDesc[0];

  //printf("File descriptor: %d\n", m_fdCANDrvTx);

  if (m_fdCAN < 0)
  {
    return ERR_INTERNAL_ERR;
  }

  return ERR_SUCCESS;
}
#endif

#ifndef BCI_MODE 
//Ping the specified CAN ID for device info.
int CDdisc::PingCANId(int nCANId)
{ 
  int nRetVal = ERR_SUCCESS;
  unsigned char ucData[CAN_PKT_MAX_LEN];
  BOOL bLoopExit = FALSE;
  fd_set readFDs;
  struct timeval tvSelect;
  int nTimeoutMS = DDISC_SELECT_TIMEOUT_MSEC;

  m_obDataFragment.Flush();
  
  printf("Pinging CAN ID: %d\n", nCANId);
  if ( (nRetVal = TxPing(nCANId)) < 0)
  {
    bLoopExit = TRUE;
  }

  while (FALSE == bLoopExit)
  {
    //Setup the FDs for select
    FD_SET(m_fdCAN, &readFDs);
    //Select timeout
    tvSelect.tv_sec = nTimeoutMS/1000;
    tvSelect.tv_usec = (INT32)((INT64)nTimeoutMS * 1000) - (INT64)((INT64)tvSelect.tv_sec * 1000000); // remaining usecs.

    nRetVal = select(m_fdCAN + 1, &readFDs, NULL, NULL, &tvSelect);
    //Wait for data
    if (nRetVal < 0)
    {
      bLoopExit = TRUE;
      nRetVal = ERR_INTERNAL_ERR;
      LogError(DDISC_ERR_SELECT, __LINE__);
    }
    else if (0 == nRetVal)
    {
      bLoopExit = TRUE;
      nRetVal = ERR_TIMEOUT;
      //LogError(DDISC_ERR_SLOT_RESP_TIMEOUT, __LINE__);
    }
    //Got data!
    else
    {
      //Compute the remaining time & use it for the next call
      nTimeoutMS = (tvSelect.tv_sec * 1000) + (tvSelect.tv_usec/1000);

      //Verify if there is data on CAN...
      if (FD_ISSET(m_fdCAN, &readFDs))
      {
        //Read data from CAN
        nRetVal = read(m_fdCAN, ucData, CAN_PKT_MAX_LEN);

        //Read error
        if (nRetVal < 0)
        {
          bLoopExit = TRUE;
          nRetVal = ERR_INTERNAL_ERR;
          LogError(DDISC_ERR_CAN_DRV_RX, __LINE__);
        }
        //No data
        else if (0 == nRetVal)
        {
          //No data
        }
        //Data on CAN!
        else
        {
          //Add data to the de-fragmenter and see if all the 
          //  fragments have been received...
          nRetVal = ManageFragmentation(ucData, nRetVal, nCANId);

          //Yet to receive all fragments...
          if (ERR_DATA_PENDING == nRetVal)
          {
            //There's still time to receive fragments
            if (nTimeoutMS)
            {
              continue;
            }
            //Timed out... didn't receive any/all packets within 
            //timeout period
            else
            {
              bLoopExit = TRUE;
              nRetVal = ERR_TIMEOUT;
              LogError(DDISC_ERR_SLOT_RESP_TIMEOUT, __LINE__);
            }
          }
          //Some other error occurred...
          else if (nRetVal < 0)
          {
            //   printf("%d: error code %d",nRetVal);
            bLoopExit = TRUE;
            LogError(DDISC_ERR_DEFRAG, __LINE__);
          }
          //Got the complete packet!
          else
          {
            bLoopExit = TRUE;
            nRetVal = ERR_SUCCESS;
          }
        }
      }// if (FD_ISSET....)
    }// if (select(....))

    FD_ZERO( &readFDs );
 
  }// while (FALSE == bLoopExit)

  // If discovery info was successfully received for this slot ID...
  if (ERR_SUCCESS == nRetVal)
  {
    printf("\t... found device.\n");
    //Parse this info and put it into a file
    if ( (nRetVal = ParseDiscInfo(nCANId)) < 0)
    {
      LogError(DDISC_ERR_INFO_PARSE, __LINE__);
    }
  }

  return nRetVal;
}

//Transmit the ping command
int CDdisc::TxPing(int nCANId)
{
  int nRetVal = ERR_SUCCESS;
  CmdAckUnion stCmd = {0};
  DevAddrUnion stDevAddr;
  unsigned char ucTxData[CAN_PKT_MAX_LEN];
  
  memset(&stCmd, 0, sizeof(CmdAckUnion));
  memset(&stDevAddr, 0, sizeof(DevAddrUnion));

  //Setup the command
  SetCmdAckCommand(&stCmd.byCmdAck, BD_GET_DEV_CAP);
  //Setup the function type
  SetFnType(&stDevAddr.usDevAd, FN_CAP);

  //Take care of Endianness
  FixEndian(stDevAddr.usDevAd);
  
  ucTxData[1] = nCANId & 0x00FF;
  ucTxData[0] = (nCANId & 0xFF00) << 8;

  memcpy(&ucTxData[DDISC_TX_PKT_HDR_OFFSET], &stDevAddr, sizeof(DevAddrUnion));
  memcpy(&ucTxData[DDISC_TX_PKT_DATA_OFFSET], &stCmd, sizeof(CmdAckUnion));

  if (write(m_fdCAN, ucTxData, 2 + sizeof(DevAddrUnion) + sizeof(CmdAckUnion)) <  0)
  {
    nRetVal = ERR_INTERNAL_ERR;
    LogError(DDISC_ERR_CAN_DRV_TX, __LINE__);
  }
  
  return nRetVal;
}
//-----------------------
#else
//if BCI_MODE
//Ping the specified CAN ID for device info.
int CDdisc::PingCANId(int nCANId)
{ 
  int nRetVal = ERR_SUCCESS;
  unsigned char ucData[CAN_PKT_MAX_LEN+1];
  unsigned char ucData1[CAN_PKT_MAX_LEN+10];
  
  BOOL bLoopExit = FALSE;
  fd_set readFDs;
  struct timeval tvSelect;
  int nTimeoutMS = DDISC_SELECT_TIMEOUT_MSEC;

  printf("Pinging CAN ID: %d\n", nCANId);
  if ( (nRetVal = TxPing(nCANId)) < 0)
  {
    bLoopExit = TRUE;
  }

  int err;
  while (FALSE == bLoopExit)
  {

    BCI_ts_CanMsg ReceivedCANMsg0;
    err = BCI_ReceiveCanMsg(CANBoard, 0, &ReceivedCANMsg0, DDISC_SELECT_TIMEOUT_MSEC);//BCI_NO_WAIT);
    if( err ==  BCI_OK )
    {
      //BCI_ShowCANMsg (&ReceivedCANMsg0);
      if(CAN_PKT_MAX_LEN >= ReceivedCANMsg0.dlc)
      {
        memcpy(ucData,
               ReceivedCANMsg0.a_data, ReceivedCANMsg0.dlc);
        nRetVal = ReceivedCANMsg0.dlc;
        //printf("BCI data received bytes: %d\n", nRetVal);
      
        /*
          memcpy(ucData1,&ReceivedCANMsg0.id,sizeof(ReceivedCANMsg0.id));
          memcpy(ucData1+sizeof(ReceivedCANMsg0.id),  
          ReceivedCANMsg0.a_data, ReceivedCANMsg0.dlc);
        */  
#if 0
        CANDRespStruct stCANData;
        DevAddrUnion stHostToDev;
        CANDRegInfo stCANDRegInfo;
        DevAddrUnion stDevToHost;
        //Populate the data length field
        memcpy(stCANData.stRespData.stRxData.PktData,
               ReceivedCANMsg0.a_data, ReceivedCANMsg0.dlc);
        //      nRetVal = ReceivedCANMsg0.dlc;

        stCANData.stRespData.stRxData.PktLen =ReceivedCANMsg0.dlc ;

        //Extract packet identification information (for top layer)
        memcpy(&stDevToHost.usDevAd, stCANData.stRespData.stRxData.PktData,
               sizeof(DevAddrUnion));

        //Take care of Endianness
        FixEndian(stDevToHost.usDevAd);

        //This information is required to check if the board is
        //  registered or not
        stCANDRegInfo.m_stCmdInfo.stRegCmdData.SlotID = GetSlotID(&stDevToHost.usDevAd);
        stCANDRegInfo.m_stCmdInfo.stRegCmdData.FnType = GetFnType(&stDevToHost.usDevAd);
        stCANDRegInfo.m_stCmdInfo.stRegCmdData.FnCount = GetFnCount(&stDevToHost.usDevAd);

        /*      printf("\tDev Data SL: %d, FT: %d, FC: %d\n",
          stCANDRegInfo.m_stCmdInfo.stRegCmdData.SlotID,
          stCANDRegInfo.m_stCmdInfo.stRegCmdData.FnType,
          stCANDRegInfo.m_stCmdInfo.stRegCmdData.FnCount);*/

#endif
      
#if 0
        for (int dCnt = 0; dCnt < nRetVal; dCnt++)
        {
          printf("0x%x ", ucData[dCnt]);
        }
        printf(" (Len %d) \n",nRetVal);
#endif

        nRetVal = ManageFragmentation(ucData,ReceivedCANMsg0.dlc, nCANId);
      
        //Yet to receive all fragments...
        if (ERR_DATA_PENDING == nRetVal)
        {
          //There's still time to receive fragments
          if (nTimeoutMS)
          {
          
        
            continue;
          }
          //Timed out... didn't receive any/all packets within 
          //  timeout period
          else
          {
            bLoopExit = TRUE;
            nRetVal = ERR_TIMEOUT;
            LogError(DDISC_ERR_SLOT_RESP_TIMEOUT, __LINE__);
          }
        }
        //Some other error occurred...
        else if (nRetVal < 0)
        {
          //printf("%d: error code",nRetVal);
          bLoopExit = TRUE;
          LogError(DDISC_ERR_DEFRAG, __LINE__);
        }
        //Got the complete packet!
        else
        {
          bLoopExit = TRUE;
          nRetVal = ERR_SUCCESS;
        }
     

     
      }//if(CAN_PKT_MAX_LEN >= ReceivedCANMsg0.dlc)
      //else
      //printf("%d: ERROR:Packet size > buff size. Please increase buffer to %d\n",__LINE__,ReceivedCANMsg0.dlc);
    }
    else
    {
      //DO NOTHING
      nRetVal = -1;
      /*
        switch(err)
        {
        case BCI_HDL_ERR:printf("%d: BCI ERROR:wrong or unknown board handle\n",__LINE__);
        break;
        case BCI_NO:printf("%d: BCI ERROR:no CAN message received/timeout\n",__LINE__);
        break;
        case BCI_PARA_ERR:printf("%d: BCI ERROR:invalid CAN number\n",__LINE__);
        break;
        case BCI_STATE_ERR:printf("%d: BCI ERROR:BCI is in wrong state\n",__LINE__);
        break;
        default:
        printf("%d: BCI ERROR:Unknown error\n",__LINE__);
        break;
        }
      */
      bLoopExit = TRUE;
    }
    
  }//while (FALSE == bLoopExit)
  
      
  // If discovery info was successfully received for this slot ID...
  if (ERR_SUCCESS == nRetVal)
  {
    printf("\t... found device.\n");
    //Parse this info and put it into a file
    if ( (nRetVal = ParseDiscInfo(nCANId) ) < 0)
    {
      LogError(DDISC_ERR_INFO_PARSE, __LINE__);
    }
  }
  else
    LogError((DDISC_ERRS)nRetVal,__LINE__);
  return nRetVal;
}

//Transmit the ping command
int CDdisc::TxPing(int nCANId)
{
  int nRetVal = ERR_SUCCESS;
  CmdAckUnion stCmd = {0};
  DevAddrUnion stDevAddr;
  unsigned char ucTxData[CAN_PKT_MAX_LEN];
  
  memset(&stCmd, 0, sizeof(CmdAckUnion));
  memset(&stDevAddr, 0, sizeof(DevAddrUnion));

  //Setup the command
  SetCmdAckCommand(&stCmd.byCmdAck, BD_GET_DEV_CAP);
  //Setup the function type
  SetFnType(&stDevAddr.usDevAd, FN_CAP);

  //Take care of Endianness
  FixEndian(stDevAddr.usDevAd);
  
  ucTxData[1] = nCANId & 0x00FF;
  ucTxData[0] = (nCANId & 0xFF00) << 8;

  memcpy(&ucTxData[DDISC_TX_PKT_HDR_OFFSET], &stDevAddr, sizeof(DevAddrUnion));
  memcpy(&ucTxData[DDISC_TX_PKT_DATA_OFFSET], &stCmd, sizeof(CmdAckUnion));
  

  BCI_ts_CanMsg ToSendCANMsg0;
  BCI_CreateCANMsg (&ToSendCANMsg0, nCANId,
                    //&ucTxData[2], sizeof(DevAddrUnion) + sizeof(CmdAckUnion), BCI_MFF_11_DAT);
                    &ucTxData[2], sizeof(DevAddrUnion) + sizeof(CmdAckUnion), BCI_MFF_11_DAT);
  CAN0TransmitOK = BCI_TransmitCanMsg (CANBoard, 0, &ToSendCANMsg0);
  
  if (CAN0TransmitOK == BCI_OK)
  {
    nRetVal = CAN_ACK_PACKET_LEN;
  }
  else
  {
    nRetVal = -1;
    /*
      switch(CAN0TransmitOK)
      {
      case BCI_HDL_ERR:printf("%d: BCI ERROR:wrong or unknown board handle\n",__LINE__);
      break;
      case BCI_NO:printf("%d: BCI ERROR:no CAN message received/timeout\n",__LINE__);
      break;
      case BCI_PARA_ERR:printf("%d: BCI ERROR:invalid CAN number\n",__LINE__);
      break;
      case BCI_STATE_ERR:printf("%d: BCI ERROR:BCI is in wrong state\n",__LINE__);
      break;
      default:
      printf("%d: BCI ERROR:Unknown error\n",__LINE__);
      break;
      }
    */
  }
  
#ifdef  BCI_TX_DEL
  
  unsigned long delay = 100000;
  do
  {
    delay--;
  } while (delay > 0);
#endif
  
  return nRetVal;
}
//-----------------------
#endif //BCI_MODE


//Manage fragmented data    
int CDdisc::ManageFragmentation(unsigned char *byAddData, 
                                unsigned int unLen,
                                int nCANId)
{
  int nRetVal = ERR_SUCCESS;
  int nDataLen = 0;
  void *pReadMem = NULL;
  DevAddrUnion stDevAddr;

  //The data length has to be at least 3 bytes (2 bytes of header +
  //  1 byte of data)
  if ( (NULL == byAddData) || (unLen < (sizeof(DevAddrUnion) + 1) ) )
  {
    return ERR_INVALID_ARGS;
  }
  
  memcpy(&stDevAddr.stDevAdD2H, byAddData, sizeof(DevAddrUnion));

  //Check if the response is from the same CAN device
  if ( (nRetVal = VerifyHeader(&stDevAddr, nCANId)) < 0)
  {
    //Unexpected packet
    LogError(DDISC_ERR_PKT_INVALID, __LINE__);
    nRetVal = ERR_DATA_PENDING; 
  }
  else
  {
    if (ERR_SUCCESS == nRetVal)
    {
      //Add data to the fragmentation layer
      if (m_obDataFragment.AddData(byAddData, unLen) < 0)
      {
        nRetVal = ERR_INTERNAL_ERR;
        LogError(DDISC_ERR_ADD_FRAG, __LINE__);
      }
    }
    //#ifdef BCI_MODE 
    //if( nRetVal == ERR_WRONG_CRC ) //for testing
    // nRetVal = ERR_SUCCESS;
    //#endif
    if (ERR_SUCCESS == nRetVal)
    {
      nDataLen = m_obDataFragment.GetCount();
      pReadMem = realloc(m_pbyDiscInfo, nDataLen);

      if (NULL == pReadMem)
      {
        nRetVal = ERR_MEMORY_ERR;
        LogError(DDISC_ERR_INFO_MEM, __LINE__);
      }
      else
      {
        m_pbyDiscInfo = (unsigned char *) pReadMem;
        nRetVal = m_obDataFragment.GetData(m_pbyDiscInfo, nDataLen);
        m_nNrDiscInfo = nDataLen;
      }
    }
  }

  return nRetVal;
}

//Verify if the received packet header is from
//  the same device that we 'pinged'
int CDdisc::VerifyHeader(DevAddrUnion *stDevHdr, int nCANId)
{ 
  int nRetVal = ERR_SUCCESS;

  //Handle Endianess
  FixEndian(stDevHdr->usDevAd);

  //Check if the received Slot ID matches the 
  //  CAN ID and the received function type
  //  matches FN_CAP (device capability)
  if ( (nCANId != GetSlotID(&stDevHdr->usDevAd)) ||
       (FN_CAP != GetFnType(&stDevHdr->usDevAd)) )
  {
    nRetVal = ERR_PROTOCOL;
  }

  return nRetVal;
}

//Parse the received device info
int CDdisc::ParseDiscInfo(int nCANId)
{
  int nRetVal = ERR_SUCCESS;
  DevFnCapUnion stCapInfo;
  int FnType = 0;
  int FnCnt = 0;
  int RevNo = 0;
  char szSlotName[100];
  std::string devName;
  std::string devInfo;
  unsigned char ucNumFuncTypes =  0;
  int nNumFirmInfoBytes = 0;

  // Read from the first byte the number of distinct function types
  if (m_nNrDiscInfo > 0)
  {
    ucNumFuncTypes =  m_pbyDiscInfo[0];

    // Check the total bytes number
    nNumFirmInfoBytes = (m_nNrDiscInfo - 1) - (sizeof(DevFnCapUnion) * ucNumFuncTypes);
    if (nNumFirmInfoBytes < 0)
      nRetVal = ERR_INTERNAL_ERR;
  }
  else
    nRetVal = ERR_INTERNAL_ERR;

  if (nRetVal == ERR_SUCCESS)
  {
    // Parse received data and populate the database with device information
    int nInfoIx = 1;
    for (unsigned char ucFunInd = 1; (nInfoIx < m_nNrDiscInfo) && (ucFunInd <= ucNumFuncTypes); ucFunInd++)
    {
      memcpy(&stCapInfo, &m_pbyDiscInfo[nInfoIx], sizeof(DevFnCapUnion));
      nInfoIx += sizeof(DevFnCapUnion);
      
      //printf("cap 0x%x  ", stCapInfo.usDevCap);
      FixEndian(stCapInfo.usDevCap);
      //printf("cap 0x%x  \n", stCapInfo.usDevCap);
      //ByteSwap((unsigned char *)&stCapInfo.usDevCap, sizeof(stCapInfo.usDevCap));
      
      FnType = GetDiscFnType(&stCapInfo.usDevCap);
      if ((DEV_FN_TYPE)FnType != FN_UNKNOWN)
      {
        FnCnt = GetDiscFnCount(&stCapInfo.usDevCap);
        RevNo = GetDiscRevision(&stCapInfo.usDevCap);

#ifndef MODEL_370XA
        // Skip heater/solenoid boards that are incompatible with this GC
        if (nCANId == GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID
        ||  nCANId == GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID + 1)
        {
          if (m_bIs1500XA_R2 && RevNo != 8)
            continue;
          if (!m_bIs1500XA_R2 && RevNo == 8)
            continue;
        }
#endif

        for (int nCnt = 1; nCnt <= FnCnt; nCnt++)
        {
          devInfo = stCapInfo.stDevCapInfo.szDevName;
          if ((nRetVal = m_obResolveName.GenerateName(szSlotName, FnType, nCANId, nCnt)) < 0)
          {
#ifndef MAKE_FALLBACK
            DEBUG1("UNKNOWN, %s\n", stCapInfo.stDevCapInfo.szDevName);
#endif //#ifndef MAKE_FALLBACK
            if (m_obDBInterface.AddDevNameHWConfigTable("UNKNOWN", 0, nCANId, RevNo, devInfo) < 0)
              LogError(DDISC_ERR_CFG_TBL_UPDATE, __LINE__);
          }
          else
          {
            devName = szSlotName;
            if (m_obDBInterface.AddDevNameHWConfigTable(devName, FnType, nCANId, RevNo, devInfo) < 0)
              LogError(DDISC_ERR_CFG_TBL_UPDATE, __LINE__);
          }
        }
      }
    }

    // Check firmware Version/Date information is present or not
    if (m_nNrDiscInfo > nInfoIx && (sizeof(FIRMWARE_INFO) == nNumFirmInfoBytes))
    {
      FILE *pFile = fopen("/home/Daniel/current_software_info.txt", "a");
      if (pFile)
      { 
        // Firmware Version/Date information is present
        // Get the information and append to "current_software_info.txt" file
        FIRMWARE_INFO FirmInfo;
        memcpy(FirmInfo.szFirmwareVersion, &m_pbyDiscInfo[nInfoIx], FIRMWARE_VERSION_STR_SZ);
        nInfoIx += FIRMWARE_VERSION_STR_SZ;
        memcpy(FirmInfo.szFirmwareDate, &m_pbyDiscInfo[nInfoIx], FIRMWARE_DATE_STR_SZ);

        char *szIO_FirmName = NULL;
        char *szIO_SlotName = NULL;
        switch (nCANId)
        {
#ifdef MODEL_370XA
        case GC370XA_LOI_CAN_MSG_ID:
          szIO_SlotName = "370XA LOI Slot";
          szIO_FirmName = "370XA LOI Firmware";
          break;
#endif //#ifdef MODEL_370XA
        case GC700XP_FPD_G2_BASE_CAN_MSG_ID:
          szIO_SlotName = "FPD G2 Slot 1";
          szIO_FirmName = "FPD G2 Firmware";
          break;
        case GC700XP_FPD_G2_BASE_CAN_MSG_ID + 1:
          szIO_SlotName = "FPD G2 Slot 2";
          szIO_FirmName = "FPD G2 Firmware";
          break;
        case GC700XP_PREAMP_BD_CAN_BASE_MSG_ID:
          szIO_SlotName = "Slot 1";
          if (RevNo < 7)
            szIO_FirmName = "Preamp Firmware";
          else
            szIO_FirmName = "Preamp G2 Firmware";
          break;
        case GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 1:
          szIO_SlotName = "Slot 3";
          if (RevNo < 7)
            szIO_FirmName = "Preamp Firmware";
          else
            szIO_FirmName = "Preamp G2 Firmware";
          break;
        case GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 2:
          szIO_SlotName = "Slot 5";
          if (RevNo < 7)  // Won't happen since G1 can't give 3rd CAN ID, but...
            szIO_FirmName = "Preamp Firmware";
          else
            szIO_FirmName = "Preamp G2 Firmware";
          break;
        case GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID:
          szIO_SlotName = "Slot 2";
          if (RevNo < 7)
            szIO_FirmName = "HeaterSolenoid Firmware";
          else
            szIO_FirmName = "HeaterSolenoid G2 Firmware";
          break;
        case GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID + 1:
          szIO_SlotName = "Slot 4";
          if (RevNo < 7)
            szIO_FirmName = "HeaterSolenoid Firmware";
          else
            szIO_FirmName = "HeaterSolenoid G2 Firmware";
          break;
        case GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID:
          szIO_SlotName = "FFB Slot";
          szIO_FirmName = "FFB Firmware";
          break;
        case GC700XP_GRAPHICAL_LOI_BD_BASE_CAN_MSG_ID:
          szIO_SlotName = "LOI Slot";
          szIO_FirmName = "LOIIRKeyPad Firmware";
          break;
        case GC370XA_ANALYZER_BD_BASE_CAN_MSG_ID:
          szIO_SlotName = "Analyzer Slot";
          szIO_FirmName = "AnalyzerIO Firmware";
          break;
        case GC370XA_IMB_BD_BASE_CAN_MSG_ID:
          szIO_SlotName = "Module Slot";
          szIO_FirmName = "Module Firmware";
          break;
        case GC700XP_BASE_IO_BD_BASE_CAN_MSG_ID:
          szIO_SlotName = "BaseIO Slot";
          if (RevNo < 7)
            szIO_FirmName = "BaseIO Firmware";
          else
            szIO_FirmName = "BaseIO G2 Firmware";
          break;
        default:
          break;
        }
        
        if (szIO_FirmName && szIO_SlotName)
        {
          fprintf(pFile, "%s, %.*s, %.*s, , %s, %d\n", szIO_FirmName, FIRMWARE_VERSION_STR_SZ, 
                  FirmInfo.szFirmwareVersion, FIRMWARE_DATE_STR_SZ, FirmInfo.szFirmwareDate, szIO_SlotName, RevNo);
        }
        else
        {
          fprintf(pFile, "Unknown, %.*s, %.*s, , Slot %d, %d\n", FIRMWARE_VERSION_STR_SZ, 
                  FirmInfo.szFirmwareVersion, FIRMWARE_DATE_STR_SZ, FirmInfo.szFirmwareDate, nCANId, RevNo);
        }

        fclose(pFile);
      }
    }
  }

  return nRetVal;
}


//Error log info
void CDdisc::LogError(DDISC_ERRS eDdiscError, long lLineNr)
{
#ifndef MAKE_FALLBACK
  char *szErrString = NULL;
  
  switch (eDdiscError)
  {
  case DDISC_ERR_CAN_DRV_OPEN:
    szErrString = "DDISC_ELOG: Error opening CAN driver";
    DEBUG1("DDISC_ELOG: Error opening CAN driver.\n");
    break;
  case DDISC_ERR_CAN_DRV_TX:
    szErrString = "DDISC_LOG: Error transmitting data through the CAN driver";
    DEBUG1("DDISC_LOG: Error transmitting data through the CAN driver.\n");
    break;
  case DDISC_ERR_CAN_DRV_RX:
    szErrString = "DDISC_ELOG: Error receiving data from the CAN driver";
    DEBUG1("DDISC_ELOG: Error receiving data from the CAN driver.\n");
    break;
  case DDISC_ERR_ADD_FRAG:
    szErrString = "DDISC_ELOG: Error adding data to the fragmentation layer";
    DEBUG1("DDISC_ELOG: Error adding data to the fragmentation layer.\n");
    break;
  case DDISC_ERR_SELECT:
    szErrString = "DDISC_ELOG: select(...) returned with error";
    DEBUG1("DDISC_ELOG: select(...) returned with error.\n");
    break;
  case DDISC_ERR_DEFRAG:
    szErrString = "DDISC_ELOG: Error occurred when receiving fragmented packets";
    DEBUG1("DDISC_ELOG: Error occurred when receiving fragmented packets.\n");
    break;
  case DDISC_ERR_INFO_MEM:
    szErrString = "DDISC_LOG: Error occurred allocating memory for storing device information";
    DEBUG1("DDISC_LOG: Error occurred allocating memory for storing device information.\n");
    break;
  case DDISC_ERR_SLOT_RESP_TIMEOUT:
    szErrString = "DDISC_LOG: Time out! Error receiving device information";
    DEBUG1("DDISC_LOG: Time out! Error receiving device information.\n");
    break;
  case DDISC_ERR_PKT_INVALID:
    szErrString = "DDISC_LOG: Error! Not a valid device information packet";
    DEBUG1("DDISC_LOG: Error! Not a valid device information packet.\n");
    break;
  case DDISC_ERR_INFO_PARSE:
    szErrString = "DDISC_LOG: Error occurred parsing device information";
    DEBUG1("DDISC_LOG: Error occurred parsing device information.\n");
    break;
  case DDISC_ERR_CFG_TBL_INIT:
    szErrString = "DDISC_LOG: Error initializing the configuration table";
    DEBUG1("DDISC_LOG: Error initializing the configuration table.\n");
    break;
  case DDISC_ERR_CFG_TBL_UPDATE:
    szErrString = "DDISC_LOG: Error updating the configuration table with device information";
    DEBUG1("DDISC_LOG: Error updating the configuration table with device information.\n");
    break;
  case DDISC_ERR_NAME_RESOLUTION:
    szErrString = "DDISC_LOG: Error occurred while resolving device name: Unknown device";
    DEBUG1("DDISC_LOG: Error occurred while resolving device name: Unknown device.\n");
    break;
  case DDISC_ERR_CFG_WRITE:
    szErrString = "DDISC_LOG: Database could not be updated";
    DEBUG1("DDISC_LOG: Database could not be updated.\n");
    break;
  case DDISC_ERR_FAIL_2_FTP_LOI_VERSION_FILE:
    szErrString = "DDISC_LOG: FAILED to get LOI version details";
    DEBUG1("DDISC_LOG: FAILED to get LOI version details.\n");
    break;
  
  default:
    szErrString = "DDISC_LOG: Unknown error";
    DEBUG1("DDISC_LOG: Unknown error.\n");
    break;
  }

  SysLog(ERR_WARNING, ERR_GENERAL, __FILE__, lLineNr, szErrString);
#endif //#ifndef MAKE_FALLBACK
}


#ifdef BCI_MODE
#define DEF_CAN_DEV_PATH    "/dev/can0"
#else   //Not BCI_MODE
#define DEF_CAN_DEV_PATH    "/dev/can1"
#endif

void PrintUsage()
{
  printf("Application usage:\n");
  printf("<app_name> -d <dev_path>\n");
  printf("Eg:\n");
  printf("ddisc -d /dev/can1\n");
}

int main(int argc, char *argv[])
{
  char *pcDevPath = NULL;
  int nOptVal = 0;

#ifndef MAKE_FALLBACK
  InstallDebug(argc, argv);
#endif //#ifndef MAKE_FALLBACK
  
  while (argv[optind] != NULL)
  {
    nOptVal = getopt(argc, argv, "vd:");
    
    switch (nOptVal)
    {
    case 'd':
      pcDevPath = optarg;
      break;
    
    case 'v':
      break;

    default:
      printf("Invalid option.\n");
      PrintUsage();
      return -1;  
    }
  }   

  if (NULL == pcDevPath)
  {
    pcDevPath = DEF_CAN_DEV_PATH;
  }

  CDdisc obDiscovery;
  
  if (obDiscovery.HandleDiscovery(pcDevPath) < 0)
  {
    exit(1);
  }
  
  return 0;
}

/* Function for pinging the SPI expansion slots 1 and 2 in the 700XA Backplane
 */
int CDdisc::PingSPI()
{
  // Get Card Type for ROC Exapnsion Slot 1
  switch (GetROCCardType(ROC_EXPANSION_SLOT_1))
  {
  default:
  case ROC_UNKNOWN_CARD:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Unknown/No Card!");
#endif //#ifndef MAKE_FALLBACK
    break;

  case ROC_DISCRETE_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Discrete Output.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_1:DIGI_OUT_1"),
                                             FN_DIGI_OUT,
                                             0,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_1:DIGI_OUT_2"),
                                             FN_DIGI_OUT,
                                             0,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_1:DIGI_OUT_3"),
                                             FN_DIGI_OUT,
                                             0,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_1:DIGI_OUT_4"),
                                             FN_DIGI_OUT,
                                             0,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_1:DIGI_OUT_5"),
                                             FN_DIGI_OUT,
                                             0,
                                             1,
                                             std::string ("Digital Output"));
    break;

  case ROC_RS232:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Comm RS232."); 
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("SERIAL:EXP_SLOT_1:SERIAL_1"), 
                                             FN_SERIAL, 
                                             0, 
                                             1, 
                                             std::string ("Serial IO (RS232)"));
    break;

  case ROC_RS485:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Comm RS485.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("SERIAL:EXP_SLOT_1:SERIAL_1"), 
                                             FN_SERIAL, 
                                             0, 
                                             1, 
                                             std::string ("Serial IO (RS422/RS485)"));
    break;

  case ROC_DISCRETE_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Discrete Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_1"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_2"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_3"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_4"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_5"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_6"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_7"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_1:DIGI_IN_8"),
                                             FN_DIGI_IN,
                                             0,
                                             1,
                                             std::string ("Digital Input"));
    break;

  case ROC_MODEM:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Comm Modem."); 
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("SERIAL:EXP_SLOT_1:SERIAL_1"), 
                                             FN_SERIAL, 
                                             0, 
                                             1, 
                                             std::string ("Serial IO (MODEM)"));
#endif //#ifndef MAKE_FALLBACK
    break;

  case ROC_ANALOG_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Analog Output."); 
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_1:ANA_OUT_1"), 
                                             FN_ANA_OUT, 
                                             0, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_1:ANA_OUT_2"), 
                                             FN_ANA_OUT, 
                                             0, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_1:ANA_OUT_3"), 
                                             FN_ANA_OUT, 
                                             0, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_1:ANA_OUT_4"), 
                                             FN_ANA_OUT, 
                                             0, 
                                             1, 
                                             std::string ("Analog Output"));
    break;

  case ROC_ANALOG_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 1 - Analog Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_1:ANA_IN_1"),
                                             FN_ANA_IN,
                                             0,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_1:ANA_IN_2"),
                                             FN_ANA_IN,
                                             0,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_1:ANA_IN_3"),
                                             FN_ANA_IN,
                                             0,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_1:ANA_IN_4"),
                                             FN_ANA_IN,
                                             0,
                                             1,
                                             std::string ("Analog Input"));
    break;  
  }

  // Get Card Type for ROC Exapnsion Slot 2
  switch (GetROCCardType(ROC_EXPANSION_SLOT_2))
  {
  default:
  case ROC_UNKNOWN_CARD:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Unknown/No Card!");
#endif //#ifndef MAKE_FALLBACK
    break;

  case ROC_DISCRETE_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Discrete Output.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_2:DIGI_OUT_1"),
                                             FN_DIGI_OUT,
                                             1,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_2:DIGI_OUT_2"),
                                             FN_DIGI_OUT,
                                             1,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_2:DIGI_OUT_3"),
                                             FN_DIGI_OUT,
                                             1,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_2:DIGI_OUT_4"),
                                             FN_DIGI_OUT,
                                             1,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_2:DIGI_OUT_5"),
                                             FN_DIGI_OUT,
                                             1,
                                             1,
                                             std::string ("Digital Output"));
    break;

  case ROC_RS232:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Comm RS232.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("SERIAL:EXP_SLOT_2:SERIAL_1"), 
                                             FN_SERIAL, 
                                             1, 
                                             1, 
                                             std::string ("Serial IO (RS232)"));
    break;

  case ROC_RS485:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Comm RS485."); 
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("SERIAL:EXP_SLOT_2:SERIAL_1"), 
                                             FN_SERIAL, 
                                             1, 
                                             1, 
                                             std::string ("Serial IO (RS422/RS485)"));
    break;

  case ROC_DISCRETE_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Discrete Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_1"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_2"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_3"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_4"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_5"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_6"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_7"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_2:DIGI_IN_8"),
                                             FN_DIGI_IN,
                                             1,
                                             1,
                                             std::string ("Digital Input"));
    break;

  case ROC_MODEM:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Comm Modem."); 
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("SERIAL:EXP_SLOT_2:SERIAL_1"), 
                                             FN_SERIAL, 
                                             1, 
                                             1, 
                                             std::string ("Serial IO (Modem)"));
#endif //#ifndef MAKE_FALLBACK
    break;

  case ROC_ANALOG_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Analog Output."); 
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_2:ANA_OUT_1"), 
                                             FN_ANA_OUT, 
                                             1, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_2:ANA_OUT_2"), 
                                             FN_ANA_OUT, 
                                             1, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_2:ANA_OUT_3"), 
                                             FN_ANA_OUT, 
                                             1, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_2:ANA_OUT_4"), 
                                             FN_ANA_OUT, 
                                             1, 
                                             1, 
                                             std::string ("Analog Output"));
    break;
 
  case ROC_ANALOG_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 2 - Analog Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_2:ANA_IN_1"),
                                             FN_ANA_IN,
                                             1,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_2:ANA_IN_2"),
                                             FN_ANA_IN,
                                             1,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_2:ANA_IN_3"),
                                             FN_ANA_IN,
                                             1,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_2:ANA_IN_4"),
                                             FN_ANA_IN,
                                             1,
                                             1,
                                             std::string ("Analog Input"));
    break;
  }

  // Get Card Type for ROC Exapnsion Slot 3
  switch (GetROCCardType(ROC_EXPANSION_SLOT_3))
  {
  default:
  case ROC_RS232:   // Not supported in slots 3 & 4
  case ROC_RS485:
  case ROC_MODEM:
  case ROC_UNKNOWN_CARD:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 3 - Unknown/No Card!");
#endif //#ifndef MAKE_FALLBACK
    break;

  case ROC_DISCRETE_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 3 - Discrete Output.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_3:DIGI_OUT_1"),
                                             FN_DIGI_OUT,
                                             2,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_3:DIGI_OUT_2"),
                                             FN_DIGI_OUT,
                                             2,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_3:DIGI_OUT_3"),
                                             FN_DIGI_OUT,
                                             2,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_3:DIGI_OUT_4"),
                                             FN_DIGI_OUT,
                                             2,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_3:DIGI_OUT_5"),
                                             FN_DIGI_OUT,
                                             2,
                                             1,
                                             std::string ("Digital Output"));
    break;

  case ROC_DISCRETE_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 3 - Discrete Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_1"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_2"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_3"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_4"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_5"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_6"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_7"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_3:DIGI_IN_8"),
                                             FN_DIGI_IN,
                                             2,
                                             1,
                                             std::string ("Digital Input"));
    break;

  case ROC_ANALOG_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 3 - Analog Output."); 
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_3:ANA_OUT_1"), 
                                             FN_ANA_OUT, 
                                             2, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_3:ANA_OUT_2"), 
                                             FN_ANA_OUT, 
                                             2, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_3:ANA_OUT_3"), 
                                             FN_ANA_OUT, 
                                             2, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_3:ANA_OUT_4"), 
                                             FN_ANA_OUT, 
                                             2, 
                                             1, 
                                             std::string ("Analog Output"));
    break;
 
  case ROC_ANALOG_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 3 - Analog Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_3:ANA_IN_1"),
                                             FN_ANA_IN,
                                             2,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_3:ANA_IN_2"),
                                             FN_ANA_IN,
                                             2,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_3:ANA_IN_3"),
                                             FN_ANA_IN,
                                             2,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_3:ANA_IN_4"),
                                             FN_ANA_IN,
                                             2,
                                             1,
                                             std::string ("Analog Input"));
    break;
  }

  // Get Card Type for ROC Exapnsion Slot 4
  switch (GetROCCardType(ROC_EXPANSION_SLOT_4))
  {
  default:
  case ROC_RS232:   // Not supported in slots 3 & 4
  case ROC_RS485:
  case ROC_MODEM:
  case ROC_UNKNOWN_CARD:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 4 - Unknown/No Card!");
#endif //#ifndef MAKE_FALLBACK
    break;

  case ROC_DISCRETE_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 4 - Discrete Output.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_4:DIGI_OUT_1"),
                                             FN_DIGI_OUT,
                                             3,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_4:DIGI_OUT_2"),
                                             FN_DIGI_OUT,
                                             3,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_4:DIGI_OUT_3"),
                                             FN_DIGI_OUT,
                                             3,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_4:DIGI_OUT_4"),
                                             FN_DIGI_OUT,
                                             3,
                                             1,
                                             std::string ("Digital Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_OUT:EXP_SLOT_4:DIGI_OUT_5"),
                                             FN_DIGI_OUT,
                                             3,
                                             1,
                                             std::string ("Digital Output"));
    break;

  case ROC_DISCRETE_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 4 - Discrete Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_1"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_2"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_3"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_4"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_5"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_6"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_7"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("DIGI_IN:EXP_SLOT_4:DIGI_IN_8"),
                                             FN_DIGI_IN,
                                             3,
                                             1,
                                             std::string ("Digital Input"));
    break;

  case ROC_ANALOG_OUTPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 4 - Analog Output."); 
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_4:ANA_OUT_1"), 
                                             FN_ANA_OUT, 
                                             3, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_4:ANA_OUT_2"), 
                                             FN_ANA_OUT, 
                                             3, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_4:ANA_OUT_3"), 
                                             FN_ANA_OUT, 
                                             3, 
                                             1, 
                                             std::string ("Analog Output"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_OUT:EXP_SLOT_4:ANA_OUT_4"), 
                                             FN_ANA_OUT, 
                                             3, 
                                             1, 
                                             std::string ("Analog Output"));
    break;
 
  case ROC_ANALOG_INPUT:
#ifndef MAKE_FALLBACK
    DEBUG1 ("PingSPI: Expansion Slot 4 - Analog Input.");
#endif //#ifndef MAKE_FALLBACK
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_4:ANA_IN_1"),
                                             FN_ANA_IN,
                                             3,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_4:ANA_IN_2"),
                                             FN_ANA_IN,
                                             3,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_4:ANA_IN_3"),
                                             FN_ANA_IN,
                                             3,
                                             1,
                                             std::string ("Analog Input"));
    m_obDBInterface.AddDevNameHWConfigTable (std::string ("ANA_IN:EXP_SLOT_4:ANA_IN_4"),
                                             FN_ANA_IN,
                                             3,
                                             1,
                                             std::string ("Analog Input"));
    break;
  }

  return 0;
}

/*
  Card Type       Ohms    Volts   Lower   Upper
  ---------------------------------------------
  Discrete Output 346.8   0.18    0.15    0.21
  RS232           590     0.30    0.27    0.33
  RS485           881     0.42    0.39    0.45
  Discrete Input  1737    0.71    0.68    0.74
  Modem           4120    1.21    1.18    1.24
  Analog Input    ????    ????    ????    ????
  Analog Output   9760    1.72    1.69    1.75


  8 bit ADC gives us 0.01 V of resolution per bit (2.5V Ref / 256). For card 
  type detection, check if the voltage is within +-0.3V of the calculated value  

*/
ROC_CARD_TYPE CDdisc::GetROCCardType (ROC_CARD_SLOT eSlot)
{
  return m_obDBInterface.GetROCCardType (eSlot);
  
  // Comment following lines till we fix hardware issues related to CPU SPI bus and Flex Cable interference
  /*
  // TODO: Need supports for slots 3 & 4 for 1500XA R2 if we ever reinstate this code
    
  ROC_CARD_TYPE eType = ROC_UNKNOWN_CARD; // Default - Unknown card!
  float fType = -1; // Card type reading in Volts
  CCPU_ADC1_AD7908 obCPU_ADC1;
  CCPU_ADC2_AD7908 obCPU_ADC2;
  int nRet = -1, nAttempts = 0;
  
  // Get card type voltage for the ROC expansion slot
  switch (eSlot)
  {
  case ROC_EXPANSION_SLOT_1: // ROC Slot 1 (Upper)
  do 
  {
  nRet = obCPU_ADC1.GetReading (5, fType); // ROC 1 hooked up to Channel 5 / ADC 1
  nAttempts++;
  }
  while (nRet == -1 && nAttempts < 10); // Try and read till we get a valid reading!
  break;

  case ROC_EXPANSION_SLOT_2: // ROC Slot 2 (Lower)
  do 
  {
  nRet = obCPU_ADC2.GetReading (1, fType); // ROC 2 hooked up to Channel 1 / ADC 2
  nAttempts++;
  }
  while (nRet == -1 && nAttempts < 10); // Try and read till we get a valid reading!
    
  break;
  }

  #ifndef MAKE_FALLBACK
  DEBUG1 ("Card Type Voltage for ROC Slot# %d = %7.2g", bySlotID, fType); 
  #endif //#ifndef MAKE_FALLBACK

  // Find out the card type, use chart above...
  if ( fType >= 0.15 && fType <= 0.21 )
  eType = ROC_DISCRETE_OUTPUT;
  else if ( fType >= 0.27 && fType <= 0.33 )
  eType = ROC_RS232;
  else if ( fType >= 0.39 && fType <= 0.45 )
  eType = ROC_RS485;
  else if ( fType >= 0.68 && fType <= 0.74 )
  eType = ROC_DISCRETE_INPUT;
  else if ( fType >= 1.18 && fType <= 1.24 )
  eType = ROC_MODEM;
  else if ( fType >= 1.69 && fType <= 1.75 )
  eType = ROC_ANALOG_OUTPUT;
  
  return eType;
  */
}

#ifdef MODEL_370XA
int CDdisc::PingUDP()
{
  int nRetVal = ERR_SUCCESS;

  CUDPClient oUDPClient;
  if( oUDPClient.Initialize() == CUDPClient::RET_SUCCESS )
  {
    LOI370XA_CMMD_HEADER_STRUCT stDevCapCmmd;
    stDevCapCmmd.byCmmd = LOI370XA_GET_DEV_CAPABILITY;
    stDevCapCmmd.bySeqNum = oUDPClient.GetNewSeqNumber();
    int nNumTxBytes = sizeof( stDevCapCmmd );
    char byRxdData[BUFLEN] = {0};

    if ( oUDPClient.SendData((char*) &stDevCapCmmd, nNumTxBytes) == CUDPClient::RET_SUCCESS )
    {
      int nNumRxBytes = 0;
      if ( (nNumRxBytes = oUDPClient.GetData(byRxdData, BUFLEN)) > 0  )
      {
        LOI370XA_CMMD_HEADER_STRUCT * pstDevCapRespHeader = (LOI370XA_CMMD_HEADER_STRUCT *) byRxdData;
        if( ( pstDevCapRespHeader->byCmmd == stDevCapCmmd.byCmmd ) &&
            ( pstDevCapRespHeader->bySeqNum == stDevCapCmmd.bySeqNum ) )
        {
          void * pReadMem = realloc( m_pbyDiscInfo, nNumRxBytes-2 );
          if( pReadMem )
          {
            m_pbyDiscInfo = (unsigned char *) pReadMem;
            m_nNrDiscInfo = nNumRxBytes - sizeof(LOI370XA_CMMD_HEADER_STRUCT);
            memcpy( m_pbyDiscInfo, & byRxdData[sizeof(LOI370XA_CMMD_HEADER_STRUCT)], m_nNrDiscInfo );
#define LOI370XA_SLOT_ID 3
            if ( (nRetVal = ParseDiscInfo(LOI370XA_SLOT_ID)) < 0)
            {
              LogError(DDISC_ERR_INFO_PARSE, __LINE__);
            }
          }
          else
          {
            nRetVal = ERR_MEMORY_ERR;
            LogError(DDISC_ERR_INFO_MEM, __LINE__);
          }
        }
      }
    }
  }

  return nRetVal;
}
#endif //#ifdef MODEL_370XA
