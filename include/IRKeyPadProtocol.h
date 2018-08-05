/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: IRKeyPadProtocol.h
 *
 *  Description: IR Keypad protocol definitions.
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
 *  $Id: IRKeyPadProtocol.h,v 1.4 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 *  $Log: IRKeyPadProtocol.h,v $
 *  Revision 1.4  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.3  2014/06/26 10:01:16  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.2  2012/03/21 06:49:07  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2009/06/25 18:59:54  epm-na\rsaha
 *  Added interfaces in HAL layer to set/get/save/restore IR key sensitivity settings.
 *
 ***********************************************************************/

#ifndef _IR_KEYPAD_PROTOCOL_H
#define _IR_KEYPAD_PROTOCOL_H

#include "DevProtocol.h"

// IRKeyPad Function - Command (Set / Get) Enums 
// Max command size - 7 bits
typedef enum {
   
  // Set commands
  CMD_IRKEYPAD_FN_SET_INDIVIDUAL_KEY_SENSITIVITY = 0,     // Sets individual IR Key sensitivity.    
  CMD_IRKEYPAD_FN_SET_ALL_KEY_SENSITIVITY = 1,            // Sets all IR Key sensitivity.
  CMD_IRKEYPAD_FN_SAVE_ALL_KEY_SENSITIVITY = 2,           // Saves Key sesitivity information to memory
    
  //Get commands
  CMD_IRKEYPAD_FN_GET_INDIVIDUAL_KEY_SENSITIVITY = 40,    // Returns present sensitivity of a individual key.
  CMD_IRKEYPAD_FN_GET_ALL_KEY_SENSITIVITY = 41,           // Returns present sensitivity of all the keys.
  CMD_IRKEYPAD_FN_LOAD_PREVIOUS_KEY_SENSITIVITY = 42,     // Loads default Key sensitivity.
  CMD_IRKEYPAD_FN_RESTORE_DEFAULT_KEY_SENSITIVITY = 43,   // Loads default Key sensitivity.

}IRKEY_CMD_ENUM;


typedef struct {
  unsigned char   KeySensitivity;
  unsigned char   KeyNum;
}__attribute (( packed )) SINGLE_IRKEY_DATA_STRUCT;

// Structure to be used for Setting / Getting IRKey sensitivity setting.
typedef struct {
  CmdAckUnion               stCmd;
  SINGLE_IRKEY_DATA_STRUCT  stData;
}__attribute (( packed )) CAN_SINGLE_IRKEY_STRUCT;

// Structure to be used for Setting / Getting all the IRKey sensitivity settings.
typedef struct {
  CmdAckUnion   stCmd;
  unsigned char arrKeySensitivity[8];
}__attribute (( packed )) CAN_ALL_IRKEY_STRUCT;

#endif //#ifndef _IR_KEYPAD_PROTOCOL_H

