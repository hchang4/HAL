/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampStream.cpp
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
 * *  $Id: PreampStream.cpp,v 1.16 2016/01/14 21:32:01 emrsn\rrthomas Exp $
 * *  $Log: PreampStream.cpp,v $
 * *  Revision 1.16  2016/01/14 21:32:01  emrsn\rrthomas
 * *  Added a device type with which we can broadcast a Cycle Clock Start Message to all the Preamp boards at one shot. This will make it possible for Preaming Streaming objects present on different physical boards to start the analysis cycle at the exact same time. If we have four detectors in a Cycle Clock and if we use individual start messages to start each streaming object, then it is possible that they will start in the order in which we sent them the start messages. If for some reason one or more of these messages are delayed, we will end up with different detectors that start a different times.
 * *
 * *  For this added the following -
 * *  1. Added a way to associate each Preamp Streaming Object (or detector) with a Cycle Clock. Added a function to set the Cycle Clock for each detector.
 * *  2. New Function Type called Cycle Clock Sync. This will reside in the Preamp Boards (G1 and G2) and can be accessible as CAN address "1". All preamps will receive this message with a Cycle Clock number (1/2/3/4). When this message is recevied all the detectors associated with that particular cycle clock will start streaming data to the main CPU.
 * *
 * *  Revision 1.15  2015/05/04 14:49:28  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.14  2014/06/26 10:00:54  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.13  2013/01/30 06:18:48  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.12  2012/10/03 08:34:53  emrsn\rsaha
 * *  Revised to fix the overflow condtion of unsigned short time-stamp. In overflow case now substracting from 65536 instead of 65535, otherwise with overflow time gets in-correctly decremented by 1 milliseconds.
 * *
 * *  Revision 1.11  2012/03/21 06:48:59  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.10  2010/03/01 14:28:01  emrsn\rsaha
 * *  Revised Start Broadcast All algo to clear off the cache copy of the previous run samples to filter spikes. [same as Start Broadcast algo for individual detectors.] This is done to prevent previous run sample readings getting displayed in the next run.
 * *  The above problem was encountered while debugging FPD Auto spike at the end of the run for mode 0. And while testing as mode was changed to 1 the FPD AutoZero started appearing at the begining of the run for detector 2.
 * *
 * *  Revision 1.9  2009/09/09 19:56:48  EPM-NA\lratner
 * *  Modified the Preamp Streaming HAL object to detect and remove spurious spikes from the preamp data before sending it to peak_processing. This approach uses three points and computes the slope to identify spikes. Once a spike it identified, the bad sample is replaced with an average of the previous and next point.
 * *  Above-mentioned changes were made only in PreampStream.cpp and PreampStream.h, but I had to move #include <sstream> from h files to cpp, hence the other checkins! Without this, adding #include <vector> in PreampStream.h was causing several compilation errors related to redefinition of "min" and "max"!
 * *
 * *  Revision 1.8  2009/05/12 19:18:49  epm-na\rthomas
 * *  Bug fix - (1) SetAllChBroadcastMode always retuned error code "INTERNAL_ERROR" when starting / stopping broadcast. (2) Flag doInitOnly had inverse logic! (3) Implemented the "Flush" method. "Flush" method is used to clear the Stream Pipe before starting broadcast. This ensures that any old data from the previous cycle is flushed from the Queue before starting the new cycle.
 * *
 * *  Revision 1.7  2009/05/07 16:39:13  epm-na\rthomas
 * *  Starts/Stops broadcast for all channels in a preamp. Added this API to prevent the start time of the two channels from being affected by communication jitters. This API should be used in Mode 1 (1 Stream, 2 Detectors, 1 Method) to prevent a lag between detector 1 and 2.
 * *  But having said that, the 2nd Preamp Streaming object needs to do some initialization for beginning/stopping streaming. For such cases, call this API with the doInitOnly flag set to "TRUE"; and this should be done preferrably before starting broadcast and after stopping broadcast using the other channel. This will ensure the object is ready for streaming but the API in itself will not communicate with the Preamp device.
 * *
 * *  Revision 1.6  2008/03/06 20:47:04  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.5  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.4  2007/04/26 08:57:00  epm-na\rthomas
 * *   - Added functions to read CAL status (in progress or not) through
 * *     either premap config or streaming objects.
 * *
 * *  Revision 1.3  2007/04/25 11:00:52  epm-na\rthomas
 * *   - Added ability to enable self calibration through preamp stream and
 * *     config objects.
 * *
 * *  Revision 1.2  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include <string.h>
