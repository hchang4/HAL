/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: CPU_ADC_AD7908.h
 *
 *  Description: CPU ADC access layer.
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
 *  $Id: CPU_ADC_AD7908.h,v 1.5 2015/04/30 09:32:31 EMRSN\Dkad Exp $
 *  $Log: CPU_ADC_AD7908.h,v $
 *  Revision 1.5  2015/04/30 09:32:31  EMRSN\Dkad
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 *  -Set the indent style to C/C++.
 *  -Set the indent size to 2, and replace tab with blank space.
 *  -Turn off interpretation of tab
 *
 *  Revision 1.4  2014/06/26 10:01:10  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.3  2012/03/21 06:49:03  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.2  2010/05/06 23:27:26  EMRSN\RRTHOMAS
 *  Added code to perform a software reset the ADC on startup as required by the datasheet of the ADC.
 *
 *  Revision 1.1  2009/06/22 20:43:06  epm-na\rsaha
 *  Interfaces to read 8 channel ADCs present on CPU.
 *
 *************************************************************************/


class CCPU_ADC_AD7908
{
private:
  int m_fdADCTx;
  int m_fdADCRx;
  bool m_bInitComplete;

  void AcquireLock( FILE * & fpLockFile, struct flock & fl );
  void ReleaseLock( FILE * fpLockFile, struct flock fl );

  void SetChannelNum( unsigned short & usTxCmd, int nChannelNum );

protected:
  CCPU_ADC_AD7908( const char * pszDevPath );

public:
  ~CCPU_ADC_AD7908();

  int GetReading( int nChannelNum, float & fADCVoltage );
};

class CCPU_ADC1_AD7908 : public CCPU_ADC_AD7908
{
public:
  CCPU_ADC1_AD7908();
};

class CCPU_ADC2_AD7908 : public CCPU_ADC_AD7908
{
public:
  CCPU_ADC2_AD7908();
};


