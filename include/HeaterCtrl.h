/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: HeaterCtrl.h
 * *
 * *  Description: Heater control device function access layer.
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
 * *  $Id: HeaterCtrl.h,v 1.11 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 * *  $Log: HeaterCtrl.h,v $
 * *  Revision 1.11  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.10  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.9  2014/06/26 10:01:14  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.8  2012/03/21 06:49:06  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.7  2010/10/14 18:01:20  emrsn\rsaha
 * *  Added support for heater temperature compensation/correction.
 * *  APIs added to Set, Get and Mark Compensation Base Temperature at HeaterSolenoid IO board.
 * *  APIs added to Set and Get Compensation Temperature slope at HeaterSolenoid IO board.
 * *
 * *  Revision 1.6  2009/09/11 14:36:56  epm-na\rsaha
 * *  Added new command to set Heater type as  - AC or DC, in Heater Control Function.
 * *
 * *  Revision 1.5  2009/02/24 17:13:42  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.4  2007/03/13 08:33:35  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *
 * *  Revision 1.3  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#ifndef _HEATER_CTRL_H
#define _HEATER_CTRL_H

#include "BaseDev.h"  // For CBaseDev
#include "HtrSolProtocol.h" // For Heater / Solenoid protocol-specific definitions / enums / structures...

// Heater Control
class CHeaterCtrl : public CBaseDev
{
public:
  enum HEATER_TYPE
  {
    AC_HEATER = 0,
    DC_HEATER = 1
  };

  CHeaterCtrl();  // Default Constructor
  
  ~CHeaterCtrl(); // Destructor

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

  // Set Channel Temp in milli degree C
  int SetChTempMilliDegC (int nTempMilliDegC);
  
  // Set Heater Channel PWM in milli %
  int SetChPWMMilliP (int nTempMilliP);
  
  int TurnHtrOff (); // Turn Heater OFF

  // Set the proportional gain (milli)
  int SetChPropGain(unsigned long ulPropGainMilli);

  // Set the integral gain (milli)
  int SetChIntGain(unsigned long ulIntGainMilli);

  // Set the differential gain (direct value)
  int SetChDiffGain(unsigned long ulDiffGain);
  
  // Get Heater PWM in milli %
  int GetHtrChPWMMilliP (unsigned int* pnPWMMilliP);

  // Get heater current in Micro Amps.
  int GetHtrCurrent (int* pnHtrCurrentMicroAmps);
  
  // Get Temp in milli degree C
  int GetHtrChTempMilliDegC (int* pnTempMilliDegC); 

  // Get Sensor Status (OK, ERR)
  int GetRTDStatus (int* pnStatus);

  // Set the proportional gain (milli)
  int GetOnBoardTemp (int* pnTempMilliDegC);
  
  // Get the on-board temp in Milli Degree C.
  int GetChPropGain(unsigned long *pulPropGainMilli);

  // Set the integral gain (milli)
  int GetChIntGain(unsigned long *pulIntGainMilli);

  // Set the differential gain (direct value)
  int GetChDiffGain(unsigned long *pulDiffGain);

  // Set Heater type - AC or DC
  int SetHeaterType( HEATER_TYPE eHtrType );

  // Get Heater type - AC or DC
  int GetHeaterType( HEATER_TYPE * eHtrType );

  // Set Compensation Base Temperature.
  int SetCompBaseTemp( int nCompBaseTempMilliDegC );

  // Get Compensation Base Temperature.
  int GetCompBaseTemp( int * p_nCompBaseTempMilliDegC );

  // Mark Compensation Base Temperature.
  int MarkCompBaseTemp();

  // Set Compensation Temperature Slope.
  int SetCompTempSlope( float fCompTempSlope );

  // Get Compensation Temperature Slope.
  int GetCompTempSlope( float * p_fCompTempSlope );

  // Set RTD lead resistor value
  int SetRTDleadR(unsigned char rtdNdx, float fValue);
};

#endif // #ifndef _HEATER_CTRL_H
