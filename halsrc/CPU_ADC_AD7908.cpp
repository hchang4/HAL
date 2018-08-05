/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: CPU_ADC_AD7908.cpp
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
 *  $Id: CPU_ADC_AD7908.cpp,v 1.8 2016/09/30 20:34:25 emrsn\rrthomas Exp $
 *  $Log: CPU_ADC_AD7908.cpp,v $
 *  Revision 1.8  2016/09/30 20:34:25  emrsn\rrthomas
 *  This module is included in the fallback utilities (networkdfallback and ddiscfallback) so removed all references to the DEBUGx calls. DEBUGx calls can't be made when compiling for fallback utlities.
 *
 *  Revision 1.7  2016/08/09 17:09:06  emrsn\jbeatty
 *  Support for 1500XA R2
 *
 *  Revision 1.6  2015/05/04 14:49:25  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.5  2014/06/26 10:00:07  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.4  2013/01/30 06:18:32  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.3  2012/03/21 06:48:46  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.2  2010/05/06 23:27:26  EMRSN\RRTHOMAS
 *  Added code to perform a software reset the ADC on startup as required by the datasheet of the ADC.
 *
 *  Revision 1.1  2009/06/22 20:47:09  epm-na\rsaha
 *  Interface definitions to read 8 channel ADCs present on CPU.
 *
 *************************************************************************/



#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


#include "CPU_ADC_AD7908.h"

#define DATA_BUFF_SZ      256
#define CPU_ADC1_DEV_PATH "/dev/cpuadc1"
#define CPU_ADC2_DEV_PATH "/dev/cpuadc2"
#define CPU_ADC_LOCK_FILE "/tmp/CPUADCOperatins_LOCK"

//Control register Command format
//Write = 1, SEQ = Shadow = 0 : No sequencing, PM1 = PM0 = 1 : Normal Operation, Range = 1 : 0V to 2.5V, Coding = 1 : O/p straight binary data.
//Write | Seq | Don't Care | Add2 | Add1 | Add0 | PM1 | PM0 | Shadow | Don't Care | Range | Coding | 4 trailing 0s
//  1   |  0  |      0     |   ?  |   ?  |   ?  |  1  |  1  |   0    |      0     |   1   |   1    |     0000
#define ADC_CTRL_REG_CMD  0x8330 
#define ADC_CMD_CH_MASK   0xE3FF
#define ADC_CMD_CH_POS    10
#define ADC_READ_CMD      0x0000
//Read data format
//1 leading 0 | Add2 | Add1 | Add0 | DB7 | DB6 | DB5 | DB4 | DB3 | DB2 | DB1 | DB0 | 4 trailing 0s
//     0      |   ?  |   ?  |   ?  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |  x  |     0000
#define ADC_READ_CH_MASK   0x7000
#define ADC_READ_CH_POS    12
#define ADC_READ_DATA_MASK 0x0FF0
#define ADC_READ_DATA_POS  4


CCPU_ADC_AD7908::CCPU_ADC_AD7908( const char * pszDevPath )
{
  m_fdADCTx = -1;
  m_fdADCRx = -1;
  m_bInitComplete = false;

  if( pszDevPath )
  {
    m_fdADCTx = open( pszDevPath, O_WRONLY );
    if( m_fdADCTx >= 0 )
    {
      m_fdADCRx =  open( pszDevPath, O_RDONLY | O_NONBLOCK );
      if( m_fdADCRx < 0 )
      {
        close( m_fdADCTx );
        m_fdADCTx = -1;
      }
    }
  }
}

CCPU_ADC_AD7908::~CCPU_ADC_AD7908()
{
  if( m_fdADCTx >= 0 )
  {
    close( m_fdADCTx );
  }
  if( m_fdADCRx >= 0 )
  {
    close( m_fdADCRx );
  }
}


void CCPU_ADC_AD7908::AcquireLock( FILE * & fpLockFile, struct flock & fl )
{
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  fl.l_pid = getpid();

  fpLockFile = fopen( CPU_ADC_LOCK_FILE, "w+" );
  if( fpLockFile )
  {
    fcntl( fpLockFile->_fileno, F_SETLKW, & fl );
  }
}

