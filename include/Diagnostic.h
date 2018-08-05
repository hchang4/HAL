/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: Diagnostic.h
 *
 *  Description: Retrives Diagnostic informations of different boards.
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
 *  $Id: Diagnostic.h,v 1.10 2016/08/30 09:57:51 emrsn\jbeatty Exp $
 *  $Log: Diagnostic.h,v $
 *  Revision 1.10  2016/08/30 09:57:51  emrsn\jbeatty
 *  Mods for 1500XA R2 support
 *
 *  Revision 1.9  2015/04/30 09:32:31  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.8  2014/06/26 10:01:11  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.7  2013/01/29 14:33:45  emrsn\rsaha
 *  Revised 370XA Analyzer IO diagnostics details as per Rev.E schematics.
 *
 *  Revision 1.6  2012/03/21 06:49:04  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.5  2010/10/14 17:58:12  emrsn\rsaha
 *  Updated Analyzer IO Diagnostics structure : Added V+, V- and 3.3V supply readings and removed EPC version information as its no longer supported.
 *
 *  Revision 1.4  2010/06/10 20:10:01  emrsn\rsaha
 *  Added HAL interfaces for Pressure read, Serial RS232/485 control support and diagnostics details of Analyzer IO card.
 *
 *  Revision 1.3  2009/11/20 12:26:53  epm-na\rsaha
 *  Added support to retrieve LOI IRKeypad's diagnostic details.
 *
 *  Revision 1.2  2009/02/24 17:13:41  epm-na\rsaha
 *  Revised to display build number of the version information.
 *
 *  Revision 1.1  2008/11/25 00:15:18  epm-na\rsaha
 *  Has API's to retrive diagnostic information from preamp, heatersolenoid and baseIO board.
 *
 *************************************************************************/

#include "BaseDev.h"

#define PREAMP_1_SLOT_STR ":SLOT_1:"
#define PREAMP_2_SLOT_STR ":SLOT_3:"
#define PREAMP_3_SLOT_STR ":SLOT_5:"
#define HTRSOL_1_SLOT_STR ":SLOT_2:"
#define HTRSOL_2_SLOT_STR ":SLOT_4:"
#define BASEIO_SLOT_STR   ":SLOT_BASE_IO:"
#define IRKEY_SLOT_STR    ":LOI_SLOT:"
#define ANALYZER_SLOT_STR ":ANALYZER_SLOT:"


#define MAX_DIAGNO_RX_DATA  sizeof(BOARD_DIAGNOSTICS) + 2/*CRC length*/


typedef struct {
  int MajorNum;
  int MinorNum;
} __attribute(( packed )) FIRMWARE_REV;

typedef struct {
  float Input5V;      //In Voltage.
  float Input16V;     //In Voltage.
  long  OnBoardTemp;  //In Milli Degree C.
  DEVICE_SYSTEM_INFO_STRUCT SysInfo;
} __attribute(( packed )) PREAMP_DIAGNOSTIC;

typedef struct {
  float Input3_3V;    //In Voltage.
  float Input5V;      //In Voltage.
  long  OnBoardTemp;  //In Milli Degree C.
  DEVICE_SYSTEM_INFO_STRUCT SysInfo;
} __attribute(( packed )) HTRSOL_DIAGNOSTIC;

typedef struct {
  float Input3_3V;       //In Voltage.
  float Input5V;         //In Voltage.
  float Input24V;        //In Voltage.
  float Input24VCurrent; //In Amps
  long  OnBoardTemp;     //In Milli Degree C.
  float FIDSenseVoltage; //In Voltage.
  DEVICE_SYSTEM_INFO_STRUCT SysInfo;
} __attribute(( packed )) BASEIO_DIAGNOSTIC;

typedef struct {
  int OnBoardTemp;  //In tenths of Degree C. e.g: 17C is 170
  DEVICE_SYSTEM_INFO_STRUCT SysInfo;
} __attribute(( packed )) IRKEY_DIAGNOSTIC;

typedef struct {
  float Input10V;     //In Voltage.
  float Input22V;     //In Voltage.
  int   OnBoardTemp;  //In Milli Degree C.
  DEVICE_SYSTEM_INFO_STRUCT SysInfo;
} __attribute(( packed )) ANALYZER_DIAGNOSTIC;

typedef enum {
  PREAMP_DIAGNO,
  HTRSOL_DIAGNO,
  BASEIO_DIAGNO,
  IRKEY_DIAGNO,
  ANALYZER_DIAGNO,
  UNKNOWN_DIAGNO
} DIAGNOSTIC_TYPE;

typedef union {
  PREAMP_DIAGNOSTIC    Preamp;
  HTRSOL_DIAGNOSTIC    HtrSol;
  BASEIO_DIAGNOSTIC    BaseIO;
  IRKEY_DIAGNOSTIC     IRKey;
  ANALYZER_DIAGNOSTIC  Analyzer;
} BOARD_DIAGNOSTICS;


class CDiagnostic : public CBaseDev
{
  DIAGNOSTIC_TYPE GetDiagnoType();
public:
  CDiagnostic(){}
  ~CDiagnostic(){}

  int OpenHal( char * pszDevName );
  int CloseHal();

  int GetDiagnosticsDetails( DIAGNOSTIC_TYPE & DiagnoType, 
                             BOARD_DIAGNOSTICS & DiagoDetails );
};


