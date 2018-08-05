/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: PreampConfig.cpp
 * *
 * *  Description: Preamp device function configuration access layer.
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
 * *  $Id: PreampConfig.cpp,v 1.20 2015/06/18 09:46:25 EMRSN\jchekkala Exp $
 * *  $Log: PreampConfig.cpp,v $
 * *  Revision 1.20  2015/06/18 09:46:25  EMRSN\jchekkala
 * *  Auto Zero will be enabled for TCD 1 and TCD 2. SetAutoZero method will be allowed for both G1/G2 Boards.
 * *  Fixed Bug 8189 - Multiple issues in Detector daemon
 * *
 * *  Revision 1.19  2015/05/04 14:49:28  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.18  2014/12/02 11:38:57  emrsn\jchekkala
 * *  Manual Base Line Adjustment for Preamp G2 Boards will be disabled and the same will be enabled for Preamp G1 Boards.
 * *
 * *  Revision 1.17  2014/06/26 10:00:44  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.16  2013/01/30 06:18:46  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.15  2012/09/24 03:56:41  emrsn\rsaha
 * *  Revised to Enable/Disable 700XA Preamp IO AutoZero algorithm. Previously only Enable support was there in HAL.
 * *
 * *  Revision 1.14  2012/09/07 12:50:11  emrsn\rsaha
 * *  Revised with support to Enable preamp AutoZero and to read bridge left and right detector status.
 * *
 * *  Revision 1.13  2012/03/21 06:48:57  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.12  2009/11/09 13:25:00  epm-na\rsaha
 * *  Revised to check response command type with request command type. If they don't match then try sending the request again with leftover timeout interval.
 * *  NOTE - It is observed that at times we are receiving response of the previous command instead of the current, as the previous command gets timed-out due to heavy CPU usage by Pre-Amp IO card during Calibration. So even though before transmitting a request, we do flush our response pipe, at times the response of the previous command is arriving just after transmitting the current command.
 * *  Example - Pre-Amp scaling factor getting read as 0.0000.
 * *
 * *  Revision 1.11  2009/02/24 17:09:47  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.10  2008/11/17 22:14:04  epm-na\rsaha
 * *  Added interface to retrive low to high scaling factor from a preamp.
 * *
 * *  Revision 1.9  2008/05/22 06:32:59  EPM-NA\kdarji
 * *  - Added timeout parameter in all the methods which are used to read and write
 * *  from/in-to preamp board. Client application i.e. xpdetectord will tell the
 * *  amount of time, to wait for response from preamp board.
 * *  [Note: In case of client application will not provide timeout parameter,
 * *  then default timeout time is as it was previously i.e. HAL_DFLT_TIMEOUT
 * *  (300 MilliSeconds)]
 * *
 * *  Revision 1.8  2008/03/06 20:47:04  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.7  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.6  2007/04/26 08:57:00  epm-na\rthomas
 * *   - Added functions to read CAL status (in progress or not) through
 * *     either premap config or streaming objects.
 * *
 * *  Revision 1.5  2007/04/25 11:00:52  epm-na\rthomas
 * *   - Added ability to enable self calibration through preamp stream and
 * *     config objects.
 * *
 * *  Revision 1.4  2007/03/13 08:39:39  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *  - Fixed data union variable names used to get / set heater control
 * *    P, I and D gains.
 * *  - Fixed Makefile in halsrc to use the library path.
 * *
 * *  Revision 1.3  2006/12/08 21:32:39  epm-na\DPrabhakar
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
#include "PreampConfig.h"
#include "BoardSlotInfo.h"

CPreampConfig::CPreampConfig()  // Default Constructor
{
  m_iBoardRevision = -1;
}