void CCPU_ADC_AD7908::ReleaseLock( FILE * fpLockFile, struct flock fl )
{
  if( fpLockFile )
  {
    fl.l_type = F_UNLCK;
    fcntl( fpLockFile->_fileno, F_SETLKW, & fl );
    fclose( fpLockFile );
  }
}

void CCPU_ADC_AD7908::SetChannelNum( unsigned short & usTxCmd, int nChannelNum )
{
  usTxCmd = usTxCmd & ADC_CMD_CH_MASK;
  unsigned short usChannelNum = nChannelNum << ADC_CMD_CH_POS;
  usTxCmd = usTxCmd | usChannelNum;
}


int CCPU_ADC_AD7908::GetReading( int nChannelNum, float & fADCVoltage )
{
  int nRetVal = 0;

  if( nChannelNum >= 1 && nChannelNum <= 8 )
  {
    nChannelNum--;
    if( m_fdADCRx >= 0 && m_fdADCTx >= 0 )
    {
      unsigned short usTxCmd = ADC_CTRL_REG_CMD;
      SetChannelNum( usTxCmd, nChannelNum );
      char szData[DATA_BUFF_SZ] = {0};
      
      
      //ADC read involve 2 step process - 
      //  a. Configure ADC Control Register. 
      //  b. Read Channel Data.
      //If two different objects of the this class talks at the same time then it will result in
      //corrupt ADC reads. To solve this -
      //Take a system lock across GC. This will prevent other objects of the same class to talk 
      //at the same time.
      FILE * fpLockFile = NULL;
      struct flock fl;
      AcquireLock( fpLockFile, fl );

      // Reset the ADC the very first time by setting DIN to "1" and blasting it with 32 clocks. 
      // Refer Page 23 of the datasheet, Figure 24.
      if (!m_bInitComplete)
      {
        m_bInitComplete = true; // Don't do this again
        sprintf (szData, "sp 0xFFFF 0xFFFF");
  
        if( write( m_fdADCTx, szData, strlen( szData ) ) != (int) strlen( szData ) )
        {
          nRetVal = -1;
        }
      }

      // Setup control register to read from appropriate channel
      sprintf( szData, "sp 0x%x", usTxCmd );
      if( write( m_fdADCTx, szData, strlen( szData ) ) != (int) strlen( szData ) )
      {
        nRetVal = -1;
      }

      usleep (1000); // Sleep, let the ADC convert

      if (-1 != nRetVal )
      {
        if( write( m_fdADCTx, szData, strlen( szData ) ) != (int) strlen( szData ) )
        {
          nRetVal = -1;
        }
  
        if( -1 != nRetVal )
        {
          memset( szData, 0, DATA_BUFF_SZ );
          if( read( m_fdADCRx, szData, DATA_BUFF_SZ ) > 0 )
          {
            unsigned short usADCCount = 0;
            sscanf( szData, "%hx", & usADCCount );
      
            unsigned short usChannel = usADCCount & ADC_READ_CH_MASK;
            usChannel = usChannel >> ADC_READ_CH_POS;
            if( usChannel == (unsigned short) nChannelNum )
            {
              usADCCount = usADCCount & ADC_READ_DATA_MASK;
              usADCCount = usADCCount >> ADC_READ_DATA_POS;
        
              //Count - ADC Voltage
              //    0         0.0 V
              //  255         2.5 V
              //Resolution -> (2.5/255) V
              fADCVoltage = (float)usADCCount * 2.5 / 255.0;
            }
            else
            {
              nRetVal = -1;
            }
          }
          else
          {
            nRetVal = -1;
          }
        }
      }
      
      ReleaseLock( fpLockFile, fl );
    }
    else
    {
      nRetVal = -1;
    }
  }
  else
  {
    nRetVal = -1;
  }
  
  return nRetVal;
}

CCPU_ADC1_AD7908::CCPU_ADC1_AD7908() : CCPU_ADC_AD7908( CPU_ADC1_DEV_PATH )
{
}

CCPU_ADC2_AD7908::CCPU_ADC2_AD7908() : CCPU_ADC_AD7908( CPU_ADC2_DEV_PATH )
{
}


