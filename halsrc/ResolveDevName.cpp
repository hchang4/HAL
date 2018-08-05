/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: ResolveDevName.cpp
 * *
 * *  Description: Device name resolution and generation APIs
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
 * *  $Id: ResolveDevName.cpp,v 1.27 2016/11/01 13:05:35 emrsn\jbeatty Exp $
 * *  $Log: ResolveDevName.cpp,v $
 * *  Revision 1.27  2016/11/01 13:05:35  emrsn\jbeatty
 * *  Added support for FPD G2
 * *
 * *  Revision 1.26  2016/08/30 09:57:09  emrsn\jbeatty
 * *  Mods for 1500XA R2 support
 * *
 * *  Revision 1.25  2016/08/09 17:09:06  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.24  2016/01/14 21:32:01  emrsn\rrthomas
 * *  Added a device type with which we can broadcast a Cycle Clock Start Message to all the Preamp boards at one shot. This will make it possible for Preaming Streaming objects present on different physical boards to start the analysis cycle at the exact same time. If we have four detectors in a Cycle Clock and if we use individual start messages to start each streaming object, then it is possible that they will start in the order in which we sent them the start messages. If for some reason one or more of these messages are delayed, we will end up with different detectors that start a different times.
 * *
 * *  For this added the following -
 * *  1. Added a way to associate each Preamp Streaming Object (or detector) with a Cycle Clock. Added a function to set the Cycle Clock for each detector.
 * *  2. New Function Type called Cycle Clock Sync. This will reside in the Preamp Boards (G1 and G2) and can be accessible as CAN address "1". All preamps will receive this message with a Cycle Clock number (1/2/3/4). When this message is recevied all the detectors associated with that particular cycle clock will start streaming data to the main CPU.
 * *
 * *  Revision 1.23  2015/05/04 14:49:28  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.22  2014/06/26 10:01:00  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.21  2013/01/30 06:18:50  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.20  2012/03/21 06:49:00  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.19  2012/01/03 13:08:27  emrsn\rsaha
 * *  Added APIs to support to communicate with IMB.
 * *
 * *  Revision 1.18  2011/07/13 11:01:37  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.17  2010/06/10 20:13:29  emrsn\rsaha
 * *  Added HAL interfaces for Pressure read, Serial RS232/485 control support and diagnostics details of Analyzer IO card.
 * *
 * *  Revision 1.16  2010/03/31 06:51:31  emrsn\rsaha
 * *  Revised to resolve 370XA Analyzer IO slot Id to slot name.
 * *
 * *  Revision 1.15  2009/12/17 14:12:27  emrsn\rsaha
 * *  Added support for FPD of BaseIO.
 * *
 * *  Revision 1.14  2009/04/15 13:34:25  epm-na\rsaha
 * *  Added support for FID Ignition and Gain control.
 * *
 * *  Revision 1.13  2008/11/25 00:12:05  epm-na\rsaha
 * *  Added new function type DIAGNOSTIC. At present Preamp, Heater Solenoid and BaseIO supports this diagnostic functionality.
 * *
 * *  Revision 1.12  2008/11/16 00:31:02  epm-na\rthomas
 * *  Changed the "Slot ID" for the ROC expansion slots from 0 and 1 to 1 and 2 to make it consistent with the rocio1 and 2 device names.
 * *
 * *  Revision 1.11  2008/10/22 21:56:24  epm-na\rsaha
 * *  Added two function type FFB_STATUS and FFB_COMMAND.
 * *  FFB_STATUS  - This function will be used to send the GC status information to the FFB board.
 * *  FFB_COMMAND - This function will be used to receive the control information from a FFB board to configure GC.
 * *
 * *  Revision 1.10  2008/10/09 23:43:07  epm-na\rthomas
 * *  Added function type and names for the Foundation Fieldbus and Graphical LOI board types, so that device discovery can formulate intelligent names, function types and slots for the Foundation Fieldbus card and the Graphical LOI board.
 * *
 * *  Revision 1.9  2008/03/06 20:47:04  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.8  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.7  2007/04/05 13:20:30  epm-na\rthomas
 * *  LTLOI: Added LTLOI function and corresponding class to access the LTLOI
 * *  hardware through the Base IO board.
 * *
 * *  Revision 1.6  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DevProtocol.h"
  
