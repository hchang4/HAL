/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: IMBComm.cpp 
 * *
 * *  Description: IMB Communication access layer.
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
 * *  $Id: IMBComm.cpp,v 1.21 2016/06/13 16:18:02 emrsn\wjyu Exp $
 * *  $Log: IMBComm.cpp,v $
 * *  Revision 1.21  2016/06/13 16:18:02  emrsn\wjyu
 * *  Revised for TestHAL application
 * *
 * *  Revision 1.20  2016/05/27 20:23:48  emrsn\wjyu
 * *  revised to support C7+ application
 * *
 * *  Revision 1.19  2015/05/04 14:49:27  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.18  2014/10/22 18:07:53  emrsn\wjyu
 * *  Revise the format of the imb serial number string in the format of (left to right) word 3, word 2, word 1, word 0
 * *  (It was in the format of word 0, word 1, word 2, word 3)
 * *
 * *  Revision 1.17  2014/10/03 21:02:04  emrsn\wjyu
 * *  Revised GetFlashImage() and SetFlashImage() API to support FACTORY and MIRROR section.
 * *  Before, those two functions only support the GLOBAL section
 * *  (For 6587 GC should not allow reading Module data if IMB is Incompatible )
 * *
 * *  Revision 1.16  2014/06/26 10:00:29  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.15  2014/05/18 23:46:05  emrsn\wjyu
 * *  IMBComm revision - Added GetAPIID() method
 * *
 * *  Revision 1.14  2014/05/14 10:03:01  emrsn\wjyu
 * *  For IMB Firmware 1.0.1
 * *  Revised IMBComm, added GetIMBInfo() function
 * *
 * *  Revision 1.13  2014/05/09 13:43:47  emrsn\wjyu
 * *  For BZ 7326 The IMB DEFAULT section will be removed
 * *  Updated IMBComm
 * *
 * *  Revision 1.12  2014/05/02 19:02:46  emrsn\wjyu
 * *  For BZ 7292 - Need to store the 12 digit Factory Assigned Module Serial Number in IMB
 * *  Revised GC IMBComm
 * *
 * *  Revision 1.11  2013/05/20 21:01:11  emrsn\wjyu
 * *  Added function to update IMB Flash
 * *
 * *  Revision 1.10  2013/05/01 18:31:42  emrsn\wjyu
 * *  For IMB firmware version 0.0.15
 * *  Removed Timed Events Adjustments structure from the Mirror Data Section
 * *
 * *  Revision 1.9  2013/03/07 14:42:03  emrsn\wjyu
 * *  Add function to read IMB Flash Image
 * *
 * *  Revision 1.8  2013/02/08 17:21:12  emrsn\rsaha
 * *  Revised IMB class interface to retrieve IMB Database ID. A new method GetDBID() is added to retrieve the same. Database identifier is only one, section wise database identifiers has been removed.
 * *
 * *  Revision 1.7  2013/01/30 06:18:41  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.6  2013/01/03 12:00:51  emrsn\rsaha
 * *  IMB firmware is revised with the following -
 * *
 * *  - Database ID is moved from Global Data section. Now Factory, Mirror and Default data section all have their individual Database Identifier field.- Time Event Adjustment Data also made part of Mirror Data Section.
 * *  - Revised Project settings to use 512 bytes less for stack region, since due to TEV Adj Data structure inclusion in Mirror Data Section there is increase of about 385 bytes of RAM usage.
 * *
 * *  Revision 1.5  2012/11/30 09:02:11  emrsn\rsaha
 * *   - DB ID is now in Global data section, previously it was present in all the three - Default, Factory and Mirror secotions.
 * *   - Software TEV table row size is increased from 16 to 30 and Gain TEV table row size is increased from 2 to 3.
 * *   - Revised AVT settings and AVT Valve Sel - Removed Peak Area Change limit from AVT settings and added it in AVT Valve.
 * *   - Revised AVT Time Event Adjustment table -
 * *           1. Removed Default RT.
 * *           2. Added Ideal RF Ration.
 * *           3. Added Ideal RF Reference Component.
 * *           4. Added Ideal RF Limit.
 * *   - Added AVT Settings, Valve Selection and Time Event Adjustments to Factory section also, previously they were in Default section only.
 * *   - Carrier and Sample Zero and Span calibration factors are placed in Mirror sections also, previously they were in Factory section only.
 * *   - Added Analysis Time configurations in all three - Default, Factory and Mirror sections.
 * *
 * *   - Revised Communication interface with CPU HAL layer to Read/Write the newly revised fields.
 * *
 * *  Revision 1.4  2012/03/21 06:48:54  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2012/02/07 11:38:05  emrsn\rsaha
 * *  Revised the following -
 * *   Added APIs to Set/Get Carrier/Sample Pressure Zero/Span calibration factors to Factory section.
 * *
 * *  Revision 1.2  2012/01/31 15:08:35  emrsn\rsaha
 * *  Revised HAL - IMB communication protocol with smaller packet size.
 * *
 * *  Revision 1.1  2012/01/03 13:08:26  emrsn\rsaha
 * *  Added APIs to support to communicate with IMB.
 * *
 * *************************************************************************/

#include "string.h"
#include "stdio.h"
#include "unistd.h"
#include <stddef.h> // for the offsetof macro

#include "FixEndian.h"
  
#include "debug.h"
#include "IMBProtocol.h"
#include "IMBComm.h"


#define IMB_REQUEST_TIME_OUT  900
CIMBComm::CIMBComm()  // Default Constructor
{
  // IMB firmware API version
  //m_nAPIVer = 1;
}

