/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: RTD.h
 * *
 * *  Description: RTD (temperature) device function access layer.
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
 * *  $Id: RTD.h,v 1.8 2016/01/08 20:32:58 emrsn\rrthomas Exp $
 * *  $Log: RTD.h,v $
 * *  Revision 1.8  2016/01/08 20:32:58  emrsn\rrthomas
 * *  Add ability to read PWM using the RTD enum for debugging purposes.
 * *
 * *  Revision 1.7  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.6  2014/06/26 10:01:17  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.5  2012/03/21 06:49:08  EMRSN\jchekkala
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



#ifndef _RTD_H
#define _RTD_H

#include "BaseDev.h"

// RTD
class CRTD : public CBaseDev {

private:

public:
  CRTD();   // Default Constructor
  
  ~CRTD();  // Destructor

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

  // Get temperature in Milli Degree C.
  int GetTempInMilliDegC (long* TempMilliDegC);
  
  // Get PWM of the Heater Channel associated with RTD in Milli-percent
  int GetPWMInMilliPercent (long* PWMMilliPercent);
};

#endif // #ifndef _RTD_H
