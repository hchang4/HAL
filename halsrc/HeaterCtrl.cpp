/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: HeaterCtrl.cpp
 * *
 * *  Description: Heater control device function access layer.
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
 * *  $Id: HeaterCtrl.cpp,v 1.16 2016/08/09 17:09:06 emrsn\jbeatty Exp $

 * *  $Log: HeaterCtrl.cpp,v $
 * *  Revision 1.16  2016/08/09 17:09:06  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.15  2015/05/04 14:49:27  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.14  2014/06/26 10:00:24  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.13  2013/01/30 06:18:38  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.12  2012/03/21 06:48:52  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.11  2010/10/14 18:04:52  emrsn\rsaha
 * *  Added APIs to Get and Set Compensation Base temperature and Slope value.
 * *
 * *  Revision 1.10  2009/09/11 14:38:09  epm-na\rsaha
 * *  Added new command to set Heater type as  - AC or DC, in Heater Control Function.
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
 * *  Revision 1.6  2007/03/13 08:39:39  epm-na\rthomas
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

#include "FixEndian.h"
  
#include "debug.h"
#include "HeaterCtrl.h"

CHeaterCtrl::CHeaterCtrl()  // Default Constructor
{
}

CHeaterCtrl::~CHeaterCtrl() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CHeaterCtrl::OpenHal (char* pszDevName)
{
  if (NULL == pszDevName)
  {
    return ERR_INVALID_ARGS;
  }

  return CBaseDev::OpenHal (pszDevName, FALSE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CHeaterCtrl::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Get Device Status
int CHeaterCtrl::GetStatus (int* pnStatus)      // Pointer to write status to
{
  if (NULL == pnStatus)
  {
    return ERR_INVALID_ARGS;
  }

  return ERR_NOT_IMPLEMENTED;
}

int CHeaterCtrl::GetBoardInfo ( unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
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
          DEBUG2("CHeaterCtrl::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetBoardInfo: Function called before Open Call!");
  }

  return nRetVal;
}

// Set Channel Temp in milli degree C
int CHeaterCtrl::SetChTempMilliDegC (int nTempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        
        //stCmd.stCmd.Command = CMD_HTR_FN_SET_HTR_CTRL_ON_PID; // Command 
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_HTR_CTRL_ON_PID);
        
        stCmd.stData.htrsolDataUnion.TempmDegC = nTempMilliDegC;  // Data

        FixEndian(stCmd.stData.htrsolDataUnion.TempmDegC);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::SetChTempMilliDegC: Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SetChTempMilliDegC(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetChTempMilliDegC(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetChTempMilliDegC(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetChTempMilliDegC(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetChTempMilliDegC: Function called before Open Call!");
  }

  return nRetVal;
}

// Set Heater Channel PWM in milli %
int CHeaterCtrl::SetChPWMMilliP (int nTempMilliP)
{
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        //stCmd.stCmd.Command = CMD_HTR_FN_SET_HTR_CTRL_ON_PWM; // Command 
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_HTR_CTRL_ON_PWM);
        
        stCmd.stData.htrsolDataUnion.PWMmPct = nTempMilliP;   // Data

        FixEndian(stCmd.stData.htrsolDataUnion.PWMmPct);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SeChPWMMIlliP(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetChPWMMilliP(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetChPWMMilliP(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetChPWMMilliP(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetChPWMMilliP: Function called before Open Call!");
  }

  return nRetVal;
}

int CHeaterCtrl::TurnHtrOff () // Turn Heater OFF
{
  
  int nRetVal = ERR_SUCCESS;

  //CMD_ACK_STRUCT stCmd = {0};
  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_SET_HTR_CTRL_OFF);
        //stCmd.stCmdAck.Command = CMD_HTR_FN_SET_HTR_CTRL_OFF; // Command 

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::TurnHtrOff: Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::TurnHtrOff(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::TurnHtrOff(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::TurnHtrOff(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::TurnHtrOff(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::TurnHtrOff: Function called before Open Call!");
  }

  return nRetVal;
}

// Get Heater PWM in milli %
int CHeaterCtrl::GetHtrChPWMMilliP (unsigned int* pnPWMMilliP)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == pnPWMMilliP)
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
        //stCmd.Command = CMD_HTR_FN_GET_HTR_PWM; // Command 
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_HTR_PWM);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.PWMmPct);
            *pnPWMMilliP = stResp.stData.htrsolDataUnion.PWMmPct;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP: Device sent a ACK, but response packet size is invalid!: %d", 
                   nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP: Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetHtrChPWMMilliP: Function called before Open Call!");
  }

  return nRetVal;
}

// Get heater current in Micro Amps.
int CHeaterCtrl::GetHtrCurrent (int* pnHtrCurrentMicroAmps)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};
  
  if (NULL == pnHtrCurrentMicroAmps)
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
        //stCmd.Command = CMD_HTR_FN_GET_HTR_TOT_CURRENT; // Command 
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_HTR_TOT_CURRENT);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHtrCurrent: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            *pnHtrCurrentMicroAmps = stResp.stData.htrsolDataUnion.HtrCurrent;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CMD_ACK_STRUCT))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)

          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHtrCurrent: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetHtrCurrent: Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetHtrCurrent: Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetHtrCurrent(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetHtrCurrent(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetHtrCurrent(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetHtrCurrent: Function called before Open Call!");
  }

  return nRetVal;
}

// Set the proportional gain (milli
int CHeaterCtrl::SetChPropGain(unsigned long ulPropGainMilli)
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        //stCmd.stCmd.Command = CMD_HTR_FN_SET_HTR_CTRL_ON_PWM; // Command 
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_PROP_GAIN);
        
        stCmd.stData.htrsolDataUnion.PropGain = ulPropGainMilli;    // Data

        FixEndian(stCmd.stData.htrsolDataUnion.PropGain);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::SetChPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SetChPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetChPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetChPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetChPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetChPropGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Set the integral gain (milli)    