CIMBComm::~CIMBComm() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CIMBComm::OpenHal (char* pszDevName)
{
  int nRetVal = CBaseDev::OpenHal(pszDevName, FALSE);
  return nRetVal;
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CIMBComm::CloseHal ()
{
  return CBaseDev::CloseHal();
}

// This has to be set before using API-version-dependent functions !!!!
// set the IMB device API version number
//void CIMBComm::SetAPIVer(const int nAPIVer)
//{
//  m_nAPIVer = nAPIVer;
//}

int CIMBComm::GetGlobalDataIMBInit2( unsigned char * pbyIMBInit )
{
  DB_INT32 i32RetVal = -1;
  unsigned char temp = 0;
  unsigned int refLenRead = 0;
  i32RetVal = GetFlashImage2( (unsigned char*) & temp,
                              IMB_GLOBAL_FLASH_SECTION,
                              offsetof(IMB_GLOBAL_DATA_STRUCT2, byIMBInit),
                              sizeof(unsigned char) ,
                              refLenRead);
  if(pbyIMBInit != NULL)
    *pbyIMBInit = temp;

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_IMB_GLOBAL_DATA_STRUCT stResp = {0};

  if (NULL == pGlobalData)
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_IMB_FN_GET_GLOBAL_DATA_SECTION);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),      // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pGlobalData = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetGlobalData(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetGlobalData(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetGlobalData(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetGlobalData(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetGlobalData(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetMirrorNumAnalysis2( int * pMirrorNumAnalysis )
{
  DB_INT32 i32RetVal = -1;
  IMB_FLASH_SECTION_ENUM eSectionType = IMB_MIRROR_FLASH_SECTION;
  int temp = 0;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          INVALID_OFFSET2 ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2, nNumAnalysis),
                                          sizeof(int), (unsigned char*) & temp);
  if(pMirrorNumAnalysis != NULL)
    *pMirrorNumAnalysis = temp;

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_IMB_MIRROR_NUM_ANALYSIS_STRUCT stResp = {0};

  if (NULL == pMirrorNumAnalysis)
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_IMB_FN_GET_MIRROR_NUM_ANALYSIS);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),      // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pMirrorNumAnalysis = stResp.nNumAnalysis;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetMirrorNumAnalysis(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetMirrorNumAnalysis(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetMirrorNumAnalysis(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetMirrorNumAnalysis(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetMirrorNumAnalysis(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetMirrorNumAnalysis2(const int nMirrorNumAnalysis )
{
  DB_INT32 i32RetVal = -1;
  IMB_FLASH_SECTION_ENUM eSectionType = IMB_MIRROR_FLASH_SECTION;
  int temp = nMirrorNumAnalysis;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          INVALID_OFFSET2 ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2, nNumAnalysis),
                                          sizeof(int),  (unsigned char *) & temp);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_MIRROR_NUM_ANALYSIS_STRUCT stMirrorNumAnalysisCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  stMirrorNumAnalysisCmd.nNumAnalysis = nMirrorNumAnalysis;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stMirrorNumAnalysisCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_MIRROR_NUM_ANALYSIS);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stMirrorNumAnalysisCmd, // Command
                                                     sizeof (stMirrorNumAnalysisCmd),        // Size of command
                                                     (unsigned char *) &stResp,          // Response from remote board
                                                     sizeof (stResp),            // Size of expected response
                                                     TRUE, 
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetMirrorNumAnalysis() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetMirrorNumAnalysis(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetMirrorNumAnalysis(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetMirrorNumAnalysis(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetMirrorNumAnalysis(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetMirrorNumAnalysis(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionMetaInfo2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_SECTION_META_INFO2 & stMetaInfo )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stMetaInfo) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stMetaInfo),
                                          sizeof(IMB_SECTION_META_INFO2),  (unsigned char *) & stMetaInfo);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_METAINFO_STRUCT stSectionMetaInfoCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if (eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stSectionMetaInfoCmd.bySectionType = (unsigned char) eSectionType;
  stSectionMetaInfoCmd.stData = stMetaInfo;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionMetaInfoCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_META_INFO);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionMetaInfoCmd, // Command
                                                     sizeof (stSectionMetaInfoCmd),      // Size of command
                                                     (unsigned char *) &stResp,      // Response from remote board
                                                     sizeof (stResp),        // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionMetaInfo() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionMetaInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionMetaInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionMetaInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionMetaInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionMetaInfo(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionMetaInfo2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_SECTION_META_INFO2 * pstMetaInfo )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stMetaInfo) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2, stMetaInfo),
                                          sizeof(IMB_SECTION_META_INFO2), (unsigned char*) pstMetaInfo);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_METAINFO_STRUCT stResp = {0};

  if (NULL == pstMetaInfo || eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_META_INFO);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),   // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstMetaInfo = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionMetaInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionMetaInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionMetaInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionMetaInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionMetaInfo(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionAVTSettingsTable2(const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_SETTINGS_TABLE_STRUCT & stAVTSettingsTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTSettings) ,
                                          INVALID_OFFSET2,
                                          sizeof(IMB_AVT_SETTINGS_TABLE_STRUCT),  (unsigned char *) & stAVTSettingsTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_AVT_STG_STRUCT stSectionAVTSettingsCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if ( (eSectionType >= IMB_INVALID_FLASH_SECTION) || (IMB_MIRROR_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stSectionAVTSettingsCmd.bySectionType = (unsigned char) eSectionType;
  stSectionAVTSettingsCmd.stData = stAVTSettingsTable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionAVTSettingsCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_AVT_SETTINGS_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionAVTSettingsCmd, // Command
                                                     sizeof (stSectionAVTSettingsCmd),     // Size of command
                                                     (unsigned char *) &stResp,    // Response from remote board
                                                     sizeof (stResp),      // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionAVTSettingsTable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionAVTSettingsTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionAVTSettingsTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionAVTSettingsTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionAVTSettingsTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionAVTSettingsTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionAVTSettingsTable2(const IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_SETTINGS_TABLE_STRUCT * pstAVTSettingsTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTSettings) ,
                                          INVALID_OFFSET2,
                                          sizeof(IMB_AVT_SETTINGS_TABLE_STRUCT), (unsigned char*) pstAVTSettingsTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_AVT_STG_STRUCT stResp = {0};

  if ( (NULL == pstAVTSettingsTable) || (eSectionType >= IMB_INVALID_FLASH_SECTION) || (IMB_MIRROR_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }
  
  stIMBCmd.bySectionType = (unsigned char) eSectionType;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_AVT_SETTINGS_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),     // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstAVTSettingsTable = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionAVTSettingsTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionAVTSettingsTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionAVTSettingsTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionAVTSettingsTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionAVTSettingsTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionAVTValveSelTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_VALVE_SEL_TABLE_STRUCT & stAVTValveSelTable )
{
  DB_INT32 i32RetVal = -1;
  // there is no AVT-Valve-Selection in MIRROR
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                           offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTValveSel) ,
                                           INVALID_OFFSET2,
                                           sizeof(IMB_AVT_VALVE_SEL_TABLE_STRUCT),  (unsigned char *) & stAVTValveSelTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_AVTVSEL_TABLE_STRUCT stSectionAVTValveSelCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if ( (eSectionType >= IMB_INVALID_FLASH_SECTION) || (IMB_MIRROR_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stSectionAVTValveSelCmd.bySectionType = (unsigned char) eSectionType;
  stSectionAVTValveSelCmd.stData = stAVTValveSelTable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionAVTValveSelCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_AVT_VSEL_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionAVTValveSelCmd, // Command
                                                     sizeof (stSectionAVTValveSelCmd),       // Size of command
                                                     (unsigned char *) &stResp,      // Response from remote board
                                                     sizeof (stResp),        // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionAVTValveSelTable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionAVTValveSelTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionAVTValveSelTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionAVTValveSelTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionAVTValveSelTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionAVTValveSelTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionAVTValveSelTable2( IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_VALVE_SEL_TABLE_STRUCT * pstAVTValveSelTable )
{
  DB_INT32 i32RetVal = -1;
  // there is no AVT Valve Sel in MIRROR
  i32RetVal= getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTValveSel) ,
                                          INVALID_OFFSET2,
                                          sizeof(IMB_AVT_VALVE_SEL_TABLE_STRUCT), (unsigned char*)  pstAVTValveSelTable);

    return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_AVTVSEL_TABLE_STRUCT stResp = {0};

  if ( (NULL == pstAVTValveSelTable) || (eSectionType >= IMB_INVALID_FLASH_SECTION) || (IMB_MIRROR_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_AVT_VSEL_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),     // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstAVTValveSelTable = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionAVTValveSelTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionAVTValveSelTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionAVTValveSelTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionAVTValveSelTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionAVTValveSelTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionAVTTimeEventAdjTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 & stAVTTEATable )
{
  DB_INT32 i32RetVal = -1;
  // there is no AVT Time Event Adj Table in MIRROR
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                           offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTTimeEventAdj) ,
                                           INVALID_OFFSET2,
                                           sizeof(IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2),  (unsigned char *) & stAVTTEATable);

  return i32RetVal;
  
#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_AVTTEADJ_TABLE_STRUCT stSectionAVTTimeEventAdjCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};


  if ( (eSectionType >= IMB_INVALID_FLASH_SECTION) || (IMB_MIRROR_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stSectionAVTTimeEventAdjCmd.bySectionType = (unsigned char) eSectionType;
  stSectionAVTTimeEventAdjCmd.stData = stAVTTEATable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionAVTTimeEventAdjCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_AVT_TEADJ_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionAVTTimeEventAdjCmd, // Command
                                                     sizeof (stSectionAVTTimeEventAdjCmd),     // Size of command
                                                     (unsigned char *) &stResp,      // Response from remote board
                                                     sizeof (stResp),        // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionAVTTimeEventAdjTable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionAVTTimeEventAdjTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionAVTTimeEventAdjTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionAVTTimeEventAdjTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionAVTTimeEventAdjTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionAVTTimeEventAdjTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionAVTTimeEventAdjTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 * pstAVTTEATable )
{
  DB_INT32 i32RetVal = -1;
  // there is no AVT Time Event Adj Table in MIRROR
  i32RetVal= getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTTimeEventAdj) ,
                                          INVALID_OFFSET2,
                                          sizeof(IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2), (unsigned char*) pstAVTTEATable);

  return i32RetVal;
  
#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_AVTTEADJ_TABLE_STRUCT stResp = {0};

  if ( (NULL == pstAVTTEATable) || (eSectionType >= IMB_INVALID_FLASH_SECTION) || (IMB_MIRROR_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_AVT_TEADJ_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),     // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstAVTTEATable = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionAVTTimeEventAdjTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionAVTTimeEventAdjTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionAVTTimeEventAdjTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionAVTTimeEventAdjTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionAVTTimeEventAdjTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionCDTTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_CDT_TABLE_STRUCT & stCDTTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stCDT) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stCDT) ,
                                          sizeof(IMB_CDT_TABLE_STRUCT),  (unsigned char *) & stCDTTable);
  
  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_CDT_CONFIG_STRUCT stSectionCDTConfigCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if (eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stSectionCDTConfigCmd.bySectionType = (unsigned char) eSectionType;
  stSectionCDTConfigCmd.stData = stCDTConfig;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionCDTConfigCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_CDT_CONFIG);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionCDTConfigCmd, // Command
                                                     sizeof (stSectionCDTConfigCmd),       // Size of command
                                                     (unsigned char *) &stResp,      // Response from remote board
                                                     sizeof (stResp),        // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionCDTConfig() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionCDTConfig(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionCDTConfig(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionCDTConfig(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionCDTConfig(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionCDTConfig(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionCDTTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_TABLE_STRUCT * pstCDTTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal= getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stCDT) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stCDT) ,
                                          sizeof(IMB_CDT_TABLE_STRUCT), (unsigned char*) pstCDTTable);
  
  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_CDT_CONFIG_STRUCT stResp = {0};

  if (NULL == pstCDTConfig || eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_CDT_CONFIG);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),   // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstCDTConfig = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionCDTConfig(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionCDTConfig(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionCDTConfig(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionCDTConfig(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionCDTConfig(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

/*
int CIMBComm::SetSectionCDTUpdate( IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_UPDATE_STRUCT stCDTUpdate )
{

  if(m_nAPIVer == 2)
  {
    //this does NOT work !!!
    unsigned int nOffset0 = 
      (eSectionType == (IMB_FACTORY_FLASH_SECTION)) ? offsetof(IMB_FACTORY_DATA_STRUCT, stCDT) : offsetof(IMB_MIRROR_DATA_STRUCT, stCDT)
       + offsetof(IMB_CDT_TABLE_STRUCT, stCDTUpdate);

    unsigned int nOffset1 = 
      (eSectionType == (IMB_FACTORY_FLASH_SECTION)) ? offsetof(IMB_FACTORY_DATA_STRUCT, stCDT) : offsetof(IMB_MIRROR_DATA_STRUCT, stCDT);

    unsigned int nOffset2 =
      offsetof(IMB_CDT_TABLE_STRUCT, stCDTUpdate);

    unsigned int nOffset = nOffset1 + nOffset2;

    printf("SetSectionCDTUpdate section %d, offset= %d, 1= %d, 2= %d \n", eSectionType, nOffset, nOffset1, nOffset2 );
    printf("SetSectionCDTUpdate section %d, offset0=%d               \n", eSectionType, nOffset0 );

    unsigned int unDataLen = sizeof(IMB_CDT_UPDATE_STRUCT);

    return SetFlashImage( (unsigned char *) (& stCDTUpdate), eSectionType, nOffset, unDataLen );
  }
  

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_CDT_UPDATE_STRUCT stSectionCDTUpdateCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if (eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stSectionCDTUpdateCmd.bySectionType = (unsigned char) eSectionType;
  stSectionCDTUpdateCmd.stData = stCDTUpdate;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionCDTUpdateCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_CDT_UPDATE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionCDTUpdateCmd, // Command
                                                     sizeof (stSectionCDTUpdateCmd),     // Size of command
                                                     (unsigned char *) &stResp,    // Response from remote board
                                                     sizeof (stResp),      // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionCDTUpdate() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionCDTUpdate(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionCDTUpdate(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionCDTUpdate(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionCDTUpdate(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionCDTUpdate(): Function called before Open Call!");
  }

  return nRetVal;
}
*/

/*
int CIMBComm::GetSectionCDTUpdate( IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_UPDATE_STRUCT * pstCDTUpdate )
{

  if(m_nAPIVer == 2)
  {
    //this does NOT work !!!
    unsigned int nOffset0 = 
      (eSectionType == IMB_FACTORY_FLASH_SECTION) ? offsetof(IMB_FACTORY_DATA_STRUCT, stCDT) : offsetof(IMB_MIRROR_DATA_STRUCT, stCDT)
      + offsetof(IMB_CDT_TABLE_STRUCT, stCDTUpdate);

    unsigned int nOffset1 = 
      (eSectionType == IMB_FACTORY_FLASH_SECTION) ? offsetof(IMB_FACTORY_DATA_STRUCT, stCDT) : offsetof(IMB_MIRROR_DATA_STRUCT, stCDT);

    unsigned int nOffset2 = offsetof(IMB_CDT_TABLE_STRUCT, stCDTUpdate);

    unsigned int nOffset = nOffset1 + nOffset2;

    printf("GetSectionCDTUpdate section %d, offset= %d, 1= %d, 2= %d \n", eSectionType, nOffset, nOffset1, nOffset2 );
    printf("GetSectionCDTUpdate section %d, offset0=%d               \n", eSectionType, nOffset0 );

    unsigned int unDataLen = sizeof(IMB_CDT_UPDATE_STRUCT);
    unsigned int refLenRead = 0;

    return GetFlashImage( (unsigned char*) pstCDTUpdate, eSectionType, nOffset, unDataLen , refLenRead);
  }
  

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_CDT_UPDATE_STRUCT stResp = {0};

  if (NULL == pstCDTUpdate || eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_CDT_UPDATE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),   // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstCDTUpdate = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionCDTUpdate(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionCDTUpdate(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionCDTUpdate(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionCDTUpdate(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionCDTUpdate(): Function called before Open Call!");
  }

  return nRetVal;
}
*/

int CIMBComm::SetSectionHwTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_HW_TEV_TABLE_STRUCT & stHwTEVTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stHwTEV) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stHwTEV),
                                          sizeof(IMB_HW_TEV_TABLE_STRUCT),  (unsigned char *) & stHwTEVTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_HTEV_TABLE_STRUCT stSectionHwTEVCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if (eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stSectionHwTEVCmd.bySectionType = (unsigned char) eSectionType;
  stSectionHwTEVCmd.stData = stHwTEVTable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionHwTEVCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_HTEV_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionHwTEVCmd, // Command
                                                     sizeof (stSectionHwTEVCmd),     // Size of command
                                                     (unsigned char *) &stResp,    // Response from remote board
                                                     sizeof (stResp),      // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionHwTEVTable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionHwTEVTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionHwTEVTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionHwTEVTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionHwTEVTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionHwTEVTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionHwTEVTable2(const IMB_FLASH_SECTION_ENUM eSectionType, IMB_HW_TEV_TABLE_STRUCT * pstHwTEVTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stHwTEV) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stHwTEV),
                                          sizeof(IMB_HW_TEV_TABLE_STRUCT), (unsigned char*) pstHwTEVTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_HTEV_TABLE_STRUCT stResp = {0};

  if (NULL == pstHwTEVTable || eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_HTEV_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),   // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstHwTEVTable = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionHwTEVTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionHwTEVTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionHwTEVTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionHwTEVTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionHwTEVTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionSwTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_SW_TEV_TABLE_STRUCT & stSwTEVTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stSwTEV) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stSwTEV),
                                          sizeof(IMB_SW_TEV_TABLE_STRUCT),  (unsigned char *) & stSwTEVTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_STEV_TABLE_STRUCT stSectionSwTEVCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if (eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stSectionSwTEVCmd.bySectionType = (unsigned char) eSectionType;
  stSectionSwTEVCmd.stData = stSwTEVTable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionSwTEVCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_STEV_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionSwTEVCmd, // Command
                                                     sizeof (stSectionSwTEVCmd),     // Size of command
                                                     (unsigned char *) &stResp,    // Response from remote board
                                                     sizeof (stResp),      // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionSwTEVTable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionSwTEVTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionSwTEVTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionSwTEVTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionSwTEVTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionSwTEVTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionSwTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_SW_TEV_TABLE_STRUCT * pstSwTEVTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                    offsetof(IMB_FACTORY_DATA_STRUCT2, stSwTEV) ,
                                    offsetof(IMB_MIRROR_DATA_STRUCT2,  stSwTEV),
                                    sizeof(IMB_SW_TEV_TABLE_STRUCT), (unsigned char*) pstSwTEVTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_STEV_TABLE_STRUCT stResp = {0};

  if (NULL == pstSwTEVTable || eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_STEV_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),   // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstSwTEVTable = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionSwTEVTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionSwTEVTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionSwTEVTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionSwTEVTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionSwTEVTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionAnalysisConfigTable2(const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_ANALYSIS_TIME_CONFIG_STRUCT & stAnlyTimeCfgTable )
{
  DB_INT32 i32RetVal = -1;

  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stAnlyTimeCfg) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stAnlyTimeCfg),
                                          sizeof(IMB_ANALYSIS_TIME_CONFIG_STRUCT),  (unsigned char *) & stAnlyTimeCfgTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_ANALYSIS_TIME_CONFIG_TABLE_STRUCT stSectionAnlyTimeCfgCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if (eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stSectionAnlyTimeCfgCmd.bySectionType = (unsigned char) eSectionType;
  stSectionAnlyTimeCfgCmd.stData = stAnlyTimeCfgTable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionAnlyTimeCfgCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_ANALYSIS_CONFIG);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionAnlyTimeCfgCmd, // Command
                                                     sizeof (stSectionAnlyTimeCfgCmd),       // Size of command
                                                     (unsigned char *) &stResp,      // Response from remote board
                                                     sizeof (stResp),        // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionAnalysisConfigTable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionAnalysisConfigTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionAnalysisConfigTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionAnalysisConfigTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionAnalysisConfigTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionAnalysisConfigTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionAnalysisConfigTable2(const IMB_FLASH_SECTION_ENUM eSectionType, IMB_ANALYSIS_TIME_CONFIG_STRUCT * pstAnlyTimeCfgTable )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stAnlyTimeCfg) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stAnlyTimeCfg),
                                          sizeof(IMB_ANALYSIS_TIME_CONFIG_STRUCT), (unsigned char*) pstAnlyTimeCfgTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_ANALYSIS_TIME_CONFIG_TABLE_STRUCT stResp = {0};

  if (NULL == pstAnlyTimeCfgTable || eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_ANALYSIS_CONFIG);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),   // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstAnlyTimeCfgTable = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionAnalysisConfigTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionAnalysisConfigTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionAnalysisConfigTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionAnalysisConfigTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionAnalysisConfigTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionGainTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_GAIN_TEV_TABLE_STRUCT & stGainTEVTable )
{
  DB_INT32 i32RetVal = -1;

  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stGainTEV) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stGainTEV),
                                          sizeof(IMB_GAIN_TEV_TABLE_STRUCT),  (unsigned char *) & stGainTEVTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_GTEV_TABLE_STRUCT stSectionGainTEVCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  if (eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stSectionGainTEVCmd.bySectionType = (unsigned char) eSectionType;
  stSectionGainTEVCmd.stData = stGainTEVTable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionGainTEVCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_GTEV_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionGainTEVCmd, // Command
                                                     sizeof (stSectionGainTEVCmd),       // Size of command
                                                     (unsigned char *) &stResp,      // Response from remote board
                                                     sizeof (stResp),        // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionGainTEVTable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionGainTEVTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionGainTEVTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionGainTEVTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionGainTEVTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionGainTEVTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionGainTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_GAIN_TEV_TABLE_STRUCT * pstGainTEVTable )
{
  DB_INT32 i32RetVal = -1;

  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stGainTEV) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stGainTEV),
                                          sizeof(IMB_GAIN_TEV_TABLE_STRUCT), (unsigned char*)  pstGainTEVTable);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_GTEV_TABLE_STRUCT stResp = {0};

  if (NULL == pstGainTEVTable || eSectionType >= IMB_INVALID_FLASH_SECTION)
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = (unsigned char) eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_GTEV_TABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),   // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *pstGainTEVTable = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionGainTEVTable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionGainTEVTable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionGainTEVTable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionGainTEVTable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionGainTEVTable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionSetPoints2(const IMB_FLASH_SECTION_ENUM eSectionType, const float fPresSetPoint, const float fTempSetPoint )
{
  DB_INT32 i32RetVal = -1;

  {
    float temp = fPresSetPoint;
    i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fPresSetPoint) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fPresSetPoint),
                                            sizeof(float) ,  (unsigned char *) & temp);
  }

  if(i32RetVal >=0 )
  {
    float temp2 = fTempSetPoint;
    i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fTempSetPoint) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fTempSetPoint),
                                            sizeof(float) ,  (unsigned char *) & temp2);
  }

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_SET_POINTS_STRUCT stSectionSetPointsCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  // DEFAULT SECTION no longer used, wyu - May, 2014
  if ( (eSectionType >= IMB_INVALID_FLASH_SECTION) )// || (IMB_DEFAULT_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stSectionSetPointsCmd.bySectionType = (unsigned char) eSectionType;
  stSectionSetPointsCmd.fPresSetPoint = fPresSetPoint;
  stSectionSetPointsCmd.fTempSetPoint = fTempSetPoint;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionSetPointsCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_SET_POINTS);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionSetPointsCmd, // Command
                                                     sizeof (stSectionSetPointsCmd),           // Size of command
                                                     (unsigned char *) &stResp,          // Response from remote board
                                                     sizeof (stResp),            // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionSetPoints() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionSetPoints(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionSetPoints(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionSetPoints(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionSetPoints(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionSetPoints(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionSetPoints2( const IMB_FLASH_SECTION_ENUM eSectionType, float * pfPresSetPoint, float *pfTempSetPoint )
{
  DB_INT32 i32RetVal = -1;

  {
    float temp = 0;
    i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fPresSetPoint) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fPresSetPoint),
                                            sizeof(float) , (unsigned char*) & temp);
    if(pfPresSetPoint != NULL)
      *pfPresSetPoint = temp;
  }

  if(i32RetVal >= 0)
  {
    float temp2 = 0;
    i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fTempSetPoint) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fTempSetPoint),
                                            sizeof(float) , (unsigned char*) & temp2);
    if(pfTempSetPoint != NULL)
      *pfTempSetPoint = temp2;
  }

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};  
  CAN_CMD_IMB_SECTION_SET_POINTS_STRUCT stResp = {0};

  // DEFAULT SECTION no longer used, wyu - May, 2014
  if ( (NULL == pfPresSetPoint) || (NULL == pfTempSetPoint) || (eSectionType >= IMB_INVALID_FLASH_SECTION) ) // || (IMB_DEFAULT_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = eSectionType;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_SET_POINTS);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),       // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),     // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          * pfPresSetPoint = stResp.fPresSetPoint;
          * pfTempSetPoint = stResp.fTempSetPoint;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionSetPoints(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionSetPoints(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionSetPoints(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionSetPoints(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionSetPoints(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetMirrorAVTEnable2( const unsigned char byAVTEnable )
{
  DB_INT32 i32RetVal = -1;
  IMB_FLASH_SECTION_ENUM eSectionType = IMB_MIRROR_FLASH_SECTION;
  unsigned char temp = byAVTEnable;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          INVALID_OFFSET2 ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2, byAVTEnabled),
                                          sizeof(unsigned char), (unsigned char *) & temp);

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_MIRROR_AVT_ENABLE_STRUCT stMirrorAVTEnableCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  stMirrorAVTEnableCmd.byAVTEnabled = byAVTEnable;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stMirrorAVTEnableCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_MIRROR_AVT_ENABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stMirrorAVTEnableCmd, // Command
                                                     sizeof (stMirrorAVTEnableCmd),          // Size of command
                                                     (unsigned char *) &stResp,          // Response from remote board
                                                     sizeof (stResp),            // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetMirrorAVTEnable() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetMirrorAVTEnable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetMirrorAVTEnable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetMirrorAVTEnable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetMirrorAVTEnable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetMirrorAVTEnable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetMirrorAVTEnable2( unsigned char * pbyAVTEnable )
{
  DB_INT32 i32RetVal = -1;
  IMB_FLASH_SECTION_ENUM eSectionType = IMB_MIRROR_FLASH_SECTION;
  unsigned char temp = 0;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          INVALID_OFFSET2 ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2, byAVTEnabled),
                                          sizeof(unsigned char), (unsigned char*) & temp);
  if(pbyAVTEnable != NULL)
    *pbyAVTEnable = temp;

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_IMB_MIRROR_AVT_ENABLE_STRUCT stResp = {0};

  if (NULL == pbyAVTEnable)
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_IMB_FN_GET_MIRROR_AVT_ENABLE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),     // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          * pbyAVTEnable = stResp.byAVTEnabled;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetMirrorAVTEnable(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetMirrorAVTEnable(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetMirrorAVTEnable(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetMirrorAVTEnable(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetMirrorAVTEnable(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetGlobalDataIMBInit2( unsigned char byIMBInit )
{
  DB_INT32 i32RetVal = -1;
  unsigned char temp = byIMBInit;

  i32RetVal = SetFlashImage2( (unsigned char *) (& temp),
                              IMB_GLOBAL_FLASH_SECTION,
                              offsetof(IMB_GLOBAL_DATA_STRUCT2, byIMBInit),
                              sizeof(unsigned char) );

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_GLOBAL_INIT_STRUCT stGlobalIMBInit = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  stGlobalIMBInit.byIMBInit = byIMBInit;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stGlobalIMBInit.stCmd.byCmdAck, CMD_IMB_FN_SET_GLOBAL_DATA_IMB_INIT);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stGlobalIMBInit, // Command
                                                     sizeof (stGlobalIMBInit),           // Size of command
                                                     (unsigned char *) &stResp,          // Response from remote board
                                                     sizeof (stResp),            // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetGlobalDataIMBInit() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetGlobalDataIMBInit(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetGlobalDataIMBInit(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetGlobalDataIMBInit(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetGlobalDataIMBInit(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetGlobalDataIMBInit(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::SetSectionCarrierCalFactors2( const IMB_FLASH_SECTION_ENUM eSectionType, const float fZeroCalFaq, const float fSpanCalFaq )
{

  DB_INT32 i32RetVal = -1;
  {
    float temp = fZeroCalFaq;
    i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fCarrierPressZeroCalFaq) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fCarrierPressZeroCalFaq),
                                            sizeof(float) ,  (unsigned char *) & temp);
  }

  if(i32RetVal >= 0)
  {
    float temp2 = fSpanCalFaq;
    i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fCarrierPressSpanCalFaq) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fCarrierPressSpanCalFaq),
                                            sizeof(float) ,  (unsigned char *) & temp2);
  }

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_CAL_FAQ_STRUCT stSectionCarrCalFaqCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  // DEFAULT SECTION no longer used, wyu - May, 2014
  if ( (eSectionType >= IMB_INVALID_FLASH_SECTION) ) // || (IMB_DEFAULT_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stSectionCarrCalFaqCmd.bySectionType = eSectionType;
  stSectionCarrCalFaqCmd.fZeroCalFaq = fZeroCalFaq;
  stSectionCarrCalFaqCmd.fSpanCalFaq = fSpanCalFaq;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionCarrCalFaqCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_CARRIER_CAL_FAQ);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionCarrCalFaqCmd, // Command
                                                     sizeof (stSectionCarrCalFaqCmd),          // Size of command
                                                     (unsigned char *) &stResp,          // Response from remote board
                                                     sizeof (stResp),            // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionCarrierCalFaq() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionCarrierCalFaq(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionCarrierCalFaq(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionCarrierCalFaq(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionCarrierCalFaq(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionCarrierCalFaq(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

