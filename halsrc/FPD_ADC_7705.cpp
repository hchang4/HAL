/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FPD_ADC_7705.cpp
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
 *  $Id: FPD_ADC_7705.cpp,v 1.10 2016/08/30 09:57:09 emrsn\jbeatty Exp $
 *  $Log: FPD_ADC_7705.cpp,v $
 *  Revision 1.10  2016/08/30 09:57:09  emrsn\jbeatty
 *  Mods for 1500XA R2 support
 *
 *  Revision 1.9  2015/05/04 14:49:27  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.8  2014/06/26 10:00:22  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.7  2013/01/30 06:18:37  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.6  2012/03/21 06:48:51  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.5  2010/02/18 14:50:17  emrsn\rsaha
 *  Added API to read back ADC registers(Clock register) to find whether ADC SPI interface is broken or not. If interface is broken then initializing the FPD ADC once again. This is done to solve losing of ADC interface due to noisy MOSI bin.
 *
 *  Revision 1.4  2010/01/27 21:57:35  emrsn\rrthomas
 *  FPD ADC readback fixes
 *  - Changed configuration of both ADC channels to Bipolar. AD7705 cannot be setup as with one channel as bipolar and the other as unipolar. Changed the voltage translation to use the bipolar voltage range of +/- 5 Volt instead of 0 to 5V.
 *  - Added HAL APIs for reading FPD ADC raw ADC counts (for testing)
 *  - Added TestHAL methods to read FPD raw ADC counts (for testing)
 *
 *  This code is WIP, tested and works but ADC setup needs cleanup.
 *
 *  Revision 1.3  2010/01/18 14:53:20  emrsn\rsaha
 *  Revised to set clock 2MHz to FPD ADC and do reset before doing ADC Init.
 *  NOTE - Before programming the Production GCs clock need to switched to 2.4576MHz[As per Al production GCs will run at 2.4576MHz].
 *
 *  Revision 1.2  2009/12/18 09:43:17  emrsn\rsaha
 *  Added two TODOs, will remove them once testing against hardware is done.
 *  The TODOs are -
 *  1. Sleeping for 10us for each iteration while waiting for ADC to be ready with data. Would change/remove this timing after testing.
 *  2. Force fully breaking the while loop after one iteration in the wait loop for ADC to be ready. This is done since our Junior setup assumes 2nd Det as FPD, even though FPD board is absent, this makes xpdetectord to spin in this while loop infinitely as it tries to read FPD preamplifier voltage.
 *
 *  Revision 1.1  2009/12/17 14:10:52  emrsn\rsaha
 *  HAL class to talk over SPI and read FPD Pre-Amplifier voltage and AutoZero DAC voltage readings.
 *
 *************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

  
#include "debug.h"

#include "FPD_ADC_7705.h"
#define DATA_BUFF_SZ      256

// Soft links to /dev/fidadc1(2)
#define FPD1_ADC_DEV_PATH   "/dev/flame1adc"
#define FPD2_ADC_DEV_PATH   "/dev/flame2adc"

