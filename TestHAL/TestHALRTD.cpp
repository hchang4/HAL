#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/time.h>

#include "AnalogIn.h"
#include "HeaterCtrl.h"
#include "SolenoidCtrl.h"
#include "RTD.h"
#include "PreampStream.h"
#include "PreampProtocol.h"


//#define MIN(a, b) ( (a <= b) ? a : b)
//#define MAX(a, b) ( (a >= b) ? a : b)


int nIndex = 0;
int nIndexFlag = 0;

int g_nExitApp = 0;

int g_nVerbose = 0;


int DEBUG(char *str, ...)
{
  if (g_nVerbose)
  {
    va_list args;
    va_start(args, str);
    return vprintf(str, args);
  }
  return 0;
}

//Signal handler for SIGINT
void SIGINTHandler(int sigNum)
{
  g_nExitApp = 1;
}

#define _TEST_HAL_DEBUG

void TestAnalogInput (char* pszDevName)
{
#if 0
  CAnalogIn obAnlg;
  int nAnlgData = 0;
  int nRetValue = 0;

  // Open the device
  //#ifdef _TEST_HAL_DEBUG
  //  DEBUG ("TestAnalogInput(): Before Open...\n");
  //#endif
  nRetValue = obAnlg.OpenHal (pszDevName);
  //#ifdef _TEST_HAL_DEBUG
  //  DEBUG ("TestAnalogInput(): After Open...\n");
  //#endif

  if (!nRetValue)
  {
    // Read analog data from it
    nRetValue = obAnlg.ReadAnalogIn (&nAnlgData);
    if (!nRetValue)
    {
#ifdef _TEST_HAL_DEBUG
      DEBUG ("TestAnalogInput(): CAnalogIn::ReadAnalogIn() - Analog input = %d...\n", nAnlgData);
#endif
    }
    else
    {
#ifdef _TEST_HAL_DEBUG
      DEBUG ("TestAnalogInput(): CAnalogIn::ReadAnalogIn() failed with error code %d!\n", nRetValue);
#endif
    }

    // Close the device
    nRetValue = obAnlg.CloseHal();
    if (!nRetValue)
    {
#ifdef _TEST_HAL_DEBUG
      DEBUG ("TestAnalogInput(): CAnalogIn::CloseHal() failed with error code %d!\n", nRetValue);
#endif
    }
  }
  else
  {
#ifdef _TEST_HAL_DEBUG
    DEBUG ("TestAnalogInput(): CAnalogIn::OpenHal() failed with error code %d!\n", nRetValue);
#endif
  }
#endif
}

#define NR_HTR_CHANNELS   4

#define HTR_FUNC_SET_TEMP 1
#define HTR_FUNC_SET_PWM  2
#define HTR_FUNC_GET_TEMP 3
#define HTR_FUNC_GET_PWM  4
#define HTR_FUNC_OFF      5
#define HTR_BOARD_INFO    6
#define HTR_FUNC_EXIT     7


