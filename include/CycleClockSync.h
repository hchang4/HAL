/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: CycleClockSync.h
 *
 *  Description: Synchronize Cycle Clock Start/Stop across all IO Boards
 *
 *  Copyright:        Copyright (c) 2011-2015, 
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C++'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *
 *************************************************************************/


#ifndef _CYCLE_CLOCK_SYNC_H
#define _CYCLE_CLOCK_SYNC_H

#include "BaseDev.h"
#include "PreampProtocol.h"

class CCycleClockSync : public CBaseDev
{
private:
  // Start or Stop Cycle Clock
  int SetCycleClockState(CYCLE_CLOCK_CMD_ENUM eState);

public:
  CCycleClockSync();  // Default Constructor

  ~CCycleClockSync(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  int StartCycleClock ();

  int StopCycleClock ();
};

#endif // #ifndef _CYCLE_CLOCK_SYNC_H