#include "debug.h"
#include "ResolveDevName.h"
#include "CommTypes.h"

#ifdef WIN32
#define strtok_r strtok_s
#endif

#define USING_STRTOK_R // TODO: Use strtok_r instead of strtok. The latter is not thread safe! TODO: strtok_r behaves goofy (
// modifies the input string, when it is not supposed to). Need to check if strtok_r is ok to use!

const DeviceTypeMapStruct CResolveDevName::stDevTypeMap[MAX_NUM_DEV_FUNCTIONS] =
{
  {"ANA_IN", FN_ANA_IN},                // Analog In
  {"ANA_OUT", FN_ANA_OUT},              // Analog Out 
  {"DIGI_IN", FN_DIGI_IN},              // Digital Input
  {"DIGI_OUT", FN_DIGI_OUT},            // Digital Output
  {"PREAMP_STR", FN_PREAMP_STR},        // Bridge Preamp streaming function (on dual channel bridge preamp board)
  {"PREAMP_CFG", FN_PREAMP_CFG},        // Bridge Preamp configuration (on dual channel bridge preamp board)
  {"RTD", FN_RTD},                      // RTD (on Heater/Solenoid ctrl board)
  {"HTR_CTRL", FN_HTR_CTRL},            // Heater Channel (On Heater/Solenoid board)
  {"SOL", FN_SOL},                      // Solenoid (on Heater/Solenoid board)
  {"SERIAL", FN_SERIAL},                // Serial COM
  {"EPC", FN_EPC},                      // EPC Controller (On Base IO board)
  {"LTLOI", FN_LTLOI},                  // LTLOI (On Base IO board)
  {"FFB_STATUS", FN_FFB_STATUS},        // Funtion to send GC status information to FFB board.
  {"FFB_COMMAND", FN_FFB_COMMAND},      // Funtion to get remote commands from FFB board to configure GC.
  {"GRAPHICAL_LOI", FN_GRAPHICAL_LOI},  // Local Operator Interface (Graphical)
  {"DIAGNOSTIC", FN_DIAGNOSTIC},        // Function to retrive Diagnostic information from all boards.
  {"FID", FN_FID},                      // Function to control FID Ignition and Gain.
  {"FPD", FN_FPD},                      // Function to control FPD AutoZero.
  {"PRESSURE", FN_PRESSURE},            // Function to retrieve Sample pressure.
  {"CTRL", FN_CTRL},                    // Function for Control pins - e.g. - RS232/RS485 toggle for 370XA.
  {"IMB_COMM", FN_IMB_COMM},            // Function for IMB board communication.
  {"CC", FN_CYCLE_CLOCK_SYNC},          // Function for Synchronizing Cycle Clock across multiple boards
  {"FPD_G2", FN_FPD_G2}                 // Function to control G2 FPD
};

