/***********************************************************************
 *                      Rosemount Analytical 
 *                    10241 West Little York, Suite 200 
 *                           Houston, TX 77040 
 *                                            
 *
 *  Filename: Ltloi.cpp
 *
 *  Description: LTLOI HAL object
 *
 *  Copyright:        Copyright (c) 2011-2012, 
 *                    Rosemount Analytical 
 *                    All Rights Reserved.
 *
 *  Operating System:  None.
 *  Language:          'C'
 *  Target:            Gas Chromatograph Model GC700XP
 *
 *  Revision History:
 *  $Id: LtLoi.cpp,v 1.10 2015/05/04 14:49:28 EMRSN\sboyidi Exp $
 *  $Log: LtLoi.cpp,v $
 *  Revision 1.10  2015/05/04 14:49:28  EMRSN\sboyidi
 *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 *  Fix : Updated the indentation in the source files as per below requirements.
 *        -Set the indent style to C/C++.
 *        -Set the indent size to 2, and replace tab with blank space.
 *        -Turn off interpretation of tab.
 *
 *  Revision 1.9  2014/06/26 10:00:40  EMRSN\sboyidi
 *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 *  Fix : Updated the indentation in all the source files as per below requirements.
 *        - Set the indent style to C/C++.
 *        - Set the indent size to 2, and replace tab with blank space.
 *        - Turn off interpretation of tab.
 *
 *  Revision 1.8  2013/01/30 06:18:45  EMRSN\mvaishampayan
 *  1. Removed ENABLE_DEBUG macro from all source files.
 *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 *
 *  Revision 1.7  2012/03/21 06:48:56  EMRSN\jchekkala
 *  File header was updated with Company Name and Address.
 *
 *  Revision 1.6  2009/02/24 17:09:47  epm-na\rsaha
 *  Revised to display build number of the version information.
 *
 *  Revision 1.5  2008/03/06 20:47:04  epm-na\rthomas
 *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 *  Modified Makefile to make HAL SO to link against debug.o.
 *  Added "ENABLE_DEBUG" in cpp files.
 *
 *  Revision 1.4  2007/08/21 21:25:01  epm-na\lratner
 *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 *  except for commented out instances, which received DEBUG3() calls.
 *
 *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 *
 *  Revision 1.3  2007/08/09 07:04:05  EPM-NA\ssatpute
 *  Added below functions to return receive pipe file descriptor - streaming data.
 *  1. CANGetRxStrmFd() in CANComm.cpp file
 *  2. GetRxStreamingFd() in LtLoi.cpp file
 *
 *  Revision 1.2  2007/06/08 19:25:04  EPM-NA\vmalik
 *  Fixed bug in code where it would not compile under bigendian due to
 *  typo in data struct member passed to Fixendian()
 *
 *  Revision 1.1  2007/04/05 13:20:30  epm-na\rthomas
 *  LTLOI: Added LTLOI function and corresponding class to access the LTLOI
 *  hardware through the Base IO board.
 *
 *
 *************************************************************************/

#include <stdio.h>
#include "FixEndian.h"
  
#include "debug.h"
#include "LtLoi.h"

CLtLoi::CLtLoi()  // Default Constructor
{
}

CLtLoi::~CLtLoi() // Destructor
{
}

// Open the device. Returns 0 on success, negative error code on failure.
int CLtLoi::OpenHal (char* pszDevName)
{
  return CBaseDev::OpenHal (pszDevName, TRUE);
}

// Closes the device. Returns 0 on success, negative error code on failure.
int CLtLoi::CloseHal ()
{
  return CBaseDev::CloseHal ();
}