void TestHeaterCtrl()
{
  static char szHtrDevNames[NR_HTR_CHANNELS][50] = {
    "HTR_CTRL:SLOT_2:HTR_CTRL_1",
    "HTR_CTRL:SLOT_2:HTR_CTRL_2",
    "HTR_CTRL:SLOT_2:HTR_CTRL_3",
    "HTR_CTRL:SLOT_2:HTR_CTRL_4",
  };
  
  static CHeaterCtrl obHtr[NR_HTR_CHANNELS];
  
  int nRetVal = 0;

  int nHtrFunc = 0;

  unsigned int unCh = 0;
  unsigned int unValue = 0;

  for (int nHtrs = 0; nHtrs < NR_HTR_CHANNELS; nHtrs++)
  {
    nRetVal = obHtr[nHtrs].OpenHal(szHtrDevNames[nHtrs]);
    
    if (nRetVal < 0)
    {
      DEBUG("Error opening Htr Channel: %d\n", nHtrs);
      return;
    }
  }
  
  while (g_nExitApp != 1)
  {
    printf("Enter heater channel number (1 to 4): \n");
    fflush(stdin);
    scanf("%d", &unCh);

    if ( (unCh < 1) || (unCh > NR_HTR_CHANNELS) )
    {
      DEBUG("Invalid value... try again.\n");
      continue;
    }
    unCh--;

    if (g_nExitApp)
    {
      break;
    }

    printf("Select heater function:\n");
    printf("  1. Set channel temperature (deg C).\n");
    printf("  2. Set challel PWM (percent).\n");
    printf("  3. Get channel temperature (deg C).\n");
    printf("  4. Get channel PWM (percent).\n");
    printf("  5. Turn heater off.\n");
    printf("  6. Get board information.\n");
    printf("  7. Exit application.\n");
    fflush(stdin);
    scanf("%d", &nHtrFunc);

    if ( (nHtrFunc == HTR_FUNC_EXIT) || (g_nExitApp))
    {
      break;
    }

    if ( (nHtrFunc != HTR_FUNC_OFF) && (nHtrFunc != HTR_FUNC_GET_TEMP) &&
         (nHtrFunc != HTR_FUNC_GET_PWM) && (nHtrFunc != HTR_BOARD_INFO) )
    {
      printf("Enter value: ");
      fflush(stdin);
      scanf("%d", &unValue);
    }

    printf("\n");
    
    if (g_nExitApp)
    {
      break;
    }


    switch (nHtrFunc)
    {
    case HTR_FUNC_SET_TEMP:
      if (obHtr[unCh].SetChTempMilliDegC(unValue * 1000) < 0)
      {
        printf("CHeaterCtrl[%d].SetChTempMilliDegC() failed.\n", unCh + 1);
      }
      break;

    case HTR_FUNC_SET_PWM:
      if (obHtr[unCh].SetChPWMMilliP(unValue * 1000) < 0)
      {
        printf("CHeaterCtrl[%d].SetChPWMMilliP() failed.\n", unCh + 1);
      }
      break;

    case HTR_FUNC_GET_TEMP:
      if (obHtr[unCh].GetHtrChTempMilliDegC( (int*)&unValue) < 0)
      {
        printf("CHeaterCtrl[%d].GetHtrChTempMilliDegC() failed.\n", unCh + 1);
      }
      else
      {
        printf("Heater %d Temperature: %d\n", unCh + 1, unValue/1000);
      }
      break;
      
    case HTR_FUNC_GET_PWM:
      if (obHtr[unCh].GetHtrChPWMMilliP(&unValue) < 0)
      {
        printf("CHeaterCtrl[%d].GetHtrChPWMMilliP() failed.\n", unCh + 1);
      }
      else
      {
        printf("Heater %d PWM percent: %d\n", unCh + 1, unValue/1000);
      }
      break;

    case HTR_FUNC_OFF:
      if (obHtr[unCh].TurnHtrOff() < 0)
      {
        printf("CHeaterCtrl[%d].TurnHtrOff() failed.\n", unCh + 1);
      }
      break;

    case HTR_BOARD_INFO:
    {
#warning "FIXME: Fix this code once the version information is properly implemented"
      unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
      if (obHtr[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev) < 0)
      {
        printf("CHeaterCtrl[%d].GetBoardInfo() failed.\n", unCh + 1);
      }
      else
      {
        printf("Maj: %d, Min: %d, Build: %d, Rev: %d\n", byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev);
      }
    }
    break;

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
    
  }

  
  for (int nHtrs = 0; nHtrs < NR_HTR_CHANNELS; nHtrs++)
  {
    nRetVal = obHtr[nHtrs].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("CHeaterCtrl[%d].Close() failed\n", nHtrs + 1);
    }
  }

}

#define NR_SOL_CHANNELS   12

