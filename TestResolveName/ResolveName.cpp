// ResolveName.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include "ResolveDevName.h"

char szTestNames[17][MAX_DEV_NAME]= {
  "ANA_IN:SLOT_1:ANA_IN_1",
  "DIGI_IN:REMOTE_EXP_SLOT_1:DIGI_IN_1",
  "RTD:SLOT_2:RTD1",
  "RTD:SLOT_2:RTD2",
  "RTD:SLOT_2:RTD3",
  "RTD:SLOT_2:RTD4",
  "RTD:SLOT_2:RTD5",
  "HTR_CTRL:SLOT_BASE_IO:HTR_CTRL_1",
  "HTR_CTRL:SLOT_4:HTR_CTRL_1",
  "PREAMP:SLOT_3:PREAMP_1",
  "PREAMP:SLOT_3:PREAMP_2",
  "SERIAL:EXP_SLOT_2:SERIAL_1",
  "ANNA_IN:SLOT_1:ANA_IN_1",  // Fn Type incorrect
  "ANA_IN:SLOT_10:ANA_IN_1",  // Slot Address incorrect
  "ANA_IN:SLOT_1:ANA_IN_x", // Enumeration incorrect
  "ANNA_IN:SLOT_10:ANA_IN_1", // Fn Type & Slot Address incorrect
  "ANNA_IN:SLOT_10:ANA_IN_xx",  // Totally incorrect
};

int main( void )
{
  int nRetVal = 0;
  CResolveDevName obDevName;

  //FILE* fp = fopen ("\\Results.txt", "w");  // For Windows
  FILE* fp = fopen ("//Results.txt", "w");  // For Linux

  if (fp)
  {

    fprintf (fp, "***** Test Results *****\n\n");
    fprintf (fp, "%50s\t%10s\t%10s\t%10s\t%10s\t%10s\n", 
             "Name",
             "RetVal",
             "FnType", 
             "SlotID",
             "FnEnum",
             "CANorSPI");

    for (int nCount = 0; nCount < 17; ++nCount)
    {
      unsigned char byFnType = (unsigned char)-1, bySlotID = (unsigned char)-1, byFnEnum = (unsigned char)-1;
      BOOL bCANorSPI = FALSE;

      nRetVal = obDevName.ResolveName (szTestNames[nCount], &byFnType, &bySlotID, &byFnEnum, &bCANorSPI);

      fprintf (fp, "%50s\t%10d\t%10d\t%10d\t%10d\t%10d\n", 
               szTestNames[nCount], //"Name",
               nRetVal, //"RetVal",
               byFnType,  //"FnType",
               bySlotID,  //"SlotID", 
               byFnEnum,  //"FnEnum"
               bCANorSPI);  //"CANorSPI"
    }

    fclose (fp);
  }

  return 0;
}


