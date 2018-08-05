/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: AnalyzerProtocol.h
 *
 *  Description: Pressure and Control function enum and protocol definition.
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
 *  $Id: AnalyzerProtocol.h,v 1.4 2015/04/30 09:32:30 EMRSN\Dkad Exp $
 *  $Log: AnalyzerProtocol.h,v $
 *  Revision 1.4  2015/04/30 09:32:30  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.3  2014/06/26 10:01:08  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.2  2012/03/21 06:49:02  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2010/06/10 20:10:01  emrsn\rsaha
 *  Added HAL interfaces for Pressure read, Serial RS232/485 control support and diagnostics details of Analyzer IO card.
 *
 *************************************************************************/

#ifndef _ANALYZER_PROTOCOL_H
#define _ANALYZER_PROTOCOL_H

#include "DevProtocol.h"

/************************************************************************************/
// Pressure Function - Commands & Data Structures
/************************************************************************************/

// Pressure Function - Command (Get) Enums 
// Max command size - 7 bits
typedef enum {
  //Get commands
  CMD_FN_GET_PRESSURE = 40, // Get pressure (in millivolts)
} PRESSURE_CMD_ENUM;

// Structure to be used for configuring (Getting) Pressure readings.
typedef struct {
  CmdAckUnion         stCmd;
  unsigned long       ulPressureMilliVolts; // Data
} __attribute (( packed )) CAN_PRESSURE_DATA_STRUCT;

/************************************************************************************/
// Control Function - Commands & Data Structures
/************************************************************************************/

// Control Function - Command (Set / Get) Enums.
// Max command size - 7 bits
typedef enum {
  // Set commands
  CMD_FN_CTRL_SET_CH_STATE = 0,   // Set the channel state to ON/OFF.
  CMD_FN_CTRL_GET_CH_STATE = 40,  // Get the channel state to ON/OFF.
} CTRL_CMD_ENUM;


//Control I/O - ON/OFF enums
typedef enum {
  MIN_CTRL_STATE,
  CTRL_STATE_OFF = MIN_CTRL_STATE,
  CTRL_STATE_ON,
  MAX_CTRL_STATE,
} CTRL_STATE;

// Structure to be used for configuring (Setting) Control settings.
typedef struct {
  CmdAckUnion     stCmd;
  CTRL_STATE      eCtrlState;               // Data
} __attribute (( packed )) CAN_CTRL_DATA_STRUCT;


#endif //#ifdef _ANALYZER_PROTOCOL_H



