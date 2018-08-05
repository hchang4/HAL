/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: IRKeyPad.h
 *
 *  Description: IR Key Pad access layer.
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
 *  $Id: IRKeyPad.h,v 1.4 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 *  $Log: IRKeyPad.h,v $
 *  Revision 1.4  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.3  2014/06/26 10:01:16  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.2  2012/03/21 06:49:07  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2009/06/25 18:59:54  epm-na\rsaha
 *  Added interfaces in HAL layer to set/get/save/restore IR key sensitivity settings.
 *
 ***********************************************************************/


#include "BaseDev.h"

struct IRKeySettings
{
  unsigned char ucSensitivity[8];
};



class CIRKeyPad : public CBaseDev
{
  int SendCommand( unsigned char * ucTXData,
                   unsigned int unTXNumData,
                   unsigned char * ucRXData,
                   unsigned int unRXNumData,
                   unsigned int unTimeOut = HAL_DFLT_TIMEOUT );

public:
  CIRKeyPad();
  ~CIRKeyPad();

  int OpenHal( char * pszDevName );
  int CloseHal();

  //Sets the specified IR Key Sensitivity setting to IRKeyPAD.
  int SetKeySensitivity( unsigned char ucKeyNum, unsigned char ucSensitivity );
  //Sets all IR Key Sensitivity settings.
  int SetAllKeySensitivity( unsigned char ucSensitivity );
  //Commands IRKeyPad to save the present IR Key Sensitivity settings to memory.
  int SaveKeySensitivitySettings();
  //Retrieves the specific IR Key Sensitivity setting and retruns back to application.
  int GetKeySensitivity( unsigned char ucKeyNum, unsigned char & ucSensitivity );
  //Retrieves all IR Key Sensitivity settings and returns them back to application.
  int GetAllKeySensitivity( IRKeySettings & KeySettings );
  //Commands IRKeyPad to load previously saved IR Key Sensitivity settings
  //and returns the setting values back to application.
  int LoadAllReviousKeySensitivity( IRKeySettings & KeySettings );
  //Commands IRKeyPad to restore back all the default IR Key Sensitivity
  //settings and returns the default values back to application.
  int RestoreAllDefaultKeySensitivity( IRKeySettings & KeySettings );
};


