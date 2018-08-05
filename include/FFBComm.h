/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FFBComm.h
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
 *  $Id: FFBComm.h,v 1.13 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 *  $Log: FFBComm.h,v $
 *  Revision 1.13  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.12  2014/06/26 10:01:12  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.11  2012/03/21 06:49:04  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.10  2010/09/24 18:53:04  emrsn\rsaha
 *  Restoring NUM_STREAM_CONFIG_INFO macro use.
 *
 *  Revision 1.9  2010/09/21 21:43:35  emrsn\LRatner
 *  Restored definition of NUM_STREAM_CONFIG_INFO
 *
 *  Revision 1.8  2010/09/17 20:44:04  emrsn\LRatner
 *  Getting MAX_STREAM count from global.h
 *
 *  Revision 1.7  2009/07/18 09:01:11  epm-na\rsaha
 *  Enabled code to count number of retries made in Reliability layer.
 *
 *  Revision 1.6  2009/05/13 22:51:44  epm-na\rsaha
 *  Revised with API to query FFB Device capability information. This helps in detecting FFB board presense incase FFB Board boots up late to GC.
 *
 *  Revision 1.5  2009/04/06 21:22:13  epm-na\rsaha
 *  Revised HAL layer with interface to retrieve 64bit FFB server time.
 *
 *  Revision 1.4  2009/04/02 20:45:57  epm-na\rsaha
 *  Added method to retrieve streaming FD  for CFFBCmmd object.
 *
 *  Revision 1.3  2009/01/26 22:24:43  epm-na\rsaha
 *  Revised to send Detector error status information.
 *
 *  Revision 1.2  2008/11/10 23:50:08  epm-na\rsaha
 *  Updated HAL layer with interfaces to help an application communicate with the FFB board.
 *
 *  Revision 1.1  2008/10/22 22:31:18  epm-na\rsaha
 *  Initial version with minimum required API's to send/receive large data to/from FFB board. Once the protocol structure is defined for FFB communication with GC these files will be updated with the required APIs.
 *
 *************************************************************************/


#include "BaseDev.h"
#include "FFBCommProtocol.h"

#define NUM_STREAM_CONFIG_INFO          24 // number of entries in the FFB struct.  NOT number of streams.
#define NUM_PV_CONFIG_INFO    64
#define NUM_DET_CONFIG_INFO   4

#define MAX_FFB_REMOTE_REQ_SIZE sizeof(FFB_GC_NETWORK_CONFIG_STRUCT)

//Holds GC Streams configuration details.
typedef struct {
  STREAM_INFO stStreamInfo[NUM_STREAM_CONFIG_INFO];
} __attribute (( packed )) GC_STREAM_CONFIG_STRUCT;

//Holds GC PVs configuration details.
typedef struct {
  PV_INFO stPVInfo[NUM_PV_CONFIG_INFO];
} __attribute (( packed )) GC_PV_CONFIG_STRUCT;

//Holds GC Detectors configuration details.
typedef struct {
  DET_INFO stDetInfo[NUM_DET_CONFIG_INFO];
} __attribute (( packed )) GC_DET_CONFIG_STRUCT;

//Holds GC PVs update information ( value , status ).
typedef struct{
  PV_VALUE stPVData[NUM_PV_DATA_IN_PV_UPDATE];
} __attribute (( packed )) PV_DATA_UPDATE_STRUCT;

//Holds Device Capability details.
typedef struct {
  std::string strDeviceName;
  DB_INT32 nIOFunction;
  DB_INT32 nSlotNum;
  DB_INT32 nRevNum;
  std::string strDevInfo;
} FFB_DEV_CAP_DETAIL;


class CFFBStatus : public CBaseDev
{
  unsigned char m_bySeqNum;

  //Send status information to FFB.
  int SendStatusInfo( unsigned char * ucTXData,
                      unsigned int unTXNumData,
                      unsigned char * ucRXData, 
                      unsigned int unRXNumData,
                      unsigned int unTimeOut = HAL_DFLT_TIMEOUT );
    
  int ParseNSaveFFBCapInfo( unsigned char * byDiscInfo,
                            int nNumBytes,
                            std::vector< FFB_DEV_CAP_DETAIL > & v_stDevCapDetails,
                            FIRMWARE_INFO & stFirmInfo );


public:

  CFFBStatus();
  ~CFFBStatus();
    
  int OpenHal( char * pszDevName );
  int CloseHal();
    
  int GetRetryAttempts();
    
  int SendGCConfig( const char * szGCFirmwareRev,
                    const char * szGCSerialNumber,
                    BRAND_SETTING eBrandSetting,
                    FFB_ACK_CODE & eRxAckCode );
    
