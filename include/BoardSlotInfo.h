/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: BoardSlotInfo.hpp
 * *
 * *  Description: Parses current_software_info.txt file to get Revision of
 * *               given board
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
 * *  $Id: BoardSlotInfo.h,v 1.2 2016/11/01 13:03:18 emrsn\jbeatty Exp $
 * *  $Log: BoardSlotInfo.h,v $
 * *  Revision 1.2  2016/11/01 13:03:18  emrsn\jbeatty
 * *  Added support for FPD G2
 * *
 * *  Revision 1.1  2014/12/02 11:33:09  emrsn\jchekkala
 * *  Get Board Revision from current_software_info.txt file for a given Board Name
 * *  (i.e., BaseIO, Preamp and HeaterSolenoid).
 * *
 * *
 * *************************************************************************/

#ifndef _BOARD_SLOT_INFO_H
#define _BOARD_SLOT_INFO_H

#include <stdio.h>
#include <stdlib.h>

#define G2_BASEIO_BOARD (7)
#define G2_PREAMP_BOARD (7)

class CBoardSlotInfo
{
private:
  void getSlotStrByCANId(CHAR *pcSlotStrByCANId, unsigned char *cCANID);
  
public:

  CBoardSlotInfo();

  ~CBoardSlotInfo();

/* Get Board Revision from current_software_info.txt file for a given Board Name
 * If the given Board Name was not found in current_software_info.txt file or
 * If the board revision is not available in the f/w module information line
 *   then returns 0 (zero) as the Board Revision
 *   else extract and returns the Board Revision present f/w Module Information.
 */
  int GetBoardRevision(const char *cBoardName, unsigned char *pcSlotId, int &iBoardRev);
};
 
#endif // _BOARD_SLOT_INFO_H
