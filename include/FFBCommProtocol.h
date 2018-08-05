/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FFBCommProtocol.h
 *
 *  Description: Foundation Field Bus board - GC communication protocol definitions.
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
 *  $Id: FFBCommProtocol.h,v 1.14 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 *  $Log: FFBCommProtocol.h,v $
 *  Revision 1.14  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.13  2014/06/26 10:01:12  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.12  2012/03/21 06:49:05  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.11  2009/12/03 13:31:35  emrsn\rsaha
 *  Revised "AirPurgeFail" platweb alert to "LossOfPurge".
 *
 *  Revision 1.10  2009/11/25 10:01:28  emrsn\rsaha
 *  Added new PlantWeb alert "Air Purge Failed".
 *
 *  Revision 1.9  2009/08/04 10:31:28  epm-na\rsaha
 *  Added bits for "NoSampleFlow" 1 and 2 plant web alerts.
 *
 *  Revision 1.8  2009/06/25 20:13:38  epm-na\rsaha
 *  Revised GC Firmware revision string length from 15 to 32 charecter and added 4 new plant web alerts for low carrier pressure, to be used in future.
 *
 *  Revision 1.7  2009/05/13 22:51:44  epm-na\rsaha
 *  Revised with API to query FFB Device capability information. This helps in detecting FFB board presense incase FFB Board boots up late to GC.
 *
 *  Revision 1.6  2009/04/02 20:43:09  epm-na\rsaha
 *  Added enum definitions, firmware version info structure as part of device capability message and corrected few typo errors.
 *
 *  Revision 1.5  2009/01/26 21:03:47  epm-na\rsaha
 *  Introduced another field for Detector error status in GC Status message hence increasing size by 4 bytes.
 *
 *  Revision 1.4  2009/01/08 01:02:05  epm-na\rsaha
 *  Updated with Bit-Field error definition for plant web alarts.
 *
 *  Revision 1.3  2008/11/10 23:47:45  epm-na\rsaha
 *  Updated FFB header and acknowledge protocol structures to help in resolving Big/Little endian issues.
 *
 *  Revision 1.2  2008/11/04 19:38:13  epm-na\rsaha
 *  Added structure definitions for FFB-GC communication protocol.
 *
 *************************************************************************/


#ifndef _FFBCOMM_PROTOCOL_H
#define _FFBCOMM_PROTOCOL_H 

#include "DevProtocol.h"

#define FFB_REPLY_CAN_ID_OFFSET GC700XP_HOST_BASE_CAN_MSG_ID

#define GC_FIRMWARE_REV_STR_LENGTH    32
#define GC_SERIAL_NUM_STR_LENGTH      15
#define GC_USER_TAG_STR_LENGTH        30
#define GC_IP_ADDR_STR_LENGTH         15
#define GC_STREAM_NAME_STR_LENGTH     20
#define GC_PV_DESC_STR_LENGTH         32
#define NUM_PV_DATA_IN_PV_UPDATE      64

#define NUM_SUPP_FUNCTION             2
#define FIRMWARE_VERSION_STR_SZ       11      //xxx.xxx.xxx
#define FIRMWARE_DATE_STR_SZ          10      //YYYY/MM/DD

typedef struct {
  unsigned char   szFirmwareVersion[FIRMWARE_VERSION_STR_SZ]; //"xxx.xxx.xxx" - Every field can have 3 digits MAX. No null termination.
  unsigned char   szFirmwareDate[FIRMWARE_DATE_STR_SZ];       //"YYYY/MM/DD" - No null termination.
} __attribute (( packed )) FIRMWARE_INFO;

// Board Capability/Identity Structure
typedef struct {  
  unsigned char       byNumFns;                         // Number of supported functions
  DEV_FN_CAP_STRUCT FnSupport[NUM_SUPP_FUNCTION];       // Function Type + Number of Enumerations
  FIRMWARE_INFO       FirmwareInfo;
} __attribute (( packed )) BD_CAP_STRUCT;