//Set the number of clcok bits  - Returns 0 on success, negative error code on failure.
int CLtLoi::SetNumClkBits(unsigned char NumClkBits)   
{
  int nRetVal = ERR_SUCCESS;

  CAN_LTLOI_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
        
  //check if the number of clk bits passed exceeds the
  //value (MAX_LTLOI_BYTES * 8).
  if(NumClkBits > MAX_NUM_CLK_BITS)
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
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_LTLOI_FN_SET_CLK_BITS);


        stCmd.stData.BaseIOData.LtloiNumClkBits = NumClkBits;

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
            DEBUG2("CLtLoi::SetNumClkBits(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CLtLoi::SetNumClkBits(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::SetNumClkBits(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::SetNumClkBits(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::SetNumClkBits(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::SetNumClkBits(): Function called before Open Call!");
  }

  return nRetVal;
}

// Set the output byte - Returns 0 on success, negative error code on failure.
int CLtLoi::SetOutputByte(const unsigned char ByteVal, unsigned char ByteNum)   
{
  int nRetVal = ERR_SUCCESS;

  CAN_LTLOI_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  //check if the byte number is more than 11 (byte num should b/w 0 - 11)
  //return invalid argument error if it is more than 11
  if(ByteNum > MAX_LTLOI_BYTE_INDEX)
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
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_LTLOI_FN_SET_BYTE);


        stCmd.stData.BaseIOData.LtloiData.ByteVal = ByteVal;
        stCmd.stData.BaseIOData.LtloiData.ByteNum = ByteNum;

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
            DEBUG2("CLtLoi::SetOutputByte(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CLtLoi::SetOutputByte(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::SetOutputByte(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::SetOutputByte(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::SetOutputByte(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::SetOutputByte(): Function called before Open Call!");
  }

  return nRetVal;
}

// Set the out byte - Returns 0 on success, negative error code on failure.
int CLtLoi::SetOutputFlash(const unsigned char FlashByte, unsigned char ByteNum)  
{
  int nRetVal = ERR_SUCCESS;

  CAN_LTLOI_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
        
  //check if the byte number is more than 11 (byte num should b/w 0 - 11)
  //return invalid argument error if it is more than 11
  if(ByteNum > MAX_LTLOI_BYTE_INDEX)
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
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_LTLOI_FN_SET_FLASH);
        

        stCmd.stData.BaseIOData.LtloiData.ByteVal = FlashByte;
        stCmd.stData.BaseIOData.LtloiData.ByteNum = ByteNum;

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
            DEBUG2("CLtLoi::SetOutputFlash(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CLtLoi::SetOutputFlash(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::SetOutputFlash(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::SetOutputFlash(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::SetOutputFlash(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::SetOutputFlash(): Function called before Open Call!");
  }

  return nRetVal;
}


// Set the scan control state - Returns 0 on success, negative error code on failure.
int CLtLoi::ScanCtrl(LTLOI_APP_STATUS_ENUM eScanState)
{
  int nRetVal = ERR_SUCCESS;

  CAN_LTLOI_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  //check if the scan state passed exceeds the limit
  if(eScanState < LTLOI_APP_STATUS_MIN || eScanState >= LTLOI_APP_STATUS_MAX)
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
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_LTLOI_FN_SET_SCAN_STATUS);
        
        
        //check for the valid value
        if(eScanState == LTLOI_APP_STATUS_OFF)
        {
          stCmd.stData.BaseIOData.LtloiScan = LTLOI_STATUS_OFF;
        }
        //check for the valid value
        else if(eScanState == LTLOI_APP_STATUS_ON)
        {
          stCmd.stData.BaseIOData.LtloiScan = LTLOI_STATUS_ON;
        }
        else
        {
          return ERR_INVALID_ARGS;
        }

        FixEndian(stCmd.stData.BaseIOData.LtloiScan);

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
            DEBUG2("CLtLoi::ScanCtrl(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CLtLoi::ScanCtrl(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::ScanCtrl(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::ScanCtrl(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::ScanCtrl(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::ScanCtrl(): Function called before Open Call!");
  }

  return nRetVal;
}




