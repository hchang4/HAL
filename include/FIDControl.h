/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FIDControl.h
 *
 *  Description: FID control access layer.
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
 *  $Id: FIDControl.h,v 1.7 2016/08/30 09:57:51 emrsn\jbeatty Exp $
 *  $Log: FIDControl.h,v $
 *  Revision 1.7  2016/08/30 09:57:51  emrsn\jbeatty
 *  Mods for 1500XA R2 support
 *
 *  Revision 1.6  2016/08/09 17:09:44  emrsn\jbeatty
 *  Support for 1500XA R2
 *
 *  Revision 1.5  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.4  2014/06/26 10:01:13  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.3  2012/03/21 06:49:05  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.2  2009/05/14 22:11:25  epm-na\rsaha
 *  Revised to include BaseIO header file.
 *
 *  Revision 1.1  2009/04/15 13:30:29  epm-na\rsaha
 *  FID Ignition and Gain control class.
 *
 *
 *************************************************************************/

#include "BaseDev.h"
#include "BaseIOProtocol.h"

class CFIDCtrl : public CBaseDev
{
private:
  int SendCommand(FID_CMD_ENUM eFIDCmmd);

public:
  CFIDCtrl() {}
  ~CFIDCtrl() {}

  int OpenHal(char *pszDevName);
  int CloseHal();

  int TurnOnIgnition();
  int TurnOffIgnition();

  int SetGainLow();
  int SetGainHigh();
};