void TestSol()
{
  static char szSolDevNames[NR_SOL_CHANNELS][50] = { 
    "SOL:SLOT_2:SOL_1",
    "SOL:SLOT_2:SOL_2",
    "SOL:SLOT_2:SOL_3",
    "SOL:SLOT_2:SOL_4",
    "SOL:SLOT_2:SOL_5",
    "SOL:SLOT_2:SOL_6",
    "SOL:SLOT_2:SOL_7",
    "SOL:SLOT_2:SOL_8",
    "SOL:SLOT_2:SOL_9",
    "SOL:SLOT_2:SOL_10",
    "SOL:SLOT_2:SOL_11",
    "SOL:SLOT_2:SOL_12",
  };

  static CSolenoidCtrl obSol[NR_SOL_CHANNELS];

  int nRetVal = 0;

  struct timeval time1, time2;

  typedef struct
  {
    unsigned long int MinTimeuSec;
    unsigned long int MaxTimeuSec;
    unsigned long int Above20mSec;
    unsigned long int NrRuns;
    unsigned long int NrTimeouts;
  } SolStats;

  SolStats stStats;
  unsigned long tempTime;

  stStats.MinTimeuSec = 4294967295UL;
  stStats.MaxTimeuSec = 0;
  stStats.Above20mSec = 0;
  stStats.NrRuns = 0;
  stStats.NrTimeouts = 0;


  for (int nSols = 0; nSols < NR_SOL_CHANNELS; nSols++)
  {
    nRetVal = obSol[nSols].OpenHal(szSolDevNames[nSols]);

    if (nRetVal < 0)
    {
      printf("Error opening Sol Channel: %d\n", nSols + 1);
      return;
    }
  }

  //sleep(1);

  while(g_nExitApp != 1)
  {
    for (int nSols = 0; nSols < NR_SOL_CHANNELS; nSols++)
    {
      if (g_nExitApp)
      {
        break;
      }

      if (gettimeofday(&time1, NULL) < 0)
      {
        printf("Error getting before TurnONSolValve()\n");
      }
      
      nRetVal = obSol[nSols].TurnONSolValve();
      
      if (gettimeofday(&time2, NULL) < 0)
      {
        printf("Error getting time after TurnONSolValve()\n");
      }
      
      if (nRetVal < 0)
      {
        printf("CSolenoidCtrl[%d].TurnONSolValve() failed\n", nSols + 1);
        stStats.NrTimeouts++;
        
        //continue;
        goto sol_exit;
      }

      DEBUG("Sec: %ld, uSec: %ld\n", time2.tv_sec - time1.tv_sec, 
            time2.tv_usec - time1.tv_usec);

      time2.tv_sec = time2.tv_sec - time1.tv_sec;
      if (time2.tv_usec > time1.tv_usec)
      {
        time2.tv_usec -= time1.tv_usec;
      }
      else
      {
        time2.tv_usec = 1000000 + (time2.tv_usec - time1.tv_usec);
      }

      tempTime = (time2.tv_sec * 1000000) + time2.tv_usec;

      stStats.MinTimeuSec = MIN(stStats.MinTimeuSec, tempTime);
      stStats.MaxTimeuSec = MAX(stStats.MaxTimeuSec, tempTime);
      stStats.NrRuns++;
      if (tempTime >= 20000)
      {
        stStats.Above20mSec++;
        printf("TA time above 20ms: Nr: %ld\n", stStats.Above20mSec); 
      }

      
      if (g_nExitApp)
      {
        break;
      }
      //sleep(1);
      usleep(50000);
      

      if (gettimeofday(&time1, NULL) < 0)
      {
        printf("Error getting before TurnOFFSolValve()\n");
      }
      
      nRetVal = obSol[nSols].TurnOFFSolValve();
      
      if (gettimeofday(&time2, NULL) < 0)
      {
        printf("Error getting time after TurnOFFSolValve()\n");
      }
      
      if (nRetVal < 0)
      {
        printf("CSolenoidCtrl[%d].TurnOFFSolValve() failed\n", nSols + 1);
        stStats.NrTimeouts++;
        //continue;
        goto sol_exit;
      }

      DEBUG("Sec: %ld, uSec: %ld\n", time2.tv_sec - time1.tv_sec, 
            time2.tv_usec - time1.tv_usec);

      time2.tv_sec = time2.tv_sec - time1.tv_sec;
      if (time2.tv_usec > time1.tv_usec)
      {
        time2.tv_usec -= time1.tv_usec;
      }
      else
      {
        time2.tv_usec = 1000000 + (time2.tv_usec - time1.tv_usec);
      }

      tempTime = (time2.tv_sec * 1000000) + time2.tv_usec;

      stStats.MinTimeuSec = MIN(stStats.MinTimeuSec, tempTime);
      stStats.MaxTimeuSec = MAX(stStats.MaxTimeuSec, tempTime);
      stStats.NrRuns++;
      if (tempTime >= 20000)
      {
        stStats.Above20mSec++;
      }


      if (g_nExitApp)
      {
        break;
      }
      //sleep(1);
      usleep(50000);
    }
  }

sol_exit:
  printf(" ****** Sol Stats ***** \n");
  printf("MAX TA Time: %ld\n", stStats.MaxTimeuSec);
  printf("MIN TA Time: %ld\n", stStats.MinTimeuSec);
  printf("Number of runs: %ld\n", stStats.NrRuns);
  printf("Above 20 ms TA Time: %ld\n", stStats.Above20mSec);
  printf("Number of timeouts: %ld\n", stStats.NrTimeouts);
  
  DEBUG("Terminating application...\n");
  for (int nSols = 0; nSols < NR_SOL_CHANNELS; nSols++)
  {
    nRetVal = obSol[nSols].CloseHal();

    if (nRetVal < 0)
    {
      printf("Error closing Sol Channel: %d\n", nSols + 1);
      return;
    }
  }
}

