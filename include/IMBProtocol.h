/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: IMBProtocol.h
 *
 *  Description: IMB Communication enum and protocol definition.
 *
 *  Copyright:        Copyright (c) 2011-2012, 
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *  Revision History:
 *  $Id: IMBProtocol.h,v 1.25 2016/07/07 14:27:23 emrsn\wjyu Exp $
 *  $Log: IMBProtocol.h,v $
 *  Revision 1.25  2016/07/07 14:27:23  emrsn\wjyu
 *  synced with the copy of the GCXPIMB project
 *
 *  Revision 1.23  2016/07/05 18:08:16  emrsn\wjyu
 *  commented out unused structures and codes
 *
 *  Revision 1.22  2016/06/01 18:37:51  emrsn\wjyu
 *  IMB firmware 2.0.0 - fixed a compilation error in the structure
 *
 *  Revision 1.21  2016/05/27 20:49:46  emrsn\wjyu
 *  revised to support C7+ application
 *
 *  Revision 1.23  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.22  2014/06/26 10:01:15  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.21  2014/05/14 10:13:48  emrsn\wjyu
 *  For IMB firmware 1.0.1
 *  Revised libIMB, IMB serial number is in IMB Info struct instead of Global Data
 *
 *  Revision 1.19  2014/05/08 21:17:26  emrsn\wjyu
 *  For BZ 7326  The IMB DEFAULT section will be removed
 *  Removed the DEFAULT section from the IMB firmware
 *
 *  Revision 1.18  2014/05/02 18:49:40  emrsn\wjyu
 *  For BZ 7292 - Need to store the 12 digit Factory Assigned Module Serial Number in IMB
 *
 *  Revision 1.17  2014/04/21 16:40:48  emrsn\wjyu
 *  Added comments
 *  Changed variable nCurrentBackupTime in the IMB_AVT_SETTINGS_TABLE_STRUCT struct to nBackupTime (the same chnage was already done in the GC firmware)
 *
 *  Revision 1.16  2013/05/20 20:32:17  emrsn\wjyu
 *  IMB firmware 0.0.17
 *  - When writting to IMB, only cache (RAM buffer) will be updated.
 *  - Added a new command UpdateFlash to flush data from cache (RAM buffer) to flash
 *  - This is to avoid writting to flash 32 times when calling Force_IMB_Update. Data will be written to flash only once at the end using the new method
 *  - Changed nIdealRFRefComp (in Time Event Adjustment structure) from int to unsigned char to be consistent with component in CDT
 *  - (internal change , p_stGlobalData, p_stDefaultData, p_stFactoryData and p_stMirrorData are pointer to RAM buffer instead of Flash)
 *
 *  Revision 1.15  2013/05/15 06:26:41  EMRSN\jchekkala
 *  Bug fix 6711 - Revised to support storing four Dependent TEVs for each
 *  component. The component index will be stored on the IMB instead of
 *  component names and Dependent TEV will be stored in single dimensional
 *  character array seperated by semicolon.
 *
 *  Revision 1.14  2013/05/01 16:23:38  emrsn\wjyu
 *  Reduce IMB firmware RAM usage so that one component can save 4 associated time-events.
 *  -Removed Timed Events Adjustments structure from the Mirror Data Section
 *  -CDT will store component codes (1 byte) instead of full component names
 *  -Dependend TEVs will be used as one-dimetional array instead of two-dimentional array. This will save space because some components don't have dependent TEVs.
 *  -Add reserved field (40 bytes) to each data section
 *
 *  Revision 1.13  2013/04/23 22:03:30  emrsn\wjyu
 *  For IMB version 0.0.14
 *  Bug fix, when loading the IMB_CDT_DEFAULT table, dependent_tev (for CO2) gets truncated.
 *  Changed TIME_EVENT_DEPENDENT_TEV_LENGTH from 20 to 25
 *
 *  Revision 1.12  2013/03/21 19:35:46  emrsn\wjyu
 *  Bug fix -- Module Update is not working properly , stored rows in AVT Timed Event Adjustment is getting stored as 255 even though the IMB Library is passing the currently stored rows...
 *  -- Handle commands in seperate functions instead of the main HandleIMBCmds() function to reduce stack usage
 *  -- Revised commands, Get Hw Tev, Get Sw Tev, Get Cdt Config, Get Timed Event Adjustment and Get Flash Image
 *  -- Reduced buffer size for Get Flash Image from 256 bytes to 128 bytes
 *
 *  Revision 1.11  2013/03/07 14:21:05  emrsn\wjyu
 *  Add function to read IMB Flash Image
 *
 *  Revision 1.10  2013/02/25 14:41:47  emrsn\wjyu
 *  Commented out the ideal RF limit in the Global AVT settings. It is no longer used. It is now stored in the CDT and is component specific.
 *
 *  Revision 1.9  2013/02/11 08:15:41  emrsn\rsaha
 *  Version - 0.0.9, Date - 2013/02/11
 *
 *  Added back ids for each sections.
 *
 *  Revision 1.8  2013/02/08 17:14:14  emrsn\rsaha
 *  Version - 0.0.8, Date - 2013/02/08
 *
 *  Revised the following -
 *
 *  1. HW TEV row size increased from 12 to 15.
 *  2. DB ID from individual sections has been removed.
 *  3. Now on we have a sngle DB ID hardcoded in the firmeware. At present its value is Major Number : 1 and Minor Number : 0.
 *
 *  Note - Major Number will be incremented from now on whenever database structures get changed. Minor Number is reserved for now.
 *
 *  Revision 1.7  2013/01/29 14:52:00  emrsn\rsaha
 *  Version - 0.0.7, Date - 2013/01/29
 *
 *  Revised the following -
 *  1. Store 16 bytes IMB Device ID as it is, as 16 bytes IMB Serial number.
 *  2. Increased HW TEV row limit to 12 from 8.
 *  3. Resized Time field in HW, SW and Gain TEV tables from int to unsigned short.
 *
 *  Revision 1.6  2013/01/03 11:56:13  emrsn\rsaha
 *  Version - 0.0.6, Date - 2013/01/03
 *
 *  IMB firmware is revised with the following -
 *
 *  - Database ID is moved from Global Data section. Now Factory, Mirror and Default data section all have their individual Database Identifier field.
 *  - Time Event Adjustment Data also made part of Mirror Data Section.
 *  - Revised Project settings to use 512 bytes less for stack region, since due to TEV Adj Data structure inclusion in Mirror Data Section there is increase of about 385 bytes of RAM usage.
 *
 *  Revision 1.5  2012/11/30 08:31:20  emrsn\rsaha
 *  Version - 0.0.5, Date - 2012/11/30
 *
 *  IMB firmware is revised with the following -
 *
 *  - DB ID is now in Global data section, previously it was present in all the three - Default, Factory and Mirror secotions.
 *  - Revised AVT settings and AVT Valve Sel - Removed Peak Area Change limit from AVT settings and added it in AVT Valve.
 *  - Revised AVT Time Event Adjustment table -
 *    1. Removed Default RT.
 *    2. Added Ideal RF Ration.
 *    3. Added Ideal RF Reference Component.
 *    4. Added Ideal RF Limit.
 *  - Added AVT Settings, Valve Selection and Time Event Adjustments to Factory section also, previously they were in Default section only.
 *  - Carrier and Sample Zero and Span calibration factors are placed in Mirror sections also, previously they were in Factory section only.
 *  - Added Analysis Time configurations in all three - Default, Factory and Mirror sections.
 *
 *  - Revised Communication interface with CPU HAL layer to Read/Write the newly revised fields.
 *
 *  Revision 1.4  2012/03/22 08:45:08  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.3  2012/02/07 11:26:12  emrsn\rsaha
 *  IMB firmware revised and tested on actual IMB board. Version - 0.0.2 Date - 2012/02/07
 *  - This supports revised communication protocol with smaller request/response sizes.
 *  - Stored Rows filed is added to Table structures.
 *  - Added Zero and Span calibration value for Carrier and Sample pressure in Factory Section.
 *  - Uses ram to mirror falsh IMB data to protect against data loss during flash erase.
 *  - Reduce RX and TX CAN queue size to 100 from 200 to recover RAM.
 *  - Heap size increased to 1024
 *  - Stack size increase to 1024.
 *  - Tested with THUMB debug build.
 *  - Reduced buffer size used to accumulate received fragmented CAN packets from 2048 to 512 as the Communication protocol has been revised.
 *
 *
 *
 *  NOTE - To generate 8 bytes of Serial Number from 16 Bytes of Controller Unique ID, we are XOR-ing first half and second half of the Unique ID and storing them as Serial Number.
 *
 *  Revision 1.2  2012/01/31 15:15:09  emrsn\rsaha
 *  Simulator Build : Revised HAL - IMB communication protocol with smaller packet size.
 *
 *  Revision 1.1  2012/01/03 13:19:22  emrsn\rsaha
 *  Tested IMB firmware code with Simulator Support.
 *
 *************************************************************************/

