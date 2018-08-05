/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: BaseDev.cpp 
 * *
 * *  Description: HAL base class for all device functions
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
 * *  $Id: BaseDev.cpp,v 1.19 2016/11/17 13:55:11 emrsn\jbeatty Exp $
 * *  $Log: BaseDev.cpp,v $
 * *  Revision 1.19  2016/11/17 13:55:11  emrsn\jbeatty
 * *  Finished incomplete support for FPDs on 1500XA R2 backplane
 * *
 * *  Revision 1.18  2016/09/02 15:20:36  emrsn\jbeatty
 * *  Fixes for 1500XA R2 FIDs
 * *
 * *  Revision 1.17  2016/08/09 17:09:06  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.16  2015/05/04 14:49:25  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.15  2014/06/26 10:00:01  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.14  2013/01/30 06:18:31  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.13  2012/03/21 06:48:45  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.12  2011/08/13 10:50:03  emrsn\rsaha
 * *  Revised as per Rony's comment.
 * *  No long having sequence number process specific; instead storing it in /tmp/UDPCommSequenceFile for access across system.
 * *  Revised to release dynamically allocated memory for UDP Client object.
 * *
 * *  Revision 1.11  2011/07/13 11:01:36  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *  Revision 1.10  2011/02/15 16:14:06  emrsn\rsaha
 * *  Revised to open /dev/rocio? with Read/Write access. Earlier it was only Write access. To support ROCIO AIs we need read access.
 * *
 * *  Revision 1.9  2008/11/16 00:32:48  epm-na\rthomas
 * *  Changed the HALOpen and HALClose functions to open and close the SPI device (/dev/rocio1 and /dev/rocio2) for communicating with ROC cards that plug into the 700XA SPI Expansion slots.
 * *
 * *  Revision 1.8  2008/03/06 20:47:03  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.7  2007/08/21 21:25:00  epm-na\lratner
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
 * *  Revision 1.5  2006/12/17 21:01:24  EPM-NA\ssatpute
 * *  GetErrorMsg() method added to resolve error message from error code.
 * *
 * *  Revision 1.4  2006/12/08 21:32:38  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
  
#include "debug.h"
#include "BaseDev.h"

#ifdef WIN32
#define bzero(x, y) memset(x, 0, y)
#endif 

// 1500XA R2 backplane FID gain and ignite files
#define FID_BKP_IGNITE_FILE  "/dev/flame2ignite"  // also for backplane FPD autozero
#define FID_BKP_GAIN_FILE    "/dev/flame2gain"

// TODO: DO we need lock files for each of the above???

CBaseDev::CBaseDev()  // Default Constructor
{
  // Initialize
  bzero(m_szDevName, sizeof(m_szDevName));
  m_pobCAN = NULL;
  m_pobReliabilityCAN = NULL;
  m_bIsDevOpen = FALSE;
  m_bySlotID = m_byFnType = m_byFnEnum = (unsigned char)-1;
  m_eCommType = COMM_NONE;
  m_bIsStreaming = FALSE;
  m_nExpSlotSPIfd = -1;
  m_pobUDP = NULL;
  m_fdFidBkpIgnite = -1;
  m_fdFidBkpGain = -1;
}

CBaseDev::~CBaseDev() // Destructor
{
  // Our object is getting deallocated without closing the device
  if (m_bIsDevOpen)
  {
    CloseHal();
    DEBUG2("CBaseDev::~CBaseDev(): Destructor invoked before CBaseDev::Close()!");
  }
}

