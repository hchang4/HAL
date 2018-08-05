/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: HelperClasses.cpp
 * *
 * *  Description: Definition of classes required for ddiscfallback compilation.
 * *               Since ddiscfallback needed to be indepent of HAL library, all these classes are
 * *               collected from HAL and place it here. This file only used for ddiscfallback.
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
 * *  $Id: HelperClasses.cpp,v 1.10 2016/11/01 13:06:10 emrsn\jbeatty Exp $
 * *  $Log: HelperClasses.cpp,v $
 * *  Revision 1.10  2016/11/01 13:06:10  emrsn\jbeatty
 * *  Added support for FPD G2
 * *
 * *  Revision 1.9  2016/08/30 09:56:11  emrsn\jbeatty
 * *  Mods for 1500XA R2 support
 * *
 * *  Revision 1.8  2016/08/09 17:08:09  emrsn\jbeatty
 * *  Support for 1500XA R2 (ddiscfallback build still to be fixed)
 * *
 * *  Revision 1.7  2015/05/04 14:31:44  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.6  2014/06/26 09:59:43  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.5  2012/03/21 07:54:07  emrsn\rsaha
 * *  Revised for ddiscfallback application to correctly identify IMB slot and functionality.
 * *
 * *  Revision 1.4  2012/03/21 06:48:41  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2011/07/13 11:05:27  emrsn\rsaha
 * *  Revised to do device capability query for 370XA LOI over UDP.
 * *
 * *  Revision 1.2  2010/10/29 13:24:16  emrsn\rsaha
 * *  This file is added to support ddiscfallback compilation.
 * *  All the HAL classes that ddisc depends on are isolated in this file so to help build ddiscfallback without linking to HAL library.
 * *  This file is only get compiled during ddiscfallback build process.
 * *
 * *************************************************************************/


#ifdef MAKE_FALLBACK

#include <stdio.h>
#include <string.h>
#include "FixEndian.h"
#include "HelperClasses.h"

static const unsigned short crc16tab[256] = {
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
  0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
  0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
  0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
  0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
  0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
  0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
  0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
  0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
  0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
  0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
  0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
  0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
  0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
  0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
  0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
  0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
  0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
  0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};


/*
  This function calculates the new CRC value.           
*/
unsigned short updcrcr(unsigned short crc16, unsigned short ch)
{
  //Current CRC value is EXORed with CRC table values
  unsigned short tmp;
  tmp=crc16^ch;
  crc16=(crc16>>8)^crc16tab[tmp & 0xff];
  return crc16;
}

/*
  This function calls the updcrcr funtion for each Byte.          
*/
unsigned short crc16(unsigned char *buf, unsigned int len)
{
  unsigned int counter;
  unsigned short crc = 0;
  for( counter = 0; counter < len; counter++)
  {
    crc = updcrcr (crc, buf[counter]);
  }
  return crc;
}

CFragment::CFragment()  // Constructor
{
  m_pbyCollectedData = NULL;
  m_unArrayLen = 0;
  m_bProcFragment = FALSE;
  m_bCRCFailure = FALSE;
  m_bNewData = FALSE;
  m_bCRCExists = FALSE;
}

CFragment::~CFragment() // Destructor
{
  Flush();
}

// Clear the data in m_pbyCollectedData
int CFragment::Flush ()
{
  //Release allocated memory
  if (m_pbyCollectedData)
  {
    free(m_pbyCollectedData);
  }

  //Reset variables
  m_pbyCollectedData = NULL;
  m_unArrayLen = 0;
  m_bProcFragment = FALSE;
  m_bCRCFailure = FALSE;
  m_bNewData = FALSE;
  m_bCRCExists = FALSE;
  
  return ERR_NOT_IMPLEMENTED;
}

//Extract the received CRC and also compute CRC over the received data
void CFragment::CRCCompute(unsigned int unBufferStart, unsigned int unBufferLen)
{
  // The received CRC
  unsigned short usRxCRC = 0;
  // CRC computed over the received data
  unsigned short usCalcCRC = 0;

  ENDIAN_ADJ_UNION endianAdj;
  

  //Extract the RX'ed CRC from the last two bytes of the received data
  endianAdj.parts.msb = m_pbyCollectedData[m_unArrayLen - 1];
  endianAdj.parts.lsb = m_pbyCollectedData[m_unArrayLen - 2];
  usRxCRC = endianAdj.asUint16;
  FixEndian(usRxCRC);

  //Compute CRC over the data packet - excluding the header in the beginning
  //  and the received CRC at the end of the packet.
  usCalcCRC = crc16(&m_pbyCollectedData[unBufferStart], unBufferLen);
  
  //Check for a match and set status accordingly
  if (usRxCRC != usCalcCRC)
  {
    m_bCRCFailure = TRUE;
  }
  else
  {
    m_bCRCFailure = FALSE;
  }
}

