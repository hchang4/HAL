
/***********************************************************************
 * *                          Rosemount Analytical 
 * *                     10241 West Little York, Suite 200 
 * *                            Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: BoardSlotInfo.cpp
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
 * *  $Id: BoardSlotInfo.cpp,v 1.3 2016/11/01 13:05:34 emrsn\jbeatty Exp $
 * *  $Log: BoardSlotInfo.cpp,v $
 * *  Revision 1.3  2016/11/01 13:05:34  emrsn\jbeatty
 * *  Added support for FPD G2
 * *
 * *  Revision 1.2  2016/08/30 09:57:09  emrsn\jbeatty
 * *  Mods for 1500XA R2 support
 * *
 * *  Revision 1.1  2014/12/02 11:33:36  emrsn\jchekkala
 * *  Get Board Revision from current_software_info.txt file for a given Board Name
 * *  (i.e., BaseIO, Preamp and HeaterSolenoid).
 * *
 * *
 * *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <global.h>

#include "debug.h"
#include <xp_common.h>
#include <Definitions.h>
#include "BoardSlotInfo.h"

CBoardSlotInfo::CBoardSlotInfo()
{
}

CBoardSlotInfo::~CBoardSlotInfo()
{
}

void CBoardSlotInfo::getSlotStrByCANId(CHAR *pcSlotStrByCANId, unsigned char *cCANID)
{
  char *pcSlot = NULL;

  switch (*cCANID)
  {
#ifdef MODEL_370XA
  case GC370XA_LOI_CAN_MSG_ID:
    pcSlot = "370XA LOI Slot";
    break;
#endif //#ifdef MODEL_370XA
  case GC700XP_FPD_G2_BASE_CAN_MSG_ID:
    pcSlot = "FPD G2 Slot 1";
    break;
  case GC700XP_FPD_G2_BASE_CAN_MSG_ID + 1:
    pcSlot = "FPD G2 Slot 2";
    break;
  case GC700XP_PREAMP_BD_CAN_BASE_MSG_ID:
    pcSlot = "Slot 1";
    break;
  case GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 1:
    pcSlot = "Slot 3";
    break;
  case GC700XP_PREAMP_BD_CAN_BASE_MSG_ID + 2:
    pcSlot = "Slot 5";
    break;
  case GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID:
    pcSlot = "Slot 2";
    break;
  case GC700XP_SOL_HTR_CTRL_BD_BASE_CAN_MSG_ID + 1:
    pcSlot = "Slot 4";
    break;
  case GC700XP_FIELDBUS_BD_BASE_CAN_MSG_ID:
    pcSlot = "FFB Slot";
    break;
  case GC700XP_GRAPHICAL_LOI_BD_BASE_CAN_MSG_ID:
    pcSlot = "LOI Slot";
    break;
#ifdef MODEL_370XA
  case GC370XA_ANALYZER_BD_BASE_CAN_MSG_ID:
    pcSlot = "Analyzer Slot";
    break;
  case GC370XA_IMB_BD_BASE_CAN_MSG_ID:
    pcSlot = "Module Slot";
    break;
#endif
  case GC700XP_BASE_IO_BD_BASE_CAN_MSG_ID:
    pcSlot = "BaseIO Slot";
    break;
  default:
    break;
  }
  
  if (pcSlot != NULL)
    strcpy(pcSlotStrByCANId, pcSlot);
}

/* Get Board Revision from current_software_info.txt file for a given Board Name
 * If the given Board Name was not found in current_software_info.txt file or
 * If the board revision is not available in the f/w module information line
 *   then returns 0 (zero) as the Board Revision
 *   else extract and returns the Board Revision present f/w Module Information.
 */
int CBoardSlotInfo::GetBoardRevision(const char *cBoardName, unsigned char *pcCANID, int &iBoardRev)
{
  int iRetVal = ERR_SUCCESS;
  CHAR cSwNameStr[200] = {0};
  CHAR cVerStr[100] = {0};
  CHAR cDateStr[100] = {0};
  CHAR cSlotStrFile[100] = {0};
  CHAR cBoardRev[100] = {0};
  unsigned char ucCANId = *pcCANID;
  FILE *fp = NULL;
  const char *pcCurrSwFileName = "/home/Daniel/current_software_info.txt";

  iBoardRev = 0; // In case of Given Software not found Revision will be 0
  if ((fp = fopen(pcCurrSwFileName, "r")) == NULL)
  {
    DEBUG1("Unable to open file: (%s)\n", pcCurrSwFileName);
    iRetVal = -1;
  }
  else
  {
    DEBUG2("Successfully opened file: (%s)\n", pcCurrSwFileName);
    while (TRUE)
    {
      CHAR cLineBuf[1024] = {0};
      if (fgets(cLineBuf, sizeof(cLineBuf), fp) != NULL)
      {
        // expect at least 5 items to be scanned
        if (sscanf(cLineBuf, "%[^,], %[^,], %[^,], , %[^,], %[^,\r\n]",
                   cSwNameStr, cVerStr, cDateStr, cSlotStrFile, cBoardRev) == 5)
        {
          CHAR cSlotStrByCANId[256] = {0};
          getSlotStrByCANId(cSlotStrByCANId, &ucCANId);
          DEBUG2("Read 5 Items %s, %s, %s, %s, %s\ncSlotStrByCANId (%s), cCANID(%d)\n", 
                 cSwNameStr, cVerStr, cDateStr, cSlotStrFile, cBoardRev, cSlotStrByCANId, (int)ucCANId);
          
          // Ensure pre/post blank spaces
          if (strcmp(cSlotStrByCANId, cSlotStrFile) == 0) // Slot Id Matched
          {
            if (strncmp(cSwNameStr, cBoardName, strlen(cBoardName)) == 0) // Board/Sw Name Matched
            {
              // Base IO firmware String
              iBoardRev = atoi(cBoardRev);
              DEBUG3("%s : Revision %d\n", cBoardName, (int)iBoardRev);
              break;  // No need to read further Lines
            }
            else
              DEBUG2("Board Revision Doesnot Match, %s vs %s\n", cSwNameStr, cBoardName);
          }
          else
            DEBUG2("SlotIds Doesnot Match byCANid (%s), from curr_soft_info file (%s)\n", cSlotStrByCANId, cSlotStrFile);
        }
        else
          DEBUG3("Could Not Read 5 Items, Skip the line\n");
      }
      else
      {
        DEBUG2("End of File Reached\n");
        iRetVal = -1;
        break;
      }
    }
    
    fclose(fp);
  }
  
  DEBUG2("Returning %s Board Revision (%d)\n", cBoardName, (int)iBoardRev);  
  return iRetVal;
}