#ifndef _IMB_PROTOCOL_H
#define _IMB_PROTOCOL_H


#include "DevProtocol.h"


//Enum definitions for different flash sections in IMB.
typedef enum {
  //IMB_DEFAULT_FLASH_SECTION     = 0,     
  IMB_GLOBAL_FLASH_SECTION      = 0,
  IMB_FLASH_SECTION_START       = 1,    //START   ???
  IMB_FACTORY_FLASH_SECTION     = 1,
  IMB_MIRROR_FLASH_SECTION      = 2,
  IMB_INVALID_FLASH_SECTION     = 3,
  IMB_FLASH_SECTION_END         = 3     //END
}IMB_FLASH_SECTION_ENUM;

//Enum definitions for Set/Get Commands.
typedef enum {
  // Set commands
  CMD_IMB_FN_SET_GLOBAL_DATA_IMB_INIT       = 0,    // GLOBAL -- set INIT flag
  CMD_IMB_FN_SET_SECTION_META_INFO          = 1,    // METAINFO
  CMD_IMB_FN_SET_SECTION_AVT_SETTINGS_TABLE = 2,    // AVT - Settings
  CMD_IMB_FN_SET_SECTION_AVT_VSEL_TABLE     = 3,    // AVT - Valve Selections
  CMD_IMB_FN_SET_SECTION_AVT_TEADJ_TABLE    = 4,    // AVT - Timed Events Adjustment
  CMD_IMB_FN_SET_SECTION_CDT_CONFIG         = 5,    // CDT - CONFIG (RT_SEC_DEV and RF_PCT)
  CMD_IMB_FN_SET_SECTION_CDT_UPDATE         = 6,    // CDT - Update (RT and RF)
  CMD_IMB_FN_SET_SECTION_HTEV_TABLE         = 7,    // TEV - Hardware TEV
  CMD_IMB_FN_SET_SECTION_STEV_TABLE         = 8,    // TEV - Software TEV
  CMD_IMB_FN_SET_SECTION_GTEV_TABLE         = 9,    // TEV - Gain TEV
  CMD_IMB_FN_SET_SECTION_SET_POINTS         = 10,   // Set Points (Pressure and Heater Temperature)
  CMD_IMB_FN_SET_MIRROR_AVT_ENABLE          = 11,   // AVT_ENABLED flag
  CMD_IMB_FN_SET_MIRROR_NUM_ANALYSIS        = 12,   // Number of Analysis
  CMD_IMB_FN_SET_SECTION_CARRIER_CAL_FAQ    = 13,   // Carrier Calibration Factors
  CMD_IMB_FN_SET_SECTION_SAMPLE_CAL_FAQ     = 14,   // Sample Pressure Calibration Factors
  CMD_IMB_FN_SET_SECTION_ANALYSIS_CONFIG    = 15,   // TEV - Analysis Time CONFIG

  //Write IMB Data
  CMD_IMB_FN_WRITE_IMB_DATA                 = 20,


  // Get commands
  CMD_IMB_FN_GET_GLOBAL_DATA_SECTION        = 40,
  CMD_IMB_FN_GET_SECTION_META_INFO          = 41,   // METAINFO
  CMD_IMB_FN_GET_SECTION_AVT_SETTINGS_TABLE = 42,   // AVT - Settings
  CMD_IMB_FN_GET_SECTION_AVT_VSEL_TABLE     = 43,   // AVT - Valve Selections
  CMD_IMB_FN_GET_SECTION_AVT_TEADJ_TABLE    = 44,   // AVT - Timed Events Adjustment
  CMD_IMB_FN_GET_SECTION_CDT_CONFIG         = 45,   // CDT - CONFIG (RT_SEC_DEV and RF_PCT)
  CMD_IMB_FN_GET_SECTION_CDT_UPDATE         = 46,   // CDT - Update (RT and RF)
  CMD_IMB_FN_GET_SECTION_HTEV_TABLE         = 47,   // TEV - Hardware TEV
  CMD_IMB_FN_GET_SECTION_STEV_TABLE         = 48,   // TEV - Software TEV
  CMD_IMB_FN_GET_SECTION_GTEV_TABLE         = 49,   // TEV - Gain TEV
  CMD_IMB_FN_GET_SECTION_SET_POINTS         = 50,   // Set Points (Pressure and Heater Temperature)
  CMD_IMB_FN_GET_MIRROR_AVT_ENABLE          = 51,   // AVT_ENABLED flag
  CMD_IMB_FN_GET_MIRROR_NUM_ANALYSIS        = 52,   // Number of Analysis
  CMD_IMB_FN_GET_SECTION_CARRIER_CAL_FAQ    = 53,   // Carrier Calibration Factors
  CMD_IMB_FN_GET_SECTION_SAMPLE_CAL_FAQ     = 54,   // Sample Pressure Calibration Factors
  CMD_IMB_FN_GET_SECTION_ANALYSIS_CONFIG    = 55,   // TEV - Analysis Time CONFIG
  CMD_IMB_FN_GET_STRUCTURE_ID               = 56,

  //Get Flash Image command - Change to Read IMB Data
  CMD_IMB_FN_READ_IMB_DATA                  = 60,

  //Update Flash
  CMD_IMB_FN_UPDATE_FLASH                   = 80,  //considered as a set command ?



}IMB_COMM_CMD_ENUM;