//The 'bySlotID' field is hard-coded to map into a specific Slot Name
const DeviceAddrMapStruct CResolveDevName::stSlotIDMap[MAX_NUM_SLOTS] =
{
  {"EXP_SLOT_1", SPI_COMM, 1},    // Expansion Slot 1 on SPI Bus
  {"EXP_SLOT_2", SPI_COMM, 2},    // Expansion Slot 2 on SPI Bus
  {"EXP_SLOT_3", SPI_COMM, 3},    // Expansion Slot 3 on SPI Bus
  {"EXP_SLOT_4", SPI_COMM, 4},    // Expansion Slot 4 on SPI Bus
  {"SLOT_1", CAN_COMM, GC700XP_PREAMP_BD_CAN_BASE_MSG_ID},          // Slot 1 on CAN Bus
  {"SLOT_2", CAN_COMM, GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID},    // Slot 2 on CAN Bus
  {"SLOT_3", CAN_COMM, GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 1},      // Slot 3 on CAN Bus
  {"SLOT_4", CAN_COMM, GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID + 1},// Slot 4 on CAN Bus
  {"SLOT_5", CAN_COMM, GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 2},      // Slot 5 on CAN Bus
  {"SLOT_BASE_IO", CAN_COMM, GC700XP_BASE_IO_BD_BASE_CAN_MSG_ID},   // Base IO Slot on CAN Bus
  {"FIELDBUS_SLOT", CAN_COMM, GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID}, // CAN Slot on FF Board.
  {"LOI_SLOT", CAN_COMM, GC700XP_GRAPHICAL_LOI_BD_BASE_CAN_MSG_ID}, // CAN Slot on LTLOI / Graphical LOI connector
  {"ANALYZER_SLOT", CAN_COMM, GC370XA_ANALYZER_BD_BASE_CAN_MSG_ID}, // CAN Slot on 370XA Analyzer board.
  {"IMB_SLOT", CAN_COMM, GC370XA_IMB_BD_BASE_CAN_MSG_ID}, // CAN Slot on 370XA IMB.
  {"REMOTE_EXP_SLOT_3", CAN_COMM, 11},  // Remote Expansion Slot 3 on CAN Bus
  {"REMOTE_EXP_SLOT_4", CAN_COMM, 12},  // Remote Expansion Slot 4 on CAN Bus
  {"370XA_LOI_SLOT", UDP_COMM, 3},       // 370XA LOI Slot
  {"BROADCAST", CAN_COMM, CYCLE_CLOCK_SYNC_CAN_MSG_ID},      // 370XA LOI Slot
  {"BACKPLANE", FID_COMM, 26},
  {"FPD_G2_1", CAN_COMM, GC700XP_FPD_G2_BASE_CAN_MSG_ID},
  {"FPD_G2_2", CAN_COMM, GC700XP_FPD_G2_BASE_CAN_MSG_ID + 1},
};