// Reading the input byte - Returns 0 on success, negative error code on failure.
int CLtLoi::ReadInputByte(unsigned char *ByteVal, unsigned char ByteNum)     
{
  int nRetVal = ERR_SUCCESS;

  CAN_LTLOI_DATA_STRUCT stCmd = {0};
  CAN_LTLOI_DATA_STRUCT stResp = {0};

  if (NULL == ByteVal)
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
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_LTLOI_FN_GET_BYTE);

        stCmd.stData.BaseIOData.LtloiData.ByteNum = ByteNum;

        // Send a command and wait for ackowledgement from remote device
        nRetVal = m_pobReliabilityCAN->GetRemoteResp((unsigned char *) &stCmd, // Command
                                                     sizeof (stCmd),        // Size of command
                                                     (unsigned char *) &stResp, // Response from remote board
                                                     sizeof (stResp));      // Size of expected response

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          //FixEndian(stResp.stData.BaseIOData.DigIOState);
          *ByteVal = stResp.stData.BaseIOData.LtloiData.ByteVal;
          

          nRetVal = ERR_SUCCESS;
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CLtLoi::ReadInputByte(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::ReadInputByte(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::ReadInputByte(): Unexpected invalid pointer!");
        
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::ReadInputByte(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::ReadInputByte(): Function called before Open Call!");
  }

  return nRetVal;
}



// Set the streaming state to either ON or OFF state - Returns 0 on success, 
//negative error code on failure.
int CLtLoi::StreamingState(LTLOI_APP_STATUS_ENUM eStreamingOnOff)
{
  int nRetVal = ERR_SUCCESS;

  CAN_LTLOI_DATA_STRUCT stCmd = {0};
  CAN_BASEIO_STATUS_STRUCT stResp = {0};
  
  //check if the scan state passed exceeds the limit
  if( eStreamingOnOff < LTLOI_APP_STATUS_MIN ||  eStreamingOnOff >= LTLOI_APP_STATUS_MAX)
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
        SetCmdAckCommand(&stCmd.stCmd.byCmdAck, CMD_LTLOI_FN_SET_STREAM_STATUS);
        
        
        //check for the valid value
        if( eStreamingOnOff == LTLOI_APP_STATUS_OFF)
        {
          stCmd.stData.BaseIOData.LtloiStream = LTLOI_STATUS_OFF;
        }
        //check for the valid value
        else if( eStreamingOnOff == LTLOI_APP_STATUS_ON)
        {
          stCmd.stData.BaseIOData.LtloiStream = LTLOI_STATUS_ON;
        }
        else
        {
          return ERR_INVALID_ARGS;
        }

        FixEndian(stCmd.stData.BaseIOData.LtloiStream);

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
            DEBUG2("CLtLoi::StreamingState(): Device sent a NACK! Dev error code = %d!", nRetVal);
          }
          else
          {
            nRetVal = ERR_SUCCESS;
          }
        }
        //Not the expected packet size
        else if (nRetVal >= 0)
        {
          DEBUG2("CLtLoi::StreamingState(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::StreamingState(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::StreamingState(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::StreamingState(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::StreamingState(): Function called before Open Call!");
  }

  return nRetVal;
}