CDataFragment::CDataFragment()  // Constructor
{
}

CDataFragment::~CDataFragment() // Destructor
{
}

// Add part of fragmented data to the end of m_pbyCollectedData
int CDataFragment::AddData (unsigned char* pbyData, unsigned char byNumBytes)
{
  DevAddrUnion stDevToHost;
  void *pReallocMem = NULL;

  if ( (NULL == pbyData) || (0 == byNumBytes) )
  {
    return -ERR_INVALID_ARGS;
  }

  //Extract the header
  memcpy(&stDevToHost.stDevAdD2H, pbyData, sizeof(DevAddrUnion));
  FixEndian(stDevToHost.usDevAd);

  //For the first packet, save the header too
  if (FALSE == m_bProcFragment)
  {
    //If the fragment bit is set, this is a multi-packet, fragmented message
    if (GetFragment(&stDevToHost.usDevAd))
    {
      m_bProcFragment = TRUE;
      m_bNewData = FALSE;
    }
    //This is a non-fragmented packet - set flag to indicate that the data is
    //  available for read
    else
    {
      m_bNewData = TRUE;
      //No CRC for non-fragmented packets
      m_bCRCExists = FALSE;
    }

    m_unArrayLen = 0;
  }

  pReallocMem = realloc(  m_pbyCollectedData, 
                          m_unArrayLen + byNumBytes - RX_PKT_HDR_LEN);
  if (NULL == pReallocMem)
  {
    //TODO - do we need to free the prev allocated memory here or
    //  let the user do it through Flush ()
  
    return ERR_MEMORY_ERR;
  }

  //Point to the newly allocated memory
  m_pbyCollectedData = (unsigned char *) pReallocMem;
  
  //Copy/append data to the data buffer
  memcpy(&m_pbyCollectedData[m_unArrayLen], &pbyData[PKT_DATA_START_IX], 
         byNumBytes - RX_PKT_HDR_LEN);
  //Length of the data buffer
  m_unArrayLen += (byNumBytes - RX_PKT_HDR_LEN);

  // If this is the end of the fragmented packet 
  if ( (!GetFragment(&stDevToHost.usDevAd)) && (FALSE == m_bNewData) )
  {
    //Process CRC related stuff
    CRCCompute(0, m_unArrayLen - RX_PKT_CRC_LEN);
    //No longer processing a fragment
    m_bProcFragment = FALSE;
    //New data exists
    m_bNewData = TRUE;
    //CRC exists for fragmented packets
    m_bCRCExists = TRUE;
  }

  return ERR_SUCCESS;
}

// Get the accumulated data from multiple fragmented packets 
int CDataFragment::GetData (unsigned char* pbyData, unsigned int unNumBytesToRead)
{
  unsigned int unDataLen = 0;

  if ( (NULL == pbyData) || (0 == unNumBytesToRead) )
  {
    return ERR_INVALID_ARGS;
  }


  if (m_bNewData)
  {
    //TODO - do we reset m_bNewData here?
    m_bNewData = FALSE;

    if (m_bCRCFailure)
    {
      return ERR_WRONG_CRC;
    }
    else
    {
      if (m_bCRCExists)
      {
        unDataLen = m_unArrayLen - RX_PKT_CRC_LEN;
      }
      else
      {
        unDataLen = m_unArrayLen;
      }

      //TODO - if the user requests for less data than available,
      //  maintain an index of how much has been read so that the
      //  next call to this function will pass the unread data.
      //  DO THIS LATER

      //If asking for more than what's present, return whatever is
      //  available.
      if (unNumBytesToRead >= unDataLen)
      {
        //Copy the max. available data
        memcpy(pbyData, m_pbyCollectedData, unDataLen);
      }
      else
      {
        //Copy data to the user buffer (till only the user supplied length)
        memcpy(pbyData, m_pbyCollectedData, unNumBytesToRead);
      }

      return unDataLen;
    }
      
  }
  else if (m_bProcFragment)
  {
    return ERR_DATA_PENDING;
  }
  else
  {
    return ERR_INVALID_SEQ;
  }
}

// Get the total number of bytes remaining in m_pbyCollectedData 
int CDataFragment::GetCount ()
{
  if (m_bCRCExists)
  {
    return (m_unArrayLen - RX_PKT_CRC_LEN);
  }
  else
  {
    return m_unArrayLen;
  }
}