int CResolveDevName::ResolveName(char* pszDevName, unsigned char* pbyFnType, unsigned char* pbySlotID, unsigned char* pbyFnEnum, COMM_TYPE* peCommType)
#ifndef USING_STRTOK_R
{
  int nRetVal = ERR_SUCCESS;
  int nCount = 0, nPos = 0;
  char* pszFnType = NULL;
  char* pszSlotID = NULL;
  char* pszFnEnum = NULL;
  char szSeparator[] = ":";
  char szDevName [MAX_DEV_NAME] = "";

  // Check for invalid pointers
  if (pszDevName == NULL ||
      pbyFnType == NULL ||
      pbySlotID == NULL ||
      pbyFnEnum == NULL ||
      peCommType == NULL)
  {
    nRetVal = ERR_INVALID_ARGS;
    DEBUG2("ResolveName: Invalid arguments! One or more pointers are NULL!");
  }

  DEBUG3("ResolveName: Dev name = %s...", pszDevName);

  if (nRetVal == ERR_SUCCESS)
  {
    // Initialize 
    *pbyFnType = *pbyFnEnum = *pbySlotID = (unsigned char) -1;

    // Copy Dev Name to a temp variable. strtok_r always overwrites input string
    strncpy (szDevName, pszDevName, MAX_DEV_NAME);

    //DEBUG3("ResolveName: Before 1st strtok_r!");

    // Establish string and get the first token (Function Type)
    pszFnType = strtok(szDevName, szSeparator);

    //DEBUG3("ResolveName: Before 2nd strtok_r!");

    // Get the second token (Slot ID)
    pszSlotID = strtok(NULL, szSeparator);

    //DEBUG3("ResolveName: Before 3rd strtok_r!");

    // Get the third token (Function Enum)
    pszFnEnum = strtok(NULL, szSeparator);

    DEBUG3("ResolveName: After 3rd strtok_r!");

    // Check if the device name is in FnType:SlotID:FnEnum format
    if (pszFnType != NULL && pszSlotID != NULL && pszFnEnum != NULL)
    {
      DEBUG3("ResolveName: FnType - %s, SlotID - %s FnEnum - %s...", pszFnType, pszSlotID, pszFnEnum);

      // Get the Function Type - Check if Fn Type is in our list of 
      // device functions.
      for (nCount = 0; nCount < MAX_NUM_DEV_FUNCTIONS; ++nCount)
      {
        if (strcmp (pszFnType, stDevTypeMap[nCount].szName) == 0)
        {
          *pbyFnType = stDevTypeMap[nCount].byEnum;
          break;
        }
      }

      // Get the Slot ID / CAN Message ID / SPI ID - Check if Slot ID is in our list of Slot IDs
      for (nCount = 0; nCount < MAX_NUM_SLOTS; ++nCount)
      {
        if (strcmp (pszSlotID, stSlotIDMap[nCount].szSlotName) == 0)
        {
          *pbySlotID = stSlotIDMap[nCount].bySlotID;
          *peCommType = stSlotIDMap[nCount].eCommType;
          break;
        }
      }

      // Get the function enumeration
      if (strcmp(pszFnEnum, "FPD_G2_1") == 0)
        *pbyFnEnum = 1;
      else
      {
        if ((nPos = (int)strcspn(pszFnEnum, "0123456789")) < (int)strlen(pszFnEnum))
          *pbyFnEnum = (unsigned char)atoi(pszFnEnum + nPos);
      }

      // Check if we got what we wanted
      if (*pbyFnType == (unsigned char) -1 ||
          *pbySlotID == (unsigned char) -1 ||
          *pbyFnEnum == (unsigned char) -1)
      {
        nRetVal = ERR_INVALID_DEV_NAME;
        DEBUG2("ResolveName: Unable to resolve device name - %s! One or more device name components invalid!", pszDevName);
      }
    }
    else
    {
      nRetVal = ERR_INVALID_DEV_NAME;
      DEBUG2("ResolveName: Unable to resolve device name - %s! Device name not in FnType:Slot:FnEnum Format!", pszDevName);
    }
  }
  
  return nRetVal;
}
#else
{
  int nRetVal = ERR_SUCCESS;
  int nCount = 0, nPos = 0;
  char* pszFnType = NULL;
  char* pszSlotID = NULL;
  char* pszFnEnum = NULL;
  char szSeparator[] = ":";
  char szDevName [MAX_DEV_NAME] = {0}; // Local copy of Dev Name
  char szTemp[MAX_DEV_NAME] = {0};    // Memory for strtok_r to tokenize

  // Check for invalid pointers
  if (pszDevName == NULL ||
      pbyFnType == NULL ||
      pbySlotID == NULL ||
      pbyFnEnum == NULL ||
      peCommType == NULL)
  {
    nRetVal = ERR_INVALID_ARGS;
    DEBUG2("ResolveName: Invalid arguments! One or more pointers are NULL!");
  }

  DEBUG2("ResolveName: Dev name = %s...", pszDevName);

  if (nRetVal == ERR_SUCCESS)
  {
    // Initialize 
    *pbyFnType = *pbyFnEnum = *pbySlotID = (unsigned char) -1;

    // Copy Dev Name to a temp variable. strtok_r always overwrites input string
    strncpy(szDevName, pszDevName, MAX_DEV_NAME - 1);

    //DEBUG3("ResolveName: Before 1st strtok_r!");

    // Establish string and get the first token (Function Type)
    pszFnType = strtok_r(szDevName, szSeparator, (char**)&szTemp);

    //DEBUG3("ResolveName: Before 2nd strtok_r!");

    // Get the second token (Slot ID)
    pszSlotID = strtok_r(NULL, szSeparator, (char**)&szTemp);

    //DEBUG3("ResolveName: Before 3rd strtok_r!");

    // Get the third token (Function Enum)
    pszFnEnum = strtok_r(NULL, szSeparator, (char**)&szTemp);

    //DEBUG3("ResolveName: After 3rd strtok_r!");

    // Check if the device name is in FnType:SlotID:FnEnum format
    if (pszFnType != NULL && pszSlotID != NULL && pszFnEnum != NULL)
    {
      DEBUG3("ResolveName: FnType - %s, SlotID - %s FnEnum - %s...", pszFnType, pszSlotID, pszFnEnum);

      // Get the Function Type - check if Fn Type is in our list of device functions
      for (nCount = 0; nCount < MAX_NUM_DEV_FUNCTIONS; ++nCount)
      {
        if (strcmp(pszFnType, stDevTypeMap[nCount].szName) == 0)
        {
          *pbyFnType = stDevTypeMap[nCount].byEnum;
          break;
        }
      }

      // Get the Slot ID / CAN Message ID / SPI ID - Check if Slot ID is in our list of Slot IDs
      for (nCount = 0; nCount < MAX_NUM_SLOTS; ++nCount)
      {
        if (strcmp(pszSlotID, stSlotIDMap[nCount].szSlotName) == 0)
        {
          *pbySlotID = stSlotIDMap[nCount].bySlotID;
          *peCommType = stSlotIDMap[nCount].eCommType;
          break;
        }
      }

      // Get the function enumeration
      if (strcmp(pszFnEnum, "FPD_G2_1") == 0)
        *pbyFnEnum = 1;
      else
      {
        if ((nPos = (int)strcspn(pszFnEnum, "0123456789")) < (int)strlen(pszFnEnum))
          *pbyFnEnum = (unsigned char)atoi(pszFnEnum + nPos);
      }

      // Check if we got what we wanted
      if (*pbyFnType == (unsigned char) -1 ||
          *pbySlotID == (unsigned char) -1 ||
          *pbyFnEnum == (unsigned char) -1)
      {
        nRetVal = ERR_INVALID_DEV_NAME;
        DEBUG2("ResolveName: Unable to resolve device name - %s! One or more device name components invalid!", pszDevName);
      }
    }
    else
    {
      nRetVal = ERR_INVALID_DEV_NAME;
      DEBUG2("ResolveName: Unable to resolve device name - %s! Device name not in FnType:Slot:FnEnum Format!", pszDevName);
    }
  }
  
  return nRetVal;
}
#endif


