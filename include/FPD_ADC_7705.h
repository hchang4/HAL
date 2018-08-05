/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FPD_ADC_7705.h
 *
 *  Description: FPD ADC access layer.
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
 *  $Id: FPD_ADC_7705.h,v 1.8 2016/08/30 09:57:51 emrsn\jbeatty Exp $
 *  $Log: FPD_ADC_7705.h,v $
 *  Revision 1.8  2016/08/30 09:57:51  emrsn\jbeatty
 *  Mods for 1500XA R2 support
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
 *  Revision 1.5  2012/03/21 06:49:06  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.4  2010/02/18 14:39:21  emrsn\rsaha
 *  Added API to read back ADC registers to find whether ADC SPI interface is broken or not.
 *
 *  Revision 1.3  2010/01/27 21:57:35  emrsn\rrthomas
 *  FPD ADC readback fixes
 *  - Changed configuration of both ADC channels to Bipolar. AD7705 cannot be setup as with one channel as bipolar and the other as unipolar. Changed the voltage translation to use the bipolar voltage range of +/- 5 Volt instead of 0 to 5V.
 *  - Added HAL APIs for reading FPD ADC raw ADC counts (for testing)
 *  - Added TestHAL methods to read FPD raw ADC counts (for testing)
 *
 *  This code is WIP, tested and works but ADC setup needs cleanup.
 *
 *  Revision 1.2  2010/01/18 14:54:22  emrsn\rsaha
 *  Added API to do reset.
 *
 *  Revision 1.1  2009/12/17 14:05:26  emrsn\rsaha
 *  HAL class to talk over SPI and read FPD Pre-Amplifier voltage and AutoZero DAC voltage readings.
 *
 *************************************************************************/

class CFPD_ADC_7705
{
private:
  int m_fd;
  int m_nFpdNo;

  void AcquireLock(FILE* & fpLockFile, struct flock &fl);
  void ReleaseLock(FILE* fpLockFile, struct flock fl);

  bool IsADCReset();
  bool ReadRegister(unsigned char ucCmmd, unsigned char ucChannelNum, unsigned short &usRegData);
  bool SendCommand2ADC(unsigned char ucTxCmd, unsigned char ucChannelNum, bool bUseChannelNum = false);
  void Reset();
  bool InitADCChannels();
  bool Wait4DataReady(unsigned char ucChannelNum);
  bool GetCount(unsigned char ucChannelNum, unsigned short &usADCCount);

public:
  CFPD_ADC_7705(int nFpdNo);
  ~CFPD_ADC_7705();

  bool GetFPDPreampVoltage(float &fVolt);
  bool GetFPDDACVoltage(float &fVolt);
  bool GetFPDDACCounts(unsigned short &usVal);
  bool GetFPDPreampCounts(unsigned short &usVal);
};
