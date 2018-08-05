/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: FIDOperations.cpp
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
 *  $Id: FIDOperations.cpp,v 1.12 2016/09/20 12:28:59 emrsn\rrthomas Exp $
 *  $Log: FIDOperations.cpp,v $
 *  Revision 1.12  2016/09/20 12:28:59  emrsn\rrthomas
 *  Fix for FID Auto Zero from Cycle Clock 1 from intefering with Auto Zero from Cycle Clock 2 and vice versa. State Variables that store the last DAC offset position on a per FID basis were declared static, so both FIDs were sharing these variables. Made them member variables of the class so that each instance can have its own copy.
 *
 *  Revision 1.11  2016/08/09 17:09:06  emrsn\jbeatty
 *  Support for 1500XA R2
 *
 *  Revision 1.10  2015/05/04 14:49:26  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.9  2014/06/26 10:00:19  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.8  2013/01/30 06:18:37  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.7  2012/03/21 06:48:50  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.6  2009/11/02 12:16:16  epm-na\rsaha
 *  Moved GetADC() functionality to CFID_ADC_AD7811YRU class.
 *
 *  Revision 1.5  2009/10/27 05:58:45  epm-na\rthomas
 *  Updated -
 *  1. AutoZero algo to consider R4=10K.
 *  2  Placed check condition to see whether DAC value to set is beyond its range, if so setting them to their min/max limits.
 *
 *  Revision 1.4  2009/09/10 05:12:01  epm-na\rsaha
 *  Revised computing of offset DAC o/p in NullElectrometer() algorithm to reflect the following changes -
 *  1. High gain resistor R8 changed from 10G to 5G.
 *  2. Low gain resistor R2 changed from 250M to 5M.
 *
 *  Revision 1.3  2009/08/27 14:28:15  epm-na\rsaha
 *  Updated AutoZero and NullElectrometer algo.
 *  Previously if the Pre-Amplifier voltage and Electrometer voltage are very close to their normalized value (4.2 and 0.0 respectively) then we used to skip performing AutoZero and NullElectormeter. Now removed this condition check.
 *
 *  Revision 1.2  2009/06/02 15:35:30  epm-na\rsaha
 *  Revised to return actual AOUT values.
 *
 *  Revision 1.1  2009/05/14 22:12:46  epm-na\rsaha
 *  Files to read/write FID DAC, ADC values and perform operations such as FID Null Electrometer, Auto Zero.
 *
 *************************************************************************/

#include "FIDOperations.h"
  
#include <debug.h>

#define FID1_OPR_LOCK "/tmp/FID1Operations_LOCK"
#define FID2_OPR_LOCK "/tmp/FID2Operations_LOCK"

CFIDOperations::CFIDOperations(int nFidNo) :
  m_obFIDDAC1(nFidNo),
  m_obFIDDAC2(nFidNo),
  m_obFIDADC(nFidNo)
{
  m_nFidNo = nFidNo;
  m_bInitNullElectrometer = true; 
  m_bInitAutoZero = true;
  m_fNullDACVolt = 0.0F; 
  m_fAutoZeroDACVolt = 0.0F; 
}

// -10V to +10V, 16bit DAC
static const float DACRESOLUTION = 20.0F / 65535.0F;

void CFIDOperations::AcquireLock(FILE *&fpLockFile, struct flock &fl)
{
  fl.l_type = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = 0;
  fl.l_len = 0;
  fl.l_pid = getpid();

  if (m_nFidNo == 1)
    fpLockFile = fopen(FID1_OPR_LOCK, "w+");
  else if (m_nFidNo == 2)
    fpLockFile = fopen(FID2_OPR_LOCK, "w+");
  else
    fpLockFile = NULL;
  if (fpLockFile)
  {
    if (fcntl(fpLockFile->_fileno, F_SETLKW, &fl) != 0)
    {
      // Treat it as a soft error don't stop executing
      DEBUG1("CFIDOperations::AcquireLock - Failed to lock !!!");
    }
  }
}

void CFIDOperations::ReleaseLock(FILE *fpLockFile, struct flock fl)
{
  if (fpLockFile)
  {
    fl.l_type = F_UNLCK;
    if (fcntl(fpLockFile->_fileno, F_SETLKW, &fl) != 0)
    {
      DEBUG1("CFIDOperations::ReleaseLock - Failed to unlock !!!");
    }
    fclose(fpLockFile);
  }
}


bool CFIDOperations::SetDAC1(float fVolt)
{
  unsigned short usDACCount = (unsigned short)((fVolt + 10.0) / DACRESOLUTION);
  if (m_obFIDDAC1.SetCount(usDACCount) < 0)
    return false;

  return true;
}

bool CFIDOperations::SetDAC2(float fVolt)
{
  unsigned short usDACCount = (unsigned short)((fVolt + 10.0) / DACRESOLUTION);
  if (m_obFIDDAC2.SetCount(usDACCount) < 0)
    return false;

  return true;
}

