/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Fragment.cpp
 * *
 * *  Description: Base class for de-fragmentation layer.
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
 * *  $Id: Fragment.cpp,v 1.12 2015/05/04 14:49:27 EMRSN\sboyidi Exp $
 * *  $Log: Fragment.cpp,v $
 * *  Revision 1.12  2015/05/04 14:49:27  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.11  2014/06/26 10:00:23  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.10  2013/01/30 06:18:38  EMRSN\mvaishampayan
 * *  1. Removed ENABLE_DEBUG macro from all source files.
 * *  2. Enabled the DEBUG mode for the release_engr and the pc target; disabled the DEBUG mode for release_prod target.
 * *  3. Resolved compiler warnings/errors which came into picture after removing the ENABLE_DEBUG macro.
 * *
 * *  Revision 1.9  2012/03/21 06:48:52  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.8  2008/10/22 21:54:59  epm-na\rsaha
 * *  Bug Fix - After a CRC check success reseting the error flag. Previously after a CRC check failure all successive calls reported CRC failure even though the CRC check was successful.
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
 * *  Revision 1.5  2007/01/10 23:20:08  EPM-NA\ssatpute
 * *  1.  Fixed BigEndian issue with CRC check.
 * *  2.  Tested with hardware board.
 * *  3.  Working fine.
 * *
 * *  Revision 1.4  2007/01/10 21:52:55  EPM-NA\ssatpute
 * *  Added interface for BCI Can Interface board. Tested. Working fine.
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
#include "crc16.h"
#include "FixEndian.h"
  
#include "debug.h"
#include "Fragment.h"

CFragment::CFragment()  // Constructor
{
  m_pbyCollectedData = NULL;
  m_unArrayLen = 0;
  m_bProcFragment = FALSE;
  m_bCRCFailure = FALSE;
  m_bNewData = FALSE;
  m_bCRCExists = FALSE;
}

CFragment::~CFragment() // Destructor
{
  Flush();
}

// Clear the data in m_pbyCollectedData
int CFragment::Flush ()
{
  //Release allocated memory
  if (m_pbyCollectedData)
  {
    free(m_pbyCollectedData);
  }

  //Reset variables
  m_pbyCollectedData = NULL;
  m_unArrayLen = 0;
  m_bProcFragment = FALSE;
  m_bCRCFailure = FALSE;
  m_bNewData = FALSE;
  m_bCRCExists = FALSE;
  
  return ERR_NOT_IMPLEMENTED;
}

//Extract the received CRC and also compute CRC over the received data
void CFragment::CRCCompute(unsigned int unBufferStart, unsigned int unBufferLen)
{
  // The received CRC
  unsigned short usRxCRC = 0;
  // CRC computed over the received data
  unsigned short usCalcCRC = 0;

  ENDIAN_ADJ_UNION endianAdj;
  

  //Extract the RX'ed CRC from the last two bytes of the received data
  


  endianAdj.parts.msb = m_pbyCollectedData[m_unArrayLen - 1];
  endianAdj.parts.lsb = m_pbyCollectedData[m_unArrayLen - 2];
  usRxCRC = endianAdj.asUint16;
  FixEndian(usRxCRC);

  //usRxCRC = m_pbyCollectedData[m_unArrayLen - 1];  //lsb
  //usRxCRC = usRxCRC | (m_pbyCollectedData[m_unArrayLen - 2] << 8); //msb

  //FixEndian(usRxCRC);

  //Compute CRC over the data packet - excluding the header in the beginning
  //  and the received CRC at the end of the packet.
  usCalcCRC = crc16(&m_pbyCollectedData[unBufferStart], unBufferLen);
  
  DEBUG1("usRxCRC: 0x%x  usCalcCRC: 0x%x",usRxCRC,usCalcCRC);
  //Check for a match and set status accordingly
  if (usRxCRC != usCalcCRC)
  {
    m_bCRCFailure = TRUE;
  }
  else
  {
    m_bCRCFailure = FALSE;
  }
}

