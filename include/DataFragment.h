/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: DataFragment.h
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
 * *  $Id: DataFragment.h,v 1.6 2015/04/30 09:32:31 EMRSN\Dkad Exp $
 * *  $Log: DataFragment.h,v $
 * *  Revision 1.6  2015/04/30 09:32:31  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.5  2014/06/26 10:01:10  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.4  2012/03/21 06:49:04  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2008/10/22 22:28:50  epm-na\rsaha
 * *  Updated a parameter of type unsigned short to unsigned int for few methods to support fragmented data exchange of more than 256 bytes.
 * *
 * *  Revision 1.2  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



// Fragement.h - Header file for CFragment
//

#ifndef _DATAFRAGMENT_H
#define _DATAFRAGMENT_H

#include "Fragment.h"
#include "Definitions.h"


#define RX_PKT_HDR_LEN    sizeof(DevAddrUnion)
#define RX_PKT_CRC_LEN    2
#define PKT_DATA_START_IX 2

class CDataFragment : public CFragment {
private:

public:

  CDataFragment();  // Constructor

  ~CDataFragment(); // Destructor

  // Add part of fragmented data to the end of m_pbyCollectedData
  int AddData (unsigned char* pbyData, unsigned char byNumBytes);

  // Get the accumulated data from multiple fragmented packets 
  int GetData (unsigned char* pbyData, unsigned int unNumBytesToRead);

  // Get the number of data bytes in the buffer
  int GetCount ();
};

#endif // #ifndef _FRAGMENT_H