//Message IDs
typedef enum {
  GC_CONFIG_MSG        = 0x01,       //GC Configuration (Basic). (GC -> FFB)
  GC_USER_TAG_MSG,                   //GC User Tag.(GC -> FFB)
  FFB2GC_NETW_CONFIG_MSG,            //Network Configuration. (FFB -> GC)
  GC2FFB_NETW_CONFIG_MSG,            //Network Configuration. (GC -> FFB)
  GC_STREAM_INIT_MSG,                //GC's Stream Configuration/Initialization details.(GC -> FFB)
  GC_PV_INIT_MSG,                    //GC's PV Configuration details.(GC -> FFB)
  GC_DET_INIT_MSG,                   //GC's Detector Configuration details.(GC -> FFB)
  GC_DEV_MODE_MSG,                   //GC's device mode.(GC -> FFB)
  FFB_CTRL_ACTION_MSG  = 0x10,       //Control action request. (FFB -> GC)
  GC_STATUS_MSG        = 0x15,       //Regular GC Status information. (GC-> FFB)
  GC_OPER_CHG_MSG      = 0x20,       //Regular GC Operation change information.(GC -> FFB)
  GC_PV_UPDATE_MSG,                  //Regular GC PV update.(GC -> FFB)
  GC2FFB_TIME_REQUEST_MSSG,          //Request 64bit FFB time. (GC -> FFB)
  FFB_BD_CAP_MSSG                    //Request FFB Board Capability Information. (GC->FFB)
}FFB_MSG_ID;

//Acknowledge message codes.
typedef enum {
  ACK_FFB_NO_ERR = 0,             //No error
  NACK_UNEXPECTED_MSG,            //Unexpected Message.
  NACK_PARAMETER_ERR,             //Parameter Error.
  NACK_CONFIG_REQ_REJECTED,       //Configuration Request Rejected.
  NACK_CONTROL_ACTION_REJECTED,   //Control Action Rejected.
  NACK_WRONG_MODE_REQUEST         //Wrong mode for Request.
}FFB_ACK_CODE;

typedef struct {
  unsigned char AckMssg : 1;  //Flag indicates message is an acknowledge.
  unsigned char MssgID  : 7; 
} __attribute ((packed)) FFB_CMD_INFO_STRUCT;

typedef union {
  unsigned char byCmd;
  FFB_CMD_INFO_STRUCT stCmd;
} __attribute ((packed)) FFB_CMD_INFO_UNION;

//Outgoing/Incoming [FFB <-> GC] message format.
typedef struct {
  unsigned char byMssgSeqNum;
  FFB_CMD_INFO_UNION CmdInfo;
} __attribute (( packed )) FFB_HEADER_INFO_STRUCT;

typedef struct {
  unsigned char AckRetransmit : 1; //Flag indicates repeated message reception.
  unsigned char AckCode       : 7;
} __attribute (( packed )) FFB_CMD_ACK_INFO_STRUCT;

typedef union {
  unsigned char byAck;
  FFB_CMD_ACK_INFO_STRUCT stAck;
} __attribute (( packed )) FFB_CMD_ACK_INFO_UNION;

//Acknowledge [GC <-> FFB] message format.
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  FFB_CMD_ACK_INFO_UNION AckInfo;
} __attribute (( packed )) FFB_ACK_STRUCT;

//Branding settings
typedef enum {
  ROSEMOUNT_ANALYTICAL = 0,
  DANIEL
} BRAND_SETTING;

//GC Configuration message [GC -> FFB].
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  unsigned char GCFirmwareRev[GC_FIRMWARE_REV_STR_LENGTH];
  unsigned char GCSerialNum[GC_SERIAL_NUM_STR_LENGTH];
  unsigned char byBrandSetting;
} __attribute(( packed )) FFB_GC_CONFIG_STRUCT;

//GC User Tag message [GC -> FFB].
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  unsigned char GCUserTag[GC_USER_TAG_STR_LENGTH];
} __attribute (( packed )) FFB_GC_USER_TAG_STRUCT;

