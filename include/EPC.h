/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: EPC.h
 * *
 * *  Description: EPC device function access layer.
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
 * *  $Id: EPC.h,v 1.10 2015/04/30 09:32:31 EMRSN\Dkad Exp $
 * *  $Log: EPC.h,v $
 * *  Revision 1.10  2015/04/30 09:32:31  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.9  2014/06/26 10:01:11  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.8  2013/10/08 15:09:48  emrsn\rrthomas
 * *  EPC Pressure Sensor Temperature correction made configurable using MON2020. Here is the correction formula (implemented in Analyzer IO firmware) -
 * *
 * *  Corrected_Carrier_Pressure = Carrier_Pressure  (Ambient_temperature  base_temperature) * pressure_correction
 * *
 * *  "Ambient_temperature" is read from RTD2 which is mounted on the IMB Board in close proximity to the Pressure Sensor.
 * *  "base_temperature" is available in EPC_CONFIG table.
 * *  "pressure_correction" is available in EPC_CONFIG table.
 * *
 * *  Revision 1.7  2012/03/21 06:49:04  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.6  2011/07/08 21:45:35  emrsn\rrthomas
 * *  Changed how "Diff Gain" is sent to the EPC function in the Analyzer Board. Earlier, D Gain had to be a integer, changed it so that we can setup D Gain as a float. Need a fractional D Gain for testing EPCs on the 370XA.
 * *
 * *  Revision 1.5  2011/06/01 08:19:19  emrsn\rsaha
 * *  Extended HAL EPC interfaces for 370XA.
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



#ifndef _EPC_H
#define _EPC_H

#include "BaseDev.h"
#include "BaseIOProtocol.h"

// Electronic Pressure Controller
class CEPC : public CBaseDev {

private:

public:
  CEPC(); // Default Constructor
  
  ~CEPC();  // Destructor

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

  // Gets the Pressure in Milli Volts
  int GetPressure (unsigned long* PressureMilliVolt);

  // Sets the Pressure in Milli Volts
  int SetPressure (unsigned long PressureMilliVolt);

  // Gets the on-board temperature in Milli Degree C.
  int GetOnBoardTemp(int* TempMilliDegC);

#ifdef MODEL_370XA
  int SetPWMMilliP (int nTempMilliP);
  int TurnEPCOff ();
  int GetPWMMilliP(unsigned int* pnPWMMilliP);
  int SetPropGain(unsigned long ulPropGainMilli);
  int SetIntGain(unsigned long ulIntGainMilli);
  int SetDiffGain(unsigned long ulDiffGainMilli);
  int GetPropGain(unsigned long *pulPropGainMilli);
  int GetIntGain(unsigned long *pulIntGainMilli);
  int GetDiffGain(unsigned long *pulDiffGainMilli);
  
  // Setup correction factors for Pressure Sensor temperature compensation (Pressure sensor on 
  // the 370XA is outside the oven and affected by ambient temperature changes).
  int SetPressCompBaseTemp(float fBaseTemp);
  int SetPressCompCorr(float fPressCorr);
#endif //#ifdef MODEL_370XA
};

#endif // #ifndef _EPC_H