// Structure to be used for Sending Status / Nack Error code to the Host
typedef struct {
  CmdAckUnion   stCmd;
  STATUS_STRUCT   stData; // Status
} __attribute (( packed )) CAN_IMB_STATUS_STRUCT;

//Structures for holding data of different tables.
//------------------------------------------------

#define   HW_TEV_TABLE_MAX_ROW_SZ               15
#define   SW_TEV_TABLE_MAX_ROW_SZ               30
#define   GAIN_TEV_TABLE_MAX_ROW_SZ             5
#define   CDT_TABLE_MAX_ROW_SZ                  12
#define   VALVE_SEL_TABLE_MAX_ROW_SZ            3
#define   TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ       12

#define   TIME_EVENT_DEPENDENT_TEV_LENGTH       250
#define   META_INFO_SOFTWARE_REV_LENGTH         20
#define   IMB_SERIAL_NUMBER_LENGTH              16       // IMB serial number of the LPC11 device serial number (read-only)
#define   MODULE_SERIAL_NUMBER_LENGTH           12       // Factory Assigned Module Serial Number (read-write)

#define   FLASH_IMAGE_DATA_LENGTH               128

#define   RESERVED_FIELD_LENGTH                 40
#define   RESERVED_FIELD_LENGTH2                20      //Reserved
#define   RESERVED_FIELD_LENGTH3                20      //Reserved