bool CFIDOperations::NullElectrometer(bool bHighGain)
{
  bool bRet = true;

  FILE *fpLockFile = NULL;
  struct flock fl;
  AcquireLock(fpLockFile, fl);

  if (m_bInitNullElectrometer)
  {
    m_bInitNullElectrometer = false;

    if (!SetDAC2(m_fNullDACVolt))
    {
      // Failed to set DAC
      DEBUG1("CFIDOperations::NullElectrometer - Failed to set DAC");
      bRet = false;
    }
    else
      usleep(10);
  }

  if (bRet)
  {
    bRet = false;
    while (true)
    {
      float fADCVolt;
      if (!m_obFIDADC.GetCHVoltage(1, fADCVolt))
      {
        // Failed to read ADC
        DEBUG1("CFIDOperations::NullElectrometer - Failed to read ADC");
        break;
      }

      DEBUG1("CFIDOperations::NullElectrometer - Read ADC CH 1 Volt - %f V", fADCVolt);

      if (fADCVolt >= 3.0)
      {
        DEBUG1("CFIDOperations::NullElectrometer - Below MIN");

        m_fNullDACVolt += 1;
        if (m_fNullDACVolt > 10)
        {
          // Went out of range, failed to do Null terminate
          DEBUG1("CFIDOperations::NullElectrometer - Failed, DAC is already to its MAX limit");
          break;
        }
      }
      else if (fADCVolt <= -3.0)
      {
        DEBUG1("CFIDOperations::NullElectrometer - Above MAX");

        m_fNullDACVolt -= 1;
        if (m_fNullDACVolt < -10)
        {
          // Went out of range, failed to do Null terminate
          DEBUG1("CFIDOperations::NullElectrometer - Failed, DAC is already to its MIN limit");
          break;
        }
      }
      else
      {
        // Following is the gain applied on fDACVolt to fADCVolt in ELECTROMETER
        // At FID High Gain, fDACVolt to fADCVolt gain is -(20:1), -(100G/5G)
        // At FID Low Gain, fDACVolt to fADCVolt gain is -(20000:1), -(100G/5M)
        if (bHighGain)
          m_fNullDACVolt = m_fNullDACVolt - (fADCVolt / -0.05);
        else
          m_fNullDACVolt = m_fNullDACVolt - (fADCVolt / -0.00005);

        // Check for a runaway condition! If we have reached the limits of the DAC output range, then
        // set the max range and hope for the best.
        if (m_fNullDACVolt > 10)
          m_fNullDACVolt = 10;
        else if (m_fNullDACVolt < -10)
          m_fNullDACVolt = -10;

        bRet = true;
      }

      DEBUG1("CFIDOperations::NullElectrometer - Setting DAC 2 voltage - %f V", m_fNullDACVolt);

      if (!SetDAC2(m_fNullDACVolt))
      {
        // Failed to set DAC
        DEBUG1("CFIDOperations::NullElectrometer - Failed to set DAC");
        break;
      }

      if (bRet)
        break;

      usleep(10);
    }
  }

  ReleaseLock(fpLockFile, fl);

  return bRet;
}

bool CFIDOperations::AutoZero()
{
  bool bRet = true;

  FILE *fpLockFile = NULL;
  struct flock fl;
  AcquireLock(fpLockFile, fl);

  if (m_bInitAutoZero)
  {
    m_bInitAutoZero = false;

    if (!SetDAC1(m_fAutoZeroDACVolt))
    {
      // Failed to set DAC
      DEBUG1("CFIDOperations::AutoZero - Failed to set DAC");
      bRet = false;
    }
    else
      usleep(10);
  }

  if (bRet)
  {
    bRet = false;
    while (true)
    {
      float fADCVolt = 0;
      // Get the ADC Channel - 2 reading
      if (!m_obFIDADC.GetCHVoltage(2, fADCVolt))
      {
        // Failed to read ADC
        DEBUG1("CFIDOperations::AutoZero - Failed to read ADC");
        break;
      }

      DEBUG1("CFIDOperations::AutoZero - Read ADC CH 2 voltage - %f V", fADCVolt);

      if (fADCVolt >= 7.2)
      {
        DEBUG1("CFIDOperations::AutoZero - Below MIN");

        m_fAutoZeroDACVolt += 1;
        if (m_fAutoZeroDACVolt > 10)
        {
          // Went out of range, failed to do Auto Zero
          DEBUG1("CFIDOperations::AutoZero - Failed, DAC is already to its MAX limit");
          break;
        }
      }
      else if (fADCVolt <= 1.2)
      {
        DEBUG1("CFIDOperations::AutoZero - Above MAX");

        m_fAutoZeroDACVolt -= 1;
        if (m_fAutoZeroDACVolt < -10)
        {
          // Went out of range, failed to do Auto Zero
          DEBUG1("CFIDOperations::AutoZero - Failed, DAC is already to its MIN limit");
          break;
        }
      }
      else
      {
        // Get the offset since in Balanced condition fADCVolt should be 4.2, hence
        fADCVolt = fADCVolt - 4.2;

        // Following is the gain applied on fDACVolt to fADCVolt in AUTO ZERO.
        // fDACVolt to fADCVolt gain is -(5:1), -(10K/2K)
        m_fAutoZeroDACVolt = m_fAutoZeroDACVolt - (fADCVolt / -0.2);
                
        // Check for a runaway condition! If we have reached the limits of the DAC output range, then
        // set the max range and hope for the best.
        if (m_fAutoZeroDACVolt > 10)
          m_fAutoZeroDACVolt = 10;
        else if (m_fAutoZeroDACVolt < -10)
          m_fAutoZeroDACVolt = -10;

        bRet = true;
      }

      DEBUG1("CFIDOperations::AutoZero - Setting DAC 1 voltage - %f V", m_fAutoZeroDACVolt);

      if (!SetDAC1(m_fAutoZeroDACVolt))
      {
        // Failed to set DAC
        DEBUG1("CFIDOperations::AutoZero - Failed to set DAC");
        break;
      }

      if (bRet)
        break;

      usleep(10);
    }
  }

  ReleaseLock(fpLockFile, fl);

  return bRet;
}
