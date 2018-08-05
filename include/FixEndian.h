/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: FixEndian.h
 * *
 * *  Description: Code to work around bitfield Endianness issues
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
 * *  $Id: FixEndian.h,v 1.11 2015/04/30 09:32:32 EMRSN\Dkad Exp $
 * *  $Log: FixEndian.h,v $
 * *  Revision 1.11  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.10  2014/06/26 10:01:13  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.9  2012/03/21 06:49:06  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.8  2008/11/10 23:54:23  epm-na\rsaha
 * *  Added methods to resolve Big/Little endian issues for FFB-GC communication.
 * *  The APIs help to retrive/store acknowlege bit, message ID, retransmisstion bit, acknowledge code from/to communication packets.
 * *
 * *  Revision 1.7  2007/08/21 21:27:51  epm-na\lratner
 * *  Added #include global.h, to include definition of INT16, FLOAT32, etc.
 * *
 * *  Revision 1.6  2007/01/10 21:52:34  EPM-NA\ssatpute
 * *  Added interface for BCI Can Interface board. Tested. Working fine.
 * *
 * *  Revision 1.5  2006/12/08 21:32:40  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Updated function enumeration to accommodate an 'unknown' function.
 * *  - Added Analog In and Analog Out code. Related changes to BaseIOProtocol.h
 * *  - Minor fixes and changes
 * *
 * *
 * *************************************************************************/

#include "global.h"



#ifndef _FIX_ENDIAN_H
#define _FIX_ENDIAN_H

//For DevAddrUnion
#define MASK_SLOT_ID  0x001F
#define MASK_FN_TYPE  0x03E0
#define MASK_FN_COUNT 0x3C00
#define MASK_FRAGMENT 0x4000
#define MASK_DATATYPE 0x8000

#define SLOT_ID_BIT_POS   0
#define FN_TYPE_BIT_POS   5
#define FN_COUNT_BIT_POS  10
#define FRAGMENT_BIT_POS  14
#define DATATYPE_BIT_POS  15


//For CMD_ACK_STRUCT
#define MASK_COMMAND  0x7F
#define MASK_ERROR    0x80

#define COMMAND_BIT_POS   0
#define ERROR_BIT_POS     7

//For device information structure DEV_FN_CAP_STRUCT
#define DISC_MASK_FN_TYPE 0x001F
#define DISC_MASK_FN_CNT  0x01E0
#define DISC_MASK_RESR    0xFE00

#define DISC_FN_TYPE_BIT_POS  0
#define DISC_FN_CNT_BIT_POS   5
#define DISC_RESR_BIT_POS     9


//For FFB protocol structures.
//For FFB_CMD_INFO_STRUCT
#define MASK_ACK_BIT_4_FFB_CMD    0x1
#define MASK_MSSG_ID_4_FFB_CMD    0xFE

#define ACK_BIT_POS_4_FFB_CMD   0
#define MSSG_ID_POS_4_FFB_CMD   1

//For FFB_CMD_ACK_INFO_STRUCT
#define MASK_RETRANSMIT_BIT_4_FFB_ACK 0x01
#define MASK_ACK_CODE_4_FFB_ACK       0xFE

#define RETRANSMIT_BIT_POS_4_FFB_ACK  0
#define ACK_CODE_POS_4_FFB_ACK        1


//For DevAddrUnion
static inline void SetSlotID(unsigned short *pusHeader, unsigned short usValue)
{
  *pusHeader &= ~(MASK_SLOT_ID);
  *pusHeader |= (usValue << SLOT_ID_BIT_POS);
}

static inline void SetFnType(unsigned short *pusHeader, unsigned short usValue)
{
  *pusHeader &= ~(MASK_FN_TYPE);
  *pusHeader |= (usValue << FN_TYPE_BIT_POS);
}

static inline void SetFnCount(unsigned short *pusHeader, unsigned short usValue)
{
  *pusHeader &= ~(MASK_FN_COUNT);
  *pusHeader |= (usValue << FN_COUNT_BIT_POS);
}

static inline void SetFragment(unsigned short *pusHeader, unsigned short usValue)
{
  *pusHeader &= ~(MASK_FRAGMENT);
  *pusHeader |= (usValue << FRAGMENT_BIT_POS);
}

static inline void SetDatatype(unsigned short *pusHeader, unsigned short usValue)
{
  *pusHeader &= ~(MASK_DATATYPE);
  *pusHeader |= (usValue << DATATYPE_BIT_POS);
}


//For DevAddrUnion
static inline int GetSlotID(unsigned short *pusHeader)
{
  return ((*pusHeader & MASK_SLOT_ID) >> SLOT_ID_BIT_POS);
}

static inline int GetFnType(unsigned short *pusHeader)
{
  return ((*pusHeader & MASK_FN_TYPE) >> FN_TYPE_BIT_POS);
}

static inline int GetFnCount(unsigned short *pusHeader)
{
  return ((*pusHeader & MASK_FN_COUNT) >> FN_COUNT_BIT_POS);
}

static inline int GetFragment(unsigned short *pusHeader)
{
  return ((*pusHeader & MASK_FRAGMENT) >> FRAGMENT_BIT_POS);
}

static inline int GetDatatype(unsigned short *pusHeader)
{
  return ((*pusHeader & MASK_DATATYPE) >> DATATYPE_BIT_POS);
}


//For CMD_ACK_STRUCT
static inline void SetCmdAckError(unsigned char *pusHeader, unsigned char usValue)
{
  *pusHeader &= ~(MASK_ERROR);
  *pusHeader |= (usValue << ERROR_BIT_POS);
}

static inline void SetCmdAckCommand(unsigned char *pusHeader, unsigned char usValue)
{
  *pusHeader &= ~(MASK_COMMAND);
  *pusHeader |= (usValue << COMMAND_BIT_POS);
}