//Hardware TEV.
typedef struct {
  unsigned char       byTEVType[HW_TEV_TABLE_MAX_ROW_SZ];
  unsigned char       byValveDO[HW_TEV_TABLE_MAX_ROW_SZ];
  unsigned char       byState[HW_TEV_TABLE_MAX_ROW_SZ];
  unsigned short      usTime[HW_TEV_TABLE_MAX_ROW_SZ];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_HW_TEV_TABLE_STRUCT;

//Software TEV.
typedef struct {
  unsigned char       byTEVType[SW_TEV_TABLE_MAX_ROW_SZ];
  unsigned char       byDetector[SW_TEV_TABLE_MAX_ROW_SZ];
  short               sValue[SW_TEV_TABLE_MAX_ROW_SZ];
  unsigned short      usTime[SW_TEV_TABLE_MAX_ROW_SZ];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_SW_TEV_TABLE_STRUCT;

//Gain TEV.
typedef struct {
  unsigned char       byDetector[GAIN_TEV_TABLE_MAX_ROW_SZ];
  unsigned char       byGain[GAIN_TEV_TABLE_MAX_ROW_SZ];
  unsigned short      usTime[GAIN_TEV_TABLE_MAX_ROW_SZ];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_GAIN_TEV_TABLE_STRUCT;

//CDT Configuration Columns
typedef struct {
  unsigned char       szComponent[CDT_TABLE_MAX_ROW_SZ];
  short               sRTDev[CDT_TABLE_MAX_ROW_SZ];
  short               sRFPercent[CDT_TABLE_MAX_ROW_SZ];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_CDT_CONFIG_STRUCT;

//CDT Update Columns.
typedef struct {
  int                 nRT[CDT_TABLE_MAX_ROW_SZ];
  float               fRF[CDT_TABLE_MAX_ROW_SZ];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_CDT_UPDATE_STRUCT;

//CDT Table.
typedef struct {
  IMB_CDT_CONFIG_STRUCT stCDTConfig;
  IMB_CDT_UPDATE_STRUCT stCDTUpdate;
} __attribute (( packed )) IMB_CDT_TABLE_STRUCT;

//Global AVT table.
typedef struct {
  float               fValveTimingAdj;
  float               fAdjAlarmLimit;
  //float               fIdealRFLimit;  // no longer used (now saved in CDT -- Time Events Adjustment)
  int                 nEVCAreaRepeats;
  int                 nUseRepeats4ValveTiming;
  int                 nBackupTime;
} __attribute (( packed )) IMB_AVT_SETTINGS_TABLE_STRUCT;

//Valve Selection table.
typedef struct {
  int                 nName[VALVE_SEL_TABLE_MAX_ROW_SZ];
  float               fInitialTimingOn[VALVE_SEL_TABLE_MAX_ROW_SZ];
  float               fInitialTimingOff[VALVE_SEL_TABLE_MAX_ROW_SZ];
  int                 nTimingToAdjust[VALVE_SEL_TABLE_MAX_ROW_SZ];
  unsigned char       byEarlyValveCutComp[VALVE_SEL_TABLE_MAX_ROW_SZ];
  unsigned char       byLateValveCutComp[VALVE_SEL_TABLE_MAX_ROW_SZ];
  float               fPeakAreaChangeLimit[VALVE_SEL_TABLE_MAX_ROW_SZ];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_AVT_VALVE_SEL_TABLE_STRUCT;

//Time Events Adjustment (Part of CDT)
typedef struct {
  float               fIdealRFRatio[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ];
  unsigned char       nIdealRFRefComp[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ];
  float               fIdealRFLimit[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ];
  unsigned char       szDependentTEV[TIME_EVENT_DEPENDENT_TEV_LENGTH];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT;

//Analysis Time Configuration
typedef struct {
  int                 nCycleTime;
  int                 nAnalysisTime;
} __attribute (( packed )) IMB_ANALYSIS_TIME_CONFIG_STRUCT;

//IMB Database Identifier
typedef struct {
  unsigned short      usMajor;
  unsigned short      usMinor;
} __attribute (( packed )) IMB_DB_ID_STRUCT;

//IMB Structure Identifier
typedef struct {
  unsigned short      struMajor;
  unsigned short      struMinor;
} __attribute (( packed )) IMB_STRUCTURE_ID_STRUCT;

//IMB API Identifier
typedef struct {
  unsigned short      apiMajor;
  unsigned short      apiMinor;
} __attribute (( packed )) IMB_API_ID_STRUCT;

//Meta info details of sections.
typedef struct {
  unsigned char      szGCSWRev[META_INFO_SOFTWARE_REV_LENGTH];
  IMB_DB_ID_STRUCT   stIMBDbId;
  unsigned char      metaReserved[RESERVED_FIELD_LENGTH3];      //Reserved
  unsigned char      metaReserved2[RESERVED_FIELD_LENGTH3];     //Reserved
} __attribute (( packed )) IMB_SECTION_META_INFO;


//Structures for holding data of different sections.
//--------------------------------------------------

//////////////////////////////////////////////////////////////////////
//Structure to be used for "Global Data Section"
//////////////////////////////////////////////////////////////////////
typedef struct {
  //unsigned char       byIMBSerialNumber[IMB_SERIAL_NUMBER_LENGTH];        // IMB serial number of the LPC11 device serial number (read-only)
  unsigned char       byIMBInit;
  unsigned char       byModuleSerialNum[MODULE_SERIAL_NUMBER_LENGTH];  // Factory assigned Module Serial Number (read-write)
} __attribute (( packed )) IMB_GLOBAL_DATA_STRUCT;


//////////////////////////////////////////////////////////////////////
//Structure to be used for "Factory Data Section"
//////////////////////////////////////////////////////////////////////
typedef struct {
  IMB_SECTION_META_INFO                  stMetaInfo;
  IMB_HW_TEV_TABLE_STRUCT                stHwTEV;               //TEV
  IMB_SW_TEV_TABLE_STRUCT                stSwTEV;
  IMB_GAIN_TEV_TABLE_STRUCT              stGainTEV;
  IMB_ANALYSIS_TIME_CONFIG_STRUCT        stAnlyTimeCfg;
  IMB_CDT_TABLE_STRUCT                   stCDT;                 //CDT
  IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT    stAVTTimeEventAdj;
  IMB_AVT_SETTINGS_TABLE_STRUCT          stAVTSettings;         //AVT
  IMB_AVT_VALVE_SEL_TABLE_STRUCT         stAVTValveSel;
  float                                  fPresSetPoint;
  float                                  fTempSetPoint;
  float                                  fCarrierPressZeroCalFaq;  // used for Base Temperature
  float                                  fCarrierPressSpanCalFaq;  // used for Correction Factor
  float                                  fSamplePressZeroCalFaq;
  float                                  fSamplePressSpanCalFaq;
  unsigned char                          reserved[RESERVED_FIELD_LENGTH];  //reserved
} __attribute (( packed )) IMB_FACTORY_DATA_STRUCT;

//////////////////////////////////////////////////////////////////////
//Structure to be used for "Mirror Data Section"
//////////////////////////////////////////////////////////////////////
typedef struct {
  IMB_SECTION_META_INFO                  stMetaInfo;
  IMB_HW_TEV_TABLE_STRUCT                stHwTEV;               //TEV
  IMB_SW_TEV_TABLE_STRUCT                stSwTEV;
  IMB_GAIN_TEV_TABLE_STRUCT              stGainTEV;
  IMB_ANALYSIS_TIME_CONFIG_STRUCT        stAnlyTimeCfg;
  IMB_CDT_TABLE_STRUCT                   stCDT;                 //CDT
  //IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT  stAVTTimeEventAdj;
  float                                  fPresSetPoint;
  float                                  fTempSetPoint;
  float                                  fCarrierPressZeroCalFaq;
  float                                  fCarrierPressSpanCalFaq;
  float                                  fSamplePressZeroCalFaq;
  float                                  fSamplePressSpanCalFaq;
  int                                    nNumAnalysis;
  unsigned char                          byAVTEnabled;
  unsigned char                          reserved[RESERVED_FIELD_LENGTH];  //reserved
} __attribute (( packed )) IMB_MIRROR_DATA_STRUCT;

//####################################################################
//####################################################################
// VERSION_2 structures
//####################################################################
//####################################################################

#define   SW_TEV_TABLE_MAX_ROW_SZ_EXT           20        // Total SW_TEV_TABLE_MAX_ROW_SZ ( 30 ) + 20 = 50
#define   CDT_TABLE_MAX_ROW_SZ_EXT              3         // Total CDT_TABLE_MAX_ROW ( 12 ) + 3 = 15
#define   TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT   3         // Total TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ ( 12 ) + 3 = 15
#define   TIME_EVENT_DEPENDENT_TEV_LENGTH_EXT   50
#define   TIME_EVENT_DEPENDENT_TEV_LENGTH2      (TIME_EVENT_DEPENDENT_TEV_LENGTH - TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ)

#define   CDT_CUSTOM_NAME_MAX_ROW_SZ	          5         // CDT-custom-name max row size
#define   CDT_CUSTOM_NAME_MAX_STR_SZ    	      20        // CDT-custom-name max string size
#define   APPLICATION_TYPE_STR_LENGTH           20        // Application-Type string length (read-write)
#define   MV_ALLOWABLE_DEV_MAX_ROW_SZ           15        // This MUST match the CDT maxrow size !!!!!!
#define   UID_64_MAX_STR_SZ    	                11        // GUID-64 represented as Base64 string

//////////////////////////////////////////////////////////////////////
//Software TEV - Extention - for VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  unsigned char       byTEVType[SW_TEV_TABLE_MAX_ROW_SZ_EXT];
  unsigned char       byDetector[SW_TEV_TABLE_MAX_ROW_SZ_EXT];
  short               sValue[SW_TEV_TABLE_MAX_ROW_SZ_EXT];
  unsigned short      usTime[SW_TEV_TABLE_MAX_ROW_SZ_EXT];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_SW_TEV_TABLE_STRUCT_EXT;


//////////////////////////////////////////////////////////////////////
//CDT Configuration - Extention- for VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  unsigned char       szComponent[CDT_TABLE_MAX_ROW_SZ_EXT];
  short               sRTDev[CDT_TABLE_MAX_ROW_SZ_EXT];
  short               sRFPercent[CDT_TABLE_MAX_ROW_SZ_EXT];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_CDT_CONFIG_STRUCT_EXT;


//////////////////////////////////////////////////////////////////////
//CDT Update - Extention- VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  int                 nRT[CDT_TABLE_MAX_ROW_SZ_EXT];
  float               fRF[CDT_TABLE_MAX_ROW_SZ_EXT];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_CDT_UPDATE_STRUCT_EXT;

//////////////////////////////////////////////////////////////////////
//CDT Table - Extention - for VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  IMB_CDT_CONFIG_STRUCT_EXT stCDTConfigExt;
  IMB_CDT_UPDATE_STRUCT_EXT stCDTUpdateExt;
} __attribute (( packed )) IMB_CDT_TABLE_STRUCT_EXT;

//////////////////////////////////////////////////////////////////////
//Time Events Adjustment (Part of CDT) - for VERSION_2
//It should has the SAME SIZE as the VERSION1 structure !!!!
//////////////////////////////////////////////////////////////////////
typedef struct {
  float               fIdealRFRatio[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ];
  unsigned char       nIdealRFRefComp[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ];
  float               fIdealRFLimit[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ];
  unsigned char       szDependentTEV[TIME_EVENT_DEPENDENT_TEV_LENGTH2];
  unsigned char       chComponent[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ];           // Component-Index, NEW !!!!!
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2;


//////////////////////////////////////////////////////////////////////
//Time Events Adjustment - Extention - for VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  float               fIdealRFRatio[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT];
  unsigned char       nIdealRFRefComp[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT];
  float               fIdealRFLimit[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT];
  unsigned char       szDependentTEV[TIME_EVENT_DEPENDENT_TEV_LENGTH_EXT];      // NOT used (as of may, 2016), kept for future use
  unsigned char       chComponent[TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT];         // Component-Index
  unsigned char       byStoredRows;
  //unsigned char       maxRowSize() { return TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT; };
} __attribute (( packed )) IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT;

//////////////////////////////////////////////////////////////////////
//CDT Custom Name - for VERSION_2
//////////////////////////////////////////////////////////////////////
//typedef struct {
//  unsigned char       szCompCustName[CDT_CUSTOM_NAME_MAX_STR_SZ];
//} __attribute (( packed )) IMB_CDT_CUSTOM_NAME;

typedef struct {
  unsigned char         szCompCustName[CDT_CUSTOM_NAME_MAX_ROW_SZ][CDT_CUSTOM_NAME_MAX_STR_SZ];      //Custom-Component Name
  unsigned char         szComponent[CDT_CUSTOM_NAME_MAX_ROW_SZ];                                     //Component Code (Index)
  unsigned char       	byStoredRows;
} __attribute (( packed )) IMB_CDT_CUSTOM_NAME_STRUCT;


//////////////////////////////////////////////////////////////////////
// Module-Validation Allowable-Deviations - added in VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  unsigned char       chComponent[MV_ALLOWABLE_DEV_MAX_ROW_SZ];           // Component-Index
  short               sAllowableDev[MV_ALLOWABLE_DEV_MAX_ROW_SZ];
  unsigned char       byStoredRows;
} __attribute (( packed )) IMB_MV_ALLOWABLE_DEV_STRUCT;


//////////////////////////////////////////////////////////////////////
// UID - added in VERSION_2 (currently using UID-64)
//////////////////////////////////////////////////////////////////////
typedef struct {
  unsigned char       chUID64[UID_64_MAX_STR_SZ];           // UID-64
  unsigned char       metaReserved[RESERVED_FIELD_LENGTH3 - UID_64_MAX_STR_SZ];      //Reserved, pad to make it 20 chars
} __attribute (( packed )) IMB_UID_STRUCT;

//////////////////////////////////////////////////////////////////////
// MetaInfo - added in VERSION_2
// stMetaUIdA - the UID does NOT change from version to version
// stMetaUIdB - each version will have a unique ID
// It should has the SAME SIZE as the VERSION1 structure !!!!
//////////////////////////////////////////////////////////////////////
typedef struct {
  unsigned char      szGCSWRev[META_INFO_SOFTWARE_REV_LENGTH];
  IMB_DB_ID_STRUCT   stIMBDbId;
  IMB_UID_STRUCT     stMetaUIDA;									// UID-64, added in VERSION2
  IMB_UID_STRUCT     stMetaUIDB;                  // UID-64, added in VERSION2
} __attribute (( packed )) IMB_SECTION_META_INFO2;

//////////////////////////////////////////////////////////////////////
//Application Name - for VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  unsigned char         byApplicationType[APPLICATION_TYPE_STR_LENGTH];  // Application Type (read-write)
} __attribute (( packed )) IMB_APPLICATION_TYPE_STRUCT;

//////////////////////////////////////////////////////////////////////
//Structure to be used for "Global Data Section" - VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  unsigned char       				byIMBInit;
  unsigned char       				byModuleSerialNum[MODULE_SERIAL_NUMBER_LENGTH];  // Factory assigned Module Serial Number (read-write)
  //--------------------------------------------------------------------
  //structures added for VERSION_2
  //--------------------------------------------------------------------
  IMB_SECTION_META_INFO2            stMetaInfo;         // MetaInfo -- NEW in VERSION2 !!!!!!!!!!!!!!!!!!!!!!
  IMB_APPLICATION_TYPE_STRUCT       stApplicationType;  // Application Type (read-write)
} __attribute (( packed )) IMB_GLOBAL_DATA_STRUCT2;


//////////////////////////////////////////////////////////////////////
//Structure to be used for "Factory Data Section" - VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  IMB_SECTION_META_INFO2                 stMetaInfo;				        //MetaInfo - VERSION2, the same length
  IMB_HW_TEV_TABLE_STRUCT                stHwTEV;                   //TEV
  IMB_SW_TEV_TABLE_STRUCT                stSwTEV;				            //SW-TEV
  IMB_GAIN_TEV_TABLE_STRUCT              stGainTEV;
  IMB_ANALYSIS_TIME_CONFIG_STRUCT        stAnlyTimeCfg;
  IMB_CDT_TABLE_STRUCT                   stCDT;                     //CDT
  IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2   stAVTTimeEventAdj;         //Timed-Event-ADJ - VERSION2, the same length
  IMB_AVT_SETTINGS_TABLE_STRUCT          stAVTSettings;             //AVT
  IMB_AVT_VALVE_SEL_TABLE_STRUCT         stAVTValveSel;             //VALVE-SEL
  float                                  fPresSetPoint;
  float                                  fTempSetPoint;
  float                                  fCarrierPressZeroCalFaq;   // used for Base Temperature
  float                                  fCarrierPressSpanCalFaq;   // used for Correction Factor
  float                                  fSamplePressZeroCalFaq;
  float                                  fSamplePressSpanCalFaq;
  unsigned char                          reserved[RESERVED_FIELD_LENGTH];
  //--------------------------------------------------------------------
  //structures added for VERSION_2
  //--------------------------------------------------------------------
  IMB_CDT_CUSTOM_NAME_STRUCT             stCDTCustom;               //CDT-custom-name      - for VERSION_2
  IMB_SW_TEV_TABLE_STRUCT_EXT            stSwTEVExt;				        //SW-TEV-Eextention    - for VERSION_2
  IMB_CDT_TABLE_STRUCT_EXT               stCDTExt;                  //CDT-Eextention       - for VERSION_2
  IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT stAVTTimeEventAdjExt;     //Timed_Event-ADJ-Eextention - for VERSION_2
  IMB_MV_ALLOWABLE_DEV_STRUCT            stMVAllowableDEV;          //MV Allowable-Deviations - added for VERSION_2
} __attribute (( packed )) IMB_FACTORY_DATA_STRUCT2;

//////////////////////////////////////////////////////////////////////
//Structure to be used for "Mirror Data Section" - VERSION_2
//////////////////////////////////////////////////////////////////////
typedef struct {
  IMB_SECTION_META_INFO2                 stMetaInfo;
  IMB_HW_TEV_TABLE_STRUCT                stHwTEV;                   //TEV
  IMB_SW_TEV_TABLE_STRUCT                stSwTEV;				            //SW-TEV
  IMB_GAIN_TEV_TABLE_STRUCT              stGainTEV;
  IMB_ANALYSIS_TIME_CONFIG_STRUCT        stAnlyTimeCfg;
  IMB_CDT_TABLE_STRUCT                   stCDT;                     //CDT
  //IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT  stAVTTimeEventAdj;
  float                                  fPresSetPoint;
  float                                  fTempSetPoint;
  float                                  fCarrierPressZeroCalFaq;
  float                                  fCarrierPressSpanCalFaq;
  float                                  fSamplePressZeroCalFaq;
  float                                  fSamplePressSpanCalFaq;
  int                                    nNumAnalysis;
  unsigned char                          byAVTEnabled;
  unsigned char                          reserved[RESERVED_FIELD_LENGTH];
  //--------------------------------------------------------------------
  //structures added for VERSION_2
  //--------------------------------------------------------------------
  IMB_CDT_CUSTOM_NAME_STRUCT             stCDTCustom;				        //CDT-custom-name   - for VERSION_2
  IMB_SW_TEV_TABLE_STRUCT_EXT            stSwTEVExt;				        //SW-TEV-Eextention - for VERSION_2
  IMB_CDT_TABLE_STRUCT_EXT               stCDTExt;                  //CDT-Eextention    - for VERSION_2
} __attribute (( packed )) IMB_MIRROR_DATA_STRUCT2;

//####################################################################
//####################################################################
// end of VERSION_2 structures
//####################################################################
//####################################################################

//////////////////////////////////////////////////////////////////////
//Command Structures.
//-------------------
// Structure to be used for configuring (Setting / Getting) Global Data Section.
#ifdef IMB_COMM_V1

typedef struct {
  CmdAckUnion     stCmd;
  IMB_GLOBAL_DATA_STRUCT          stData; //Global Data Section.
} __attribute (( packed )) CAN_CMD_IMB_GLOBAL_DATA_STRUCT;

#endif

// Command Structure to request specific section details.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char     bySectionType;
} __attribute (( packed )) CAN_CMD_IMB_SECTION_DETAILS_REQUEST_STRUCT;

#ifdef IMB_COMM_V1

// Structure to be used for Meta-Info (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char     bySectionType;
  IMB_SECTION_META_INFO   stData; //Section Meta info details.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_METAINFO_STRUCT;


// Structure to be used for Global AVT table (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char                   bySectionType;
  IMB_AVT_SETTINGS_TABLE_STRUCT stData; //Global AVT Table for Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_AVT_STG_STRUCT;


// Structure to be used for Valve Table (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char                   bySectionType;
  IMB_AVT_VALVE_SEL_TABLE_STRUCT  stData; //IMB Valve Table for Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_AVTVSEL_TABLE_STRUCT;

// Structure to be used for Time Event Adjustment Table (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion                          stCmd;
  unsigned char                        bySectionType;
  IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT  stData; //IMB Time Event Adjustment Table for Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_AVTTEADJ_TABLE_STRUCT;

// Structure to be used for CDT configuration (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char     bySectionType;
  IMB_CDT_CONFIG_STRUCT   stData; //CDT Configurations for a Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_CDT_CONFIG_STRUCT;

// Structure to be used for CDT updates (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char     bySectionType;
  IMB_CDT_UPDATE_STRUCT   stData; //CDT Updates for a Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_CDT_UPDATE_STRUCT;

// Structure to be used for HW TEV (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char     bySectionType;
  IMB_HW_TEV_TABLE_STRUCT   stData; //IMB HW TEV Table for Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_HTEV_TABLE_STRUCT;

// Structure to be used for SW TEV (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char     bySectionType;
  IMB_SW_TEV_TABLE_STRUCT   stData; //IMB SW TEV Table for Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_STEV_TABLE_STRUCT;

// Structure to be used for Gain TEV (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char     bySectionType;
  IMB_GAIN_TEV_TABLE_STRUCT stData; //IMB Gain TEV Table for Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_GTEV_TABLE_STRUCT;

// Structure to be used for Analysis Configuration (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char                   bySectionType;
  IMB_ANALYSIS_TIME_CONFIG_STRUCT stData; //IMB Analysis Configuration Table for Section.
} __attribute (( packed )) CAN_CMD_IMB_SECTION_ANALYSIS_TIME_CONFIG_TABLE_STRUCT;

