/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FFBComm.cpp
 *
 *  Description: Foundation Field Bus board access layer.
 *
 *  Copyright:        Copyright (c) 2011-2012, 
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C++'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *  Revision History:
 *  $Id: FFBComm.cpp,v 1.13 2015/05/04 14:49:26 EMRSN\sboyidi Exp $
 *  $Log: FFBComm.cpp,v $
 *  Revision 1.13  2015/05/04 14:49:26  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.12  2014/06/26 10:00:16  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.11  2012/03/21 06:48:49  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.10  2010/09/21 21:44:19  emrsn\LRatner
 *  Using NUM_STREAM_CONFIG_INFO definition/
 *
 *  Revision 1.9  2010/09/17 20:43:40  emrsn\LRatner
 *  Getting MAX_STREAM count from global.h
 *
 *  Revision 1.8  2009/07/18 09:03:10  epm-na\rsaha
 *  Enabled code to count number of retries made in Reliability layer.
 *
 *  Revision 1.7  2009/05/13 22:56:45  epm-na\rsaha
 *  Revised with API definition to query FFB Device capability information.
 *
 *  Revision 1.6  2009/04/06 21:27:54  epm-na\rsaha
 *  Revised HAL layer with interface to retrieve 64bit FFB server time.
 *
 *  Revision 1.5  2009/04/02 20:46:26  epm-na\rsaha
 *  Added method to retrieve streaming FD  for CFFBCmmd object.
 *
 *  Revision 1.4  2009/01/26 22:24:11  epm-na\rsaha
 *  Updated to send the Detector error status information.
 *
 *  Revision 1.3  2008/11/10 23:56:46  epm-na\rsaha
 *  Updated HAL layer with interfaces to help an application communicate with the FFB board.
 *
 *  Revision 1.2  2008/11/05 16:44:51  epm-na\rsaha
 *  Updated to remove a compiler error.
 *
 *  Revision 1.1  2008/10/22 22:13:25  epm-na\rsaha
 *  Initial version with minimum required API's to send/receive large data to/from FFB board. Once the protocol structure is defined for FFB communication with GC this file will be updated with the required APIs.
 *
 *************************************************************************/

#include "string.h"
#include "debug.h"
#include "FixEndian.h"

#include <vector>
#include <string>
#include "FFBComm.h"
#include "ResolveDevName.h"

#define BUFF_SZ_4_FFB_CAP_RESP  1024
#define BUFF_SZ_4_SLOT_NAME     100

CFFBStatus::CFFBStatus()
{
  m_bySeqNum = 0;
}

CFFBStatus::~CFFBStatus()
{
}

int CFFBStatus::OpenHal( char* pszDevName )
{
  if( NULL == pszDevName )
  {
    return ERR_INVALID_ARGS;
  }
    
  return CBaseDev::OpenHal( pszDevName, FALSE );
}

int CFFBStatus::CloseHal()
{
  return CBaseDev::CloseHal();
}

int CFFBStatus::GetRetryAttempts()
{
  return m_pobReliabilityCAN->m_nRetryAttempts;
}


