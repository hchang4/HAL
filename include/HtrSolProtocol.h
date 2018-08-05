/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: HtrSolProtocol.h
 * *
 * *  Description: Heater Solenoid RTD protocol code & data structures
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
 * *  $Id: HtrSolProtocol.h,v 1.11 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 * *  $Log: HtrSolProtocol.h,v $
 * *  Revision 1.11  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.10  2016/01/08 20:32:58  emrsn\rrthomas
 * *  Add ability to read PWM using the RTD enum for debugging purposes.
 * *
 * *  Revision 1.9  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.8  2014/06/26 10:01:14  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.7  2012/03/21 06:49:06  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.6  2010/10/14 18:01:20  emrsn\rsaha
 * *  Added support for heater temperature compensation/correction.
 * *  APIs added to Set, Get and Mark Compensation Base Temperature at HeaterSolenoid IO board.
 * *  APIs added to Set and Get Compensation Temperature slope at HeaterSolenoid IO board.
 * *
 * *  Revision 1.5  2009/09/11 14:36:56  epm-na\rsaha
 * *  Added new command to set Heater type as  - AC or DC, in Heater Control Function.
 * *
 * *  Revision 1.4  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#ifndef _HTR_SOL_PROTOCOL_H
#define _HTR_SOL_PROTOCOL_H

#include "DevProtocol.h"

#define HTR_SOL_FN_COUNT   3  // Heater, Solenoid, RTD
#define HTR_FN_CNT         4  // 4 Heater channels on this board
#define SOL_FN_CNT         12 // TODO: xx Solenoid Valves on this board
#define RTD_FN_CNT         5  // 5 RTD sensors on this board

// Board Capability / Identity Structure
/************************************************************************************/
// Common structures
/************************************************************************************/
typedef struct
{
  union
  {
    unsigned long PWMmPct;      // PWM setting in milli %- MUST be 0 for off cmd!!
    long    TempmDegC;          // Temp in Milli-degree C
    long    OnBdTempmDegC;      // On-board temperature
    long    HtrCurrent;         // Heater current
    unsigned char ErrorType;    // Error code
    unsigned long PropGain;     // Proportional gain in milli
    unsigned long IntGain;      // Integral gain in milli
    unsigned long DiffGain;     // Differential gain (direct value)
    unsigned long HtrType;      // Heater type - AC or DC
    unsigned long CompBaseTemp; // Compensation base temperature.
    float   CompTempSlope;      // Compensation temperature slope.
    float   LeadResistance;     // RTD circuit lead resistor value (RTDs 1-5, first slot, 1500XA R2 only)
  } __attribute (( packed )) htrsolDataUnion;
}
#ifndef WIN32
  __attribute (( packed )) 
#endif 
    HTR_SOL_RTD_DATA_STRUCT;

// Structure to be used for getting board information
typedef struct
{
  CmdAckUnion     stCmd;            // Command
  DEVICE_SYSTEM_INFO_STRUCT stData; // Data
} __attribute (( packed )) CAN_CMD_HTR_SOL_RTD_SYSINFO_STRUCT;

/************************************************************************************/
// Heater Function - Commands & Data Structures
/************************************************************************************/

