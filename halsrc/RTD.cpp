/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: RTD.cpp
 * *
 * *  Description: RTD (temperature) device function access layer.
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
 * *  $Id: RTD.cpp,v 1.14 2016/01/08 20:32:58 emrsn\rrthomas Exp $
 * *  $Log: RTD.cpp,v $
 * *  Revision 1.14  2016/01/08 20:32:58  emrsn\rrthomas
 * *  Add ability to read PWM using the RTD enum for debugging purposes.
 * *
 * *  Revision 1.13  2015/05/04 14:49:28  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.12  2014/06/26 10:00:59  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.11  2013/01/30 06:18:50  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.10  2012/03/21 06:49:00  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.9  2009/02/24 17:09:47  epm-na\rsaha
 * *  Revised to display build number of the version information.
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
 * *  Revision 1.6  2007/03/13 08:39:40  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *  - Fixed data union variable names used to get / set heater control
 * *    P, I and D gains.
 * *  - Fixed Makefile in halsrc to use the library path.
 * *
 * *  Revision 1.5  2006/12/08 21:32:39  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include "HtrSolProtocol.h"
#include "FixEndian.h"
  
#include "debug.h"
#include "RTD.h"


CRTD::CRTD()  // Default Constructor
{
}

CRTD::~CRTD() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CRTD::OpenHal (char* pszDevName)
{
  if (NULL == pszDevName)
  {
    return ERR_INVALID_ARGS;
  }
  
  return CBaseDev::OpenHal (pszDevName, FALSE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CRTD::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Get Device Status
int CRTD::GetStatus (int* pnStatus)     // Pointer to write status to
{
  if (NULL == pnStatus)
  {
    return ERR_INVALID_ARGS;
  }

  return ERR_NOT_IMPLEMENTED;
}

int CRTD::GetBoardInfo (unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
                        unsigned char* byFirmwareVerMin,  // Firmware Version - Minor Number
                        unsigned char* byFirmwareVerBuild,  // Firmware Version - Build Number
                        unsigned char* byBoardRevision)   // Board Revision
{
  int nRetVal = ERR_SUCCESS;
  
  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_SOL_RTD_SYSINFO_STRUCT stResp = {0};
  
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
        SetCmdAckCommand(&stCmd.byCmdAck, BD_GET_SYSTEM_INFO);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *)&stResp,  // Response from remote board
                                                     sizeof(stResp));     // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof(stResp))
        {
          *byFirmwareVerMaj = stResp.stData.majorVer;
          *byFirmwareVerMin = stResp.stData.minorVer;
          *byFirmwareVerBuild = stResp.stData.buildVer;
          *byBoardRevision = stResp.stData.Revision;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CRTD::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CRTD::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CRTD::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CRTD::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CRTD::GetBoardInfo: Function called before Open Call!");
  }

  return nRetVal;
}

// Get temperature in Milli Degree C.
int CRTD::GetTempInMilliDegC (long* TempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_SOL_STRUCT stResp = {0};//TODO - check if we need a similar structure for RTD

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_RTD_FN_GET_RTD_TEMP_MDEG);
#if 0
        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobCAN->CANGetRemoteResp((unsigned char *) &stCmd, // Command
                                             sizeof (stCmd),        // Size of command
                                             (unsigned char *) &stResp, // Response from remote board
                                             sizeof (stResp));      // Size of expected response
#endif


        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CRTD::GetTempInMilliDegC: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.OnBdTempmDegC);
            *TempMilliDegC = stResp.stData.htrsolDataUnion.OnBdTempmDegC;
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CRTD::GetTempInMilliDegC: Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CRTD::GetTempInMilliDegC: CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CRTD::GetTempInMilliDegC: Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CRTD::GetTempInMilliDegC: Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CRTD::GetTempInMilliDegC: Function called before Open Call!");
  }

  return nRetVal;
}

// Get PWM of the Heater Channel associated with RTD in Milli-percent
int CRTD::GetPWMInMilliPercent (long* PWMMilliPercent)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_SOL_STRUCT stResp = {0};//TODO - check if we need a similar structure for RTD

  if (NULL == PWMMilliPercent)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_RTD_FN_GET_HTR_PWM_MPERCENT);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CRTD::GetPWMInMilliPercent: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.OnBdTempmDegC);
            *PWMMilliPercent = stResp.stData.htrsolDataUnion.OnBdTempmDegC;
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CRTD::GetPWMInMilliPercent: Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CRTD::GetPWMInMilliPercent: CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CRTD::GetPWMInMilliPercent: Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CRTD::GetPWMInMilliPercent: Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CRTD::GetPWMInMilliPercent: Function called before Open Call!");
  }

  return nRetVal;
}

