/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampStreamSim.cpp
 * *
 * *  Description: Preamp device function streaming data from a Raw data file 
 * *               for demos/simulations. 
 * *
 * *  Copyright:        Copyright (c) 2011-2012, 
 * *                    Rosemount Analytical 
 * *                    All Rights Reserved.
 * *
 * *  Operating System:  None.
 * *  Language:          'C++'
 * *  Target:            Gas Chromatograph Model GC700XP
 * *
 * *
 * *************************************************************************/

#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include "debug.h"
#include "PreampStreamSim.h"

CDetNameToRawFileMapping CPreampStreamSim::m_oDetMapping;
bool CPreampStreamSim::m_bDelayAssigned = false; 

double dTotalElapsedTime = 0.0;
int nTotalNumSamples = 0; 

CPreampStreamSim::CPreampStreamSim()  // Default Constructor
{
  m_bIsDevOpen = false;
  m_bStreamingStarted = false; 
  m_pRawDataFileFd = NULL; 
  m_nBridgeData = 0;
  m_ullTimeStamp = 0; 
  m_nNumDetSimulationFiles = 0;
  m_nDetFileInUse = 0;
  m_bOkToSleep = false; 
}

CPreampStreamSim::~CPreampStreamSim() // Destructor
{
  if (m_bIsDevOpen)
  {
    CloseHal();
  }
}

// Open the device. Returns 0 on success, negative error code on failure.
int CPreampStreamSim::OpenHal (char* pszDevName)
{
  int retValue = ERR_SUCCESS;

  if (pszDevName != NULL)
  {
    m_sDevName = pszDevName;
    m_bIsDevOpen = true; 
  }
  else
  {
    retValue = ERR_INVALID_ARGS;
  }

  return retValue;
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CPreampStreamSim::CloseHal ()
{
  if (m_bStreamingStarted)
  {
    //Close open files
    SetBroadcastMode(false); 
  }
  
  // Get ready for next "Open" call
  m_sDevName.clear();
  m_bIsDevOpen = false; 

  return ERR_SUCCESS;
}

int CPreampStreamSim::SetBroadcastMode (unsigned char Start)
{
  int nRetVal = ERR_SUCCESS;
    
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    if (1 == Start) // Start Broadcast
    {
      //Check if streaming has already been enabled...
      if (TRUE == m_bStreamingStarted)
      {
        nRetVal = ERR_INVALID_SEQ;
      }
      else
      {
        dTotalElapsedTime = 0; 
        nTotalNumSamples = 0; 
        
        nRetVal = OpenRawDataFile ();
        if (nRetVal == ERR_SUCCESS)
        {
          m_bStreamingStarted = TRUE;
          m_ullTimeStamp = 0;
          m_nBridgeData = 0; 
        }
        
        if (!m_bDelayAssigned) 
        {
          m_bDelayAssigned = true; // Turn ON for the 1st instance ONLY
          m_bOkToSleep = true;
        }

      }
    }
    else // (0 == Start) Stop Broadcast
    {
      CloseRawDataFile ();
      m_bStreamingStarted = FALSE;
      m_bDelayAssigned = false;
      m_bOkToSleep = false; 

      //printf ("Avg Elapsed Time = %g\n", dTotalElapsedTime/nTotalNumSamples);
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampStreamSim::SetBroadcastMode: Function called before Open Call!");
  }

  return nRetVal;
}

int CPreampStreamSim::ReadStreamData (unsigned int *BridgeData, unsigned long long *TimeStamp)
{
  return ReadRawDataFile ( (int*)BridgeData, TimeStamp);
}

int CPreampStreamSim::OpenRawDataFile ()
{
  std::string sFilePath = GetNextFile ();
  
  m_pRawDataFileFd = fopen (sFilePath.c_str(), "r");
  if (m_pRawDataFileFd != NULL)
  {
    return ERR_SUCCESS;
  }
  else
  {
    DEBUG1 ("File open failed. Error: %d, %s.", errno, strerror(errno));
    return ERR_INTERNAL_ERR;
  }
}

int CPreampStreamSim::CloseRawDataFile ()
{
  if (m_pRawDataFileFd != NULL) 
  {
    fclose (m_pRawDataFileFd);
    m_pRawDataFileFd = NULL;
  }

  return ERR_SUCCESS;
}

int CPreampStreamSim::ReadRawDataFile (int *Data, unsigned long long *TimeStamp)
{
  char szBuffer [200];
  timeval start, end, diff; 
  gettimeofday (&start, NULL);

  // Increment Timestamp, upper layer expects the timestamp to increment by 20ms for each sample
  m_ullTimeStamp += 20; 
  *TimeStamp = m_ullTimeStamp;

  // Initialize return value just in case we can't open Det Data File or if Anly Time > the data the file contains
  *Data = m_nBridgeData;
  
  if (m_pRawDataFileFd != NULL)
  {
    char* szTemp = fgets (szBuffer, 200, m_pRawDataFileFd);
    if (szTemp != NULL)
    {
      int nTs = 0, nValue = 0;
      sscanf (szBuffer, "%d %d", &nTs, &nValue);

      *Data = nValue; 
      m_nBridgeData = nValue; // Cache it in case Anly Time > Num of available samples in file. 
                              // We will latch last good value. 
    }
  }

  gettimeofday (&end, NULL);
  DiffTime (start, end, &diff);
  int nElapsedTime_us = diff.tv_sec * 1000000 + diff.tv_usec;
  if (nElapsedTime_us < 18000 && m_bOkToSleep) usleep (18000 - nElapsedTime_us);

  dTotalElapsedTime += nElapsedTime_us;
  ++nTotalNumSamples;

  return ERR_SUCCESS;
}

bool CPreampStreamSim::IsSimulationEnabled (std::string sDetName)
{
  // How many Raw Data Files should we cycle through? 
  m_nNumDetSimulationFiles = m_oDetMapping.IsSimulationEnabled(sDetName);

  return (m_nNumDetSimulationFiles > 0);
}

std::string CPreampStreamSim::GetNextFile ()
{
  char szDetFilePath[200];

  if (++m_nDetFileInUse > m_nNumDetSimulationFiles) 
    m_nDetFileInUse = 1;

  std::string sRawDataTemplatePath = m_oDetMapping.GetRawFileTemplatePath(m_sDevName);

  sprintf (szDetFilePath, sRawDataTemplatePath.c_str(), m_nDetFileInUse);
  DEBUG1 ("Next File to use: %d, %s\n", m_nDetFileInUse, szDetFilePath);

  return std::string (szDetFilePath);
}
