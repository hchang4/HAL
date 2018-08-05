/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FID_ADC_AD7811YRU.cpp
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
 *  $Id: FID_ADC_AD7811YRU.cpp,v 1.11 2017/03/28 16:21:23 emrsn\rrthomas Exp $
 *  $Log: FID_ADC_AD7811YRU.cpp,v $
 *  Revision 1.11  2017/03/28 16:21:23  emrsn\rrthomas
 *  Fix for FID railing issue seen on 1500XAR2 dual-FID GC. Issue occurs when we Auto-zero either FID detector. A couple of time in one day, the Preamp output goes to +100 million or -100 million counts.
 *
 *  This happens because the Auto Zero ADC occasionally reads an out of range value. The root cause of this was that (1) multiple tasks access the FID1 and FID2 ADC simultaenously (2) both the ADCs use a two step process - turn ON + setup the channel to read and then perform the actual conversion/read (3) Once an ADC is turned ON, it can talk on the bus irrespective of whether it gets a Chip Select on not. In short, the ADCs talk over each other when you simultanesouly access the ADCs from multiple tasks (such as detector daemon and hardware tev daemon).
 *
 *  To resolve the issue, use a single lock for the two ADCs. So block tasks from simultaenously accessing ADC1 and ADC2.
 *
 *  Also, changed the SPI polarity from Negative to Positive. The original code was written to work on the predecessor of the -055 CPU board that had issues with the SPI bus.
 *
 *  Revision 1.10  2016/08/09 17:09:06  emrsn\jbeatty
 *  Support for 1500XA R2
 *
 *  Revision 1.9  2015/05/04 14:49:26  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.8  2014/06/26 10:00:20  EMRSN\sboyidi
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
 *  Revision 1.6  2012/03/21 06:48:50  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.5  2009/11/02 12:18:42  epm-na\rsaha
 *  Added method GetCHVoltage() to return ADC Channel readings in volts.
 *
 *  Revision 1.4  2009/10/27 05:59:53  epm-na\rthomas
 *  Revised to use single Fd for read and write.
 *
 *  Revision 1.3  2009/06/02 15:32:25  epm-na\rsaha
 *  Corrected warning.
 *
 *  Revision 1.2  2009/05/15 17:22:13  epm-na\rsaha
 *  Changed FID ADC and DAC device path from /nvdata/ to /dev/
 *
 *  Revision 1.1  2009/05/14 22:12:46  epm-na\rsaha
 *  Files to read/write FID DAC, ADC values and perform operations such as FID Null Electrometer, Auto Zero.
 *
 *************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
  
#include "debug.h"

#include "FID_ADC_AD7811YRU.h"

#define DATA_BUFF_SZ       256

// Soft links to /dev/fidadc1(2)
#define FID1_ADC_DEV_PATH   "/dev/flame1adc"
#define FID2_ADC_DEV_PATH   "/dev/flame2adc"

#define ADC_CH_MASK        0xFCFF
#define ADC_CH_POS         8

// Control register Command format
// PD1 = 1, PD0 = 0 : Full Power down at the end of conversion, Vin4/!AGND = 0 : Ain are single ended referenced to analog ground (AGND),
// DIFF/!SGL = 0 : Ain are single ended referenced to analog ground (AGND), EXTREF = 1 : Use External refernce.
//   X*  |  A0  |  PD1  |  PD0  |  Vin4/!AGND  |  DIFF/!SGL  |  CH1  |  CH0  |  !CONVST  |  EXTREF  | 6 trailing 0s
//   0   |  0   |   1   |   0   |       0      |      0      |   0   |   0   |     0     |    1     |  000000
#define ADC_CTRL_REG_CFG   0x2040