//For CMD_ACK_STRUCT
static inline unsigned char GetCmdAckError(unsigned char *pusHeader)
{
  return ((*pusHeader & MASK_ERROR) >> ERROR_BIT_POS);
}

static inline unsigned char GetCmdAckCommand(unsigned char *pusHeader)
{
  return ((*pusHeader & MASK_COMMAND) >> COMMAND_BIT_POS);
}


//For device information structure DEV_FN_CAP_STRUCT
static inline void SetDiscFnType(unsigned short *pusHeader, unsigned short usValue)
{
  *pusHeader &= ~(DISC_MASK_FN_TYPE);
  *pusHeader |= (usValue << DISC_FN_TYPE_BIT_POS);
}

static inline void SetDiscFnCount(unsigned short *pusHeader, unsigned short usValue)
{
  *pusHeader &= ~(DISC_MASK_FN_CNT);
  *pusHeader |= (usValue << DISC_FN_CNT_BIT_POS);
}

static inline int GetDiscFnType(unsigned short *pusHeader)
{
  return ((*pusHeader & DISC_MASK_FN_TYPE) >> DISC_FN_TYPE_BIT_POS);
}

static inline int GetDiscFnCount(unsigned short *pusHeader)
{
  return ((*pusHeader & DISC_MASK_FN_CNT) >> DISC_FN_CNT_BIT_POS);
}

//The reserved field is used for board revision information
static inline int GetDiscRevision(unsigned short *pusHeader)
{
  return ((*pusHeader & DISC_MASK_RESR) >> DISC_RESR_BIT_POS);
}

//This is already present in the main project's include directory.
#if 0
# if __BYTE_ORDER == __BIG_ENDIAN
#warning "BIGENDIAN!Turning on byte order swapping!!!!"
#define FixEndian(x) ByteSwap((unsigned char *) &x, sizeof(x))

static inline void ByteSwap(unsigned char * b, int n)
{
  register int i = 0;
  register int j = n -1 ;
  register unsigned char tmp;
  
  while( i < j )
  {
    tmp = b[i]; b[i] = b[j]; b[j] = tmp; //swap the bytes.
    i++; j--;
  }
}

#else

#define FixEndian(x)

#endif




#endif

typedef union {

  INT16 asInt16;           /* The short value as seen by this machine */
  UINT16 asUint16;
  FLOAT32 asFloat32;       /* the FLOAT32 value as seen by this machine */
  INT32 asInt32;           /* the INT32 value as seen by this machine */

  struct {
    UCHAR lsb;
    UCHAR msb;
  } parts;

  struct {
    UCHAR fltFourthByte;
    UCHAR fltThirdByte;
    UCHAR fltSecondByte;
    UCHAR fltFirstByte;
  } partsFlt;

  struct {
    UCHAR lngFourthByte;
    UCHAR lngThirdByte;
    UCHAR lngSecondByte;
    UCHAR lngFirstByte;
  } partsLng;

} ENDIAN_ADJ_UNION; /* union to care of the endianness of the m/c */



//Updates the Acknowlege bit field (which separates acknowledge message from 
//command message) in the FFB message header.
static inline void SetAckBit_FFB( unsigned char * pbyCmd, unsigned char byValue )
{
  * pbyCmd &= ~(MASK_ACK_BIT_4_FFB_CMD);
  * pbyCmd |= (byValue << ACK_BIT_POS_4_FFB_CMD);
}

//Updates the Message ID in the FFB message header.
static inline void SetMssgID_FFB( unsigned char * pbyCmd, unsigned char byValue )
{
  * pbyCmd &= ~(MASK_MSSG_ID_4_FFB_CMD);
  * pbyCmd |= (byValue << MSSG_ID_POS_4_FFB_CMD);
}

//Retrieves the Acknowledge bit field (which separates acknowledge message from
//command message) from the FFB message header.
static inline int GetAckBit_FFB( unsigned char byCmd )
{
  return ( byCmd & MASK_ACK_BIT_4_FFB_CMD ) >> ACK_BIT_POS_4_FFB_CMD;
}

//Retrieves the Message ID from the FFB message header.
static inline int GetMssgID_FFB( unsigned char byCmd )
{
  return ( byCmd & MASK_MSSG_ID_4_FFB_CMD ) >> MSSG_ID_POS_4_FFB_CMD;
}

//Updates the Retransmission bit field (which indicates whether a command is sent twice)
//in the FFB acknowledge message.
static inline void SetRT_FFB( unsigned char * pbyAck, unsigned char byValue )
{
  * pbyAck &= ~(MASK_RETRANSMIT_BIT_4_FFB_ACK);
  * pbyAck |= (byValue << RETRANSMIT_BIT_POS_4_FFB_ACK);
}

//Updates the Acknowledge code in the FFB acknowledge message.
static inline void SetAckCode_FFB( unsigned char * pbyAck, unsigned char byValue )
{
  * pbyAck &= ~(MASK_ACK_CODE_4_FFB_ACK);
  * pbyAck |= (byValue << ACK_CODE_POS_4_FFB_ACK);
}

//Retrives the Retransmission bit field (which indicates whether a command is sent twice)
//from the FFB acknowledge message.
static inline int GetRT_FFB( unsigned char byAck )
{
  return ( byAck & MASK_RETRANSMIT_BIT_4_FFB_ACK ) >> RETRANSMIT_BIT_POS_4_FFB_ACK;
}

//Retrives the Acknowledge code from the FFB acknowledge message.
static inline int GetAckCode_FFB( unsigned char byAck )
{
  return ( byAck & MASK_ACK_CODE_4_FFB_ACK ) >> ACK_CODE_POS_4_FFB_ACK;
}

#endif //_FIX_ENDIAN_H


