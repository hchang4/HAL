/***********************************************************************
 *                          Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                             
 *
 *  Filename: CycleClockSync.cpp
 *
 *  Description: Synchronize Cycle Clock Start/Stop across all IO Boards
 *
 *  Copyright:        Copyright (c) 2011-2015, 
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C++'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *************************************************************************/

#include "FixEndian.h"
#include "debug.h"
#include "CycleClockSync.h"

CCycleClockSync::CCycleClockSync()    // Default Constructor
{
}

CCycleClockSync::~CCycleClockSync()   // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CCycleClockSync::OpenHal (char* pszDevName)
{
  return CBaseDev::OpenHal (pszDevName, FALSE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CCycleClockSync::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

// Start or Stop Cycle Clock
int CCycleClockSync::SetCycleClockState(CYCLE_CLOCK_CMD_ENUM eState)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0}; 

  // Check if the device is open!
  if (m_bIsDevOpen)
  {
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Transmit
      if (m_pobCAN)
      {
        SetCmdAckCommand(&stCmd.byCmdAck, eState);

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->TxCmd((unsigned char *) &stCmd,  // Command
                                             sizeof (stCmd));           // Size of command

      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CCycleClockSync::SetCycleClockState(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CCycleClockSync::SetCycleClockState(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CCycleClockSync::SetCycleClockState(): Function called before Open Call!");
  }

  return nRetVal;
}


int CCycleClockSync::StartCycleClock ()
{
  return SetCycleClockState (CMD_CYCLE_CLOCK_START);
}

int CCycleClockSync::StopCycleClock ()
{
  return SetCycleClockState (CMD_CYCLE_CLOCK_STOP);
}

