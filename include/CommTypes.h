/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: CommTypes.h 
 * *
 * *  Description: Definition of different types of supported communication medium.
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
 * *  $Id: CommTypes.h,v 1.5 2016/08/09 17:09:44 emrsn\jbeatty Exp $
 * *  $Log: CommTypes.h,v $
 * *  Revision 1.5  2016/08/09 17:09:44  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.4  2015/04/30 09:32:31  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.3  2014/06/26 10:01:10  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.2  2012/03/21 06:49:03  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.1  2011/07/13 10:52:51  emrsn\rsaha
 * *  Revised to update Analog Outputs over UDP present in 370XA LOI hardware.
 * *
 * *************************************************************************/

#ifndef __COMM_TYPES_H__
#define __COMM_TYPES_H__
 
// Communication medium for GC to communicate with a specific add-on card
enum COMM_TYPE
{
  COMM_NONE,              // None
  SERIAL_COMM,            // Serial (RS232)
  SPI_COMM,               // Serial Peripheral Interface
  CAN_COMM,               // Controller Area Network
  UDP_COMM,               // User Datagram Protocol
  FID_COMM
};

#endif //#ifndef __COMM_TYPES_H__