#define ADC_CH_MASK       0xFC
//Communication register Command format
//R/!W = 0 : Write, RS2 = 0, RS1 = 1, RS = 0 : Clock Register next
//  0/!DRDY  |  RS2  |  RS1  |  RS0  |  R/!W  |  STBY  |  CH1  |  CH0  
//     0     |   0   |   1   |   0   |   0    |   0    |   0   |   0 
#define ADC_WRITE_CLOCK_REG_NXT_CMD 0x20
//R/!W = 1 : Write, RS2 = 0, RS1 = 1, RS = 0 : Clock Register next
//  0/!DRDY  |  RS2  |  RS1  |  RS0  |  R/!W  |  STBY  |  CH1  |  CH0
//     0     |   0   |   1   |   0   |   1    |   0    |   0   |   0
#define ADC_READ_CLOCK_REG_NXT_CMD      0x28
//R/!W = 0 : Write, RS2 = 0, RS1 = 0, RS = 1 : Setup Register next
//  0/!DRDY  |  RS2  |  RS1  |  RS0  |  R/!W  |  STBY  |  CH1  |  CH0  
//     0     |   0   |   0   |   1   |   0    |   0    |   0   |   0 
#define ADC_WRITE_SETUP_REG_NXT_CMD 0x10
//R/!W = 1 : Write, RS2 = 0, RS1 = 0, RS = 1 : Setup Register next
//  0/!DRDY  |  RS2  |  RS1  |  RS0  |  R/!W  |  STBY  |  CH1  |  CH0
//     0     |   0   |   0   |   1   |   1    |   0    |   0   |   0
#define ADC_READ_SETUP_REG_NXT_CMD      0x18
//R/!W = 1 : Read, RS2 = 0, RS1 = 1, RS = 1 : Data Register next
//  0/!DRDY  |  RS2  |  RS1  |  RS0  |  R/!W  |  STBY  |  CH1  |  CH0  
//     0     |   0   |   1   |   1   |   1    |   0    |   0   |   0 
#define ADC_READ_DATA_REG_NXT_CMD 0x38
//R/!W = 1 : Read, RS2 = 0, RS1 = 0, RS = 0 : Communication Register next
//  0/!DRDY  |  RS2  |  RS1  |  RS0  |  R/!W  |  STBY  |  CH1  |  CH0  
//     0     |   0   |   0   |   0   |   1    |   0    |   0   |   0 
#define ADC_READ_COMM_REG_NXT_CMD 0x08
//Mask to read DRDY bit status.
//  0/!DRDY  |  RS2  |  RS1  |  RS0  |  R/!W  |  STBY  |  CH1  |  CH0  
//     1     |   0   |   0   |   0   |   0    |   0    |   0   |   0 
#define ADC_READ_DRDY_PIN_STATUS_MASK 0x80

//Clock register Command format
//CLK = 0, FS = 1, FS0 = 0 : 100Hz O/p update rate.
//  Zero  |  Zero  |  Zero  |  CLKDIS  |  CLKDIV  |  CLK  |  FS1  |  FS0  
//   0    |   0    |   0    |    0     |    1     |   0   |   1   |   0   
#define ADC_100HZ_CLOCK_REG_CMD   0x0A

//Setup register Command format
//MD1 = 0, MD0 = 1 : Calibrate, !B/U = 0 : Bipolar Operation
//  MD1  |  MD0  |  G2  |  G1  |  G0  |  !B/U  |  BUF  |  FSYNC  
//   0   |   1   |  0   |  0   |  0   |   0    |   0   |    0   
#define ADC_BP_CAL_SETUP_REG_CMD  0x40
//MD1 = 0, MD0 = 1 : Calibrate, !B/U = 1 : Unipolar Operation
//  MD1  |  MD0  |  G2  |  G1  |  G0  |  !B/U  |  BUF  |  FSYNC  
//   0   |   1   |  0   |  0   |  0   |   1    |   0   |    0   
#define ADC_UP_CAL_SETUP_REG_CMD  0x44

#define FPD1_ADC_LOCK_FILE "/tmp/FPD1ADCOperations_LOCK"
#define FPD2_ADC_LOCK_FILE "/tmp/FPD2ADCOperations_LOCK"

//  0V to +5V, 16bit ADC
//so    x count is =  5 * x / 65535 V
static const float FPDADCRESOLUTION = (float) ( 5.0 / 65535.0 );

CFPD_ADC_7705::CFPD_ADC_7705(int nFpdNo)
{
  m_fd = -1;
  m_nFpdNo = nFpdNo;

  if (nFpdNo == 1)
    m_fd = open(FPD1_ADC_DEV_PATH, O_RDWR);
  else
    m_fd = open(FPD2_ADC_DEV_PATH, O_RDWR);

  if (m_fd == -1)
  {
    DEBUG1("CFPD_ADC_7705::CFPD_ADC_7705 - Fail to open %s!", (nFpdNo == 1) ? FPD1_ADC_DEV_PATH : FPD2_ADC_DEV_PATH);
  }
}

CFPD_ADC_7705::~CFPD_ADC_7705()
{
  if (m_fd != -1)
    close(m_fd);
}

void CFPD_ADC_7705::AcquireLock(FILE* &fpLockFile, struct flock &fl)
{
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  fl.l_pid = getpid();

  if (m_nFpdNo == 1)
    fpLockFile = fopen(FPD1_ADC_LOCK_FILE, "w+");
  else if (m_nFpdNo == 2)
    fpLockFile = fopen(FPD2_ADC_LOCK_FILE, "w+");
  else
    fpLockFile = NULL;

  if (fpLockFile)
  {
    if (fcntl(fpLockFile->_fileno, F_SETLKW, &fl) != 0)
    {
      // Treat it as a soft error don't stop executing
      DEBUG1("CFPD_ADC_7705::AcquireLock - Failed to lock");
    }
  }
}

