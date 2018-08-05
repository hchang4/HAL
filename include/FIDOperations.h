/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FIDOperations.h
 *
 *  Description: FID Operations [e.g. AutoZero, NullElectrometer] layer.
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
 *  $Id: FIDOperations.h,v 1.7 2016/09/20 12:28:59 emrsn\rrthomas Exp $
 *  $Log: FIDOperations.h,v $
 *  Revision 1.7  2016/09/20 12:28:59  emrsn\rrthomas
 *  Fix for FID Auto Zero from Cycle Clock 1 from intefering with Auto Zero from Cycle Clock 2 and vice versa. State Variables that store the last DAC offset position on a per FID basis were declared static, so both FIDs were sharing these variables. Made them member variables of the class so that each instance can have its own copy.
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
 *  Revision 1.2  2009/11/02 12:11:08  epm-na\rsaha
 *  Moved GetADC() functionality to CFID_ADC_AD7811YRU class.
 *
 *  Revision 1.1  2009/05/14 22:09:33  epm-na\rsaha
 *  Files to read/write FID DAC, ADC values and perform operations such as FID Null Electrometer, Auto Zero.
 *
 *************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "FID_ADC_AD7811YRU.h"
#include "FID_DAC_AD5570ARSZ.h"

class CFIDOperations
{
private:
  CFID_DAC1_AD5570ARSZ m_obFIDDAC1;
  CFID_DAC2_AD5570ARSZ m_obFIDDAC2;
  CFID_ADC_AD7811YRU m_obFIDADC;

  bool SetDAC1(float fVolt);
  bool SetDAC2(float fVolt);

  void AcquireLock(FILE *&fpLockFile, struct flock &fl);
  void ReleaseLock(FILE *fpLockFile, struct flock fl);

  int m_nFidNo;
  bool m_bInitNullElectrometer, m_bInitAutoZero;
  float m_fNullDACVolt, m_fAutoZeroDACVolt;
  

public:
  CFIDOperations(int nFidNo);
  ~CFIDOperations() {}

  bool NullElectrometer(bool bHighGain);
  bool AutoZero();
};