int CIMBComm::GetSectionCarrierCalFactors2(const IMB_FLASH_SECTION_ENUM eSectionType, float * pfZeroCalFaq, float * pfSpanCalFaq )
{
  DB_INT32 i32RetVal = -1;

  {
    float temp = 0;
    i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fCarrierPressZeroCalFaq) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fCarrierPressZeroCalFaq),
                                            sizeof(float) , (unsigned char*) & temp);
    if(pfZeroCalFaq != NULL)
      *pfZeroCalFaq = temp;
  }

  if(i32RetVal >= 0)
  {
    float temp2 = 0;
    i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                            offsetof(IMB_FACTORY_DATA_STRUCT2, fCarrierPressSpanCalFaq) ,
                                            offsetof(IMB_MIRROR_DATA_STRUCT2,  fCarrierPressSpanCalFaq),
                                            sizeof(float) , (unsigned char*) & temp2);
    if(pfSpanCalFaq != NULL)
      *pfSpanCalFaq = temp2;
  }

  return i32RetVal;

#ifdef IMB_COMM_V1

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_CAL_FAQ_STRUCT stResp = {0};

  // DEFAULT SECTION no longer used, wyu - May, 2014
  if ( (NULL == pfZeroCalFaq) || (NULL == pfSpanCalFaq) || (eSectionType >= IMB_INVALID_FLASH_SECTION) ) // || (IMB_DEFAULT_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }
  
  stIMBCmd.bySectionType = eSectionType;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_CARRIER_CAL_FAQ);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),       // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),     // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          * pfZeroCalFaq = stResp.fZeroCalFaq;
          * pfSpanCalFaq = stResp.fSpanCalFaq;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionCarrierCalFaq(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionCarrierCalFaq(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionCarrierCalFaq(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionCarrierCalFaq(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionCarrierCalFaq(): Function called before Open Call!");
  }

  return nRetVal;

#endif

}

// NOT USED
/*
int CIMBComm::SetSectionSampleCalFaq( IMB_FLASH_SECTION_ENUM eSectionType, float fZeroCalFaq, float fSpanCalFaq )
{

  if(m_nAPIVer == 2)
  {
    //Zero Cal(ibration) Factors - Sample
    //Span Cal(ibration) Factors - Sample
    //This oreder has to match the IMB_FACTORY_DATA_STRUCT and IMB_MIRROR_DATA_STRUCT !!!
    float temp[2] = { fZeroCalFaq, fSpanCalFaq };

    unsigned int unOffset = 
      (eSectionType == IMB_FACTORY_FLASH_SECTION) ? offsetof(IMB_FACTORY_DATA_STRUCT, fSamplePressZeroCalFaq) : offsetof(IMB_MIRROR_DATA_STRUCT, fSamplePressZeroCalFaq);

    unsigned int unDataLen = sizeof(float) * 2;

    return SetFlashImage( (unsigned char *) (temp), eSectionType, unOffset, unDataLen );
  }

  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_CAL_FAQ_STRUCT stSectionSampCalFaqCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  // DEFAULT SECTION no longer used, wyu - May, 2014
  if ( (eSectionType >= IMB_INVALID_FLASH_SECTION) ) // || (IMB_DEFAULT_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stSectionSampCalFaqCmd.bySectionType = eSectionType;
  stSectionSampCalFaqCmd.fZeroCalFaq = fZeroCalFaq;
  stSectionSampCalFaqCmd.fSpanCalFaq = fSpanCalFaq;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stSectionSampCalFaqCmd.stCmd.byCmdAck, CMD_IMB_FN_SET_SECTION_SAMPLE_CAL_FAQ);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stSectionSampCalFaqCmd, // Command
                                                     sizeof (stSectionSampCalFaqCmd),          // Size of command
                                                     (unsigned char *) &stResp,          // Response from remote board
                                                     sizeof (stResp),            // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetSectionSampleCalFaq() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetSectionSampleCalFaq(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetSectionSampleCalFaq(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetSectionSampleCalFaq(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetSectionSampleCalFaq(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetSectionSampleCalFaq(): Function called before Open Call!");
  }

  return nRetVal;
}
*/

// NOT USED
/*
int CIMBComm::GetSectionSampleCalFaq( IMB_FLASH_SECTION_ENUM eSectionType, float * pfZeroCalFaq, float * pfSpanCalFaq )
{

  if(m_nAPIVer == 2)
  {
    //Zero Cal(ibration) Factors - Sample
    //Span Cal(ibration) Factors - Sample
    //This oreder has to match the IMB_FACTORY_DATA_STRUCT and IMB_MIRROR_DATA_STRUCT !!!
    float temp[2] = { 0, 0};
    unsigned int unOffset = 
      (eSectionType == IMB_FACTORY_FLASH_SECTION) ? offsetof(IMB_FACTORY_DATA_STRUCT, fSamplePressZeroCalFaq) : offsetof(IMB_MIRROR_DATA_STRUCT, fSamplePressZeroCalFaq);

    unsigned int unDataLen = sizeof(float) * 2;
    unsigned int refLenRead = 0;

    int ret = GetFlashImage( (unsigned char*) temp, eSectionType, unOffset, unDataLen , refLenRead);

    if(pfZeroCalFaq != NULL)
      *pfZeroCalFaq = temp[0];

    if(pfSpanCalFaq != NULL)
      *pfSpanCalFaq = temp[1];

    return ret;
  }


  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT stIMBCmd = {0};
  CAN_CMD_IMB_SECTION_CAL_FAQ_STRUCT stResp = {0};

  // DEFAULT SECTION no longer used, wyu - May, 2014
  if ( (NULL == pfZeroCalFaq) || (NULL == pfSpanCalFaq) || (eSectionType >= IMB_INVALID_FLASH_SECTION) ) // || (IMB_DEFAULT_FLASH_SECTION == eSectionType) )
  {
    return ERR_INVALID_ARGS;
  }

  stIMBCmd.bySectionType = eSectionType;
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stIMBCmd.stCmd.byCmdAck, CMD_IMB_FN_GET_SECTION_SAMPLE_CAL_FAQ);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stIMBCmd, // Command
                                                     sizeof (stIMBCmd),       // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),     // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          * pfZeroCalFaq = stResp.fZeroCalFaq;
          * pfSpanCalFaq = stResp.fSpanCalFaq;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetSectionSampleCalFaq(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetSectionSampleCalFaq(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetSectionSampleCalFaq(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetSectionSampleCalFaq(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetSectionSampleCalFaq(): Function called before Open Call!");
  }

  return nRetVal;
}
*/

//Get IMB Structure ID
int CIMBComm::GetStructureID( IMB_STRUCTURE_ID_STRUCT * pstIMBDbId )
{

  IMB_IMB_META_INFO metaInfo;

  int nRetVal = GetIMBInfo( & metaInfo );

  FixEndian(metaInfo.stIMBStruId.struMajor);
  FixEndian(metaInfo.stIMBStruId.struMinor);

  * pstIMBDbId = metaInfo.stIMBStruId;

  return nRetVal;
}

//Get IMB API ID
int CIMBComm::GetAPIID( IMB_API_ID_STRUCT * pstIMBApiId )
{

  IMB_IMB_META_INFO metaInfo;

  int nRetVal = GetIMBInfo( & metaInfo );

  FixEndian(metaInfo.stIMBApiId.apiMajor);
  FixEndian(metaInfo.stIMBApiId.apiMinor);

  * pstIMBApiId = metaInfo.stIMBApiId;

  return nRetVal;
}



//Get IMB serianl num in hex
//returns 1 on success, returns -1 on failure
int CIMBComm::ConvertIMBSerialNumberToHex( const IMB_IMB_META_INFO & refIMBInfo, std::string & strImbSerialNumInHex )
{

  int nRetVal = 1;
  //IMB_IMB_META_INFO stIMBInfo;

  //int nRetVal = GetIMBInfo( & stIMBInfo );

  //0. print the original data
  printf( "IMB Serial Num  - \n" ); 
  printf( "------------------------------\n" ); 

  //printf( "byIMBSerialNumber (CHAR) = %.*s\n", IMB_SERIAL_NUMBER_LENGTH, refIMBInfo.byIMBSerialNum );
  printf( "byIMBSerialNumber (HEX, high to low, last to first)  = " );
  printf( "\n");        
        
  for( int nInd = IMB_SERIAL_NUMBER_LENGTH -1; nInd >= 0; nInd-- )
  {
    printf( "0x%02X ", refIMBInfo.byIMBSerialNum[nInd] );
  }
  printf( "\n");


  //1. copy bytes to a temp char array
  DB_CHAR cModuleSNo[IMB_SERIAL_NUMBER_LENGTH+1];
  memcpy(cModuleSNo, refIMBInfo.byIMBSerialNum, IMB_SERIAL_NUMBER_LENGTH);
  cModuleSNo[IMB_SERIAL_NUMBER_LENGTH] = '\0';

  //2. IMB Serial Number is formed by 4 different integers sent as 16 characters, so perform 
  //Endian Conversion by accessing each of the 4 integers
  DB_UINT32 *pIMBSerilNo = (DB_UINT32 *)cModuleSNo;
  DB_UINT32  wIMBSerilNo[4];                         // IMB serial num, 4 words
  for( int iNdex = 0; iNdex < 4; iNdex++ )
  {
    DB_UINT32 ui32SingleInt = *pIMBSerilNo;
    printf("word %d before Endian conversion: %08X", iNdex, (unsigned int)ui32SingleInt);         
    printf( "\n");

    FixEndian(ui32SingleInt);
    printf("word %d after  Endian conversion: %08X", iNdex, (unsigned int)ui32SingleInt);         

    wIMBSerilNo[iNdex] = ui32SingleInt;
    pIMBSerilNo++;
    printf( "\n");
  }
  printf( "\n");

  //3. convert to Hex Format string
  DB_CHAR cHexIMBSerialNumber[IMB_SERIAL_NUMBER_LENGTH_IN_CHARS+1] = {'\0'};
  sprintf(cHexIMBSerialNumber,"%08X%08X%08X%08X", (unsigned int)wIMBSerilNo[3],(unsigned int)wIMBSerilNo[2], (unsigned int)wIMBSerilNo[1], (unsigned int)wIMBSerilNo[0]);
  cHexIMBSerialNumber[IMB_SERIAL_NUMBER_LENGTH_IN_CHARS] = '\0';

  if(strlen(cHexIMBSerialNumber) != IMB_SERIAL_NUMBER_LENGTH_IN_CHARS)
  {
    printf("Hex string length error, len = %d", strlen(cHexIMBSerialNumber));
    nRetVal = -1;
  }
  printf( "Hex string = %s\n ", cHexIMBSerialNumber );

  // End converting IMB serial num to string

  strImbSerialNumInHex = cHexIMBSerialNumber;

  return nRetVal;
}


//Get IMB Info
int CIMBComm::GetIMBInfo( IMB_IMB_META_INFO * pstIMBInfo )
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_IMB_INFO_STRUCT stResp = {0};

  if (NULL == pstIMBInfo)
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_IMB_FN_GET_STRUCTURE_ID);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),      // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp),   // Size of expected response
                                                     FALSE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          //FixEndian(stResp.stIMBDbId.usMajor);
          //FixEndian(stResp.stIMBDbId.usMinor);
          *pstIMBInfo = stResp.stData;
          
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetStructureID(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetStructureID(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetStructureID(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetStructureID(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetStructureID(): Function called before Open Call!");
  }

  return nRetVal;
}


// Set Module Serial Number
int CIMBComm::SetGlobalDataModuleSerialNum2( unsigned char * pchData)
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = SetFlashImage2(pchData,
                             IMB_GLOBAL_FLASH_SECTION,
                             offsetof(IMB_GLOBAL_DATA_STRUCT2, byModuleSerialNum) ,
                             sizeof(unsigned char) * MODULE_SERIAL_NUMBER_LENGTH );

  return i32RetVal;
}

// Get Module Serial Number
int CIMBComm::GetGlobalDataModuleSerialNum2( unsigned char * pchData )
{
  DB_INT32 i32RetVal = -1;
  unsigned int refLenRead = 0;
  i32RetVal = GetFlashImage2(pchData,
                             IMB_GLOBAL_FLASH_SECTION,
                             offsetof(IMB_GLOBAL_DATA_STRUCT2, byModuleSerialNum),
                             sizeof(unsigned char) * MODULE_SERIAL_NUMBER_LENGTH,
                             refLenRead);

  return i32RetVal;
}

// Set the the flash image (write data to IMB)
// THIS VERSION DOES NOT HAVE THE LENTH CONSTRAINT !!!!
int CIMBComm::SetFlashImage2( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int unOffset, const unsigned int unLength)
{
  //the while loop
  unsigned int count = 0;
  int ret = 0;
  do
  {
    // is this last batch?
    bool bLastBatch = ((count+1) * FLASH_IMAGE_DATA_LENGTH) < unLength;
    // the data length to be sent in this batch
    unsigned int unDataSent = 
      bLastBatch ? FLASH_IMAGE_DATA_LENGTH : (unLength % FLASH_IMAGE_DATA_LENGTH);
    // the offst to be used in this batch
    unsigned int unOffsetSent = unOffset + count * FLASH_IMAGE_DATA_LENGTH;
    // the data pointer for this batcch
    unsigned char * pData = (unsigned char *) pchFlashImage + count * FLASH_IMAGE_DATA_LENGTH;

    ret = SetFlashImage( pData, bySection, unOffsetSent, unDataSent );
    if(ret < 0)
      return ret;

    count++;
  } while ( (count * FLASH_IMAGE_DATA_LENGTH) < unLength);

  return ret;
}


// Set the the flash image (write data to IMB)
// pchFlashImage -- buffer of data, max len 128 (FLASH_IMAGE_DATA_LENGTH) bytes
// offset        -- offset to write the IMB flash image
// length        -- length to write the IMB flash image
int CIMBComm::SetFlashImage( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int unOffset, const unsigned int unLength)
{
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_IMB_WRITE_DATA_STRUCT stUpdateFlashImageCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  memcpy(stUpdateFlashImageCmd.inflashData, pchFlashImage, unLength);

  //which section to read
  stUpdateFlashImageCmd.bySectionType = bySection;

  stUpdateFlashImageCmd.uinFlashOffset = unOffset;
  FixEndian(stUpdateFlashImageCmd.uinFlashOffset);

  stUpdateFlashImageCmd.uinFlashLen    = unLength;
  FixEndian(stUpdateFlashImageCmd.uinFlashLen);

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stUpdateFlashImageCmd.stCmd.byCmdAck, CMD_IMB_FN_WRITE_IMB_DATA);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stUpdateFlashImageCmd, // Command
                                                     sizeof (stUpdateFlashImageCmd),           // Size of command
                                                     (unsigned char *) &stResp,              // Response from remote board
                                                     sizeof (stResp),                    // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::SetFlashImage() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::SetFlashImage(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::SetFlashImage(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::SetFlashImage(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::SetFlashImage(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::SetFlashImage(): Function called before Open Call!");
  }

  return nRetVal;
}