//GC Network Configuration message [GC -> FFB and FFB -> GC]
typedef struct {
  unsigned char IPAddress1[GC_IP_ADDR_STR_LENGTH];
  unsigned char NetMask1[GC_IP_ADDR_STR_LENGTH];
  unsigned char IPAddress2[GC_IP_ADDR_STR_LENGTH];
  unsigned char NetMask2[GC_IP_ADDR_STR_LENGTH];
  unsigned char DefaultGatewayIPAddr[GC_IP_ADDR_STR_LENGTH];
} __attribute (( packed )) NETWORK_CONFIG_STRUCT;

typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  NETWORK_CONFIG_STRUCT stNtwInfo;
} __attribute (( packed )) FFB_GC_NETWORK_CONFIG_STRUCT;

//Stream Usage
typedef enum {
  STREAM_USAGE_NONE = 0,
  ANALYSIS,
  CALIBRATION,
  VALIDATION
} STREAM_USAGE;

//Detector assignment
typedef enum {
  DET_NONE = 0,
  DET1,
  DET2,
  DET1N2
} DET_ASSIGNMENT;

//GC Stream Configuration message [GC -> FFB].
typedef struct {
  unsigned char byStreamNum;
  unsigned char byStreamUsage;
  unsigned char byDetectorAssigned;
  unsigned char StreamName[GC_STREAM_NAME_STR_LENGTH];
} __attribute (( packed )) STREAM_INFO;
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  STREAM_INFO stStreamInfo;
} __attribute (( packed )) FFB_GC_STREAM_INIT_STRUCT;

//GC PV Configuration message [GC -> FFB].
typedef struct {
  unsigned char byPVNum;
  unsigned char PVDescription[GC_PV_DESC_STR_LENGTH];
} __attribute (( packed )) PV_INFO;
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  PV_INFO stPVInfo;
} __attribute (( packed )) FFB_GC_PV_INIT_STRUCT;

//Detector Types
typedef enum {
  DET_UNUSED = 0,
  FID,
  TCD
} DET_TYPE;

//GC Dectector Configuration message [GC -> FFB].
typedef struct {
  unsigned char byDetectorNum;
  unsigned char byDetectorType;
} __attribute (( packed )) DET_INFO;
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  DET_INFO stDetInfo;
} __attribute (( packed )) FFB_GC_DET_INIT_STRUCT;

//Device Mode
typedef enum {
  _DEV_MODE_UNAVAILABLE = 0,
  _1STR_1DET,
  _1STR_2DET,
  _2STR_2DET
} DEV_MODE;

//GC Device mode message [GC -> FFB].
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  unsigned char byDeviceMode;
} __attribute (( packed )) FFB_GC_DEVICE_MODE_STRUCT;


//Control Actions
typedef enum {
  NO_ACTION = 0,
  START_SINGLE_STREAM_CONTINUOUS,
  START_SINGLE_STREAM_ONE_TIME,
  START_MANUAL_CALIBRATION,
  START_MANUAL_VALIDATION,
  HALT_CURRENT_OPERATION,
  START_AUTO_SEQUENCE,
  START_FORCED_MANUAL_CALIBRATION
} CTRL_ACTION;

//Purge Actions
typedef enum {
  NOPURGE = 0,
  PURGE_BEFORE_CONTROL_ACTION
} PURGE_ACTION;    

// Operations
typedef enum {
  OPERATION_1 = 0,
  OPERATION_2,
  NUM_OPERATIONS
}  GC_OPERATION;

//FFB Control Action Request message [FFB -> GC].
typedef struct {
  unsigned char byOperationNum;
  unsigned char byStreamNum;
  unsigned char byCtrlAction;
  unsigned char byPurgeAction;
} __attribute (( packed )) CTRL_ACTION_STRUCT;
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  CTRL_ACTION_STRUCT stCtrlInfo;
} __attribute (( packed )) FFB_CTRL_ACTION_STRUCT;