// Heater Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum
{
  // Set commands
  CMD_HTR_FN_SET_HTR_CTRL_ON_PID  = 0,  // Requires a setpoint in milli DegC. PID then takes over
  CMD_HTR_FN_SET_HTR_CTRL_ON_PWM  = 1,  // Requires a setpoint in milli %. NO PID CONTROL. IT IS SWITCHED OFF. Expected to be remote PID!!
  CMD_HTR_FN_SET_HTR_CTRL_OFF = 2,  // Turns off selected heater
  CMD_HTR_FN_SET_PROP_GAIN  = 3,  // Set proportional gain in milli
  CMD_HTR_FN_SET_INT_GAIN   = 4,  // Set integral gain in milli
  CMD_HTR_FN_SET_DIFF_GAIN  = 5,  // Set differential gain (direct value)
  CMD_HTR_FN_SET_HTR_TYPE   = 6,  // Set Heater type - AC or DC
  CMD_HTR_FN_SET_COMP_BASE_TEMP = 7,  // Set temperature compensation base value.
  CMD_HTR_FN_MARK_COMP_BASE_TEMP  = 8,  // Mark current board temperature as temperature compensation base value.
  CMD_HTR_FN_SET_COMP_TEMP_SLOPE  = 9,  // Set temperature compensation slope.
  CMD_HTR_FN_SET_RTD_LEAD_RESIST  = 10,

  //Get commands
  CMD_HTR_FN_GET_TEMP_ERR_STATUS  = 40, // Return the ok/err status of the requested RTD channel
  CMD_HTR_FN_GET_HTR_PWM    = 41, // Get the current value of the PWM on requested htr channel
  CMD_HTR_FN_GET_TEMP_MDEG  = 42, // Return the current temperature of the requested RTD channel in milli Degs
  CMD_HTR_FN_GET_HTR_ERR_STATUS = 43, // Return the ok/err status of the requested htr channel
  CMD_HTR_FN_GET_HTR_TOT_CURRENT  = 44, // Return the total heater current
  CMD_HTR_FN_GET_ON_BD_TEMP = 45, // Get the onboard temperature reading
  CMD_HTR_FN_GET_PROP_GAIN  = 46, // Get proportional gain in milli
  CMD_HTR_FN_GET_INT_GAIN   = 47, // Get integral gain in milli
  CMD_HTR_FN_GET_DIFF_GAIN  = 48, // Get differential gain (direct value)
  CMD_HTR_FN_GET_HTR_TYPE   = 49, // Get Heater type - AC or DC
  CMD_HTR_FN_GET_COMP_BASE_TEMP = 50, // Get temperature compensation base value.
  CMD_HTR_FN_GET_COMP_TEMP_SLOPE  = 51, // Get temperature compensation slope.
} HTR_CMD_ENUM;

// Structure to be used for Setting / Getting Htr PWM / PID Values, Getting On-board Temp
typedef struct
{
  CmdAckUnion   stCmd;
  HTR_SOL_RTD_DATA_STRUCT stData; // Data
} 
#ifndef WIN32
  __attribute (( packed )) 
#endif 
    CAN_CMD_HTR_STRUCT;

// Structure to be used for Sending Status / Nack Error code to the Host
typedef struct
{
  CmdAckUnion stCmd;
  STATUS_STRUCT stData; // Status
} 
#ifndef WIN32
  __attribute (( packed )) 
#endif 
    CAN_CMD_HTR_STATUS_STRUCT;

/************************************************************************************/
// Solenoid Valve Function - Commands & Data Structures
/************************************************************************************/

// Solenoid Valve Function - Command (Set / Get) Enums
// Max command size - 7 bits
typedef enum
{
  // Set commands
  CMD_SOL_FN_TURN_SOL_ON = 0,       // Turn ON Solenoid Valve
  CMD_SOL_FN_TURN_SOL_OFF = 1,      // Turn OFF Solenoid Valve

  // Get commands
  CMD_SOL_FN_GET_SOL_ERR_STATUS = 40,   // Return the ok/err status of the solenoid
  CMD_SOL_FN_GET_ON_BD_TEMP = 41,       // Get the onboard temperature reading
  CMD_SOL_FN_GET_SOL_TOT_CURRENT = 42   // Get the total Solenoid Current
} SOL_CMD_ENUM;

// Structure to be used for Getting On-board Temp
typedef struct
{
  CmdAckUnion       stCmd;         // Command 
  HTR_SOL_RTD_DATA_STRUCT stData;  // Data
} 
#ifndef WIN32
  __attribute (( packed )) 
#endif 
    CAN_CMD_SOL_STRUCT;

// Structure to be used for Sending Status / Nack Error code to the Host
typedef struct
{
  CmdAckUnion     stCmd;    // Command 
  STATUS_STRUCT   stData;   // Status
} 
#ifndef WIN32
  __attribute (( packed )) 
#endif 
    CMD_SOL_GET_STATUS_STRUCT;

/************************************************************************************/
// RTD Function - Commands & Data Structures
/************************************************************************************/

// RTD Function - Command (Set / Get) Enums
// Max command size - 7 bits
typedef enum
{
  // Set commands

  // Get commands
  CMD_RTD_FN_GET_RTD_ERR_STATUS = 40,   // Get the error status of the RTD
  CMD_RTD_FN_GET_RTD_TEMP_MDEG = 41,    // Get the temperature in milli Degs
  CMD_RTD_FN_GET_HTR_PWM_MPERCENT = 42, // HACK: Get the PWM in millipercent for Heater Channel 1 - 4
} RTD_CMD_ENUM;

#endif // #ifndef _HTR_SOL_PROTOCOL_H