// Read the the flash image from IMB
// THIS VERSION DOES NOT HAVE THE LENTH CONSTRAINT !!!!
int CIMBComm::GetFlashImage2( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int offset, const unsigned int length, unsigned int& refActualLen )
{
  //the while loop
  unsigned int count = 0;
  int ret = 0;
  do
  {
    // is this last batch?
    bool bLastBatch = ((count+1) * FLASH_IMAGE_DATA_LENGTH) < length;
    // the data length to be sent in this batch
    unsigned int unDataSent = 
      bLastBatch ? FLASH_IMAGE_DATA_LENGTH : (length % FLASH_IMAGE_DATA_LENGTH);
    // the offset to be used in this batch
    unsigned int unOffsetSent = offset + count * FLASH_IMAGE_DATA_LENGTH;
    // the data pointer for this batcch
    unsigned char * pData = (unsigned char *) pchFlashImage + count * FLASH_IMAGE_DATA_LENGTH;

    ret = GetFlashImage( pData, bySection, unOffsetSent, unDataSent, refActualLen );
    if(ret < 0)
      return ret;

    count++;
  } while ( (count * FLASH_IMAGE_DATA_LENGTH) < length);

  return ret;
}



// Read the the flash image from IMB
// pchFlashImage -- buffer to store image image, max len 128 (FLASH_IMAGE_DATA_LENGTH) bytes
// offset        -- offset to read the IMB flash image
// length        -- length to read the IMB flash image
// refRetLen     -- the length of the ACTUAL data read from IMB
int CIMBComm::GetFlashImage( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int offset, const unsigned int length, unsigned int& refActualLen )
{
  int nRetVal = ERR_SUCCESS;

  //CmdAckUnion stCmd = {0};
  CAN_CMD_IMB_READ_DATA_REQUEST_STRUCT stFlashCmd = {0};    // the command to read flash image
  CAN_CMD_IMB_READ_DATA_STRUCT stResp = {0};           // the command to receive 

  if (NULL == pchFlashImage)
  {
    return ERR_INVALID_ARGS;
  }

  //which section to read
  stFlashCmd.bySectionType = bySection;

  //offset
  stFlashCmd.nFlashOffset = offset; 
  FixEndian(stFlashCmd.nFlashOffset);

  printf("offset = %u, after FixEndian = %u \n", offset, stFlashCmd.nFlashOffset);

  //flash length to read
  stFlashCmd.nFlashLen = length;
  FixEndian(stFlashCmd.nFlashLen);
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stFlashCmd.stCmd.byCmdAck, CMD_IMB_FN_READ_IMB_DATA );

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stFlashCmd, // Command
                                                     sizeof (stFlashCmd),                 // Size of command
                                                     (unsigned char *) &stResp,                     // Response from remote board
                                                     sizeof (stResp),                       // Size of expected response
                                                     TRUE,
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
        	// error - read out of bounds, set invalid-args error
              DEBUG1("CIMBComm::GetFlashImage(): Read out of bounds: %d", nRetVal);
              nRetVal = ERR_INVALID_ARGS;
          }
          else
          {
            FixEndian(stResp.flashDataLen);
            //actual data length
            refActualLen = stResp.flashDataLen;

            if(refActualLen >0)
            	memcpy(pchFlashImage, stResp.flashData, refActualLen);
          
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::GetFlashImage(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::GetFlashImage(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::GetFlashImage(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::GetFlashImage(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::GetFlashImage(): Function called before Open Call!");
  }

  return nRetVal;
}

/*----------------------------------------------------------------------------
 * Sets the flash image of FACTORY or MIRROR section
 *--------------------------------------------------------------------------*/
DB_INT32 CIMBComm::setFactoryMirrorFlashImage2(const IMB_FLASH_SECTION_ENUM _eIMBSection,
                                               const size_t factoryOffset, const size_t mirrorOffset,
                                               unsigned int datalen,  unsigned char * pstSwTEVTable)
{
  DB_INT32 i32RetVal = -1;
  unsigned int unOffset =
      (_eIMBSection == IMB_FACTORY_FLASH_SECTION) ? factoryOffset : mirrorOffset;

  if(unOffset == INVALID_OFFSET2)
  {
    DEBUG1("_setFlashImage2 - Invalid Offset %u", unOffset);
    //SysLog((ERR_SEVERITY_ENUM)ERR_FATAL,(ERR_NUM_ENUM)ERR_WARNING,__FILE__,
    //    __LINE__,"_setFlashImage2 - Invalid Offset %u", unOffset);
    return i32RetVal;
  }

  unsigned int unDataLen = datalen;
  i32RetVal = SetFlashImage2( (unsigned char *) pstSwTEVTable, _eIMBSection, unOffset, unDataLen );
  return i32RetVal;

}


/*----------------------------------------------------------------------------
 * Gets the flash image of FACTORY or MIRROR section
 *--------------------------------------------------------------------------*/
DB_INT32 CIMBComm::getFactoryMirrorFlashImage2(const IMB_FLASH_SECTION_ENUM _eIMBSection,
                                               const size_t factoryOffset, const size_t mirrorOffset,
                                               unsigned int datalen, unsigned char * pstSwTEVTable)
{
  DB_INT32 i32RetVal = -1;

  unsigned int unOffset =
      (_eIMBSection == IMB_FACTORY_FLASH_SECTION) ? factoryOffset : mirrorOffset;

  if(unOffset == INVALID_OFFSET2)
  {
    DEBUG1("_getFlashImage2 - Invalid Offset %u", unOffset);
    //SysLog((ERR_SEVERITY_ENUM)ERR_FATAL,(ERR_NUM_ENUM)ERR_WARNING,__FILE__,
    //   __LINE__,"_getFlashImage2 - Invalid Offset %u", unOffset);
    return i32RetVal;
  }

  unsigned int unDataLen = datalen;
  unsigned int refLenRead = 0;

  i32RetVal = GetFlashImage2( pstSwTEVTable, _eIMBSection, unOffset, unDataLen , refLenRead);

  return i32RetVal;
}

/*----------------------------------------------------------------------------
 * Writes data from IMB RAM to IMB flash
 * This should be called after the SetFlashImage() call/command
 *--------------------------------------------------------------------------*/
int CIMBComm::UpdateFlash( )
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stUpdateFlashCmd = {0};
  CAN_IMB_STATUS_STRUCT stResp = {0};

  //stMirrorNumAnalysisCmd.nNumAnalysis = nMirrorNumAnalysis;

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(& stUpdateFlashCmd.byCmdAck, CMD_IMB_FN_UPDATE_FLASH);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) & stUpdateFlashCmd, // Command
                                                     sizeof (stUpdateFlashCmd),                // Size of command
                                                     (unsigned char *) &stResp,          // Response from remote board
                                                     sizeof (stResp),            // Size of expected response
                                                     TRUE, 
                                                     IMB_REQUEST_TIME_OUT );

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CIMBComm::UpdateFlash() : Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CIMBComm::UpdateFlash(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CIMBComm::UpdateFlash(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CIMBComm::UpdateFlash(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
    case SERIAL_COMM:
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CIMBComm::UpdateFlash(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CIMBComm::UpdateFlash(): Function called before Open Call!");
  }

  return nRetVal;
}

int CIMBComm::SetSectionSwTEVTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_SW_TEV_TABLE_STRUCT_EXT & stSwTEVTableExt )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stSwTEVExt) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stSwTEVExt),
                                          sizeof(IMB_SW_TEV_TABLE_STRUCT_EXT),  (unsigned char *) & stSwTEVTableExt);

  return i32RetVal;
}