int CFFBStatus::SendStatusInfo( unsigned char * ucTXData, 
                                unsigned int unTXNumData, 
                                unsigned char * ucRXData, 
                                unsigned int unRXNumData,
                                unsigned int unTimeOut )
{
  int nRetVal = ERR_SUCCESS;
    
  if( m_bIsDevOpen )
  {
    switch( m_eCommType )
    {
    case CAN_COMM:
      if( m_pobCAN )
      {
        //Send the command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp(   ucTXData,           // Command
                                                        unTXNumData,        // Size of command
                                                        ucRXData,           // Response from remote board
                                                        unRXNumData,        // Size of expected response
                                                        FALSE,              // Not stream data
                                                        unTimeOut );        // Time out interval
    
    
        //Check if we got the correct response packet
        if( nRetVal < 0 )
        {
          DEBUG1("CFFBStatus::SendStatusInfo() - CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
        else if( nRetVal > (int)unRXNumData )
        {
          DEBUG1("CFFBStatus::SendStatusInfo() - Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }                    
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG1("CFFBStatus::SendStatusInfo() - Unexpected invalid pointer!");
      }
    
      break;
    case SPI_COMM:
      //TODO : To be implemented.
    case SERIAL_COMM:
      //TODO : To be implemented.
    case COMM_NONE:                     
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG1( "CFFBStatus::SendStatusInfo() - Invalid switch case!" );
      break;
    }
  }
  else
  {
    //Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG1("CFFBStatus::SendStatusInfo() - Function called before Open Call!");
  }
    
  return nRetVal;
}


int CFFBStatus::SendGCConfig( const char * szGCFirmwareRev,
                              const char * szGCSerialNumber,
                              BRAND_SETTING eBrandSetting,
                              FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_CONFIG_STRUCT stGCConfig;
  //Set message sequence number.
  stGCConfig.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stGCConfig.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stGCConfig.stHeaderInfo.CmdInfo.byCmd, GC_CONFIG_MSG );

  //Pack data.
  strncpy( (char *) stGCConfig.GCFirmwareRev, szGCFirmwareRev, GC_FIRMWARE_REV_STR_LENGTH );
  strncpy( (char *) stGCConfig.GCSerialNum, szGCSerialNumber, GC_SERIAL_NUM_STR_LENGTH );
  stGCConfig.byBrandSetting = (unsigned char) eBrandSetting;
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stGCConfig, sizeof( stGCConfig ), 
                                (unsigned char *) & stAck, sizeof( stAck ) );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) && 
        GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_CONFIG_MSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendGCUserTag( const char * szGCUserTag,
                               FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_USER_TAG_STRUCT stGCUserTag;
  //Set message sequence number.
  stGCUserTag.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stGCUserTag.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stGCUserTag.stHeaderInfo.CmdInfo.byCmd, GC_USER_TAG_MSG );

  //Pack data.
  strncpy( (char *) stGCUserTag.GCUserTag, szGCUserTag, GC_USER_TAG_STR_LENGTH );
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stGCUserTag, sizeof( stGCUserTag ), 
                                (unsigned char *) & stAck, sizeof( stAck ) );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_USER_TAG_MSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendNWConfig(   const char * szIPAddr1,
                                const char * szNetMask1,
                                const char * szIPAddr2,
                                const char * szNetMask2,
                                const char * szDefaultGateway,
                                FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_NETWORK_CONFIG_STRUCT stGCNwConfig;
  //Set message sequence number.
  stGCNwConfig.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stGCNwConfig.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stGCNwConfig.stHeaderInfo.CmdInfo.byCmd, GC2FFB_NETW_CONFIG_MSG );

  //Pack data.
  strncpy( (char *) stGCNwConfig.stNtwInfo.IPAddress1, szIPAddr1, GC_IP_ADDR_STR_LENGTH );
  strncpy( (char *) stGCNwConfig.stNtwInfo.NetMask1, szNetMask1, GC_IP_ADDR_STR_LENGTH );
  strncpy( (char *) stGCNwConfig.stNtwInfo.IPAddress2, szIPAddr2, GC_IP_ADDR_STR_LENGTH );
  strncpy( (char *) stGCNwConfig.stNtwInfo.NetMask2, szNetMask2, GC_IP_ADDR_STR_LENGTH );
  strncpy( (char *) stGCNwConfig.stNtwInfo.DefaultGatewayIPAddr, szDefaultGateway, GC_IP_ADDR_STR_LENGTH );
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stGCNwConfig, sizeof( stGCNwConfig ), 
                                (unsigned char *) & stAck, sizeof( stAck ) );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC2FFB_NETW_CONFIG_MSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendStreamConfig( GC_STREAM_CONFIG_STRUCT * pStreamConfig,
                                  FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_STREAM_INIT_STRUCT stGCStreamConfig;
  //Set message ID.   
  stGCStreamConfig.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stGCStreamConfig.stHeaderInfo.CmdInfo.byCmd, GC_STREAM_INIT_MSG );
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = ERR_SUCCESS;
  //Send one stream config infomation at a time.
  for( int nInd = 0; nInd < NUM_STREAM_CONFIG_INFO; nInd++ )
  {
    //Set message sequence number.
    stGCStreamConfig.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;

    //Pack data.
    stGCStreamConfig.stStreamInfo.byStreamNum = pStreamConfig->stStreamInfo[nInd].byStreamNum;
    stGCStreamConfig.stStreamInfo.byStreamUsage = pStreamConfig->stStreamInfo[nInd].byStreamUsage;
    stGCStreamConfig.stStreamInfo.byDetectorAssigned = pStreamConfig->stStreamInfo[nInd].byDetectorAssigned;
    strncpy( (char *) stGCStreamConfig.stStreamInfo.StreamName, 
             (const char *) pStreamConfig->stStreamInfo[nInd].StreamName, 
             GC_STREAM_NAME_STR_LENGTH );
      
    nRetVal = SendStatusInfo( (unsigned char *) & stGCStreamConfig, sizeof( stGCStreamConfig ), 
                              (unsigned char *) & stAck, sizeof( stAck ) );
    if( nRetVal > 0 )
    {
      //Check whether the response is an acknowlege message and it has the same message ID.
      if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
          GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_STREAM_INIT_MSG )
      {
        //Send back the acknowledge code to Application.
        eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
      }
      else
      {
        //TBD - Confirm what need to be done here, for the time being break here.
        nRetVal = ERR_PROTOCOL;
        break;
      }
    }
    else
    {
      //TBD - Confirm what need to be done here, for the time being break here.
      nRetVal = ERR_CMD_FAILED;
      break;
    }
  }
    
  return nRetVal;     
}


