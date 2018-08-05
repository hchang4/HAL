/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: ResolveDevName.h
 * *
 * *  Description: Device name resolution and generation APIs
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
 * *  $Id: ResolveDevName.h,v 1.6 2015/04/30 09:32:33 EMRSN\Dkad Exp $
 * *  $Log: ResolveDevName.h,v $
 * *  Revision 1.6  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.5  2014/06/26 10:01:18  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.4  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2011/07/13 10:52:52  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.2  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



// ResolveDevName.h - Header for class CResolveDevName
//

#ifndef _RESOLVE_DEV_NAME_H
#define _RESOLVE_DEV_NAME_H

#include "Definitions.h"

// Mapping of Fn Type to Fn Type Enum
struct DeviceTypeMapStruct {
  char szName[15];      // Function Name
  unsigned char byEnum; // Enum equivalent for Function Name  
};

// Mapping to Slot Name to Slot Address
struct DeviceAddrMapStruct {
  char szSlotName[20];    // Slot Name
  COMM_TYPE eCommType;    // Is this Slot on the CAN Bus or SPI Bus? TRUE - On CAN, FALSE - On SPI
  unsigned char bySlotID; // CAN Message Id / SPI Address
};

class CResolveDevName {
private:
  
  // Mapping of Device Fn Type to Fn Type Enums
  static const DeviceTypeMapStruct stDevTypeMap[MAX_NUM_DEV_FUNCTIONS];

  // Mapping of Slot ID Name to Device Address
  static const DeviceAddrMapStruct stSlotIDMap[MAX_NUM_SLOTS];

public:
  // Function takes in the Device name in "FnType:SlotID:FnEnum" format as input. 
  // Looks up each component of the device name and 
  // (1) Checks for errors in the Device name format
  // (2) Checks for invalid Fn Type, Slot ID and Fn Enum
  // (3) Returns Fn Type, Slot ID and Fn Enum as enumerated values
  // Function returns 0 on success, negative error code on failure
  int ResolveName (char* pszDevName, 
                   unsigned char* pbyFnType, 
                   unsigned char* pbySlotID, 
                   unsigned char* pbyFnEnum, 
                   COMM_TYPE* peCommType);

  // TODO:
  // Function takes in Fn Type, Slot ID and Fn Enum and writes 
  // the device name into pszDevName pointer.
  // Function returns 0 on success, negative error code on failure
  int GenerateName (char* pszDevName, 
                    unsigned char byFnType, 
                    unsigned char bySlotID, 
                    unsigned char byFnEnum); 
};

#endif //#ifndef _RESOLVE_DEV_NAME_H

