/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: EPC.cpp
 * *
 * *  Description: EPC device function access layer.
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
 * *  $Id: EPC.cpp,v 1.15 2015/05/04 14:49:26 EMRSN\sboyidi Exp $
 * *  $Log: EPC.cpp,v $
 * *  Revision 1.15  2015/05/04 14:49:26  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.14  2014/06/26 10:00:13  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.13  2013/10/08 15:09:47  emrsn\rrthomas
 * *  EPC Pressure Sensor Temperature correction made configurable using MON2020. Here is the correction formula (implemented in Analyzer IO firmware) -
 * *
 * *  Corrected_Carrier_Pressure = Carrier_Pressure  (Ambient_temperature  base_temperature) * pressure_correction
 * *
 * *  "Ambient_temperature" is read from RTD2 which is mounted on the IMB Board in close proximity to the Pressure Sensor.
 * *  "base_temperature" is available in EPC_CONFIG table.
 * *  "pressure_correction" is available in EPC_CONFIG table.
 * *
 * *  Revision 1.12  2013/01/30 06:18:34  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.11  2012/03/21 06:48:48  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.10  2011/07/08 21:45:35  emrsn\rrthomas
 * *  Changed how "Diff Gain" is sent to the EPC function in the Analyzer Board. Earlier, D Gain had to be a integer, changed it so that we can setup D Gain as a float. Need a fractional D Gain for testing EPCs on the 370XA.
 * *
 * *  Revision 1.9  2011/06/01 08:21:17  emrsn\rsaha
 * *  Extended HAL EPC interfaces for 370XA.
 * *
 * *  Revision 1.8  2009/02/24 17:09:47  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.7  2008/03/06 20:47:04  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.6  2007/08/21 21:25:01  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.5  2007/03/13 08:39:39  epm-na\rthomas
 * *  - Added interface to get the current preamp value through the preamp config
 * *    object.
 * *  - Fixed data union variable names used to get / set heater control
 * *    P, I and D gains.
 * *  - Fixed Makefile in halsrc to use the library path.
 * *
 * *  Revision 1.4  2006/12/13 23:27:07  EPM-NA\slove2
 * *  added stdio.h so code compiles
 * *
 * *  Revision 1.3  2006/12/08 21:32:38  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include "FixEndian.h"
  
#include "debug.h"
#include "EPC.h"

CEPC::CEPC()  // Default Constructor
{
}

CEPC::~CEPC() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CEPC::OpenHal (char* pszDevName)
{
  return CBaseDev::OpenHal (pszDevName, FALSE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CEPC::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Get Device Status
int CEPC::GetStatus (int* pnStatus)     // Pointer to write status to
{
  return ERR_NOT_IMPLEMENTED;
}

//Get board revision information
int CEPC::GetBoardInfo (unsigned char* byFirmwareVerMaj,    // Firmware Version - Major Number
                        unsigned char* byFirmwareVerMin,    // Firmware Version - Minor Number
                        unsigned char* byFirmwareVerBuild,    // Firmware Version - Build Number
                        unsigned char* byBoardRevision)     // Board Revision
{
  int nRetVal = ERR_SUCCESS;
  
  CmdAckUnion stCmd = {0};
  CAN_BASEIO_SYSINFO_STRUCT stResp = {0};
  
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
          DEBUG2("CEPC::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::GetBoardInfo(): Function called before Open Call!");
  }

  return nRetVal;
}

// Gets the Pressure in Milli Volts
int CEPC::GetPressure (unsigned long* PressureMilliVolt)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_EPC_DATA_STRUCT stResp = {0};

  if (NULL == PressureMilliVolt)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_EPC_FN_GET_PRESSURE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.PressureMilliVolt);
          *PressureMilliVolt = stResp.stData.BaseIOData.PressureMilliVolt;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::GetPressure(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::GetPressure(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::GetPressure(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::GetPressure(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::GetPressure(): Function called before Open Call!");
  }

  return nRetVal;
}

// Sets the Pressure in Milli Volts
int CEPC::SetPressure (unsigned long PressureMilliVolt)
{
  int nRetVal = ERR_SUCCESS;

  CAN_EPC_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
#ifdef MODEL_370XA
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_EPC_CTRL_ON_PID);
#else //#ifdef MODEL_370XA
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_PRESSURE);
#endif //#ifdef MODEL_370XA

        stCmd.stData.BaseIOData.PressureMilliVolt = PressureMilliVolt;

        FixEndian(stCmd.stData.BaseIOData.PressureMilliVolt);

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
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CEPC::SetPressure(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }

        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::SetPressure(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::SetPressure(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::SetPressure(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::SetPressure(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::SetPressure(): Function called before Open Call!");
  }

  return nRetVal;
}

// Gets the on-board temperature in Milli Degree C.
int CEPC::GetOnBoardTemp(int* TempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_EPC_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_EPC_FN_GET_ON_BD_TEMP);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.OnBdTempmDegC);
          *TempMilliDegC = stResp.stData.BaseIOData.OnBdTempmDegC;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::GetOnBoardTemp(): Function called before Open Call!");
  }

  return nRetVal;
}


#ifdef MODEL_370XA

// Set Heater Channel PWM in milli %
int CEPC::SetPWMMilliP (int nTempMilliP)
{
  int nRetVal = ERR_SUCCESS;

  CAN_EPC_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_EPC_CTRL_ON_PWM);
        
        stCmd.stData.BaseIOData.PWMmPct = nTempMilliP;    // Data

        FixEndian(stCmd.stData.BaseIOData.PWMmPct);

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
            DEBUG2("CEPC::SetPWMMilliP: Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::SetPWMMIlliP(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::SetPWMMilliP(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::SetPWMMilliP(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::SetPWMMilliP(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::SetPWMMilliP: Function called before Open Call!");
  }

  return nRetVal;
}