int CIMBComm::GetSectionSwTEVTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_SW_TEV_TABLE_STRUCT_EXT * pstSwTEVTableExt )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stSwTEVExt) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stSwTEVExt),
                                          sizeof(IMB_SW_TEV_TABLE_STRUCT_EXT), (unsigned char*) pstSwTEVTableExt);

  return i32RetVal;
}

int CIMBComm::SetSectionCDTTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_CDT_TABLE_STRUCT_EXT & stCDTTableExt )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stCDTExt) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stCDTExt) ,
                                          sizeof(IMB_CDT_TABLE_STRUCT_EXT),  (unsigned char *) & stCDTTableExt);

  return i32RetVal;
}

int CIMBComm::GetSectionCDTTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_TABLE_STRUCT_EXT * pstCDTTableExt )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal= getFactoryMirrorFlashImage2(eSectionType,
                                         offsetof(IMB_FACTORY_DATA_STRUCT2, stCDTExt) ,
                                         offsetof(IMB_MIRROR_DATA_STRUCT2,  stCDTExt) ,
                                         sizeof(IMB_CDT_TABLE_STRUCT_EXT), (unsigned char*) pstCDTTableExt);

  return i32RetVal;
}


int CIMBComm::SetSectionCDTCustomNames2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_CDT_CUSTOM_NAME_STRUCT & stCDTCustom )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stCDTCustom) ,
                                          offsetof(IMB_MIRROR_DATA_STRUCT2,  stCDTCustom) ,
                                          sizeof(IMB_CDT_CUSTOM_NAME_STRUCT),  (unsigned char *) & stCDTCustom);

  return i32RetVal;
}