void CFPD_ADC_7705::ReleaseLock(FILE *fpLockFile, struct flock fl)
{
  if (fpLockFile)
  {
    fl.l_type = F_UNLCK;
    if (fcntl(fpLockFile->_fileno, F_SETLKW, &fl) != 0)
      DEBUG1("CFPD_ADC_7705::ReleaseLock - Failed to unlock");
    fclose(fpLockFile);
  }
}

bool CFPD_ADC_7705::SendCommand2ADC(unsigned char ucTxCmd, unsigned char ucChannelNum, bool bUseChannelNum)
{
  bool bRet = true;
  if (bUseChannelNum)
  {
    ucTxCmd = ucTxCmd & ADC_CH_MASK;
    ucTxCmd = ucTxCmd | ucChannelNum;
  }

  char szData[DATA_BUFF_SZ] = {0};
  sprintf(szData, "bn 0x%02x", ucTxCmd);
  DEBUG1("CFPD_ADC_7705::SendCommand2ADC - Sending %s", szData);

  if (write(m_fd, szData, strlen(szData)) != (int)strlen(szData))
  {
    bRet = false;
    DEBUG1("CFPD_ADC_7705::SendCommand2ADC - Failed to write command to ADC");
  }
  else
  {
    char szData[DATA_BUFF_SZ];
    memset(szData, 0, DATA_BUFF_SZ);
    if (read(m_fd, szData, DATA_BUFF_SZ) > 0)
      DEBUG1("CFPD_ADC_7705::SendCommand2ADC - Read %s", szData);
  }

  return bRet;
}

void CFPD_ADC_7705::Reset()
{
  char *pszData = "bn 0xFF 0xFF 0xFF 0xFF";
  DEBUG1("CFPD_ADC_7705::Reset - Sending %s", pszData);

  if (write(m_fd, pszData, strlen(pszData)) != (int)strlen(pszData))
  {
    DEBUG1("CFPD_ADC_7705::Reset - Failed to write command to ADC");
  }
  else
  {
    char szData[DATA_BUFF_SZ];
    memset(szData, 0, DATA_BUFF_SZ);
    if (read(m_fd, szData, DATA_BUFF_SZ) > 0)
      DEBUG1("CFPD_ADC_7705:::Reset - Read %s", szData);

    // Give some time before talking to the ADC again
    usleep(1000);
  }
}

bool CFPD_ADC_7705::InitADCChannels()
{
  bool bRet = true;
  if (IsADCReset())
  {
    if (m_fd != -1)
    {
      DEBUG1("ADC Interface is lost!!!\n");

      Reset();

      if (!SendCommand2ADC(ADC_WRITE_CLOCK_REG_NXT_CMD, 0, true))
      {
        bRet = false;
        DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Communication Register for Clock Operation for CH0!!!");
      }
      
      if (bRet)
      {
        if (!SendCommand2ADC(ADC_100HZ_CLOCK_REG_CMD, 0, false))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Clock Register for CH0!!!");
        }
      }

      if (bRet)
      {
        if (!SendCommand2ADC(ADC_WRITE_SETUP_REG_NXT_CMD, 0, true))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Communication Register for Setup Operation for CH0!!!");
        }
      }
      if (bRet)
      {
        if (!SendCommand2ADC(ADC_BP_CAL_SETUP_REG_CMD, 0, false))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Setup Register for CH 0 !!!");
        }
      }
      if (bRet)
      {
        if (!Wait4DataReady(0))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Wait on ADC CH 0 ready failed");
        }
      }

      if (bRet)
      {
        if (!SendCommand2ADC(ADC_WRITE_CLOCK_REG_NXT_CMD, 1, true))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Communication Register for Clock Operation for CH1!!!");
        }
      }
      
      if (bRet)
      {
        if (!SendCommand2ADC(ADC_100HZ_CLOCK_REG_CMD, 0, false))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Clock Register for CH1!!!");
        }
      }

      if (bRet)
      {
        if (!SendCommand2ADC(ADC_WRITE_SETUP_REG_NXT_CMD, 1, true))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Communication Register for Setup Operation for CH1!!!");
        }
      }
      if (bRet)
      {
        if (!SendCommand2ADC(ADC_BP_CAL_SETUP_REG_CMD, 0, false))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Failed to set Setup Register for CH 1 !!!");
        }
      }
      if (bRet)
      {
        if (!Wait4DataReady(1))
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705::InitADCChannels - Wait on ADC CH 1 ready failed !!!");
        }
      }
    }
    else
    {
      bRet = false;
      DEBUG1("CFPD_ADC_7705::InitADCChannels - Device is not opened !!!");
    }
  }

  return bRet;
}

