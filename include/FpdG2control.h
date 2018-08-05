/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FpdG2control.h
 *
 *  Description: FPD G2 control access layer
 *
 *  Copyright:        Copyright (c) 2016-
 *                    Rosemount Analytical
 *                    All Rights Reserved
 *
 *  Operating System:  Linux
 *  Language:          C++
 *  Target:            Gas Chromatograph Model GC700/1500/1500XA R2
 *
 *  Revision History:
 *  - Initial version
 *************************************************************************/

#ifndef _FPD_G2_CONTROL_H_
#define _FPD_G2_CONTROL_H_

#include "BaseDev.h"
#include "FpdG2protocol.h"

class CFpdG2control : public CBaseDev
{
private:
  int SendCommand(FPD_G2_CMD_ENUM eCmd);
  int SetData(FPD_G2_CMD_ENUM eCmd, CAN_CMD_FPD_G2_DATA_STRUCT &data);
  int GetData(FPD_G2_CMD_ENUM eCmd, CAN_CMD_FPD_G2_DATA_STRUCT &data);

public:
  CFpdG2control() {}
  ~CFpdG2control() {}

  int OpenHal(char *pszDevName);
  int CloseHal();

  int SetTECsetpoint(long tecTemp);
  int SetIgnitionMode(FPD_G2_IGN_MODE ignMode);
  int SetIgnitionAttempts(long ignTries);
  int SetWaitTime(long waitTime);
  int SetIgniterOnTime(long ignDur);
  int SetFlameOnTemp(long flameOnTemp);
  int SetFlameOffTemp(long flameOffTemp);

  int GetTECsetpoint(long &tecTemp);
  int GetIgnitionMode(FPD_G2_IGN_MODE &ignMode);
  int GetIgnitionAttempts(long &ignTries);
  int GetWaitTime(long &waitTime);
  int GetIgniterOnTime(long &ignDur);
  int GetFlameOnTemp(long &flameOnTemp);
  int GetFlameOffTemp(long &flameOffTemp);
  int GetFlameTemp(long &flameTemp);
  int GetFlameStatus(FPD_G2_FLAME_STATUS &flameStatus);
  int GetTECtemp(long &tecTemp);
  int GetIgnitionStatus(FPD_G2_IGN_STATUS &ignStatus);

  int TurnOnIgnition();
  int TurnOffIgnition();
};

#endif  //#ifndef _FPD_G2_CONTROL_H_