CPreampConfig::~CPreampConfig() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CPreampConfig::OpenHal (char* pszDevName)
{
  if (NULL == pszDevName)
  {
    return ERR_INVALID_ARGS;
  }

  return CBaseDev::OpenHal (pszDevName, TRUE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CPreampConfig::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Get Device Status
int CPreampConfig::GetStatus (int* pnStatus)      // Pointer to write status to
{
  if (NULL == pnStatus)
  {
    return ERR_INVALID_ARGS;
  }
  else
  {
    return ERR_NOT_IMPLEMENTED;
  }
}

int CPreampConfig::GetBoardInfo (unsigned char* byFirmwareVerMaj,   // Firmware Version - Major Number
                                 unsigned char* byFirmwareVerMin, // Firmware Version - Minor Number
                                 unsigned char* byFirmwareVerBuild, // Firmware Version - Build Number
                                 unsigned char* byBoardRevision,  // Board Revision
                                 unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;
  
  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_SYSINFO_STRUCT stResp = {0};
  
  if ( (NULL == byFirmwareVerMaj) || (NULL == byFirmwareVerMin) || 
       (NULL == byFirmwareVerBuild) || (NULL == byBoardRevision))
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, BD_GET_SYSTEM_INFO);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                       sizeof (stCmd),        // Size of command
                                                       (unsigned char *)&stResp,  // Response from remote board
                                                       sizeof(stResp),      // Size of expected response
                                                       FALSE,      // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof(stResp))
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( BD_GET_SYSTEM_INFO == ucRxdRespCmdType )
            {
              *byFirmwareVerMaj = stResp.stData.majorVer;
              *byFirmwareVerMin = stResp.stData.minorVer;
              *byFirmwareVerBuild = stResp.stData.buildVer;
              *byBoardRevision = stResp.stData.Revision;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetBoardInfo(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetBoardInfo: Function called before Open Call!");
  }

  return nRetVal;

  
}

// Get Bridge current in Micro Amperes.
int CPreampConfig::GetBridgeCurrent (int* BridgeCurrentMicroAmps , 
                                     unsigned int unTimeOut) // Time to wait for response from remote device
{
  if (NULL == BridgeCurrentMicroAmps)
  {
    return ERR_INVALID_ARGS;
  }
  
  return ERR_NOT_IMPLEMENTED;
}

// Gets the Bridge gain adjustment.
int CPreampConfig::GetBridgeGain (PREAMP_BRIDGE_GAIN_ENUM *eBridgeGain, 
                                  unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eBridgeGain)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_GAIN);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,    // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_GAIN == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.Gain);
              *eBridgeGain = stResp.stData.preampData.Gain;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetBridgeGain(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfign:GetBridgeGain(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetBridgeGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetBridgeGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetBridgeGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetBridgeGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Sets the Bridge gain adjustment.
int CPreampConfig::SetBridgeGain (PREAMP_BRIDGE_GAIN_ENUM eBridgeGain, 
                                  unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0};
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};

  if ( (eBridgeGain < MIN_PREAMP_BRIDGE_GAIN) || 
       (eBridgeGain >= MAX_PREAMP_BRIDGE_GAIN) )
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_GAIN);

          stCmd.stData.preampData.Gain = eBridgeGain;   // Data

          FixEndian(stCmd.stData.preampData.Gain);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                       sizeof (stCmd),        // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp),     // Size of expected response
                                                       FALSE,       // Streaming data
                                                       unTimeOut);  // Time to wait for response 


          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_SET_GAIN == ucRxdRespCmdType )
            {
              // Check if the device ACK'd or NACK'd
              //if (stResp.stCmd.Err == 1)
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreampConfig::SetBridgeGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::SetBridgeGain(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::SetBridgeGain(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::SetBridgeGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::SetBridgeGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::SetBridgeGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::SetBridgeGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Sets the Sampling Rate
int CPreampConfig::GetSamplingRate (PREAMP_SAMPLING_RATE_ENUM *eSamplingRate, 
                                    unsigned int unTimeOut) // Time to wait for response from remote device
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eSamplingRate)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_SAMPLING_RATE);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,   // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_SAMPLING_RATE == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.SamplingRate);
              *eSamplingRate = stResp.stData.preampData.SamplingRate;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetSamplingRate(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetSamplingRate(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetSamplingRate(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetSamplingRate(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetSamplingRate(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetSamplingRate(): Function called before Open Call!");
  }

  return nRetVal;
}

// Gets the Sampling Rate
int CPreampConfig::SetSamplingRate (PREAMP_SAMPLING_RATE_ENUM eSamplingRate, 
                                    unsigned int unTimeOut) // Time to wait for response from remote device
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0};
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};

  if ( (eSamplingRate < MIN_PREAMP_SAMPLING_RATE) ||
       (eSamplingRate >= MAX_PREAMP_SAMPLING_RATE) )
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_SAMPLING_RATE);

          stCmd.stData.preampData.SamplingRate = eSamplingRate;   // Data

          FixEndian(stCmd.stData.preampData.SamplingRate);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),      // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp),   // Size of expected response
                                                       FALSE,  // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_SET_SAMPLING_RATE == ucRxdRespCmdType )
            {
              // Check if the device ACK'd or NACK'd
              //if (stResp.stCmd.Err == 1)
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreampConfig::SetSamplingRate(): Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::SetSamplingRate(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;
              
              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig:SetSamplingRate(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::SetSamplingRate(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::SetSamplingRate(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::SetSamplingRate(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::SetSamplingRate(): Function called before Open Call!");
  }

  return nRetVal;
}

// Get the current status of the AC bridge setting
int CPreampConfig::GetACBridgeStatus(PREAMP_STATE_ENUM *eACBridgeStatus,
                                     unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eACBridgeStatus)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_AC_BRIDGE);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE, // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_AC_BRIDGE == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.ACBridgeStatus);
              *eACBridgeStatus = stResp.stData.preampData.ACBridgeStatus;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetACBridgeStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;
              
              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetACBridgeStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetACBridgeStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetACBridgeStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetACBridgeStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetACBridgeStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Turn ON/OFF the AC Bridge
int CPreampConfig::SetACBridgeStatus(PREAMP_STATE_ENUM eACBridgeStatus, 
                                     unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0};
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};

  if ( (eACBridgeStatus < MIN_PREAMP_STATE) || 
       (eACBridgeStatus >= MAX_PREAMP_STATE) )
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_AC_BRIDGE);

          stCmd.stData.preampData.ACBridgeStatus = eACBridgeStatus;   // Data

          FixEndian(stCmd.stData.preampData.ACBridgeStatus);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,  // Streaming data
                                                       unTimeOut); // Time to wait for response 


          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_SET_AC_BRIDGE == ucRxdRespCmdType )
            {
              // Check if the device ACK'd or NACK'd
              //if (stResp.stCmd.Err == 1)
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreampConfig::SetACBridgeStatus(): Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::SetACBridgeStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;
              
              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::SetACBridgeStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::SetACBridgeStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::SetACBridgeStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfign:SetACBridgeStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::SetACBridgeStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Get filter ON/OFF status
int CPreampConfig::GetFilterStatus(PREAMP_STATE_ENUM *eFilterStatus,
                                   unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eFilterStatus)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_FILTER);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,  // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_FILTER == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.FilterStatus);
              *eFilterStatus = stResp.stData.preampData.FilterStatus;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetFilterStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetFilterStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetFilterStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetFilterStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetFilterStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetFilterStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Turn ON/OFF filter