#include "PreampProtocol.h"
#include "FixEndian.h"
  
#include "debug.h"
#include "PreampStream.h"


#define MAX_DEV_TS_VALUE  65536

CPreampStream::CPreampStream()  // Default Constructor
  :  SPIKE_FILT_SAMPLE_SIZE (3), SPIKE_SLOPE_THRESHOLD (100000)
{
  m_ullTimeStamp = 0;
  m_unBridgeData = 0;
  m_usPrevTimeStamp = 0;
  m_usStartTimeStamp = 0;
  m_bRxFirstTimeStamp = FALSE;
  m_bStreamingStarted = FALSE;
}

CPreampStream::~CPreampStream() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CPreampStream::OpenHal (char* pszDevName)
{
  if (NULL == pszDevName)
  {
    return ERR_INVALID_ARGS;
  }

  return CBaseDev::OpenHal (pszDevName, TRUE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CPreampStream::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Starts / Stops broadcast. Start = 1 starts broadcast, Start = 0 stops broadcast.
int CPreampStream::SetBroadcastMode (unsigned char Start, bool doInitOnly)
{
  int nRetVal = ERR_SUCCESS;
    
  //CMD_ACK_STRUCT stCmd = {0};
  CmdAckUnion stCmd = {0}; 
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};
    
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
        
      // Transmit
      if (m_pobCAN)
      {
        //TODO - use macros
        if (1 == Start)
        {
          //Check if streaming has already been enabled...
          if (TRUE == m_bStreamingStarted)
          {
            nRetVal = ERR_INVALID_SEQ;
            break;
          }
          else
          {
            //Flush existing data
            Flush();
            m_bStreamingStarted = TRUE;
            m_ullTimeStamp = 0;
            m_usPrevTimeStamp = 0;
            m_usStartTimeStamp = 0;
            m_unBridgeData = 0;
            m_bRxFirstTimeStamp = TRUE;
            m_vstSpikeFiltData.clear();
            SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_STR_FN_EN_BROADCAST);
          }
        }
        else
        {
          m_bStreamingStarted = FALSE;
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_STR_FN_DIS_BROADCAST);
        }

        if (!doInitOnly)
        {

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                       sizeof (stCmd),             // Size of command
                                                       (unsigned char *) &stResp,  // Response from remote board
                                                       sizeof (stResp));           // Size of expected response
                    
          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            // Check if the device ACK'd or NACK'd
            if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
            {
              nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
              DEBUG2("CPreampStream::SetBroadcastMode: Device sent a NACK! Dev error code = %d!", nRetVal);
            }
            else
            {
              nRetVal = ERR_SUCCESS;
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampStream::SetBroadcastMode(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampStream::SetBroadcastMode(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampStream::SetBroadcastMode(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampStream::SetBroadcastMode(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampStream::SetBroadcastMode: Function called before Open Call!");
  }

  return nRetVal;
}

// Read stream data. Specify timeout in milli-seconds
int CPreampStream::ReadStreamData (unsigned int *BridgeData, unsigned long long *TimeStamp, unsigned int Timeout)
{
  int nRetVal = ERR_SUCCESS;
  PREAMP_STREAM_STRUCT stResp;
  SampleDataStruct stCurrentSample;

  if ( (NULL == BridgeData) || (NULL == TimeStamp) )
  {
    return ERR_INVALID_ARGS;
  }
    
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
        
      // Transmit
      if (m_pobCAN)
      {
        do 
        {
          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobCAN->CANRxStrmTimeout((unsigned char *) &stResp,  // Response from remote board
                                               sizeof (stResp), Timeout);  // Size of expected response

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            FixEndian(stResp.unBridgeData);
            FixEndian(stResp.usTimeStamp);
    
            UpdateTime(stResp.usTimeStamp);
    
            //*BridgeData = stResp.unBridgeData;
            //*TimeStamp = (m_ullTimeStamp - m_usStartTimeStamp);

            stCurrentSample.nBridgeVal = stResp.unBridgeData;
            stCurrentSample.ullTimestamp = m_ullTimeStamp - m_usStartTimeStamp;
            m_vstSpikeFiltData.push_back (stCurrentSample);
    
            nRetVal = ERR_SUCCESS;
          }
          else if (nRetVal < 0)// if (nRetVal <= 0)
          {
            DEBUG2("CPreampStream::ReadStreamData(): CCANComm::CANRxStrmTimeout failed with error code %d!", nRetVal);
          }
          else // if(nRetVal != sizeof (stResp))
          {
            DEBUG2("CPreampStream::ReadStreamData(): CCANComm::CANRxStrmTimeout - received unexpected number of bytes: %d !", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        } while (ERR_SUCCESS == nRetVal && SPIKE_FILT_SAMPLE_SIZE > m_vstSpikeFiltData.size());

        if (ERR_SUCCESS == nRetVal)
        {
          nRetVal = GetFilteredData (BridgeData, TimeStamp);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampStream::ReadStreamData(): Unexpected invalid pointer!");
      }
      break;
    
    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampStream::ReadStreamData(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampStream::ReadStreamData: Function called before Open Call!");
  }
    
  return nRetVal;
}

// Read stream data, block on read
int CPreampStream::ReadStreamDataBlocking (unsigned int *BridgeData, unsigned long long *TimeStamp)
{
  int nRetVal = ERR_SUCCESS;
  PREAMP_STREAM_STRUCT stResp;
   
  if ( (NULL == BridgeData) || (NULL == TimeStamp) )
  {
    return ERR_INVALID_ARGS;
  }
    
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
        
      // Transmit
      if (m_pobCAN)
      {
        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobCAN->CANRxStrmBlocking((unsigned char *) &stResp,  // Response from remote board
                                              sizeof (stResp));         // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.unBridgeData);
          FixEndian(stResp.usTimeStamp);

          UpdateTime(stResp.usTimeStamp);
          
          *BridgeData = stResp.unBridgeData;
          *TimeStamp = (m_ullTimeStamp - m_usStartTimeStamp);

          nRetVal = ERR_SUCCESS;
        }
        else if (nRetVal < 0)// if (nRetVal <= 0)
        {
          DEBUG2("CPreampStream::ReadStreamDataBlocking(): CCANComm::CANRxStrmBlocking failed with error code %d!", nRetVal);
        }
        else // if(nRetVal != sizeof (stResp))
        {
          DEBUG2("CPreampStream::ReadStreamDataBlocking(): CCANComm::CANRxStrmBlocking - received unexpected number of bytes: %d !", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampStream::ReadStreamDataBlocking(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPeampStream::ReadStreamDataBlocking(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampStream::ReadStreamDataBlocking: Function called before Open Call!");
  }

  return nRetVal;
}

//Enable self calibration
int CPreampStream::EnableSelfCalibration(void)
{
  int nRetVal = ERR_SUCCESS;
    
  //CMD_ACK_STRUCT stCmd = {0};
  CmdAckUnion stCmd = {0}; 
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};
    
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
        
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_STR_FN_SET_CAL_ON);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),             // Size of command
                                                     (unsigned char *) &stResp,  // Response from remote board
                                                     sizeof (stResp));           // Size of expected response
                    
        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CPreampStream::EnableSelfCalibration: Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CPreampStream::EnableSelfCalibration(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CPreampStream::EnableSelfCalibration(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampStream::EnableSelfCalibration(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampStream::EnableSelfCalibration(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampStream::EnableSelfCalibration: Function called before Open Call!");
  }

  return nRetVal;
}

// Read calibration status (CAN in progress or not)
int CPreampStream::GetCalOnOffStatus(PREAMP_STATE_ENUM *eCalInProgress)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eCalInProgress)
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_STR_FN_GET_CAL_STATUS);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.preampData.CalInProgress);
          *eCalInProgress = stResp.stData.preampData.CalInProgress;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CPreampStream::GetCalOnOffStatus(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CPreampStream::GetCalOnOffStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampStream::GetCalOnOffStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampStream::GetCalOnOffStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetCalOnOffStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Update the local time stamp counter (also takes care of roll-over condition)
void CPreampStream::UpdateTime(unsigned short usCurrentTimeStamp)
{
  //Update the total time stamp counter, taking roll-over into consideration.
  //  The time stamp from the board is 2 bytes long and hence rolls over 
  //  every MAX_DEV_TS_VALUE. But the time stamp counter maintained here
  //  is 8 bytes long and the total ON time is maintained by handling
  //  the roll-over condition.

  if (TRUE == m_bRxFirstTimeStamp)
  {
    m_bRxFirstTimeStamp = FALSE;
    m_usStartTimeStamp = usCurrentTimeStamp;
  }

  if (m_usPrevTimeStamp < usCurrentTimeStamp)
  {
    m_ullTimeStamp += (usCurrentTimeStamp - m_usPrevTimeStamp);
  }
  else if (m_usPrevTimeStamp > usCurrentTimeStamp)
  {
    m_ullTimeStamp += (MAX_DEV_TS_VALUE - (m_usPrevTimeStamp - usCurrentTimeStamp));
  }
  // else if (m_usPrevTimeStamp == usCurrentTimeStamp) - READ BELOW
  //If both are equal, it is considered a duplicate and not added
  //  to the total time. This is based on the assumption that 
  //  it will never take more than MAX_DEV_TS_VALUE to be able to
  //  receive the next packet.

  m_usPrevTimeStamp = usCurrentTimeStamp;
}

//Flushes the contents of the Named Pipe that is used for streaming data for this device.
int CPreampStream::Flush()
{
  if (m_pobCAN)
  {
    return m_pobCAN->CANFlushStreamPipe ();
  }
  else
  {
    return ERR_INTERNAL_ERR;
  }
}

// Starts / Stops broadcast for all channels in a preamp. Added this API to prevent 
// the start time of the two channels from being affected by communication jitters. This API
// should be used in Mode 1 (1 Stream, 2 Detectors, 1 Method) to prevent a lag between 
// detector 1 and 2.
// But having said that, the 2nd Preamp Streaming object needs to do some initialization
// for beginning streaming. For such cases, call this API with the doInitOnly flag set to "TRUE".
// This will ensure the object is ready for streaming but the API in itself will not communicate 
// with the Preamp device.
int CPreampStream::SetAllChBroadcastMode(unsigned char Start, bool doInitOnly /*= false*/)
{
  int nRetVal = ERR_SUCCESS;
    
  //CMD_ACK_STRUCT stCmd = {0};
  CmdAckUnion stCmd = {0}; 
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};
    
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      
      // Transmit
      if (m_pobCAN)
      {
        //TODO - use macros
        if (1 == Start)
        {
          //Check if streaming has already been enabled...
          if (TRUE == m_bStreamingStarted)
          {
            nRetVal = ERR_INVALID_SEQ;
            break;
          }
          else
          {
            //Flush existing data
            Flush();
            m_bStreamingStarted = TRUE;
            m_ullTimeStamp = 0;
            m_usPrevTimeStamp = 0;
            m_usStartTimeStamp = 0;
            m_unBridgeData = 0;
            m_bRxFirstTimeStamp = TRUE;
            m_vstSpikeFiltData.clear();
            SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_STR_FN_EN_BROADCAST_ALL);
          }
        }
        else
        {
          m_bStreamingStarted = FALSE;
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_STR_FN_DIS_BROADCAST_ALL);
        }

        if (!doInitOnly)
        {
  
          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                       sizeof (stCmd),             // Size of command
                                                       (unsigned char *) &stResp,  // Response from remote board
                                                       sizeof (stResp));           // Size of expected response
  
          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            // Check if the device ACK'd or NACK'd
            if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
            {
              nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
              DEBUG2("CPreampStream::SetBroadcastMode: Device sent a NACK! Dev error code = %d!", nRetVal);
            }
            else
            {
              nRetVal = ERR_SUCCESS;
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampStream::SetBroadcastMode(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampStream::SetBroadcastMode(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampStream::SetBroadcastMode(): Unexpected invalid pointer!");
      }
      break;
      
    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampStream::SetBroadcastMode(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampStream::SetBroadcastMode: Function called before Open Call!");
  }
  
  return nRetVal;
  
}


/*Spike Trap
  Search and remove random spike found in chromatograms. 
  This works on the following idea proposed by Skip Watkins based on several bad chromatograms
  seen from several GCs - All spikes ever detected consists of a sinle bad point (provided averaging and filtering is turned 
  off in the dual-TCD preamp board). This single bad point will have a large offset from the rest of the chromatogram (be it a 
  peak or the baseline). If we were to see the slope of the chromatogram, you would typically see a large slope associated with 
  spikes. It is not sufficient to check slope of successive points to detect and fix spikes; this is because valve upsets
  and backflush (the backflush process onto Det2 and not the backflush valve upset alone) can cause large swings in preamp ADC counts. 
  Hence, we need a smart way to detect and fix spikes. And here's how - 
  Detect Spikes - Rate of change of slope at a spike should be significant and the slope before the spike and the slope after the 
  spike would be of opposite sign.
  Fix - Identify the data point and replace with an average of the previous and next points. 
*/
int CPreampStream::GetFilteredData (unsigned int *BridgeData, unsigned long long *TimeStamp)
{
  int nRetVal = ERR_SUCCESS, nSlope1 = 0, nSlope2 = 0;

  if (SPIKE_FILT_SAMPLE_SIZE == m_vstSpikeFiltData.size())
  {
    // Compute slope with the three points
    nSlope1 = m_vstSpikeFiltData[1].nBridgeVal - m_vstSpikeFiltData[0].nBridgeVal;
    nSlope2 = m_vstSpikeFiltData[2].nBridgeVal - m_vstSpikeFiltData[1].nBridgeVal;
    
    // Check if there is a sike 
    if ( ( abs (nSlope1) >= SPIKE_SLOPE_THRESHOLD && abs (nSlope2) >= SPIKE_SLOPE_THRESHOLD) &&
         ( (nSlope1 < 0 && nSlope2 > 0) || (nSlope1 > 0 && nSlope2 < 0) ) )
    {
      // We trapped a spike! Fix it! 
      
      DEBUG1 ("SPIKE! %s, %d, %d, %d", 
              m_szDevName, 
              m_vstSpikeFiltData[0].nBridgeVal, 
              m_vstSpikeFiltData[1].nBridgeVal, 
              m_vstSpikeFiltData[2].nBridgeVal);

      // New value is the average of the points around the spike
      m_vstSpikeFiltData[1].nBridgeVal = (m_vstSpikeFiltData[0].nBridgeVal + m_vstSpikeFiltData[2].nBridgeVal) / 2;
      DEBUG1 ("SPIKE_FIX! Replacing with %d",  m_vstSpikeFiltData[1].nBridgeVal);
    }

    // Return data from the top of this vector and remove that element from our filter queue
    *BridgeData = m_vstSpikeFiltData[0].nBridgeVal;
    *TimeStamp = m_vstSpikeFiltData[0].ullTimestamp;

    m_vstSpikeFiltData.erase (m_vstSpikeFiltData.begin());
  }
  else
  {
    // Unexpected! This function should never be called without enough pionts to filter!
    nRetVal = ERR_INTERNAL_ERR;
  }
  return nRetVal;
}

// Set Cycle Clock associated with this detector.
int CPreampStream::SetCycleClock (unsigned int cycleClock)
{
  int nRetVal = ERR_SUCCESS;
    
  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0}; 
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};
    
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
        
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_STR_FN_SET_CYCLE_CLOCK);

        stCmd.stData.preampData.CycleClock = (CYCLE_CLOCK_ENUM)cycleClock;   // Data
        
        FixEndian(stCmd.stData.preampData.CycleClock);
        

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),             // Size of command
                                                     (unsigned char *) &stResp,  // Response from remote board
                                                     sizeof (stResp));           // Size of expected response
                    
        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CPreampStream::SetCycleClock: Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CPreampStream::SetCycleClock: Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CPreampStream::SetCycleClock: CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampStream::SetCycleClock: Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampStream::SetCycleClock: Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampStream::SetCycleClock: Function called before Open Call!");
  }

  return nRetVal;
}
