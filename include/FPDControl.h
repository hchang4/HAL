/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FPDControl.h
 *
 *  Description: FPD control access layer.
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
 *  $Id: FPDControl.h,v 1.4 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 *  $Log: FPDControl.h,v $
 *  Revision 1.4  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.3  2014/06/26 10:01:13  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.2  2012/03/21 06:49:06  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2009/12/17 14:04:09  emrsn\rsaha
 *  HAL class to talk to BaseIO and do FPD AutoZero.
 *
 *************************************************************************/


#include "BaseDev.h"
#include "BaseIOProtocol.h"

class CFPDCtrl : public CBaseDev
{
public:
  CFPDCtrl(){}
  ~CFPDCtrl(){}

  int OpenHal( char * pszDevName );
  int CloseHal();

  int AutoZero();
};


