/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampStreamSim.h
 * *
 * *  Description: Preamp device function streaming data access layer.
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

#ifndef _PREAMPSTREAMSIM_H
#define _PREAMPSTREAMSIM_H
#include <string>
#include <Definitions.h>

class CDetNameToRawFileMapping
{
private: 
  struct DetNameToRawFileMappingStruct 
  {
    std::string sDevName;
    std::string sSimDetailsFile;
    std::string sRawDataPathTemplate;
  };
  
  enum SIMULATE_FILE_PATH_ENUM{
    GET_SIM_DETAILS_FILE_PATH, 
    GET_DET_DATA_FILE_PATH_TEMPLATE,
  };
    
  DetNameToRawFileMappingStruct m_stSimulatorFileName[6];

  std::string getPath (std::string sDetName, SIMULATE_FILE_PATH_ENUM eWhatPath)
  {
    std::string sFilePath; 

    for (int i = 0; i < 6; ++i)
    {
      if (m_stSimulatorFileName[i].sDevName == sDetName)
      {
        switch (eWhatPath)
        {
        case GET_SIM_DETAILS_FILE_PATH:
          sFilePath = m_stSimulatorFileName[i].sSimDetailsFile;
          break;

        case GET_DET_DATA_FILE_PATH_TEMPLATE:
          sFilePath = m_stSimulatorFileName[i].sRawDataPathTemplate;
          break; 
        }
        break;
      }
    }

    return sFilePath; 
  }

public: 
  
  CDetNameToRawFileMapping()
  {
    // Detector 1
#ifdef MODEL_700XA
    m_stSimulatorFileName[0].sDevName = "PREAMP_STR:SLOT_1:PREAMP_STR_1";
#else
    m_stSimulatorFileName[0].sDevName = "PREAMP_STR:ANALYZER_SLOT:PREAMP_STR_1";
#endif

#ifdef COMPILE_FOR_PC
    m_stSimulatorFileName[0].sSimDetailsFile = "/home/Daniel/det1";
    m_stSimulatorFileName[0].sRawDataPathTemplate = "/home/Daniel/%d_det1.raw";
#else
    m_stSimulatorFileName[0].sSimDetailsFile = "/nvdata/simulate/det1";
    m_stSimulatorFileName[0].sRawDataPathTemplate = "/nvdata/simulate/%d_det1.raw";
#endif
    
    // Detector 2
    m_stSimulatorFileName[1].sDevName = "PREAMP_STR:SLOT_1:PREAMP_STR_2";
#ifdef COMPILE_FOR_PC
    m_stSimulatorFileName[1].sSimDetailsFile = "/home/Daniel/det2";
    m_stSimulatorFileName[1].sRawDataPathTemplate = "/home/Daniel/%d_det2.raw";
#else
    m_stSimulatorFileName[1].sSimDetailsFile = "/nvdata/simulate/det2";
    m_stSimulatorFileName[1].sRawDataPathTemplate = "/nvdata/simulate/%d_det2.raw";
#endif
    
    // Detector 3
    m_stSimulatorFileName[2].sDevName = "PREAMP_STR:SLOT_3:PREAMP_STR_1";
#ifdef COMPILE_FOR_PC
    m_stSimulatorFileName[2].sSimDetailsFile = "/home/Daniel/det3";
    m_stSimulatorFileName[2].sRawDataPathTemplate = "/home/Daniel/%d_det3.raw";
#else
    m_stSimulatorFileName[2].sSimDetailsFile = "/nvdata/simulate/det3";
    m_stSimulatorFileName[2].sRawDataPathTemplate = "/nvdata/simulate/%d_det3.raw";
#endif
    
    // Detector 4
    m_stSimulatorFileName[3].sDevName = "PREAMP_STR:SLOT_3:PREAMP_STR_2";
#ifdef COMPILE_FOR_PC
    m_stSimulatorFileName[3].sSimDetailsFile = "/home/Daniel/det4";
    m_stSimulatorFileName[3].sRawDataPathTemplate = "/home/Daniel/%d_det4.raw";
#else
    m_stSimulatorFileName[3].sSimDetailsFile = "/nvdata/simulate/det4";
    m_stSimulatorFileName[3].sRawDataPathTemplate = "/nvdata/simulate/%d_det4.raw";
#endif
    
    // Detector 5
    m_stSimulatorFileName[4].sDevName = "PREAMP_STR:SLOT_5:PREAMP_STR_1";
#ifdef COMPILE_FOR_PC
    m_stSimulatorFileName[4].sSimDetailsFile = "/home/Daniel/det5";
    m_stSimulatorFileName[4].sRawDataPathTemplate = "/home/Daniel/%d_det5.raw";
#else
    m_stSimulatorFileName[4].sSimDetailsFile = "/nvdata/simulate/det5";
    m_stSimulatorFileName[4].sRawDataPathTemplate = "/nvdata/simulate/%d_det5.raw";
#endif
    
    // Detector 6
    m_stSimulatorFileName[5].sDevName = "PREAMP_STR:SLOT_5:PREAMP_STR_2";
#ifdef COMPILE_FOR_PC
    m_stSimulatorFileName[5].sSimDetailsFile = "/home/Daniel/det6";
    m_stSimulatorFileName[5].sRawDataPathTemplate = "/home/Daniel/%d_det6.raw";
#else
    m_stSimulatorFileName[5].sSimDetailsFile = "/nvdata/simulate/det6";
    m_stSimulatorFileName[5].sRawDataPathTemplate = "/nvdata/simulate/%d_det6.raw";
#endif
  }