#define NR_RTD_CHANNELS   15
//#define NR_RTD_CHANNELS   5

void TestRTD()
{
  static char szRTDDevNames[NR_RTD_CHANNELS][50] = { 
    "RTD:SLOT_2:RTD_1",
    "RTD:SLOT_2:RTD_2",
    "RTD:SLOT_2:RTD_3",
    "RTD:SLOT_2:RTD_4",
    "RTD:SLOT_2:RTD_5",
    "RTD:SLOT_3:RTD_1",
    "RTD:SLOT_3:RTD_2",
    "RTD:SLOT_3:RTD_3",
    "RTD:SLOT_3:RTD_4",
    "RTD:SLOT_3:RTD_5",
    "RTD:SLOT_4:RTD_1",
    "RTD:SLOT_4:RTD_2",
    "RTD:SLOT_4:RTD_3",
    "RTD:SLOT_4:RTD_4",
    "RTD:SLOT_4:RTD_5",
  };

  static CRTD obRTD[NR_RTD_CHANNELS];

  int nRetVal = 0;

  long nRTDVal = 0;
  
  unsigned int unValue = 0;

  struct timeval time1, time2;
  
  static char szHtrDevNames[NR_HTR_CHANNELS][50] = {
    "HTR_CTRL:SLOT_2:HTR_CTRL_1",
    "HTR_CTRL:SLOT_2:HTR_CTRL_2",
    "HTR_CTRL:SLOT_2:HTR_CTRL_3",
    "HTR_CTRL:SLOT_2:HTR_CTRL_4",
  };
  
  static CHeaterCtrl obHtr[NR_HTR_CHANNELS - 2];
  
  for (int nHtrs = 0; nHtrs < NR_HTR_CHANNELS - 2; nHtrs++)
  {
    nRetVal = obHtr[nHtrs].OpenHal(szHtrDevNames[nHtrs]);
    
    if (nRetVal < 0)
    {
      DEBUG("Error opening Htr Channel: %d\n", nHtrs);
      return;
    }
  }

  for (int nRTDs = 0; nRTDs < NR_RTD_CHANNELS; nRTDs++)
  {
    nRetVal = obRTD[nRTDs].OpenHal(szRTDDevNames[nRTDs]);

    if (nRetVal < 0)
    {
      printf("Error opening RTD Channel: %d\n", nRTDs + 1);
      return;
    }
  }

  //sleep(1);

  gettimeofday(&time1, NULL);
  
  while (g_nExitApp != 1) 
  {
    gettimeofday(&time2, NULL);

    
    time2.tv_sec = time2.tv_sec - time1.tv_sec;

    if (time2.tv_usec > time1.tv_usec)
    {
      time2.tv_usec -= time1.tv_usec;
    }
    else
    {
      time2.tv_usec = 1000000 + (time2.tv_usec - time1.tv_usec);
      time2.tv_sec--;
    }

    printf("%ld, %ld ", time2.tv_sec,  time2.tv_usec);


    for (int nRTDs = 0; nRTDs < NR_RTD_CHANNELS; nRTDs++)
    {
      if (g_nExitApp)
      {
        goto exit_rtd;
      }

      nRetVal = obRTD[nRTDs].GetTempInMilliDegC(&nRTDVal);
      if (nRetVal < 0)
      {
        printf("CRTD[%d]..GetTempInMilliDegC() failed\n", nRTDs + 1);
        //continue;
        goto exit_rtd;
      }
      else
      {
        printf("%ld, ", nRTDVal);
      }
    }
    
    for (int nHtrs = 0; nHtrs < NR_HTR_CHANNELS - 2; nHtrs++)
    {
      if (obHtr[nHtrs].GetHtrChPWMMilliP(&unValue) < 0)
      {
        printf("CHeaterCtrl[%d].GetHtrChPWMMilliP() failed.\n", nHtrs + 1);
        goto exit_rtd;
      }
      else
      {
        printf("%d, ", unValue);
      }
    }

    printf("\n");
    //Use the user specified time interval b/n RTD reads....
    if (nIndexFlag)
    {
      usleep(nIndex * 1000);
    }
    //Else use a default interval of 1 second
    else
    {
      usleep(1000000);
    }
  }

exit_rtd: 
  DEBUG("Terminating application...\n");
  for (int nRTDs = 0; nRTDs < NR_RTD_CHANNELS; nRTDs++)
  {
    nRetVal = obRTD[nRTDs].CloseHal();

    if (nRetVal < 0)
    {
      printf("Error closing RTD Channel: %d\n", nRTDs + 1);
      return;
    }
  }

  for (int nHtrs = 0; nHtrs < NR_HTR_CHANNELS - 2; nHtrs++)
  {
    nRetVal = obHtr[nHtrs].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("CHeaterCtrl[%d].Close() failed\n", nHtrs + 1);
    }
  }


}

