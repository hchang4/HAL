/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: Pressure.h
 *
 *  Description: Pressure sensors access layer.
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
 *  $Id: Pressure.h,v 1.4 2015/04/30 09:32:33 EMRSN\Dkad Exp $
 *  $Log: Pressure.h,v $
 *  Revision 1.4  2015/04/30 09:32:33  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.3  2014/06/26 10:01:17  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.2  2012/03/21 06:49:08  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2010/06/10 20:10:01  emrsn\rsaha
 *  Added HAL interfaces for Pressure read, Serial RS232/485 control support and diagnostics details of Analyzer IO card.
 *
 *************************************************************************/


#ifndef _PRESSURE_H
#define _PRESSURE_H

#include "BaseDev.h"
#include "AnalyzerProtocol.h"

// Pressure Sensor
class CPressure : public CBaseDev
{
private:

public:
  CPressure();  // Default Constructor

  ~CPressure(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Gets the Pressure in Milli Volts
  int GetPressure (unsigned long* PressureMilliVolt);
};

#endif // #ifndef _PRESSURE_H