  int IsSimulationEnabled(std::string sDetName)
  {
    std::string sSimFileDetailsPath; 
    int nNumDetDataFiles = 0; 

    sSimFileDetailsPath = getPath (sDetName, GET_SIM_DETAILS_FILE_PATH);

    if (!sSimFileDetailsPath.empty())
    {
      FILE* fp = fopen (sSimFileDetailsPath.c_str(), "r");
      if (fp != NULL)
      {
        fscanf (fp, "%d", &nNumDetDataFiles);
        fclose (fp);
      }
    }

    return nNumDetDataFiles;
  }

  std::string GetRawFileTemplatePath (std::string sDetName)
  {
    return getPath (sDetName, GET_DET_DATA_FILE_PATH_TEMPLATE);
  }
};

// Bridge Preamplifier
class CPreampStreamSim {

private:
  bool m_bIsDevOpen;
  bool m_bStreamingStarted;
  FILE* m_pRawDataFileFd; 
  std::string m_sDevName;

  int m_nBridgeData; 
  unsigned long long m_ullTimeStamp;

  int m_nNumDetSimulationFiles; // Number of Raw Data Files to use
  int m_nDetFileInUse; // Which file are we currently using? 

  static CDetNameToRawFileMapping m_oDetMapping; // One per process

  // Workaround to get inter-sample delay of 20ms between samples. If we have two 
  // detectors, then only one detector simulator needs to sleep. If both sleep then
  // our CGM slows down.
  static bool m_bDelayAssigned; 
  bool m_bOkToSleep;

  int OpenRawDataFile ();
  int CloseRawDataFile ();
  int ReadRawDataFile (int *Data, unsigned long long *TimeStamp);

public:
  CPreampStreamSim();  // Default Constructor
  
  ~CPreampStreamSim(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Starts / Stops broadcast. Start = 1 starts broadcast, Start = 0 stops broadcast.
  int SetBroadcastMode (unsigned char Start);

  // Read stream data. Specify timeout in milli-seconds
  int ReadStreamData (unsigned int *BridgeData, unsigned long long *TimeStamp);

  bool IsSimulationEnabled (std::string sDetName);

  std::string GetNextFile ();
};

#endif // #ifndef _PREAMPSTREAMSIM_H