#define USING_STRTOK_R // TODO: Use strtok_r instead of strtok. The latter is not thread safe! TODO: strtok_r behaves goofy (
// modifies the input string, when it is not supposed to). Need to check if strtok_r is ok to use!

const DeviceTypeMapStruct CResolveDevName::stDevTypeMap[MAX_NUM_DEV_FUNCTIONS] =
{
  {"ANA_IN", FN_ANA_IN},      // Analog In
  {"ANA_OUT", FN_ANA_OUT},    // Analog Out 
  {"DIGI_IN", FN_DIGI_IN},    // Digital Input
  {"DIGI_OUT", FN_DIGI_OUT},    // Digital Output
  {"PREAMP_STR", FN_PREAMP_STR},    // Bridge Preamp streaming function (on dual channel bridge preamp board)
  {"PREAMP_CFG", FN_PREAMP_CFG},    // Bridge Preamp configuration (on dual channel bridge preamp board)
  {"RTD", FN_RTD},      // RTD (on Heater/Solenoid ctrl board)
  {"HTR_CTRL", FN_HTR_CTRL},    // Heater Channel (On Heater/Solenoid board)
  {"SOL", FN_SOL},      // Solenoid (on Heater/Solenoid board)
  {"SERIAL", FN_SERIAL},      // Serial COM
  {"EPC", FN_EPC},      // EPC Controller (On Base IO board)
  {"LTLOI", FN_LTLOI},      // LTLOI (On Base IO board)
  {"FFB_STATUS", FN_FFB_STATUS},    // Funtion to send GC status information to FFB board.
  {"FFB_COMMAND", FN_FFB_COMMAND},  // Funtion to get remote commands from FFB board to configure GC.
  {"GRAPHICAL_LOI", FN_GRAPHICAL_LOI},  // Local Operator Interface (Graphical)
  {"DIAGNOSTIC", FN_DIAGNOSTIC},    // Function to retrive Diagnostic information from all boards.
  {"FID", FN_FID},      //Function to cotrol FID Ignition and Gain.
  {"FPD", FN_FPD},      //Function to cotrol FPD AutoZero.
  {"PRESSURE", FN_PRESSURE},    // Function to retrieve Sample pressure.
  {"CTRL", FN_CTRL},      // Function for Control pins - e.g. - RS232/RS485 toggle for 370XA.
  {"IMB_COMM", FN_IMB_COMM},            // Function for IMB board communication.
  {"CC", FN_CYCLE_CLOCK_SYNC},          // Function for Synchronizing Cycle Clock across multiple boards
  {"FPD_G2", FN_FPD_G2}                 // Function to control G2 FPD
};

//The 'bySlotID' field is hard-coded to map into a specific Slot Name
const DeviceAddrMapStruct CResolveDevName::stSlotIDMap[MAX_NUM_SLOTS] =
{
  {"EXP_SLOT_1", FALSE, 1},   // Expansion Slot 1 on SPI Bus
  {"EXP_SLOT_2", FALSE, 2},   // Expansion Slot 2 on SPI Bus
  {"EXP_SLOT_3", FALSE, 3},   // Expansion Slot 3 on SPI Bus
  {"EXP_SLOT_4", FALSE, 4},   // Expansion Slot 4 on SPI Bus
  {"SLOT_1", TRUE, GC700XP_PREAMP_BD_CAN_BASE_MSG_ID},    // Slot 1 on CAN Bus
  {"SLOT_2", TRUE, GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID},  // Slot 2 on CAN Bus
  {"SLOT_3", TRUE, GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 1},  // Slot 3 on CAN Bus
  {"SLOT_4", TRUE, GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID + 1},  // Slot 4 on CAN Bus
  {"SLOT_5", TRUE, GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 2},  // Slot 5 on CAN Bus
  {"SLOT_BASE_IO", TRUE, GC700XP_BASE_IO_BD_BASE_CAN_MSG_ID}, // Base IO Slot on CAN Bus
  {"FIELDBUS_SLOT", TRUE, GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID}, // CAN Slot on FF Board.
  {"LOI_SLOT", TRUE, GC700XP_GRAPHICAL_LOI_BD_BASE_CAN_MSG_ID}, // CAN Slot on LTLOI / Graphical LOI connector
  {"ANALYZER_SLOT", TRUE, GC370XA_ANALYZER_BD_BASE_CAN_MSG_ID}, // CAN Slot on 370XA Analyzer board.OD
  {"IMB_SLOT", TRUE, GC370XA_IMB_BD_BASE_CAN_MSG_ID}, // CAN Slot on 370XA IMB.
  {"REMOTE_EXP_SLOT_3", TRUE, 11},  // Remote Expansion Slot 3 on CAN Bus
  {"REMOTE_EXP_SLOT_4", TRUE, 12},  // Remote Expansion Slot 4 on CAN Bus
  {"370XA_LOI_SLOT", FALSE, 3},     // 370XA LOI Slot
  {"BROADCAST", TRUE, CYCLE_CLOCK_SYNC_CAN_MSG_ID},      // 370XA LOI Slot
  {"BACKPLANE", FALSE, 26},
  {"FPD_G2_1", TRUE, GC700XP_FPD_G2_BASE_CAN_MSG_ID},
  {"FPD_G2_2", TRUE, GC700XP_FPD_G2_BASE_CAN_MSG_ID + 1},
};

