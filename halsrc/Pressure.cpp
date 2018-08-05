/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: Pressure.cpp
 *
 *  Description: Pressure sensors access layer.
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
 *  $Id: Pressure.cpp,v 1.5 2015/05/04 14:49:28 EMRSN\sboyidi Exp $
 *  $Log: Pressure.cpp,v $
 *  Revision 1.5  2015/05/04 14:49:28  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.4  2014/06/26 10:00:58  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.3  2013/01/30 06:18:49  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.2  2012/03/21 06:48:59  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.1  2010/06/10 20:13:29  emrsn\rsaha
 *  Added HAL interfaces for Pressure read, Serial RS232/485 control support and diagnostics details of Analyzer IO card.
 *
 *************************************************************************/

#include "FixEndian.h"
  
#include "debug.h"
#include "Pressure.h"

CPressure::CPressure()    // Default Constructor
{
}

CPressure::~CPressure()   // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CPressure::OpenHal (char* pszDevName)
{
  return CBaseDev::OpenHal (pszDevName, FALSE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CPressure::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Gets the Pressure in Milli Volts
int CPressure::GetPressure (unsigned long* PressureMilliVolt)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_PRESSURE_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_FN_GET_PRESSURE);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd,  // Command
                                                     sizeof (stCmd),            // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));          // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))
        {
          FixEndian(stResp.ulPressureMilliVolts);
          *PressureMilliVolt = stResp.ulPressureMilliVolts;
          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CPressure::GetPressure(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CPressure::GetPressure(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CPressure::GetPressure(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CPressure::GetPressure(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CPressure::GetPressure(): Function called before Open Call!");
  }

  return nRetVal;
}



