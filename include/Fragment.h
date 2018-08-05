/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Fragment.h
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
 * *  $Id: Fragment.h,v 1.8 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 * *  $Log: Fragment.h,v $
 * *  Revision 1.8  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.7  2014/06/26 10:01:13  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.6  2012/03/21 06:49:06  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.5  2008/10/22 22:28:50  epm-na\rsaha
 * *  Updated a parameter of type unsigned short to unsigned int for few methods to support fragmented data exchange of more than 256 bytes.
 * *
 * *  Revision 1.4  2007/01/10 21:52:34  EPM-NA\ssatpute
 * *  Added interface for BCI Can Interface board. Tested. Working fine.
 * *
 * *  Revision 1.3  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



// Fragement.h - Header file for CFragment
//

#ifndef _FRAGMENT_H
#define _FRAGMENT_H

#include "Definitions.h"


class CFragment {
protected:
  
  // Pointer where the data from multiple fragmented packets is stored.
  unsigned char* m_pbyCollectedData;

  // Length of m_pbyCollectedData
  unsigned int m_unArrayLen;

  // Indicates if data collection for a fragmented packet is in progress
  BOOL m_bProcFragment;

  // Indicated if the computed CRC doesn't match the Rx'ed CRC
  BOOL m_bCRCFailure;

  // New, completely formed packet, exists
  BOOL m_bNewData;

  // Non-fragmented packets do not have CRC
  BOOL m_bCRCExists;

  // Extract RX'ed CRC. Compute CRC over RX'ed data. Compare the two
  //   and set m_bCRCFailure based on the result.
  virtual void CRCCompute(unsigned int unBufferStart, unsigned int unBufferLen);

public:

  CFragment();  // Constructor

  virtual ~CFragment(); // Destructor

  // Add part of fragmented data to the end of m_pbyCollectedData
  virtual int AddData (unsigned char* pbyData, unsigned char byNumBytes) { return -1; };

  // Clear the data in m_pbyCollectedData
  virtual int Flush ();

  // Get the accumulated data from multiple fragmented packets 
  virtual int GetData (unsigned char* pbyData, unsigned int unNumBytesToRead) { return -1; };

  // Get the total number of bytes remaining in m_pbyCollectedData
  virtual int GetCount () { return -1; };
};

#endif // #ifndef _FRAGMENT_H

