/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampStream.h
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
 * *  Revision History:
 * *  $Id: PreampStream.h,v 1.10 2016/01/14 21:32:02 emrsn\rrthomas Exp $
 * *  $Log: PreampStream.h,v $
 * *  Revision 1.10  2016/01/14 21:32:02  emrsn\rrthomas
 * *  Added a device type with which we can broadcast a Cycle Clock Start Message to all the Preamp boards at one shot. This will make it possible for Preaming Streaming objects present on different physical boards to start the analysis cycle at the exact same time. If we have four detectors in a Cycle Clock and if we use individual start messages to start each streaming object, then it is possible that they will start in the order in which we sent them the start messages. If for some reason one or more of these messages are delayed, we will end up with different detectors that start a different times.
 * *
 * *  For this added the following -
 * *  1. Added a way to associate each Preamp Streaming Object (or detector) with a Cycle Clock. Added a function to set the Cycle Clock for each detector.
 * *  2. New Function Type called Cycle Clock Sync. This will reside in the Preamp Boards (G1 and G2) and can be accessible as CAN address "1". All preamps will receive this message with a Cycle Clock number (1/2/3/4). When this message is recevied all the detectors associated with that particular cycle clock will start streaming data to the main CPU.
 * *
 * *  Revision 1.9  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.8  2014/06/26 10:01:17  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.7  2012/03/21 06:49:08  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.6  2009/09/09 19:56:48  EPM-NA\lratner
 * *  Modified the Preamp Streaming HAL object to detect and remove spurious spikes from the preamp data before sending it to peak_processing. This approach uses three points and computes the slope to identify spikes. Once a spike it identified, the bad sample is replaced with an average of the previous and next point.
 * *  Above-mentioned changes were made only in PreampStream.cpp and PreampStream.h, but I had to move #include <sstream> from h files to cpp, hence the other checkins! Without this, adding #include <vector> in PreampStream.h was causing several compilation errors related to redefinition of "min" and "max"!
 * *
 * *  Revision 1.5  2009/05/07 16:39:13  epm-na\rthomas
 * *  Starts/Stops broadcast for all channels in a preamp. Added this API to prevent the start time of the two channels from being affected by communication jitters. This API should be used in Mode 1 (1 Stream, 2 Detectors, 1 Method) to prevent a lag between detector 1 and 2.
 * *  But having said that, the 2nd Preamp Streaming object needs to do some initialization for beginning/stopping streaming. For such cases, call this API with the doInitOnly flag set to "TRUE"; and this should be done preferrably before starting broadcast and after stopping broadcast using the other channel. This will ensure the object is ready for streaming but the API in itself will not communicate with the Preamp device.
 * *
 * *  Revision 1.4  2007/04/26 08:56:42  epm-na\rthomas
 * *   - Added functions to read CAL status (in progress or not) through
 * *     either premap config or streaming objects.
 * *
 * *  Revision 1.3  2007/04/25 10:59:39  epm-na\rthomas
 * *   - Added ability to enable self calibration through preamp stream and
 * *     config objects.
 * *
 * *  Revision 1.2  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/




#ifndef _PREAMPSTREAM_H
#define _PREAMPSTREAM_H
#include <vector>
#include "BaseDev.h"
#include "PreampProtocol.h"

// Bridge Preamplifier
class CPreampStream : public CBaseDev {

private:
  unsigned long long m_ullTimeStamp;
  unsigned short m_usPrevTimeStamp;
  unsigned short m_usStartTimeStamp;
  unsigned int m_unBridgeData;
  BOOL m_bStreamingStarted;
  BOOL m_bRxFirstTimeStamp;

  // Number of samples for filtering spikes
  const std::size_t SPIKE_FILT_SAMPLE_SIZE;
  // Slope threshold to use for determining a spike
  const int SPIKE_SLOPE_THRESHOLD;
  // Structure to store Detector data for Spike filtering
  struct SampleDataStruct
  {
    int nBridgeVal; // Detector ADC counts
    unsigned long long ullTimestamp; // Data acq timestamp
  };
  
  // Holding area for storing detector data for performing spike trapping/elimination
  std::vector <SampleDataStruct> m_vstSpikeFiltData;

  // Traps the spike and returns a clean data point. If there is no spike, this returns
  // unmodified data point.
  int GetFilteredData (unsigned int *BridgeData, unsigned long long *TimeStamp);

  void UpdateTime(unsigned short usCurrentTimeStamp);

public:
  CPreampStream();  // Default Constructor
  
  ~CPreampStream(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Starts / Stops broadcast. Start = 1 starts broadcast, Start = 0 stops broadcast.
  int SetBroadcastMode (unsigned char Start, bool doInitOnly = false);

  // Starts / Stops broadcast for all channels in a preamp. Use only for Mode 1 (1 Stream, 2 Detectors, 1 Method) 
  int SetAllChBroadcastMode(unsigned char Start, bool doInitOnly);

  // Read stream data. Specify timeout in milli-seconds
  int ReadStreamData (unsigned int *BridgeData, unsigned long long *TimeStamp, unsigned int Timeout);

  // Read stream data, block on read
  int ReadStreamDataBlocking (unsigned int *BridgeData, unsigned long long *TimeStamp);

  // Enable self calibration
  int EnableSelfCalibration(void);

  // Read calibration status (CAN in progress or not)
  int GetCalOnOffStatus(PREAMP_STATE_ENUM *eCalInProgress);
  
  // Flushes the contents of the Named Pipe that is used for streaming data for this device.
  int Flush();

  // Set Cycle Clock associated with this detector.
  int SetCycleClock (unsigned int cycleClock);
};

#endif // #ifndef _PREAMPSTREAM_H