int CIMBComm::GetSectionCDTCustomNames2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_CUSTOM_NAME_STRUCT * pstCDTCustom )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal= getFactoryMirrorFlashImage2(eSectionType,
                                         offsetof(IMB_FACTORY_DATA_STRUCT2, stCDTCustom)  ,
                                         offsetof(IMB_MIRROR_DATA_STRUCT2,  stCDTCustom) ,
                                         sizeof(IMB_CDT_CUSTOM_NAME_STRUCT), (unsigned char*) pstCDTCustom);

  return i32RetVal;
}

int CIMBComm::SetSectionAVTTimeEventAdjTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT & stAVTTEATableExt )
{
  DB_INT32 i32RetVal = -1;
  // there is no AVT Time Event Adj Table in MIRROR
  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTTimeEventAdjExt) ,
                                          INVALID_OFFSET2,
                                          sizeof(IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT),  (unsigned char *) & stAVTTEATableExt);

  return i32RetVal;
}

int CIMBComm::GetSectionAVTTimeEventAdjTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT * pstAVTTEATableExt )
{
  DB_INT32 i32RetVal = -1;
  // there is no AVT Time Event Adj Table in MIRROR
  i32RetVal= getFactoryMirrorFlashImage2(eSectionType,
                                         offsetof(IMB_FACTORY_DATA_STRUCT2, stAVTTimeEventAdjExt) ,
                                         INVALID_OFFSET2,
                                         sizeof(IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT), (unsigned char*) pstAVTTEATableExt);

  return i32RetVal;
}

int CIMBComm::SetSectionMVAllowableDEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_MV_ALLOWABLE_DEV_STRUCT & stMVAllowableDEVTable )
{
  DB_INT32 i32RetVal = -1;

  i32RetVal = setFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stMVAllowableDEV) ,
                                          INVALID_OFFSET2 ,
                                          sizeof(IMB_MV_ALLOWABLE_DEV_STRUCT),  (unsigned char *) & stMVAllowableDEVTable);

  return i32RetVal;
}

int CIMBComm::GetSectionMVAllowableDEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_MV_ALLOWABLE_DEV_STRUCT * pstMVAllowableDEVTable )
{
  DB_INT32 i32RetVal = -1;

  i32RetVal = getFactoryMirrorFlashImage2(eSectionType,
                                          offsetof(IMB_FACTORY_DATA_STRUCT2, stMVAllowableDEV) ,
                                          INVALID_OFFSET2,
                                          sizeof(IMB_MV_ALLOWABLE_DEV_STRUCT), (unsigned char*) pstMVAllowableDEVTable);

  return i32RetVal;
}

int CIMBComm::SetGlobalDataApplicationType2( const IMB_APPLICATION_TYPE_STRUCT & stAppType )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = SetFlashImage2((unsigned char *) & stAppType,
                             IMB_GLOBAL_FLASH_SECTION,
                             offsetof(IMB_GLOBAL_DATA_STRUCT2, stApplicationType) ,
                             sizeof(IMB_APPLICATION_TYPE_STRUCT) );

  return i32RetVal;
}

