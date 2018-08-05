/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: HwInhibitCtrl.h
 *
 *  Description: Preamp hardware inhibit control access layer
 *
 *  Copyright:        Copyright (c) 2017-
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C++'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *************************************************************************/

#ifndef _HW_INHIBIT_CTRL_H
#define _HW_INHIBIT_CTRL_H

#include "BaseDev.h"
#include "PreampProtocol.h"

// Preamp Hardware Inhibit controller
class CHwInhibitCtrl : public CBaseDev
{
private:

public:
  CHwInhibitCtrl();  // Default Constructor

  ~CHwInhibitCtrl(); // Destructor

  // Open the device, returning 0 on success or negative error code on failure
  int OpenHal(char* pszDevName);

  // Close the device
  int CloseHal();

  // Set the hardware inhibit ON or OFF
  int SetInhibit(PREAMP_HWINHIBIT_STATE eState);
};

#endif  // #ifndef _HW_INHIBIT_CTRL_H
