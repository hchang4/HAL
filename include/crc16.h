/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: crc16.h
 * *
 * *  Description: CRC 16 (CRC is only used for fragmented packets)
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
 * *  $Id: crc16.h,v 1.4 2012/03/21 06:49:09 EMRSN\jchekkala Exp $
 * *  $Log: crc16.h,v $
 * *  Revision 1.4  2012/03/21 06:49:09  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2008/10/22 22:28:50  epm-na\rsaha
 * *  Updated a parameter of type unsigned short to unsigned int for few methods to support fragmented data exchange of more than 256 bytes.
 * *
 * *  Revision 1.2  2006/12/08 21:32:41  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/



#ifndef _CRC_H_
#define _CRC_H_

unsigned short crc16(unsigned char *buf, unsigned int len);

#endif /* _CRC_H_ */
