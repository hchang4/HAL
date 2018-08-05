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
 *  $Id: FID_DAC_AD5570ARSZ.cpp,v 1.10 2016/09/20 12:25:56 emrsn\rrthomas Exp $
 *  $Log: FID_DAC_AD5570ARSZ.cpp,v $
 *  Revision 1.10  2016/09/20 12:25:56  emrsn\rrthomas
 *  Minor - change command format from decimal to hex to improve readability.
 *
 *  Revision 1.9  2016/08/09 17:09:06  emrsn\jbeatty
 *  Support for 1500XA R2
 *
 *  Revision 1.8  2015/05/04 14:49:26  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.7  2014/06/26 10:00:21  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.6  2013/01/30 06:18:37  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.5  2012/03/21 06:48:51  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.4  2009/10/27 06:05:17  epm-na\rsaha
 *  Revised to remember the last set DAC1 and DAC2 o/ps across process.
 *
 *  Revision 1.3  2009/06/02 15:32:25  epm-na\rsaha
 *  Corrected warning.
 *
 *  Revision 1.2  2009/05/15 17:22:13  epm-na\rsaha
 *  Changed FID ADC and DAC device path from /nvdata/ to /dev/.
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

#include "FID_DAC_AD5570ARSZ.h"

#define DATA_BUFF_SZ      256

// Soft links to "/dev/fiddac1/2 or fiddac3/4)"
#define FID1_DAC1_DEV_PATH "/dev/flame1dac1"
#define FID1_DAC2_DEV_PATH "/dev/flame1dac2"
#define FID2_DAC1_DEV_PATH "/dev/flame2dac1"
#define FID2_DAC2_DEV_PATH "/dev/flame2dac2"

unsigned short CFID_DAC1_AD5570ARSZ::m_usDAC1Count = 0;
unsigned short CFID_DAC2_AD5570ARSZ::m_usDAC2Count = 0;

CDAC_AD5570ARSZ::CDAC_AD5570ARSZ(const char *pszDevPath)
{
  m_fdDACTx = -1;
  m_fdDACRx = -1;

  m_usDACCount = 0;

  if (pszDevPath)
  {
    m_fdDACTx = open(pszDevPath, O_WRONLY);
    if (m_fdDACTx >= 0)
    {
      m_fdDACRx = open(pszDevPath, O_RDONLY | O_NONBLOCK);
      if (m_fdDACRx < 0)
      {
        DEBUG1("CDAC_AD5570ARSZ::CDAC_AD5570ARSZ - Fail to open %s for reading !!!", pszDevPath);

        close(m_fdDACTx);
        m_fdDACTx = -1;
      }
    }
    else
    {
      DEBUG1("CDAC_AD5570ARSZ::CDAC_AD5570ARSZ - Fail to open %s for writing !!!", pszDevPath);
    }
  }
  else
  {
    DEBUG1("CDAC_AD5570ARSZ::CDAC_AD5570ARSZ - Invalid device path !!!");
  }
}

CDAC_AD5570ARSZ::~CDAC_AD5570ARSZ()
{
  if (m_fdDACTx >= 0)
    close(m_fdDACTx);

  if (m_fdDACRx >= 0)
    close(m_fdDACRx);
}

int CDAC_AD5570ARSZ::SetCount(unsigned short usDACCount)
{
  int nRetVal = -1;
  if (m_fdDACTx >= 0)
  {
    char szTxData[DATA_BUFF_SZ] = {0};
    sprintf(szTxData, "sn 0x%x", usDACCount);
    if (write(m_fdDACTx, szTxData, strlen(szTxData)) == (int)strlen(szTxData))
    {
      // Keep the cache copy of the count
      m_usDACCount = usDACCount;
      nRetVal = 0;
    }
    else
    {
      DEBUG1("CDAC_AD5570ARSZ::SetCount - Failed to write !!!");
    }
  }
  else
  {
    DEBUG1("CDAC_AD5570ARSZ::SetCount - Device is not opened !!!");
  }
  return nRetVal;
}

int CDAC_AD5570ARSZ::GetCount(unsigned short &usDACCount)
{
  int nRetVal = -1;

  if (m_fdDACRx >= 0)
  {
    // read() call returns the data read over SPI during the last write. The read data from DAC is the
    // old shift register count. So if last time a new count was set, our read would return the old 
    // count not the current count.
    // To work around it we write the same count once again to DAC, so that the data read from DAC is 
    // the current count.
    // Again if consecutive read is performed without inbetween writes then read() returns 0 since
    // a read gives only available new cache copy. So a write is necessary.
    if (SetCount(m_usDACCount) == 0)
    {
      char szRxData[DATA_BUFF_SZ] = {0};
      if (read(m_fdDACRx, szRxData, DATA_BUFF_SZ) > 0)
      {
        sscanf(szRxData, "%hx", &usDACCount);
        nRetVal = 0;
      }
      else
      {
        DEBUG1("CDAC_AD5570ARSZ::GetCount - Failed to read !!!");
      }
    }
  }
  else
  {
    DEBUG1("CDAC_AD5570ARSZ::GetCount - Device is not opened !!!");
  }

  return nRetVal;
}

CFID_DAC1_AD5570ARSZ::CFID_DAC1_AD5570ARSZ(int nFidNo) : CDAC_AD5570ARSZ((nFidNo == 1) ? FID1_DAC1_DEV_PATH : FID2_DAC1_DEV_PATH)
{
}

CFID_DAC2_AD5570ARSZ::CFID_DAC2_AD5570ARSZ(int nFidNo) : CDAC_AD5570ARSZ((nFidNo == 1) ? FID1_DAC2_DEV_PATH : FID2_DAC2_DEV_PATH)
{
}

int CFID_DAC1_AD5570ARSZ::GetCount(unsigned short &usDACCount)
{
#ifdef DAC_OUT_CONNECTED_TO_MISO
  return CDAC_AD5570ARSZ::GetCount(usDACCount);
#else
  usDACCount = m_usDAC1Count;
  return 0;
#endif
}

int CFID_DAC2_AD5570ARSZ::GetCount(unsigned short &usDACCount)
{
#ifdef DAC_OUT_CONNECTED_TO_MISO
  return CDAC_AD5570ARSZ::GetCount(usDACCount);
#else
  usDACCount = m_usDAC2Count;
  return 0;
#endif
}

int CFID_DAC1_AD5570ARSZ::SetCount(unsigned short usDACCount)
{
  m_usDAC1Count = usDACCount;
  return CDAC_AD5570ARSZ::SetCount(usDACCount);
}

int CFID_DAC2_AD5570ARSZ::SetCount(unsigned short usDACCount)
{
  m_usDAC2Count = usDACCount;
  return CDAC_AD5570ARSZ::SetCount(usDACCount);
}