int CHeaterCtrl::SetChIntGain(unsigned long ulIntGainMilli)
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        //stCmd.stCmd.Command = CMD_HTR_FN_SET_HTR_CTRL_ON_PWM; // Command 
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_INT_GAIN);
        
        stCmd.stData.htrsolDataUnion.IntGain = ulIntGainMilli;    // Data

        FixEndian(stCmd.stData.htrsolDataUnion.IntGain);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::SetChIntGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SetChIntGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetChIntGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetChIntGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetChIntGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetChIntGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Set the differential gain (direct value)
int CHeaterCtrl::SetChDiffGain(unsigned long ulDiffGain)
{
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        //stCmd.stCmd.Command = CMD_HTR_FN_SET_HTR_CTRL_ON_PWM; // Command 
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_DIFF_GAIN);
        
        stCmd.stData.htrsolDataUnion.DiffGain = ulDiffGain;   // Data

        FixEndian(stCmd.stData.htrsolDataUnion.DiffGain);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::SetChDiffGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SetChDiffGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetChDiffGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetChDiffGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetChDiffGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetChDiffGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Get Temp in milli degree C
int CHeaterCtrl::GetHtrChTempMilliDegC (int* pnTempMilliDegC) 
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == pnTempMilliDegC)
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
        //stCmd.Command = CMD_HTR_FN_GET_TEMP_MDEG; // Command 
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_TEMP_MDEG);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.TempmDegC);
            *pnTempMilliDegC = stResp.stData.htrsolDataUnion.TempmDegC;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_PROTOCOL;
            DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC: Device sent a ACK, but response packet size is invalid!: %d", 
                   nRetVal);
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC: Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC: Function called before Open Call!");
  }

  return nRetVal;
}

// Get Sensor Status (OK, ERR)
int CHeaterCtrl::GetRTDStatus (int* pnStatus) 
{
  if (NULL == pnStatus)
  {
    return ERR_INVALID_ARGS;
  }

  return ERR_NOT_IMPLEMENTED;
}

// Get the on-board temp in Milli Degree C.
int CHeaterCtrl::GetOnBoardTemp (int* pnTempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == pnTempMilliDegC)
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
        //stCmd.Command = CMD_HTR_FN_GET_ON_BD_TEMP;  // Command 
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_ON_BD_TEMP);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetOnBoardTemp: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.OnBdTempmDegC);
            *pnTempMilliDegC = stResp.stData.htrsolDataUnion.OnBdTempmDegC;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetOnBoardTemp: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetOnBoardTemp: Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetHtrChTempMilliDegC: Function called before Open Call!");
  }

  return nRetVal;

}

int CHeaterCtrl::GetChPropGain(unsigned long *pulPropGainMilli)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == pulPropGainMilli)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_PROP_GAIN);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.PropGain);
            *pulPropGainMilli = stResp.stData.htrsolDataUnion.PropGain;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetChPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetChPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetChPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetChPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetChPropGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Set the integral gain (milli)
