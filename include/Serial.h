/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: Serial.h
 * *
 * *  Description: Serial IO device function access layer
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
 * *  $Id: Serial.h,v 1.10 2015/04/30 09:32:33 EMRSN\Dkad Exp $
 * *  $Log: Serial.h,v $
 * *  Revision 1.10  2015/04/30 09:32:33  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.9  2014/12/02 11:39:34  emrsn\jchekkala
 * *  set Serial driver mode to either .RS485. or .RS422 will be supported for serial ports on Base IO G2 board.
 * *  Will not indicate the need of Hardware Switches for serial ports on Base IO G2 board.
 * *
 * *  Revision 1.8  2014/06/26 10:01:18  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.7  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.6  2009/02/24 17:13:42  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.5  2006/12/15 16:32:42  epm-na\DPrabhakar
 * *  CSerial's Read() and Write() APIs were clashing the Read and Write macros
 * *  defined for GC700XP. Have changed the names to ReadCh(), ReadChBlocking(),
 * *  and WriteCh() to work around this issue.
 * *
 * *  Related changes to the TestHAL application.
 * *
 * *  Revision 1.4  2006/12/13 22:21:14  epm-na\DPrabhakar
 * *  Code clean up
 * *    - Removed un-used enum in BaseIOProtocol.h
 * *
 * *  Revision 1.3  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/




#ifndef _SERIAL_H
#define _SERIAL_H

#include "BaseDev.h"
#include "BaseIOProtocol.h"
#include "Fifo.h"


//The packet size can be anything from 1 to MAX_SERIALIO_PKT_LEN...
//  Not sure if having the FIFO size as a multiple of the 
//  packet length helps in anyway.
//IMPORTANT: This value must be a power of 2
//#define MAX_SER_CH_RX_FIFO_SIZE   (MAX_SERIALIO_PKT_LEN * 100)
#define MAX_SER_CH_RX_FIFO_SIZE   2048

// Serial Port
class CSerial : public CBaseDev {

private:
  CFifo *m_pobRxFifo;

  int m_iBoardRevision;
  int ReadCommon(unsigned char *Data, unsigned int NumBytes,  unsigned int *Timeout);

public:
  CSerial();  // Default Constructor
  
  ~CSerial(); // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // Get Device Status
  int GetStatus (int* pnStatus);      // Pointer to write status to

  // Get the board revision information
  int GetBoardInfo(unsigned char* byFirmwareVerMaj,   // Firmware Version - Major Number
                   unsigned char* byFirmwareVerMin,   // Firmware Version - Minor Number
                   unsigned char* byFirmwareVerBuild, // Firmware Version - Build Number
                   unsigned char* byBoardRevision);   // Board Revision
    
  // Set Serial Port parameters
  //int SetCommParams (unsigned int Baud, unsigned char StartBits, unsigned char StopBits, unsigned char Parity); 
  int SetCommParams(BASEIO_SER_BAUD_ENUM eBaudRate, 
                    BASEIO_SER_DATA_LEN_ENUM eDataLen,
                    BASEIO_SER_PARITY_ENUM eParity,
                    BASEIO_SER_STOP_BITS_ENUM eStopBits,
                    BASEIO_SER_PORT_MODE_ENUM ePortMode);

  //Get Serial Port parameters
  int GetCommParams(BASEIO_SER_BAUD_ENUM *eBaudRate, 
                    BASEIO_SER_DATA_LEN_ENUM *eDataLen,
                    BASEIO_SER_PARITY_ENUM *eParity,
                    BASEIO_SER_STOP_BITS_ENUM *eStopBits,
                    BASEIO_SER_PORT_MODE_ENUM *ePortMode);


  //Set the timeout (in ms) for transmission of less than a packet of data on the device
  int SetTxTimeoutMs(unsigned long ulTimeout);

  // Reads data from Remote Serial Port. Note: This is a streaming data read method and reads the data queue’d up in the rx fifo. 
  // Data will queue up to max FIFO size which is at least 4kB.
  int ReadCh(unsigned char* Data, unsigned int NumBytes, unsigned int Timeout);
  
  // Blocking version of the Read() function.
  int ReadChBlocking(unsigned char* Data, unsigned int NumBytes);

  // Write data to remote serial port
  int WriteCh(unsigned char* Data, unsigned int NumBytes);
  
  // Gets the on-board temperature in Milli Degree C.
  int GetOnBoardTemp (int* TempMilliDegC);

  // Flushes the contents of the Named Pipe that is used for streaming data for this device.
  int Flush(); 

  // Returns TRUE if G2 BaseIO Board else FALSE
  bool IsRS422ModeSupported( void );
  
  // Returns FALSE if G2 BaseIO Board else TRUE
  bool IsNeedHWSwitches( void );
  
};

#endif // #ifndef _SERIAL_H