int CFFBStatus::SendPVConfig( GC_PV_CONFIG_STRUCT * pPVConfig, 
                              FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_PV_INIT_STRUCT stGCPVConfig; 
  //Set message ID.
  stGCPVConfig.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stGCPVConfig.stHeaderInfo.CmdInfo.byCmd, GC_PV_INIT_MSG );
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = ERR_SUCCESS;
  //Send one PV config information at a time.
  for( int nInd = 0; nInd < NUM_PV_CONFIG_INFO; nInd++ )
  {
    //Set message sequence number.
    stGCPVConfig.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;

    //Pack data.
    stGCPVConfig.stPVInfo.byPVNum = pPVConfig->stPVInfo[nInd].byPVNum;
    strncpy( (char *) stGCPVConfig.stPVInfo.PVDescription, (const char *) pPVConfig->stPVInfo[nInd].PVDescription, GC_PV_DESC_STR_LENGTH );
    
    nRetVal = SendStatusInfo( (unsigned char *) & stGCPVConfig, sizeof( stGCPVConfig ), 
                              (unsigned char *) & stAck, sizeof( stAck ) );
    if( nRetVal > 0 )
    {
      //Check whether the response is an acknowlege message and it has the same message ID.
      if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
          GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_PV_INIT_MSG )
      {
        //Send back the acknowledge code to Application.
        eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
      }
      else
      {
        //TBD - Confirm what need to be done here, for the time being break here.
        nRetVal = ERR_PROTOCOL;
        break;
      }
    }
    else
    {
      //TBD - Confirm what need to be done here, for the time being break here.
      nRetVal = ERR_CMD_FAILED;
      break;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendDetConfig( GC_DET_CONFIG_STRUCT * pDetConfig, 
                               FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_DET_INIT_STRUCT stGCDetConfig;
  //Set message ID.   
  stGCDetConfig.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stGCDetConfig.stHeaderInfo.CmdInfo.byCmd, GC_DET_INIT_MSG );
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = ERR_SUCCESS;
  //Send one detector config information at a time.
  for( int nInd = 0; nInd < NUM_DET_CONFIG_INFO; nInd++ )
  {
    //Set message sequence number.
    stGCDetConfig.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;

    //Pack data.
    stGCDetConfig.stDetInfo.byDetectorNum = pDetConfig->stDetInfo[nInd].byDetectorNum;
    stGCDetConfig.stDetInfo.byDetectorType = pDetConfig->stDetInfo[nInd].byDetectorType;
        
    nRetVal = SendStatusInfo( (unsigned char *) & stGCDetConfig, sizeof( stGCDetConfig ), 
                              (unsigned char *) & stAck, sizeof( stAck ) );
    if( nRetVal > 0 )
    {
      //Check whether the response is an acknowlege message and it has the same message ID.
      if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
          GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_DET_INIT_MSG )
      {
        //Send back the acknowledge code to Application.
        eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
      }
      else
      {
        //TBD - Confirm what need to be done here, for the time being break here.
        nRetVal = ERR_PROTOCOL;
        break;
      }
    }
    else
    {
      //TBD - Confirm what need to be done here, for the time being break here.
      nRetVal = ERR_CMD_FAILED;
      break;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendDeviceMode( DEV_MODE eDeviceMode,
                                FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_DEVICE_MODE_STRUCT stDeviceMode;
  //Set message sequence number.
  stDeviceMode.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stDeviceMode.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stDeviceMode.stHeaderInfo.CmdInfo.byCmd, GC_DEV_MODE_MSG );

  //Pack data.
  stDeviceMode.byDeviceMode = (unsigned char) eDeviceMode;
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stDeviceMode, sizeof( stDeviceMode ), 
                                (unsigned char *) & stAck, sizeof(stAck) );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_DEV_MODE_MSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendGCOperationChange(  unsigned char byOperationNum,
                                        OPERATION_MODE eCurrOperationMode,
                                        unsigned char byStreamNum,
                                        unsigned char byNextStreamNum,
                                        unsigned short usRunTime,
                                        unsigned short usAnalysisTime,
                                        unsigned short usCycleTime,
                                        FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_OPERATION_CHANGE_STRUCT stOperationChange;
  //Set message sequence number.
  stOperationChange.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stOperationChange.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stOperationChange.stHeaderInfo.CmdInfo.byCmd, GC_OPER_CHG_MSG );

  //Pack data.
  stOperationChange.byOperationNum = byOperationNum;
  stOperationChange.byCurrOperationMode = (unsigned char) eCurrOperationMode;
  stOperationChange.byStreamNum = byStreamNum;
  stOperationChange.byNextStreamNum = byNextStreamNum;
    
  FixEndian( usRunTime );
  stOperationChange.usRunTime = usRunTime;
    
  FixEndian( usAnalysisTime );
  stOperationChange.usAnalysisTime = usAnalysisTime;
    
  FixEndian( usCycleTime );
  stOperationChange.usCycleTime = usCycleTime;
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stOperationChange, sizeof( stOperationChange ), 
                                (unsigned char *) & stAck, sizeof(stAck) );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_OPER_CHG_MSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendGCStatus(   unsigned short usRunTime_CC1,
                                unsigned short usRunTime_CC2,
                                unsigned long long ullErrInfo,
                                unsigned long ulDetErrInfo,
                                CONFIG_ACTION & eConfigAction,
                                FFB_ACK_CODE & eRxAckCode )
{
  FFB_GC_STATUS_STRUCT stGCStatus;
  //Set message sequence number.
  stGCStatus.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stGCStatus.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stGCStatus.stHeaderInfo.CmdInfo.byCmd, GC_STATUS_MSG );
    
  //Pack data.
  FixEndian( usRunTime_CC1 );
  stGCStatus.usRunTimeOpr1 = usRunTime_CC1;
    
  FixEndian( usRunTime_CC2 );
  stGCStatus.usRunTimeOpr2 = usRunTime_CC2;
    
  FixEndian( ullErrInfo );
  stGCStatus.ullErrInfo = ullErrInfo;

  FixEndian( ulDetErrInfo );
  stGCStatus.ulDetErrInfo = ulDetErrInfo;
    
  FFB_GC_STATUS_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stGCStatus, sizeof( stGCStatus ), 
                                (unsigned char *) & stAck, sizeof(stAck) );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stAckInfo.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAck.stAckInfo.stHeaderInfo.CmdInfo.byCmd ) == GC_STATUS_MSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.stAckInfo.AckInfo.byAck );
      eConfigAction = (CONFIG_ACTION) stAck.byConfigAction;
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
    
  return nRetVal;
}


