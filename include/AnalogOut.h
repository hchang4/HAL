/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: AnalogOut.h
 * *
 * *  Description: Analog OUT device function access layer.
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
 * *  $Id: AnalogOut.h,v 1.11 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 * *  $Log: AnalogOut.h,v $
 * *  Revision 1.11  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.10  2015/04/30 09:32:30  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.9  2015/03/13 09:53:52  EMRSN\sboyidi
 * *  Bug 8188 - Need to restrict maximum ROC AO Output Current to 20.94 mA.
 * *  Fix : Added macro to restrict maximum ROC AO Output Current to 20.94 mA.
 * *
 * *  Revision 1.8  2014/12/02 11:37:41  emrsn\jchekkala
 * *  Selectable Loop Power Feature will be supported for G2 BaseIO Board.
 * *  In case of G1 BaseIO board, selectable loop power feature will be disabled.
 * *
 * *  Revision 1.7  2014/06/26 10:01:08  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.6  2012/03/21 06:49:02  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.5  2010/05/06 23:25:28  EMRSN\RRTHOMAS
 * *  Added support for ROC Analog Output card on ROC Expansion Slot 2.
 * *
 * *  Revision 1.4  2009/02/24 17:13:41  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.3  2008/11/16 00:41:07  epm-na\rthomas
 * *  Added macros computing the scaled output for ROC Analog Output cards. Added variables to store scaled values for all the Analog Output channels. This is required to overcome the limitation that we cannot write to a single channel at a time, we have to write the scaled output values to all the channels.
 * *
 * *  Revision 1.2  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#ifndef _ANALOG_OUT_H
#define _ANALOG_OUT_H

#include "BaseDev.h"
#include "BaseIOProtocol.h"

#define NR_ENUM_ROC_ANALOG_OUT   4 // Number of Analog Outputs in a ROC Analog output card.
/*
  ROC Analog output card 
  ----------------------
  Total current range - 20.96 milli Amps
  Total number of analog outputs per card - 4
  Each analog out can be controlled by sending it a 16 bit unsigned value from 0 to 65535
  Conversion formula - 
  Output = Desired current in mA * 2^16 / 20.96
*/
#define ROC_CONVERT_I_TO_LEVEL(x) (x * 65536 / 20.96) 
#define MAX_CURRENT_mA            (20.94f)

// Analog Output Device
class CAnalogOut : public CBaseDev
{
private:
  //HACK!!!!
  // Even if we want to update only one analog channel in a ROC card, we have to write to all analog output channels (because 
  // the values are SPI'ed into the ROC card). Hence, we have to store the values for every other analog channel 
  // in each object of CAnalogOut!  
  // How to handle 16 channels across four boards? 
  // The simplest solution for this is to store all 16 values (4 per card) and handle all 16 values in this class as 
  // a static array and in the "SetCurrent" function index into the array based on which expansion slot the board is connected.
  static unsigned short m_usCurrent[4 * NR_ENUM_ROC_ANALOG_OUT];
  int m_iBoardRevision;

public:
  CAnalogOut(); // Default Constructor
  
  ~CAnalogOut();  // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Set current in nano amps
  int SetCurrent(unsigned long CurrentNanoAmps);

  // Get Device Status
  int GetStatus (int* pnStatus);      // Pointer to write status to

  int GetBoardInfo (unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
                    unsigned char* byFirmwareVerMin,  // Firmware Version - Minor Number
                    unsigned char* byFirmwareVerBuild,// Firmware Version - Build Number
                    unsigned char* byBoardRevision);  // Board Revision

  // Gets the on-board temperature in Milli Degree C.
  int GetOnBoardTemp(int* TempMilliDegC);

  // Set the Loop Power Internal/External - Returns 0 on success, negative error code on failure.
  int SetLoopPower( BASEIO_ANLOUT_PWR_ENUM enumLoopPower );

  // Return True if BaseIO G2 Board else False
  bool IsLoopPowerSelectable( void );

};

#endif // #ifndef _ANALOG_OUT_H
