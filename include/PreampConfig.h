/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampConfig.h
 * *
 * *  Description: Preamp device function configuration access layer.
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
 * *  $Id: PreampConfig.h,v 1.14 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 * *  $Log: PreampConfig.h,v $
 * *  Revision 1.14  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.13  2014/12/02 11:38:33  emrsn\jchekkala
 * *  Manual Base Line Adjustment for Preamp G2 Boards will be disabled and the same will be enabled for Preamp G1 Boards.
 * *
 * *  Revision 1.12  2014/06/26 10:01:16  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.11  2012/09/24 03:55:07  emrsn\rsaha
 * *  Revised to Enable/Disable 700XA Preamp IO AutoZero algorithm. Previously only Enable support was there in HAL.
 * *
 * *  Revision 1.10  2012/09/07 12:49:23  emrsn\rsaha
 * *  Revised with support to Enable preamp AutoZero and to read bridge left and right detector status.
 * *
 * *  Revision 1.9  2012/03/21 06:49:08  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.8  2009/02/24 17:13:42  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.7  2008/11/17 22:12:26  epm-na\rsaha
 * *  Updated protocol to retrive low to high sub channel scaling factor of a preamp.
 * *
 * *  Revision 1.6  2008/05/22 06:33:02  EPM-NA\kdarji
 * *  - Added timeout parameter in all the methods which are used to read and write
 * *  from/in-to preamp board. Client application i.e. xpdetectord will tell the
 * *  amount of time, to wait for response from preamp board.
 * *  [Note: In case of client application will not provide timeout parameter,
 * *  then default timeout time is as it was previously i.e. HAL_DFLT_TIMEOUT
 * *  (300 MilliSeconds)]
 * *
 * *  Revision 1.5  2007/04/26 08:56:42  epm-na\rthomas
 * *   - Added functions to read CAL status (in progress or not) through
 * *     either premap config or streaming objects.
 * *
 * *  Revision 1.4  2007/04/25 10:59:39  epm-na\rthomas
 * *   - Added ability to enable self calibration through preamp stream and
 * *     config objects.
 * *
 * *  Revision 1.3  2007/03/13 08:33:35  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *
 * *  Revision 1.2  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



#ifndef _PREAMPCONFIG_H
#define _PREAMPCONFIG_H

#include "BaseDev.h"
#include "PreampProtocol.h"

// Bridge Preamplifier
class CPreampConfig : public CBaseDev {

private:
  int m_iBoardRevision;
public:
  CPreampConfig();  // Default Constructor
  
  ~CPreampConfig(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Get Device Status
  int GetStatus (int* pnStatus);      // Pointer to write status to

  int GetBoardInfo (unsigned char* byFirmwareVerMaj,      // Firmware Version - Major Number
                    unsigned char* byFirmwareVerMin,      // Firmware Version - Minor Number
                    unsigned char* byFirmwareVerBuild,    // Firmware Version - Build Number
                    unsigned char* byBoardRevision,       // Board Revision
                    unsigned int unTimeOut = HAL_DFLT_TIMEOUT);   // Time to wait for response from remote device

  // Get Bridge current in Micro Amperes.
  int GetBridgeCurrent (int* BridgeCurrentMicroAmps,
                        unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Gets the Bridge gain adjustment.
  int GetBridgeGain (PREAMP_BRIDGE_GAIN_ENUM *eBridgeGain,
                     unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device
  
  // Sets the Bridge gain adjustment.
  int SetBridgeGain (PREAMP_BRIDGE_GAIN_ENUM eBridgeGain,
                     unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Gets the Sampling Rate
  int GetSamplingRate (PREAMP_SAMPLING_RATE_ENUM *eSamplingRate,
                       unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Sets the Sampling Rate
  int SetSamplingRate (PREAMP_SAMPLING_RATE_ENUM eSamplingRate,
                       unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device
  
  // Get the current status of the AC bridge setting
  int GetACBridgeStatus(PREAMP_STATE_ENUM *eACBridgeStatus,
                        unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device
  
  // Turn ON/OFF the AC Bridge
  int SetACBridgeStatus(PREAMP_STATE_ENUM eACBridgeStatus,
                        unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Get filter ON/OFF status
  int GetFilterStatus(PREAMP_STATE_ENUM *eFilterStatus,
                      unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device
  
  // Turn ON/OFF filter
  int SetFilterStatus(PREAMP_STATE_ENUM eFilterStatus, 
                      unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Turn on moving avg.
  int GetMovingAvgStatus(PREAMP_STATE_ENUM *eMovAvgStatus,
                         unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device
  
  // Get moving avg. ON/OFF status
  int SetMovingAvgStatus(PREAMP_STATE_ENUM eMovAvgStatus, 
                         unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Get the bridge offset value for the specified offset type
  int GetBridgeOffset(PREAMP_OFFSET_TYPE eOffsetType, 
                      PREAMP_BRIDGE_ADJ_ENUM *eOffsetValue,
                      unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device
  
  // Set the bridge offset value for the specified offset type
  int SetBridgeOffset(PREAMP_OFFSET_TYPE eOffsetType,
                      PREAMP_BRIDGE_ADJ_ENUM eOffsetValue,
                      unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device
  
  // Gets the on-board temperature in Milli Degree C.
  int GetOnBoardTemp (int* TempMilliDegC, 
                      unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Gets the current current preamp value.
  int GetPreampValue (long *PreampValue,
                      unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Gets the scaling factor between low and high gain sub channels.
  int GetScalingFactor(float * ScalingFactor,
                       unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Enable self calibration
  int EnableSelfCalibration(unsigned int unTimeOut = HAL_DFLT_TIMEOUT); //Time to wait for response from remote device

  // Enable /Disable Auto Zero
  int SetAutoZero(bool bEnable, 
                  unsigned int unTimeOut = HAL_DFLT_TIMEOUT); //Time to wait for response from remote device

  // Read calibration status (CAN in progress or not)
  int GetCalOnOffStatus(PREAMP_STATE_ENUM *eCalInProgress,
                        unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Read Detector Bridge Right status
  int GetBRRightDetStatus(PREAMP_STATE_ENUM *eDetStatus,
                          unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Read Detector Bridge Left status
  int GetBRLeftDetStatus(PREAMP_STATE_ENUM *eDetStatus,
                         unsigned int unTimeOut = HAL_DFLT_TIMEOUT); // Time to wait for response from remote device

  // Flushes the contents of the Named Pipe that is used for streaming data for this device.
  int Flush();

  // Returns FALSE if G2 Preamp Board else TRUE. Device to be opened before this call.
  bool IsBaseLineAdjustable( void );
    
};

#endif // #ifndef _PREAMPCONFIG_H