//GC Status message [GC -> FFB].
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  unsigned short usRunTimeOpr1;
  unsigned short usRunTimeOpr2;
  unsigned long long ullErrInfo;
  unsigned long ulDetErrInfo;
} __attribute (( packed )) FFB_GC_STATUS_STRUCT;

// Config Actions
typedef enum {
  CANT_ACCEPT = 0,
  READY_TO_ACCEPT
} CONFIG_ACTION;
    
//GC Status Acknowledge message [FFB -> GC].
typedef struct {
  FFB_ACK_STRUCT stAckInfo;
  unsigned char byConfigAction;
} __attribute (( packed )) FFB_GC_STATUS_ACK_STRUCT;

//FFB Board's (64bit) time request message.
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
} __attribute (( packed )) FFB_TIME_REQ_STRUCT;

//FFB Board's (64bit) time Acknowledge message.
typedef struct {
  FFB_ACK_STRUCT stAckInfo;
  unsigned long long ullTime;
} __attribute (( packed )) FFB_TIME_ACK_STRUCT;

//FFB Board Capability Request message.
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
} __attribute (( packed )) FFB_BD_CAP_REQ_STRUCT; 

//FFB Board Capability Response message.
typedef struct {
  FFB_ACK_STRUCT stAckInfo;
  BD_CAP_STRUCT stFFBDevCaps;
} __attribute (( packed )) FFB_BD_CAP_ACK_STRUCT;

//Operation Modes
typedef enum {
  UNAVAILABLE = 0,
  WARM_UP,
  IDLE,
  AUTO_SEQUENCE,
  AUTO_CALIBRATION,
  AUTO_VALIDATION,
  SINGLE_STREAM_CONTINIOUS,
  SINGLE_STREAM_ONE_TIME,
  MANUAL_CAL,
  FORCED_CAL,
  MANUAL_VALIDATION,
  PURGE
} OPERATION_MODE;

//GC Operation Change message [GC -> FFB].
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  unsigned char byOperationNum;
  unsigned char byCurrOperationMode;
  unsigned char byStreamNum;
  unsigned char byNextStreamNum;
  unsigned short usRunTime;
  unsigned short usAnalysisTime;
  unsigned short usCycleTime;
} __attribute (( packed )) FFB_GC_OPERATION_CHANGE_STRUCT;

// PV Status
typedef enum {
  GOOD = 0,
  BAD
} PVSTATUS;
    
typedef struct {
  float fValue;
  unsigned char byPVStatus;
} __attribute (( packed )) PV_VALUE;

//GC PV value update message [GC -> FFB].
typedef struct {
  FFB_HEADER_INFO_STRUCT stHeaderInfo;
  PV_VALUE stPVData[NUM_PV_DATA_IN_PV_UPDATE]; 
} __attribute (( packed )) FFB_PV_UPDATE_STRUCT;