int CBaseDev::OpenHal(char* pszDevName,         // Name of the device to open
                      BOOL bStream /*= FALSE*/) // Is the device a streaming device?
{
  int nRetVal = ERR_SUCCESS;

  // Check for invalid input pointer
  if (pszDevName == NULL)
  {
    nRetVal = ERR_INVALID_ARGS;
    DEBUG2("CBaseDev::Open(): Invalid arguments!");
    return nRetVal;
  }

  // Check if device already open
  if (m_bIsDevOpen)
  {
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CBaseDev::Open( %s) Failed! Attempt to reopen", pszDevName);
    return nRetVal;
  }

  // Copy the name of the device to our member variable. Will be useful for logging error messages
  strncpy(m_szDevName, pszDevName, sizeof(m_szDevName) - 1);
  m_szDevName[sizeof(m_szDevName) - 1] = '\0';

  m_bIsStreaming = bStream;

  // Decode the device address from the device name
  DEBUG3("CBaseDev::Open(): Before ResolveName()");
  nRetVal = m_obResDevName.ResolveName(pszDevName, &m_byFnType, &m_bySlotID, &m_byFnEnum, &m_eCommType);
  DEBUG3("CBaseDev::Open(): After ResolveName()");
  if (nRetVal == ERR_SUCCESS) // Successful?
  {
    switch (m_eCommType)
    {
      case CAN_COMM:
        m_pobCAN = new CCANComm; // Allocate memory
        if (m_pobCAN)
        {
          m_pobReliabilityCAN = new CReliability(m_pobCAN);
          if (m_pobReliabilityCAN)
          {
            // Open CAN Comm Channel
            nRetVal = m_pobCAN->CANCommOpen(m_bySlotID, m_byFnType, m_byFnEnum, bStream);
            if (nRetVal == ERR_SUCCESS)
            {
              // Successfully opened the device
              m_bIsDevOpen = TRUE;
            }
            else
            {
              DEBUG2("CBaseDev::Open(%s): CCANComm::CANCommOpen() failed with error code %d.", pszDevName, nRetVal);
            }
          }
          else
          {
            nRetVal = ERR_MEMORY_ERR;
            DEBUG2("CBaseDev::Open(%s) Failed! Unable to allocate memory for reliability layer using \"new\"", pszDevName);
          }
        }
        else
        {
          nRetVal = ERR_MEMORY_ERR;
          DEBUG2("CBaseDev::Open(%s) Failed! Unable to allocate memory using \"new\"", pszDevName);
        }
        break;

      case SPI_COMM:  // Device uses SPI
        {
          // Generate device name and open the device
          char szSPIDev[15];
          sprintf(szSPIDev, "/dev/rocio%d", (int)m_bySlotID);
          m_nExpSlotSPIfd = open(szSPIDev, O_RDWR);
          if (m_nExpSlotSPIfd != -1)
          {
            // Successfully opened the device
            m_bIsDevOpen = TRUE;
          }
          else
          {
            nRetVal = ERR_INTERNAL_ERR;
            DEBUG2("CBaseDev::Open(%s) failed while opening %s!", pszDevName, szSPIDev);
          }
        }
        break;

      case UDP_COMM:  // Device uses UDP
        m_pobUDP = new CUDPClient();
        if (m_pobUDP)
        {
          if (m_pobUDP->Initialize() != -1)
          {
            m_bIsDevOpen = TRUE;
          }
          else
          {
            nRetVal = ERR_INTERNAL_ERR;
            DEBUG2("CBaseDev::Open(%s) failed while initializing UDP communication !", pszDevName);
          }
        }
        else
        {
          nRetVal = ERR_MEMORY_ERR;
          DEBUG2("CBaseDev::Open(%s) Failed! Unable to allocate memory using \"new\"", pszDevName);
        }
        break;

      case FID_COMM:  // for 1500XA backplane FID (and FPD)
        m_fdFidBkpIgnite = open(FID_BKP_IGNITE_FILE, O_WRONLY);
        if (m_fdFidBkpIgnite < 0)
        {
          nRetVal = ERR_OPEN_FILE;
          DEBUG2("CBaseDev::OpenHal(%s) failed - unable to open file", FID_BKP_IGNITE_FILE);
        }
        else
        {
          m_fdFidBkpGain = open(FID_BKP_GAIN_FILE, O_WRONLY);
          if (m_fdFidBkpGain < 0)
          {
            close(m_fdFidBkpIgnite);
            m_fdFidBkpIgnite = -1;
            nRetVal = ERR_OPEN_FILE;
            DEBUG2("CBaseDev::OpenHal(%s) failed - unable to open file", FID_BKP_GAIN_FILE);
          }
          else
            m_bIsDevOpen = TRUE;
        }
        break;

      default:
        break;
    }
  }
  else
    DEBUG2("CBaseDev::Open(): CResolveDevName::ResolveName(%s) failed with error code = %d!", pszDevName, nRetVal);

  return nRetVal;
}
  