int CEPC::TurnEPCOff () // Turn Heater OFF
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_EPC_FN_SET_EPC_CTRL_OFF);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CEPC::TurnEPCOff: Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::TurnEPCOff(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::TurnEPCOff(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::TurnEPCOff(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::TurnEPCOff(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::TurnEPCOff: Function called before Open Call!");
  }

  return nRetVal;
}

// Get Heater PWM in milli %
int CEPC::GetPWMMilliP(unsigned int* pnPWMMilliP)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_EPC_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_EPC_FN_GET_EPC_PWM);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.PWMmPct);
          *pnPWMMilliP = stResp.stData.BaseIOData.PWMmPct;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::GetPWMMilliP: Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::GetPWMMilliP(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::GetPWMMilliP(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::GetPWMMilliP(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::GetPWMMilliP: Function called before Open Call!");
  }

  return nRetVal;
}


// Set the proportional gain (milli
int CEPC::SetPropGain(unsigned long ulPropGainMilli)
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_EPC_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_PROP_GAIN);
        
        stCmd.stData.BaseIOData.PropGain = ulPropGainMilli;   // Data

        FixEndian(stCmd.stData.BaseIOData.PropGain);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CEPC::SetPropGain(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::SetPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::SetPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::SetPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::SetPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::SetPropGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Set the integral gain (milli)    
int CEPC::SetIntGain(unsigned long ulIntGainMilli)
{ 
  int nRetVal = ERR_SUCCESS;

  CAN_EPC_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_INT_GAIN);
        
        stCmd.stData.BaseIOData.IntGain = ulIntGainMilli;   // Data

        FixEndian(stCmd.stData.BaseIOData.IntGain);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CEPC::SetIntGain(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::SetIntGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::SetIntGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::SetIntGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::SetIntGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::SetIntGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Set the differential gain (milli)
int CEPC::SetDiffGain(unsigned long ulDiffGainMilli)
{
  int nRetVal = ERR_SUCCESS;

  CAN_EPC_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_DIFF_GAIN);
        
        stCmd.stData.BaseIOData.DiffGain = ulDiffGainMilli;   // Data

        FixEndian(stCmd.stData.BaseIOData.DiffGain);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CEPC::SetDiffGain(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::SetDiffGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::SetDiffGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::SetDiffGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::SetDiffGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::SetDiffGain(): Function called before Open Call!");
  }

  return nRetVal;

}

int CEPC::GetPropGain(unsigned long *pulPropGainMilli)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_EPC_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_EPC_FN_GET_PROP_GAIN);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.PropGain);
          *pulPropGainMilli = stResp.stData.BaseIOData.PropGain;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::GetPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::GetPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::GetPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::GetPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::GetPropGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Get the integral gain (milli)
int CEPC::GetIntGain(unsigned long *pulIntGainMilli)
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_EPC_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_EPC_FN_GET_INT_GAIN);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.IntGain);
          *pulIntGainMilli = stResp.stData.BaseIOData.IntGain;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::GetPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::GetPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::GetPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::GetPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::GetPropGain(): Function called before Open Call!");
  }

  return nRetVal;

}

// Get the differential gain (milli)
int CEPC::GetDiffGain(unsigned long *pulDiffGainMilli)
{ 
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_EPC_DATA_STRUCT stResp = {0};

  if (NULL == pulDiffGainMilli)
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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_EPC_FN_GET_DIFF_GAIN);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          FixEndian(stResp.stData.BaseIOData.DiffGain);
          *pulDiffGainMilli = stResp.stData.BaseIOData.DiffGain;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::GetPropGain(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::GetPropGain(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::GetPropGain(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::GetPropGain(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::GetPropGain(): Function called before Open Call!");
  }

  return nRetVal;
}


int CEPC::SetPressCompBaseTemp(float fBaseTemp)
{
  int nRetVal = ERR_SUCCESS;

  CAN_EPC_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_TEMP_COMP_BASE);
        
        stCmd.stData.BaseIOData.CompFactor = fBaseTemp;   // Data

        FixEndian(stCmd.stData.BaseIOData.CompFactor);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CEPC::SetPressCorrBaseTemp(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::SetPressCorrBaseTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::SetPressCorrBaseTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::SetPressCorrBaseTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::SetPressCorrBaseTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::SetPressCorrBaseTemp(): Function called before Open Call!");
  }

  return nRetVal;

}


int CEPC::SetPressCompCorr(float fPressCorr)
{
  int nRetVal = ERR_SUCCESS;

  CAN_EPC_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:

      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_EPC_FN_SET_TEMP_COMP_CORR);
        
        stCmd.stData.BaseIOData.CompFactor = fPressCorr;    // Data

        FixEndian(stCmd.stData.BaseIOData.CompFactor);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),    // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));    // Size of expected response


        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          // Check if the device ACK'd or NACK'd
          if (GetCmdAckError(&stResp.stCmd.byCmdAck) == 1)
          {
            nRetVal = TransDevError (stResp.stData.StatusDataUnion.ErrorType);
            DEBUG2("CEPC::SetPressCorrBaseTemp(): Device sent a NACK! Dev error code = %d!", nRetVal);

          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CEPC::SetPressCorrBaseTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CEPC::SetPressCorrBaseTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CEPC::SetPressCorrBaseTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CEPC::SetPressCorrBaseTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CEPC::SetPressCorrBaseTemp(): Function called before Open Call!");
  }

  return nRetVal;

}

#endif //#ifdef MODEL_370XA