int CResolveDevName::ResolveName (char* pszDevName, unsigned char* pbyFnType, unsigned char* pbySlotID, unsigned char* pbyFnEnum, BOOL* pbCANorSPI)
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
      pbCANorSPI == NULL)
  {
    nRetVal = ERR_INVALID_ARGS;
  }


  if (nRetVal == ERR_SUCCESS)
  {
    // Initialize 
    *pbyFnType = *pbyFnEnum = *pbySlotID = (unsigned char) -1;

    // Copy Dev Name to a temp variable. strtok_r always overwrites input string
    strncpy (szDevName, pszDevName, MAX_DEV_NAME);

    // Establish string and get the first token (Function Type)
    pszFnType = strtok(szDevName, szSeparator);

    // Get the second token (Slot ID)
    pszSlotID = strtok(NULL, szSeparator);

    // Get the third token (Function Enum)
    pszFnEnum = strtok(NULL, szSeparator);

    // Check if the device name is in FnType:SlotID:FnEnum format
    if (pszFnType != NULL && pszSlotID != NULL && pszFnEnum != NULL)
    {
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
          *pbCANorSPI = stSlotIDMap[nCount].bIsOnCANBus;
          break;
        }
      }

      // Get the function enumeration
      if ((nPos = (int) strcspn (pszFnEnum, "0123456789")) < (int)strlen (pszFnEnum))
      {
        *pbyFnEnum = (unsigned char) atoi (pszFnEnum + nPos);
      }

      // Check if we got what we wanted
      if (*pbyFnType == (unsigned char) -1 ||
          *pbySlotID == (unsigned char) -1 ||
          *pbyFnEnum == (unsigned char) -1)
      {
        nRetVal = ERR_INVALID_DEV_NAME;
      }
    }
    else
    {
      nRetVal = ERR_INVALID_DEV_NAME;
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
      pbCANorSPI == NULL)
  {
    nRetVal = ERR_INVALID_ARGS;
  }

  if (nRetVal == ERR_SUCCESS)
  {
    // Initialize 
    *pbyFnType = *pbyFnEnum = *pbySlotID = (unsigned char) -1;

    // Copy Dev Name to a temp variable. strtok_r always overwrites input string
    strncpy (szDevName, pszDevName, MAX_DEV_NAME-1);

    // Establish string and get the first token (Function Type)
    pszFnType = strtok_r(szDevName, szSeparator, (char**)&szTemp);

    // Get the second token (Slot ID)
    pszSlotID = strtok_r(NULL, szSeparator, (char**)&szTemp);

    // Get the third token (Function Enum)
    pszFnEnum = strtok_r(NULL, szSeparator, (char**)&szTemp);

    // Check if the device name is in FnType:SlotID:FnEnum format
    if (pszFnType != NULL && pszSlotID != NULL && pszFnEnum != NULL)
    {
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
          *pbCANorSPI = stSlotIDMap[nCount].bIsOnCANBus;
          break;
        }
      }

      // Get the function enumeration
      if ((nPos = (int) strcspn (pszFnEnum, "0123456789")) < (int)strlen (pszFnEnum))
      {
        *pbyFnEnum = (unsigned char) atoi (pszFnEnum + nPos);
      }

      // Check if we got what we wanted
      if (*pbyFnType == (unsigned char) -1 ||
          *pbySlotID == (unsigned char) -1 ||
          *pbyFnEnum == (unsigned char) -1)
      {
        nRetVal = ERR_INVALID_DEV_NAME;
      }
    }
    else
    {
      nRetVal = ERR_INVALID_DEV_NAME;
    }
  }
  
  return nRetVal;
}
#endif


int CResolveDevName::GenerateName (char* pszDevName, 
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

#endif //#ifdef MAKE_FALLBACK