int CHeaterCtrl::GetChIntGain(unsigned long *pulIntGainMilli)
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == pulIntGainMilli)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_INT_GAIN);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp,         // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.IntGain);
            *pulIntGainMilli = stResp.stData.htrsolDataUnion.IntGain;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetChPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetChPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetChPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetChPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetChPropGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Set the differential gain (direct value)
int CHeaterCtrl::GetChDiffGain(unsigned long *pulDiffGain)
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == pulDiffGain)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_DIFF_GAIN);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.DiffGain);
            *pulDiffGain = stResp.stData.htrsolDataUnion.DiffGain;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetChPropGain(): Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetChPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetChPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetChPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetChPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetChPropGain(): Function called before Open Call!");
  }

  return nRetVal;
}

// Set Heater type - AC or DC
int CHeaterCtrl::SetHeaterType( HEATER_TYPE eHtrType )
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_HTR_TYPE);
        
        stCmd.stData.htrsolDataUnion.HtrType = eHtrType;

        FixEndian(stCmd.stData.htrsolDataUnion.HtrType);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),          // Size of command
                                                     (unsigned char *) &stResp,     // Response from remote board
                                                     sizeof (stResp));        // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::SetHeaterType(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SetHeaterType(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetHeaterType(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetHeaterType(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetHeaterType(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetHeaterType(): Function called before Open Call!");
  }

  return nRetVal;

}

// Get Heater type - AC or DC
int CHeaterCtrl::GetHeaterType( HEATER_TYPE * p_eHtrType )
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == p_eHtrType)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_HTR_TYPE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),          // Size of command
                                                     (unsigned char *) &stResp,     // Response from remote board
                                                     sizeof (stResp));        // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHeaterType(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.HtrType);
            * p_eHtrType = (HEATER_TYPE) stResp.stData.htrsolDataUnion.HtrType;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          //if (stResp.stCmd.Err == 1)
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetHeaterType(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetHeaterType(): Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetHeaterType(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetHeaterType(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetHeaterType(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetHeaterType(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetHeaterType(): Function called before Open Call!");
  }

  return nRetVal;
}


// Set Compensation Base Temperature.
int CHeaterCtrl::SetCompBaseTemp( int nCompBaseTempMilliDegC )
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_COMP_BASE_TEMP);
        
        stCmd.stData.htrsolDataUnion.CompBaseTemp = nCompBaseTempMilliDegC;

        FixEndian(stCmd.stData.htrsolDataUnion.CompBaseTemp);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),          // Size of command
                                                     (unsigned char *) &stResp,     // Response from remote board
                                                     sizeof (stResp));        // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::SetCompBaseTemp(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SetCompBaseTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetCompBaseTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetCompBaseTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetCompBaseTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetCompBaseTemp(): Function called before Open Call!");
  }

  return nRetVal;

}

// Get Compensation Base Temperature
int CHeaterCtrl::GetCompBaseTemp( int * p_nCompBaseTempMilliDegC )
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == p_nCompBaseTempMilliDegC)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_COMP_BASE_TEMP);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),          // Size of command
                                                     (unsigned char *) &stResp,     // Response from remote board
                                                     sizeof (stResp));        // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetCompBaseTemp(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.CompBaseTemp);
            * p_nCompBaseTempMilliDegC = stResp.stData.htrsolDataUnion.CompBaseTemp;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetCompBaseTemp(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetCompBaseTemp(): Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetCompBaseTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetCompBaseTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetCompBaseTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetCompBaseTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetCompBaseTemp(): Function called before Open Call!");
  }

  return nRetVal;
}


// Mark Compensation Base Temperature.
int CHeaterCtrl::MarkCompBaseTemp()
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_MARK_COMP_BASE_TEMP);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),          // Size of command
                                                     (unsigned char *) &stResp,     // Response from remote board
                                                     sizeof (stResp));        // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::MarkCompBaseTemp(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::MarkCompBaseTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::MarkCompBaseTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::MarkCompBaseTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::MarkCompBaseTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::MarkCompBaseTemp(): Function called before Open Call!");
  }

  return nRetVal;

}