// Closes the device. Returns 0 on success, negative error code on failure.
int CBaseDev::CloseHal()
{
  int nRetVal = ERR_SUCCESS;

  // Check if device is open
  if (m_bIsDevOpen)
  {
    // Check the type of communication mechanism being used.
    switch (m_eCommType)
    {
    case CAN_COMM:
      // Release memory allocated to m_pobCAN object
      if (m_pobCAN)
      {
        nRetVal = m_pobCAN->CANCommClose();
        delete m_pobCAN;
        m_pobCAN = NULL;

        if (m_pobReliabilityCAN)
        {
          delete m_pobReliabilityCAN;
          m_pobReliabilityCAN = NULL;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CBaseDev::Close(): Invalid condition! CCANComm object NULL");
      }
      break;
    
    case SPI_COMM:
      if (m_nExpSlotSPIfd != -1)
      {
        close(m_nExpSlotSPIfd);
        m_nExpSlotSPIfd = -1;
      }
      break;


    case UDP_COMM:
      if (m_pobUDP)
      {
        delete m_pobUDP;
        m_pobUDP = NULL;
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CBaseDev::Close(): Invalid condition! UDPClient object NULL");
      }
      break;
    
    case FID_COMM:
      if (m_fdFidBkpIgnite >= 0)
      {
        close(m_fdFidBkpIgnite);
        m_fdFidBkpIgnite = -1;
      }
      if (m_fdFidBkpGain >= 0)
      {
        close(m_fdFidBkpGain);
        m_fdFidBkpGain = -1;
      }
      break;

    case SERIAL_COMM:
      //TODO:  To be implemented
    
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CBaseDev::Close(): Invalid switch case!");
      break;
    }

    // Clear all our variables
    bzero(m_szDevName, sizeof (m_szDevName));
    m_eCommType = COMM_NONE;
    m_bIsDevOpen = FALSE;
    m_bySlotID = m_byFnType = m_byFnEnum = (unsigned char)-1;
    m_bIsStreaming = FALSE;
  }
  else
  {
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CBaseDev::Close(): Attempting to close device that is not open!");
  }

  return nRetVal;
}

// Translate device error into HAL error code.
int CBaseDev::TransDevError(int nDevError)
{
  int nHALRetVal;

  switch (nDevError)
  {
    case ACK_NO_ERR:
      nHALRetVal = ERR_SUCCESS;
      break;

    case NACK_INVALID_FN_TYPE:
    case NACK_INVALID_FN_ENUM:
    case NACK_INVALID_CMD:
      nHALRetVal = ERR_PROTOCOL;
      break;

    case NACK_INT_ERR:
      nHALRetVal = ERR_DEV_INT_ERR;
      break;

    case NACK_CMD_FAILED:
      nHALRetVal = ERR_CMD_FAILED;
      break;

    default:
      nHALRetVal = ERR_DEV_UNKNOWN;
      break;
  }

  return nHALRetVal;
}

/*------------------------------------------------------------------------------
 * Function return error message based on error code
 *-----------------------------------------------------------------------------*/
CHAR *CBaseDev::GetErrorMsg(ERR_CODE iErrorCode)
{
  switch (iErrorCode)
  {
    case ERR_CMD_FAILED: return "Command execution failed";
    case ERR_OPEN_FILE: return "Unable to open file";
    case ERR_DATA_PENDING: return "Data not received completely";
    case ERR_TIMEOUT: return "Timout error";
    case ERR_DEV_UNKNOWN: return "Unknown error code from remote device";
    case ERR_DEV_INT_ERR: return "Internal error in remote device";
    case ERR_MEMORY_ERR: return "Memory allocation error";
    case ERR_INVALID_DEV_NAME: return "HAL unable to resolve the device name into SlotID, Fn Type, Fn Enum"; 
    case ERR_DEV_NOT_REG: return "Device not registered with cand";
    case ERR_DEV_IN_USE: return "Can't register device. It is in use";
    case ERR_NO_RESP: return "No response from remote client, Communication failed";
    case ERR_INTERNAL_ERR: return "Internal Error (Unable to open pipes or cand may not be running)";
    case ERR_PROTOCOL: return "Protocol Error";
    case ERR_INVALID_ARGS: return "Invalid arguments passed to the function";
    case ERR_INVALID_SEQ: return "Device not opened.";
    case ERR_SUCCESS: return "No error";
    default:
    case ERR_NOT_IMPLEMENTED: return "Unknown error";
  }
  return NULL;
};
