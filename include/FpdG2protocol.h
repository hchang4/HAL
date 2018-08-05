/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FpdG2protocol.h
 *
 *  Description: FPD G2 protocol code & data structures
 *
 *  Copyright:        Copyright (c) 2016-
 *                    Rosemount Analytical
 *                    All Rights Reserved
 *
 *  Operating System:  Linux
 *  Language:          C++
 *  Target:            Gas Chromatograph Model GC700/1500/1500XA R2
 *
 *  Revision History:
 *  - Initial version
 *************************************************************************/

#ifndef _FPD_G2_PROTOCOL_H_
#define _FPD_G2_PROTOCOL_H_

#include "DevProtocol.h"

#define MIN_FPD_G2_IGN_TRIES       1
#define MAX_FPD_G2_IGN_TRIES       5

#define MIN_FPD_G2_WAIT_TIME       10
#define MAX_FPD_G2_WAIT_TIME       120

#define MIN_FPD_G2_IGN_ON_TIME     1
#define MAX_FPD_G2_IGN_ON_TIME     7

#define MIN_FPD_G2_FLAME_SENSE     50
#define MAX_FPD_G2_FLAME_SENSE     150

// Flame status enum
typedef enum
{
  MIN_FPD_G2_FLAME_STATUS = 1,
  FPD_G2_FLAME_STATUS_ON = MIN_FPD_G2_FLAME_STATUS,
  FPD_G2_FLAME_STATUS_OFF,
  FPD_G2_FLAME_STATUS_OVERTEMP,
  MAX_FPD_G2_FLAME_STATUS
} FPD_G2_FLAME_STATUS;

// Ignition mode enum
typedef enum
{
  MIN_FPD_G2_IGN_MODE = 1,
  FPD_G2_IGN_MODE_MANUAL = MIN_FPD_G2_IGN_MODE,
  FPD_G2_IGN_MODE_AUTO,
  MAX_FPD_G2_IGN_MODE
} FPD_G2_IGN_MODE;

typedef enum
{
  MIN_FPD_G2_IGN_STAT = 0,
  FPD_G2_IGN_OFF = MIN_FPD_G2_IGN_STAT,
  FPD_G2_IGN_IGNITER_ON,
  FPD_G2_IGN_IGNITER_OFF,
  MAX_FPD_G2_IGN_STAT
} FPD_G2_IGN_STATUS;

// FPD G2 Function - Command (Set / Get) Enums (max command size = 7 bits)
typedef enum
{
  // Set commands
  CMD_FPD_G2_FN_SET_TEC_SETPOINT = 0,     // Set TEC (cooler) setpoint
  CMD_FPD_G2_FN_SET_IGN_MODE,             // Set ignition mode
  CMD_FPD_G2_FN_SET_IGN_TRIES,            // Set number of ignition attempts
  CMD_FPD_G2_FN_SET_WAIT_TIME,            // Set wait time between ignition attempts
  CMD_FPD_G2_FN_SET_IGN_DURATION,         // Set igniter "on" duration
  CMD_FPD_G2_FN_SET_FLAME_ON_TEMP,        // Set flame on sense temperature
  CMD_FPD_G2_FN_SET_FLAME_OFF_TEMP,       // Set flame off sense temperature
  CMD_FPD_G2_FN_START_IGNITION,           // Start flame ignition sequence
  CMD_FPD_G2_FN_STOP_IGNITION,            // Stop flame ignition sequence

  // Get commands
  CMD_FPD_G2_FN_GET_TEC_SETPOINT = 40,    // Get TEC (cooler) setpoint
  CMD_FPD_G2_FN_GET_IGN_MODE,             // Get ignition mode
  CMD_FPD_G2_FN_GET_IGN_TRIES,            // Get number of ignition attempts
  CMD_FPD_G2_FN_GET_WAIT_TIME,            // Get wait time between ignition attempts
  CMD_FPD_G2_FN_GET_IGN_DURATION,         // Get igniter "on" duration
  CMD_FPD_G2_FN_GET_FLAME_ON_TEMP,        // Get flame on sense temperature
  CMD_FPD_G2_FN_GET_FLAME_OFF_TEMP,       // Get flame off sense temperature
  CMD_FPD_G2_FN_GET_FLAME_TEMP,           // Get flame temperature
  CMD_FPD_G2_FN_GET_FLAME_STATUS,         // Get flame status
  CMD_FPD_G2_FN_GET_TEC_TEMP,             // Get TEC (cooler) temperature
  CMD_FPD_G2_FN_GET_IGN_STATUS            // Get igniter status
} FPD_G2_CMD_ENUM;

// Get / Set data structure
typedef struct
{
  union
  {
    unsigned char       ErrorType;      // error code
    long                tecTemp;        // TEC (cooler) temperature (degC)
    FPD_G2_IGN_MODE     ignMode;        // ignition mode (manual or auto)
    long                ignTries;       // number of ignition attempts (1 - 5)
    long                waitTime;       // wait time between ignition attempts (10 - 120 sec)
    long                ignDur;         // igniter "on" duration (1 - 7 sec)
    long                flameOnTemp;    // flame on sense temperature (50 - 150 degC)
    long                flameOffTemp;   // flame off sense temperature (50 - 150 degC)
    long                flameTemp;      // flame temperature (degC)
    FPD_G2_FLAME_STATUS flameStatus;    // flame status
    FPD_G2_IGN_STATUS   ignStatus;      // ignition status
  } __attribute (( packed )) fpdG2Data;
} __attribute (( packed )) FPD_G2_DATA_STRUCT;

// Structure to be used for configuring (Setting / Getting) Preamp settings
typedef struct
{
  CmdAckUnion         stCmd;
  FPD_G2_DATA_STRUCT  stData;
} __attribute (( packed )) CAN_CMD_FPD_G2_DATA_STRUCT;

// Structure to be used for Sending Status / Nack Error code to the Host
typedef struct
{
  CmdAckUnion   stCmd;
  STATUS_STRUCT stData;   // defined in DevProtocol.h
} __attribute (( packed )) CAN_CMD_FPD_G2_STATUS_STRUCT;

#endif  //#ifndef _FPD_G2_PROTOCOL_H_
