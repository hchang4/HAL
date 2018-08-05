/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FID_ADC_AD7811YRU.h
 *
 *  Description: FID ADC access layer.
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
 *  $Id: FID_ADC_AD7811YRU.h,v 1.8 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 *  $Log: FID_ADC_AD7811YRU.h,v $
 *  Revision 1.8  2016/08/09 17:09:44  emrsn\jbeatty
 *  Support for 1500XA R2
 *
 *  Revision 1.7  2015/04/30 09:32:32  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.6  2014/06/26 10:01:13  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.5  2012/03/21 06:49:05  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.4  2009/11/02 12:14:03  epm-na\rsaha
 *  Added method GetCHVoltage() to return ADC Channel readings in volts.
 *
 *  Revision 1.3  2009/10/27 06:10:08  epm-na\rsaha
 *  Revised to use single Fd for read and write.
 *
 *  Revision 1.2  2009/06/22 20:51:06  epm-na\rsaha
 *  Few corrections.
 *
 *  Revision 1.1  2009/05/14 22:09:33  epm-na\rsaha
 *  Files to read/write FID DAC, ADC values and perform operations such as FID Null Electrometer, Auto Zero.
 *
 *************************************************************************/

class CFID_ADC_AD7811YRU
{
private:
  int m_fd;
  int m_nFidNo;
  
  void AcquireLock(FILE *&fpLockFile, struct flock &fl);
  void ReleaseLock(FILE *fpLockFile, struct flock fl);

  void SetChannelNum(unsigned short &usTxCmd, int nChannelNum);

public:
  CFID_ADC_AD7811YRU(int nFidNo);
  ~CFID_ADC_AD7811YRU();

  int GetCount(int nChannelNum, unsigned short &usADCCount);
  bool GetCHVoltage(int nChannelNum, float &fVolt);
};