int CPreampConfig::SetFilterStatus(PREAMP_STATE_ENUM eFilterStatus, 
                                   unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0};
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};

  if ( (eFilterStatus < MIN_PREAMP_STATE) || 
       (eFilterStatus >= MAX_PREAMP_STATE) )
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_FILTER);

          stCmd.stData.preampData.FilterStatus = eFilterStatus;   // Data

          FixEndian(stCmd.stData.preampData.FilterStatus);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,  // Streaming data
                                                       unTimeOut); // Time to wait for response 


          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_SET_FILTER == ucRxdRespCmdType )
            {
              // Check if the device ACK'd or NACK'd
              //if (stResp.stCmd.Err == 1)
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreampConfig::SetFilterStatus(): Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::SetFilterStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::SetFilterStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::SetFilterStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::SetFilterStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::SetFilterStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::SetFilterStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Turn on moving avg.
int CPreampConfig::GetMovingAvgStatus(PREAMP_STATE_ENUM *eMovAvgStatus,
                                      unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eMovAvgStatus)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_MOV_AVG);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,    // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_MOV_AVG == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.MovAvgStatus);
              *eMovAvgStatus = stResp.stData.preampData.MovAvgStatus;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetMovingAvgStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetMovingAvgStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetMovingAvgStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetMovingAvgStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetMovingAvgStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetMovingAvgStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Get moving avg. ON/OFF status