int CFFBStatus::SendPVUpdate( PV_DATA_UPDATE_STRUCT * pPVUpdate, 
                              FFB_ACK_CODE & eRxAckCode,
                              unsigned int unTimeOut )
{
  FFB_PV_UPDATE_STRUCT stPVUpdate;
  //Set message sequence number.
  stPVUpdate.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stPVUpdate.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stPVUpdate.stHeaderInfo.CmdInfo.byCmd, GC_PV_UPDATE_MSG );
    
  //Pack data.
  for( int nInd = 0; nInd < NUM_PV_DATA_IN_PV_UPDATE; nInd++ )
  {
    stPVUpdate.stPVData[nInd].byPVStatus = pPVUpdate->stPVData[nInd].byPVStatus;
    stPVUpdate.stPVData[nInd].fValue = pPVUpdate->stPVData[nInd].fValue;
    FixEndian( stPVUpdate.stPVData[nInd].fValue );
  }
    
  FFB_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stPVUpdate,
                                sizeof( stPVUpdate ), 
                                (unsigned char *) & stAck, 
                                sizeof(stAck),
                                unTimeOut );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAck.stHeaderInfo.CmdInfo.byCmd ) == GC_PV_UPDATE_MSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.AckInfo.byAck );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
    
  return nRetVal;
}

