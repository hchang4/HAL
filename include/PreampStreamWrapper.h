/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampStreamWrapper.h
 * *
 * *  Description: Preamp device function streaming data access layer.
 * *
 * *  Copyright:        Copyright (c) 2011-2012, 
 * *                    Rosemount Analytical 
 * *                    All Rights Reserved.
 * *
 * *  Operating System:  None.
 * *  Language:          'C++'
 * *  Target:            Gas Chromatograph Model GC700XP
 * *
 * *
 * *
 * *************************************************************************/

#ifndef _PREAMPSTREAMWRAPPER_H
#define _PREAMPSTREAMWRAPPER_H

#include "PreampProtocol.h"
#include "PreampStream.h"
#include "PreampStreamSim.h"


class CPreampStreamWrapper {

private:
  bool m_bSimulate;
  
  CPreampStream m_oPreampStrmHW;
  CPreampStreamSim m_oPreampStrmSim; 

public:
  CPreampStreamWrapper();  // Default Constructor
  
  ~CPreampStreamWrapper(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Starts / Stops broadcast. Start = 1 starts broadcast, Start = 0 stops broadcast.
  int SetBroadcastMode (unsigned char Start, bool doInitOnly);

  // Starts / Stops broadcast for all channels in a preamp. Use only for Mode 1 (1 Stream, 2 Detectors, 1 Method) 
  int SetAllChBroadcastMode(unsigned char Start, bool doInitOnly = false);

  // Read stream data. Specify timeout in milli-seconds
  int ReadStreamData (unsigned int *BridgeData, unsigned long long *TimeStamp, unsigned int Timeout);

  // Read stream data, block on read
  int ReadStreamDataBlocking (unsigned int *BridgeData, unsigned long long *TimeStamp);

  // Enable self calibration
  int EnableSelfCalibration(void);

  // Read calibration status (CAN in progress or not)
  int GetCalOnOffStatus(PREAMP_STATE_ENUM *eCalInProgress);
  
  // Flushes the contents of the Named Pipe that is used for streaming data for this device.
  int Flush();

  // Set Cycle Clock associated with this detector
  int SetCycleClock (unsigned int cycleClock);
};

#endif // #ifndef _PREAMPSTREAMWRAPPER_H