int CPreampConfig::SetMovingAvgStatus(PREAMP_STATE_ENUM eMovAvgStatus, 
                                      unsigned int unTimeOut) // Time to wait for response from remote device
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0};
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};

  if ( (eMovAvgStatus < MIN_PREAMP_STATE) || 
       (eMovAvgStatus >= MAX_PREAMP_STATE) )
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_MOV_AVG);

          stCmd.stData.preampData.MovAvgStatus = eMovAvgStatus;   // Data

          FixEndian(stCmd.stData.preampData.MovAvgStatus);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,    // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_SET_MOV_AVG == ucRxdRespCmdType )
            {
              // Check if the device ACK'd or NACK'd
              //if (stResp.stCmd.Err == 1)
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreampConfig::SetMovingAvgStatus(): Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::SetMovingAvgStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;
              
              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::SetMovingAvgStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::SetMovingAvgStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::SetMovingAvgStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::SetMovingAvgStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::SetMovingAvgStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Get the bridge offset value for the specified offset type
int CPreampConfig::GetBridgeOffset(PREAMP_OFFSET_TYPE eOffsetType, 
                                   PREAMP_BRIDGE_ADJ_ENUM *eOffsetValue,
                                   unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;
  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eOffsetValue)
  {
    return ERR_INVALID_ARGS;
  }

  if ( (eOffsetType < MIN_PREAMP_OFFSET_TYPE) || 
       (eOffsetType >= MAX_PREAMP_OFFSET_TYPE) )
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
        while( true )
        {
          unsigned char ucCmmdType;
          switch (eOffsetType)
          {
          default:
          case BRIDGE_LEFT_COARSE:
            SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_LEFT_COARSE);
            ucCmmdType = CMD_PREAMP_CFG_FN_GET_LEFT_COARSE;
            break;

          case BRIDGE_RIGHT_COARSE:
            SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_RIGHT_COARSE);
            ucCmmdType = CMD_PREAMP_CFG_FN_GET_RIGHT_COARSE;
            break;

          case BRIDGE_RIGHT_FINE:
            SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_RIGHT_FINE);
            ucCmmdType = CMD_PREAMP_CFG_FN_GET_RIGHT_FINE;
            break;
          }

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                       sizeof (stCmd),        // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp),     // Size of expected response
                                                       FALSE,        // Streaming data
                                                       unTimeOut);  // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( ucRxdRespCmdType == ucCmmdType )
            {
              FixEndian(stResp.stData.preampData.OffsetValue);
              *eOffsetValue = stResp.stData.preampData.OffsetValue;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetBridgeOffset(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;
              
              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetBridgeOffset(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetBridgeOffset(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetBridgeOffset(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetBridgeOffset(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetBridgeOffset(): Function called before Open Call!");
  }

  return nRetVal;
}

// Set the bridge offset value for the specified offset type
int CPreampConfig::SetBridgeOffset(PREAMP_OFFSET_TYPE eOffsetType, 
                                   PREAMP_BRIDGE_ADJ_ENUM eOffsetValue,
                                   unsigned int unTimeOut) // Time to wait for response from remote device
{
  DEBUG2("SetBridgeOffset() Called by pid (%d)\n", (int)getpid());
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_PREAMP_DATA_STRUCT stCmd = {0};
  CAN_CMD_PREAMP_STATUS_STRUCT stResp = {0};

  if ( (eOffsetValue < BRIDGE_ADJ_0000) || 
       (eOffsetValue >= NUM_BRIDGE_ADJ_ENUM) )
  {
    return ERR_INVALID_ARGS;
  }

  if ( (eOffsetType < MIN_PREAMP_OFFSET_TYPE) || 
       (eOffsetType >= MAX_PREAMP_OFFSET_TYPE) )
  {
    return ERR_INVALID_ARGS;
  }
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    if( !IsBaseLineAdjustable() )
    {
      DEBUG2("Preamp G2 Board. Doesnot Support Base Line Adjustment\n");
      return ERR_INVALID_ARGS;
    }
    else
    {
      DEBUG2("Preamp G1 Board. Let's Attempt Base Line Adjustment\n");
    }

    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        while( true )
        {
          unsigned char ucCmmdType;
          switch (eOffsetType)
          {
          default:
          case BRIDGE_LEFT_COARSE:
            SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_LEFT_COARSE);
            ucCmmdType = CMD_PREAMP_CFG_FN_SET_LEFT_COARSE;
            break;

          case BRIDGE_RIGHT_COARSE:
            SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_RIGHT_COARSE);
            ucCmmdType = CMD_PREAMP_CFG_FN_SET_RIGHT_COARSE;
            break;

          case BRIDGE_RIGHT_FINE:
            SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_RIGHT_FINE);
            ucCmmdType = CMD_PREAMP_CFG_FN_SET_RIGHT_FINE;
            break;
          }

          stCmd.stData.preampData.OffsetValue = eOffsetValue;   // Data

          FixEndian(stCmd.stData.preampData.OffsetValue);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp),// Size of expected response
                                                       FALSE,   // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( ucRxdRespCmdType == ucCmmdType )
            {
              // Check if the device ACK'd or NACK'd
              //if (stResp.stCmd.Err == 1)
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreamp:Stream:SetSamplingRate(): Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::SetSamplingRate(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;
              
              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::SetSamplingRate(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::SetSamplingRate(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }

          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::SetSamplingRate(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::SetSamplingRate(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::SetSamplingRate(): Function called before Open Call!");
  }

  return nRetVal;
}

// Gets the on-board temperature in Milli Degree C.
int CPreampConfig::GetOnBoardTemp (int* TempMilliDegC,
                                   unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == TempMilliDegC)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_ON_BD_TEMP);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,     // Streaming data
                                                       unTimeOut ); // Time to wait for response  

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_ON_BD_TEMP == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.OnBdTempmDegC);
              *TempMilliDegC = stResp.stData.preampData.OnBdTempmDegC;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetOnBoardTemp(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetOnBoardTemp(): Function called before Open Call!");
  }

  return nRetVal;
}


// Gets the current current preamp value.
int CPreampConfig::GetPreampValue (long *PreampValue,
                                   unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == PreampValue)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_PREAMP_VALUE);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,      // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_PREAMP_VALUE == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.PreampValue);
              *PreampValue = stResp.stData.preampData.PreampValue;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetPreampValue(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetPreampValue(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetPreampValue(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }

          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetPreampValue(): Unexpected invalid pointer!");
      }
      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetPreampValue(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetPreampValue(): Function called before Open Call!");
  }

  return nRetVal;
}