// Structure to be used for Pressure & Temperature set points (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion                 stCmd;
  unsigned char                   bySectionType;
  float                           fPresSetPoint;
  float                           fTempSetPoint;
} __attribute (( packed )) CAN_CMD_IMB_SECTION_SET_POINTS_STRUCT;

// Structure to be used for Pressure Calibration Factors (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion                     stCmd;
  unsigned char                   bySectionType;
  float                           fZeroCalFaq;
  float                           fSpanCalFaq;
} __attribute (( packed )) CAN_CMD_IMB_SECTION_CAL_FAQ_STRUCT;

// Structure to be used for AVT Enable status (Setting / Getting) in Sections.
typedef struct {
  CmdAckUnion                     stCmd;
  unsigned char                   byAVTEnabled;
} __attribute (( packed )) CAN_CMD_IMB_MIRROR_AVT_ENABLE_STRUCT;

// Structure to be used for Number of Analysis (Setting / Getting) in Mirror Data Section.
typedef struct {
  CmdAckUnion     stCmd;
  int       nNumAnalysis; //Number of Analysis for Mirror Section.
} __attribute (( packed )) CAN_CMD_IMB_MIRROR_NUM_ANALYSIS_STRUCT;


// Structure to be used for Setting IMB Initialized Flag.
typedef struct {
  CmdAckUnion                 stCmd;
  unsigned char                   byIMBInit;
} __attribute (( packed )) CAN_CMD_IMB_GLOBAL_INIT_STRUCT;