//Bit field definition of Error informaion for Plant Web Alart in GC Status message.
//1 - Error, 0 - OK.
typedef struct {
  unsigned long long HtrSol1_Sol1_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol2_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol3_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol4_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol5_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol6_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol7_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol8_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol9_Failed2Fire     : 1;
  unsigned long long HtrSol1_Sol10_Failed2Fire    : 1;
  unsigned long long HtrSol1_Sol11_Failed2Fire    : 1;
  unsigned long long HtrSol1_Sol12_Failed2Fire    : 1;
  unsigned long long HtrSol1_Htr1_ExcessDev       : 1;
  unsigned long long HtrSol1_Htr2_ExcessDev       : 1;
  unsigned long long HtrSol1_Htr3_ExcessDev       : 1;
  unsigned long long HtrSol1_Htr4_ExcessDev       : 1;
  unsigned long long HtrSol1_Comm_Failed          : 1;
  unsigned long long HtrSol2_Sol1_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol2_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol3_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol4_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol5_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol6_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol7_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol8_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol9_Failed2Fire     : 1;
  unsigned long long HtrSol2_Sol10_Failed2Fire    : 1;
  unsigned long long HtrSol2_Sol11_Failed2Fire    : 1;
  unsigned long long HtrSol2_Sol12_Failed2Fire    : 1;
  unsigned long long HtrSol2_Htr1_ExcessDev       : 1;
  unsigned long long HtrSol2_Htr2_ExcessDev       : 1;
  unsigned long long HtrSol2_Htr3_ExcessDev       : 1;
  unsigned long long HtrSol2_Htr4_ExcessDev       : 1;
  unsigned long long HtrSol2_Comm_Failed          : 1;
  unsigned long long Preamp1_Det1_ScaleFactorErr  : 1;
  unsigned long long Preamp1_Det2_ScaleFactorErr  : 1;
  unsigned long long Preamp1_Comm_Failed          : 1;
  unsigned long long Preamp2_Det1_ScaleFactorErr  : 1;
  unsigned long long Preamp2_Det2_ScaleFactorErr  : 1;
  unsigned long long Preamp2_Comm_Failed          : 1;
  unsigned long long BaseIO_EPC1_ExcessDev        : 1;
  unsigned long long BaseIO_EPC2_ExcessDev        : 1;
  unsigned long long BaseIO_EPC3_ExcessDev        : 1;
  unsigned long long BaseIO_EPC4_ExcessDev        : 1;
  unsigned long long BaseIO_EPC5_ExcessDev        : 1;
  unsigned long long BaseIO_Comm_Failed           : 1;
  unsigned long long GC_1_Idle                    : 1;
  unsigned long long GC_2_Idle                    : 1;
  unsigned long long GC_1_CalibrationFailed       : 1;
  unsigned long long GC_2_CalibrationFailed       : 1;
  unsigned long long GC_1_ValidationFailed        : 1;
  unsigned long long GC_2_ValidationFailed        : 1;
  unsigned long long GC_FID_FlameOut              : 1;
  unsigned long long GC_FID_OverTemp              : 1;
  unsigned long long GC_WarmStartFailed           : 1;
  unsigned long long GC_InMaintenanceMode         : 1;
  unsigned long long LowCarrierPressure_1         : 1;
  unsigned long long LowCarrierPressure_2         : 1;
  unsigned long long LowCarrierPressure_3         : 1;
  unsigned long long LowCarrierPressure_4         : 1;
  unsigned long long NoSampleFlow_1               : 1;
  unsigned long long NoSampleFlow_2               : 1;
  unsigned long long LossOfPurge                  : 1;
  unsigned long long Reserved                     : 1; //Reseved for future use.
} __attribute (( packed )) PWA_ERR_INFO;

typedef union {
  unsigned long long ullPwaErrInfo;
  PWA_ERR_INFO pwaErrInfo;
} __attribute ((packed)) PWA_ERR_INFO_UNION;

//Bit field definition of Detector Error informaion for Plant Web Alart in GC Status message.
//1 - Error, 0 - OK.
typedef struct {
  unsigned long DETECTOR_1_STATUS                : 1;
  unsigned long DETECTOR_1_FLAME_STATUS          : 2; //0: Unused, 1: Flame ON, 2: Flame OFF
  unsigned long DETECTOR_2_STATUS                : 1;
  unsigned long DETECTOR_2_FLAME_STATUS          : 2; //0: Unused, 1: Flame ON, 2: Flame OFF
  unsigned long DETECTOR_3_STATUS                : 1;
  unsigned long DETECTOR_3_FLAME_STATUS          : 2; //0: Unused, 1: Flame ON, 2: Flame OFF
  unsigned long DETECTOR_4_STATUS                : 1;
  unsigned long DETECTOR_4_FLAME_STATUS          : 2; //0: Unused, 1: Flame ON, 2: Flame OFF
  unsigned long Reserved                         : 20; //Reseved for future use.
} __attribute (( packed )) DET_STATUS_INFO;

typedef union {
  unsigned long ulDetStatusInfo;
  DET_STATUS_INFO DetStatusInfo;
} __attribute ((packed)) DET_STATUS_INFO_UNION;


#endif