// Control register Command format
// PD1 = 0, PD0 = 0 : Full Power down, Vin4/!AGND = 0 : Ain are single ended referenced to analog ground (AGND),
// DIFF/!SGL = 0 : Ain are single ended referenced to analog ground (AGND), EXTREF = 1 : Use External refernce.
//   X*  |  A0  |  PD1  |  PD0  |  Vin4/!AGND  |  DIFF/!SGL  |  CH1  |  CH0  |  !CONVST  |  EXTREF  | 6 trailing 0s
//   0   |  0   |   0   |   0   |       0      |      0      |   0   |   0   |     0     |    1     |  000000
#define ADC_DATA_READ_CMD  0x0040
#define ADC_DATA_READ_MASK 0xFFC0
#define ADC_DATA_POS       6

#define FID_ADC_LOCK_FILE "/tmp/FIDADCOperations_LOCK"

// 0V to +2.5V, 10bit ADC
// so    x    - 2.526 * x / 1024
static const float ADCRESOLUTION = (float)(2.5 / 1023);

CFID_ADC_AD7811YRU::CFID_ADC_AD7811YRU(int nFidNo)
{
  m_nFidNo = nFidNo;
  if (nFidNo == 1)
    m_fd = open(FID1_ADC_DEV_PATH, O_RDWR);
  else if (nFidNo == 2)
    m_fd = open(FID2_ADC_DEV_PATH, O_RDWR);
  else
    m_fd = -1;
  if (m_fd == -1)
  {
    DEBUG1("CFID_ADC_AD7811YRU::CFID_ADC_AD7811YRU - Fail to open FID %d [%s]", nFidNo, (nFidNo == 1) ? FID1_ADC_DEV_PATH : FID2_ADC_DEV_PATH);
  }
}

CFID_ADC_AD7811YRU::~CFID_ADC_AD7811YRU()
{
  if (m_fd != -1)
    close(m_fd);
}

void CFID_ADC_AD7811YRU::AcquireLock(FILE *&fpLockFile, struct flock &fl)
{
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  fl.l_pid = getpid();

  fpLockFile = fopen(FID_ADC_LOCK_FILE, "w+");
  if (fpLockFile)
  {
    if (fcntl(fpLockFile->_fileno, F_SETLKW, &fl) != 0)
    {
      // Treat it as a soft error don't stop executing
      DEBUG1("CFID_ADC_AD7811YRU::AcquireLock - Failed to lock");
    }
  }
}

void CFID_ADC_AD7811YRU::ReleaseLock(FILE *fpLockFile, struct flock fl)
{
  if (fpLockFile)
  {
    fl.l_type = F_UNLCK;
    if (fcntl(fpLockFile->_fileno, F_SETLKW, &fl) != 0)
    {
      DEBUG1("CFID_ADC_AD7811YRU::ReleaseLock - Failed to unlock !!!");
    }
    fclose(fpLockFile);
  }
}

void CFID_ADC_AD7811YRU::SetChannelNum(unsigned short &usTxCmd, int nChannelNum)
{
  usTxCmd = usTxCmd & ADC_CH_MASK;
  unsigned short usChannelNum = nChannelNum << ADC_CH_POS;
  usTxCmd = usTxCmd | usChannelNum;
}