int CFFBStatus::RetrieveFFBTime( unsigned long long & ullTime,/*64Bit Time in 1/32msec units.*/
                                 FFB_ACK_CODE & eRxAckCode )
{
  FFB_TIME_REQ_STRUCT stReqTime;
  //Set message sequence number.
  stReqTime.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stReqTime.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stReqTime.stHeaderInfo.CmdInfo.byCmd, GC2FFB_TIME_REQUEST_MSSG );

  FFB_TIME_ACK_STRUCT stAck;
  int nRetVal = SendStatusInfo( (unsigned char *) & stReqTime,
                                sizeof( stReqTime ),
                                (unsigned char *) & stAck,
                                sizeof( stAck ) );
  if( nRetVal > 0 )
  {
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAck.stAckInfo.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAck.stAckInfo.stHeaderInfo.CmdInfo.byCmd ) == GC2FFB_TIME_REQUEST_MSSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAck.stAckInfo.AckInfo.byAck );
      //Get FFB board time.
      ullTime = stAck.ullTime;

      FixEndian( ullTime );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }

  return nRetVal;
}

int CFFBStatus::QueryFFBCapabilityInfo( std::vector< FFB_DEV_CAP_DETAIL > & v_stDevCapDetails,
                                        FIRMWARE_INFO & stFirmInfo,
                                        FFB_ACK_CODE & eRxAckCode,
                                        unsigned int unTimeOut )
{
  FFB_BD_CAP_REQ_STRUCT stBDCaps;
  //Set message sequence number.
  stBDCaps.stHeaderInfo.byMssgSeqNum = ++m_bySeqNum;
  //Set message ID.
  stBDCaps.stHeaderInfo.CmdInfo.byCmd = 0;
  SetMssgID_FFB( & stBDCaps.stHeaderInfo.CmdInfo.byCmd, FFB_BD_CAP_MSSG );

  unsigned char ucAck[BUFF_SZ_4_FFB_CAP_RESP] = {0};
  int nRetVal = SendStatusInfo( (unsigned char *) & stBDCaps,
                                sizeof( stBDCaps ),
                                ucAck,
                                BUFF_SZ_4_FFB_CAP_RESP,
                                unTimeOut );
  if( nRetVal > (int) sizeof(FFB_ACK_STRUCT) )
  {
    FFB_ACK_STRUCT stAckInfo;
    memcpy( (void*) & stAckInfo, ucAck, sizeof( stAckInfo ) );
    //Check whether the response is an acknowlege message and it has the same message ID.
    if( GetAckBit_FFB( stAckInfo.stHeaderInfo.CmdInfo.byCmd ) &&
        GetMssgID_FFB( stAckInfo.stHeaderInfo.CmdInfo.byCmd ) == FFB_BD_CAP_MSSG )
    {
      //Send back the acknowledge code to Application.
      eRxAckCode = (FFB_ACK_CODE) GetAckCode_FFB( stAckInfo.AckInfo.byAck );

      //Parse and Save the FFB Device Function informations.
      nRetVal = ParseNSaveFFBCapInfo( (ucAck + sizeof( stAckInfo )), 
                                      nRetVal - sizeof( stAckInfo ),
                                      v_stDevCapDetails,
                                      stFirmInfo );
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }

  return nRetVal;
}


int CFFBStatus::ParseNSaveFFBCapInfo( unsigned char * byDiscInfo, 
                                      int nNumBytes, 
                                      std::vector< FFB_DEV_CAP_DETAIL > & v_stDevCapDetails, 
                                      FIRMWARE_INFO & stFirmInfo )
{
  int nRetVal = ERR_SUCCESS;

  unsigned char ucNumFuncTypes = 0;
  int nNumFirmInfoBytes = 0;
  if( nNumBytes > 0 && byDiscInfo )
  {
    ucNumFuncTypes = byDiscInfo[0];
    //Check total bytes count.
    nNumFirmInfoBytes = (nNumBytes-1) - ( sizeof(DevFnCapUnion) * ucNumFuncTypes );
    if( nNumFirmInfoBytes < 0 )
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
  else
  {
    nRetVal = ERR_PROTOCOL;
  }

  if( ERR_SUCCESS == nRetVal )
  {
    v_stDevCapDetails.clear();

    DevFnCapUnion stCapInfo;
    int nInfoInd = 1;

    for( unsigned char ucFunInd = 1; (nInfoInd < nNumBytes) && (ucFunInd <= ucNumFuncTypes); ucFunInd++ )
    {
      memcpy( & stCapInfo, & byDiscInfo[nInfoInd], sizeof(DevFnCapUnion) );

      FixEndian( stCapInfo.usDevCap );
      int FnType = GetDiscFnType( & stCapInfo.usDevCap );
      int FnCnt = GetDiscFnCount( & stCapInfo.usDevCap );
      int RevNo = GetDiscRevision( & stCapInfo.usDevCap );

      char szSlotName[BUFF_SZ_4_SLOT_NAME];
      std::string strDevName;

      CResolveDevName oResolveName;

      for( int nCnt = 1; nCnt <= FnCnt; nCnt++ )
      {
        if( (nRetVal = oResolveName.GenerateName(szSlotName, FnType, GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID, nCnt)) < 0 )
        {
          DEBUG1( "CFFBStatus::ParseNSaveFFBCapInfo() - Discarding unknown Device Info !!!" );
        }
        else
        {
          //Store device capability information.
          FFB_DEV_CAP_DETAIL stDevCap;
          stDevCap.strDeviceName = szSlotName;
          stDevCap.nIOFunction = FnType;
          stDevCap.nSlotNum = GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID;
          stDevCap.nRevNum = RevNo;
          stDevCap.strDevInfo = stCapInfo.stDevCapInfo.szDevName;

          v_stDevCapDetails.push_back( stDevCap );
        }
      }

      nInfoInd += sizeof(DevFnCapUnion);
    }

    if( nNumBytes > nInfoInd && (sizeof(FIRMWARE_INFO) == nNumFirmInfoBytes ) )
    {
      //Firmware Version/Date information is present.
      //Get the information and append to "current_software_info.txt" file.
      memcpy( stFirmInfo.szFirmwareVersion, & byDiscInfo[nInfoInd], FIRMWARE_VERSION_STR_SZ );
      nInfoInd += FIRMWARE_VERSION_STR_SZ;
      memcpy( stFirmInfo.szFirmwareDate, & byDiscInfo[nInfoInd], FIRMWARE_DATE_STR_SZ );
    }
  }

  return nRetVal;
}


CFFBCmmd::CFFBCmmd()
{
}

CFFBCmmd::~CFFBCmmd()
{
}

int CFFBCmmd::OpenHal( char* pszDevName )
{
  if( NULL == pszDevName )
  {
    return ERR_INVALID_ARGS;
  }
    
  return CBaseDev::OpenHal( pszDevName, TRUE );
}

int CFFBCmmd::CloseHal()
{
  return CBaseDev::CloseHal();
}

int CFFBCmmd::GetStreamData( unsigned char * ucData, unsigned int unLength, bool bBlock, unsigned int unTimeOut )
{
  int nRetVal = ERR_SUCCESS;
    
  if( m_bIsDevOpen )
  {
    switch( m_eCommType )
    {
    case CAN_COMM:
      if( m_pobCAN )
      {
        if( bBlock )
        {
          nRetVal = m_pobCAN->CANRxStrmBlocking( (unsigned char *) ucData,  // Response from remote board
                                                 unLength );                // Size of expected response
        }
        else
        { 
          nRetVal = m_pobCAN->CANRxStrmTimeout( (unsigned char *) ucData,  // Response from remote board
                                                unLength,                  // Size of expected response
                                                unTimeOut );
        }
        
        //Not the expected packet size
        if (nRetVal < 0)
        {
          DEBUG1("CFFBCmmd::GetStreamData() - CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG1("CFFBCmmd::GetStreamData() - Unexpected invalid pointer!");
      }
        
      break;
    case SPI_COMM:
      //TODO : To be implemented.
    case SERIAL_COMM:
      //TODO : To be implemented.
    case COMM_NONE:                     
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG1( "CFFBCmmd::GetStreamData() - Invalid switch case!" );
      break;
    }
  }
  else
  {
    //Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG1("CFFBCmmd::GetStreamData() - Function called before Open Call!");
  }
    
  return nRetVal;
}

int CFFBCmmd::SendResponse( unsigned char * ucData, unsigned int unLength )
{
  int nRetVal = ERR_SUCCESS;
    
  if( m_bIsDevOpen )
  {
    switch( m_eCommType )
    {
    case CAN_COMM:
      if( m_pobCAN )
      {
        nRetVal = m_pobCAN->CANTxCmd( (unsigned char *) ucData,  // Response from remote board
                                      unLength);               // Size of expected response
    
        //Not the expected packet size
        if (nRetVal < 0)
        {
          DEBUG1("CFFBCmmd::SendResponse() - CCANComm::CANTxCmd failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG1("CFFBCmmd::GetSendResponse() - Unexpected invalid pointer!");
      }
    
      break;
    case SPI_COMM:
      //TODO : To be implemented.
    case SERIAL_COMM:
      //TODO : To be implemented.
    case COMM_NONE:                     
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG1( "CFFBCmmd::GetSendResponse() - Invalid switch case!" );
      break;
    }
  }
  else
  {
    //Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG1("CFFBCmmd::SendResponse() - Function called before Open Call!");
  }
    
  return nRetVal;
}


int CFFBCmmd::GetRemoteRequest( REMOTE_REQUEST_STRUCT & stRemoteReq, 
                                REMOTE_REQUEST_TYPE & eRemReqType, 
                                unsigned char & byMssgSeqNum )
{
  return GetRemoteRequest( stRemoteReq, eRemReqType, byMssgSeqNum, true, 0 );
}

int CFFBCmmd::GetRemoteRequest( REMOTE_REQUEST_STRUCT & stRemoteReq, 
                                REMOTE_REQUEST_TYPE & eRemReqType, 
                                unsigned char & byMssgSeqNum, 
                                unsigned int unTimeOut )
{
  return GetRemoteRequest( stRemoteReq, eRemReqType, byMssgSeqNum, false, unTimeOut );
}

int CFFBCmmd::GetRemoteRequest( REMOTE_REQUEST_STRUCT & stRemoteReq, 
                                REMOTE_REQUEST_TYPE & eRemReqType, 
                                unsigned char & byMssgSeqNum,
                                bool bBlock,
                                unsigned int unTimeOut )
{
  unsigned char ucRxData[ MAX_FFB_REMOTE_REQ_SIZE ] = {'\0'};
  eRemReqType = REQ_TYPE_UNKNOWN;

  int nRetVal = GetStreamData( ucRxData, MAX_FFB_REMOTE_REQ_SIZE, bBlock, unTimeOut );

  if( nRetVal > 0 && nRetVal > (int) sizeof(FFB_HEADER_INFO_STRUCT) )
  {
    FFB_HEADER_INFO_STRUCT * pHeaderInfo = (FFB_HEADER_INFO_STRUCT *) ucRxData;

    if( ! GetAckBit_FFB( pHeaderInfo->CmdInfo.byCmd ) )
    {
      byMssgSeqNum = pHeaderInfo->byMssgSeqNum;
    
      switch( GetMssgID_FFB( pHeaderInfo->CmdInfo.byCmd ) )
      {
      case FFB2GC_NETW_CONFIG_MSG:
        if( sizeof( FFB_GC_NETWORK_CONFIG_STRUCT ) == nRetVal )
        {
          eRemReqType = GC_NETWORK_CONFIG;
          FFB_GC_NETWORK_CONFIG_STRUCT * pNtwConfig = (FFB_GC_NETWORK_CONFIG_STRUCT *) ucRxData;
          strncpy( (char *) stRemoteReq.stNtwInfo.IPAddress1, 
                   (const char *) pNtwConfig->stNtwInfo.IPAddress1, 
                   GC_IP_ADDR_STR_LENGTH );
          strncpy( (char *) stRemoteReq.stNtwInfo.NetMask1, 
                   (const char *) pNtwConfig->stNtwInfo.NetMask1, 
                   GC_IP_ADDR_STR_LENGTH );
          strncpy( (char *) stRemoteReq.stNtwInfo.IPAddress2, 
                   ( const char *) pNtwConfig->stNtwInfo.IPAddress2, 
                   GC_IP_ADDR_STR_LENGTH );
          strncpy( (char *) stRemoteReq.stNtwInfo.NetMask2, 
                   (const char *) pNtwConfig->stNtwInfo.NetMask2, 
                   GC_IP_ADDR_STR_LENGTH );
          strncpy( (char *) stRemoteReq.stNtwInfo.DefaultGatewayIPAddr, 
                   (const char *) pNtwConfig->stNtwInfo.DefaultGatewayIPAddr, 
                   GC_IP_ADDR_STR_LENGTH );
        }
        else
        {
          nRetVal = ERR_PROTOCOL;
        }
        break;
      case FFB_CTRL_ACTION_MSG:
        if( sizeof( FFB_CTRL_ACTION_STRUCT ) == nRetVal )
        {
          eRemReqType = GC_CTRL_ACTION;
          FFB_CTRL_ACTION_STRUCT * pCtrlAction = (FFB_CTRL_ACTION_STRUCT *) ucRxData;
          stRemoteReq.stCtrlActInfo = pCtrlAction->stCtrlInfo;
        }
        else
        {
          nRetVal = ERR_PROTOCOL;
        }
        break;
      default:
        nRetVal = ERR_PROTOCOL;
        break;
      }
    }
    else
    {
      nRetVal = ERR_PROTOCOL;
    }
  }
  else
  {
    nRetVal = ERR_PROTOCOL;
  }
    
  return nRetVal;
}

int CFFBCmmd::SendRemoteRequestResponse( REMOTE_REQUEST_TYPE eRemReqType,
                                         FFB_ACK_CODE eAckCode,
                                         unsigned char byAckRetransmit,
                                         unsigned char byMssgSeqNum )
{
  int nRetVal = ERR_SUCCESS;
  FFB_ACK_STRUCT stAck;
  stAck.stHeaderInfo.byMssgSeqNum = byMssgSeqNum;
  SetAckBit_FFB( & stAck.stHeaderInfo.CmdInfo.byCmd, 1 );
    
  switch( eRemReqType )
  {
  case GC_NETWORK_CONFIG:
    SetMssgID_FFB( & stAck.stHeaderInfo.CmdInfo.byCmd, FFB2GC_NETW_CONFIG_MSG );       
    break;
  case GC_CTRL_ACTION:
    SetMssgID_FFB( & stAck.stHeaderInfo.CmdInfo.byCmd, FFB_CTRL_ACTION_MSG );
    break;
  default:
    nRetVal = ERR_INVALID_ARGS;
    break;
  }
    
  if( ERR_SUCCESS == nRetVal )
  {
    byAckRetransmit &= 0x1;
    SetRT_FFB( & stAck.AckInfo.byAck, byAckRetransmit );
    SetAckCode_FFB( & stAck.AckInfo.byAck, eAckCode );
    nRetVal = SendResponse( (unsigned char *) & stAck, sizeof( stAck ) );
  }
    
  return nRetVal;
}



//Return Streaming Receive Pipe File Descriptor
int CFFBCmmd::GetRxStreamingFd()
{
  int nRetVal = ERR_SUCCESS;
  if( m_pobCAN )
  {
    nRetVal = m_pobCAN->CANGetRxStrmFd();
  }
  else
  {
    nRetVal = ERR_INTERNAL_ERR;
    DEBUG1("CFFBCmmd::GetRxStreamingFd(): Unexpected invalid pointer!");
  }
  return nRetVal;
}