#define NR_PRE_CHANNELS   2

int TestPreamp(int nPres)
{
  static char szPreDevNames[NR_PRE_CHANNELS][50] = { 
    "PREAMP_STR:SLOT_1:PREAMP_1",
    "PREAMP_STR:SLOT_1:PREAMP_2",
  };

  static CPreampStream obPreampStr[NR_PRE_CHANNELS];

  int nRetVal = 0;
  
  unsigned long long ullTS = 0;
  unsigned int unBridgeData = 0;
  unsigned int unRandomReads = 0;

  if ( (nPres < 0) || (nPres >= NR_PRE_CHANNELS) )
  {
    return -1;
  }

  while (g_nExitApp != 1) //TEST MODE
  {
    DEBUG("****** INITIALIZING PREAMP *******\n");
    //for (int nPres = 0; nPres < 1/*NR_PRE_CHANNELS*/; nPres++)
    {
      nRetVal = obPreampStr[nPres].OpenHal(szPreDevNames[nPres]);

      if (nRetVal < 0)
      {
        printf("Error opening Preamp Channel: %d\n", nPres + 1);
        return 0;
      }
      nRetVal = obPreampStr[nPres].SetBroadcastMode(TRUE);

      if (nRetVal < 0)
      {
        printf("Error enabling broadcast mode on Preamp %d\n", nPres + 1);
        return 0;
      }
    }

    //sleep(1);
    usleep(rand() % 1000001);

    unRandomReads = (rand() % 501);
    DEBUG("Reading %d samples.\n", unRandomReads);

    for (unsigned int nReads = 0; nReads < unRandomReads; nReads++)
    {
      //for (int nPres = 0; nPres < 1/*NR_PRE_CHANNELS*/; nPres++)
      {

        nRetVal = obPreampStr[nPres].ReadStreamDataBlocking(&unBridgeData, &ullTS);
        //nRetVal = obPreamp[nPres].ReadStreamData(&unBridgeData, &ullTS, 1000);
        if (nRetVal < 0)
        {
          DEBUG("CPreamp[%d]..ReadStreamDataBlocking() failed\n", nPres + 1);
          continue;
        }
        else
        {
          if (nRetVal > 6)
          {
            printf("Received more data than expected.\n");
          }
          else
          {
            //DEBUG("Preamp: %d data\n", nPres + 1);
#if 0
            for (int nCnt = 0; nCnt < nRetVal; nCnt++)
            {
              DEBUG("%d ", ucData[nCnt]);
            }
            DEBUG("\n");
#endif
            //DEBUG("Temp: %d, TS: %lld\n", unBridgeData, ullTS);
          }
        }
      }
    }
  
    DEBUG("****** CLOSING PREAMP *******\n");
    //for (int nPres = 0; nPres < 1/*NR_PRE_CHANNELS*/; nPres++)
    {
      nRetVal = obPreampStr[nPres].SetBroadcastMode(FALSE);

      if (nRetVal < 0)
      {
        printf("Error disabling broadcast mode on Preamp %d\n", nPres + 1);
        return 0;
      }
    
      nRetVal = obPreampStr[nPres].CloseHal();

      if (nRetVal < 0)
      {
        printf("Error closing Preamp channel: %d\n", nPres + 1);
        return 0;
      }

    }

    usleep( rand() % 1000001);
  }

  return 0;
}