int CFID_ADC_AD7811YRU::GetCount(int nChannelNum, unsigned short &usADCCount)
{
  int nRetVal = 0;
  if (nChannelNum >= 1 && nChannelNum <= 3)
  {
    nChannelNum--;

    if (m_fd != -1)
    {
      unsigned short usTxCmd = ADC_CTRL_REG_CFG;
      SetChannelNum(usTxCmd, nChannelNum);
      char szData[DATA_BUFF_SZ] = {0};
      sprintf(szData, "s 0x%x", usTxCmd);
      //DEBUG1("Channel %d - Write -  %s", nChannelNum + 1, szData);
      
      // ADC read involve 2 step process - a. Configure ADC Control Register b. Read Channel Data.
      // If two different objects of the this class talks at the same time then it will result in
      // corrupt ADC reads. To solve this -
      // Take a system lock across GC. This will prevent other objects of the same class to talk 
      // at the same time.
      FILE *fpLockFile = NULL;
      struct flock fl;
      AcquireLock(fpLockFile, fl);

      if (write(m_fd, szData, strlen(szData)) != (int)strlen(szData))
      {
        nRetVal = -1;
        DEBUG1("CFID_ADC_AD7811YRU::GetCount - Failed to write to command to Control Register");
      }

      // Flush data read during the write operation
      memset(szData, 0, DATA_BUFF_SZ);
      if (read(m_fd, szData, DATA_BUFF_SZ) > 0)
      {
        //DEBUG1("Data to Flush - %s", szData);
      }
      else
      {
        DEBUG1("CFID_ADC_AD7811YRU::GetCount - Failed to read");
      }

      if (nRetVal != -1)
      {
        usTxCmd = ADC_DATA_READ_CMD;
        SetChannelNum(usTxCmd, nChannelNum);
        memset(szData, 0, DATA_BUFF_SZ);
        sprintf(szData, "s %d", usTxCmd);
  
        // Wait for ADC to finish conversion
        usleep(5);

        if (write(m_fd, szData, strlen(szData)) != (int)strlen(szData))
        {
          nRetVal = -1;
          DEBUG1("CFID_ADC_AD7811YRU::GetCount - Failed to write command for reading ADC value");
        }
      }

      if (nRetVal != -1)
      {
        memset(szData, 0, DATA_BUFF_SZ);
        if (read(m_fd, szData, DATA_BUFF_SZ) > 0)
        {
          sscanf(szData, "%hx", & usADCCount);

          // Left shift once to get rid of the first garbage bit that was read 
          // in SPI. Refer ADC7811 datasheet. DOUT of ADC only comes on +ve edge of 
          // SCLK and DIN is read on -ve edge of SCLK. Since the ADC7811 SCLK remain 
          // high during idle time, the first +ve edge from SPI donot trigger any DOUT.
          //usADCCount = usADCCount << 1;
          usADCCount = usADCCount & ADC_DATA_READ_MASK;
          usADCCount = usADCCount >> ADC_DATA_POS;
        }
        else
        {
          nRetVal = -1;
          DEBUG1("CFID_ADC_AD7811YRU::GetCount - Failed to read");
        }
      }

      ReleaseLock(fpLockFile, fl);
    }
    else
    {
      nRetVal = -1;
      DEBUG1("CFID_ADC_AD7811YRU::GetCount - Device is not opened");
    }
  }
  else
  {
    nRetVal = -1;
    DEBUG1("CFID_ADC_AD7811YRU::GetCount - Invalid channel number - %d", nChannelNum);
  }

  return nRetVal;
}

bool CFID_ADC_AD7811YRU::GetCHVoltage(int nChannelNum, float &fVolt)
{
  bool bRet = true;

  fVolt = 0;
  unsigned short usADCCount = 0;
  if (GetCount(nChannelNum, usADCCount) < 0)
  {
    bRet = false;
  }
  else
  {
    fVolt = usADCCount * ADCRESOLUTION;

    switch (nChannelNum)
    {
    case 1:
      //fAOUT1 = fADCVolt * m + c;
      //fAOUT1, fADCVolt
      //    +3V,     0.0V
      //   0.0V,    +0.3V
      //    -3V,    +0.6V
      fVolt = (fVolt * -10) + 3;
      break;
    case 2:
      //fAOUT2 = fADCVolt * m + c;
      //fAOUT2, fADCVolt
      //  +7.2V,   +0.12V
      //  +4.2V,   +0.42V
      //  +1.2V,   +0.72V
      fVolt = (fVolt * -10) + 8.4;
      break;
    case 3:
      //fAOUT3 = fADCVolt * 100
      //fAOUT3, fADCVolt
      //  100V,     1.0V
      fVolt = fVolt * 100;
      break;
    default:
      bRet = false;
      DEBUG1("CFID_ADC_AD7811YRU::GetCHVoltage - Unknown ADC Channel !!!");
      break;
    }
  }

  return bRet;
}