int CIMBComm::GetGlobalDataApplicationType2( IMB_APPLICATION_TYPE_STRUCT * pstApplicationType )
{
  DB_INT32 i32RetVal = -1;
  unsigned int refLenRead = 0;
  i32RetVal = GetFlashImage2((unsigned char *) pstApplicationType,
                             IMB_GLOBAL_FLASH_SECTION,
                             offsetof(IMB_GLOBAL_DATA_STRUCT2, stApplicationType),
                             sizeof(IMB_APPLICATION_TYPE_STRUCT),
                             refLenRead);

  return i32RetVal;
}

int CIMBComm::SetGlobalDataMetaInfo2( const IMB_SECTION_META_INFO2 & _stMetaInfo )
{
  DB_INT32 i32RetVal = -1;
  i32RetVal = SetFlashImage2((unsigned char *) & _stMetaInfo,
                              IMB_GLOBAL_FLASH_SECTION,
                              offsetof(IMB_GLOBAL_DATA_STRUCT2, stMetaInfo) ,
                              sizeof(IMB_SECTION_META_INFO2) );

  return i32RetVal;
}

int CIMBComm::GetGlobalDataMetaInfo2( IMB_SECTION_META_INFO2 * pstMetaInfo )
{
  DB_INT32 i32RetVal = -1;
  unsigned int refLenRead = 0;
  i32RetVal = GetFlashImage2((unsigned char *) pstMetaInfo,
                              IMB_GLOBAL_FLASH_SECTION,
                              offsetof(IMB_GLOBAL_DATA_STRUCT2, stMetaInfo),
                              sizeof(IMB_SECTION_META_INFO2),
                              refLenRead);

  return i32RetVal;
}