  int SendGCUserTag( const char * szGCUserTag, 
                     FFB_ACK_CODE & eRxAckCode );
    
  int SendNWConfig( const char * szIPAddr1, 
                    const char * szNetMask1,
                    const char * szIPAddr2, 
                    const char * szNetMask2,
                    const char * szDefaultGateway, 
                    FFB_ACK_CODE & eRxAckCode );
    
  int SendStreamConfig( GC_STREAM_CONFIG_STRUCT * pStreamConfig,
                        FFB_ACK_CODE & eRxAckCode );
    
  int SendPVConfig( GC_PV_CONFIG_STRUCT * pPVConfig,
                    FFB_ACK_CODE & eRxAckCode );
    
  int SendDetConfig( GC_DET_CONFIG_STRUCT * pDetConfig,
                     FFB_ACK_CODE & eRxAckCode );
    
  int SendDeviceMode( DEV_MODE eDeviceMode, 
                      FFB_ACK_CODE & eRxAckCode );
    
  int SendGCOperationChange(  unsigned char byOperationNum,
                              OPERATION_MODE eCurrOperationMode,
                              unsigned char byStreamNum,
                              unsigned char byNextStreamNum,
                              unsigned short usRunTime,
                              unsigned short usAnalysisTime,
                              unsigned short usCycleTime,
                              FFB_ACK_CODE & eRxAckCode );
    
  int SendGCStatus( unsigned short usRunTime_CC1,
                    unsigned short usRunTime_CC2,
                    unsigned long long ullErrInfo,
                    unsigned long ulDetErrInfo,
                    CONFIG_ACTION & eConfigAction,
                    FFB_ACK_CODE & eRxAckCode );
    
  int SendPVUpdate( PV_DATA_UPDATE_STRUCT * pPVData,
                    FFB_ACK_CODE & eRxAckCode, 
                    unsigned int unTimeOut  = HAL_DFLT_TIMEOUT );

  int RetrieveFFBTime( unsigned long long & ullTime, /*64Bit Time in 1/32msec units.*/
                       FFB_ACK_CODE & eRxAckCode );

  int QueryFFBCapabilityInfo( std::vector< FFB_DEV_CAP_DETAIL > & v_stDevCapDetails,
                              FIRMWARE_INFO & stFirmInfo,
                              FFB_ACK_CODE & eRxAckCode,
                              unsigned int unTimeOut = HAL_DFLT_TIMEOUT );
};




//Holds remote requests received from FFB device.
typedef union {
  NETWORK_CONFIG_STRUCT stNtwInfo;
  CTRL_ACTION_STRUCT stCtrlActInfo;
} __attribute (( packed )) REMOTE_REQUEST_STRUCT;

//Holds the type of remote request received.
typedef enum {
  GC_NETWORK_CONFIG = 0,
  GC_CTRL_ACTION,
  REQ_TYPE_UNKNOWN
} REMOTE_REQUEST_TYPE;



class CFFBCmmd : public CBaseDev
{
  int GetRemoteRequest(   REMOTE_REQUEST_STRUCT & stRemoteReq, 
                          REMOTE_REQUEST_TYPE & eRemReqType,
                          unsigned char & byMssgSeqNum,
                          bool bBlock,
                          unsigned int unTimeOut );

  //Receives the data that FFB board sends via stream pipe.
  int GetStreamData( unsigned char * ucData, unsigned int unLength,
                     bool bBlock, unsigned int unTimeOut );
    
  //Send response to GC.
  int SendResponse( unsigned char * ucData, unsigned int unLength );
    
public:

  CFFBCmmd();
  ~CFFBCmmd();
    
  int OpenHal( char * pszDevName );
  int CloseHal();

  int GetRxStreamingFd();
    
  int GetRemoteRequest(   REMOTE_REQUEST_STRUCT & stRemoteReq, 
                          REMOTE_REQUEST_TYPE & eRemReqType,
                          unsigned char & byMssgSeqNum );

  int GetRemoteRequest(   REMOTE_REQUEST_STRUCT & stRemoteReq, 
                          REMOTE_REQUEST_TYPE & eRemReqType,
                          unsigned char & byMssgSeqNum,
                          unsigned int unTimeOut );
    
  int SendRemoteRequestResponse(  REMOTE_REQUEST_TYPE eRemReqType,
                                  FFB_ACK_CODE eAckCode,
                                  unsigned char byRepeatTxRecv,
                                  unsigned char byMssgSeqNum );
};
