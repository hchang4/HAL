/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: HelperClasses.h
 * *
 * *  Description: Definition of classes required for ddiscfallback compilation.
 * *       Since ddiscfallback needed to be indepent of HAL library, all these classes are
 * *       collected from HAL and place it here. This file only used for ddiscfallback.
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
 * *  $Id: HelperClasses.h,v 1.5 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 * *  $Log: HelperClasses.h,v $
 * *  Revision 1.5  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.4  2014/06/26 10:01:14  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.3  2012/03/21 06:49:06  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.2  2010/10/29 13:25:04  emrsn\rsaha
 * *  This file is added to support ddiscfallback compilation.
 * *  All the HAL classes that ddisc depends on are isolated in this file so to help build ddiscfallback without linking to HAL library.
 * *  This file is only get compiled during ddiscfallback build process.
 * *
 * *************************************************************************/


#ifndef _HELPER_CLASSES_H_
#define _HELPER_CLASSES_H_

#ifdef MAKE_FALLBACK

#include "Definitions.h"

#define RX_PKT_HDR_LEN    sizeof(DevAddrUnion)
#define RX_PKT_CRC_LEN    2
#define PKT_DATA_START_IX 2

unsigned short crc16(unsigned char *buf, unsigned int len);

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

// Mapping of Fn Type to Fn Type Enum
struct DeviceTypeMapStruct {
  char szName[15];      // Function Name
  unsigned char byEnum; // Enum equivalent for Function Name  
};

// Mapping to Slot Name to Slot Address
struct DeviceAddrMapStruct {
  char szSlotName[20];    // Slot Name
  BOOL bIsOnCANBus;       // Is this Slot on the CAN Bus or SPI Bus? TRUE - On CAN, FALSE - On SPI
  unsigned char bySlotID; // CAN Message Id / SPI Address
};

class CResolveDevName {
private:
  
  // Mapping of Device Fn Type to Fn Type Enums
  static const DeviceTypeMapStruct stDevTypeMap[MAX_NUM_DEV_FUNCTIONS];

  // Mapping of Slot ID Name to Device Address
  static const DeviceAddrMapStruct stSlotIDMap[MAX_NUM_SLOTS];

public:
  // Function takes in the Device name in "FnType:SlotID:FnEnum" format as input. 
  // Looks up each component of the device name and 
  // (1) Checks for errors in the Device name format
  // (2) Checks for invalid Fn Type, Slot ID and Fn Enum
  // (3) Returns Fn Type, Slot ID and Fn Enum as enumerated values
  // Function returns 0 on success, negative error code on failure
  int ResolveName (char* pszDevName, 
                   unsigned char* pbyFnType, 
                   unsigned char* pbySlotID, 
                   unsigned char* pbyFnEnum, 
                   BOOL* pbCANorSPI);

  // TODO:
  // Function takes in Fn Type, Slot ID and Fn Enum and writes 
  // the device name into pszDevName pointer.
  // Function returns 0 on success, negative error code on failure
  int GenerateName (char* pszDevName, 
                    unsigned char byFnType, 
                    unsigned char bySlotID, 
                    unsigned char byFnEnum); 
};

#endif //#ifdef MAKE_FALLBACK

#endif //#ifdef _HELPER_CLASSES_H_


