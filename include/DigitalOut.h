/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: DigitalOut.h
 * *
 * *  Description: Digital Output device function access layer.
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
 * *  $Id: DigitalOut.h,v 1.9 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 * *  $Log: DigitalOut.h,v $
 * *  Revision 1.9  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.8  2015/04/30 09:32:31  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.7  2014/06/26 10:01:11  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.6  2012/03/21 06:49:04  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.5  2011/02/19 12:48:58  emrsn\rsaha
 * *  Added support for ROCIO DIs and DOs for both expansion slot 1 and 2.
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

#ifndef _DIGITAL_OUT_H
#define _DIGITAL_OUT_H

#include "BaseDev.h"
#include "BaseIOProtocol.h"

#define NR_ENUM_ROC_DIGITAL_OUT 5

// Digital Input Device
class CDigitalOut : public CBaseDev
{
private:
  static bool m_bROCIO_DOState[4 * NR_ENUM_ROC_DIGITAL_OUT];

public:
  CDigitalOut();  // Default Constructor
  
  ~CDigitalOut(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Get the DigitalIn status - Returns 0 on success, negative error code on failure.
  int SetDigitalOutStatus(BOOL bData);    // Output line status

  // Get Device Status
  int GetStatus (int* pnStatus);      // Pointer to write status to

  int GetBoardInfo (unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
                    unsigned char* byFirmwareVerMin,  // Firmware Version - Minor Number
                    unsigned char* byFirmwareVerBuild,// Firmware Version - Build Number
                    unsigned char* byBoardRevision);  // Board Revision

  // Gets the on-board temperature in Milli Degree C.
  int GetOnBoardTemp(int* TempMilliDegC);
};

#endif // #ifndef _DIGITAL_OUT_H
