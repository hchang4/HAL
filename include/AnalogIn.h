/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: AnalogIn.h
 * *
 * *  Description: Analog Input device function access layer.
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
 * *  $Id: AnalogIn.h,v 1.8 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 * *  $Log: AnalogIn.h,v $
 * *  Revision 1.8  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.7  2015/04/30 09:32:30  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.6  2014/06/26 10:01:08  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.5  2012/03/21 06:49:02  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.4  2011/02/15 15:39:57  emrsn\rsaha
 * *  Revised to read ROCIO 16bit Analog Inputs.
 * *
 * *  Revision 1.3  2009/02/24 17:13:41  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.2  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#ifndef _ANALOG_IN_H
#define _ANALOG_IN_H

#include "BaseDev.h"
#include "BaseIOProtocol.h"

// Digital Input Device
class CAnalogIn : public CBaseDev
{

private:
  static bool m_bROCIO1_InitDone;
  static bool m_bROCIO2_InitDone;
  static bool m_bROCIO3_InitDone;
  static bool m_bROCIO4_InitDone;
  static int m_nROCIOAICount[16];

  // Initialize ROCIO card
  int InitROCIO();

  // Calculate CRC value for responses comming from RCOIO
  void CalcCRC(const int *nDataPtr, int nNumBytes, char &byCRC1, char &byCRC2);

  // Read ROCIO AIs
  int GetROCIOReading();

public:
  CAnalogIn();  // Default Constructor
  
  ~CAnalogIn(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal(char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal();

  // Get current in nano amps
  int GetCurrent(unsigned long *pCurrentNanoAmps);
  
  // Get voltage in nano amps
  int GetVoltage(unsigned long *pMilliVolts);

  // Get Device Status
  int GetStatus(int* pnStatus);      // Pointer to write status to

  int GetBoardInfo(unsigned char* byFirmwareVerMaj,    // Firmware Version - Major Number
                   unsigned char* byFirmwareVerMin,    // Firmware Version - Minor Number
                   unsigned char* byFirmwareVerBuild,  // Firmware Version - Minor Number
                   unsigned char* byBoardRevision);    // Board Revision

  // Gets the on-board temperature in Milli Degree C
  int GetOnBoardTemp(int* TempMilliDegC);
};

#endif // #ifndef _ANALOG_IN_H