//Read the input byte in blocking mode if the timeout is NULL else read with timeout
int CLtLoi::RxStreamingByteCommon(unsigned char *ByteVal, unsigned char *ByteNum, unsigned int *Timeout)
{
  int nRetVal = ERR_SUCCESS;
  LTLOI_STREAM_STRUCT stResp;

  if ( (NULL == ByteVal) || (NULL == ByteNum) )
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
        if(Timeout)
        {
          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobCAN->CANRxStrmTimeout((unsigned char *) &stResp,  // Response from remote board
                                               sizeof (stResp),*Timeout); // Size of expected response
        }
        else
        {
          // Send a command and wait for ackowledgement from remote device
          nRetVal = m_pobCAN->CANRxStrmBlocking((unsigned char *) &stResp,  // Response from remote board
                                                sizeof (stResp)); // Size of expected response
        }

        // Check if we got the correct response packet
        if (nRetVal == sizeof (stResp))  
        {
          *ByteVal = stResp.LtloiData.ByteVal;
          *ByteNum = stResp.LtloiData.ByteNum;
          
          nRetVal = ERR_SUCCESS;
        }
        else if (nRetVal < 0)// if (nRetVal <= 0)
        {
          DEBUG2("CLtLoi::RxStreamingByteCommon(): CCANComm::CANRxStrmBlocking failed with error code %d!", nRetVal);
        }
        else // if(nRetVal != sizeof (stResp))
        {
          DEBUG2("CLtLoi::RxStreamingByteCommon(): CCANComm::CANRxStrmBlocking - received unexpected number of bytes: %d!", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::RxStreamingByteCommon(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::RxStreamingByteCommon(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::RxStreamingByteCommon(): Function called before Open Call!");
  }

  return nRetVal;
}


//Read the input byte in blocking mode  - Returns 0 on success, negative error code on failure.
int CLtLoi::RxStreamingByteBlocking(unsigned char *ByteVal, unsigned char *ByteNum)
{
  return RxStreamingByteCommon(ByteVal, ByteNum, NULL);
}

// Read the input byte with timeout  - Returns 0 on success, negative error code on failure.
int CLtLoi::RxStreamingByte(unsigned char *ByteVal, unsigned char *ByteNum, unsigned int Timeout)
{
  return RxStreamingByteCommon(ByteVal, ByteNum, &Timeout);
}



//Get board revision information
int CLtLoi::GetBoardInfo (unsigned char* byFirmwareVerMaj,  // Firmware Version - Major Number
                          unsigned char* byFirmwareVerMin,  // Firmware Version - Minor Number
                          unsigned char* byFirmwareVerBuild,  // Firmware Version - Minor Number
                          unsigned char* byBoardRevision) // Board Revision
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
          DEBUG2("CLtLoi::GetBoardInfo(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::GetBoardInfo(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::GetBoardInfo(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::GetBoardInfo(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::GetBoardInfo(): Function called before Open Call!");
  }

  return nRetVal;

  
  return ERR_NOT_IMPLEMENTED;
}


// Gets the on-board temperature in Milli Degree C.
int CLtLoi::GetOnBoardTemp(int* TempMilliDegC)
{
  int nRetVal = ERR_SUCCESS;

  CmdAckUnion stCmd = {0};
  CAN_LTLOI_DATA_STRUCT stResp = {0};

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
        SetCmdAckCommand(&stCmd.byCmdAck, CMD_LTLOI_FN_GET_ON_BD_TEMP);

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
          DEBUG2("CLtLoi::GetOnBoardTemp(): Unexpected packet size: %d", nRetVal);
          nRetVal = ERR_PROTOCOL;
        }
        else // if (nRetVal < 0)
        {
          DEBUG2("CLtLoi::GetOnBoardTemp(): CCANComm::CANGetRemoteResp failed with error code %d!", nRetVal);
        }
      }
      else
      {
        nRetVal = ERR_INTERNAL_ERR;
        DEBUG2("CLtLoi::GetOnBoardTemp(): Unexpected invalid pointer!");
      }

      break;

    case SPI_COMM:
      //TODO: To be implemented
    case SERIAL_COMM:
      //TODO:  To be implemented      
    case COMM_NONE:
    default:
      nRetVal = ERR_INTERNAL_ERR;
      DEBUG2("CLtLoi::GetOnBoardTemp(): Invalid switch case!");
      break;
    }
  }
  else
  {
    // Function called before Open Call!
    nRetVal = ERR_INVALID_SEQ;
    DEBUG2("CLtLoi::GetOnBoardTemp(): Function called before Open Call!");
  }

  return nRetVal;
}

// Return Receive Pipe File Descriptor
int CLtLoi::GetRxStreamingFd()
{
  int nRetVal = ERR_SUCCESS;
  if (m_pobCAN)
  {
    nRetVal = m_pobCAN->CANGetRxStrmFd();
  }
  else
  {
    nRetVal = ERR_INTERNAL_ERR;
    DEBUG2("CLtLoi::GetRxStrmFd(): Unexpected invalid pointer!");
  }
  return nRetVal;
}

