/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampStreamWrapper.cpp
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

#include "PreampStreamWrapper.h"

CPreampStreamWrapper::CPreampStreamWrapper()  // Default Constructor
  : m_bSimulate(true)
{
}

CPreampStreamWrapper::~CPreampStreamWrapper() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CPreampStreamWrapper::OpenHal (char* pszDevName)
{
  // Check if we want to get real detector data or data from a file
  m_bSimulate = m_oPreampStrmSim.IsSimulationEnabled(pszDevName);

  if (m_bSimulate)
    return m_oPreampStrmSim.OpenHal (pszDevName);
  else
    return m_oPreampStrmHW.OpenHal (pszDevName);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CPreampStreamWrapper::CloseHal ()
{
  if (m_bSimulate)
    return m_oPreampStrmSim.CloseHal();
  else
    return m_oPreampStrmHW.CloseHal();
}

// Starts / Stops broadcast. Start = 1 starts broadcast, Start = 0 stops broadcast.
int CPreampStreamWrapper::SetBroadcastMode (unsigned char Start, bool doInitOnly)
{
  if (m_bSimulate)
    return m_oPreampStrmSim.SetBroadcastMode(Start);
  else
    return m_oPreampStrmHW.SetBroadcastMode(Start, doInitOnly);
}

// Read stream data. Specify timeout in milli-seconds
int CPreampStreamWrapper::ReadStreamData (unsigned int *BridgeData, unsigned long long *TimeStamp, unsigned int Timeout)
{
  if (m_bSimulate)
    return m_oPreampStrmSim.ReadStreamData (BridgeData, TimeStamp);
  else
    return m_oPreampStrmHW.ReadStreamData (BridgeData, TimeStamp, Timeout);
}

// Read stream data, block on read
int CPreampStreamWrapper::ReadStreamDataBlocking (unsigned int *BridgeData, unsigned long long *TimeStamp)
{
  if (m_bSimulate)
    return m_oPreampStrmSim.ReadStreamData (BridgeData, TimeStamp);
  else
    return m_oPreampStrmHW.ReadStreamDataBlocking (BridgeData, TimeStamp);
}

//Enable self calibration
int CPreampStreamWrapper::EnableSelfCalibration(void) 
{
  if (m_bSimulate)
    return ERR_SUCCESS;
  else
    return m_oPreampStrmHW.EnableSelfCalibration();
}

// Read calibration status (CAL in progress or not)
int CPreampStreamWrapper::GetCalOnOffStatus(PREAMP_STATE_ENUM *eCalInProgress)
{
  if (m_bSimulate)
  {
    *eCalInProgress = PREAMP_STATE_OFF;
    return ERR_SUCCESS;
  }
  else
    return m_oPreampStrmHW.GetCalOnOffStatus(eCalInProgress);
}

//Flushes the contents of the Named Pipe that is used for streaming data for this device.
int CPreampStreamWrapper::Flush()
{
  if (m_bSimulate)
    return ERR_SUCCESS;
  else
    return m_oPreampStrmHW.Flush();
}

// Starts / Stops broadcast for all channels in a preamp. 
int CPreampStreamWrapper::SetAllChBroadcastMode(unsigned char Start, bool doInitOnly)
{
  if (m_bSimulate)
    return m_oPreampStrmSim.SetBroadcastMode(Start);
  else
    return m_oPreampStrmHW.SetAllChBroadcastMode(Start, doInitOnly);
}

// Set Cycle Clock associated with this detector.
int CPreampStreamWrapper::SetCycleClock (unsigned int cycleClock)
{
  if (m_bSimulate)
    return ERR_SUCCESS;
  else
    return m_oPreampStrmHW.SetCycleClock(cycleClock);
}
