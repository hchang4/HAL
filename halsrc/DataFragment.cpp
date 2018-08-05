/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: DataFragment.cpp
 * *
 * *  Description: DATA de-fragmentation layer. Used for join multi-packet
 * *               message from the device.
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
 * *  $Id: DataFragment.cpp,v 1.9 2015/05/04 14:49:25 EMRSN\sboyidi Exp $
 * *  $Log: DataFragment.cpp,v $
 * *  Revision 1.9  2015/05/04 14:49:25  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.8  2014/06/26 10:00:09  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.7  2013/01/30 06:18:33  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.6  2012/03/21 06:48:46  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.5  2008/10/22 22:04:35  epm-na\rsaha
 * *  Modified argument "byNumBytesToRead" of CDataFragment::GetData() of type unsinged char to "unNumBytesToRead" of type unsigned int to retrive more than 256 bytes fragmented data.
 * *
 * *  Revision 1.4  2008/03/06 20:47:03  epm-na\rthomas
 * *  Changes made inorder to enable debug output (using DEBUG1, 2, 3) from HAL Shared Library.
 * *  Modified Makefile to make HAL SO to link against debug.o.
 * *  Added "ENABLE_DEBUG" in cpp files.
 * *
 * *  Revision 1.3  2007/08/21 21:25:00  epm-na\lratner
 * *  Replaced all instances of  #ifdef _HAL_DEBUG printf() #endif with DEBUG2() calls,
 * *  except for commented out instances, which received DEBUG3() calls.
 * *
 * *  A single printf without the conditional compilation was replaced with a DEBUG1() call.
 * *
 * *  Revision 1.2  2006/12/08 21:32:38  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include <string.h>
#include "crc16.h"
#include "FixEndian.h"
  
#include "debug.h"
#include "DataFragment.h"

CDataFragment::CDataFragment()  // Constructor
{
}

CDataFragment::~CDataFragment() // Destructor
{
}

// Add part of fragmented data to the end of m_pbyCollectedData
int CDataFragment::AddData (unsigned char* pbyData, unsigned char byNumBytes)
{
  DevAddrUnion stDevToHost;
  void *pReallocMem = NULL;

  if ( (NULL == pbyData) || (0 == byNumBytes) )
  {
    return -ERR_INVALID_ARGS;
  }

  //Extract the header
  memcpy(&stDevToHost.stDevAdD2H, pbyData, sizeof(DevAddrUnion));
  FixEndian(stDevToHost.usDevAd);

  //For the first packet, save the header too
  if (FALSE == m_bProcFragment)
  {
    //If the fragment bit is set, this is a multi-packet, fragmented message
    if (GetFragment(&stDevToHost.usDevAd))
    {
      m_bProcFragment = TRUE;
      m_bNewData = FALSE;
    }
    //This is a non-fragmented packet - set flag to indicate that the data is
    //  available for read
    else
    {
      m_bNewData = TRUE;
      //No CRC for non-fragmented packets
      m_bCRCExists = FALSE;
    }

    m_unArrayLen = 0;
  }

  pReallocMem = realloc(m_pbyCollectedData, 
                        m_unArrayLen + byNumBytes - RX_PKT_HDR_LEN);
  if (NULL == pReallocMem)
  {
    DEBUG2("CFragment::AddData() - realloc() failed.");
    //TODO - do we need to free the prev allocated memory here or
    //  let the user do it through Flush ()
  
    return ERR_MEMORY_ERR;
  }

  //Point to the newly allocated memory
  m_pbyCollectedData = (unsigned char *) pReallocMem;
  
  //Copy/append data to the data buffer
  memcpy(&m_pbyCollectedData[m_unArrayLen], &pbyData[PKT_DATA_START_IX], 
         byNumBytes - RX_PKT_HDR_LEN);
  //Length of the data buffer
  m_unArrayLen += (byNumBytes - RX_PKT_HDR_LEN);

  // If this is the end of the fragmented packet 
  if ( (!GetFragment(&stDevToHost.usDevAd)) && (FALSE == m_bNewData) )
  {
    //Process CRC related stuff
    CRCCompute(0, m_unArrayLen - RX_PKT_CRC_LEN);
    //No longer processing a fragment
    m_bProcFragment = FALSE;
    //New data exists
    m_bNewData = TRUE;
    //CRC exists for fragmented packets
    m_bCRCExists = TRUE;
  }

  return ERR_SUCCESS;
}

// Get the accumulated data from multiple fragmented packets 
int CDataFragment::GetData (unsigned char* pbyData, unsigned int unNumBytesToRead)
{
  unsigned int unDataLen = 0;

  if ( (NULL == pbyData) || (0 == unNumBytesToRead) )
  {
    return ERR_INVALID_ARGS;
  }


  if (m_bNewData)
  {
    //TODO - do we reset m_bNewData here?
    m_bNewData = FALSE;

    if (m_bCRCFailure)
    {
      return ERR_WRONG_CRC;
    }
    else
    {
      if (m_bCRCExists)
      {
        unDataLen = m_unArrayLen - RX_PKT_CRC_LEN;
      }
      else
      {
        unDataLen = m_unArrayLen;
      }

      //TODO - if the user requests for less data than available,
      //  maintain an index of how much has been read so that the
      //  next call to this function will pass the unread data.
      //  DO THIS LATER

      //If asking for more than what's present, return whatever is
      //  available.
      if (unNumBytesToRead >= unDataLen)
      {
        //Copy the max. available data
        memcpy(pbyData, m_pbyCollectedData, unDataLen);
      }
      else
      {
        //Copy data to the user buffer (till only the user supplied length)
        memcpy(pbyData, m_pbyCollectedData, unNumBytesToRead);
      }

      return unDataLen;
    }
      
  }
  else if (m_bProcFragment)
  {
    return ERR_DATA_PENDING;
  }
  else
  {
    return ERR_INVALID_SEQ;
  }
}

// Get the total number of bytes remaining in m_pbyCollectedData 
int CDataFragment::GetCount ()
{
  if (m_bCRCExists)
  {
    return (m_unArrayLen - RX_PKT_CRC_LEN);
  }
  else
  {
    return m_unArrayLen;
  }
}