// Gets the scaling factor between low and high gain sub channels.
int CPreampConfig::GetScalingFactor( float * ScalingFactor, 
                                     unsigned int unTimeOut ) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == ScalingFactor)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_SCALE_FACTOR);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,      // Streaming data
                                                       unTimeOut); // Time to wait for response 


          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_SCALE_FACTOR == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.Low2HighGainScaleFactor);
              *ScalingFactor = stResp.stData.preampData.Low2HighGainScaleFactor;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetScalingFactor(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetScalingFactor(): Unexpected packet size: %d!", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetScalingFactor(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }

          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetScalingFactor(): Unexpected invalid pointer!");
      }
      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetScalingFactor(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetScalingFactor(): Function called before Open Call!");
  }

  return nRetVal;
}

// Enable self calibration
int CPreampConfig::EnableSelfCalibration(unsigned int unTimeOut) 
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_CAL_ON);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,   // Command
                                                       sizeof (stCmd),             // Size of command
                                                       (unsigned char *) &stResp,  // Response from remote board
                                                       sizeof (stResp),            // Size of expected response
                                                       FALSE,                      // Streaming data
                                                       unTimeOut);                 // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_SET_CAL_ON == ucRxdRespCmdType )
            {
              // Check if the device ACK'd or NACK'd
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreampConfig::EnableSelfCalibration: Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::EnableSelfCalibration(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::EnableSelfCalibration(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::EnableSelfCalibration(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
      
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::EnableSelfCalibration(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::EnableSelfCalibration(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::EnableSelfCalibration: Function called before Open Call!");
  }

  return nRetVal;
}

// Enable / Disable Auto Zero
int CPreampConfig::SetAutoZero( bool bEnable, unsigned int unTimeOut) 
{
  DEBUG2("SetAutoZero() Called by pid (%d)\n", (int)getpid());
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_PREAMP_CFG_FN_SET_AUTOZERO);

          if( bEnable )
          {
            stCmd.stData.preampData.AutoZero = PREAMP_STATE_ON;
          }
          else
          {
            stCmd.stData.preampData.AutoZero = PREAMP_STATE_OFF;
          }
          FixEndian(stCmd.stData.preampData.AutoZero);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,   // Command
                                                       sizeof (stCmd),             // Size of command
                                                       (unsigned char *) &stResp,  // Response from remote board
                                                       sizeof (stResp),            // Size of expected response
                                                       FALSE,                      // Streaming data
                                                       unTimeOut);                 // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_SET_AUTOZERO == ucRxdRespCmdType )
            {
              // Check if the device ACK'd or NACK'd
              if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
              {
                nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
                DEBUG2("CPreampConfig::EnableAutoZero: Device sent a NACK! Dev error code = %d!", nRetVal);
              }
              else
              {
                nRetVal = ERR_SUCCESS;
              }
            }
            else
            {
              DEBUG2( "CPreampConfig::EnableAutoZero(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::EnableAutoZero(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::EnableAutoZero(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
      
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::EnableAutoZero(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::EnableAutoZero(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::EnableAutoZero: Function called before Open Call!");
  }

  return nRetVal;
}

// Read calibration status (Calibration is in progress or not)
int CPreampConfig::GetCalOnOffStatus(PREAMP_STATE_ENUM *eCalInProgress,
                                     unsigned int unTimeOut) // Time to wait for response from remote device
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_CAL_STATUS);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,   // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_CAL_STATUS == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.CalInProgress);
              *eCalInProgress = stResp.stData.preampData.CalInProgress;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetCalOnOffStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetCalOnOffStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetCalOnOffStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetCalOnOffStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetCalOnOffStatus(): Invalid switch case!");
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

// Read Detector Bridge Right status 
int CPreampConfig::GetBRRightDetStatus(PREAMP_STATE_ENUM *eDetStatus,
                                       unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eDetStatus)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_BR_RIGHT_DET_STATUS);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,   // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_BR_RIGHT_DET_STATUS == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.DetectorStatus);
              *eDetStatus = stResp.stData.preampData.DetectorStatus;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetBRRightDetStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetBRRightDetStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetBRRightDetStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetBRRightDetStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetBRRightDetStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetBRRightDetStatus(): Function called before Open Call!");
  }

  return nRetVal;
}

// Read Detector Bridge Left status 
int CPreampConfig::GetBRLeftDetStatus(PREAMP_STATE_ENUM *eDetStatus,
                                      unsigned int unTimeOut) // Time to wait for response from remote device
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_PREAMP_DATA_STRUCT stResp = {0};

  if (NULL == eDetStatus)
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
        while( true )
        {
          SetCmdAckCommand(&stCmd.byCmdAck, CMD_PREAMP_CFG_FN_GET_BR_LEFT_DET_STATUS);

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                       sizeof (stCmd),    // Size of command
                                                       (unsigned char *) &stResp, // Response from remote board
                                                       sizeof (stResp), // Size of expected response
                                                       FALSE,   // Streaming data
                                                       unTimeOut); // Time to wait for response 

          // Check if we got the correct response packet
          if (nRetVal == sizeof (stResp))  
          {
            unsigned char ucRxdRespCmdType = GetCmdAckCommand( &stResp.stCmd.byCmdAck );
            if( CMD_PREAMP_CFG_FN_GET_BR_LEFT_DET_STATUS == ucRxdRespCmdType )
            {
              FixEndian(stResp.stData.preampData.DetectorStatus);
              *eDetStatus = stResp.stData.preampData.DetectorStatus;
              nRetVal = ERR_SUCCESS;
            }
            else
            {
              DEBUG2( "CPreampConfig::GetBRLeftDetStatus(): Response of a different Command - %d!", ucRxdRespCmdType );
              nRetVal = ERR_PROTOCOL;

              //Received response of a previously sent command.
              //Try sending the command again with remaining timeout interval.
              unTimeOut = m_pobReliabilityCAN->GetRemTimeOut();
              if( unTimeOut > 0 )
              {
                continue;
              }
            }
          }
          //Not the expected packet size
          else if (nRetVal >= 0)
          {
            DEBUG2("CPreampConfig::GetBRLeftDetStatus(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else // if (nRetVal < 0)
          {
            DEBUG2("CPreampConfig::GetBRLeftDetStatus(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
          
          break;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPreampConfig::GetBRLeftDetStatus(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPreampConfig::GetBRLeftDetStatus(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPreampConfig::GetBRLeftDetStatus(): Function called before Open Call!");
  }

  return nRetVal;
}


//Flushes the contents of the Named Pipe that is used for streaming data for this device.
int CPreampConfig::Flush()
{
  return ERR_NOT_IMPLEMENTED;
}

// Returns FALSE if G2 Preamp Board else TRUE. Device to be opened before this call.
bool CPreampConfig::IsBaseLineAdjustable( void )
{
  bool bIsBaseLineAdjustable = TRUE; // Def: G1 Board.
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    if( m_iBoardRevision < 0 )
    {
      // Get From current_software_info.txt. Once Only
      CBoardSlotInfo oCBoardSlotInfo;
      unsigned char ucCANId = CBaseDev::m_bySlotID;
      oCBoardSlotInfo.GetBoardRevision( "Preamp", &ucCANId, m_iBoardRevision );
    }
    
    if( m_iBoardRevision >= G2_PREAMP_BOARD )
    {
      // G1 Board. Base Line is Adjustable
      bIsBaseLineAdjustable = FALSE;
    }
  }

  return bIsBaseLineAdjustable;
}