int CResolveDevName::GenerateName(char* pszDevName, 
                                  unsigned char byFnType, 
                                  unsigned char bySlotID, 
                                  unsigned char byFnEnum)
{
  BOOL bFoundFn = FALSE;
  BOOL bFoundSl = FALSE;

  //Find the function information index position
  for (int nFnIx = 0; nFnIx < MAX_NUM_DEV_FUNCTIONS; nFnIx++)
  {
    if (byFnType == stDevTypeMap[nFnIx].byEnum)
    {
      //Re-map it to the index
      byFnType = nFnIx;
      bFoundFn = TRUE;
      break;
    }
  }

  //Find the slot information index position
  for (int nSlIx = 0; nSlIx < MAX_NUM_SLOTS; nSlIx++)
  {
    if (bySlotID == stSlotIDMap[nSlIx].bySlotID)
    {
      //Re-map it to the index
      bySlotID = nSlIx;
      bFoundSl = TRUE;
      break;
    }
  }

  if ( (NULL == pszDevName) || (byFnType >= MAX_NUM_DEV_FUNCTIONS) ||
       (bySlotID >= MAX_NUM_SLOTS) || (FALSE == bFoundFn) ||
       (FALSE == bFoundSl) )
  {
    return ERR_INVALID_ARGS;
  }

  sprintf(pszDevName, "%s:%s:%s_%d", stDevTypeMap[byFnType].szName, 
          stSlotIDMap[bySlotID].szSlotName, stDevTypeMap[byFnType].szName, 
          byFnEnum);

  return ERR_SUCCESS;
}