void PrintHelp()
{
  printf("Application usage\n");
  printf("<app_name> -m <app_mode> -n <value>\n");

  printf("<app_mode>:\n");
  printf("  s: Test Solenoid\n");
  printf("  t: Test RTD\n");
  printf("  h: Test Heater Control\n");
  printf("  p: Test Preamp - streaming\n");
  printf("<value> (Optional):\n");
  printf("  Channel No.:  0 to 1 when 'app_mode' is p\n");
  printf("  Time interval: In milliseconds, b/n RTD channel reads when 'app_mode' is t\n");
  printf("  Not valid for other modes.\n");
}

#define APP_MODE_SOL    0
#define APP_MODE_RTD    1
#define APP_MODE_HTR    2
#define APP_MODE_PREAMP 3


int main (int argc, char *argv[])
{
  int optVal = 0;
  int appMode = 0;

  //char szDevName[] = "ANA_IN:SLOT_1:ANA_IN_1";
  //TestAnalogInput (szDevName);

  //char szDevName[] = "HTR_CTRL:SLOT_1:HTR_CTRL_1";
  //TestHeaterCtrl (szDevName);

  if (argc < 2)
  {
    PrintHelp();
    return -1;
  }

  while (argv[optind] != NULL)
  {
    optVal = getopt(argc, argv, "m:n:v");

    switch(optVal)
    {
    case 'm':
      if (optarg != NULL)
      {
        if (strlen(optarg) > 1)
        {
          DEBUG("Not a valid mode.\n");
          PrintHelp();
          return -1;
        }
        switch(optarg[0])
        {
        case 's':
          appMode = APP_MODE_SOL;
          break;

        case 't':
          appMode = APP_MODE_RTD;
          break;

        case 'h':
          appMode = APP_MODE_HTR;
          break;

        case 'p':
          appMode = APP_MODE_PREAMP;
          break;

        default:
          printf("Not a valid mode.\n");
          PrintHelp();
          return -1;
        }
      }
      else
      {
        printf("Not a valid mode.\n");
        PrintHelp();
        return -1;
      }
      break;

    case 'n':
      if (sscanf(optarg, "%d", &nIndex) < 0)
      {
        printf("Not a valid value. Try again.\n");
        return -1;
      }
      else
      {
        nIndexFlag = 1;
      }
      break;

    case 'v':
      g_nVerbose = 1;
      break;

    default:
      printf("Improper usage\n");
      PrintHelp();
      return -1;
    }
  }

  //Install a signal handler for SIGINT - used to display app statistics
  if (SIG_ERR == signal(SIGINT, SIGINTHandler))
  {
    printf("Failed to install signal handler.\n");
  }
                
  switch(appMode)
  {
  case APP_MODE_SOL:
    TestSol();
    break;

  case APP_MODE_RTD:
    TestRTD();
    break;

  case APP_MODE_HTR:
    TestHeaterCtrl();
    break;

  case APP_MODE_PREAMP:
    if (TestPreamp(nIndex) < 0)
    {
      DEBUG("Invalid channel number.\n");
    }
    break;
  }

  return 0;
  
}


