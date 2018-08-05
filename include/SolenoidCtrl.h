/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: SolenoidCtrl.h
 * *
 * *  Description: Solenoid device function access layer
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
 * *  $Id: SolenoidCtrl.h,v 1.7 2015/04/30 09:32:33 EMRSN\Dkad Exp $
 * *  $Log: SolenoidCtrl.h,v $
 * *  Revision 1.7  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.6  2014/06/26 10:01:19  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.5  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.4  2009/02/24 17:13:42  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.3  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/




#ifndef _SOLENOID_CTRL_H
#define _SOLENOID_CTRL_H

#include "BaseDev.h"

// Serial Port
class CSolenoidCtrl : public CBaseDev {

private:

public:
  CSolenoidCtrl();  // Default Constructor
  
  ~CSolenoidCtrl(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Get Device Status
  int GetStatus (int* pnStatus);      // Pointer to write status to

  int GetBoardInfo (unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
                    unsigned char* byFirmwareVerMin,  // Firmware Version - Minor Number
                    unsigned char* byFirmwareVerBuild,// Firmware Version - Build Number
                    unsigned char* byBoardRevision);  // Board Revision

  // Enable Solenoid Valve.
  int EnableSolValve ();  

  // Disable Solenoid Valve.
  int DisableSolValve (); 
  
  // Switch Solenoid Valve ON. 
  int TurnONSolValve ();
  
  // Switch Solenoid Valve OFF. 
  int TurnOFFSolValve ();

  // Gets total solenoid current in Micro Amperes.
  int GetSolTotalCurrent (int* TotCurrentMicroAmps);

  // Get the on-board temp in Milli Degree C.
  int GetOnBoardTemp (int* TempMilliDegC);
};

#endif // #ifndef _SOLENOID_CTRL_H