// Set Compensation Temperature Slope.
int CHeaterCtrl::SetCompTempSlope( float fCompTempSlope )
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_CMD_HTR_STRUCT stCmd = {0};
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_HTR_FN_SET_COMP_TEMP_SLOPE);
        
        stCmd.stData.htrsolDataUnion.CompTempSlope = fCompTempSlope;

        FixEndian(stCmd.stData.htrsolDataUnion.CompTempSlope);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),          // Size of command
                                                     (unsigned char *) &stResp,     // Response from remote board
                                                     sizeof (stResp));        // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::SetCompTempSlope(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::SetCompTempSlope(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::SetCompTempSlope(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetCompTempSlope(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::SetCompTempSlope(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetCompTempSlope(): Function called before Open Call!");
  }

  return nRetVal;

}


// Get Compensation Temperature Slope
int CHeaterCtrl::GetCompTempSlope( float * p_fCompTempSlope )
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_CMD_HTR_STRUCT stResp = {0};

  if (NULL == p_fCompTempSlope)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_HTR_FN_GET_COMP_TEMP_SLOPE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),          // Size of command
                                                     (unsigned char *) &stResp,     // Response from remote board
                                                     sizeof (stResp));        // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetCompTempSlope(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            FixEndian(stResp.stData.htrsolDataUnion.CompTempSlope);
            * p_fCompTempSlope = stResp.stData.htrsolDataUnion.CompTempSlope;
            nRetVal = ERR_SUCCESS;
          }
        }
        else if (nRetVal == sizeof (CmdAckUnion))
        {
          // Device did not understand our command and sent us a NACK
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.htrsolDataUnion.ErrorType);
            DEBUG2("CHeaterCtrl::GetCompTempSlope(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            DEBUG2("CHeaterCtrl::GetCompTempSlope(): Device sent a ACK, but response packet size is invalid!: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CHeaterCtrl::GetCompTempSlope(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CHeaterCtrl::GetCompTempSlope(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::GetCompTempSlope(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CHeaterCtrl::GetCompTempSlope(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::GetCompTempSlope(): Function called before Open Call!");
  }

  return nRetVal;
}

typedef struct
{
  CAN_CMD_HTR_STRUCT s;
  unsigned char ucRTDndx;
} 
#ifndef WIN32
  __attribute (( packed )) 
#endif 
  CAN_CMD_HTR_RTD_LEAD_STRUCT;

// Set RTD lead resistor value
int CHeaterCtrl::SetRTDleadR(unsigned char rtdNdx, float fValue)
{
  int nRetVal = ERR_SUCCESS;
  CAN_CMD_HTR_RTD_LEAD_STRUCT stCmd;
  CAN_CMD_HTR_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
      case CAN_COMM:
        if (m_pobCAN)
        {
          SetCmdAckCommand(&stCmd.s.stCmd.byCmdAck, CMD_HTR_FN_SET_RTD_LEAD_RESIST);
          stCmd.s.stData.htrsolDataUnion.CompTempSlope = fValue;
          FixEndian(stCmd.s.stData.htrsolDataUnion.LeadResistance);
          stCmd.ucRTDndx = rtdNdx;

          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *)&stCmd, sizeof(stCmd), (unsigned char *)&stResp, sizeof(stResp));

          // Check if we got the correct response packet
          if (nRetVal == sizeof(stResp))
          {
            // Check if the device ACK'd or NACK'd
            if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
            {
              nRetVal = TransDevError(stResp.stData.StatusDataUnion.ErrorType);
              DEBUG2("CHeaterCtrl::SetRTDleadR(): Device sent a NACK! Dev error code = %d!", nRetVal);
            }
            else
              nRetVal = ERR_SUCCESS;
          }
          else if (nRetVal >= 0)
          {
            DEBUG2("CHeaterCtrl::SetRTDleadR(): Unexpected packet size: %d", nRetVal);
            nRetVal = ERR_PROTOCOL;
          }
          else
          {
            DEBUG2("CHeaterCtrl::SetRTDleadR(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
          }
        }
        else
        {
          nRetVal = ERR_INTERNAL_ERR;
          DEBUG2("CHeaterCtrl::SetRTDleadR(): Unexpected invalid pointer!");
        }
        break;

      case SPI_COMM:
        // TODO: To be implemented?
      case SERIAL_COMM:
        // TODO:  To be implemented?
      case COMM_NONE:
      default:
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CHeaterCtrl::SetRTDleadR(): Invalid switch case!");
        break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CHeaterCtrl::SetCompTempSlope(): Function called before Open Call!");
  }

  return nRetVal;
}