bool CFPD_ADC_7705::Wait4DataReady(unsigned char ucChannelNum)
{
  bool bRet = true;
  unsigned short usCommRegData;

  unsigned int unMaxAttempts = 0;

  // Wait for DRDY pin to go low
  do
  {
    usCommRegData = 0;
    // Command Communication register to read Communication register next
    if (SendCommand2ADC(ADC_READ_COMM_REG_NXT_CMD, ucChannelNum, true))
    {
      // Command to read data out of Communication register
      char *pszDataReadCmd = "bn 0x00";
      if (write(m_fd, pszDataReadCmd, strlen(pszDataReadCmd)) != (int)strlen(pszDataReadCmd))
      {
        bRet = false;
        DEBUG1("CFPD_ADC_7705:::Wait4DataReady - Failed to write to data read command from Comm Reg. of ADC!!!");
      }

      if (bRet)
      {
        // Read Data register
        char szData[DATA_BUFF_SZ];
        memset(szData, 0, DATA_BUFF_SZ);
        if (read(m_fd, szData, DATA_BUFF_SZ) > 0)
        {
          DEBUG1("CFPD_ADC_7705:::Wait4DataReady - Read response %s", szData);
          sscanf(szData, "%hx", &usCommRegData);
        }
        else
        {
          bRet = false;
          DEBUG1("CFPD_ADC_7705:::Wait4DataReady - Failed to read Comm Reg.!!!");
        }
      }
    }

    if (!(usCommRegData & ADC_READ_DRDY_PIN_STATUS_MASK))
      break;

    // Wait before next try
    usleep(2000);

    // Wait for 50 milliseconds = 5 * ADC Conversion time (1/100Hz = 10 milliseconds)
    unMaxAttempts++;
    if (unMaxAttempts > 25) 
    {
      bRet = false;
      break;
    }

  } while (bRet);

  return bRet;
}

bool CFPD_ADC_7705::GetCount(unsigned char ucChannelNum, unsigned short &usADCCount)
{
  bool bRet = true;

  if (m_fd != -1)
  {
    // If two different objects of the this class talks at the same time then it will result in
    // corrupt ADC reads. To solve this -
    // Take a system lock across GC. This will prevent other objects of the same class to talk 
    // at the same time.
    FILE *fpLockFile = NULL;
    struct flock fl;
    AcquireLock(fpLockFile, fl);

    // Check whether Interface is broken, if so initialize ADC Channels
    if (InitADCChannels())
    {
      // Sleep for 1ms. Without this, ADC interface fails for some reasons, even if we don't 
      // do any initialization in InitADCChannels().
      usleep(1000);
      
      if (Wait4DataReady(ucChannelNum))
      {
        DEBUG1("CFPD_ADC_7705::GetCount - DRDY status went low, Data is ready to read from ADC.");

        //Once again check whether Interface is broken, if so initialize ADC Channels
        if (InitADCChannels())
        {
          // Command Communication register to read Data register next
          if (SendCommand2ADC(ADC_READ_DATA_REG_NXT_CMD, ucChannelNum, true))
          {
            // Command to read data out of Data register
            char *pszDataReadCmd = "sn 0x0000";
            if (write(m_fd, pszDataReadCmd, strlen(pszDataReadCmd)) != (int)strlen(pszDataReadCmd))
            {
              bRet = false;
              DEBUG1("CFPD_ADC_7705::GetCount - Failed to write to data read command from Data Reg. of ADC!!!");
            }

            if (bRet)
            {
              // Read Data register
              char szData[DATA_BUFF_SZ];
              memset(szData, 0, DATA_BUFF_SZ);
              if (read(m_fd, szData, DATA_BUFF_SZ) > 0)
              {
                DEBUG1("CFPD_ADC_7705::GetCount - Read response %s", szData);
                sscanf(szData, "%hx", &usADCCount);
              }
              else
              {
                bRet = false;
                DEBUG1("CFPD_ADC_7705::GetCount - Failed to read Data Reg.!!!");
              }
            }
          }
          else
          {
            bRet = false;
            DEBUG1("CFPD_ADC_7705::GetCount - Failed to send command to read Data register.");
          }
        }
      }
      else
      {
        bRet = false;
        DEBUG1("CFPD_ADC_7705::GetCount - Wait4DataReady failed!!!");
      }
    }
    else
    {
      bRet = false;
      DEBUG1("CFPD_ADC_7705::GetCount - Failed setup ADC.");
    }

    ReleaseLock(fpLockFile, fl);
  }
  else
  {
    bRet = false;
    DEBUG1("CFPD_ADC_7705::GetCount - Device is not opened !!!");
  }

  return bRet;
}