#endif

//IMB Meta info.
typedef struct { 
  IMB_STRUCTURE_ID_STRUCT   stIMBStruId;                                  //Structure ID
  IMB_API_ID_STRUCT           stIMBApiId;                                 //API ID
  unsigned char               byIMBSerialNum[IMB_SERIAL_NUMBER_LENGTH];   // IMB Serial Number ( of the LPC11 device serial number (read-only))
  unsigned char               infoReserved[RESERVED_FIELD_LENGTH2];       // Reserved
  unsigned char               infoReserved2[RESERVED_FIELD_LENGTH2];      // Reserved
} __attribute (( packed )) IMB_IMB_META_INFO;


// Structure to be used for Getting IMB Meta INFO.
typedef struct {
  CmdAckUnion                 stCmd;
  IMB_IMB_META_INFO           stData;
} __attribute (( packed )) CAN_CMD_IMB_INFO_STRUCT;

// Command Structure to request read IMB data (Request)
typedef struct {
  CmdAckUnion     stCmd;
  unsigned char       bySectionType;
  unsigned int        nFlashOffset;
  unsigned int        nFlashLen;      //Requested data length
} __attribute (( packed )) CAN_CMD_IMB_READ_DATA_REQUEST_STRUCT;

// Command Structure for reading IMB flash image data (Reply)
typedef struct {
  CmdAckUnion                 stCmd;
  unsigned char               bySectionType;
  unsigned char               flashData[FLASH_IMAGE_DATA_LENGTH];
  unsigned int                nRequestOffset;   //Offset in the request
  unsigned int                nRequestLen;      //Data length in the request
  unsigned int                flashDataLen;     //Returned data length (Actual data read)
} __attribute (( packed )) CAN_CMD_IMB_READ_DATA_STRUCT;

// Command Structure for writing IMB flash image data.
typedef struct {
  CmdAckUnion                 stCmd;
  unsigned char               bySectionType;
  unsigned char               inflashData[FLASH_IMAGE_DATA_LENGTH]; // input data
  unsigned int                uinFlashOffset;   // input data offset
  unsigned int                uinFlashLen;      // input data length
} __attribute (( packed )) CAN_CMD_IMB_WRITE_DATA_STRUCT;


#endif //#ifndef _IMB_PROTOCOL_H


