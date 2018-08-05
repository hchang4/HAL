/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FID_DAC_AD5570ARSZ.cpp
 *
 *  Description: FID DAC access layer.
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
 *  $Id: FID_DAC_AD5570ARSZ.h,v 1.6 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 *  $Log: FID_DAC_AD5570ARSZ.h,v $
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
 *  Revision 1.2  2009/10/27 06:08:33  epm-na\rsaha
 *  Revised to remember the last set DAC1 and DAC2 o/ps across process.
 *
 *  Revision 1.1  2009/05/14 22:09:33  epm-na\rsaha
 *  Files to read/write FID DAC, ADC values and perform operations such as FID Null Electrometer, Auto Zero.
 *
 *************************************************************************/

class CDAC_AD5570ARSZ
{
private:
  int m_fdDACTx;
  int m_fdDACRx;

  unsigned short m_usDACCount;

protected:
  CDAC_AD5570ARSZ(const char *pszDevPath);

public:
  ~CDAC_AD5570ARSZ();

  int SetCount(unsigned short usDACCount);
  int GetCount(unsigned short &usDACCount);
};

class CFID_DAC1_AD5570ARSZ : public CDAC_AD5570ARSZ
{
private:
  static unsigned short m_usDAC1Count;

public:
  CFID_DAC1_AD5570ARSZ(int nFidNo);

  int SetCount(unsigned short usDACCount);
  int GetCount(unsigned short &usDACCount);
};

class CFID_DAC2_AD5570ARSZ : public CDAC_AD5570ARSZ
{
private:
  static unsigned short m_usDAC2Count;

public:
  CFID_DAC2_AD5570ARSZ(int nFidNo);

  int SetCount(unsigned short usDACCount);
  int GetCount(unsigned short &usDACCount);
};