bool CFPD_ADC_7705::GetFPDDACVoltage(float &fVolt)
{
  bool bRet = true;

  fVolt = 0;
  unsigned short usADCCount = 0;
  // Read twice, first reads bad if the previous read is of a different channel
  GetCount(1, usADCCount);
  if (GetCount(1, usADCCount))
  {
    //fVolt = (float)usADCCount * FPDADCRESOLUTION;
    fVolt = (float)(usADCCount - 32767) * 5.0 / 32767;
  }
  else
    bRet = false;
  
  return bRet;
}

bool CFPD_ADC_7705::GetFPDPreampVoltage(float &fVolt)
{
  bool bRet = true;

  fVolt = 0;
  unsigned short usADCCount = 0;
  // Read twice, first reads bad if the previous read is of a different channel
  GetCount(0, usADCCount);
  if (GetCount(0, usADCCount))
  {
    //fVolt = (float)usADCCount * FPDADCRESOLUTION;
    fVolt = (float)(usADCCount - 32767) * 5.0 / 32767;
  }
  else
    bRet = false;
  
  return bRet;
}

bool CFPD_ADC_7705::GetFPDDACCounts( unsigned short & usVal )
{
  bool bRet = true;

  unsigned short usADCCount = 0;
  // Read twice, first reads bad if the previous read is of a different channel
  GetCount(1, usADCCount);
  if (GetCount(1, usADCCount))
    usVal = usADCCount;
  else
    bRet = false;
  
  return bRet;
}

bool CFPD_ADC_7705::GetFPDPreampCounts(unsigned short &usVal)
{
  bool bRet = true;

  unsigned short usADCCount = 0;
  // Read twice, first reads bad if the previous read is of a different channel
  GetCount(0, usADCCount);
  if (GetCount(0, usADCCount))
    usVal = usADCCount;
  else
    bRet = false;
  
  return bRet;
}

bool CFPD_ADC_7705::IsADCReset()
{
  bool bRet = false;
  unsigned short usRegData = 0;
  ReadRegister(ADC_READ_CLOCK_REG_NXT_CMD, 0, usRegData);
  if (ADC_100HZ_CLOCK_REG_CMD != usRegData)
  {
    bRet = true;
  }   
  else
  {
    /*
    //SETUP register should read 0x0 after calibration is done.
    ReadRegister( 0x0, 0, usRegData );
    if( 0x0 != usRegData )
    {
    bRet = true;
    }
    */
  }

  return bRet;
}

bool CFPD_ADC_7705::ReadRegister(unsigned char ucCmmd, unsigned char ucChannelNum, unsigned short &usRegData)
{
  bool bRet = true;
  usRegData = 0;

  if (SendCommand2ADC(ucCmmd, ucChannelNum, true))
  {
    // Command to read data out of the register
    char *pszDataReadCmd = "bn 0x00";
    if (write(m_fd, pszDataReadCmd, strlen(pszDataReadCmd)) != (int)strlen(pszDataReadCmd))
    {
      bRet = false;
      DEBUG1("CFPD_ADC_7705::ReadRegister - Failed to write to data read command from register of ADC!!!\n");
    }

    if (bRet)
    {
      // Read Data register
      char szData[DATA_BUFF_SZ];
      memset(szData, 0, DATA_BUFF_SZ);
      if (read(m_fd, szData, DATA_BUFF_SZ) > 0)
      {
        DEBUG1("CFPD_ADC_7705::ReadRegister - Data Read = %s\n", szData);
        sscanf(szData, "%hx", &usRegData);
      }
      else
      {
        bRet = false;
        DEBUG1("CFPD_ADC_7705::ReadRegister - Failed to read register data!!!\n");
      }
    }
  }
  return bRet;
}
