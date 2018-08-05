/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: TestHAL.cpp
 * *
 * *  Description: HAL and device function test application.
 * *               See README for details
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
 * *  $Id: TestHAL.cpp,v 1.99 2017/01/17 17:04:30 emrsn\jbeatty Exp $
 * *  $Log: TestHAL.cpp,v $
 * *  Revision 1.99  2017/01/17 17:04:30  emrsn\jbeatty
 * *  Added new diagnostic to continuously read board temperature and voltages for selected Preamp (Slot 1, 3, and 5) or Heater Solenoid (Slot 2 and 4).
 * *
 * *  Revision 1.98  2016/11/17 13:51:01  emrsn\jbeatty
 * *  Modified to match the FID/FPD slot swaps in v2 1500XA R2 backplane
 * *
 * *  Revision 1.97  2016/11/01 13:06:43  emrsn\jbeatty
 * *  Added support for FPD G2
 * *
 * *  Revision 1.96  2016/09/06 13:30:33  emrsn\jbeatty
 * *  Added missing newline after FID autozero failure error message
 * *
 * *  Revision 1.95  2016/09/02 15:22:40  emrsn\jbeatty
 * *  Fixes for 1500XA R2 FIDs
 * *
 * *  Revision 1.94  2016/08/30 09:58:26  emrsn\jbeatty
 * *  Mods for 1500XA R2 support
 * *
 * *  Revision 1.93  2016/08/09 17:10:12  emrsn\jbeatty
 * *  Support for 1500XA R2
 * *
 * *  Revision 1.92  2016/07/27 17:58:45  emrsn\wjyu
 * *  added 'Reset IMB data' function in the TestHAL application (for the Test Team)
 * *
 * *  Revision 1.91  2016/06/22 20:44:01  emrsn\wjyu
 * *  revised to handle issue - add '\0' to the temp timed event adjustment string
 * *
 * *  Revision 1.90  2016/06/13 16:21:08  emrsn\wjyu
 * *  Revised TestHAL application to read IMB using new data layout
 * *
 * *  Revision 1.89  2016/01/08 20:32:58  emrsn\rrthomas
 * *  Add ability to read PWM using the RTD enum for debugging purposes.
 * *
 * *  Revision 1.88  2015/05/04 15:05:15  EMRSN\sboyidi
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files as per below requirements.
 * *        -Set the indent style to C/C++.
 * *        -Set the indent size to 2, and replace tab with blank space.
 * *        -Turn off interpretation of tab.
 * *
 * *  Revision 1.87  2014/10/22 18:11:09  emrsn\wjyu
 * *  In TestHAL app, use IMBComm function call to convert IMB serial number raw bytes to hex string
 * *  (Before, it was converted in the TestHAL app)
 * *
 * *  Revision 1.86  2014/10/03 20:57:10  emrsn\wjyu
 * *  Revised GetFlashImage() and SetFlashImage() API to support FACTORY and MIRROR section.
 * *  Before, those two functions only support the GLOBAL section
 * *  (For 6587 GC should not allow reading Module data if IMB is Incompatible )
 * *
 * *  Revision 1.85  2014/07/25 19:57:59  emrsn\rrthomas
 * *  Modify TestRTD to read RTD temps from 370XA. Also took out code that was reading BTU at end of cycle from the ReadRTD routine. This is no longer needed.
 * *
 * *  Revision 1.84  2014/06/26 09:59:04  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.83  2014/05/14 10:11:09  emrsn\wjyu
 * *  For IMB firmware 1.0.1
 * *  Revised TestHAL, IMB serial number is in IMB Info struct instead of Global Data
 * *
 * *  Revision 1.82  2014/05/09 14:34:37  emrsn\wjyu
 * *  For BZ 7326 The IMB DEFAULT section will be removed
 * *  Revised TestHAl app
 * *
 * *  Revision 1.81  2014/05/02 19:55:02  emrsn\wjyu
 * *  For BZ 7292 - Need to store the 12 digit Factory Assigned Module Serial Number in IMB
 * *  Revised TestHAL app
 * *
 * *  Revision 1.80  2014/03/20 18:30:53  emrsn\wjyu
 * *  Change column current_backup_time to backup_time
 * *  This is to reflect the AVT table change described in BZ 7234
 * *
 * *  Revision 1.79  2013/11/21 19:36:05  emrsn\rrthomas
 * *  Added another test for KeyLock Switch - option 'Z' will check if Db Pt 2 changes state. This test is for checking if we detect spurious changes on the KeyLock input DIs.
 * *
 * *  Revision 1.78  2013/11/20 21:08:23  emrsn\rrthomas
 * *  Added a closed loop test for KeyLock Switch.
 * *
 * *  Setup - Install ROC DO board in Exp Slot 1. Connect the DO1 to the KeyLock Switch.
 * *  Test Description - "TestHAL -mz" closes (or opens) the KeyLock switch contact, waits for the switch state to stabilize, then reads the "Is Configuration Protected" DB point to see if the firmware has read the state correctly. It then waits for a random delay (in the range 0 microseconds to 10 seconds) and after this timeout, starts the process over again.
 * *
 * *  Revision 1.77  2013/10/08 15:09:47  emrsn\rrthomas
 * *  EPC Pressure Sensor Temperature correction made configurable using MON2020. Here is the correction formula (implemented in Analyzer IO firmware) -
 * *
 * *  Corrected_Carrier_Pressure = Carrier_Pressure  (Ambient_temperature  base_temperature) * pressure_correction
 * *
 * *  "Ambient_temperature" is read from RTD2 which is mounted on the IMB Board in close proximity to the Pressure Sensor.
 * *  "base_temperature" is available in EPC_CONFIG table.
 * *  "pressure_correction" is available in EPC_CONFIG table.
 * *
 * *  Revision 1.76  2013/05/20 20:56:55  emrsn\wjyu
 * *  Added function to update IMB flash
 * *
 * *  Revision 1.75  2013/05/15 06:26:10  EMRSN\jchekkala
 * *  Bug fix 6711 - Revised to support storing four Dependent TEVs for each
 * *  component. The component index will be stored on the IMB instead of
 * *  component names and Dependent TEV will be stored in single dimensional
 * *  character array seperated by semicolon.
 * *
 * *  Revision 1.74  2013/05/01 18:46:43  emrsn\wjyu
 * *  For IMB firmware version 0.0.15
 * *  Removed Timed Events Adjustments structure from the Mirror Data Section
 * *  CDT will store component codes (1 byte) instead of full component names
 * *
 * *  Revision 1.73  2013/03/21 19:46:46  emrsn\wjyu
 * *  For IMB firmware version 0.0.12
 * *  Reduced buffer size for Get Flash Image from 256 bytes to 128 bytes
 * *
 * *  Revision 1.72  2013/03/07 14:47:47  emrsn\wjyu
 * *  Add function to read IMB Flash Image
 * *
 * *  Revision 1.71  2013/02/25 14:15:07  emrsn\wjyu
 * *  Commented out the ideal RF limit in the Global AVT settings. It is no longer used. It is now stored in the CDT and is component specific.
 * *
 * *  Revision 1.70  2013/02/11 08:30:08  emrsn\rsaha
 * *  Revised to test section identifiers as they are added back to each section.
 * *
 * *  Revision 1.69  2013/02/08 17:23:23  emrsn\rsaha
 * *  Revised to test newly added method GetDBID() of IMB HAL interface.
 * *
 * *  Revision 1.68  2013/01/29 14:38:41  emrsn\rsaha
 * *  Revised to test latest changes made to 370XA Analyzer IO and IMB firmware.
 * *
 * *  Revision 1.67  2013/01/03 12:03:00  emrsn\rsaha
 * *  Revised to test newly revised IMB firmware -
 * *
 * *  - Database ID is moved from Global Data section. Now Factory, Mirror and Default data section all have their individual Database Identifier field.- Time Event Adjustment Data also made part of Mirror Data Section.
 * *  - Revised Project settings to use 512 bytes less for stack region, since due to TEV Adj Data structure inclusion in Mirror Data Section there is increase of about 385 bytes of RAM usage.
 * *
 * *  Revision 1.66  2012/11/30 09:05:11  emrsn\rsaha
 * *  Revised to test HAL changes made for IMB.
 * *
 * *  Revision 1.65  2012/09/24 03:58:46  emrsn\rsaha
 * *  Revised to test newly modified HAL API to Enable/Disable 700XA Preamp IO AutoZero algorithm.
 * *
 * *  Revision 1.64  2012/09/18 13:14:56  emrsn\rsaha
 * *  Revised the following -
 * *  1. Enable 700XA Preamp AutoZero.
 * *  2. Read Left and Right Bridge detector status.
 * *  3. Support to test Analyzer Detector functionality.
 * *
 * *  Revision 1.63  2012/04/25 09:38:45  emrsn\rsaha
 * *  Revised to test Analog Output and long run loop back Analog Output and Analog Input testing for 370XA GC.
 * *
 * *  Revision 1.62  2012/03/30 06:12:37  emrsn\rsaha
 * *  Revised to wait for BaseIO flusing data over serial while performing a file transfer operation.
 * *
 * *  Revision 1.61  2012/03/21 06:48:32  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.60  2012/02/07 11:44:31  emrsn\rsaha
 * *  Revised to test newly added HAL IMB APIs.
 * *
 * *  Added two new options to quickly test and validate different section data on IMB board via automatically executing all the HAL IMB APIs one after other. To do the same -
 * *  1. Use option 33 to execute all SET APIs to write dummy random data to all the sections in IMB flash.
 * *  2. Use option 34 to execute all GET APIs to read this dummy random data back from IMB flash and compare with the written data to result success or failure.
 * *
 * *  Revision 1.59  2012/01/31 15:10:54  emrsn\rsaha
 * *  Revised to test newly updated IMB communicaiton APIs in HAL.
 * *
 * *  Revision 1.58  2012/01/03 13:14:46  emrsn\rsaha
 * *  Revised to test newly added HAL APIs and check IMB communication. One need to launch TestHAL with "-m g" option to run IMB test.
 * *  Please refer TestIMBComm() to understand how to use HAL APIs to communicate with IMB device.
 * *
 * *  Revision 1.57  2011/07/13 15:06:12  emrsn\rsaha
 * *  Fixed compilation warning occuring due to wrong MACRO name.
 * *
 * *  Revision 1.56  2011/06/01 08:23:43  emrsn\rsaha
 * *  Revised to test extended EPC interfaces added to HAL for 370XA.
 * *
 * *  Revision 1.55  2011/03/24 15:26:59  emrsn\rsaha
 * *  Added methods to do long duration continuos ROCIO AI-AO, DI-DO testings.
 * *
 * *  Revision 1.54  2011/02/19 13:00:03  emrsn\rsaha
 * *  Revised to test ROCIO DIs and DOs for both expansion slot 1 and 2.
 * *
 * *  Revision 1.53  2011/02/15 15:57:17  emrsn\rsaha
 * *  Revised to test ROCIO AIs.
 * *
 * *  Revision 1.52  2010/10/14 18:15:34  emrsn\rsaha
 * *  Revised the following -
 * *  1. Option to read Analog Input continuously.
 * *  2. Option to Set/Get/Mark Compensation Base Temperature.
 * *  3. Option to Set/Get Compensation temperature slope.
 * *  4. Support to test all 4 Analyser IO Digital Input channels.
 * *
 * *  Revision 1.51  2010/09/21 21:45:03  emrsn\LRatner
 * *  Using NUM_STREAM_CONFIG_INFO definition.
 * *
 * *  Revision 1.50  2010/09/17 20:43:08  emrsn\LRatner
 * *  Getting MAX_STREAM count from global.h
 * *
 * *  Revision 1.49  2010/06/10 20:27:03  emrsn\rsaha
 * *  Revised to test newly added Pressure read and Serial RS232/485 control functionality of the Analyzer IO card. Also reviesed code as necessary to test functionalities which are common between Analyzer IO of 370XA and HeaterSolenoid, BaseIO, Preamp of 700XA.
 * *
 * *  Revision 1.48  2010/05/06 23:14:23  EMRSN\RRTHOMAS
 * *  Changed the "-m a" (CPU ADC) funtion to read all the channels continously from CPU ADCs.
 * *
 * *  Revision 1.47  2010/01/27 21:57:34  emrsn\rrthomas
 * *  FPD ADC readback fixes
 * *  - Changed configuration of both ADC channels to Bipolar. AD7705 cannot be setup as with one channel as bipolar and the other as unipolar. Changed the voltage translation to use the bipolar voltage range of +/- 5 Volt instead of 0 to 5V.
 * *  - Added HAL APIs for reading FPD ADC raw ADC counts (for testing)
 * *  - Added TestHAL methods to read FPD raw ADC counts (for testing)
 * *
 * *  This code is WIP, tested and works but ADC setup needs cleanup.
 * *
 * *  Revision 1.46  2010/01/18 14:56:15  emrsn\rsaha
 * *  Added one more option in FPD test menu to read AutoZero DAC offset and Preamplifier voltages continiously.
 * *
 * *  Revision 1.45  2009/12/17 23:40:52  emrsn\rrthomas
 * *  Added a new method to print Heater (or RTD) temperature and PWM continously for temperature stability tests.
 * *
 * *  Revision 1.44  2009/12/17 14:14:12  emrsn\rsaha
 * *  Revised to test FPD functionality newly added to HAL layer.
 * *
 * *  Revision 1.43  2009/12/15 23:08:06  emrsn\rrthomas
 * *  Print date (along with time) for the TestRTD routine. This will help to plot the temperature data in Excel using XY scatter.
 * *
 * *  Revision 1.42  2009/11/02 12:56:11  epm-na\rsaha
 * *  Revised to use updated FID HAL interface. Added a option to do FID Gain change to high, Null Electrometer and Auto Zero with user specified interval inbetween. This is to debug time delay requirements after FID Gain change and Null Electrometer.
 * *
 * *  Revision 1.41  2009/10/27 06:36:33  epm-na\rsaha
 * *  Updated TestFID to test the following -
 * *  - Read AOUT continuously all 3 FID ADC channels.
 * *  - Read ADC counts continuously for all 3 FID ADC channels.
 * *  - Set Coarse Adjustment Value.
 * *  - Set Fine Adjustment Value.
 * *  - Increase DAC1 Coarse Adjustment. (Change current DAC1 count to [DAC1 Count + Coarse Value])
 * *  - Decrease DAC1 Coarse Adjustment. (Change current DAC1 count to [DAC1 Count - Coarse Value])
 * *  - Increase DAC1 Fine Adjustment. (Change current DAC1 count to [DAC1 Count + Fine Value])\n")
 * *  - Decrease DAC1 Fine Adjustment. (Change current DAC1 count to [DAC1 Count - Fine Value])\n")
 * *  - Increase DAC1 by Fine Adjustment continiously.
 * *  - Decrease DAC1 by Fine Adjustment continiously.
 * *  - Trace changes to Preamp Det2 with DAC1 incremental(Fine Adjustment) changes.
 * *  - Read current Preamp Det2 reading.
 * *  - Adjust Preamp Det2 reading.
 * *  - Set FID Gain HIGH and then Baseline Preamp Det2 reading.
 * *  - Set FID Gain LOW and then Baseline Preamp Det2 reading.
 * *  - Set delay time after FID Gain Change. (For option \'B\' and \'b\')
 * *
 * *  Revision 1.40  2009/09/11 14:41:51  epm-na\rsaha
 * *  Revised to test newly added command to set Heater type as  - AC or DC, in Heater Control Function.
 * *
 * *  Revision 1.39  2009/09/09 19:56:48  EPM-NA\lratner
 * *  Modified the Preamp Streaming HAL object to detect and remove spurious spikes from the preamp data before sending it to peak_processing. This approach uses three points and computes the slope to identify spikes. Once a spike it identified, the bad sample is replaced with an average of the previous and next point.
 * *  Above-mentioned changes were made only in PreampStream.cpp and PreampStream.h, but I had to move #include <sstream> from h files to cpp, hence the other checkins! Without this, adding #include <vector> in PreampStream.h was causing several compilation errors related to redefinition of "min" and "max"!
 * *
 * *  Revision 1.38  2009/07/18 09:04:28  epm-na\rsaha
 * *  Enabled code to count number of retries made in Reliability layer.
 * *
 * *  Revision 1.37  2009/06/25 19:05:06  epm-na\rsaha
 * *  Revised to test IR Keypad related HAL interfaces.
 * *
 * *  Revision 1.36  2009/06/23 16:17:59  epm-na\rsaha
 * *  Revise to test HAL interfaces added to read CPU 8 channel ADCs.
 * *
 * *  Revision 1.35  2009/06/03 15:46:43  epm-na\rsaha
 * *  Revised to test FID DAC-ADC, Null Electrometer, Auto Zero.
 * *
 * *  Revision 1.34  2009/05/14 14:08:56  epm-na\rsaha
 * *  Revised to test HAL APIs to query FFB time, FFB Device Capability information.
 * *
 * *  Revision 1.33  2009/04/15 13:38:06  epm-na\rsaha
 * *  Revised to test FID using newly added HAL APIs.
 * *
 * *  Revision 1.32  2009/04/07 22:18:33  epm-na\rsaha
 * *  Revised to print FFB server 64 bit time after converting it from 1/32msec unit to 1sec unit.
 * *
 * *  Revision 1.31  2009/04/06 21:30:49  epm-na\rsaha
 * *  Modified to test HAL api of retrieving 64 bit time from FFB board.
 * *
 * *  Revision 1.30  2009/04/02 21:20:33  epm-na\rsaha
 * *  Corrected typo error.
 * *
 * *  Revision 1.29  2009/02/24 17:16:38  epm-na\rsaha
 * *  Revised to display build number of the version information.
 * *
 * *  Revision 1.28  2009/01/26 22:26:59  epm-na\rsaha
 * *  Revised to send Detector status information via HAL interface.
 * *
 * *  Revision 1.27  2009/01/06 17:11:15  epm-na\rsaha
 * *  Added user option to send status message in every 1 sec to FFB board and to send three different set of PV variable data.
 * *  This changes are implemented to meet testing requirement of Minnesota FFB-team.
 * *
 * *  Revision 1.26  2008/11/25 00:17:42  epm-na\rsaha
 * *  Revised to test diagnostic APIs added to HAL layer.
 * *
 * *  Revision 1.25  2008/11/17 22:16:17  epm-na\rsaha
 * *  Update to test newly added HAL interface to retrive scalilng factor from preamp.
 * *
 * *  Revision 1.24  2008/11/16 00:23:13  epm-na\rthomas
 * *  Added support for testing ROC Analog Output Expansion Cards in the "Test Analog Output" function.
 * *
 * *  Revision 1.23  2008/11/11 00:12:15  epm-na\rsaha
 * *  Updated to test the newly added HAL APIs for FFB-GC communication.
 * *
 * *  Revision 1.22  2008/10/29 17:43:27  epm-na\rsaha
 * *  Revised SendGCStatus2FFBBoard() and GetRemoteRequest4mFFBBoard() methods to check the flag updated by SIGINT signal handler before executing the next loop. Previously they were in a continious while loop.
 * *
 * *  Revision 1.21  2008/10/22 22:56:44  epm-na\rsaha
 * *  Initial version to test the communication stack of FFB board.
 * *  Added methods SendGCStatus2FFBBoard() and GetRemoteRequest4mFFBBoard() to test the FFB board functionality.
 * *  SendGCStatus2FFBBoard() - This sends a variable length random data to FFB board and waits for a acknowledgement.
 * *  GetRemoteRequest4mFFBBoard() - This listens to the stream pipe to receive variable length data sent by FFB board and log errors on screen.
 * *  Added the following command line options.
 * *  f - Opens a status line connection with FFB board and keep on sending random size data.
 * *  F - Opens a command line connection with FFB board and keeps listening to data sent by FFB board.
 * *  To test run two instances of TestHAL, first one with 'F and second one with 'f' option.
 * *  The test application (started with f option) sends each data packet with a sequence number attached infront to FFB board, and logs how many times it has to retry beacuse of time-out and how many times transmission failed/succeded against total attempts. The FFB board sends this data back to GC as a remote request. The application (started with F option) receives this data and checks how many data packets are a) lost, b) repeated, c) corrupted due to CRC error.
 * *
 * *  Revision 1.20  2008/04/30 17:58:35  epm-na\rthomas
 * *  Added code to read EPC Pressure continuously and to print them to the console.
 * *
 * *  Revision 1.19  2008/04/11 21:06:58  epm-na\rthomas
 * *  Temporary checkin for realiability testing... HACKED code into TestHAL / TestRTD to print "stream_1_calcs.gross_dry_btu" whenever it changes... This checkin needs to be cleaned up...
 * *
 * *  Revision 1.18  2008/04/11 15:18:28  epm-na\rthomas
 * *  Includes following changes -
 * *  (1) Added two commands in TestPreamp (Configuration) to switch ON / OFF broadcast. This was done to test failures in "Device Discovery" when the GC motherboard is rebooted with the Preamp still sending out streaming data.
 * *  (2) Changed the RTD Test Routine to output a timestamp when printing the temperatures of the 5 RTDs.
 * *
 * *  Revision 1.17  2008/01/03 18:26:59  epm-na\lratner
 * *  Resolved most of the compiler warnings that are not caused by '#warning' pragmas.
 * *
 * *  Revision 1.16  2007/04/26 08:57:36  epm-na\rthomas
 * *   - Added functions to read CAL status (in progress or not) through
 * *     either premap config or streaming objects. Updated test app to test
 * *     the same.
 * *
 * *  Revision 1.15  2007/04/25 11:01:35  epm-na\rthomas
 * *   - Added ability to enable self calibration through preamp stream and
 * *     config objects; updated test application to test the same.
 * *
 * *  Revision 1.14  2007/04/05 13:21:29  epm-na\rthomas
 * *  Added code for testing LTLOI. Use the '-m l' option to test LTLOI
 * *
 * *  Revision 1.13  2007/03/27 08:55:00  epm-na\rthomas
 * *  Preamp:
 * *   - Replaced references to 2X and 20X with LOW and HIGH gain respectively.
 * *
 * *  Revision 1.12  2007/03/13 08:38:25  epm-na\rthomas
 * *  - Added option to get preamp value through the preamp config object.
 * *
 * *  Revision 1.11  2006/12/15 16:32:41  epm-na\DPrabhakar
 * *  CSerial's Read() and Write() APIs were clashing the Read and Write macros
 * *  defined for GC700XP. Have changed the names to ReadCh(), ReadChBlocking(),
 * *  and WriteCh() to work around this issue.
 * *
 * *  Related changes to the TestHAL application.
 * *
 * *  Revision 1.10  2006/12/13 22:37:44  epm-na\DPrabhakar
 * *  Serial IO - reduced the number of bytes being read when testing
 * *    full duplex (send and receive)
 * *
 * *  Revision 1.9  2006/12/08 21:29:01  epm-na\DPrabhakar
 * *  - Added Daniel file headers.
 * *  - Added README file containing application execution information.
 * *  - Updated TestHAL for testing Analog Input and Analog Output.
 * *
 * *
 * *************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <errno.h>


#include "AnalogIn.h"
#include "AnalogOut.h"
#include "HeaterCtrl.h"
#include "SolenoidCtrl.h"
#include "RTD.h"
#include "PreampStream.h"
#include "PreampConfig.h"
#include "PreampProtocol.h"
#include "Serial.h"
#include "DigitalIn.h"
#include "DigitalOut.h"
#include "EPC.h"
#include "LtLoi.h"
#include "Diagnostic.h"
#include "FIDControl.h"
#include "FFBComm.h"
#include "FIDOperations.h"
#include "CPU_ADC_AD7908.h"
#include "IRKeyPad.h"
#include "FPDControl.h"
#include "FPD_ADC_7705.h"
#include "Pressure.h"
#include "SerialModeCtrl.h"
#include "IMBComm.h"
#include "FpdG2control.h"
#include "hardwareHelpers.h"

#include "tableapi.hpp"

int nIndex = 0;
int nIndexFlag = 0;

int g_nExitApp = 0;

int g_nVerbose = 0;

int g_nConfigure = 0;

bool g_b370XAIOBoards = false;


void TestTempStability();

//local prototypes

// DEFAULT SECTION no longer used, wyu - May, 2014
//static void TestIMBDefaultData(IMB_DEFAULT_DATA_STRUCT& stSetDataStruct, IMB_DEFAULT_DATA_STRUCT& stGetDataStruct);

DB_INT32 registerTable(DBCLIENT* dbcOpenPtr, DB_INT32 dbPoint);

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

DB_BOOLEAN GetKeyLockState (DBCLIENT *dbcOpenPtr, DB_BOOLEAN &state)
{
  DBAPI_PT_LIST_STRUCT pts[1] = {
    {DB_PT_IS_CONFIG_PROTECTED, UNIT_NA, TYPE_BOOLEAN, NULL},
  };

  DBAPI_DATA_LIST_STRUCT* dataListPtr;
  if (DbcRead(dbcOpenPtr, pts, &dataListPtr) <= 0)
  {
    return false;
  }
  else
  {
    state = dataListPtr[0].data.asBoolean;
    return true;
  }
}

void TestKeyLockSW()
{
  int fd; 
  char szDiscreteON[] = "bn 0x81";
  char szDiscreteOFF[] = "bn 0x80";
  char *ptr = szDiscreteON;

  int length = strlen (szDiscreteON);
  DB_BOOLEAN keyLockState, testKeyLockEngaged = true; 

  unsigned int sleepDur, count = 0, fail = 0;

  DBCLIENT *dbcOpenPtr = DbcOpen(TASK_ID_GC, (ACCESS_LEVEL_ENUM)SYSTEM, 2);
  if (dbcOpenPtr == NULL)
  {
    printf ("Oops: DbcOpen failed!\n");
    return;
  }

  fd = open ("/dev/rocio1", O_RDWR);
  if (fd == -1)
  {
    printf ("Oops: open failed with error (%d)\n", errno);
    DbcClose (dbcOpenPtr);
    return; 
  }

  while (!g_nExitApp)
  {
    ++count;

    if (write (fd, ptr, length) != length)
    {
      printf ("Oops: write failed with error (%d)\n", errno);
    }
    else
    {
      sleep (2); //Wait for input to settle and for xpdiod/xpltloid to read the change
    
      if (GetKeyLockState (dbcOpenPtr, keyLockState))
      {
        //printf ("Got %d\n", keyLockState);
        
        if (keyLockState != testKeyLockEngaged)
        {
          ++fail;
          printf ("Oops: [%u/%u] No match. Sent %s Got %d\n", fail, count, ptr, keyLockState);
        }
        //else
        //printf ("Yahoo: Match\n");
        
        if (testKeyLockEngaged)
        {
          testKeyLockEngaged = false;
          ptr = szDiscreteOFF;
        }
        else
        {
          testKeyLockEngaged = true;
          ptr = szDiscreteON;
        }
        
      }
      else
      {
        printf ("GetKeyLockState failed!\n");
      }

      sleepDur = rand() % 10000000;
      //printf ("Sleeping for %uus\n", sleepDur);
      usleep (sleepDur);
    }
  }

  printf ("Test Count = %u Failed = %u\n", count, fail);

  DbcClose (dbcOpenPtr);
  close (fd);
}

void TrackKeyLockSWChanges ()
{
  int ret = 0, maxFd = 0;
  fd_set watchSet;
  DBCLIENT *dbcOpenPtr;

  dbcOpenPtr = DbcOpen(TASK_ID_GC, (ACCESS_LEVEL_ENUM)SYSTEM, 2);
  if (dbcOpenPtr == NULL)
  {
    printf ("Oops: DbcOpen failed!\n");
    return;
  }

  registerTable(dbcOpenPtr, DB_PT_IS_CONFIG_PROTECTED);
  
  while (!g_nExitApp)
  {
    maxFd = dbcOpenPtr->registerPipeFd;
    FD_ZERO(&watchSet);
    FD_CLR((int)dbcOpenPtr->registerPipeFd, &watchSet);
    FD_SET((int)dbcOpenPtr->registerPipeFd, &watchSet);
      
    ret = select(maxFd + 1, &watchSet, NULL, NULL, NULL);
    if (ret <= 0) 
    {
      printf ("select returned (%d) error (%d)\n", ret, errno);

      if (ret == -1 && errno == EINTR)
        continue; // Maybe we caught a signal
    }
      
    if(FD_ISSET((int)dbcOpenPtr->registerPipeFd, &watchSet))
    {
      DB_INT32  newUpdate = 0;
      DBAPI_DATA_LIST_STRUCT *dataListPtr;
      if( ( newUpdate = DbcWait(dbcOpenPtr, &dataListPtr, 0) ) == 1)
      {
        while(dataListPtr)
        {
          if(dataListPtr->dbPoint == DB_PT_IS_CONFIG_PROTECTED)
          {
            printf ("Got %d\n", dataListPtr->data.asBoolean);
          }
        
          dataListPtr = dataListPtr->nextPtr;
        }
      }
    }
  }

  DbcClose (dbcOpenPtr);
}

#define NR_AIN_CHANNELS            18

#define AIN_FUNC_GET_CURRENT       1
#define AIN_FUNC_GET_VOLTAGE       2
#define AIN_FUNC_GET_BRD_TEMP      3
#define AIN_BOARD_INFO             4
#define AIN_FUNC_EXIT              5
#define AIN_FUNC_GET_CURRENT_CONT  6
#define AIN_FUNC_GET_VOLTAGE_CONT  7

void TestAnalogIn()
{
  int nRetVal = 0;

  static char *szAinDevNames[NR_AIN_CHANNELS] = {NULL,NULL};
  int nNumAnalogInChannels = NR_AIN_CHANNELS;
  if (g_b370XAIOBoards)
  {
    nNumAnalogInChannels = 1;
    szAinDevNames[0] = "ANA_IN:ANALYZER_SLOT:ANA_IN_1";
  }
  else
  {
    szAinDevNames[0] = "ANA_IN:SLOT_BASE_IO:ANA_IN_1";
    szAinDevNames[1] = "ANA_IN:SLOT_BASE_IO:ANA_IN_2";
    szAinDevNames[2] = "ANA_IN:EXP_SLOT_1:ANA_IN_1";
    szAinDevNames[3] = "ANA_IN:EXP_SLOT_1:ANA_IN_2";
    szAinDevNames[4] = "ANA_IN:EXP_SLOT_1:ANA_IN_3";
    szAinDevNames[5] = "ANA_IN:EXP_SLOT_1:ANA_IN_4";
    szAinDevNames[6] = "ANA_IN:EXP_SLOT_2:ANA_IN_1";
    szAinDevNames[7] = "ANA_IN:EXP_SLOT_2:ANA_IN_2";
    szAinDevNames[8] = "ANA_IN:EXP_SLOT_2:ANA_IN_3";
    szAinDevNames[9] = "ANA_IN:EXP_SLOT_2:ANA_IN_4";
    szAinDevNames[10] = "ANA_IN:EXP_SLOT_3:ANA_IN_1";
    szAinDevNames[11] = "ANA_IN:EXP_SLOT_3:ANA_IN_2";
    szAinDevNames[12] = "ANA_IN:EXP_SLOT_3:ANA_IN_3";
    szAinDevNames[13] = "ANA_IN:EXP_SLOT_3:ANA_IN_4";
    szAinDevNames[14] = "ANA_IN:EXP_SLOT_4:ANA_IN_1";
    szAinDevNames[15] = "ANA_IN:EXP_SLOT_4:ANA_IN_2";
    szAinDevNames[16] = "ANA_IN:EXP_SLOT_4:ANA_IN_3";
    szAinDevNames[17] = "ANA_IN:EXP_SLOT_4:ANA_IN_4";
  }

  static CAnalogIn obAin[NR_AIN_CHANNELS];

  int nAinFunc = 0;
  unsigned int unCh = 0;
  unsigned int unValue = 0;
  unsigned long ulValue = 0;
  int nValue;

  int nStartAICh = 0;
  int nEndAICh = 0;
  if (g_b370XAIOBoards)
  {
    nStartAICh = 0;
    nEndAICh = 0;
  }
  else
  {
    int nAISlot = 0;
    printf("Enter AI slot -\n 1. BaseIO\n 2. rocio1\n 3. rocio2\n 4. rocio3\n 5. rocio4\n - ");
    fflush(stdin);
    scanf("%d", &nAISlot);
    switch (nAISlot)
    {
    case 1:
      nStartAICh = 0;
      nEndAICh = 1;
      break;
    case 2:
      nStartAICh = 2;
      nEndAICh = 5;
      break;
    case 3:
      nStartAICh = 6;
      nEndAICh = 9;
      break;
    case 4:
      nStartAICh = 10;
      nEndAICh = 13;
      break;
    case 5:
      nStartAICh = 14;
      nEndAICh = 17;
      break;
    default:
      printf("Entered AI slot is invalid!!!");
      return;
    }
  }

  for (int nAins = nStartAICh; nAins <= nEndAICh; nAins++)
  {
    nRetVal = obAin[nAins].OpenHal(szAinDevNames[nAins]);
    if (nRetVal < 0)
    {
      printf("Error %d opening Analog IN Channel: %d\n", nRetVal, nAins + 1);
      //OpenHAL fails since for ROCIO AIs we initialize AIs in OpenHAL.
      return;
    }
  }

  while (g_nExitApp != 1)
  {
    if (!g_b370XAIOBoards)
    {
      printf("Enter Analog IN channel number (%d to %d): \n", nStartAICh, nEndAICh);
      fflush(stdin);
      scanf("%d", &unCh);

      if ((unCh < (unsigned int)nStartAICh) || (unCh > (unsigned int)nEndAICh))
      {
        printf("Invalid value... try again.\n");
        continue;
      }
      //unCh--;
    }
    else
      unCh = 0;

    if (g_nExitApp)
      break;

    printf("Select Analog IN function:\n");
    printf("  %d. Get Analog In Current.\n", AIN_FUNC_GET_CURRENT);
    printf("  %d. Get Analog In Voltage.\n", AIN_FUNC_GET_VOLTAGE);
    printf("  %d. Get board temperature.\n", AIN_FUNC_GET_BRD_TEMP);
    printf("  %d. Get board information.\n", AIN_BOARD_INFO);
    printf("  %d. Get Analog In Current continuously.\n", AIN_FUNC_GET_CURRENT_CONT);
    printf("  %d. Get Analog In Voltage continuously.\n", AIN_FUNC_GET_VOLTAGE_CONT);
    printf("  %d. Exit application.\n", AIN_FUNC_EXIT);
    fflush(stdin);
    scanf("%d", &nAinFunc);

    if ((nAinFunc == AIN_FUNC_EXIT) || g_nExitApp)
      break;

    if ((nAinFunc != AIN_FUNC_GET_CURRENT) && (nAinFunc != AIN_BOARD_INFO)
    &&  (nAinFunc != AIN_FUNC_GET_BRD_TEMP) && (nAinFunc != AIN_FUNC_GET_VOLTAGE)
    &&  (nAinFunc != AIN_FUNC_GET_CURRENT_CONT) && (nAinFunc != AIN_FUNC_GET_VOLTAGE_CONT))
    {
      printf("Enter value: ");
      fflush(stdin);
      scanf("%d", &unValue);
    }

    printf("\n");

    if (g_nExitApp)
      break;

    switch (nAinFunc)
    {
    case AIN_FUNC_GET_CURRENT:
      if ((nRetVal = obAin[unCh].GetCurrent(&ulValue)) < 0)
        printf("CAnalogIn[%d].GetCurrent() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Current (na): %lu\n", ulValue);
      break;

    case AIN_FUNC_GET_VOLTAGE:
      if ((nRetVal = obAin[unCh].GetVoltage(&ulValue)) < 0)
        printf("CAnalogIn[%d].GetVoltage() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Voltage (mV): %lu\n", ulValue);
      break;


    case AIN_FUNC_GET_BRD_TEMP:
      if ((nRetVal = obAin[unCh].GetOnBoardTemp(&nValue)) < 0)
        printf("CAnalogIn[%d].GetChDiffGain() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Board Temperature: %f\n", (float)nValue/1000);
      break;

    case AIN_BOARD_INFO:
    {
      unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
      if ((nRetVal = obAin[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
      {
        printf("CAnalogIn[%d].GetBoardInfo() failed: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
        printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
        printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
        printf("Board Revision : %d\n", byBoardRev);
      }
    }
    break;

    case AIN_FUNC_GET_CURRENT_CONT:
      while (g_nExitApp != 1)
      {
        for (int nInd = nStartAICh; nInd <=nEndAICh; nInd++)
        {
          if ((nRetVal = obAin[nInd].GetCurrent(&ulValue)) < 0)
            printf("CAnalogIn[%d].GetCurrent() failed.: %d, ", nInd + 1, nRetVal);
          else
            printf("%lu, ", ulValue);
        }
        printf("\n");

        //Use the user specified time interval b/n Analog Input reads....
        if (nIndexFlag)
          usleep(nIndex * 1000);
        else // else use a default interval of 1 second
          usleep(1000000);
      }
      g_nExitApp = 0;
      break;

    case AIN_FUNC_GET_VOLTAGE_CONT:
      while (g_nExitApp != 1)
      {
        for (int nInd = nStartAICh; nInd <=nEndAICh; nInd++)
        {
          if ((nRetVal = obAin[nInd].GetVoltage(&ulValue)) < 0)
            printf("CAnalogIn[%d].GetVoltage() failed.: %d, ", nInd + 1, nRetVal);
          else
            printf("%lu, ", ulValue);
        }
        printf("\n");

        //Use the user specified time interval b/n Analog Input reads....
        if (nIndexFlag)
          usleep(nIndex * 1000);
        else  // else use a default interval of 1 second
          usleep(1000000);
      }
      g_nExitApp = 0;
      break;

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
  }

  for (int nAins = nStartAICh; nAins <= nEndAICh; nAins++)
  {
    nRetVal = obAin[nAins].CloseHal();
    if (nRetVal < 0)
      printf("CAnalogIn[%d].Close() failed: %d\n", nAins + 1, nRetVal);
  }
}

#define NR_AOT_CHANNELS        22

#define AOT_FUNC_SET_CURRENT   1
#define AOT_FUNC_GET_BRD_TEMP  2
#define AOT_BOARD_INFO         3
#define AOT_FUNC_EXIT          4

void TestAnalogOut()
{
  static char *szAotDevNames[NR_AOT_CHANNELS] = {"NULL","NULL","NULL","NULL","NULL","NULL","NULL",
                                                 "NULL","NULL","NULL","NULL","NULL","NULL","NULL",
                                                 "NULL","NULL","NULL","NULL","NULL","NULL","NULL","NULL"};

  if (g_b370XAIOBoards)
  {
    szAotDevNames[0] = "ANA_OUT:370XA_LOI_SLOT:ANA_OUT_1";
    szAotDevNames[1] = "ANA_OUT:370XA_LOI_SLOT:ANA_OUT_2";
  }
  else
  {
    szAotDevNames[0] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_1";
    szAotDevNames[1] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_2";
    szAotDevNames[2] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_3";
    szAotDevNames[3] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_4";
    szAotDevNames[4] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_5";
    szAotDevNames[5] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_6";
    szAotDevNames[6] = "ANA_OUT:EXP_SLOT_1:ANA_OUT_1";
    szAotDevNames[7] = "ANA_OUT:EXP_SLOT_1:ANA_OUT_2";
    szAotDevNames[8] = "ANA_OUT:EXP_SLOT_1:ANA_OUT_3";
    szAotDevNames[9] = "ANA_OUT:EXP_SLOT_1:ANA_OUT_4";
    szAotDevNames[10] = "ANA_OUT:EXP_SLOT_2:ANA_OUT_1";
    szAotDevNames[11] = "ANA_OUT:EXP_SLOT_2:ANA_OUT_2";
    szAotDevNames[12] = "ANA_OUT:EXP_SLOT_2:ANA_OUT_3";
    szAotDevNames[13] = "ANA_OUT:EXP_SLOT_2:ANA_OUT_4";
    szAotDevNames[14] = "ANA_OUT:EXP_SLOT_3:ANA_OUT_1";
    szAotDevNames[15] = "ANA_OUT:EXP_SLOT_3:ANA_OUT_2";
    szAotDevNames[16] = "ANA_OUT:EXP_SLOT_3:ANA_OUT_3";
    szAotDevNames[17] = "ANA_OUT:EXP_SLOT_3:ANA_OUT_4";
    szAotDevNames[18] = "ANA_OUT:EXP_SLOT_4:ANA_OUT_1";
    szAotDevNames[19] = "ANA_OUT:EXP_SLOT_4:ANA_OUT_2";
    szAotDevNames[20] = "ANA_OUT:EXP_SLOT_4:ANA_OUT_3";
    szAotDevNames[21] = "ANA_OUT:EXP_SLOT_4:ANA_OUT_4";
  };

  static CAnalogOut obAot[NR_AOT_CHANNELS];
  
  int nRetVal = 0;
  int nAotFunc = 0;

  unsigned int unCh = 0;
  unsigned long ulValue = 0;
  int nValue;

  int nStartAOCh = 0;
  int nEndAOCh = 0;
  if (g_b370XAIOBoards)
  {
    nStartAOCh = 0;
    nEndAOCh = 1;
  }
  else
  {
    int nAOSlot = 0;
    printf("Enter AO slot -\n 1. BaseIO\n 2. rocio1\n 3. rocio2\n 4. rocio3\n 5. rocio4\n - ");
    fflush(stdin);
    scanf("%d", &nAOSlot);
    switch (nAOSlot)
    {
    case 1:
      nStartAOCh = 0;
      nEndAOCh = 5;
      break;
    case 2:
      nStartAOCh = 6;
      nEndAOCh = 9;
      break;
    case 3:
      nStartAOCh = 10;
      nEndAOCh = 13;
      break;
    case 4:
      nStartAOCh = 14;
      nEndAOCh = 17;
      break;
    case 5:
      nStartAOCh = 18;
      nEndAOCh = 21;
      break;
    default:
      printf("Entered AO slot is invalid!!!");
      return;
    }
  }

  for (int nAots = nStartAOCh; nAots <= nEndAOCh; nAots++)
  {
    nRetVal = obAot[nAots].OpenHal(szAotDevNames[nAots]);
    if (nRetVal < 0)
    {
      printf("Error %d opening Analog OUT Channel: %d\n", nRetVal, nAots + 1);
      return;
    }
  }
  
  while (g_nExitApp != 1)
  {
    printf("Enter Analog OUT channel number (%d to %d): \n", nStartAOCh, nEndAOCh);
    fflush(stdin);
    scanf("%d", &unCh);
    if ((unCh < (unsigned int)nStartAOCh) || (unCh > (unsigned int)nEndAOCh))
    {
      printf("Invalid value... try again.\n");
      continue;
    }

    if (g_nExitApp)
      break;

    printf("Select Analog OUT function:\n");
    printf("  %d. Set Analog OUT current.\n", AOT_FUNC_SET_CURRENT);
    if (!g_b370XAIOBoards)
    {
      printf("  %d. Get board temperature.\n", AOT_FUNC_GET_BRD_TEMP);
      printf("  %d. Get board information.\n", AOT_BOARD_INFO);
    }
    printf("  %d. Exit application.\n", AOT_FUNC_EXIT);
    fflush(stdin);
    scanf("%d", &nAotFunc);

    if ((nAotFunc == AOT_FUNC_EXIT) || g_nExitApp)
      break;

    if ((nAotFunc != AOT_BOARD_INFO) && (nAotFunc != AOT_FUNC_GET_BRD_TEMP))
    {
      printf("Enter value: ");
      fflush(stdin);
      scanf("%lu", &ulValue);
    }

    printf("\n");
    
    if (g_nExitApp)
      break;

    switch (nAotFunc)
    {
    case AOT_FUNC_SET_CURRENT:
      if ((nRetVal = obAot[unCh].SetCurrent(ulValue)) < 0)
        printf("CAnalogOut[%d].SetCurrent() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case AOT_FUNC_GET_BRD_TEMP:
      if (!g_b370XAIOBoards)
      {
        if ((nRetVal = obAot[unCh].GetOnBoardTemp(&nValue)) < 0)
          printf("CAnalogOut[%d].GetOnBoardTemp() failed.: %d\n", unCh + 1, nRetVal);
        else
          printf("Board Temperature: %f\n", (float)nValue/1000);
        break;
      }

    case AOT_BOARD_INFO:
      if (!g_b370XAIOBoards)
      {
        unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
        if ((nRetVal = obAot[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
        {
          printf("CAnalogOut[%d].GetBoardInfo() failed: %d\n", unCh + 1, nRetVal);
        }
        else
        {
          printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
          printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
          printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
          printf("Board Revision : %d\n", byBoardRev);
        }
        break;
      }

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
  }
  
  for (int nAots = nStartAOCh; nAots <= nEndAOCh; nAots++)
  {
    nRetVal = obAot[nAots].CloseHal();
    if (nRetVal < 0)
      printf("CAnalogOut[%d].Close() failed: %d\n", nAots + 1, nRetVal);
  }
}


#define NR_HTR_CHANNELS              4
#define ANALYZER_NR_HTR_CHANNELS     2

enum
{
  HTR_FUNC_SET_TEMP = 1,
  HTR_FUNC_SET_PWM,
  HTR_FUNC_GET_TEMP,
  HTR_FUNC_GET_TEMP_CONT,
  HTR_FUNC_GET_PWM,
  HTR_FUNC_OFF,
  HTR_FUNC_SET_PROP,
  HTR_FUNC_SET_INT,
  HTR_FUNC_SET_DIFF,
  HTR_FUNC_SET_HTR_TYPE,
  HTR_FUNC_SET_RTD_LEAD_R,
  HTR_FUNC_SET_COMP_BASE_TEMP,
  HTR_FUNC_MARK_COMP_BASE_TEMP,
  HTR_FUNC_SET_COMP_TEMP_SLOPE,
  HTR_FUNC_GET_PROP,
  HTR_FUNC_GET_INT,
  HTR_FUNC_GET_DIFF,
  HTR_FUNC_GET_BRD_TEMP,
  HTR_FUNC_GET_HTR_TYPE,
  HTR_FUNC_GET_COMP_BASE_TEMP,
  HTR_FUNC_GET_COMP_TEMP_SLOPE,
  HTR_BOARD_INFO,
  HTR_FUNC_EXIT
};

void TestHeaterCtrl()
{
  int nRetVal = 0;

  static char *szHtrDevNames[NR_HTR_CHANNELS] = {NULL, NULL, NULL, NULL};
  int nNumHtrChannels = NR_HTR_CHANNELS;
  if (g_b370XAIOBoards)
  {
    nNumHtrChannels = ANALYZER_NR_HTR_CHANNELS;
    szHtrDevNames[0] = "HTR_CTRL:ANALYZER_SLOT:HTR_CTRL_1";
    szHtrDevNames[1] = "HTR_CTRL:ANALYZER_SLOT:HTR_CTRL_2";
  }
  else
  {
    szHtrDevNames[0] = "HTR_CTRL:SLOT_2:HTR_CTRL_1";
    szHtrDevNames[1] = "HTR_CTRL:SLOT_2:HTR_CTRL_2";
    szHtrDevNames[2] = "HTR_CTRL:SLOT_2:HTR_CTRL_3";
    szHtrDevNames[3] = "HTR_CTRL:SLOT_2:HTR_CTRL_4";
  };
  
  static CHeaterCtrl obHtr[NR_HTR_CHANNELS];
  
  int nHtrFunc = 0;
  unsigned unCh = 0;
  unsigned unValue = 0;
  unsigned long ulValue = 0;
  float fValue = 0;
  int nValue;

  for (int nHtrs = 0; nHtrs < nNumHtrChannels; nHtrs++)
  {
    nRetVal = obHtr[nHtrs].OpenHal(szHtrDevNames[nHtrs]);
    if (nRetVal < 0)
    {
      DEBUG("Error %d opening Htr Channel: %d\n", nRetVal, nHtrs);
      return;
    }
  }
  
  while (g_nExitApp != 1)
  {
    printf("Enter heater channel number (1 to %d): \n", nNumHtrChannels);
    fflush(stdin);
    scanf("%d", &unCh);

    if ((unCh < 1) || (unCh > (unsigned)nNumHtrChannels))
    {
      printf("Invalid value... try again.\n");
      continue;
    }
    unCh--;

    if (g_nExitApp)
      break;

    printf("Select heater function:\n");
    printf("  %d. Set channel temperature (deg C).\n", HTR_FUNC_SET_TEMP);
    printf("  %d. Set channel PWM (percent).\n", HTR_FUNC_SET_PWM);
    printf("  %d. Get channel temperature (deg C).\n", HTR_FUNC_GET_TEMP);
    printf("  %d. Get channel temperature (deg C) continuously.\n", HTR_FUNC_GET_TEMP_CONT);
    printf("  %d. Get channel PWM (percent).\n", HTR_FUNC_GET_PWM);
    printf("  %d. Turn heater off.\n", HTR_FUNC_OFF);
    printf("  %d. Set proportional gain.\n", HTR_FUNC_SET_PROP);
    printf("  %d. Set integral gain.\n", HTR_FUNC_SET_INT);
    printf("  %d. Set differential gain.\n", HTR_FUNC_SET_DIFF);
    printf("  %d. Set Heater type.\n", HTR_FUNC_SET_HTR_TYPE);
    printf("  %d. Set RTD lead resistor value.\n", HTR_FUNC_SET_RTD_LEAD_R);
    printf("  %d. Set Compensation Base Temperature.\n", HTR_FUNC_SET_COMP_BASE_TEMP);
    printf("  %d. Mark Compensation Base Temperature.\n", HTR_FUNC_MARK_COMP_BASE_TEMP);
    printf("  %d. Set Compensation Temperature Slope.\n", HTR_FUNC_SET_COMP_TEMP_SLOPE);
    printf("  %d. Get proportional gain.\n", HTR_FUNC_GET_PROP);
    printf("  %d. Get integral gain.\n", HTR_FUNC_GET_INT);
    printf("  %d. Get differential gain.\n", HTR_FUNC_GET_DIFF);
    printf("  %d. Get board temperature.\n", HTR_FUNC_GET_BRD_TEMP);
    printf("  %d. Get Heater type.\n", HTR_FUNC_GET_HTR_TYPE);
    printf("  %d. Get Compensation Base Temperature.\n", HTR_FUNC_GET_COMP_BASE_TEMP);
    printf("  %d. Get Compensation Temperature Slope.\n", HTR_FUNC_GET_COMP_TEMP_SLOPE);
    printf("  %d. Get board information.\n", HTR_BOARD_INFO);
    printf("  %d. Exit application.\n", HTR_FUNC_EXIT);

    fflush(stdin);
    scanf("%d", &nHtrFunc);

    if ((nHtrFunc == HTR_FUNC_EXIT) || g_nExitApp)
      break;

    switch (nHtrFunc)
    {
    case HTR_FUNC_SET_TEMP: 
    case HTR_FUNC_SET_PWM: 
    case HTR_FUNC_SET_PROP: 
    case HTR_FUNC_SET_INT:
    case HTR_FUNC_SET_DIFF: 
    case HTR_FUNC_SET_COMP_BASE_TEMP:
    case HTR_FUNC_SET_COMP_TEMP_SLOPE:
    case HTR_FUNC_SET_RTD_LEAD_R:
      printf("Enter value: ");
      fflush(stdin);
      if ((nHtrFunc != HTR_FUNC_SET_PROP)
      &&  (nHtrFunc != HTR_FUNC_SET_INT)
      &&  (nHtrFunc != HTR_FUNC_SET_RTD_LEAD_R)
      &&  (nHtrFunc != HTR_FUNC_SET_COMP_TEMP_SLOPE))
        scanf("%d", &unValue);
      else
      {
        scanf("%f", &fValue);
        if (nHtrFunc != HTR_FUNC_SET_RTD_LEAD_R)
          unValue = (unsigned)(fValue * 1000);
        printf("fValue: %f\n", fValue);
        if (nHtrFunc != HTR_FUNC_SET_RTD_LEAD_R)
          printf("unValue: %u\n", unValue);
      }
      break;
    }

    printf("\n");
    
    if (g_nExitApp)
      break;

    switch (nHtrFunc)
    {
    case HTR_FUNC_SET_TEMP:
      if ((nRetVal = obHtr[unCh].SetChTempMilliDegC(unValue * 1000)) < 0)
        printf("CHeaterCtrl[%d].SetChTempMilliDegC() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_SET_PWM:
      if ((nRetVal = obHtr[unCh].SetChPWMMilliP(unValue * 1000)) < 0)
        printf("CHeaterCtrl[%d].SetChPWMMilliP() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_GET_TEMP:
      if ((nRetVal = obHtr[unCh].GetHtrChTempMilliDegC(&nValue)) < 0)
        printf("CHeaterCtrl[%d].GetHtrChTempMilliDegC() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater (%d) temperature is %f DegC.\n", unCh + 1, nValue/1000.0f);
      break;

    case HTR_FUNC_GET_TEMP_CONT:
      do
      {
        if ((nRetVal = obHtr[unCh].GetHtrChTempMilliDegC(&nValue)) < 0)
          printf("CHeaterCtrl[%d].GetHtrChTempMilliDegC() failed.: %d\n", unCh + 1, nRetVal);
        else
          printf("%d, %f\n", unCh + 1, nValue/1000.0f);
        usleep(1000000);
      } while (g_nExitApp != 1);
      g_nExitApp = 0;
      break;
      
    case HTR_FUNC_GET_PWM:
      if ((nRetVal = obHtr[unCh].GetHtrChPWMMilliP(&unValue)) < 0)
        printf("CHeaterCtrl[%d].GetHtrChPWMMilliP() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater %d PWM percent: %d\n", unCh + 1, unValue/1000);
      break;

    case HTR_FUNC_OFF:
      if ((nRetVal = obHtr[unCh].TurnHtrOff()) < 0)
        printf("CHeaterCtrl[%d].TurnHtrOff() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_SET_PROP:
      if ((nRetVal = obHtr[unCh].SetChPropGain(unValue)) < 0)
        printf("CHeaterCtrl[%d].GetChPropGain() failed.: %d\n", unCh + 1, nRetVal);
      break;
    
    case HTR_FUNC_SET_INT:
      if ((nRetVal = obHtr[unCh].SetChIntGain(unValue)) < 0)
        printf("CHeaterCtrl[%d].GetChIntpGain() failed.: %d\n", unCh + 1, nRetVal);
      break;
    
    case HTR_FUNC_SET_DIFF:
      if ((nRetVal = obHtr[unCh].SetChDiffGain(unValue)) < 0)
        printf("CHeaterCtrl[%d].GetChDiffGain() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_SET_HTR_TYPE:
      printf("Enter one of the following values : \n 0 - AC Heater \n 1 - DC Heater \n -");
      fflush(stdin);
      scanf("%d", &unValue);
      if (unValue < 0 || unValue > 1)
        printf( "Invalid value !!!\n" );
      else
      {
        if((nRetVal = obHtr[unCh].SetHeaterType((CHeaterCtrl::HEATER_TYPE)unValue)) < 0)
          printf("CHeaterCtrl[%d].SetHeaterType() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;

    case HTR_FUNC_SET_RTD_LEAD_R:
      if ((nRetVal = obHtr[unCh].SetRTDleadR((unsigned char)(unCh % 4), fValue)) < 0)
        printf("CHeaterCtrl[%d].SetRTDleadR() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_SET_COMP_BASE_TEMP:
      if ((nRetVal = obHtr[unCh].SetCompBaseTemp((int)unValue)) < 0)
        printf("CHeaterCtrl[%d].SetCompBaseTemp() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_MARK_COMP_BASE_TEMP:
      if ((nRetVal = obHtr[unCh].MarkCompBaseTemp()) < 0)
        printf("CHeaterCtrl[%d].MarkCompBaseTemp() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_SET_COMP_TEMP_SLOPE:
      if ((nRetVal = obHtr[unCh].SetCompTempSlope(fValue)) < 0)
        printf("CHeaterCtrl[%d].SetCompTempSlope() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case HTR_FUNC_GET_PROP:
      if ((nRetVal = obHtr[unCh].GetChPropGain(&ulValue)) < 0)
        printf("CHeaterCtrl[%d].GetChPropGain() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater %d Prop Gain: %ld\n", unCh + 1, ulValue);
      break;

    case HTR_FUNC_GET_INT:
      if ((nRetVal = obHtr[unCh].GetChIntGain(&ulValue)) < 0)
        printf("CHeaterCtrl[%d].GetChIntGain() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater %d Int Gain: %ld\n", unCh + 1, ulValue);
      break;

    case HTR_FUNC_GET_DIFF:
      if ((nRetVal = obHtr[unCh].GetChDiffGain(&ulValue)) < 0)
        printf("CHeaterCtrl[%d].GetChDiffGain() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater %d Diff Gain: %ld\n", unCh + 1, ulValue);
      break;

    case HTR_FUNC_GET_BRD_TEMP:
      if ((nRetVal = obHtr[unCh].GetOnBoardTemp(&nValue)) < 0)
        printf("CHeaterCtrl[%d].GetChDiffGain() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater Board Temperature: %f\n", (float)nValue/1000);
      break;

    case HTR_FUNC_GET_HTR_TYPE:
    {
      CHeaterCtrl::HEATER_TYPE eHtrType;
      if ((nRetVal = obHtr[unCh].GetHeaterType(&eHtrType)) < 0)
        printf("CHeaterCtrl[%d].GetHeaterType() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("%d Heater Type - %s\n", unCh + 1, (eHtrType == CHeaterCtrl::AC_HEATER) ? "AC" : "DC");
    }
    break;

    case HTR_FUNC_GET_COMP_BASE_TEMP:
      if ((nRetVal = obHtr[unCh].GetCompBaseTemp(&nValue)) < 0)
        printf("CHeaterCtrl[%d].GetCompBaseTemp() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater Board Compensation Base Temperature: %f\n", (float)nValue/1000);
      break;

    case HTR_FUNC_GET_COMP_TEMP_SLOPE:
      if ((nRetVal = obHtr[unCh].GetCompTempSlope(&fValue)) < 0)
        printf("CHeaterCtrl[%d].GetCompTempSlope() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Heater Board Compensation Temperature Slope: %f\n", fValue);
      break;

    case HTR_BOARD_INFO:
    {
      unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
      if ((nRetVal = obHtr[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
        printf("CHeaterCtrl[%d].GetBoardInfo() failed: %d\n", unCh + 1, nRetVal);
      else
      {
        printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
        printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
        printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
        printf("Board Revision : %d\n", byBoardRev);
      }
    }
    break;

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
  }
  
  for (int nHtrs = 0; nHtrs < nNumHtrChannels; nHtrs++)
  {
    nRetVal = obHtr[nHtrs].CloseHal();
    if (nRetVal < 0)
      printf("CHeaterCtrl[%d].Close() failed: %d\n", nHtrs + 1, nRetVal);
  }
}

#define NR_SOL_CHANNELS          12
#define ANALYZER_NR_SOL_CHANNELS 8
#define SOL_CONF_SOL_ON          1
#define SOL_CONF_SOL_OFF         2
#define SOL_GET_CURRENT          3
#define SOL_CONF_EXIT_CONF       4
#define SOL_CONF_EXIT_APP        5

void TestSol()
{
  int nRetVal = 0;

  static char * szSolDevNames[NR_SOL_CHANNELS] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; 
  int nNumSolChannels = NR_SOL_CHANNELS;
  if( g_b370XAIOBoards )
  {
    nNumSolChannels = ANALYZER_NR_SOL_CHANNELS;
    szSolDevNames[0] = "SOL:ANALYZER_SLOT:SOL_1";
    szSolDevNames[1] = "SOL:ANALYZER_SLOT:SOL_2";
    szSolDevNames[2] = "SOL:ANALYZER_SLOT:SOL_3";
    szSolDevNames[3] = "SOL:ANALYZER_SLOT:SOL_4";
    szSolDevNames[4] = "SOL:ANALYZER_SLOT:SOL_5";
    szSolDevNames[5] = "SOL:ANALYZER_SLOT:SOL_6";
    szSolDevNames[6] = "SOL:ANALYZER_SLOT:SOL_7";
    szSolDevNames[7] = "SOL:ANALYZER_SLOT:SOL_8";
    
  }
  else
  {
    szSolDevNames[0] = "SOL:SLOT_2:SOL_1";
    szSolDevNames[1] = "SOL:SLOT_2:SOL_2";
    szSolDevNames[2] = "SOL:SLOT_2:SOL_3";
    szSolDevNames[3] = "SOL:SLOT_2:SOL_4";
    szSolDevNames[4] = "SOL:SLOT_2:SOL_5";
    szSolDevNames[5] = "SOL:SLOT_2:SOL_6";
    szSolDevNames[6] = "SOL:SLOT_2:SOL_7";
    szSolDevNames[7] = "SOL:SLOT_2:SOL_8";
    szSolDevNames[8] = "SOL:SLOT_2:SOL_9";
    szSolDevNames[9] = "SOL:SLOT_2:SOL_10";
    szSolDevNames[10] = "SOL:SLOT_2:SOL_11";
    szSolDevNames[11] = "SOL:SLOT_2:SOL_12";
  };

  static CSolenoidCtrl obSol[NR_SOL_CHANNELS];

  struct timeval time1, time2, timeD;
  typedef struct
  {
    unsigned long int MinTimeuSec;
    unsigned long int MaxTimeuSec;
    unsigned long int SumTimemSec;
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
  stStats.SumTimemSec = 0;
  
  if (g_nConfigure)
  {
    int nValue = 0;
    int nSolOper = 0;
    int nExitConfLoop = 0;
    int nSolCh = 0;
    printf("****** CONF: SOL ******\n");
    while (!nExitConfLoop)
    {

      if (g_nExitApp)
      {
        goto sol_end;
      }
      
      printf("Enter Sol channel: (1 to %d):\n", nNumSolChannels);
      fflush(stdin);
      scanf("%d", &nSolCh);

      if ( (nSolCh < 1) || (nSolCh > nNumSolChannels) )
      {
        printf("Invalid input. Try again.\n");
        continue;
      }

      nSolCh--;

      printf("Select sol operation: \n");
      printf("  %d. Switch ON Sol.\n", SOL_CONF_SOL_ON);
      printf("  %d. Switch OFF Sol.\n", SOL_CONF_SOL_OFF);
      printf("  %d. Get total Solenoid current.\n", SOL_GET_CURRENT);
      printf("  %d. Exit conf. (Enter test loop)\n", SOL_CONF_EXIT_CONF);
      printf("  %d. Exit app.\n", SOL_CONF_EXIT_APP);
      fflush(stdin);
      scanf("%d", &nSolOper);

      switch(nSolOper)
      {
      case SOL_CONF_SOL_ON:
        nRetVal = obSol[nSolCh].OpenHal(szSolDevNames[nSolCh]);

        if (nRetVal < 0)
        {
          printf("Error %d opening Sol Channel: %d\n", nRetVal, nSolCh + 1);
          g_nExitApp = 1;
          goto sol_end;
        }

        nRetVal = obSol[nSolCh].TurnONSolValve();
      
        if (nRetVal < 0)
        {
          printf("CSolenoidCtrl[%d].TurnONSolValve() failed: %d\n", nSolCh + 1, nRetVal);
          g_nExitApp = 1;
          goto sol_end;
        }
      
        nRetVal = obSol[nSolCh].CloseHal();

        if (nRetVal < 0)
        {
          printf("Error %d closing Sol Channel: %d\n", nRetVal, nSolCh + 1);
          g_nExitApp = 1;
          goto sol_end;
        }

        break;
      
      case SOL_CONF_SOL_OFF:
        nRetVal = obSol[nSolCh].OpenHal(szSolDevNames[nSolCh]);

        if (nRetVal < 0)
        {
          printf("Error %d opening Sol Channel: %d\n", nRetVal, nSolCh + 1);
          g_nExitApp = 1;
          goto sol_end;
        }

        nRetVal = obSol[nSolCh].TurnOFFSolValve();
      
        if (nRetVal < 0)
        {
          printf("CSolenoidCtrl[%d].TurnOFFSolValve() failed: %d\n", nSolCh + 1, nRetVal);
          g_nExitApp = 1;
          goto sol_end;
        }
      
        nRetVal = obSol[nSolCh].CloseHal();

        if (nRetVal < 0)
        {
          printf("Error %d closing Sol Channel: %d\n", nRetVal, nSolCh + 1);
          g_nExitApp = 1;
          goto sol_end;
        }

        break;

      case SOL_GET_CURRENT:
        nRetVal = obSol[nSolCh].OpenHal(szSolDevNames[nSolCh]);

        if (nRetVal < 0)
        {
          printf("Error %d opening Sol Channel: %d\n", nRetVal, nSolCh + 1);
          g_nExitApp = 1;
          goto sol_end;
        }

        nRetVal = obSol[nSolCh].GetSolTotalCurrent(&nValue);

      
        if (nRetVal < 0)
        {
          printf("CSolenoidCtrl[%d].TurnOFFSolValve() failed: %d\n", nSolCh + 1, nRetVal);
          g_nExitApp = 1;
          goto sol_end;
        }
        else
        {
          printf("Total Solenoid current: %d\n", nValue);
        }
      
        nRetVal = obSol[nSolCh].CloseHal();

        if (nRetVal < 0)
        {
          printf("Error %d closing Sol Channel: %d\n", nRetVal, nSolCh + 1);
          g_nExitApp = 1;
          goto sol_end;
        }

        break;
      
      case SOL_CONF_EXIT_CONF:
        nExitConfLoop = 1;
        break;
      
      case SOL_CONF_EXIT_APP:
        nExitConfLoop = 1;
        g_nExitApp = 1;
        goto sol_end;
        break;

      default:
        printf("Invalid input. Try again.\n");
        break;
      }
    }
  }
  

  while(g_nExitApp != 1)
  {
    for (int nSols = 0; nSols < nNumSolChannels; nSols++)
    {

//Open the device
      nRetVal = obSol[nSols].OpenHal(szSolDevNames[nSols]);

      if (nRetVal < 0)
      {
        printf("Error %d opening Sol Channel: %d\n", nRetVal, nSols + 1);
        g_nExitApp = 1;
        break;
      }

//Turn ON the solenoid
      if (gettimeofday(&time1, NULL) < 0)
      {
        printf("Error getting time before TurnONSolValve()\n");
      }
      
      nRetVal = obSol[nSols].TurnONSolValve();
      
      if (gettimeofday(&time2, NULL) < 0)
      {
        printf("Error getting time after TurnONSolValve()\n");
      }
      
      if (nRetVal < 0)
      {
        printf("CSolenoidCtrl[%d].TurnONSolValve() failed: %d\n", nSols + 1, nRetVal);
        stStats.NrTimeouts++;
        g_nExitApp = 1;
        goto sol_exit;
      }



      if (time2.tv_usec > time1.tv_usec)
      {
        timeD.tv_usec = time2.tv_usec - time1.tv_usec;
        timeD.tv_sec = time2.tv_sec - time1.tv_sec;
      }
      else
      {
        timeD.tv_usec = 1000000 + (time2.tv_usec - time1.tv_usec);
        timeD.tv_sec = time2.tv_sec - time1.tv_sec;
        timeD.tv_sec--;
      }

      
      tempTime = (timeD.tv_sec * 1000000) + timeD.tv_usec;
      stStats.SumTimemSec += tempTime/1000;

      DEBUG("1: Sec: %ld, uSec: %ld\n", time1.tv_sec, time1.tv_usec);
      DEBUG("2: Sec: %ld, uSec: %ld\n", time2.tv_sec, time2.tv_usec);
      DEBUG("D: Sec: %ld, uSec: %ld\n", timeD.tv_sec, timeD.tv_usec);

      stStats.MinTimeuSec = MIN(stStats.MinTimeuSec, tempTime);
      stStats.MaxTimeuSec = MAX(stStats.MaxTimeuSec, tempTime);
      stStats.NrRuns++;
      if (tempTime >= 20000)
      {
        stStats.Above20mSec++;
      }
      
      if (nIndexFlag)
      {
        usleep(nIndex * 1000);
      }
      else
      {
        usleep(50000);
      }

//Turn OFF the solenoid
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
        printf("CSolenoidCtrl[%d].TurnOFFSolValve() failed: %d\n", nSols + 1, nRetVal);
        stStats.NrTimeouts++;
        g_nExitApp = 1;
        goto sol_exit;
      }

      if (time2.tv_usec > time1.tv_usec)
      {
        timeD.tv_usec = time2.tv_usec - time1.tv_usec;
        timeD.tv_sec = time2.tv_sec - time1.tv_sec;
      }
      else
      {
        timeD.tv_usec = 1000000 + (time2.tv_usec - time1.tv_usec);
        timeD.tv_sec = time2.tv_sec - time1.tv_sec;
        timeD.tv_sec--;
      }

      
      tempTime = (timeD.tv_sec * 1000000) + timeD.tv_usec;
      stStats.SumTimemSec += tempTime/1000;

      DEBUG("1: Sec: %ld, uSec: %ld\n", time1.tv_sec, time1.tv_usec);
      DEBUG("2: Sec: %ld, uSec: %ld\n", time2.tv_sec, time2.tv_usec);
      DEBUG("D: Sec: %ld, uSec: %ld\n", timeD.tv_sec, timeD.tv_usec);

      stStats.MinTimeuSec = MIN(stStats.MinTimeuSec, tempTime);
      stStats.MaxTimeuSec = MAX(stStats.MaxTimeuSec, tempTime);
      stStats.NrRuns++;
      if (tempTime >= 20000)
      {
        stStats.Above20mSec++;
      }

    sol_exit:
//Close the device
      nRetVal = obSol[nSols].CloseHal();

      if (nRetVal < 0)
      {
        printf("Error %d closing Sol Channel: %d\n", nRetVal, nSols + 1);
        g_nExitApp = 1;
        break;
      }

      //This is if TurnONSolValve() or TurnOFFSolValve() fail
      if (g_nExitApp)
      {
        break;
      }

      if (nIndexFlag)
      {
        usleep(nIndex * 1000);
      }
      else
      {
        usleep(50000);
      }
    }
  }

  printf(" ****** Sol Stats ***** \n");
  printf("MAX TA Time (us): %ld\n", stStats.MaxTimeuSec);
  printf("MIN TA Time (us): %ld\n", stStats.MinTimeuSec);
  printf("AVG TA Time (ms): %ld\n", stStats.SumTimemSec/stStats.NrRuns);
  printf("Number of runs: %ld\n", stStats.NrRuns);
  printf("Above 20 ms TA Time: %ld\n", stStats.Above20mSec);
  printf("Number of timeouts: %ld\n", stStats.NrTimeouts);

sol_end:
  return;
}

DB_INT32 registerTable(DBCLIENT* dbcOpenPtr, DB_INT32 dbPoint)
{
  DBAPI_PT_LIST_STRUCT ptList;
  ptList.dbPoint     = (DB_PT_ENUM)dbPoint;
  ptList.reqDataType = TYPE_NATIVE;
  ptList.units       = UNIT_NATIVE;
  ptList.nextPtr     = (DBAPI_PT_LIST_STRUCT *)NULL;
  if(-1==DbcRegister(dbcOpenPtr,(UINT8)0,(DBAPI_PT_LIST_STRUCT *)&ptList))
  {
    printf("DbcRegister Failed");
    exit(EXIT_FAILURE);
  }
  return 0;
}


#ifdef MODEL_700XA
#define NR_RTD_CHANNELS     5
#else
#define NR_RTD_CHANNELS     2
#endif
#define ANALYZER_NR_RTD_CHANNELS  2

void TestRTD(int nIndex)
{
#ifdef MODEL_700XA
  static char *szRTDDevNames[NR_RTD_CHANNELS] = { NULL, NULL, NULL, NULL, NULL};
  if (nIndex == 0)
  {
    szRTDDevNames[0] = "RTD:SLOT_2:RTD_1";
    szRTDDevNames[1] = "RTD:SLOT_2:RTD_2";
    szRTDDevNames[2] = "RTD:SLOT_2:RTD_3";
    szRTDDevNames[3] = "RTD:SLOT_2:RTD_4";
    szRTDDevNames[4] = "RTD:SLOT_2:RTD_5";
  }
  else
  {
    szRTDDevNames[0] = "RTD:SLOT_4:RTD_1";
    szRTDDevNames[1] = "RTD:SLOT_4:RTD_2";
    szRTDDevNames[2] = "RTD:SLOT_4:RTD_3";
    szRTDDevNames[3] = "RTD:SLOT_4:RTD_4";
    szRTDDevNames[4] = "RTD:SLOT_4:RTD_5";
  }
#else
  static char szRTDDevNames[NR_RTD_CHANNELS][50] =
  {
    "RTD:ANALYZER_SLOT:RTD_1",
    "RTD:ANALYZER_SLOT:RTD_2"
  };
#endif
  static CRTD obRTD[NR_RTD_CHANNELS];
  int nRetVal = 0;
  long nRTDVal = 0;

  // Open RTD Channels...
  for (int nRTDs = 0; nRTDs < NR_RTD_CHANNELS; nRTDs++)
  {
    nRetVal = obRTD[nRTDs].OpenHal(szRTDDevNames[nRTDs]);
    if (nRetVal < 0)
    {
      printf("Error %d opening RTD Channel: %d\n", nRetVal, nRTDs + 1);
      g_nExitApp = 1;
      break;
    }
  }

  while (g_nExitApp != 1) // TEST MODE
  {
    time_t curTime = time(NULL);
    struct tm *tm_curr = localtime(&curTime);
    
    // Print time when we acquired data...
    printf("%04d/%02d/%02d %02d:%02d:%02d, ", 
           tm_curr->tm_year + 1900, tm_curr->tm_mon + 1, tm_curr->tm_mday, 
           tm_curr->tm_hour, tm_curr->tm_min, tm_curr->tm_sec);
    
    // Read temperature values from the RTD, and print to the console
    for (int nRTDs = 0; nRTDs < NR_RTD_CHANNELS; nRTDs++)
    {
    
      nRetVal = obRTD[nRTDs].GetTempInMilliDegC(&nRTDVal);
      if (nRetVal < 0)
      {
        printf("CRTD[%d].GetTempInMilliDegC() failed: %d\n", nRTDs + 1, nRetVal);
        g_nExitApp = 1;
      }
      else
        printf ("%ld, ", nRTDVal);

      nRetVal = obRTD[nRTDs].GetPWMInMilliPercent(&nRTDVal);
      if (nRetVal < 0)
      {
        printf("CRTD[%d].GetPWMInMilliPercent() failed: %d\n", nRTDs + 1, nRetVal);
        g_nExitApp = 1;
      }
      else
        printf ("%ld, ", nRTDVal);
    }

    printf("\n");

    //Use the user specified time interval b/n RTD reads....
    //if (nIndexFlag)
    //{
    //  usleep(nIndex * 1000);
    //}
    //Else use a default interval of 1 second
    //else
    {
      usleep(1000000);
    }
  }

  // Close RTD Channels...
  for (int nRTDs = 0; nRTDs < NR_RTD_CHANNELS; nRTDs++)
  {
    nRetVal = obRTD[nRTDs].CloseHal();
    if (nRetVal < 0)
      printf("Error %d closing RTD Channel: %d\n", nRetVal, nRTDs + 1);
  }
}

#define NR_PRE_CHANNELS           2
#define ANALYZER_NR_PRE_CHANNELS  1

#define PREAMP_FUNC_SET_GAIN      1
#define PREAMP_FUNC_SET_SRATE     2
#define PREAMP_FUNC_SET_ACBRDG    3
#define PREAMP_FUNC_SET_FILTER    4
#define PREAMP_FUNC_SET_MAVG      5
#define PREAMP_FUNC_SET_LCOR      6
#define PREAMP_FUNC_SET_RCOR      7
#define PREAMP_FUNC_SET_RFINE     8
#define PREAMP_FUNC_GET_GAIN      9
#define PREAMP_FUNC_GET_SRATE     10
#define PREAMP_FUNC_GET_ACBRDG    11
#define PREAMP_FUNC_GET_FILTER    12
#define PREAMP_FUNC_GET_MAVG      13
#define PREAMP_FUNC_GET_LCOR      14
#define PREAMP_FUNC_GET_RCOR      15
#define PREAMP_FUNC_GET_RFINE     16
#define PREAMP_FUNC_GET_BTEMP     17
#define PREAMP_FUNC_GET_BINFO     18
#define PREAMP_FUNC_GET_PAMP      19
#define PREAMP_FUNC_GET_SFACTOR   20
#define PREAMP_FUNC_EN_AUTOZERO   21
#define PREAMP_FUNC_DIS_AUTOZERO  22
#define PREAMP_FUNC_EN_CAL_CFG    23
#define PREAMP_FUNC_EN_CAL_STR    24
#define PREAMP_FUNC_GET_CAL_CFG   25
#define PREAMP_FUNC_GET_CAL_STR   26
#define PREAMP_FUNC_GET_BRIDGE_LEFT_DET_STATUS  27
#define PREAMP_FUNC_GET_BRIDGE_RIGHT_DET_STATUS 28
#define PREAMP_FUNC_START_BROADCAST_STR         29
#define PREAMP_FUNC_STOP_BROADCAST_STR          30
#define PREAMP_FUNC_EXIT_CONF     31
#define PREAMP_FUNC_EXIT_APP      32

int TestPreamp(int nPres)
{
  static char * szPreStrDevNames[NR_PRE_CHANNELS] = { NULL, NULL };
  static char * szPreCfgDevNames[NR_PRE_CHANNELS] = { NULL, NULL };

  int nNumPreChannels = NR_PRE_CHANNELS;
  if( g_b370XAIOBoards )
  {
    nNumPreChannels = ANALYZER_NR_PRE_CHANNELS;
    szPreStrDevNames[0] = "PREAMP_STR:ANALYZER_SLOT:PREAMP_STR_1";
    szPreCfgDevNames[0] = "PREAMP_CFG:ANALYZER_SLOT:PREAMP_CFG_1";
  }
  else
  {
    szPreStrDevNames[0] = "PREAMP_STR:SLOT_1:PREAMP_1";
    szPreStrDevNames[1] = "PREAMP_STR:SLOT_1:PREAMP_2";
    szPreCfgDevNames[0] = "PREAMP_CFG:SLOT_1:PREAMP_1";
    szPreCfgDevNames[1] = "PREAMP_CFG:SLOT_1:PREAMP_2";
  }

  static CPreampStream obPreampStr[NR_PRE_CHANNELS];
  static CPreampConfig obPreampCfg[NR_PRE_CHANNELS];

  int nRetVal = 0;
  int nPreFunc = 0;
  int nOptVal = 0;
  int nExitOptLoop = 0;

  int nValue = 0;
  long lValue = 0;
  float fValue = 0;
  
  unsigned long long ullTS = 0;
  unsigned int unBridgeData = 0;
  unsigned int unRandomReads = 0;

  if ( (nPres < 0) || (nPres >= nNumPreChannels) )
  {
    return -1;
  }

  if (g_nConfigure)
  {
    printf("****** CONF: INITIALIZING PREAMP *******\n");
    nRetVal = obPreampCfg[nPres].OpenHal(szPreCfgDevNames[nPres]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Preamp Channel: %d\n", nRetVal, nPres + 1);
      return -1;
    }
  
    while ( !(nExitOptLoop))
    {
      printf("Select preamp function:\n");
      printf("  %d: Set Gain.\n" , PREAMP_FUNC_SET_GAIN);
      printf("  %d: Set Sampling rate\n", PREAMP_FUNC_SET_SRATE);
      printf("  %d: Set AC Bridge status\n", PREAMP_FUNC_SET_ACBRDG);
      printf("  %d: Set Filter status\n", PREAMP_FUNC_SET_FILTER);
      printf("  %d: Set Moving Avg status\n", PREAMP_FUNC_SET_MAVG);
      printf("  %d: Set Offset - left coarse\n", PREAMP_FUNC_SET_LCOR);
      printf("  %d: Set Offset - right coarse\n", PREAMP_FUNC_SET_RCOR);
      printf("  %d: Set Offset - right fine\n", PREAMP_FUNC_SET_RFINE);
      printf("  %d: Get Gain.\n", PREAMP_FUNC_GET_GAIN);
      printf("  %d: Get Sampling rate\n", PREAMP_FUNC_GET_SRATE);
      printf("  %d: Get AC Bridge status\n", PREAMP_FUNC_GET_ACBRDG);
      printf("  %d: Get Filter status\n", PREAMP_FUNC_GET_FILTER);
      printf("  %d: Get Moving Avg status\n", PREAMP_FUNC_GET_MAVG);
      printf("  %d: Get Offset - left coarse\n", PREAMP_FUNC_GET_LCOR);
      printf("  %d: Get Offset - right coarse\n", PREAMP_FUNC_GET_RCOR);
      printf("  %d: Get Offset - right fine\n", PREAMP_FUNC_GET_RFINE);
      printf("  %d: Get onboard temperature\n", PREAMP_FUNC_GET_BTEMP);
      printf("  %d: Get board information\n", PREAMP_FUNC_GET_BINFO);
      printf("  %d: Get preamp value\n", PREAMP_FUNC_GET_PAMP);
      printf("  %d: Get scaling factor between low and high sub channels.\n", PREAMP_FUNC_GET_SFACTOR );
      printf("  %d: Enable AutoZero\n", PREAMP_FUNC_EN_AUTOZERO);
      printf("  %d: Disable AutoZero\n", PREAMP_FUNC_DIS_AUTOZERO);
      printf("  %d: Enable CAL (through config object)\n", PREAMP_FUNC_EN_CAL_CFG);
      printf("  %d: Enable CAL (through streaming object)\n", PREAMP_FUNC_EN_CAL_STR);
      printf("  %d: Get CAL status (through config object)\n", PREAMP_FUNC_GET_CAL_CFG);
      printf("  %d: Get CAL status (through streaming object)\n", PREAMP_FUNC_GET_CAL_STR);
      printf("  %d: Get Bridge Left Detector status\n", PREAMP_FUNC_GET_BRIDGE_LEFT_DET_STATUS);
      printf("  %d: Get Bridge Right Detector status\n", PREAMP_FUNC_GET_BRIDGE_RIGHT_DET_STATUS);
      printf("  %d: Start Broadcast (through streaming object)\n", PREAMP_FUNC_START_BROADCAST_STR);
      printf("  %d: Stop Broadcast (through streaming object)\n", PREAMP_FUNC_STOP_BROADCAST_STR);
      printf("  %d: Exit config.\n", PREAMP_FUNC_EXIT_CONF);
      printf("  %d: Exit applicatin.\n", PREAMP_FUNC_EXIT_APP);
      fflush(stdin);
      scanf("%d", &nPreFunc);

      switch (nPreFunc)
      {
      case PREAMP_FUNC_SET_GAIN:
        printf("Select gain:\n");
        printf("1: Low\n");
        printf("2: High\n");
        printf("3: Auto\n");
        printf("4: No change\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        switch (nOptVal)
        {
        case 1:
          if ( (nRetVal = obPreampCfg[nPres].SetBridgeGain(PREAMP_BRIDGE_GAIN_LOW)) < 0)
          {
            printf("Error setting gain: %d\n", nRetVal);
          }
          break;

        case 2:
          if ( (nRetVal = obPreampCfg[nPres].SetBridgeGain(PREAMP_BRIDGE_GAIN_HIGH)) < 0)
          {
            printf("Error setting gain: %d\n", nRetVal);
          }
          break;

        case 3:
          if ( (nRetVal = obPreampCfg[nPres].SetBridgeGain(PREAMP_BRIDGE_GAIN_AUTO)) < 0)
          {
            printf("Error setting gain: %d\n", nRetVal);
          }
          break;

        case 4:
          break;

        default:
          printf("Invalid selection. Try again.\n");
          break;
        }
        break;

      case PREAMP_FUNC_SET_SRATE:
        printf("Select Sampling Rate:\n");
        printf("1: 6.875 Hz\n");
        printf("2: 13.75 Hz\n");
        printf("3: 27.5 Hz\n");
        printf("4: 55 Hz\n");
        printf("5: No change\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        switch (nOptVal)
        {
        case 1:
          if ( (nRetVal = obPreampCfg[nPres].SetSamplingRate(PREAMP_SAMPLING_RATE_6HZ875)) < 0)
          {
            printf("Error setting sampling rate: %d\n", nRetVal);
          }
          break;

        case 2:
          if ( (nRetVal = obPreampCfg[nPres].SetSamplingRate(PREAMP_SAMPLING_RATE_13HZ75)) < 0)
          {
            printf("Error setting sampling rate: %d\n", nRetVal);
          }
          break;

        case 3:
          if ( (nRetVal = obPreampCfg[nPres].SetSamplingRate(PREAMP_SAMPLING_RATE_27HZ5)) < 0)
          {
            printf("Error setting sampling rate: %d\n", nRetVal);
          }
          break;

        case 4:
          if ( (nRetVal = obPreampCfg[nPres].SetSamplingRate(PREAMP_SAMPLING_RATE_55HZ)) < 0)
          {
            printf("Error setting sampling rate: %d\n", nRetVal);
          }
          break;

        case 5:
          break;

        default:
          printf("Invalid selection. Try again.\n");
          break;
        }
        break;

      case PREAMP_FUNC_SET_ACBRDG:
        printf("Select AC Bridge ON/OFF status:\n");
        printf("1: Switch ON AC Bridge\n");
        printf("2: Switch OFF AC Bridge\n");
        printf("3: No change\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        switch (nOptVal)
        {
        case 1:
          if ( (nRetVal = obPreampCfg[nPres].SetACBridgeStatus(PREAMP_STATE_ON)) < 0)
          {
            printf("Error setting AC Bridge status: %d\n", nRetVal);
          }
          break;

        case 2:
          if ( (nRetVal = obPreampCfg[nPres].SetACBridgeStatus(PREAMP_STATE_OFF)) < 0)
          {
            printf("Error setting AC Bridge status: %d\n", nRetVal);
          }
          break;

        case 3:
          break;

        default:
          printf("Invalid selection. Try again.\n");
          break;
        }
        break;

      case PREAMP_FUNC_SET_FILTER:
        printf("Select Filter ON/OFF status:\n");
        printf("1: Switch ON Filter\n");
        printf("2: Switch OFF Filter\n");
        printf("3: No change\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        switch (nOptVal)
        {
        case 1:
          if ( (nRetVal = obPreampCfg[nPres].SetFilterStatus(PREAMP_STATE_ON)) < 0)
          {
            printf("Error setting Filter status: %d\n", nRetVal);
          }
          break;

        case 2:
          if ( (nRetVal = obPreampCfg[nPres].SetFilterStatus(PREAMP_STATE_OFF)) < 0)
          {
            printf("Error setting Filter status: %d\n", nRetVal);
          }
          break;

        case 3:
          break;

        default:
          printf("Invalid selection. Try again.\n");
          break;
        }
        break;


        break;
      case PREAMP_FUNC_SET_MAVG:
        printf("Select Moving Avg ON/OFF status:\n");
        printf("1: Switch ON moving avg\n");
        printf("2: Switch OFF moving avg\n");
        printf("3: No change\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        switch (nOptVal)
        {
        case 1:
          if ( (nRetVal = obPreampCfg[nPres].SetMovingAvgStatus(PREAMP_STATE_ON)) < 0)
          {
            printf("Error setting Moving Avg status: %d\n", nRetVal);
          }
          break;

        case 2:
          if ( (nRetVal = obPreampCfg[nPres].SetMovingAvgStatus(PREAMP_STATE_OFF)) < 0)
          {
            printf("Error setting Moving Avg status: %d\n", nRetVal);
          }
          break;

        case 3:
          break;

        default:
          printf("Invalid selection. Try again.\n");
          break;
        }
        break;

      case PREAMP_FUNC_SET_LCOR:
        printf("Enter left coarse offset value (0 to 15):\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        if ( (nOptVal < BRIDGE_ADJ_0000) || (nOptVal >= NUM_BRIDGE_ADJ_ENUM) )
        {
          printf("Invalid selection. Try again.\n");
          break;
        }

        if ( (nRetVal = obPreampCfg[nPres].SetBridgeOffset(BRIDGE_LEFT_COARSE, 
                                                           (PREAMP_BRIDGE_ADJ_ENUM)nOptVal)) < 0)
        {
          printf("Error setting offset value: %d\n", nRetVal);
        }

        break;

      case PREAMP_FUNC_SET_RCOR:
        printf("Enter right coarse offset value (0 to 15):\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        if ( (nOptVal < BRIDGE_ADJ_0000) || (nOptVal >= NUM_BRIDGE_ADJ_ENUM) )
        {
          printf("Invalid selection. Try again.\n");
          break;
        }

        if ( (nRetVal = obPreampCfg[nPres].SetBridgeOffset(BRIDGE_RIGHT_COARSE, 
                                                           (PREAMP_BRIDGE_ADJ_ENUM)nOptVal)) < 0)
        {
          printf("Error setting offset value: %d\n", nRetVal);
        }

        break;

      case PREAMP_FUNC_SET_RFINE:
        printf("Enter right fine offset value (0 to 15):\n");
        fflush(stdin);
        scanf("%d", &nOptVal);

        if ( (nOptVal < BRIDGE_ADJ_0000) || (nOptVal >= NUM_BRIDGE_ADJ_ENUM) )
        {
          printf("Invalid selection. Try again.\n");
          break;
        }

        if ( (nRetVal = obPreampCfg[nPres].SetBridgeOffset(BRIDGE_RIGHT_FINE, 
                                                           (PREAMP_BRIDGE_ADJ_ENUM)nOptVal)) < 0)
        {
          printf("Error setting offset value: %d\n", nRetVal);
        }

        break;

      case PREAMP_FUNC_GET_GAIN:
        if ( (nRetVal = obPreampCfg[nPres].GetBridgeGain((PREAMP_BRIDGE_GAIN_ENUM *)&nValue)) < 0)
        {
          printf("Error getting Gain: %d\n", nRetVal);
        }
        else
        {
          printf("Gain: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_SRATE:
        if ( (nRetVal = obPreampCfg[nPres].GetSamplingRate((PREAMP_SAMPLING_RATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting Sampling Rate: %d\n", nRetVal);
        }
        else
        {
          printf("Sampling rate: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_ACBRDG:
        if ( (nRetVal = obPreampCfg[nPres].GetACBridgeStatus((PREAMP_STATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting AC Bridge status: %d\n", nRetVal);
        }
        else
        {
          printf("AC Bridge status: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_FILTER:
        if ( (nRetVal = obPreampCfg[nPres].GetFilterStatus((PREAMP_STATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting Filter status: %d\n", nRetVal);
        }
        else
        {
          printf("Preamp filter status: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_MAVG:
        if ( (nRetVal = obPreampCfg[nPres].GetMovingAvgStatus((PREAMP_STATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting Moving Avg status: %d\n", nRetVal);
        }
        else
        {
          printf("Preamp filter Moving Avg status: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_LCOR:
        if ( (nRetVal = obPreampCfg[nPres].GetBridgeOffset(BRIDGE_LEFT_COARSE, 
                                                           (PREAMP_BRIDGE_ADJ_ENUM *)&nValue)) < 0)
        {
          printf("Error getting offset value: %d\n", nRetVal);
        }
        else
        {
          printf("Left coarse offset: %d\n", nValue);
        }
        break;
      
      case PREAMP_FUNC_GET_RCOR:
        if ( (nRetVal = obPreampCfg[nPres].GetBridgeOffset(BRIDGE_RIGHT_COARSE, 
                                                           (PREAMP_BRIDGE_ADJ_ENUM *)&nValue)) < 0)
        {
          printf("Error getting offset value: %d\n", nRetVal);
        }
        else
        {
          printf("Right coarse offset: %d\n", nValue);
        }
        break;
      
      case PREAMP_FUNC_GET_RFINE:
        if ( (nRetVal = obPreampCfg[nPres].GetBridgeOffset(BRIDGE_RIGHT_FINE, 
                                                           (PREAMP_BRIDGE_ADJ_ENUM *)&nValue)) < 0)
        {
          printf("Error getting offset value: %d\n", nRetVal);
        }
        else
        {
          printf("Right fine offset: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_BTEMP: 
        if ( (nRetVal = obPreampCfg[nPres].GetOnBoardTemp(&nValue)) < 0)
        {
          printf("Error getting onboard temp: %d\n", nRetVal);
        }
        else
        {
          printf("Onboard temperature: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_BINFO:
      {
        unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
        if ( (nRetVal = obPreampCfg[nPres].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
        {
          printf("CPreamp[%d].GetBoardInfo() failed: %d\n", nPres + 1, nRetVal);
        }
        else
        {
          printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
          printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
          printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
          printf("Board Revision : %d\n", byBoardRev);
        }
        
        break;
      }

      case PREAMP_FUNC_GET_PAMP:
        if ( (nRetVal = obPreampCfg[nPres].GetPreampValue(&lValue)) < 0)
        {
          printf("Error getting preamp value: %d\n", nRetVal);
        }
        else
        {
          printf("Preamp value: %ld\n", lValue);
        }
        break;

      case PREAMP_FUNC_GET_SFACTOR:
        if ( (nRetVal = obPreampCfg[nPres].GetScalingFactor(&fValue)) < 0)
        {
          printf("Error getting scaling factor: %d\n", nRetVal);
        }
        else
        {
          printf("Scaling factor between low to high gain sub channels: %f\n", fValue);
        }
        break;

      case PREAMP_FUNC_EN_AUTOZERO:
        printf("Enabling Auto Zero...\n");
        nRetVal = obPreampCfg[nPres].SetAutoZero( true );
        if (nRetVal < 0)
        {
          printf("Error %d enabling Auto Zero on Preamp %d\n", nRetVal, nPres + 1);
        }
        else
        {
          printf(" done\n");
        }
        break;

      case PREAMP_FUNC_DIS_AUTOZERO:
        printf("Disabling Auto Zero...\n");
        nRetVal = obPreampCfg[nPres].SetAutoZero( false );
        if (nRetVal < 0)
        {
          printf("Error %d disabling Auto Zero on Preamp %d\n", nRetVal, nPres + 1);
        }
        else
        {
          printf(" done\n");
        }
        break;

      case PREAMP_FUNC_EN_CAL_CFG:
        printf("Enabling self calibration through config object...\n");
        nRetVal = obPreampCfg[nPres].EnableSelfCalibration();
        if (nRetVal < 0)
        {
          printf("Error %d enabling CAL through config object on Preamp %d\n", nRetVal, nPres + 1);
        }
        else
        {
          printf(" done\n");
        }
        break;

      case PREAMP_FUNC_EN_CAL_STR:
        nRetVal = obPreampStr[nPres].OpenHal(szPreStrDevNames[nPres]);

        if (nRetVal < 0)
        {
          printf("Error %d opening Preamp streaming Channel: %d\n", nRetVal, nPres + 1);
          break;
        }
        
        printf("Enabling self calibration through streaming object...\n");
        nRetVal = obPreampStr[nPres].EnableSelfCalibration();
        
        if (nRetVal < 0)
        {
          printf("Error %d enabling CAL through streaming object on Preamp %d\n", nRetVal, nPres + 1);
        }
        else
        {
          printf(" done\n");
        }
    
        nRetVal = obPreampStr[nPres].CloseHal();
      
        if (nRetVal < 0)
        {
          printf("Error %d closing Preamp streaming channel: %d\n", nRetVal, nPres + 1);
          break;
        }
        
        break;
      
      case PREAMP_FUNC_GET_CAL_CFG:
        if ( (nRetVal = obPreampCfg[nPres].GetCalOnOffStatus((PREAMP_STATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting CAL in progress status: %d\n", nRetVal);
        }
        else
        {
          printf("Preamp CAL in progress status: %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_CAL_STR:
        nRetVal = obPreampStr[nPres].OpenHal(szPreStrDevNames[nPres]);
        
        if (nRetVal < 0)
        {
          printf("Error %d opening Preamp streaming Channel: %d\n", nRetVal, nPres + 1);
          break;
        }

        if ( (nRetVal = obPreampStr[nPres].GetCalOnOffStatus((PREAMP_STATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting CAL in progress status: %d\n", nRetVal);
        }
        else
        {
          printf("Preamp CAL in progress status: %d\n", nValue);
        }
        
        nRetVal = obPreampStr[nPres].CloseHal();
      
        if (nRetVal < 0)
        {
          printf("Error %d closing Preamp streaming channel: %d\n", nRetVal, nPres + 1);
          break;
        }
        break;
      
      case PREAMP_FUNC_GET_BRIDGE_LEFT_DET_STATUS:
        if ( (nRetVal = obPreampCfg[nPres].GetBRLeftDetStatus((PREAMP_STATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting Bridge Left Detector status : %d\n", nRetVal);
        }
        else
        {
          printf("Preamp Bridge Left Detector status : %d\n", nValue);
        }
        break;

      case PREAMP_FUNC_GET_BRIDGE_RIGHT_DET_STATUS:
        if ( (nRetVal = obPreampCfg[nPres].GetBRRightDetStatus((PREAMP_STATE_ENUM *)&nValue)) < 0)
        {
          printf("Error getting Bridge Right Detector status : %d\n", nRetVal);
        }
        else
        {
          printf("Preamp Bridge Right Detector status : %d\n", nValue);
        }
        break;
  
      case PREAMP_FUNC_START_BROADCAST_STR:
        nRetVal = obPreampStr[nPres].OpenHal(szPreStrDevNames[nPres]);

        if (nRetVal < 0)
        {
          printf("Error %d opening Preamp streaming Channel: %d\n", nRetVal, nPres + 1);
          break;
        }

        if ( (nRetVal = obPreampStr[nPres].SetBroadcastMode(1)) < 0)
        {
          printf("Error starting broadcast: %d\n", nRetVal);
        }
        else
        {
          printf("Start broadcast successful\n");
        }

        nRetVal = obPreampStr[nPres].CloseHal();

        if (nRetVal < 0)
        {
          printf("Error %d closing Preamp streaming channel: %d\n", nRetVal, nPres + 1);
          break;
        }
        break;

      case PREAMP_FUNC_STOP_BROADCAST_STR:
        nRetVal = obPreampStr[nPres].OpenHal(szPreStrDevNames[nPres]);

        if (nRetVal < 0)
        {
          printf("Error %d opening Preamp streaming Channel: %d\n", nRetVal, nPres + 1);
          break;
        }

        if ( (nRetVal = obPreampStr[nPres].SetBroadcastMode(0)) < 0)
        {
          printf("Error stopping broadcast: %d\n", nRetVal);
        }
        else
        {
          printf("Stop broadcast successful\n");
        }

        nRetVal = obPreampStr[nPres].CloseHal();

        if (nRetVal < 0)
        {
          printf("Error %d closing Preamp streaming channel: %d\n", nRetVal, nPres + 1);
          break;
        }
        break;

      case PREAMP_FUNC_EXIT_CONF:
        nExitOptLoop = 1;
        break;

      case PREAMP_FUNC_EXIT_APP:
        nExitOptLoop = 1;
        g_nExitApp = 1;
        break;
      };
    }

    nRetVal = obPreampCfg[nPres].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("Error %d closing Preamp channel: %d\n", nRetVal, nPres + 1);
      g_nExitApp = 1;
    }
  }

  while (g_nExitApp != 1) //TEST MODE
  {
    DEBUG("****** INITIALIZING PREAMP *******\n");
    nRetVal = obPreampStr[nPres].OpenHal(szPreStrDevNames[nPres]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Preamp Channel: %d\n", nRetVal, nPres + 1);
      return 0;
    }
      
    nRetVal = obPreampStr[nPres].SetBroadcastMode(TRUE);

    if (nRetVal < 0)
    {
      printf("Error %d enabling broadcast mode on Preamp %d\n", nRetVal, nPres + 1);
      g_nExitApp = 1;
      goto pre_close;
    }

    usleep(rand() % 1000001);

    unRandomReads = (rand() % 501);
    DEBUG("Reading %d samples.\n", unRandomReads);

    for (unsigned int nReads = 0; nReads < unRandomReads; nReads++)
    {
      nRetVal = obPreampStr[nPres].ReadStreamData(&unBridgeData, &ullTS, 1000);
      if (nRetVal < 0)
      {
        printf("CPreamp[%d]..ReadStreamData() failed: %d\n", nPres + 1, nRetVal);
        g_nExitApp = 1;
        goto pre_close;
        break;
      }
      else
      {
      }
    }
  
    DEBUG("****** CLOSING PREAMP *******\n");
    nRetVal = obPreampStr[nPres].SetBroadcastMode(FALSE);
      
    if (nRetVal < 0)
    {
      printf("Error %d disabling broadcast mode on Preamp %d\n", nRetVal, nPres + 1);
      g_nExitApp = 1;
      goto pre_close;
    }
  
  pre_close:
    nRetVal = obPreampStr[nPres].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("Error %d closing Preamp channel: %d\n", nRetVal, nPres + 1);
      g_nExitApp = 1;
      break;
    }

    //This is if SetBroadcastMode(TRUE/FALSE) or ReadStreamData() fail
    if (g_nExitApp)
    {
      break;
    }

    usleep( rand() % 1000001);
  }

  return 0;
}

#define NR_SER_CHANNELS            3

#define SERIAL_FUNC_SET_PORT_CFG   1
#define SERIAL_FUNC_GET_PORT_CFG   2 
#define SERIAL_FUNC_SEND_DATA      3
#define SERIAL_FUNC_RECEIVE_DATA   4
#define SERIAL_FUNC_GET_BTEMP      5
#define SERIAL_FUNC_GET_BINFO      6
#define SERIAL_FUNC_SEND_FILE      7
#define SERIAL_FUNC_RECEIVE_FILE   8
#define SERIAL_FUNC_SEND_RECV_FILE 9
#define SERIAL_FUNC_SET_TX_TIMEOUT 10
#define SERIAL_FUNC_EXIT_CONF      11
#define SERIAL_FUNC_EXIT_APP       12

int TestSerialIO(int nSer)
{
  static char szSerDevNames[NR_SER_CHANNELS][50] = { 
    "SERIAL:SLOT_BASE_IO:SERIAL_1",
    "SERIAL:SLOT_BASE_IO:SERIAL_2",
    "SERIAL:SLOT_BASE_IO:SERIAL_3",
  };

  static CSerial obSerial[NR_SER_CHANNELS];

  int nRetVal = 0;

  int nSerFunc = 0;

  int nExitOptLoop = 0;

  int nValue = 0;
  
  if ( (nSer < 0) || (nSer >= NR_SER_CHANNELS) )
  {
    return -1;
  }

  {
    printf("****** CONF: INITIALIZING SERIAL IO *******\n");
    nRetVal = obSerial[nSer].OpenHal(szSerDevNames[nSer]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Serial Channel: %d\n", nRetVal, nSer + 1);
      return -1;
    }
  
    while ( !(nExitOptLoop))
    {
      if (g_nExitApp)
      {
        nExitOptLoop = TRUE;
        goto quit_ser_conf;
      }

      printf("Select Serial function:\n");
      printf("  %d: Configure serial port.\n" , SERIAL_FUNC_SET_PORT_CFG);
      printf("  %d: Get serial port config.\n", SERIAL_FUNC_GET_PORT_CFG);
      printf("  %d: Send data.\n", SERIAL_FUNC_SEND_DATA);
      printf("  %d: Receive data.\n", SERIAL_FUNC_RECEIVE_DATA);
      printf("  %d: Board temperature.\n", SERIAL_FUNC_GET_BTEMP);
      printf("  %d: Board revision.\n", SERIAL_FUNC_GET_BINFO);
      printf("  %d: Send file.\n", SERIAL_FUNC_SEND_FILE);
      printf("  %d: Receive file.\n", SERIAL_FUNC_RECEIVE_FILE);
      printf("  %d: Send and receive file.\n", SERIAL_FUNC_SEND_RECV_FILE);
      printf("  %d: Set device TX timeout.\n", SERIAL_FUNC_SET_TX_TIMEOUT);
      printf("  %d: Exit config.\n", SERIAL_FUNC_EXIT_CONF);
      printf("  %d: Exit application.\n", SERIAL_FUNC_EXIT_APP);
      fflush(stdin);
      scanf("%d", &nSerFunc);

      switch (nSerFunc)
      {
      case SERIAL_FUNC_SET_PORT_CFG:
      {
        int nBaud = 0, nStopB = 0, nParity = 0, nSerMode = 0, nDataB = 0;
        int nAbort = FALSE;
        printf("NOTE: Selecting 'No change' for any option will abort port configuration.\n");
  
        printf("Select baudrate:\n");
        printf("7. 9600\n");
        printf("9. 19200\n");
        printf("11. 57600\n");
        printf("12. 115200\n");
        printf("13. No change\n");
        fflush(stdin);
        scanf("%d", &nBaud);

        switch(nBaud)
        {
        case 7: case 9: case 11: case 12:
          nBaud--;
          break;

        case 13:
          nAbort = TRUE;
          break;

        default:
          printf("Invalid option.\n");
          nAbort = TRUE;
          break;
        }

        if (!nAbort)
        {
          printf("Select Data size:\n");
          printf("1. 5 data bits.\n");
          printf("2. 6 data bits.\n");
          printf("3. 7 data bits.\n");
          printf("4. 8 data bits.\n");

          printf("5. No change.\n");
          fflush(stdin);
          scanf("%d", &nDataB);
        
          switch(nDataB)
          {
          case 1: case 2: case 3: case 4:
            nDataB--;
            break;

          case 5:
            nAbort = TRUE;
            break;
  
          default:
            printf("Invalid option.\n");
            nAbort = TRUE;
            break;
          }
        }

        if (!nAbort)
        {
          printf("Select Stop bits:\n");
          printf("1. 1 stop bit.\n");
          printf("2. 1.5 stop bits.\n");
          printf("3. 2 stop bits.\n");
          printf("4. No change.\n");
          fflush(stdin);
          scanf("%d", &nStopB);

          switch(nStopB)
          {
          case 1: case 2: case 3:
            nStopB--;
            break;

          case 4:
            nAbort = TRUE;
            break;

          default:
            printf("Invalid option.\n");
            nAbort = TRUE;
            break;
          }
        }

        if (!nAbort)
        {
      
          printf("Select parity:\n");
          printf("1. Even.\n");
          printf("2. Odd.\n");
          printf("3. Space.\n");
          printf("4. Mark.\n");
          printf("5. None.\n");
          printf("6. Multidrop.\n");
          printf("7. No change.\n");
          fflush(stdin);
          scanf("%d", &nParity);
        
          switch(nParity)
          {
          case 1: case 2: case 3: case 4: case 5: case 6:
            nParity--;
            break;

          case 7:
            nAbort = TRUE;
            break;
  
          default:
            printf("Invalid option.\n");
            nAbort = TRUE;
            break;
          }
        }

        if (!nAbort)
        {

          printf("Select port mode:\n");
          printf("1. RS-232 (normal)\n");
          printf("2. RS-485\n");
          printf("3. HW Handshake\n");
          printf("4. No change.\n");
          fflush(stdin);
          scanf("%d", &nSerMode);

          switch(nSerMode)
          {
          case 1: case 2: case 3:
            nSerMode--;
            break;
  
          case 4:
            nAbort = TRUE;
            break;
  
          default:
            printf("Invalid option.\n");
            nAbort = TRUE;
            break;
          }
        }
        
        if (!nAbort)
        {
          if ( (nRetVal = obSerial[nSer].SetCommParams(
                  (BASEIO_SER_BAUD_ENUM)nBaud, 
                  (BASEIO_SER_DATA_LEN_ENUM)nDataB, 
                  (BASEIO_SER_PARITY_ENUM)nParity, 
                  (BASEIO_SER_STOP_BITS_ENUM)nStopB, 
                  (BASEIO_SER_PORT_MODE_ENUM)nSerMode)) < 0)
          {
            printf("Serial port config failed with error code: %d\n", nRetVal);
          }
        }
      }
      break;

      case SERIAL_FUNC_GET_PORT_CFG:
      {
        int nBaud = 0, nStopB = 0, nParity = 0, nSerMode = 0, nDataB = 0;

        if ( (nRetVal = obSerial[nSer].GetCommParams(
                (BASEIO_SER_BAUD_ENUM *)&nBaud, 
                (BASEIO_SER_DATA_LEN_ENUM *)&nDataB, 
                (BASEIO_SER_PARITY_ENUM *) &nParity, 
                (BASEIO_SER_STOP_BITS_ENUM *) &nStopB, 
                (BASEIO_SER_PORT_MODE_ENUM *) &nSerMode)) < 0)
        {
          printf("Error %d getting serial port config\n", nRetVal);
        }
        else
        {
          printf("Baud rate (enum value): %d\n", nBaud);
          printf("Data bits (enum value): %d\n", nDataB);
          printf("Parity (enum value): %d\n", nParity);
          printf("Stop bits (enum value): %d\n", nStopB);
          printf("Port mode (enum value): %d\n", nSerMode);
        }

      }
      break;

      case SERIAL_FUNC_SEND_DATA:
      {
        char arr[1024];
        printf("Enter string to transmit:\n");
        fflush(stdin);
        fscanf(stdin, "%s", arr);
        printf("Sending: %s\n", arr);
        if ( (nRetVal = obSerial[nSer].WriteCh((unsigned char *)arr, strlen(arr))) < 0)
        {
          printf("Error %d sending data over the serial channel %d\n", nRetVal, nSer);
        }
      }
      break;

      case SERIAL_FUNC_RECEIVE_DATA:
      {
        char arr[1024];
        if ( (nRetVal = obSerial[nSer].ReadCh((unsigned char *)arr, 1023, 15000)) < 0)
        {
          printf("Error %d receiving data over the serial channel %d\n", nRetVal, nSer);
        }
        else
        {
          arr[nRetVal] = '\0';
          printf("Received: %s\n", arr);
        }
      }
      break;

      case SERIAL_FUNC_GET_BTEMP: 
        if ( (nRetVal = obSerial[nSer].GetOnBoardTemp(&nValue)) < 0)
        {
          printf("Error getting onboard temp: %d\n", nRetVal);
        }
        else
        {
          printf("Onboard temperature: %d\n", nValue);
        }
        break;

      case SERIAL_FUNC_GET_BINFO:
      {
        unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
        if ( (nRetVal = obSerial[nSer].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
        {
          printf("CSerial[%d].GetBoardInfo() failed: %d\n", nSer + 1, nRetVal);
        }
        else
        {
          printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
          printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
          printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
          printf("Board Revision : %d\n", byBoardRev);
        }
      }
      break;

      case SERIAL_FUNC_SEND_FILE:
      {
        int fd = 0;
        int fl_retval = 0;
        long fileSize = 0;
        char filePath[255] = {'\0'};
        char fileContents[255];
        printf("Enter file path: \n");
        scanf("%s", filePath);
        fd = open(filePath, O_RDONLY);
        if (fd < 0)
        {
          printf("Open failed.\n");
        }
        else
        {
          do
          {
            fl_retval = read(fd, fileContents, 50);

            if (fl_retval < 0)
            {
              printf("Error occurred reading file contents.\n");
              break;
            }
            else if (0 == fl_retval)
            {
              printf("Transmitted %ld bytes.\n", fileSize);
            }
            else
            {
              fileSize += fl_retval;
              if ( (nRetVal = obSerial[nSer].WriteCh((unsigned char *)fileContents, fl_retval)) < 0)
              {
                printf("File TX failed with error code: %d\n", nRetVal);
              }
              else
              {
                //Yield for BaseIO to flush the data through its Serial Port.
                usleep( 60000 );

                if( g_nExitApp )
                {
                  g_nExitApp = 0;
                  break;
                }
              }
            }
          } while ( (fl_retval > 0) && (nRetVal >= 0));

          close(fd);
        }
      }
      break;

      case SERIAL_FUNC_RECEIVE_FILE:
      {
        int rxfd = 0;
        int rxfl_retval = 0;
        long rxFileSize = 0;
        long rxFileBytes = 0;
        char rxFilePath[255] = {'\0'};
        char rxFileContents[1024];
        char rxRetryCnt = 0;

        printf("Enter file path:\n");
        scanf("%s", rxFilePath);
        printf("Enter expected file size:\n");
        scanf("%ld", &rxFileSize);
        rxfd = open(rxFilePath, O_RDWR | O_CREAT);
        if (rxfd < 0)
        {
          printf("RX file: Open failed.\n");
        }
        else
        {
          do
          {
            if ( (nRetVal = obSerial[nSer].ReadCh((unsigned char*)rxFileContents, 1024, 1000)) < 0)
            {
              if (nRetVal != ERR_TIMEOUT)
              {
                printf("RX failed with error code: %d\n", nRetVal);
              }
              else
              {
                rxRetryCnt++;
                if (rxRetryCnt < 15)
                {
                  nRetVal = 0;
                }
                else
                {
                  printf("Timeout on file receive: %d\n", nRetVal);
                }
              }
            }
            else
            {
              rxRetryCnt = 0;
              rxfl_retval = write(rxfd, rxFileContents, nRetVal);
                
              if (rxfl_retval != nRetVal)
              {
                printf("RX: Write to file failed.\n");
              }
              else
              {
                rxFileBytes += rxfl_retval;
                if (rxFileBytes == rxFileSize)
                {
                  printf("Received %ld bytes.\n", rxFileBytes);
                  printf("Receive complete.\n");
                  break;
                }
                else if (rxFileBytes > rxFileSize)
                {
                  printf("Received %ld bytes.\n", rxFileBytes);
                  printf("Received more than the expected number of bytes.\n");
                  break;
                }
              }
            }
          } while ( (rxfl_retval >= 0) && (nRetVal >= 0));

          close(rxfd);
        }
      }
      break;

      case SERIAL_FUNC_SEND_RECV_FILE:
      {
        int rxfd = 0;
        int rxfl_retval = 0;
        int rx_serRetVal = 0;
        long rxFileSize = 0;
        long rxFileBytes = 0;
        char rxFilePath[255] = {'\0'};
        char rxFileContents[1024];
        char rxRetryCnt = 0;
        int rxDone = 0;
            
        int fd = 0;
        int fl_retval = 0;
        int serRetVal = 0;
        long fileSize = 0;
        char filePath[255] = {'\0'};
        char fileContents[1024];

        int txDone = 0;
          
        printf("Enter path of file to TX: \n");
        scanf("%s", filePath);
        fd = open(filePath, O_RDONLY);

        printf("Enter RX file save path:\n");
        scanf("%s", rxFilePath);
        printf("Enter expected RX file size:\n");
        scanf("%ld", &rxFileSize);
        rxfd = open(rxFilePath, O_RDWR | O_CREAT);
        if (rxfd < 0)
        {
          printf("RX file: Open failed.\n");
        }


          
        if (fd < 0)
        {
          printf("TX file Open failed.\n");
        }

        else if (rxfd < 0)
        {
          printf("RX file Open failed.\n");
        }
        else
        {
          do
          {

//TRANSMIT
            if (!txDone)
            {
              fl_retval = read(fd, fileContents, 64);
  
              if (fl_retval < 0)
              {
                printf("Error occurred reading file contents.\n");
                break;
              }
              else if (0 == fl_retval)
              {
                printf("Transmitted %ld bytes.\n", fileSize);
                txDone = 1;
              }
              else
              {
                fileSize += fl_retval;
                if ( (serRetVal = obSerial[nSer].WriteCh((unsigned char *)fileContents, 
                                                         fl_retval)) < 0)
                {
                  printf("File TX failed with error code: %d\n", serRetVal);
                }
              }
            }


//RECEIVE
            if (!rxDone)
            {
              if ( (rx_serRetVal = obSerial[nSer].ReadCh((unsigned char*)rxFileContents, 
                                                         1024, 
                                                         1000/*timeout*/)) < 0)
              {
                if (rx_serRetVal != ERR_TIMEOUT)
                {
                  printf("RX failed with error code: %d\n", rx_serRetVal);
                  rxDone = 1;
                }
                else
                {
                  rxRetryCnt++;
                  if (rxRetryCnt < 3)
                  {
                    rx_serRetVal = 0;
                  }
                  else
                  {
                    rxDone = 1;
                    printf("Timeout on file receive: %d\n", rx_serRetVal);
                  }
                }
              }
              else
              {
                rxRetryCnt = 0;
                rxfl_retval = write(rxfd, rxFileContents, rx_serRetVal);
                  
                if (rxfl_retval != rx_serRetVal)
                {
                  printf("RX: Write to file failed.\n");
                }
                else
                {
                  rxFileBytes += rxfl_retval;
                  if (rxFileBytes == rxFileSize)
                  {
                    printf("Received %ld bytes.\n", rxFileBytes);
                    printf("Receive complete.\n");
                    rxDone = 1;
                  }
                  else if (rxFileBytes > rxFileSize)
                  {
                    printf("Received %ld bytes.\n", rxFileBytes);
                    printf("Received more than the expected number of bytes.\n");
                    rxDone = 1;
                  }
                }
              }
            }           
          } while ( (!rxDone) || (!txDone));

          printf("Transmit done: %d\n", txDone);
          printf("Receive done: %d\n", rxDone);
          close(fd);
          close(rxfd);

        }

      }
      break;

      case SERIAL_FUNC_SET_TX_TIMEOUT:
      {
        unsigned long Timeout;
        printf("Enter TX timeout (ms)\n");
        fflush(stdin);
        scanf("%lu", &Timeout);

        printf("User entered timeout: %lu\n", Timeout);

        if ( (nRetVal = obSerial[nSer].SetTxTimeoutMs(Timeout)) < 0)
        {
          printf("Error %d setting TX timeout\n", nRetVal);
        }
      }
      break;
        
      case SERIAL_FUNC_EXIT_CONF:
        nExitOptLoop = 1;
        break;

      case SERIAL_FUNC_EXIT_APP:
        nExitOptLoop = 1;
        g_nExitApp = 1;
        break;
      };
    }

  quit_ser_conf:
    nRetVal = obSerial[nSer].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("Error %d closing Serial channel: %d\n", nRetVal, nSer + 1);
      g_nExitApp = 1;
    }
  }

  return 0;
}

void TestLongRunDigital_InOut()
{
  int nRetVal = 0;
#define TEST_NR_DIN_CHANNELS 8
  static char * szDinDevNames[TEST_NR_DIN_CHANNELS]= {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  szDinDevNames[0] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_1";
  szDinDevNames[1] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_2";
  szDinDevNames[2] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_3";
  szDinDevNames[3] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_4";
  szDinDevNames[4] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_5";
  szDinDevNames[5] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_6";
  szDinDevNames[6] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_7";
  szDinDevNames[7] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_8";
  static CDigitalIn obDin[TEST_NR_DIN_CHANNELS];

#define TEST_NR_DOT_CHANNELS 8
  static char * szDotDevNames[TEST_NR_DOT_CHANNELS] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  szDotDevNames[0] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_1";
  szDotDevNames[1] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_2";
  szDotDevNames[2] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_3";
  szDotDevNames[3] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_4";
  szDotDevNames[4] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_5";
  szDotDevNames[5] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_2";
  szDotDevNames[6] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_3";
  szDotDevNames[7] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_4";
  static CDigitalOut obDot[TEST_NR_DOT_CHANNELS];

  for (int nInd = 0; nInd < TEST_NR_DIN_CHANNELS; nInd++)
  {
    nRetVal = obDin[nInd].OpenHal(szDinDevNames[nInd]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Digital IN Channel: %s\n", nRetVal, szDinDevNames[nInd]);
      return;
    }
  }
  for (int nInd = 0; nInd < TEST_NR_DOT_CHANNELS; nInd++)
  {
    nRetVal = obDot[nInd].OpenHal(szDotDevNames[nInd]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Digital OUT Channel: %s\n", nRetVal, szDotDevNames[nInd]);
      return;
    }
  }

  int nTxData1 = 0x0A;
  int nTxData2 = 0x15;
  
  int nTxNum = 0;
  int nTxCount = 0x00;
  while( g_nExitApp != 1 )
  {
    if( nTxNum % 2 )
    {
      nTxCount = nTxData1;
    }
    else
    {
      nTxCount = nTxData2;
    }
    nTxNum++;

    printf("Status, Tx : -");
    for( int nInd = 0; nInd < 5 /*TEST_NR_DOT_CHANNELS*/; nInd++ )
    {
      BOOL bValue = (nTxCount & (0x1 << nInd)) ? TRUE : FALSE;
      printf("%s-", (bValue==TRUE)?"1":"0");
    }
    printf("\n");
    for( int nInd = 0; nInd < 5 /*TEST_NR_DOT_CHANNELS*/; nInd++ )
    {
      BOOL bValue = (nTxCount & (0x1 << nInd)) ? TRUE : FALSE;
      if( nInd > 4 )
      {
        //BASEIO DO are of opposite polarity!!!
        bValue = bValue ? FALSE : TRUE;
      }
      if((nRetVal = obDot[nInd].SetDigitalOutStatus(bValue)) < 0)
      {
        printf("Error, CDigitalOut[%d].SetDigitalOutStatus() failed : %d\n", nInd + 1, nRetVal);
      }
    }
    printf("\n");

    //Use the user specified time interval b/n Digital Input reads....
    if (nIndexFlag)
    {
      usleep(nIndex * 1000);
    }
    //Else use a default interval of 1 second
    else
    {
      usleep(1000000);
    }

    printf("Status, Rx : -");
    int nRxCount = 0x00;
    for( int nInd = 0; nInd < 5 /*TEST_NR_DIN_CHANNELS*/; nInd++ )
    {
      BOOL bValue = FALSE;
      if((nRetVal = obDin[nInd].GetDigitalInStatus(&bValue)) < 0 )
      {
        printf( "Error, CDigitalIn[%d].GetDigitalInStatus() failed :  %d\n", nInd+1, nRetVal );
      }
      printf("%s-", (bValue==TRUE)?"1":"0");
      nRxCount = nRxCount | (((bValue==TRUE)?1:0)<<nInd);
    }
    printf("\n");
    char * szStatus = "Success";
    if( nTxCount != nRxCount )
    {
      szStatus = "Failure";
    }
    printf( "%s, Tx - 0x%x, Rx - 0x%x\n", szStatus, nTxCount, nRxCount );
    nTxCount++;
    if( nTxCount > 0xFF )
    {
      nTxCount = 0x00;
    }
  }
  for (int nInd = 0; nInd < TEST_NR_DIN_CHANNELS; nInd++)
  {
    nRetVal = obDin[nInd].CloseHal();

    if (nRetVal < 0)
    {
      printf("Error %d closing Digital IN Channel: %s\n", nRetVal,  szDinDevNames[nInd]);
    }
  }
  for (int nInd = 0; nInd < TEST_NR_DOT_CHANNELS; nInd++)
  {
    nRetVal = obDot[nInd].CloseHal();

    if (nRetVal < 0)
    {
      printf("Error %d closing Digital OUT Channel: %s\n", nRetVal, szDotDevNames[nInd]);
    }
  }
}

void TestLongRunAnalog_InOut()
{
  int nRetVal = 0;
#define OUT_CURR_INC_STEP   1000000
#define OUT_CURR_BASE_VALUE   1000000

#define TEST_NR_AIN_CHANNELS 6
  unsigned long ulReadAinCurrent[TEST_NR_AIN_CHANNELS] = {0,0,0,0,0,0};
  static char * szAinDevNames[TEST_NR_AIN_CHANNELS]= {NULL,NULL,NULL,NULL,NULL,NULL};

  int nEndAICh = -1;
  if( g_b370XAIOBoards )
  {
    szAinDevNames[0] = "ANA_IN:ANALYZER_SLOT:ANA_IN_1"; 
    nEndAICh = 0;
  }
  else
  {
    szAinDevNames[0] = "ANA_IN:EXP_SLOT_2:ANA_IN_1";
    szAinDevNames[1] = "ANA_IN:EXP_SLOT_2:ANA_IN_2";
    szAinDevNames[2] = "ANA_IN:EXP_SLOT_2:ANA_IN_3";
    szAinDevNames[3] = "ANA_IN:EXP_SLOT_2:ANA_IN_4";
    szAinDevNames[4] = "ANA_IN:EXP_SLOT_1:ANA_IN_1";
    szAinDevNames[5] = "ANA_IN:EXP_SLOT_1:ANA_IN_2";
    nEndAICh = 5;
  }

  static CAnalogIn obAin[TEST_NR_AIN_CHANNELS];

#define TEST_NR_AOT_CHANNELS 6
  unsigned long ulWriteAotCurrent[TEST_NR_AOT_CHANNELS] = {OUT_CURR_BASE_VALUE,
                                                           OUT_CURR_BASE_VALUE,
                                                           OUT_CURR_BASE_VALUE,
                                                           OUT_CURR_BASE_VALUE,
                                                           OUT_CURR_BASE_VALUE,
                                                           OUT_CURR_BASE_VALUE};
  unsigned long ulWroteAotCurrent[TEST_NR_AOT_CHANNELS] = {0,0,0,0,0,0};
  static char * szAotDevNames[TEST_NR_AOT_CHANNELS] = {NULL,NULL,NULL,NULL,NULL,NULL};

  int nEndAOCh = -1;
  if( g_b370XAIOBoards )
  {
    szAotDevNames[0] = "ANA_OUT:370XA_LOI_SLOT:ANA_OUT_1";
    nEndAOCh = 0;
  }
  else
  {
    szAotDevNames[0] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_1";
    szAotDevNames[1] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_2";
    szAotDevNames[2] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_3";
    szAotDevNames[3] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_4";
    szAotDevNames[4] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_5";
    szAotDevNames[5] = "ANA_OUT:SLOT_BASE_IO:ANA_OUT_6";
    nEndAOCh = 5;
  }

  static CAnalogOut obAot[TEST_NR_AOT_CHANNELS];

  for (int nInd = 0; nInd <= nEndAICh; nInd++)
  {
    nRetVal = obAin[nInd].OpenHal(szAinDevNames[nInd]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Analog IN Channel: %s\n", nRetVal, szAinDevNames[nInd]);
      return;
    }
  }
  for (int nInd = 0; nInd <= nEndAOCh; nInd++)
  {
    nRetVal = obAot[nInd].OpenHal(szAotDevNames[nInd]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Analog OUT Channel: %s\n", nRetVal, szAotDevNames[nInd]);
      return;
    }
  }

  while( g_nExitApp != 1 )
  {
    printf("Out Current(nA) : ");
    for( int nInd = 0; nInd <= nEndAOCh; nInd++ )
    {
      unsigned long ulOutCurrent = ulWriteAotCurrent[nInd];
      if((nRetVal = obAot[nInd].SetCurrent(ulOutCurrent)) < 0)
      {
        printf("Error, CAnalogOut[%d].SetCurrent() failed : %d, ", nInd + 1, nRetVal);
      }
      else
      {
        printf("%ld, ", ulOutCurrent);
      }
      ulWroteAotCurrent[nInd] = ulWriteAotCurrent[nInd];
      ulWriteAotCurrent[nInd] += OUT_CURR_INC_STEP;
      if( ulWriteAotCurrent[nInd] > 20000000 )
      {
        ulWriteAotCurrent[nInd] = OUT_CURR_BASE_VALUE;
      }
    }
    printf("\n");

    //Use the user specified time interval before reading Analog Inputs ...
    if (nIndexFlag)
    {
      usleep(nIndex * 1000);
    }
    //Else use a default interval of 1 second
    else
    {
      usleep(1000000);
    }

    printf("In Current(nA)  : ");
    for( int nInd = 0; nInd <= nEndAICh; nInd++ )
    {
      unsigned long ulInCurrent = 0;
      if((nRetVal = obAin[nInd].GetCurrent(&ulInCurrent)) < 0 )
      {
        printf( "Error, CAnalogIn[%d].GetCurrent() failed :  %d, ", nInd+1, nRetVal );
      }
      else
      {
        printf( "%ld, ", ulInCurrent );
      }
      ulReadAinCurrent[nInd] = ulInCurrent;
    }
    printf("\n");
    printf("Status : ");
    for( int nInd = 0; nInd <= nEndAICh; nInd++ )
    {
      char * szStatus = "Success";
      double dReadValue = ulReadAinCurrent[nInd];
      double dWroteValue = ulWroteAotCurrent[nInd];
//printf( "R %f - W %f", dReadValue, dWroteValue );
      if( (dReadValue > (dWroteValue * 1.1)) || 
          (dReadValue < (dWroteValue * 0.9)) )
      {
        szStatus = "Failure";
      }
      printf( "%s, ", szStatus );
    }
    printf("\n" );
  }
  for (int nInd = 0; nInd <= nEndAICh; nInd++)
  {
    nRetVal = obAin[nInd].CloseHal();

    if (nRetVal < 0)
    {
      printf("Error %d closing Analog IN Channel: %s\n", nRetVal,  szAinDevNames[nInd]);
    }
  }
  for (int nInd = 0; nInd <= nEndAOCh; nInd++)
  {
    nRetVal = obAot[nInd].CloseHal();

    if (nRetVal < 0)
    {
      printf("Error %d closing Analog OUT Channel: %s\n", nRetVal, szAotDevNames[nInd]);
    }
  }
}

#define NR_DIN_CHANNELS       37

#define DIN_FUNC_GET_DIN      1
#define DIN_FUNC_GET_BRD_TEMP 2
#define DIN_BOARD_INFO        3
#define DIN_FUNC_EXIT         4
#define DIN_FUNC_GET_DIN_CONT 5

void TestDigitalIn()
{
  int nRetVal = 0;
  
  static char * szDinDevNames[NR_DIN_CHANNELS]= {NULL,NULL,NULL,NULL,NULL};
  int nNumDigitalInChannels = NR_DIN_CHANNELS;
  if( g_b370XAIOBoards )
  {
    nNumDigitalInChannels = 4;
    szDinDevNames[0] = "DIGI_IN:ANALYZER_SLOT:DIGI_IN_1";
    szDinDevNames[1] = "DIGI_IN:ANALYZER_SLOT:DIGI_IN_2";
    szDinDevNames[2] = "DIGI_IN:ANALYZER_SLOT:DIGI_IN_3";
    szDinDevNames[3] = "DIGI_IN:ANALYZER_SLOT:DIGI_IN_4";
  }
  else
  {
    szDinDevNames[0] = "DIGI_IN:SLOT_BASE_IO:DIGI_IN_1";
    szDinDevNames[1] = "DIGI_IN:SLOT_BASE_IO:DIGI_IN_2";
    szDinDevNames[2] = "DIGI_IN:SLOT_BASE_IO:DIGI_IN_3";
    szDinDevNames[3] = "DIGI_IN:SLOT_BASE_IO:DIGI_IN_4";
    szDinDevNames[4] = "DIGI_IN:SLOT_BASE_IO:DIGI_IN_5";
    szDinDevNames[5] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_1";
    szDinDevNames[6] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_2";
    szDinDevNames[7] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_3";
    szDinDevNames[8] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_4";
    szDinDevNames[9] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_5";
    szDinDevNames[10] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_6";
    szDinDevNames[11] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_7";
    szDinDevNames[12] = "DIGI_IN:EXP_SLOT_1:DIGI_IN_8";
    szDinDevNames[13] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_1";
    szDinDevNames[14] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_2";
    szDinDevNames[15] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_3";
    szDinDevNames[16] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_4";
    szDinDevNames[17] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_5";
    szDinDevNames[18] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_6";
    szDinDevNames[19] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_7";
    szDinDevNames[20] = "DIGI_IN:EXP_SLOT_2:DIGI_IN_8";
    szDinDevNames[21] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_1";
    szDinDevNames[22] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_2";
    szDinDevNames[23] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_3";
    szDinDevNames[24] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_4";
    szDinDevNames[25] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_5";
    szDinDevNames[26] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_6";
    szDinDevNames[27] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_7";
    szDinDevNames[28] = "DIGI_IN:EXP_SLOT_3:DIGI_IN_8";
    szDinDevNames[29] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_1";
    szDinDevNames[30] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_2";
    szDinDevNames[31] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_3";
    szDinDevNames[32] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_4";
    szDinDevNames[33] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_5";
    szDinDevNames[34] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_6";
    szDinDevNames[35] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_7";
    szDinDevNames[36] = "DIGI_IN:EXP_SLOT_4:DIGI_IN_8";
  };
  
  static CDigitalIn obDin[NR_DIN_CHANNELS];
  
  int nDinFunc = 0;
  unsigned int unCh = 0;
  unsigned int unValue = 0;
  int nValue;
  BOOL bValue;

  int nStartDICh = 0;
  int nEndDICh = 0;
  if( g_b370XAIOBoards )
  {
    nStartDICh = 0;
    nEndDICh = 3;
  }
  else
  {
    int nDISlot = 0;
    printf("Enter DI slot -\n 1. BaseIO\n 2. rocio1\n 3. rocio2\n 4. rocio3\n 5. rocio4\n - ");
    fflush(stdin);
    scanf("%d", &nDISlot);
    switch( nDISlot )
    {
    case 1:
      nStartDICh = 0;
      nEndDICh = 4;
      break;
    case 2:
      nStartDICh = 5;
      nEndDICh = 12;
      break;
    case 3:
      nStartDICh = 13;
      nEndDICh = 20;
      break;
    case 4:
      nStartDICh = 21;
      nEndDICh = 28;
      break;
    case 5:
      nStartDICh = 29;
      nEndDICh = 36;
      break;
    default:
      printf("Entered DI slot is invalid!!!");
      return;
    }
  }

  for (int nDins = nStartDICh; nDins <= nEndDICh; nDins++)
  {
    nRetVal = obDin[nDins].OpenHal(szDinDevNames[nDins]);
    
    if (nRetVal < 0)
    {
      printf("Error %d opening Digital IN Channel: %d\n", nRetVal, nDins + 1);
      return;
    }
  }
  
  while (g_nExitApp != 1)
  {
    printf("Enter Digital IN channel number (%d to %d): \n", nStartDICh+1, nEndDICh+1);
    fflush(stdin);
    scanf("%d", &unCh);

    if ( (unCh < (unsigned int)(nStartDICh+1)) || (unCh > (unsigned int)(nEndDICh+1)) )
    {
      printf("Invalid value... try again.\n");
      continue;
    }
    unCh--;

    if (g_nExitApp)
    {
      break;
    }

    printf("Select Digital IN function:\n");
    printf("  %d. Get Digital In status.\n", DIN_FUNC_GET_DIN);
    printf("  %d. Get Digital In status continuously.\n", DIN_FUNC_GET_DIN_CONT);
    printf("  %d. Get board temperature.\n", DIN_FUNC_GET_BRD_TEMP);
    printf("  %d. Get board information.\n", DIN_BOARD_INFO);
    printf("  %d. Exit application.\n", DIN_FUNC_EXIT);
    fflush(stdin);
    scanf("%d", &nDinFunc);

    if ( (nDinFunc == DIN_FUNC_EXIT) || (g_nExitApp))
    {
      break;
    }

    if ( (nDinFunc != DIN_FUNC_GET_DIN) && (nDinFunc != DIN_BOARD_INFO) && 
         (nDinFunc != DIN_FUNC_GET_BRD_TEMP) && (nDinFunc != DIN_FUNC_GET_DIN_CONT) )
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


    switch (nDinFunc)
    {
    case DIN_FUNC_GET_DIN:
      if ( (nRetVal = obDin[unCh].GetDigitalInStatus(&bValue)) < 0)
      {
        printf("CDigitalIn[%d].GetDigitalInStatus() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("Digital In Status: %s\n", ( (bValue == FALSE) ? "LOW" : "HIGH" ) );
      }
      break;
    case DIN_FUNC_GET_DIN_CONT:
      while( g_nExitApp != 1 )
      {
        for( int nInd =nStartDICh; nInd <=nEndDICh; nInd++ )
        {
          if( (nRetVal = obDin[nInd].GetDigitalInStatus(&bValue)) < 0 )
          {
            printf( "Fail(%d) - Err:%d,", nInd+1, nRetVal );
          }
          else
          {
            printf( "%s,", (bValue == TRUE? "1": "0"));
          }
        }
        printf( "\n" );

        //Use the user specified time interval b/n Digital Input reads....
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
      break;
    case DIN_FUNC_GET_BRD_TEMP:
      if ( (nRetVal = obDin[unCh].GetOnBoardTemp(&nValue)) < 0)
      {
        printf("CDigitalIn[%d].GetChDiffGain() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("Board Temperature: %f\n", (float)nValue/1000);
      }
      break;

    case DIN_BOARD_INFO:
    {
      unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
      if ( (nRetVal = obDin[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
      {
        printf("CDigitalIn[%d].GetBoardInfo() failed: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
        printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
        printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
        printf("Board Revision : %d\n", byBoardRev);
      }
    }
    break;

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
    
  }
  
  for (int nDins = nStartDICh; nDins < nEndDICh+1; nDins++)
  {
    nRetVal = obDin[nDins].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("CDigitalIn[%d].Close() failed: %d\n", nDins + 1, nRetVal);
    }
  }

}

#define NR_DOT_CHANNELS       25

#define DOT_FUNC_SET_DOT      1
#define DOT_FUNC_GET_BRD_TEMP 2
#define DOT_BOARD_INFO        3
#define DOT_FUNC_EXIT         4

void TestDigitalOut()
{
  int nRetVal = 0;
  
  static char *szDotDevNames[NR_DOT_CHANNELS] = {NULL,NULL,NULL,NULL,NULL};
  int nNumDigitalOutChannels = NR_DOT_CHANNELS;
  if (g_b370XAIOBoards)
  {
    nNumDigitalOutChannels = 1;
    szDotDevNames[0] = "DIGI_OUT:ANALYZER_SLOT:DIGI_OUT_1";
  }
  else
  {
    szDotDevNames[0] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_1";
    szDotDevNames[1] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_2";
    szDotDevNames[2] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_3";
    szDotDevNames[3] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_4";
    szDotDevNames[4] = "DIGI_OUT:SLOT_BASE_IO:DIGI_OUT_5";
    szDotDevNames[5] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_1";
    szDotDevNames[6] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_2";
    szDotDevNames[7] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_3";
    szDotDevNames[8] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_4";
    szDotDevNames[9] = "DIGI_OUT:EXP_SLOT_1:DIGI_OUT_5";
    szDotDevNames[10] = "DIGI_OUT:EXP_SLOT_2:DIGI_OUT_1";
    szDotDevNames[11] = "DIGI_OUT:EXP_SLOT_2:DIGI_OUT_2";
    szDotDevNames[12] = "DIGI_OUT:EXP_SLOT_2:DIGI_OUT_3";
    szDotDevNames[13] = "DIGI_OUT:EXP_SLOT_2:DIGI_OUT_4";
    szDotDevNames[14] = "DIGI_OUT:EXP_SLOT_2:DIGI_OUT_5";
    szDotDevNames[15] = "DIGI_OUT:EXP_SLOT_3:DIGI_OUT_1";
    szDotDevNames[16] = "DIGI_OUT:EXP_SLOT_3:DIGI_OUT_2";
    szDotDevNames[17] = "DIGI_OUT:EXP_SLOT_3:DIGI_OUT_3";
    szDotDevNames[18] = "DIGI_OUT:EXP_SLOT_3:DIGI_OUT_4";
    szDotDevNames[19] = "DIGI_OUT:EXP_SLOT_3:DIGI_OUT_5";
    szDotDevNames[20] = "DIGI_OUT:EXP_SLOT_4:DIGI_OUT_1";
    szDotDevNames[21] = "DIGI_OUT:EXP_SLOT_4:DIGI_OUT_2";
    szDotDevNames[22] = "DIGI_OUT:EXP_SLOT_4:DIGI_OUT_3";
    szDotDevNames[23] = "DIGI_OUT:EXP_SLOT_4:DIGI_OUT_4";
    szDotDevNames[24] = "DIGI_OUT:EXP_SLOT_4:DIGI_OUT_5";
  };
  
  static CDigitalOut obDot[NR_DOT_CHANNELS];

  int nDotFunc = 0;
  unsigned int unCh = 0;
  unsigned int unValue = 0;
  int nValue;
  BOOL bValue;

  int nStartDOCh = 0;
  int nEndDOCh = 0;

  if( g_b370XAIOBoards )
  {
    nStartDOCh = 0;
    nEndDOCh = 0;
  }
  else
  {
    int nDOSlot = 0;
    printf("Enter DO slot -\n 1. BaseIO\n 2. rocio1\n 3. rocio2\n 4. rocio3\n 5. rocio4\n - ");
    fflush(stdin);
    scanf("%d", &nDOSlot);
    switch( nDOSlot )
    {
    case 1:
      nStartDOCh = 0;
      nEndDOCh = 4;
      break;
    case 2:
      nStartDOCh = 5;
      nEndDOCh = 9;
      break;
    case 3:
      nStartDOCh = 10;
      nEndDOCh = 14;
      break;
    case 4:
      nStartDOCh = 15;
      nEndDOCh = 19;
      break;
    case 5:
      nStartDOCh = 20;
      nEndDOCh = 24;
      break;
    default:
      printf("Entered DI slot is invalid!!!");
      return;
    }
  }

  for (int nDots = nStartDOCh; nDots <= nEndDOCh; nDots++)
  {
    nRetVal = obDot[nDots].OpenHal(szDotDevNames[nDots]);

    if (nRetVal < 0)
    {
      printf("Error %d opening Digital OUT Channel: %d\n", nRetVal, nDots + 1);
      return;
    }
  }


  while (g_nExitApp != 1)
  {
    if (!g_b370XAIOBoards)
    {
      printf("Enter Digital OUT channel number (%d to %d): \n", nStartDOCh + 1, nEndDOCh + 1);
      fflush(stdin);
      scanf("%d", &unCh);

      if ((unCh < (unsigned int)(nStartDOCh + 1)) || (unCh > (unsigned int)(nEndDOCh + 1)) )
      {
        printf("Invalid value... try again.\n");
        continue;
      }
      unCh--;
    }
    else
      unCh = 0;

    if (g_nExitApp)
      break;

    printf("Select Digital OUT function:\n");
    printf("  %d. Set Digital OUT status.\n", DOT_FUNC_SET_DOT);
    printf("  %d. Get board temperature.\n", DOT_FUNC_GET_BRD_TEMP);
    printf("  %d. Get board information.\n", DOT_BOARD_INFO);
    printf("  %d. Exit application.\n", DOT_FUNC_EXIT);
    fflush(stdin);
    scanf("%d", &nDotFunc);

    if ((nDotFunc == DOT_FUNC_EXIT) || g_nExitApp)
      break;

    if ((nDotFunc != DOT_BOARD_INFO) && (nDotFunc != DOT_FUNC_GET_BRD_TEMP))
    {
      printf("Enter value: ");
      fflush(stdin);
      scanf("%d", &unValue);
    }

    printf("\n");
    
    if (g_nExitApp)
      break;

    switch (nDotFunc)
    {
    case DOT_FUNC_SET_DOT:
      if (unValue)
        bValue = TRUE;
      else
        bValue = FALSE;
      if ((nRetVal = obDot[unCh].SetDigitalOutStatus(bValue)) < 0)
        printf("CDigitalOut[%d].SetDigitalOutStatus() failed.: %d\n", unCh + 1, nRetVal);
      break;

    case DOT_FUNC_GET_BRD_TEMP:
      if ((nRetVal = obDot[unCh].GetOnBoardTemp(&nValue)) < 0)
        printf("CDigitalOut[%d].GetOnBoardTemp() failed.: %d\n", unCh + 1, nRetVal);
      else
        printf("Board Temperature: %f\n", (float)nValue/1000);
      break;

    case DOT_BOARD_INFO:
    {
      unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
      if ( (nRetVal = obDot[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
      {
        printf("CDigitalOut[%d].GetBoardInfo() failed: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
        printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
        printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
        printf("Board Revision : %d\n", byBoardRev);
      }
    }
    break;

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
    
  }
  
  for (int nDots = nStartDOCh; nDots < nEndDOCh+1; nDots++)
  {
    nRetVal = obDot[nDots].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("CDigitalOut[%d].Close() failed: %d\n", nDots + 1, nRetVal);
    }
  }

}

#define NR_EPC_CHANNELS        5

#define EPC_FUNC_SET_PRES_MV   1
#define EPC_FUNC_GET_PRES_MV   2
#define EPC_FUNC_GET_BRD_TEMP  3
#define EPC_BOARD_INFO         4

#ifdef MODEL_370XA
#define EPC_FUNC_SET_PWM       5
#define EPC_FUNC_GET_PWM       6
#define EPC_FUNC_SET_PROP      7
#define EPC_FUNC_SET_INT       8
#define EPC_FUNC_SET_DIFF      9
#define EPC_FUNC_GET_PROP      10
#define EPC_FUNC_GET_INT       11
#define EPC_FUNC_GET_DIFF      12
#define EPC_FUNC_SET_COMP_BASE 13
#define EPC_FUNC_SET_COMP_CORR 14
#define EPC_FUNC_EXIT          15
#else //#ifdef MODEL_370XA
#define EPC_FUNC_EXIT          5
#endif //#ifdef MODEL_370XA


void TestEPC()
{
  int nRetVal = 0;
  
  static char * szEPCDevNames[NR_EPC_CHANNELS] = { NULL,NULL,NULL,NULL,NULL };
  int nNumEPCChannels = NR_EPC_CHANNELS;

  printf ("size = %d\n", sizeof (BASEIO_DATA_STRUCT));

  if( g_b370XAIOBoards )
  {
    nNumEPCChannels = 1;
    szEPCDevNames[0] = "EPC:ANALYZER_SLOT:EPC_1";
  }
  else
  {
    szEPCDevNames[0] = "EPC:SLOT_BASE_IO:EPC_1";
    szEPCDevNames[1] = "EPC:SLOT_BASE_IO:EPC_2";
    szEPCDevNames[2] = "EPC:SLOT_BASE_IO:EPC_3";
    szEPCDevNames[3] = "EPC:SLOT_BASE_IO:EPC_4";
    szEPCDevNames[4] = "EPC:SLOT_BASE_IO:EPC_5";
  };
  
  static CEPC obEPC[NR_EPC_CHANNELS];

  int nEPCFunc = 0;
  unsigned int unCh = 0;
  unsigned long ulValue = 0;
  int nValue;
  

#ifdef MODEL_370XA
  unsigned int unValue = 0;
  float fValue = 0.0;
#endif //#ifdef MODEL_370XA

  for (int nEPCs = 0; nEPCs < nNumEPCChannels; nEPCs++)
  {
    nRetVal = obEPC[nEPCs].OpenHal(szEPCDevNames[nEPCs]);
    
    if (nRetVal < 0)
    {
      printf("Error %d opening EPC Channel: %d\n", nRetVal, nEPCs + 1);
      return;
    }
  }
  
  while (g_nExitApp != 1)
  {
    if( ! g_b370XAIOBoards )
    {
      printf("Enter EPC channel number (1 to 5): \n");
      fflush(stdin);
      scanf("%d", &unCh);

      if ( (unCh < 1) || (unCh > (unsigned int)nNumEPCChannels) )
      {
        printf("Invalid value... try again.\n");
        continue;
      }
      unCh--;
    }
    else
    {
      unCh = 0;
    }

    if (g_nExitApp)
    {
      break;
    }

    printf("Select EPC function:\n");
    printf("  %d. Set Pressure Milli Volt.\n", EPC_FUNC_SET_PRES_MV);
    printf("  %d. Get Pressure Milli Volt.\n", EPC_FUNC_GET_PRES_MV);
    printf("  %d. Get board temperature.\n", EPC_FUNC_GET_BRD_TEMP);
    printf("  %d. Get board information.\n", EPC_BOARD_INFO);
#ifdef MODEL_370XA
    printf("  %d. Set challel PWM (percent).\n", EPC_FUNC_SET_PWM);
    printf("  %d. Get channel PWM (percent).\n", EPC_FUNC_GET_PWM);
    printf("  %d. Set proportional gain.\n", EPC_FUNC_SET_PROP);
    printf("  %d. Set integral gain.\n", EPC_FUNC_SET_INT);
    printf("  %d. Set differential gain.\n", EPC_FUNC_SET_DIFF);
    printf("  %d. Get proportional gain.\n", EPC_FUNC_GET_PROP);
    printf("  %d. Get integral gain.\n", EPC_FUNC_GET_INT);
    printf("  %d. Get differential gain.\n", EPC_FUNC_GET_DIFF);
    printf("  %d. Set Temperature Compensation Base Temperature.\n", EPC_FUNC_SET_COMP_BASE);
    printf("  %d. Set Temperature Compensation Pressure Correction.\n", EPC_FUNC_SET_COMP_CORR);
#endif //#ifdef MODEL_370XA

    printf("  %d. Exit application.\n", EPC_FUNC_EXIT);
    fflush(stdin);
    scanf("%d", &nEPCFunc);

    if ( (nEPCFunc == EPC_FUNC_EXIT) || (g_nExitApp))
    {
      break;
    }

    if ( (nEPCFunc != EPC_BOARD_INFO) && 
         (nEPCFunc != EPC_FUNC_GET_BRD_TEMP) && 

#ifdef MODEL_370XA
         (nEPCFunc != EPC_FUNC_GET_PWM) && 
         (nEPCFunc != EPC_FUNC_GET_PROP) && 
         (nEPCFunc != EPC_FUNC_GET_INT) && 
         (nEPCFunc != EPC_FUNC_GET_DIFF) && 
#endif  //#ifdef MODEL_370XA

         (nEPCFunc != EPC_FUNC_GET_PRES_MV) )
    {
      printf("Enter value: ");
      fflush(stdin);
#ifdef MODEL_370XA
      if (nEPCFunc == EPC_FUNC_SET_COMP_BASE ||
          nEPCFunc == EPC_FUNC_SET_COMP_CORR)
        scanf("%f", &fValue);
      else
#endif // #ifdef MODEL_370XA
        scanf("%lu", &ulValue);
    }

    printf("\n");
    
    if (g_nExitApp)
    {
      break;
    }


    switch (nEPCFunc)
    {
    case EPC_FUNC_SET_PRES_MV:
      if ( (nRetVal = obEPC[unCh].SetPressure(ulValue)) < 0)
      {
        printf("CEPC[%d].SetPressure() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;
    
    case EPC_FUNC_GET_PRES_MV:
      if ( (nRetVal = obEPC[unCh].GetPressure(&ulValue)) < 0)
      {
        printf("CEPC[%d].SetPressure() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("EPC channel %d, pressure (mV): %lu\n", unCh + 1, ulValue);
      }
      break;

      
    case EPC_FUNC_GET_BRD_TEMP:
      if ( (nRetVal = obEPC[unCh].GetOnBoardTemp(&nValue)) < 0)
      {
        printf("CEPC[%d].GetOnBoardTemp() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("Board Temperature: %f\n", (float)nValue/1000);
      }
      break;

    case EPC_BOARD_INFO:
    {
      unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
      if ( (nRetVal = obEPC[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
      {
        printf("CEPC[%d].GetBoardInfo() failed: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
        printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
        printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
        printf("Board Revision : %d\n", byBoardRev);
      }
    }
    break;
#ifdef MODEL_370XA
    case EPC_FUNC_SET_PWM:
      if ( (nRetVal = obEPC[unCh].SetPWMMilliP(ulValue * 1000)) < 0)
      {
        printf("CEPC[%d].SetPWMMilliP() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;
    case EPC_FUNC_GET_PWM:
      if ( (nRetVal = obEPC[unCh].GetPWMMilliP(&unValue)) < 0)
      {
        printf("CEPC[%d].GetPWMMilliP() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("EPC %d PWM percent: %d\n", unCh + 1, unValue/1000);
      }
      break;
    case EPC_FUNC_SET_PROP:
      if ( (nRetVal = obEPC[unCh].SetPropGain(ulValue)) < 0)
      {
        printf("CEPC[%d].SetPropGain() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;
    case EPC_FUNC_SET_INT:
      if ( (nRetVal = obEPC[unCh].SetIntGain(ulValue)) < 0)
      {
        printf("CEPC[%d].SetIntpGain() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;
    case EPC_FUNC_SET_DIFF:
      if ( (nRetVal = obEPC[unCh].SetDiffGain(ulValue)) < 0)
      {
        printf("CEPC[%d].SetDiffGain() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;
    case EPC_FUNC_GET_PROP:
      if ( (nRetVal = obEPC[unCh].GetPropGain(&ulValue)) < 0)
      {
        printf("CEPC[%d].GetPropGain() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("EPC %d Prop Gain: %ld\n", unCh + 1, ulValue);
      }
      break;
    case EPC_FUNC_GET_INT:
      if ( (nRetVal = obEPC[unCh].GetIntGain(&ulValue)) < 0)
      {
        printf("CEPC[%d].GetIntGain() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("EPC %d Int Gain: %ld\n", unCh + 1, ulValue);
      }
      break;
    case EPC_FUNC_GET_DIFF:
      if ( (nRetVal = obEPC[unCh].GetDiffGain(&ulValue)) < 0)
      {
        printf("CEPC[%d].GetDiffGain() failed.: %d\n", unCh + 1, nRetVal);
      }
      else
      {
        printf("EPC %d Diff Gain: %ld\n", unCh + 1, ulValue);
      }
      break;
    
    case EPC_FUNC_SET_COMP_BASE:
      if ( (nRetVal = obEPC[unCh].SetPressCompBaseTemp(fValue)) < 0)
      {
        printf("CEPC[%d].SetPressCompBaseTemp() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;
      
    case EPC_FUNC_SET_COMP_CORR:
      if ( (nRetVal = obEPC[unCh].SetPressCompCorr(fValue)) < 0)
      {
        printf("CEPC[%d].SetPressCompCorr() failed.: %d\n", unCh + 1, nRetVal);
      }
      break;
#endif //#ifdef MODEL_370XA

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
    
  }
  
  for (int nEPCs = 0; nEPCs < nNumEPCChannels; nEPCs++)
  {
    nRetVal = obEPC[nEPCs].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("CEPC[%d].Close() failed: %d\n", nEPCs + 1, nRetVal);
    }
  }

}

void TestEPCReadBack()
{
  static char szEPCDevNames[NR_EPC_CHANNELS][50] = {
    "EPC:SLOT_BASE_IO:EPC_1",
    "EPC:SLOT_BASE_IO:EPC_2",
    "EPC:SLOT_BASE_IO:EPC_3",
    "EPC:SLOT_BASE_IO:EPC_4",
    "EPC:SLOT_BASE_IO:EPC_5",
  };
  
  static CEPC obEPC[NR_EPC_CHANNELS];
  int nRetVal = 0;
  unsigned long ulValue = 0;

  for (int nEPCs = 0; nEPCs < NR_EPC_CHANNELS; nEPCs++)
  {
    nRetVal = obEPC[nEPCs].OpenHal(szEPCDevNames[nEPCs]);
    
    if (nRetVal < 0)
    {
      printf("Error %d opening EPC Channel: %d\n", nRetVal, nEPCs + 1);
      return;
    }
  }
  
  while (g_nExitApp != 1)
  {
    // Print date / time here
    time_t curTime = time(NULL);
    struct tm *tm_curr = localtime( &curTime );
    
    // Print time when we acquired data...
    printf("%02d-%02d-%04d %02d:%02d:%02d,", 
           tm_curr->tm_mon+1, tm_curr->tm_mday, tm_curr->tm_year + 1900,
           tm_curr->tm_hour, tm_curr->tm_min, tm_curr->tm_sec);

    for (int nCh = 0; nCh < NR_EPC_CHANNELS; ++nCh)
    {
      // Print EPC data here...
      if ( (nRetVal = obEPC[nCh].GetPressure(&ulValue)) < 0)
      {
        printf("CEPC[%d].GetPressure() failed.: %d\n", nCh + 1, nRetVal);
        g_nExitApp = 1;
      }
      else
      {
        printf("%lu,", ulValue);
      }
    }     
    printf ("\n");
    
    // Sleep here
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
  
  // Close EPC connections here...  
  for (int nEPCs = 0; nEPCs < NR_EPC_CHANNELS; nEPCs++)
  {
    nRetVal = obEPC[nEPCs].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("CEPC[%d].Close() failed: %d\n", nEPCs + 1, nRetVal);
    }
  }
}




#define NR_LTL_CHANNELS       1

#define LTL_FUNC_SET_CLK_BITS 1
#define LTL_FUNC_SET_BYTE     2
#define LTL_FUNC_SET_FLASH    3
#define LTL_SET_SCAN          4
#define LTL_SET_STREAM        5
#define LTL_GET_BYTE          6
#define LTL_GET_STREAM_BLOCK  7
#define LTL_FUNC_GET_BRD_TEMP 8
#define LTL_BOARD_INFO        9
#define LTL_FUNC_EXIT         10


void TestLtloi()
{
  static char szLtlDevNames[NR_LTL_CHANNELS][50] = {
    "LTLOI:SLOT_BASE_IO:LTLOI_1",
  };
  
  static CLtLoi obLtl[NR_LTL_CHANNELS];
  
  int nRetVal = 0;

  int nLtlFunc = 0;

  unsigned int unCh = 0;
  int ByteNum;
  int ByteVal;
  int nValue;
  unsigned char UCharVal = 0;
  unsigned char UCharNum = 0;

  for (int nLtls = 0; nLtls < NR_LTL_CHANNELS; nLtls++)
  {
    nRetVal = obLtl[nLtls].OpenHal(szLtlDevNames[nLtls]);
    
    if (nRetVal < 0)
    {
      printf("Error %d opening LTLOI Channel: %d\n", nRetVal, nLtls + 1);
      return;
    }
  }
  
  while (g_nExitApp != 1)
  {
    unCh = 0;

    if (g_nExitApp)
    {
      break;
    }

    printf("Select LTLOI function:\n");
    printf("  %d. Set LTLOI number of clock bits.\n", LTL_FUNC_SET_CLK_BITS);
    printf("  %d. Set LTLOI byte.\n", LTL_FUNC_SET_BYTE);
    printf("  %d. Set LTLOI flash.\n", LTL_FUNC_SET_FLASH);
    printf("  %d. Set LTLOI scan.\n", LTL_SET_SCAN);
    printf("  %d. Set LTLOI stream.\n", LTL_SET_STREAM);
    printf("  %d. Get LTLOI byte.\n", LTL_GET_BYTE);
    printf("  %d. Get LTLOI stream (blocking).\n", LTL_GET_STREAM_BLOCK);
    printf("  %d. Get board temperature.\n", LTL_FUNC_GET_BRD_TEMP);
    printf("  %d. Get board information.\n", LTL_BOARD_INFO);
    printf("  %d. Exit application.\n", LTL_FUNC_EXIT);
    fflush(stdin);
    scanf("%d", &nLtlFunc);
    getchar();
    
    if ( (nLtlFunc == LTL_FUNC_EXIT) || (g_nExitApp))
    {
      break;
    }

    switch (nLtlFunc)
    {
      break;
    }

    if (g_nExitApp)
    {
      break;
    }


    switch (nLtlFunc)
    {
    case LTL_FUNC_SET_CLK_BITS:
      printf("Enter number of clock bits\n");
      scanf("%d", &nValue);
      getchar();
      if ( (nRetVal = obLtl[unCh].SetNumClkBits((unsigned char)nValue)) < 0)
      {
        printf("CLtLoi::SetNumClkBits failed.: %d\n", nRetVal);
      }
      break;
      
    case LTL_FUNC_SET_BYTE:
      printf("Enter byte value\n");
      scanf("%d", &ByteVal);
      getchar();
      printf("Enter byte number\n");
      scanf("%d", &ByteNum);
      getchar();
      if ( (nRetVal = obLtl[unCh].SetOutputByte((unsigned char)ByteVal, (unsigned char)ByteNum)) < 0)
      {
        printf("CLtLoi::SetOutputByte failed.: %d\n", nRetVal);
      }
      break;
    case LTL_FUNC_SET_FLASH:
      printf("Enter byte value (bit positions to flash)\n");
      scanf("%d", &ByteVal);
      getchar();
      printf("Enter byte number\n");
      scanf("%d", &ByteNum);
      getchar();
      if ( (nRetVal = obLtl[unCh].SetOutputFlash((unsigned char)ByteVal, (unsigned char)ByteNum)) < 0)
      {
        printf("CLtLoi::SetOutputFlash failed.: %d\n", nRetVal);
      }
      break;
    case LTL_SET_SCAN:
      printf("Enter scan status (zero: off, non-sero: on)\n");
      scanf("%d", &nValue);
      getchar();
      if (nValue)
      {
        nValue = 1;
      }
      if ( (nRetVal = obLtl[unCh].ScanCtrl((LTLOI_APP_STATUS_ENUM)nValue)) < 0)
      {
        printf("CLtLoi::ScanCtrl failed.: %d\n", nRetVal);
      }
      break;

    case LTL_SET_STREAM:
      printf("Enter stream status (zero: off, non-sero: on)\n");
      scanf("%d", &nValue);
      getchar();
      
      if (nValue)
      {
        nValue = 1;
      }
      
      if ( (nRetVal = obLtl[unCh].StreamingState((LTLOI_APP_STATUS_ENUM)nValue)) < 0)
      {
        printf("CLtLoi::StreamingState failed.: %d\n", nRetVal);
      }
      break;

    case LTL_GET_BYTE:
      printf("Enter byte number\n");
      scanf("%d", &ByteNum);
      getchar();
      
      if ( (nRetVal = obLtl[unCh].ReadInputByte(&UCharVal, (unsigned char)ByteNum)) < 0)
      {
        printf("CLtLoi::ReadInputByte failed.: %d\n", nRetVal);
      }
      else
      {
        printf("ByteVal[%d] = %d\n", ByteNum, UCharVal);
      }
      break;

    case LTL_GET_STREAM_BLOCK:
      if ( (nRetVal = obLtl[unCh].RxStreamingByteBlocking(&UCharVal, &UCharNum)) < 0)
      {
        printf("CLtLoi::RxStreamingByteBlocking failed.: %d\n", nRetVal);
      }
      else
      {
        printf("ByteVal[%d] = %d\n", UCharNum, UCharVal);
      }
      break;
  
    case LTL_FUNC_GET_BRD_TEMP:
      if ( (nRetVal = obLtl[unCh].GetOnBoardTemp(&nValue)) < 0)
      {
        printf("CLtLoi::GetOnBoardTemp() failed.: %d\n", nRetVal);
      }
      else
      {
        printf("Board Temperature: %f\n", (float)nValue/1000);
      }
      break;

    case LTL_BOARD_INFO:
    {
      unsigned char byFirmwareVerMaj, byFirmwareVerMin, byFirmwareVerBuild, byBoardRev;
      if ( (nRetVal = obLtl[unCh].GetBoardInfo(&byFirmwareVerMaj, &byFirmwareVerMin, &byFirmwareVerBuild, &byBoardRev)) < 0)
      {
        printf("CLtLoi::GetBoardInfo() failed: %d\n", nRetVal);
      }
      else
      {
        printf("Firmware Major No.: %d\n", byFirmwareVerMaj);
        printf("Firmware Minor No.: %d\n", byFirmwareVerMin);
        printf("Firmware Build No.: %d\n", byFirmwareVerBuild);
        printf("Board Revision : %d\n", byBoardRev);
      }
    }
    break;

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }
    
  }
  
  for (int nLtls = 0; nLtls < NR_LTL_CHANNELS; nLtls++)
  {
    nRetVal = obLtl[nLtls].CloseHal();
      
    if (nRetVal < 0)
    {
      printf("CLtLoi.Close() failed: %d\n", nRetVal);
    }
  }

}



void PrintHelp()
{
  printf("Application usage\n");
  printf("<app_name> -m <app_mode> -n <value> -c (Pre configure) -v (Verbose - optional)\n");

  printf("-m <app_mode>:\n");
  printf("  Z: Track keylock switch state changes\n");
  printf("  z: Keylock loopback test with ROC DO\n");
  printf("  s: Test Solenoid\n");
  printf("  t: Test RTD\n");
  printf("  x: Read Temperatures and PWM (5 RTDs, 4 heaters). Kill xpheaterd before running this.\n");
  printf("  h: Test Heater Control\n");
  printf("  p: Test Preamp - streaming\n");
  printf("  u: Serial IO\n");
  printf("  i: Digital IN\n");
  printf("  o: Digital OUT\n");
  printf("  I: Digital IN & OUT\n");
  printf("  e: EPC\n");
  printf("  r: Analog IN\n");
  printf("  w: Analog OUT\n");
  printf("  W: Analog IN & OUT\n");
  printf("  l: LTLOI\n");
  printf("  E: EPC Readback\n");
  printf("  f: Send GC status to FFB Board\n");
  printf("  F: Listen FFB Board commands\n");
  printf("  d: Retrive diagnostic details\n");
  printf("  D: Retrieve continuous diagnostic details\n");
  printf("  c: Test FID\n");
  printf("  a: Test CPU ADCs\n");
  printf("  b: Test FPD\n");
  printf("  k: Test IR Keypad\n");
  printf("  m: Test Sample Pressure\n");
  printf("  n: Test Serial Mode Control\n");
  printf("  g: Test IMB Communication\n");
  printf("  j: Test FPD G2\n");
  printf("-n <value> (Optional):\n");
  printf("  Channel No.: 0 to 1 when 'app_mode' is p (Preamp)\n");
  printf("  Channel No.: 0 to 2 when 'app_mode' is u (Serial)\n");
  printf("  Time interval: In milliseconds, b/n RTD channel reads when 'app_mode' is t (RTD)\n");
  printf("  FID/FPD No.: 0 = BaseIO, 1 = backplane, when 'app_mode' is c(FID) or b(FPD)\n");
  printf("  Slot No.: 0 to 1 when 'app_mode' is j (FPD G2)\n");
  printf("  Not valid for other modes.\n");
  printf("-c (Pre configure):\n");
  printf("  Optional pre-configure command line switch. If any of the\n");
  printf("  test modes provide an option to configure certain device\n");
  printf("  settings before beginning the test cycle, specifying this\n");
  printf("  switch will present the user with the configuration\n");
  printf("  options. Not specifying this switch will directly run\n");
  printf("  the requested test.\n");
  printf("  Valid for <app_mode> = p (Preamp) or s (Solenoid)\n");
  printf("-a (Test DI,DO,AI,EPC against 370XA IO boards)\n");
  printf("  Not specifying this switch will default to 700XA IO boards.\n");
  printf("-v (Verbose)\n");
  printf("  Not specifying this switch will disable the display of some messages during application execution.\n");
  
}

//////////////////////////////////////////////////////////////////////
// 
//   TEST IMB FUNCTIONS
// 
//////////////////////////////////////////////////////////////////////



#define IMB_FUNC_EXIT                        0
#define IMB_FUNC_SET_SECTION_META_INFO       1
#define IMB_FUNC_GET_SECTION_META_INFO       2
#define IMB_FUNC_SET_SECTION_AVT_SET_TABLE   3
#define IMB_FUNC_GET_SECTION_AVT_SET_TABLE   4
#define IMB_FUNC_SET_SECTION_AVT_VSEL_TABLE  5
#define IMB_FUNC_GET_SECTION_AVT_VSEL_TABLE  6
#define IMB_FUNC_SET_SECTION_AVT_TEADJ_TABLE 7
#define IMB_FUNC_GET_SECTION_AVT_TEADJ_TABLE 8
#define IMB_FUNC_SET_SECTION_CDT_CONFIG      9
#define IMB_FUNC_GET_SECTION_CDT_CONFIG      10
//#define IMB_FUNC_SET_SECTION_CDT_UPDATE      11
//#define IMB_FUNC_GET_SECTION_CDT_UPDATE      12
#define IMB_FUNC_SET_SECTION_HTEV_TABLE      13
#define IMB_FUNC_GET_SECTION_HTEV_TABLE      14
#define IMB_FUNC_SET_SECTION_STEV_TABLE      15
#define IMB_FUNC_GET_SECTION_STEV_TABLE      16
#define IMB_FUNC_SET_SECTION_GTEV_TABLE      17
#define IMB_FUNC_GET_SECTION_GTEV_TABLE      18
#define IMB_FUNC_SET_SECTION_ANLY_CFG_TABLE  19
#define IMB_FUNC_GET_SECTION_ANLY_CFG_TABLE  20
#define IMB_FUNC_SET_SECTION_SET_POINTS      21
#define IMB_FUNC_GET_SECTION_SET_POINTS      22
#define IMB_FUNC_SET_SECTION_CARRIER_CAL_FAQ 29
#define IMB_FUNC_GET_SECTION_CARRIER_CAL_FAQ 30
//#define IMB_FUNC_SET_SECTION_SAMPLE_CAL_FAQ  31
//#define IMB_FUNC_GET_SECTION_SAMPLE_CAL_FAQ  32
#define IMB_FUNC_SET_SECTION_CUSTOM_NAMES    33
#define IMB_FUNC_GET_SECTION_CUSTOM_NAMES    34
#define IMB_FUNC_SET_SECTION_ALLOWABLE_DEV   35
#define IMB_FUNC_GET_SECTION_ALLOWABLE_DEV   36


// MIRROR
#define IMB_FUNC_SET_MIRROR_AVT_ENABLE       53
#define IMB_FUNC_GET_MIRROR_AVT_ENABLE       54
#define IMB_FUNC_SET_MIRROR_NUM_ANALYSIS     55
#define IMB_FUNC_GET_MIRROR_NUM_ANALYSIS     56

// GLOBAL
#define IMB_FUNC_SET_GLOBAL_DATA_INIT        61
#define IMB_FUNC_GET_GLOBAL_DATA_INIT        62
#define IMB_FUNC_SET_MODULE_SERIAL_NUM       63
#define IMB_FUNC_GET_MODULE_SERIAL_NUM       64
#define IMB_FUNC_SET_GLOBAL_META_INFO        65
#define IMB_FUNC_GET_GLOBAL_META_INFO        66
#define IMB_FUNC_SET_GLOBAL_APP_TYPE         67
#define IMB_FUNC_GET_GLOBAL_APP_TYPE         68


#define IMB_FUNC_GET_DB_ID                   91
#define IMB_FUNC_GET_IMB_INFO                92

#define IMB_FUNC_SET_ALL_DATA_SECTIONS       93
#define IMB_FUNC_GET_ALL_DATA_SECTIONS       94
#define IMB_FUNC_UPDATE_FLASH                99

#define IMB_FUNC_SET_FLASH_IMAGE             100
#define IMB_FUNC_GET_FLASH_IMAGE             101

#define IMB_FUNC_RESET_ALL_DATA_SECTIONS     102


void GenerateRandomString( unsigned char * szRandString, const int nLength )
{
  static const char szAlphaNumeric[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    ".-/";

  for (int nInd = 0; nInd < nLength-1; ++nInd) {
    szRandString[nInd] = szAlphaNumeric[rand() % (sizeof(szAlphaNumeric) - 1)];
  }

  szRandString[nLength-1] = 0;
}

void DisplayIMBGlobalDataIMBInit( unsigned char byIMBInit )
{
  printf( "Global Data - \n" ); 
  printf( "------------------------------\n" ); 
  /*
    printf( "byIMBSerialNumber (CHAR) = %.*s\n", IMB_SERIAL_NUMBER_LENGTH, stMyGlobalData.byIMBSerialNumber );
    printf( "byIMBSerialNumber (HEX)  = " );
    for( int nInd = 0; nInd < IMB_SERIAL_NUMBER_LENGTH; nInd++ )
    {
    printf( "0x%X ", stMyGlobalData.byIMBSerialNumber[nInd] );
    }
  */
  printf( "\n" );
  printf( "byIMBInit = %d\n", byIMBInit );
  printf( "------------------------------\n" ); 
}

template <class T_IMB_CDT_UPDATE_STRUCT>
void DisplayIMBCDTUpdate2( const T_IMB_CDT_UPDATE_STRUCT & stMyCDTUpdate, const int nMaxRowSz )
{
  printf( "CDT Update - \n" ); 
  printf( "-----------------------------------\n" ); 
  printf( "            nRt                 fRF\n" ); 
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    printf( "%*d", 15, stMyCDTUpdate.nRT[nInd] );
    printf( "%*f", 20, stMyCDTUpdate.fRF[nInd] );
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stMyCDTUpdate.byStoredRows );
  printf( "-----------------------------------\n" ); 
}

template <class T_IMB_CDT_UPDATE_STRUCT>
void PopulateIMBCDTUpdate2( T_IMB_CDT_UPDATE_STRUCT & stCDTUpdate, const int nMaxRowSz )
{
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    stCDTUpdate.nRT[nInd] =  (int) rand();
    stCDTUpdate.fRF[nInd] =  (float) rand() ;
  }
  stCDTUpdate.byStoredRows = (unsigned char) rand();
}

template <class T_IMB_CDT_CONFIG_STRUCT>
void DisplayIMBCDTConfig2( const T_IMB_CDT_CONFIG_STRUCT & stMyCDTConfig, const int nMaxRowSz )
{
  printf( "CDT Configuration - \n" ); 
  printf( "------------------------------------------------\n" ); 
  printf( "      Component Name      sRtDev      sRFPercent\n" ); 
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    printf( "%*d", 20, stMyCDTConfig.szComponent[nInd]);
    printf( "%*d", 12, stMyCDTConfig.sRTDev[nInd]);
    printf( "%*d", 16, stMyCDTConfig.sRFPercent[nInd]);
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stMyCDTConfig.byStoredRows );
  printf( "------------------------------------------------\n" ); 
}

template <class T_IMB_CDT_CONFIG_STRUCT>
void PopulateIMBCDTConfig2( T_IMB_CDT_CONFIG_STRUCT & stCDTConfig, const int nMaxRowSz )
{
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    //GenerateRandomString( stCDTConfig.szComponent[nInd], CDT_COMPONENT_NAME_LENGTH ); 
    stCDTConfig.szComponent[nInd] = (unsigned char) rand();
    stCDTConfig.sRTDev[nInd] = (short) rand();
    stCDTConfig.sRFPercent[nInd] =  (short) rand();
  }
  stCDTConfig.byStoredRows = (unsigned char) rand();
}

void DisplayIMBHwTEVTable( IMB_HW_TEV_TABLE_STRUCT stMyHWTable )
{
  printf( "HW TEV Table - \n" ); 
  printf( "-----------------------------------------------\n" ); 
  printf( "byTEVType     byValveDO     byState       usTime\n" ); 
  for( int nInd = 0; nInd < HW_TEV_TABLE_MAX_ROW_SZ; nInd++ )
  {
    printf( "%*d", 9, stMyHWTable.byTEVType[nInd] );
    printf( "%*d", 14, stMyHWTable.byValveDO[nInd] );
    printf( "%*d", 12, stMyHWTable.byState[nInd] );
    printf( "%*d", 12, stMyHWTable.usTime[nInd] );
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stMyHWTable.byStoredRows );
  printf( "-----------------------------------------------\n" ); 
}

void PopulateIMBHwTEVTable( IMB_HW_TEV_TABLE_STRUCT & stHwTEV )
{
  for( int nInd = 0; nInd < HW_TEV_TABLE_MAX_ROW_SZ; nInd++ )
  {
    stHwTEV.byTEVType[nInd] =  (unsigned char) rand();
    stHwTEV.byValveDO[nInd] =  (unsigned char) rand();
    stHwTEV.byState[nInd] =  (unsigned char) rand() % 2;
    stHwTEV.usTime[nInd] = (unsigned short) rand();
  }
  stHwTEV.byStoredRows = (unsigned char) rand();
}

template <class T_IMB_SW_TEV_TABLE_STRUCT>
void DisplayIMBSwTEVTable2( const T_IMB_SW_TEV_TABLE_STRUCT & stMySWTable, const int nMaxRowSz )
{
  printf( "SW TEV Table - \n" ); 
  printf( "------------------------------------------------\n" ); 
  printf( "byTEVType     byDetector      sValue       usTime\n" ); 
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    printf( "%*d", 9, stMySWTable.byTEVType[nInd] );
    printf( "%*d", 15, stMySWTable.byDetector[nInd] );
    printf( "%*d", 12, stMySWTable.sValue[nInd] );
    printf( "%*d", 12, stMySWTable.usTime[nInd] );
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stMySWTable.byStoredRows );
  printf( "------------------------------------------------\n" ); 
}

template <class T_IMB_SW_TEV_TABLE_STRUCT>
void PopulateIMBSwTEVTable2( T_IMB_SW_TEV_TABLE_STRUCT & stSwTEV, const int nMaxRowSz )
{
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    stSwTEV.byTEVType[nInd] =  (unsigned char) rand();
    stSwTEV.byDetector[nInd] =  (unsigned char) rand();
    stSwTEV.sValue[nInd] =  (short) rand();
    stSwTEV.usTime[nInd] = (unsigned short) rand();
  }
  stSwTEV.byStoredRows = (unsigned char) rand();
}

void DisplayIMBGainTEVTable( IMB_GAIN_TEV_TABLE_STRUCT stMyGainTable )
{
  printf( "Gain TEV Table - \n" ); 
  printf( "-------------------------------------\n" ); 
  printf( "byDetector      byGain          usTime\n" ); 
  for( int nInd = 0; nInd < GAIN_TEV_TABLE_MAX_ROW_SZ; nInd++ )
  {
    printf( "%*d", 10, stMyGainTable.byDetector[nInd] );
    printf( "%*d", 12, stMyGainTable.byGain[nInd] );
    printf( "%*d", 15, stMyGainTable.usTime[nInd] );
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stMyGainTable.byStoredRows );
  printf( "-------------------------------------\n" ); 
}

void PopulateIMBGainTEVTable( IMB_GAIN_TEV_TABLE_STRUCT & stGainTEV )
{
  for( int nInd = 0; nInd < GAIN_TEV_TABLE_MAX_ROW_SZ; nInd++ )
  {
    stGainTEV.byDetector[nInd] =  (unsigned char) rand();
    stGainTEV.byGain[nInd] =  (unsigned char) rand();
    stGainTEV.usTime[nInd] =  (unsigned short) rand();
  }
  stGainTEV.byStoredRows = (unsigned char) rand();
}

void DisplayIMBAnalysisTimeConfigTable( IMB_ANALYSIS_TIME_CONFIG_STRUCT stAnalysisTimeCfg )
{
  printf( "Analysis Configuration Table - \n" ); 
  printf( "-------------------------------------\n" ); 
  printf( "nCycleTime   = %d\n", stAnalysisTimeCfg.nCycleTime ); 
  printf( "nAnalysisTime          = %d\n", stAnalysisTimeCfg.nAnalysisTime );
  printf( "-------------------------------------\n" ); 
}

void PopulateIMBAnalysisTimeConfigTable( IMB_ANALYSIS_TIME_CONFIG_STRUCT & stAnalysisTimeCfg )
{
  stAnalysisTimeCfg.nCycleTime = (int) rand();
  stAnalysisTimeCfg.nAnalysisTime =  (int) rand();
}

void DisplayIMBAVTSettings( IMB_AVT_SETTINGS_TABLE_STRUCT stAVTSettings )
{
  printf( "AVT Settings - \n" ); 
  printf( "--------------------------------------------\n" ); 
  printf( "fValveTimingAdj         = %f\n", stAVTSettings.fValveTimingAdj );
  printf( "fAdjAlarmLimit          = %f\n", stAVTSettings.fAdjAlarmLimit );
  //printf( "fIdealRFLimit            = %f\n", stAVTSettings.fIdealRFLimit );  // no longer used. now saved in CDT - Time Events Adjustment
  printf( "nEVCAreaRepeats         = %d\n", stAVTSettings.nEVCAreaRepeats );
  printf( "nUseRepeats4ValveTiming = %d\n", stAVTSettings.nUseRepeats4ValveTiming );
  printf( "nBackupTime             = %d\n", stAVTSettings.nBackupTime );
  printf( "--------------------------------------------\n" ); 
}

void PopulateIMBAVTSettings( IMB_AVT_SETTINGS_TABLE_STRUCT & stAVTSettings )
{
  stAVTSettings.fValveTimingAdj = (float) rand();
  stAVTSettings.fAdjAlarmLimit = (float) rand();
  //stAVTSettings.fIdealRFLimit = (float) rand();  // no longer used. now saved in CDT - Time Events Adjustment
  stAVTSettings.nEVCAreaRepeats = (int) rand();
  stAVTSettings.nUseRepeats4ValveTiming = (int) rand();
  stAVTSettings.nBackupTime = (int) rand();
}

void DisplayIMBAVTValveSelectionTable( IMB_AVT_VALVE_SEL_TABLE_STRUCT stValveSel )
{
  printf( "AVT Valve Selection Table - \n" ); 
  printf( "---------------------------------------------------------------------------------------------------------------------------------\n" ); 
  printf( "     nName    fInitialTimingOn   fInitialTimingOff  nTimingToAdjust  byEarlyValveCutComp  byLateValveCutComp fPeakAreaChangeLimit\n" ); 
  for( int nInd = 0; nInd < VALVE_SEL_TABLE_MAX_ROW_SZ; nInd++ )
  {
    printf( "%*d", 10, stValveSel.nName[nInd] );
    printf( "%*f", 20, stValveSel.fInitialTimingOn[nInd] );
    printf( "%*f", 20, stValveSel.fInitialTimingOff[nInd] );
    printf( "%*d", 17, stValveSel.nTimingToAdjust[nInd] );
    printf( "%*d", 21, stValveSel.byEarlyValveCutComp[nInd] );
    printf( "%*d", 20, stValveSel.byLateValveCutComp[nInd] );
    printf( "%*f", 21, stValveSel.fPeakAreaChangeLimit[nInd] );
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stValveSel.byStoredRows );
  printf( "---------------------------------------------------------------------------------------------------------------------------------\n" ); 
}

void PopulateIMBAVTValveSelectionTable( IMB_AVT_VALVE_SEL_TABLE_STRUCT & stValveSel )
{
  for( int nInd = 0; nInd < VALVE_SEL_TABLE_MAX_ROW_SZ; nInd++ )
  {
    stValveSel.nName[nInd] =  (int) rand();
    stValveSel.fInitialTimingOn[nInd] = (float) rand();
    stValveSel.fInitialTimingOff[nInd] = (float) rand();
    stValveSel.nTimingToAdjust[nInd] = (int) rand();
    stValveSel.byEarlyValveCutComp[nInd] = (unsigned char) rand() % 2;
    stValveSel.byLateValveCutComp[nInd] = (unsigned char) rand() % 2;
    stValveSel.fPeakAreaChangeLimit[nInd] = (float) rand();
  }
  stValveSel.byStoredRows = (unsigned char) rand();
}

template <class T_IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2>
void DisplayIMBAVTTimeEventsAdjTable2(const T_IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 & stTimeEventAdj, const int nMaxRowSz, const int nDependentTEVLen )
{

  printf( "AVT Time Events Adjustment Table - \n" );
  printf( "------------------------------------------------------------------------------------------\n" ); 
  //printf( "       fIdealRFRatio     nIdealRFRefComp       fIdealRFLimit                szDependentTEV\n" ); 
  printf( "       fIdealRFRatio     nIdealRFRefComp       fIdealRFLimit\n" ); 
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    //Parse Dependent TEV string for a component from the comma seperated string
    //          std::getline( sDepTEVStream, sDepTEVForComponent, ';' );
          
    printf( "%*f", 20, stTimeEventAdj.fIdealRFRatio[nInd] );
    printf( "%*d", 20, stTimeEventAdj.nIdealRFRefComp[nInd] );
    printf( "%*f", 20, stTimeEventAdj.fIdealRFLimit[nInd] );
    printf( "%*d", 20, stTimeEventAdj.chComponent[nInd] );
    //  printf( "%*s\n", 30, sDepTEVForComponent.c_str() );
    //printf( "%*s\n", 30, stTimeEventAdj.szDependentTEV );
    printf( "\n" );
  }

  unsigned char * chDepTEV = new unsigned char[nDependentTEVLen +1];
  memcpy(chDepTEV, stTimeEventAdj.szDependentTEV, nDependentTEVLen);
  chDepTEV[nDependentTEVLen] = '\0';

  printf("\n       szDependentTEV\n");
  printf( "%*s\n", 30, chDepTEV );
  
  printf( "Stored Rows = %d\n", stTimeEventAdj.byStoredRows );
  printf( "------------------------------------------------------------------------------------------\n" ); 
}

template <class T_IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2>
void PopulateIMBAVTTimeEventsAdjTable2( T_IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 & stTimeEventAdj, const int nMaxRowSz, const int nDependentTEVLen )
{
  for( int nInd = 0; nInd < nMaxRowSz; nInd++ )
  {
    stTimeEventAdj.fIdealRFRatio[nInd] =  (float) rand();
    stTimeEventAdj.nIdealRFRefComp[nInd] =  (int) rand();
    stTimeEventAdj.fIdealRFLimit[nInd] =  (float) rand();
    stTimeEventAdj.chComponent[nInd] =  ( unsigned char ) rand();
  }
  GenerateRandomString( stTimeEventAdj.szDependentTEV, nDependentTEVLen );
  stTimeEventAdj.byStoredRows = (unsigned char) rand();
}

void DisplayIMBMetaInfo2( const IMB_SECTION_META_INFO2 & stMetaInfo )
{
  printf( "Meta Info - \n" ); 
  printf( "--------------------------------\n" ); 
  printf( "stIMBDbId.usMajor = %u\n", stMetaInfo.stIMBDbId.usMajor );
  printf( "stIMBDbId.usMinor = %u\n", stMetaInfo.stIMBDbId.usMinor );
  printf( "szGCSWRev         = %s\n", stMetaInfo.szGCSWRev );
  printf( "--------------------------------\n" ); 
}

void PopulateIMBMetaInfo2( IMB_SECTION_META_INFO2 & stMetaInfo )
{
  stMetaInfo.stIMBDbId.usMajor = (unsigned short) rand();
  stMetaInfo.stIMBDbId.usMinor = (unsigned short) rand();
  GenerateRandomString( stMetaInfo.szGCSWRev, META_INFO_SOFTWARE_REV_LENGTH ); 
}

// CDT (component) custom-names
void DisplayCDTCustomNames( const IMB_CDT_CUSTOM_NAME_STRUCT & stMyCustomName )
{
  printf( "CDT Custom Names - \n" );
  printf( "-------------------------------------\n" );
  printf( "Custom Names        Code        \n" );
  for( int nInd = 0; nInd < CDT_CUSTOM_NAME_MAX_ROW_SZ; nInd++ )
  {
    printf( "%*s", 20, stMyCustomName.szCompCustName[nInd] );
    printf( "%*d", 10, stMyCustomName.szComponent[nInd] );
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stMyCustomName.byStoredRows );
  printf( "-------------------------------------\n" );
}

void PopulateCDTCustomNames( IMB_CDT_CUSTOM_NAME_STRUCT & stCustomNames )
{
  for( int nInd = 0; nInd < CDT_CUSTOM_NAME_MAX_ROW_SZ; nInd++ )
  {
    GenerateRandomString(stCustomNames.szCompCustName[nInd], CDT_CUSTOM_NAME_MAX_STR_SZ);
    stCustomNames.szComponent[nInd] =  (unsigned char) rand();
  }
  stCustomNames.byStoredRows = (unsigned char) rand();
}

// Allowable-DEV
void DisplayAllowableDEV2( const IMB_MV_ALLOWABLE_DEV_STRUCT & stMyAllowableDEV )
{
  printf( "CDT Custom Names - \n" );
  printf( "-------------------------------------\n" );
  printf( "Component Code      DEV        \n" );
  for( int nInd = 0; nInd < MV_ALLOWABLE_DEV_MAX_ROW_SZ; nInd++ )
  {
    printf( "%*d", 20, stMyAllowableDEV.chComponent[nInd] );
    printf( "%*d", 10, stMyAllowableDEV.sAllowableDev[nInd] );
    printf( "\n" );
  }
  printf( "Stored Rows = %d\n", stMyAllowableDEV.byStoredRows );
  printf( "-------------------------------------\n" );
}

void PopulateAllowableDEV2( IMB_MV_ALLOWABLE_DEV_STRUCT & stAllowableDEV )
{
  for( int nInd = 0; nInd < MV_ALLOWABLE_DEV_MAX_ROW_SZ; nInd++ )
  {
    stAllowableDEV.chComponent[nInd] =  (unsigned char ) rand();
    stAllowableDEV.sAllowableDev[nInd] =  (short) rand();
  }
  stAllowableDEV.byStoredRows = (unsigned char) rand();
}


// Application-Type
void DisplayApplicationType2( const IMB_APPLICATION_TYPE_STRUCT & stApplicationType )
{
  printf( "Application Type - \n" );
  printf( "--------------------------------\n" );
  printf( "byApplicationType         = %s\n", stApplicationType.byApplicationType );
  printf( "--------------------------------\n" );
}

void PopulateApplicationType2( IMB_APPLICATION_TYPE_STRUCT & stApplicationType )
{
  GenerateRandomString( stApplicationType.byApplicationType, APPLICATION_TYPE_STR_LENGTH );
}

struct stIMBCommand
{
  int m_nCommandNumber;
  int m_nSection;
  stIMBCommand( int nCommandNumber, int nSection )
    {
      m_nCommandNumber = nCommandNumber; 
      m_nSection = nSection; 
    }
};

static void TestIMBFactoryData(IMB_FACTORY_DATA_STRUCT2 & stSetDataStruct, IMB_FACTORY_DATA_STRUCT2 & stGetDataStruct);
static void TestIMBMirrorData( IMB_MIRROR_DATA_STRUCT2  & stSetDataStruct, IMB_MIRROR_DATA_STRUCT2  & stGetDataStruct);
static void TestIMBGlobalData( IMB_GLOBAL_DATA_STRUCT2  & stSetDataStruct, IMB_GLOBAL_DATA_STRUCT2  & stGetDataStruct);

void TestIMBComm()
{
  int nRetVal = 0;

  static char * szIMBDevName  = "IMB_COMM:IMB_SLOT:IMB_COMM_1";
  static CIMBComm oIMBComm;

  int nIMBCommRequest = 0;
  IMB_FLASH_SECTION_ENUM eIMBSelectedSection = IMB_FACTORY_FLASH_SECTION;

  //*******************************************************
  //*******************************************************
  printf( " sizeof ( IMB_GLOBAL_DATA_STRUCT2 ) = %d\n",  sizeof ( IMB_GLOBAL_DATA_STRUCT2 ) );

  // DEFAULT SECTION no longer used, wyu - May, 2014
  //printf( " sizeof ( IMB_DEFAULT_DATA_STRUCT ) = %d\n",  sizeof ( IMB_DEFAULT_DATA_STRUCT ) );

  printf( " sizeof ( IMB_FACTORY_DATA_STRUCT2 ) = %d\n",  sizeof ( IMB_FACTORY_DATA_STRUCT2 ) );
  printf( " sizeof ( IMB_MIRROR_DATA_STRUCT2 ) = %d\n",  sizeof ( IMB_MIRROR_DATA_STRUCT2 ) );

  printf( "\n\n" );

  printf( " sizeof ( IMB_SECTION_META_INFO2 ) = %d\n",  sizeof ( IMB_SECTION_META_INFO2 ) );
  printf( " sizeof ( IMB_AVT_SETTINGS_TABLE_STRUCT ) = %d\n",  sizeof ( IMB_AVT_SETTINGS_TABLE_STRUCT ) );
  printf( " sizeof ( IMB_AVT_VALVE_SEL_TABLE_STRUCT ) = %d\n",  sizeof ( IMB_AVT_VALVE_SEL_TABLE_STRUCT ) );
  printf( " sizeof ( IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 ) = %d\n",  sizeof ( IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 ) );
  printf( " sizeof ( IMB_CDT_TABLE_STRUCT ) = %d\n",  sizeof ( IMB_CDT_TABLE_STRUCT ) );
  printf( " sizeof ( IMB_HW_TEV_TABLE_STRUCT ) = %d\n",  sizeof ( IMB_HW_TEV_TABLE_STRUCT ) );
  printf( " sizeof ( IMB_SW_TEV_TABLE_STRUCT ) = %d\n",  sizeof ( IMB_SW_TEV_TABLE_STRUCT ) );
  printf( " sizeof ( IMB_GAIN_TEV_TABLE_STRUCT ) = %d\n",  sizeof ( IMB_GAIN_TEV_TABLE_STRUCT ) );

  printf( "\n\n" );

  printf( " sizeof ( float ) = %d\n",  sizeof ( float ) );
  printf( " sizeof ( int ) = %d\n",  sizeof ( int ) );
  printf( " sizeof ( unsigned char ) = %d\n",  sizeof ( unsigned char ) );
  //*******************************************************
  //*******************************************************

  bool bAutomatedTest = false;
  bool bAutomatedReadTest = false;
  int nAutomatedTestInd = 0;
  std::vector< stIMBCommand > v_stIMBCommands;

  // DEFAULT SECTION no longer used, wyu - May, 2014
  //IMB_DEFAULT_DATA_STRUCT stSetDefaultDataStruct, stGetDefaultDataStruct;

  IMB_FACTORY_DATA_STRUCT2 stSetFactoryDataStruct, stGetFactoryDataStruct;
  IMB_MIRROR_DATA_STRUCT2  stSetMirrorDataStruct, stGetMirrorDataStruct;
  IMB_GLOBAL_DATA_STRUCT2  stSetGlobalDataStruct, stGetGlobalDataStruct;
  //unsigned char bySetGlobalIMBInit, byGetGlobalIMBInit;

  nRetVal = oIMBComm.OpenHal(szIMBDevName);
  if (nRetVal < 0)
  {
    printf("Error %d opening IMB Communication Channel !!!\n", nRetVal);
    return;
  }

  memset(& stSetFactoryDataStruct, 0, sizeof(IMB_FACTORY_DATA_STRUCT2));
  memset(& stGetFactoryDataStruct, 0, sizeof(IMB_FACTORY_DATA_STRUCT2));

  memset(& stSetMirrorDataStruct, 0, sizeof(IMB_MIRROR_DATA_STRUCT2));
  memset(& stGetMirrorDataStruct, 0, sizeof(IMB_MIRROR_DATA_STRUCT2));

  memset(& stSetGlobalDataStruct, 0, sizeof(IMB_GLOBAL_DATA_STRUCT2));
  memset(& stGetGlobalDataStruct, 0, sizeof(IMB_GLOBAL_DATA_STRUCT2));

  while (g_nExitApp != 1)
  {
    if( ! bAutomatedTest )
    {
      printf( "\n\n*****************************************************************************\n" );
      printf("  %*d. Exit application.\n", 2, IMB_FUNC_EXIT);
      printf("  %*d. Set Section Meta-Info.           ", 2, IMB_FUNC_SET_SECTION_META_INFO );
      printf("  %*d. Get Section Meta-Info.\n"         , 2, IMB_FUNC_GET_SECTION_META_INFO );
      printf("  %*d. Set Section AVT Settings Table.  ", 2, IMB_FUNC_SET_SECTION_AVT_SET_TABLE );
      printf("  %*d. Get Section AVT Settings Table.\n", 2, IMB_FUNC_GET_SECTION_AVT_SET_TABLE );
      printf("  %*d. Set Section AVT Valve SEL Table. ", 2, IMB_FUNC_SET_SECTION_AVT_VSEL_TABLE );
      printf("  %*d. Get Section AVT Valve SEL Table\n", 2, IMB_FUNC_GET_SECTION_AVT_VSEL_TABLE );
      printf("  %*d. Set Section AVT Time ADJ Table.  ", 2, IMB_FUNC_SET_SECTION_AVT_TEADJ_TABLE );
      printf("  %*d. Get Section AVT Time ADJ Table.\n", 2, IMB_FUNC_GET_SECTION_AVT_TEADJ_TABLE );
      printf("  %*d. Set Section CDT Configuration.   ", 2, IMB_FUNC_SET_SECTION_CDT_CONFIG );
      printf("  %*d. Get Section CDT Configuration.\n" , 2, IMB_FUNC_GET_SECTION_CDT_CONFIG );
      //printf("  %*d. Set Section CDT Updates.         ", 2, IMB_FUNC_SET_SECTION_CDT_UPDATE );
      //printf("  %*d. Get Section CDT Updates.\n"       , 2, IMB_FUNC_GET_SECTION_CDT_UPDATE );
      printf("  %*d. Set Section Hardware TEV Table.  ", 2, IMB_FUNC_SET_SECTION_HTEV_TABLE );
      printf("  %*d. Get Section Hardware TEV Table.\n", 2, IMB_FUNC_GET_SECTION_HTEV_TABLE );
      printf("  %*d. Set Section Software TEV Table.  ", 2, IMB_FUNC_SET_SECTION_STEV_TABLE );
      printf("  %*d. Get Section Software TEV Table.\n", 2, IMB_FUNC_GET_SECTION_STEV_TABLE );
      printf("  %*d. Set Section Gain TEV Table.      ", 2, IMB_FUNC_SET_SECTION_GTEV_TABLE );
      printf("  %*d. Get Section Gain TEV Table.\n"    , 2, IMB_FUNC_GET_SECTION_GTEV_TABLE );
      printf("  %*d. Set Section Analy Config Table.  ", 2, IMB_FUNC_SET_SECTION_ANLY_CFG_TABLE );
      printf("  %*d. Get Section Analy Config Table.\n", 2, IMB_FUNC_GET_SECTION_ANLY_CFG_TABLE );
      printf("  %*d. Set Section Set Points.          ", 2, IMB_FUNC_SET_SECTION_SET_POINTS );
      printf("  %*d. Get Section Set Points.\n"        , 2, IMB_FUNC_GET_SECTION_SET_POINTS );
      printf("  %*d. Set Section Carrier Cal Faq.     ", 2, IMB_FUNC_SET_SECTION_CARRIER_CAL_FAQ );
      printf("  %*d. Get Section Carrier Cal Faq.\n"   , 2, IMB_FUNC_GET_SECTION_CARRIER_CAL_FAQ );
      //printf("  %*d. Set Section Sample Cal Faq.      ", 2, IMB_FUNC_SET_SECTION_SAMPLE_CAL_FAQ );
      //printf("  %*d. Get Section Sample Cal Faq.\n"    , 2, IMB_FUNC_GET_SECTION_SAMPLE_CAL_FAQ );
      printf("  %*d. Set Section CDT Custom Names.    ", 2, IMB_FUNC_SET_SECTION_CUSTOM_NAMES );
      printf("  %*d. Get Section CDT Custom Names.\n"  , 2, IMB_FUNC_GET_SECTION_CUSTOM_NAMES );
      printf("  %*d. Set Section Allowable DEV.       ", 2, IMB_FUNC_SET_SECTION_ALLOWABLE_DEV );
      printf("  %*d. Get Section Allowable DEV.\n"     , 2, IMB_FUNC_GET_SECTION_ALLOWABLE_DEV );

      // MIRROR
      printf("  \n");
      printf("  %*d. Set MIRROR AVT Enable.           ", 2, IMB_FUNC_SET_MIRROR_AVT_ENABLE );
      printf("  %*d. Get MIRROR AVT Enable.\n"         , 2, IMB_FUNC_GET_MIRROR_AVT_ENABLE );
      printf("  %*d. Set MIRROR Analysis Number.      ", 2, IMB_FUNC_SET_MIRROR_NUM_ANALYSIS );
      printf("  %*d. Get MIRROR Analysis Number.\n"    , 2, IMB_FUNC_GET_MIRROR_NUM_ANALYSIS );

      // GLOBAL
      printf("  \n");
      printf("  %*d. Set GLOBAL Data Init Status.     ", 2, IMB_FUNC_SET_GLOBAL_DATA_INIT );
      printf("  %*d. Get GLOBAL Data Init Status.\n"   , 2, IMB_FUNC_GET_GLOBAL_DATA_INIT );
      printf("  %*d. Set GLOBAL Module Serial Number. ", 2, IMB_FUNC_SET_MODULE_SERIAL_NUM );
      printf("  %*d. Get GLOBAL Module Serial Number\n", 2, IMB_FUNC_GET_MODULE_SERIAL_NUM );
      printf("  %*d. Set GLOBAL Data Meta-Info.       ", 2, IMB_FUNC_SET_GLOBAL_META_INFO );
      printf("  %*d. Get GLOBAL Data Meta-Info.\n"     , 2, IMB_FUNC_GET_GLOBAL_META_INFO );
      printf("  %*d. Set GLOBAL Data App-Type.        ", 2, IMB_FUNC_SET_GLOBAL_APP_TYPE );
      printf("  %*d. Get GLOBAL Data App-Type.\n"      , 2, IMB_FUNC_GET_GLOBAL_APP_TYPE );

      // ALL OTHERS
      printf("  \n");
      printf("  %*d. Get DB ID.                       ", 2, IMB_FUNC_GET_DB_ID );
      printf("  %*d. Get Serial Num, API ID, DB ID.\n",  2, IMB_FUNC_GET_IMB_INFO);

      printf("  %*d. Write All Data Sections.         ",  2, IMB_FUNC_SET_ALL_DATA_SECTIONS );
      printf("  %*d. Read & Verify All Data Sections.\n", 2, IMB_FUNC_GET_ALL_DATA_SECTIONS );
      printf("  %*d. Update Flash.\n",                    2, IMB_FUNC_UPDATE_FLASH );

      printf("  %*d. Set Flash Image.                 ",  2, IMB_FUNC_SET_FLASH_IMAGE );
      printf("  %*d. Get Flash Image.\n",                 2, IMB_FUNC_GET_FLASH_IMAGE );

      printf("  %*d. Reset All Data Sections.\n",         2, IMB_FUNC_RESET_ALL_DATA_SECTIONS );

// NEW
      printf( "*****************************************************************************\n" );
      printf("Select IMB Communication Request : ");
      fflush(stdout);

      fflush(stdin);
      scanf("%d", &nIMBCommRequest);
    }
    else
    {
      nIMBCommRequest = v_stIMBCommands[nAutomatedTestInd].m_nCommandNumber;
    }

    if ( (nIMBCommRequest == IMB_FUNC_EXIT) || (g_nExitApp))
    {
      break;
    }

    char * szSectionName = NULL;

    if( nIMBCommRequest == IMB_FUNC_SET_SECTION_CDT_CONFIG ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_CDT_CONFIG ||
        //nIMBCommRequest == IMB_FUNC_SET_SECTION_CDT_UPDATE ||
        //nIMBCommRequest == IMB_FUNC_GET_SECTION_CDT_UPDATE ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_HTEV_TABLE ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_HTEV_TABLE ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_STEV_TABLE ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_STEV_TABLE ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_GTEV_TABLE ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_GTEV_TABLE ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_META_INFO  ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_META_INFO  ||

        nIMBCommRequest == IMB_FUNC_SET_SECTION_ANLY_CFG_TABLE ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_ANLY_CFG_TABLE ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_AVT_SET_TABLE  ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_AVT_SET_TABLE  ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_AVT_VSEL_TABLE ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_AVT_VSEL_TABLE  ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_AVT_TEADJ_TABLE  ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_AVT_TEADJ_TABLE  ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_SET_POINTS  ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_SET_POINTS  ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_CARRIER_CAL_FAQ  ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_CARRIER_CAL_FAQ  ||
        //nIMBCommRequest == IMB_FUNC_SET_SECTION_SAMPLE_CAL_FAQ  ||
        //nIMBCommRequest == IMB_FUNC_GET_SECTION_SAMPLE_CAL_FAQ
        nIMBCommRequest == IMB_FUNC_SET_SECTION_CUSTOM_NAMES    ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_CUSTOM_NAMES    ||
        nIMBCommRequest == IMB_FUNC_SET_SECTION_ALLOWABLE_DEV   ||
        nIMBCommRequest == IMB_FUNC_GET_SECTION_ALLOWABLE_DEV   )
    {
      int nIMBSelectedSection = 0;
      if( ! bAutomatedTest )
      {
        printf( "Enter one of the following options - \n" );
        // DEFAULT SECTION no longer used, wyu - May, 2014
        //printf( " 0 - Default Section\n" );
        printf( " 1 - Factory Section\n" );
        printf( " 2 - Mirror  Section\n" );

        fflush(stdin);
        scanf("%d", & nIMBSelectedSection);
      }
      else
      {
        nIMBSelectedSection = v_stIMBCommands[nAutomatedTestInd].m_nSection;
      }

      eIMBSelectedSection = (IMB_FLASH_SECTION_ENUM) nIMBSelectedSection;


      if( eIMBSelectedSection <IMB_FLASH_SECTION_START || eIMBSelectedSection > IMB_FLASH_SECTION_END )
      {
        printf( "Invalid Option ... Try again !!!\n" );
        continue;
      }
      else
      {
        switch( nIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case 0:
            szSectionName = "Default Section : ";
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          szSectionName = "Factory Section : ";
          break;
        case IMB_MIRROR_FLASH_SECTION:
          szSectionName = "Mirror Section : ";
          break;
        default:
          szSectionName = "Unknown Section : ";
          break;
        }
      }
    }

    switch (nIMBCommRequest)
    {
    case IMB_FUNC_SET_SECTION_META_INFO:
    {
      IMB_SECTION_META_INFO2 stMetaInfo;
      PopulateIMBMetaInfo2( stMetaInfo );
      if ( (nRetVal = oIMBComm.SetSectionMetaInfo2( eIMBSelectedSection, stMetaInfo )) < 0 )
      {
        printf("CIMBComm::SetSectionMetaInfo() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionMetaInfo() success.\n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBMetaInfo2( stMetaInfo );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stSetDefaultDataStruct.stMetaInfo = stMetaInfo;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stMetaInfo = stMetaInfo;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stMetaInfo = stMetaInfo;
          break; 
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_META_INFO:
    {
      IMB_SECTION_META_INFO2 stMetaInfo;
      if ( (nRetVal = oIMBComm.GetSectionMetaInfo2( eIMBSelectedSection, & stMetaInfo )) < 0 )
      {
        printf("CIMBComm::GetSectionMetaInfo() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionMetaInfo() success.\n");
        printf( "Read %s", szSectionName );
        DisplayIMBMetaInfo2( stMetaInfo );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stGetDefaultDataStruct.stMetaInfo = stMetaInfo;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stMetaInfo = stMetaInfo;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stMetaInfo = stMetaInfo;
          break; 
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_SET_SECTION_AVT_SET_TABLE:
    {
      IMB_AVT_SETTINGS_TABLE_STRUCT stAVTSettingTable;
      PopulateIMBAVTSettings( stAVTSettingTable );
      if ( (nRetVal = oIMBComm.SetSectionAVTSettingsTable2( eIMBSelectedSection, stAVTSettingTable )) < 0 )
      {
        printf("CIMBComm::SetSectionAVTSettingsTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionAVTSettingsTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBAVTSettings( stAVTSettingTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stSetDefaultDataStruct.stAVTSettings = stAVTSettingTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stAVTSettings = stAVTSettingTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          printf( "\nNot supported !\n" );
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_AVT_SET_TABLE:
    {
      IMB_AVT_SETTINGS_TABLE_STRUCT stAVTSettingTable;
      if ( (nRetVal = oIMBComm.GetSectionAVTSettingsTable2( eIMBSelectedSection, & stAVTSettingTable )) < 0 )
      {
        printf("CIMBComm::GetSectionAVTSettingsTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionAVTSettingsTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBAVTSettings( stAVTSettingTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stGetDefaultDataStruct.stAVTSettings = stAVTSettingTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stAVTSettings = stAVTSettingTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          printf( "\nNot supported !\n" );
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_SET_SECTION_AVT_VSEL_TABLE:
    {
      IMB_AVT_VALVE_SEL_TABLE_STRUCT stVSelTable;
      PopulateIMBAVTValveSelectionTable( stVSelTable );
      if ( (nRetVal = oIMBComm.SetSectionAVTValveSelTable2( eIMBSelectedSection, stVSelTable )) < 0 )
      {
        printf("CIMBComm::SetSectionAVTValveSelTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionAVTValveSelTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBAVTValveSelectionTable( stVSelTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stSetDefaultDataStruct.stAVTValveSel = stVSelTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stAVTValveSel = stVSelTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          printf( "\nNot supported !\n" );
        default:
          break;
        }

      }
    }
    break;
    case IMB_FUNC_GET_SECTION_AVT_VSEL_TABLE:
    {
      IMB_AVT_VALVE_SEL_TABLE_STRUCT stVSelTable;
      if ( (nRetVal = oIMBComm.GetSectionAVTValveSelTable2( eIMBSelectedSection, & stVSelTable )) < 0 )
      {
        printf("CIMBComm::GetSectionAVTValveSelTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionAVTValveSelTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBAVTValveSelectionTable( stVSelTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stGetDefaultDataStruct.stAVTValveSel = stVSelTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stAVTValveSel = stVSelTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          printf( "\nNot supported !\n" );
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_SET_SECTION_AVT_TEADJ_TABLE:
    {
      // Base Structure
      IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 stTEATable;
      PopulateIMBAVTTimeEventsAdjTable2( stTEATable, TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ, TIME_EVENT_DEPENDENT_TEV_LENGTH2 );
      if ( (nRetVal = oIMBComm.SetSectionAVTTimeEventAdjTable2( eIMBSelectedSection, stTEATable )) < 0 )
      {
        printf("CIMBComm::SetSectionAVTTimeEventAdjTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionAVTTimeEventAdjTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBAVTTimeEventsAdjTable2( stTEATable, TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ, TIME_EVENT_DEPENDENT_TEV_LENGTH2 );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stAVTTimeEventAdj = stTEATable;
          break;
        default:
          break;
        }
      }

      // EXT
      IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT stTEATableExt;
      PopulateIMBAVTTimeEventsAdjTable2( stTEATableExt, TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT, TIME_EVENT_DEPENDENT_TEV_LENGTH_EXT );
      if ( (nRetVal = oIMBComm.SetSectionAVTTimeEventAdjTableExt2( eIMBSelectedSection, stTEATableExt )) < 0 )
      {
        printf("CIMBComm::SetSectionAVTTimeEventAdjTableExt() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionAVTTimeEventAdjTableExt() success. \n");
        printf( "EXT \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBAVTTimeEventsAdjTable2( stTEATableExt, TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT, TIME_EVENT_DEPENDENT_TEV_LENGTH_EXT );
      }
      switch( eIMBSelectedSection )
      {
      case IMB_FACTORY_FLASH_SECTION:
        stSetFactoryDataStruct.stAVTTimeEventAdjExt = stTEATableExt;
        break;
      default:
        break;
      }

    }
    break;
    case IMB_FUNC_GET_SECTION_AVT_TEADJ_TABLE:
    {
      IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 stTEATable;
      if ( (nRetVal = oIMBComm.GetSectionAVTTimeEventAdjTable2( eIMBSelectedSection, & stTEATable )) < 0 )
      {
        printf("CIMBComm::GetSectionAVTTimeEventAdjTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionAVTTimeEventAdjTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBAVTTimeEventsAdjTable2( stTEATable, TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ, TIME_EVENT_DEPENDENT_TEV_LENGTH2 );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stAVTTimeEventAdj = stTEATable;
          break;
        default:
          break;
        }
      }

      //EXT
      IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT stTEATableExt;
      if ( (nRetVal = oIMBComm.GetSectionAVTTimeEventAdjTableExt2( eIMBSelectedSection, & stTEATableExt )) < 0 )
      {
        printf("CIMBComm::GetSectionAVTTimeEventAdjTableExt() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionAVTTimeEventAdjTableExt() success. \n");
        printf( "EXT \n");
        printf( "Read %s", szSectionName );
        DisplayIMBAVTTimeEventsAdjTable2( stTEATableExt, TIME_EVENT_ADJ_TABLE_MAX_ROW_SZ_EXT, TIME_EVENT_DEPENDENT_TEV_LENGTH_EXT );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stAVTTimeEventAdjExt = stTEATableExt;
          break;
        default:
          break;
        }

      }
    }
    break;
    case IMB_FUNC_SET_SECTION_CDT_CONFIG:
    {
      IMB_CDT_TABLE_STRUCT stCDTTable;
      PopulateIMBCDTConfig2( stCDTTable.stCDTConfig, CDT_TABLE_MAX_ROW_SZ );
      PopulateIMBCDTUpdate2( stCDTTable.stCDTUpdate, CDT_TABLE_MAX_ROW_SZ );
      if ( (nRetVal = oIMBComm.SetSectionCDTTable2( eIMBSelectedSection, stCDTTable )) < 0 )
      {
        printf("CIMBComm::SetSectionCDTTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionCDTTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBCDTConfig2( stCDTTable.stCDTConfig, CDT_TABLE_MAX_ROW_SZ );
        DisplayIMBCDTUpdate2( stCDTTable.stCDTUpdate, CDT_TABLE_MAX_ROW_SZ );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stCDT.stCDTConfig = stCDTTable.stCDTConfig;
          stSetFactoryDataStruct.stCDT.stCDTUpdate = stCDTTable.stCDTUpdate;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stCDT.stCDTConfig = stCDTTable.stCDTConfig;
          stSetMirrorDataStruct.stCDT.stCDTUpdate = stCDTTable.stCDTUpdate;
          break; 
        default:
          break;
        }
      }

      //EXT
      IMB_CDT_TABLE_STRUCT_EXT stCDTTableExt;
      PopulateIMBCDTConfig2( stCDTTableExt.stCDTConfigExt, CDT_TABLE_MAX_ROW_SZ_EXT );
      PopulateIMBCDTUpdate2( stCDTTableExt.stCDTUpdateExt, CDT_TABLE_MAX_ROW_SZ_EXT );
      if ( (nRetVal = oIMBComm.SetSectionCDTTableExt2( eIMBSelectedSection, stCDTTableExt )) < 0 )
      {
        printf("CIMBComm::SetSectionCDTTableExt() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionCDTTableExt() success. \n");
        printf("EXT \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBCDTConfig2( stCDTTableExt.stCDTConfigExt, CDT_TABLE_MAX_ROW_SZ_EXT );
        DisplayIMBCDTUpdate2( stCDTTableExt.stCDTUpdateExt, CDT_TABLE_MAX_ROW_SZ_EXT );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stCDTExt.stCDTConfigExt = stCDTTableExt.stCDTConfigExt;
          stSetFactoryDataStruct.stCDTExt.stCDTUpdateExt = stCDTTableExt.stCDTUpdateExt;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stCDTExt.stCDTConfigExt = stCDTTableExt.stCDTConfigExt;
          stSetMirrorDataStruct.stCDTExt.stCDTUpdateExt = stCDTTableExt.stCDTUpdateExt;
          break;
        default:
          break;
        }
      }

    }
    break;
    case IMB_FUNC_GET_SECTION_CDT_CONFIG:
    {
      IMB_CDT_TABLE_STRUCT stCDTTable;
      if ( (nRetVal = oIMBComm.GetSectionCDTTable2( eIMBSelectedSection, & stCDTTable )) < 0 )
      {
        printf("CIMBComm::GetSectionCDTTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionCDTTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBCDTConfig2( stCDTTable.stCDTConfig, CDT_TABLE_MAX_ROW_SZ );
        DisplayIMBCDTUpdate2( stCDTTable.stCDTUpdate, CDT_TABLE_MAX_ROW_SZ );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stCDT.stCDTConfig = stCDTTable.stCDTConfig;
          stGetFactoryDataStruct.stCDT.stCDTUpdate = stCDTTable.stCDTUpdate;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stCDT.stCDTConfig = stCDTTable.stCDTConfig;
          stGetMirrorDataStruct.stCDT.stCDTUpdate = stCDTTable.stCDTUpdate;
          break; 
        default:
          break;
        }
      }

      //EXT
      IMB_CDT_TABLE_STRUCT_EXT stCDTTableExt;
      if ( (nRetVal = oIMBComm.GetSectionCDTTableExt2( eIMBSelectedSection, & stCDTTableExt )) < 0 )
      {
        printf("CIMBComm::GetSectionCDTTableExt() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionCDTTableExt() success. \n");
        printf("EXT \n");
        printf( "Read %s", szSectionName );
        DisplayIMBCDTConfig2( stCDTTableExt.stCDTConfigExt, CDT_TABLE_MAX_ROW_SZ_EXT );
        DisplayIMBCDTUpdate2( stCDTTableExt.stCDTUpdateExt, CDT_TABLE_MAX_ROW_SZ_EXT );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stCDTExt.stCDTConfigExt = stCDTTableExt.stCDTConfigExt;
          stGetFactoryDataStruct.stCDTExt.stCDTUpdateExt = stCDTTableExt.stCDTUpdateExt;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stCDTExt.stCDTConfigExt = stCDTTableExt.stCDTConfigExt;
          stGetMirrorDataStruct.stCDTExt.stCDTUpdateExt = stCDTTableExt.stCDTUpdateExt;
          break;
        default:
          break;
        }
      }

    }
    break;
    /*
    case IMB_FUNC_SET_SECTION_CDT_UPDATE:
    {
      IMB_CDT_UPDATE_STRUCT stCDTUpdate;
      PopulateIMBCDTUpdate( stCDTUpdate );
      if ( (nRetVal = oIMBComm.SetSectionCDTUpdate( eIMBSelectedSection, stCDTUpdate )) < 0 ) 
      {
        printf("CIMBComm::SetSectionCDTUpdate() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionCDTUpdate() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBCDTUpdate( stCDTUpdate );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          //  case IMB_DEFAULT_FLASH_SECTION:
          //  stSetDefaultDataStruct.stCDT.stCDTUpdate = stCDTUpdate;
          //  break;
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stCDT.stCDTUpdate = stCDTUpdate;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stCDT.stCDTUpdate = stCDTUpdate;
          break; 
        default:
          break;
        }
      }
    }
    break;
    */

    /*
    case IMB_FUNC_GET_SECTION_CDT_UPDATE:
    {
      IMB_CDT_UPDATE_STRUCT stCDTUpdate;
      if ( (nRetVal = oIMBComm.GetSectionCDTUpdate( eIMBSelectedSection, & stCDTUpdate )) < 0 ) 
      {
        printf("CIMBComm::GetSectionCDTUpdate() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionCDTUpdate() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBCDTUpdate( stCDTUpdate );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          //  case IMB_DEFAULT_FLASH_SECTION:
          //  stGetDefaultDataStruct.stCDT.stCDTUpdate = stCDTUpdate;
          //  break;
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stCDT.stCDTUpdate = stCDTUpdate;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stCDT.stCDTUpdate = stCDTUpdate;
          break; 
        default:
          break;
        }
      }
    }
    break;
    */
    case IMB_FUNC_SET_SECTION_HTEV_TABLE:
    {
      IMB_HW_TEV_TABLE_STRUCT stHwTEVTable;
      PopulateIMBHwTEVTable( stHwTEVTable );
      if ( (nRetVal = oIMBComm.SetSectionHwTEVTable2( eIMBSelectedSection, stHwTEVTable )) < 0 )
      {
        printf("CIMBComm::SetSectionHwTEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionHwTEVTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBHwTEVTable( stHwTEVTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stSetDefaultDataStruct.stHwTEV = stHwTEVTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stHwTEV = stHwTEVTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stHwTEV = stHwTEVTable;
          break; 
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_HTEV_TABLE:
    {
      IMB_HW_TEV_TABLE_STRUCT stHwTEVTable;
      if ( (nRetVal = oIMBComm.GetSectionHwTEVTable2( eIMBSelectedSection, & stHwTEVTable )) < 0 )
      {
        printf("CIMBComm::GetSectionHwTEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionHwTEVTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBHwTEVTable( stHwTEVTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stGetDefaultDataStruct.stHwTEV = stHwTEVTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stHwTEV = stHwTEVTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stHwTEV = stHwTEVTable;
          break;
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_SET_SECTION_STEV_TABLE:
    {
      IMB_SW_TEV_TABLE_STRUCT stSwTEVTable;
      PopulateIMBSwTEVTable2( stSwTEVTable, SW_TEV_TABLE_MAX_ROW_SZ );
      if ( (nRetVal = oIMBComm.SetSectionSwTEVTable2( eIMBSelectedSection, stSwTEVTable )) < 0 )
      {
        printf("CIMBComm::SetSectionSwTEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionSwTEVTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBSwTEVTable2( stSwTEVTable, SW_TEV_TABLE_MAX_ROW_SZ);

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stSwTEV = stSwTEVTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stSwTEV = stSwTEVTable;
          break; 
        default:
          break;
        }
      }

      //EXT
      IMB_SW_TEV_TABLE_STRUCT_EXT stSwTEVTableExt;
      PopulateIMBSwTEVTable2( stSwTEVTableExt, SW_TEV_TABLE_MAX_ROW_SZ_EXT );
      if ( (nRetVal = oIMBComm.SetSectionSwTEVTableExt2( eIMBSelectedSection, stSwTEVTableExt )) < 0 )
      {
        printf("CIMBComm::SetSectionSwTEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionSwTEVTable() success. \n");
        printf("EXT \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBSwTEVTable2( stSwTEVTableExt, SW_TEV_TABLE_MAX_ROW_SZ_EXT);

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stSwTEVExt = stSwTEVTableExt;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stSwTEVExt = stSwTEVTableExt;
          break;
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_STEV_TABLE:
    {
      IMB_SW_TEV_TABLE_STRUCT stSwTEVTable;
      if ( (nRetVal = oIMBComm.GetSectionSwTEVTable2( eIMBSelectedSection, & stSwTEVTable )) < 0 )
      {
        printf("CIMBComm::GetSectionSwTEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionSwTEVTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBSwTEVTable2( stSwTEVTable, SW_TEV_TABLE_MAX_ROW_SZ );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stSwTEV = stSwTEVTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stSwTEV = stSwTEVTable;
          break; 
        default:
          break;
        }
      }

      //EXT
      IMB_SW_TEV_TABLE_STRUCT_EXT stSwTEVTableExt;
      if ( (nRetVal = oIMBComm.GetSectionSwTEVTableExt2( eIMBSelectedSection, & stSwTEVTableExt )) < 0 )
      {
        printf("CIMBComm::GetSectionSwTEVTableExt() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionSwTEVTableExt() success. \n");
        printf("EXT \n");
        printf( "Read %s", szSectionName );
        DisplayIMBSwTEVTable2( stSwTEVTableExt, SW_TEV_TABLE_MAX_ROW_SZ_EXT );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stSwTEVExt = stSwTEVTableExt;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stSwTEVExt = stSwTEVTableExt;
          break;
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_SET_SECTION_GTEV_TABLE:
    {
      IMB_GAIN_TEV_TABLE_STRUCT stGainTEVTable;
      PopulateIMBGainTEVTable( stGainTEVTable );
      if ( (nRetVal = oIMBComm.SetSectionGainTEVTable2( eIMBSelectedSection, stGainTEVTable )) < 0 )
      {
        printf("CIMBComm::SetSectionGainTEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionGainTEVTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBGainTEVTable( stGainTEVTable );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stGainTEV = stGainTEVTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stGainTEV = stGainTEVTable;
          break; 
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_GTEV_TABLE:
    {
      IMB_GAIN_TEV_TABLE_STRUCT stGainTEVTable;
      if ( (nRetVal = oIMBComm.GetSectionGainTEVTable2( eIMBSelectedSection, & stGainTEVTable )) < 0 )
      {
        printf("CIMBComm::GetSectionGainTEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionGainTEVTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBGainTEVTable( stGainTEVTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stGetDefaultDataStruct.stGainTEV = stGainTEVTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stGainTEV = stGainTEVTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stGainTEV = stGainTEVTable;
          break; 
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_SET_SECTION_ANLY_CFG_TABLE:
    {
      IMB_ANALYSIS_TIME_CONFIG_STRUCT stAnlyTimeCfgTable;
      PopulateIMBAnalysisTimeConfigTable( stAnlyTimeCfgTable );
      if ( (nRetVal = oIMBComm.SetSectionAnalysisConfigTable2( eIMBSelectedSection, stAnlyTimeCfgTable )) < 0 )
      {
        printf("CIMBComm::SetSectionAnalysisConfigTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionAnalysisConfigTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBAnalysisTimeConfigTable( stAnlyTimeCfgTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stSetDefaultDataStruct.stAnlyTimeCfg = stAnlyTimeCfgTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stAnlyTimeCfg = stAnlyTimeCfgTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stAnlyTimeCfg = stAnlyTimeCfgTable;
          break; 
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_ANLY_CFG_TABLE:
    {
      IMB_ANALYSIS_TIME_CONFIG_STRUCT stAnlyTimeCfgTable;
      if ( (nRetVal = oIMBComm.GetSectionAnalysisConfigTable2( eIMBSelectedSection, & stAnlyTimeCfgTable )) < 0 )
      {
        printf("CIMBComm::GetSectionAnalysisConfigTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionAnalysisConfigTable() success. \n");
        printf( "Read %s", szSectionName );
        DisplayIMBAnalysisTimeConfigTable( stAnlyTimeCfgTable );

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            stGetDefaultDataStruct.stAnlyTimeCfg = stAnlyTimeCfgTable;
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stAnlyTimeCfg = stAnlyTimeCfgTable;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stAnlyTimeCfg = stAnlyTimeCfgTable;
          break; 
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_SET_SECTION_SET_POINTS:
    {
      float fPresSetPoint = (float) rand();
      float fTempSetPoint = (float) rand();
      if ( (nRetVal = oIMBComm.SetSectionSetPoints2( eIMBSelectedSection, fPresSetPoint, fTempSetPoint )) < 0 )
      {
        printf("CIMBComm::SetSectionSetPoints() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionSetPoints() success. \n");
        printf( "Wrote %s Set Points - \n", szSectionName );
        printf( "----------------------------------\n" ); 
        printf( "fPresSetPoint = %f\n", fPresSetPoint );
        printf( "fTempSetPoint = %f\n", fTempSetPoint );
        printf( "----------------------------------\n" ); 

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            printf( "\nNot supported !\n" );
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.fPresSetPoint = fPresSetPoint;
          stSetFactoryDataStruct.fTempSetPoint = fTempSetPoint;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.fPresSetPoint = fPresSetPoint;
          stSetMirrorDataStruct.fTempSetPoint = fTempSetPoint;
          break; 
        default:
          break;
        }

      }
    }
    break;
    case IMB_FUNC_GET_SECTION_SET_POINTS:
    {
      float fPresSetPoint, fTempSetPoint;
      if ( (nRetVal = oIMBComm.GetSectionSetPoints2( eIMBSelectedSection, & fPresSetPoint, & fTempSetPoint )) < 0 )
      {
        printf("CIMBComm::GetSectionSetPoints() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionSetPoints() success. \n");
        printf( "Read %s Set Points - \n", szSectionName );
        printf( "----------------------------------\n" ); 
        printf( "fPresSetPoint = %f\n", fPresSetPoint );
        printf( "fTempSetPoint = %f\n", fTempSetPoint );
        printf( "----------------------------------\n" ); 

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            printf( "\nNot supported !\n" );
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.fPresSetPoint = fPresSetPoint;
          stGetFactoryDataStruct.fTempSetPoint = fTempSetPoint;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.fPresSetPoint = fPresSetPoint;
          stGetMirrorDataStruct.fTempSetPoint = fTempSetPoint;
          break; 
        default:
          break;
        }
        
      }
    }
    break;
    case IMB_FUNC_SET_MIRROR_AVT_ENABLE:
    {
      static unsigned char byAVTEnable = 0;
      byAVTEnable = (byAVTEnable + 1) % 2;
      if ( (nRetVal = oIMBComm.SetMirrorAVTEnable2( byAVTEnable )) < 0 )
      {
        printf("CIMBComm::SetMirrorAVTEnable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetMirrorAVTEnable() success. \n");
        printf( "Wrote Mirror AVT Enable - \n" );
        printf( "------------------\n" ); 
        printf( "AVT Enable = %d\n", byAVTEnable );
        printf( "------------------\n" ); 

        stSetMirrorDataStruct.byAVTEnabled = byAVTEnable;
      }
    }
    break;
    case IMB_FUNC_GET_MIRROR_AVT_ENABLE:
    {
      unsigned char byAVTEnable;
      if ( (nRetVal = oIMBComm.GetMirrorAVTEnable2( & byAVTEnable )) < 0 )
      {
        printf("CIMBComm::GetMirrorAVTEnable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetMirrorAVTEnable() success. \n");
        printf( "Read Mirror AVT Enable - \n" );
        printf( "------------------\n" ); 
        printf( "AVT Enable = %d\n", byAVTEnable );
        printf( "------------------\n" ); 

        stGetMirrorDataStruct.byAVTEnabled = byAVTEnable;
      }
    }
    break;

    case IMB_FUNC_SET_MIRROR_NUM_ANALYSIS:
    {
      static int nNumAnalysis = 0;
      nNumAnalysis++;
      if ( (nRetVal = oIMBComm.SetMirrorNumAnalysis2( nNumAnalysis )) < 0)
      {
        printf("CIMBComm::SetMirrorNumAnalysis() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetMirrorNumAnalysis() success. \n");
        printf( "Wrote Mirror Analysis Count - \n" );
        printf( "-------------------------------\n" ); 
        printf( "Number of Analysis = %d\n", nNumAnalysis );
        printf( "-------------------------------\n" ); 

        stSetMirrorDataStruct.nNumAnalysis = nNumAnalysis;
      }
    }
    break;

    case IMB_FUNC_GET_MIRROR_NUM_ANALYSIS:
    {
      int nNumAnalysis = 0;
      if ( (nRetVal = oIMBComm.GetMirrorNumAnalysis2( & nNumAnalysis )) < 0)
      {
        printf("CIMBComm::GetMirrorNumAnalysis() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetMirrorNumAnalysis() success. \n");
        printf( "Read Mirror Analysis Count - \n" );
        printf( "------------------------------\n" ); 
        printf( "Number of Analysis = %d\n", nNumAnalysis );
        printf( "------------------------------\n" ); 

        stGetMirrorDataStruct.nNumAnalysis = nNumAnalysis;
      }
    }
    break;

    case IMB_FUNC_SET_GLOBAL_DATA_INIT:
    {
      static unsigned char byIMBInit = 0;
      byIMBInit = (byIMBInit + 1) % 2;
      if ( (nRetVal = oIMBComm.SetGlobalDataIMBInit2( byIMBInit )) < 0)
      {
        printf("CIMBComm::SetGlobalDataIMBInit() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetGlobalDataIMBInit() success. \n");
        printf( "Wrote Global IMB Init Status - \n" );
        printf( "---------------\n" ); 
        printf( "IMB Init = %d\n", byIMBInit );
        printf( "---------------\n" ); 

        stSetGlobalDataStruct.byIMBInit = byIMBInit;
      }
    }
    break;

    case IMB_FUNC_GET_GLOBAL_DATA_INIT:
    {
      //IMB_GLOBAL_DATA_STRUCT stGlobalData;
      unsigned char byIMBInit = 0;

      if ( (nRetVal = oIMBComm.GetGlobalDataIMBInit2( & byIMBInit )) < 0)
      {
        printf("CIMBComm::GetGlobalData() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetGlobalData() success. \n");
        DisplayIMBGlobalDataIMBInit( byIMBInit );

        stGetGlobalDataStruct.byIMBInit = byIMBInit;
      }
    }
    break;

    case IMB_FUNC_SET_SECTION_CARRIER_CAL_FAQ:
    {
      float fZeroCalFaq = (float) rand();
      float fSpanCalFaq = (float) rand();
      if ( (nRetVal = oIMBComm.SetSectionCarrierCalFactors2( eIMBSelectedSection, fZeroCalFaq, fSpanCalFaq )) < 0 )
      {
        printf("CIMBComm::SetSectionCarrierCalFaq() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionCarrierCalFaq() success. \n");
        printf( "Wrote %s Carrier Calibration Factors - \n", szSectionName );
        printf( "----------------------------------\n" ); 
        printf( "fZeroCalFaq = %f\n", fZeroCalFaq );
        printf( "fSpanCalFaq = %f\n", fSpanCalFaq );
        printf( "----------------------------------\n" ); 

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            printf( "\nNot supported !\n" );
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.fCarrierPressZeroCalFaq = fZeroCalFaq;
          stSetFactoryDataStruct.fCarrierPressSpanCalFaq = fSpanCalFaq;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.fCarrierPressZeroCalFaq = fZeroCalFaq;
          stSetMirrorDataStruct.fCarrierPressSpanCalFaq = fSpanCalFaq;
          break; 
        default:
          break;
        }

      }
    }
    break;
    case IMB_FUNC_GET_SECTION_CARRIER_CAL_FAQ:
    {
      float fZeroCalFaq, fSpanCalFaq;
      if ( (nRetVal = oIMBComm.GetSectionCarrierCalFactors2( eIMBSelectedSection, & fZeroCalFaq, & fSpanCalFaq )) < 0 )
      {
        printf("CIMBComm::GetSectionCarrierCalFaq() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionCarrierCalFaq() success. \n");
        printf( "Read %s Carrier Calibration Factors - \n", szSectionName );
        printf( "----------------------------------\n" ); 
        printf( "fZeroCalFaq = %f\n", fZeroCalFaq );
        printf( "fSpanCalFaq = %f\n", fSpanCalFaq );
        printf( "----------------------------------\n" ); 

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            case IMB_DEFAULT_FLASH_SECTION:
            printf( "\nNot supported !\n" );
            break;
          */
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.fCarrierPressZeroCalFaq = fZeroCalFaq;
          stGetFactoryDataStruct.fCarrierPressSpanCalFaq = fSpanCalFaq;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.fCarrierPressZeroCalFaq = fZeroCalFaq;
          stGetMirrorDataStruct.fCarrierPressSpanCalFaq = fSpanCalFaq;
          break; 
        default:
          break;
        }

      }
    }
    break;

    /*
    case IMB_FUNC_SET_SECTION_SAMPLE_CAL_FAQ:
    {
      float fZeroCalFaq = (float) rand();
      float fSpanCalFaq = (float) rand();
      if ( (nRetVal = oIMBComm.SetSectionSampleCalFaq( eIMBSelectedSection, fZeroCalFaq, fSpanCalFaq )) < 0 ) 
      {
        printf("CIMBComm::SetSectionSampleCalFaq() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionSampleCalFaq() success. \n");
        printf( "Wrote %s Sample Calibration Factors - \n", szSectionName );
        printf( "----------------------------------\n" ); 
        printf( "fZeroCalFaq = %f\n", fZeroCalFaq );
        printf( "fSpanCalFaq = %f\n", fSpanCalFaq );
        printf( "----------------------------------\n" ); 

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          //  case IMB_DEFAULT_FLASH_SECTION:
          //  printf( "\nNot supported !\n" );
          //  break;
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.fSamplePressZeroCalFaq = fZeroCalFaq;
          stSetFactoryDataStruct.fSamplePressSpanCalFaq = fSpanCalFaq;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.fSamplePressZeroCalFaq = fZeroCalFaq;
          stSetMirrorDataStruct.fSamplePressSpanCalFaq = fSpanCalFaq;
          break; 
        default:
          break;
        }

      }
    }
    break;
    */

    /*
    case IMB_FUNC_GET_SECTION_SAMPLE_CAL_FAQ:
    {
      float fZeroCalFaq, fSpanCalFaq;
      if ( (nRetVal = oIMBComm.GetSectionSampleCalFaq( eIMBSelectedSection, & fZeroCalFaq, & fSpanCalFaq )) < 0 ) 
      {
        printf("CIMBComm::GetSectionSampleCalFaq() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionSampleCalFaq() success. \n");
        printf( "Read %s Sample Calibration Factors - \n", szSectionName );
        printf( "----------------------------------\n" ); 
        printf( "fZeroCalFaq = %f\n", fZeroCalFaq );
        printf( "fSpanCalFaq = %f\n", fSpanCalFaq );
        printf( "----------------------------------\n" ); 

        switch( eIMBSelectedSection )
        {
          // DEFAULT SECTION no longer used, wyu - May, 2014
          //  case IMB_DEFAULT_FLASH_SECTION:
          //  printf( "\nNot supported !\n" );
          //  break;
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.fSamplePressZeroCalFaq = fZeroCalFaq;
          stGetFactoryDataStruct.fSamplePressSpanCalFaq = fSpanCalFaq;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.fSamplePressZeroCalFaq = fZeroCalFaq;
          stGetMirrorDataStruct.fSamplePressSpanCalFaq = fSpanCalFaq;
          break; 
        default:
          break;
        }

      }
    }
    break;
    */
    case IMB_FUNC_SET_SECTION_CUSTOM_NAMES:
    {
      IMB_CDT_CUSTOM_NAME_STRUCT stCustomNames;
      PopulateCDTCustomNames( stCustomNames );
      if ( (nRetVal = oIMBComm.SetSectionCDTCustomNames2( eIMBSelectedSection, stCustomNames )) < 0 )
      {
        printf("CIMBComm::SetSectionCDTCustomNames() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionCDTCustomNames() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayCDTCustomNames( stCustomNames );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stCDTCustom = stCustomNames;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stSetMirrorDataStruct.stCDTCustom = stCustomNames;
          break;
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_CUSTOM_NAMES:
    {
      IMB_CDT_CUSTOM_NAME_STRUCT stCustomNames;
      if ( (nRetVal = oIMBComm.GetSectionCDTCustomNames2( eIMBSelectedSection, & stCustomNames )) < 0 )
      {
        printf("CIMBComm::GetSectionCDTCustomNames() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionCDTCustomNames() success. \n");
        printf( "Read %s", szSectionName );
        DisplayCDTCustomNames( stCustomNames );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stCDTCustom = stCustomNames;
          break;
        case IMB_MIRROR_FLASH_SECTION:
          stGetMirrorDataStruct.stCDTCustom = stCustomNames;
          break;
        default:
          break;
        }
      }
    }
    break;

    // Allowable DEV
    case IMB_FUNC_SET_SECTION_ALLOWABLE_DEV:
    {
      IMB_MV_ALLOWABLE_DEV_STRUCT stAllowableDEV;
      PopulateAllowableDEV2( stAllowableDEV );
      if ( (nRetVal = oIMBComm.SetSectionMVAllowableDEVTable2( eIMBSelectedSection, stAllowableDEV )) < 0 )
      {
        printf("CIMBComm::SetSectionMVAllowableDEVTable() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetSectionMVAllowableDEVTable() success. \n");
        printf( "Wrote %s", szSectionName );
        DisplayAllowableDEV2( stAllowableDEV );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stSetFactoryDataStruct.stMVAllowableDEV = stAllowableDEV;
          break;
        default:
          break;
        }
      }
    }
    break;
    case IMB_FUNC_GET_SECTION_ALLOWABLE_DEV:
    {
      IMB_MV_ALLOWABLE_DEV_STRUCT stAllowableDEV;
      if ( (nRetVal = oIMBComm.GetSectionMVAllowableDEVTable2( eIMBSelectedSection, & stAllowableDEV )) < 0 )
      {
        printf("CIMBComm::GetSectionCDTCustomNames() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionCDTCustomNames() success. \n");
        printf( "Read %s", szSectionName );
        DisplayAllowableDEV2( stAllowableDEV );

        switch( eIMBSelectedSection )
        {
        case IMB_FACTORY_FLASH_SECTION:
          stGetFactoryDataStruct.stMVAllowableDEV = stAllowableDEV;
          break;
        default:
          break;
        }
      }
    }
    break;

    case IMB_FUNC_GET_DB_ID:
    {
      IMB_STRUCTURE_ID_STRUCT stIMBDbId;
      if ( (nRetVal = oIMBComm.GetStructureID( & stIMBDbId )) < 0)
      {
        printf("CIMBComm::GetStructureID() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf( "DB ID - \n" ); 
        printf( "--------------------------------\n" ); 
        printf( "stIMBDbId.usMajor = %d\n", stIMBDbId.struMajor );
        printf( "stIMBDbId.usMinor = %d\n", stIMBDbId.struMinor );
        printf( "--------------------------------\n" ); 
      }
    }
    break;

    case IMB_FUNC_GET_IMB_INFO:
    {
      IMB_IMB_META_INFO stIMBInfo;
      if ( (nRetVal = oIMBComm.GetIMBInfo( & stIMBInfo )) < 0)
      {
        printf("CIMBComm::GetIMBInfo() failed : %d !!!\n", nRetVal);
      }
      else
      {
        std::string strIMBSerialInHex;
        int i32Convert =0;
        if ( (i32Convert = oIMBComm.ConvertIMBSerialNumberToHex( stIMBInfo, strIMBSerialInHex )) < 0)
        {
          printf("CIMBComm::ConvertIMBSerialNumberToHex() failed : %d !!!\n", i32Convert);
        }
        

        /*
          printf( "IMB Serial Num  - \n" ); 
          printf( "------------------------------\n" ); 

          printf( "byIMBSerialNumber (CHAR) = %.*s\n", IMB_SERIAL_NUMBER_LENGTH, stIMBInfo.byIMBSerialNum );
          printf( "byIMBSerialNumber (HEX)  = " );
          for( int nInd = 0; nInd < IMB_SERIAL_NUMBER_LENGTH; nInd++ )
          {
          printf( "0x%X ", stIMBInfo.byIMBSerialNum[nInd] );
          }
          printf( "\n");


          FixEndian(stIMBInfo.stIMBStruId.struMajor);
          FixEndian(stIMBInfo.stIMBStruId.struMinor);

          printf( "------------------------------\n" ); 
          printf( "IMB Structure ID major = %u, minor = %u\n", stIMBInfo.stIMBStruId.struMajor, stIMBInfo.stIMBStruId.struMinor );

          FixEndian(stIMBInfo.stIMBApiId.apiMajor);
          FixEndian(stIMBInfo.stIMBApiId.apiMinor);

          printf( "------------------------------\n" ); 
          printf( "IMB API ID major = %u, minor = %u\n", stIMBInfo.stIMBApiId.apiMajor, stIMBInfo.stIMBApiId.apiMinor );
        */

      }
    }
    break;


    case IMB_FUNC_UPDATE_FLASH:
    {
      if ( (nRetVal = oIMBComm.UpdateFlash( )) < 0)
      {
        printf("CIMBComm::UpdateFlash() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::UpdateFlash() sucessful : %d !!!\n", nRetVal);
      }
    }
    break;


    case IMB_FUNC_SET_FLASH_IMAGE:
    {
      unsigned int nFlashSection =0;
      unsigned int nFlashOffset = 0;
      unsigned int nFlashLen    = 0;

      printf( "Enter the section to write (0:GLOBAL 1:FACTORY 2:MIRROR) : " );
      fflush(stdin);
      scanf("%u", &nFlashSection );


      printf( "Enter the offset to write (hexdecimal) : " );
      fflush(stdin);
      scanf("%x", &nFlashOffset );

      printf( "Enter the length to write (hexdecimal, max = 0x%x): ", FLASH_IMAGE_DATA_LENGTH );
      fflush(stdin);
      scanf("%x", &nFlashLen );

      unsigned char flashData[FLASH_IMAGE_DATA_LENGTH];

      for(unsigned int ui = 0; ui < nFlashLen; ui ++)
      {
        unsigned int data = 0;
        printf( "Enter data to write in hex (offset hexdecimal = 0x%x): ", ui );
        fflush(stdin);
        scanf("%x", & data );
        flashData[ui] = (unsigned char) data;
      }

      if ( (nRetVal = oIMBComm.SetFlashImage( flashData, (unsigned char) nFlashSection, nFlashOffset, nFlashLen )) < 0)
      {
        printf("CIMBComm::GetFlashImage() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf( "Flash Image - \n" ); 

        unsigned int flashLenRead = nFlashLen;
        printf( "--------------------------------\n" ); 
        printf( "flash Len to write = %d\n", flashLenRead );
        if(flashLenRead >= 0)
        {
          const int NUM_PER_LINE = 8;
          printf( "flash Data " );
          for(int i =0; i< (int)flashLenRead; i ++)
          {
            if(i % NUM_PER_LINE == 0)
            {
              printf( "\n" );
              printf( "0x%04X :", (nFlashOffset + i) );
            }

            printf("%02X ", flashData[i]);

          }
        }
        printf( "\n" );
        printf( "--------------------------------\n" );
      }
    }
    break;


    case IMB_FUNC_GET_FLASH_IMAGE:
    {
      unsigned int nFlashSection =0;
      unsigned int nFlashOffset = 0;
      unsigned int nFlashLen    = 0;

      printf( "Enter the section to read (0:GLOBAL 1:FACTORY 2:MIRROR) : " );
      fflush(stdin);
      scanf("%u", &nFlashSection );


      printf( "Enter the offset to read (hexdecimal) : " );
      fflush(stdin);
      scanf("%x", &nFlashOffset );

      printf( "Enter the length to read (hexdecimal, max = 0x%x): ", FLASH_IMAGE_DATA_LENGTH );
      fflush(stdin);
      scanf("%x", &nFlashLen );

      unsigned char flashData[FLASH_IMAGE_DATA_LENGTH];
      unsigned int flashLenRead = 0;
      if ( (nRetVal = oIMBComm.GetFlashImage( flashData, (unsigned char) nFlashSection, nFlashOffset, nFlashLen, flashLenRead )) < 0)
      {
        printf("CIMBComm::GetFlashImage() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf( "Flash Image - \n" ); 
        printf( "--------------------------------\n" ); 
        printf( "flash Len Read = %d\n", flashLenRead );
        if(flashLenRead >= 0)
        {
          const int NUM_PER_LINE = 8;
          printf( "flash Data " );
          for(int i =0; i< (int)flashLenRead; i ++)
          {
            if(i % NUM_PER_LINE == 0)
            {
              printf( "\n" );
              printf( "0x%04X :", (nFlashOffset + i) );
            }

            printf("%02X ", flashData[i]);

          }
        }
        printf( "\n" );
        printf( "--------------------------------\n" );
      }
    }
    break;


//NEW

    case IMB_FUNC_SET_MODULE_SERIAL_NUM:
    {
      unsigned char chDummy[MODULE_SERIAL_NUMBER_LENGTH+1];
      GenerateRandomString(chDummy, MODULE_SERIAL_NUMBER_LENGTH + 1);

      if ( (nRetVal = oIMBComm.SetGlobalDataModuleSerialNum2(chDummy )) < 0)
      {
        printf("CIMBComm::SetFlashImage() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetFlashImage() success. \n");
        printf( "Wrote Module Serial Num  - \n" );
        printf( "-------------------------------\n" ); 
        printf( "Module Serial Num = %.*s\n", MODULE_SERIAL_NUMBER_LENGTH, chDummy );
        printf( "-------------------------------\n" ); 

        //stSetMirrorDataStruct.nNumAnalysis = nNumAnalysis;
      }

    }
    break;


    case IMB_FUNC_GET_MODULE_SERIAL_NUM:
    {
      unsigned char chDummy[MODULE_SERIAL_NUMBER_LENGTH+1];

      unsigned int flashLenRead = 0;
      if ( (nRetVal = oIMBComm.GetGlobalDataModuleSerialNum2(chDummy )) < 0)
      {
        printf("CIMBComm::GetDBID() failed : %d !!!\n", nRetVal);
      }
      else // if(flashLenRead >0 && flashLenRead <= MODULE_SERIAL_NUMBER_LENGTH  )
      {
        chDummy[MODULE_SERIAL_NUMBER_LENGTH+1] = '\0';
        printf( "--------------------------------\n" ); 
        printf( "Module Serial Num = %.*s\n", flashLenRead, chDummy );
        printf( "--------------------------------\n" ); 
      }
    }
    break;

    case IMB_FUNC_SET_GLOBAL_META_INFO:
    {
      IMB_SECTION_META_INFO2 stMetaInfo;
      PopulateIMBMetaInfo2( stMetaInfo );
      if ( (nRetVal = oIMBComm.SetGlobalDataMetaInfo2( stMetaInfo )) < 0 )
      {
        printf("CIMBComm::SetGlobalDataMetaInfo() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetGlobalDataMetaInfo() success.\n");
        printf( "Wrote %s", szSectionName );
        DisplayIMBMetaInfo2( stMetaInfo );

        stSetGlobalDataStruct.stMetaInfo = stMetaInfo;
      }
    }
    break;
    case IMB_FUNC_GET_GLOBAL_META_INFO:
    {
      IMB_SECTION_META_INFO2 stMetaInfo;
      if ( (nRetVal = oIMBComm.GetGlobalDataMetaInfo2( & stMetaInfo )) < 0 )
      {
        printf("CIMBComm::GetGlobalDataMetaInfo() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionMetaInfo() success.\n");
        printf( "Read %s", szSectionName );
        DisplayIMBMetaInfo2( stMetaInfo );

        stGetGlobalDataStruct.stMetaInfo = stMetaInfo;
      }
    }
    break;

    // Application-Type
    case IMB_FUNC_SET_GLOBAL_APP_TYPE:
    {
      IMB_APPLICATION_TYPE_STRUCT stApplicationType;
      PopulateApplicationType2( stApplicationType );
      if ( (nRetVal = oIMBComm.SetGlobalDataApplicationType2( stApplicationType )) < 0 )
      {
        printf("CIMBComm::SetGlobalApplicationType() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::SetGlobalApplicationType() success.\n");
        printf( "Wrote %s", szSectionName );
        DisplayApplicationType2( stApplicationType );

        stSetGlobalDataStruct.stApplicationType = stApplicationType;
      }
    }
    break;
    case IMB_FUNC_GET_GLOBAL_APP_TYPE:
    {
      IMB_APPLICATION_TYPE_STRUCT stApplicationType;
      if ( (nRetVal = oIMBComm.GetGlobalDataApplicationType2( & stApplicationType )) < 0 )
      {
        printf("CIMBComm::GetGlobalDataMetaInfo() failed : %d !!!\n", nRetVal);
      }
      else
      {
        printf("CIMBComm::GetSectionMetaInfo() success.\n");
        printf( "Read %s", szSectionName );
        DisplayApplicationType2( stApplicationType );

        stGetGlobalDataStruct.stApplicationType = stApplicationType;
      }
    }
    break;

    case IMB_FUNC_SET_ALL_DATA_SECTIONS:
    {

      memset(& stSetFactoryDataStruct, 0, sizeof(IMB_FACTORY_DATA_STRUCT2));
      memset(& stGetFactoryDataStruct, 0, sizeof(IMB_FACTORY_DATA_STRUCT2));

      memset(& stSetMirrorDataStruct, 0, sizeof(IMB_MIRROR_DATA_STRUCT2));
      memset(& stGetMirrorDataStruct, 0, sizeof(IMB_MIRROR_DATA_STRUCT2));

      memset(& stSetGlobalDataStruct, 0, sizeof(IMB_GLOBAL_DATA_STRUCT2));
      memset(& stGetGlobalDataStruct, 0, sizeof(IMB_GLOBAL_DATA_STRUCT2));

      v_stIMBCommands.clear();

      // DEFAULT SECTION no longer used, wyu - May, 2014
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_META_INFO ,  0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_META_INFO ,  1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_META_INFO ,  2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_AVT_SET_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_AVT_SET_TABLE, 1 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_AVT_VSEL_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_AVT_VSEL_TABLE, 1 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_AVT_TEADJ_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_AVT_TEADJ_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_AVT_TEADJ_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CDT_CONFIG, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CDT_CONFIG, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CDT_CONFIG, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CDT_UPDATE, 0 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CDT_UPDATE, 1 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CDT_UPDATE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_HTEV_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_HTEV_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_HTEV_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_STEV_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_STEV_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_STEV_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_GTEV_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_GTEV_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_GTEV_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_ANLY_CFG_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_ANLY_CFG_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_ANLY_CFG_TABLE, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_SET_POINTS, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_SET_POINTS, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CARRIER_CAL_FAQ, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CARRIER_CAL_FAQ, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_SAMPLE_CAL_FAQ, 1 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_SAMPLE_CAL_FAQ, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CUSTOM_NAMES, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_CUSTOM_NAMES, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_SECTION_ALLOWABLE_DEV, 1 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CUSTOM_NAMES, 2 ) );

      // MIRROR
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_MIRROR_AVT_ENABLE, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_MIRROR_NUM_ANALYSIS, -1 ) );

      // GLOBAL
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_GLOBAL_DATA_INIT, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_MODULE_SERIAL_NUM, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_GLOBAL_META_INFO, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_SET_GLOBAL_APP_TYPE, -1 ) );



      bAutomatedTest = true;
      nAutomatedTestInd = -1;
    }
    break;

    case IMB_FUNC_GET_ALL_DATA_SECTIONS:
    {
      v_stIMBCommands.clear();

      // DEFAULT SECTION no longer used, wyu - May, 2014
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_META_INFO ,  0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_META_INFO ,  1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_META_INFO ,  2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_AVT_SET_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_AVT_SET_TABLE, 1 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_AVT_VSEL_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_AVT_VSEL_TABLE, 1 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_AVT_TEADJ_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_AVT_TEADJ_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_AVT_TEADJ_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CDT_CONFIG, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CDT_CONFIG, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CDT_CONFIG, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CDT_UPDATE, 0 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CDT_UPDATE, 1 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CDT_UPDATE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_HTEV_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_HTEV_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_HTEV_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_STEV_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_STEV_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_STEV_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_GTEV_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_GTEV_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_GTEV_TABLE, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_ANLY_CFG_TABLE, 0 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_ANLY_CFG_TABLE, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_ANLY_CFG_TABLE, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_SET_POINTS, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_SET_POINTS, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CARRIER_CAL_FAQ, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CARRIER_CAL_FAQ, 2 ) );

      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_SAMPLE_CAL_FAQ, 1 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_SAMPLE_CAL_FAQ, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CUSTOM_NAMES, 1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CUSTOM_NAMES, 2 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_ALLOWABLE_DEV, 1 ) );
      //v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_SECTION_CUSTOM_NAMES, 2 ) );

      // MIRROR
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_MIRROR_AVT_ENABLE, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_MIRROR_NUM_ANALYSIS, -1 ) );

      // GLOBAL
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_GLOBAL_DATA_INIT, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_MODULE_SERIAL_NUM, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_GLOBAL_META_INFO, -1 ) );
      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_GLOBAL_APP_TYPE, -1 ) );

      v_stIMBCommands.push_back( stIMBCommand( IMB_FUNC_GET_DB_ID, -1 ) );

      bAutomatedTest = true;
      bAutomatedReadTest = true;
      nAutomatedTestInd = -1;
    }
    break;

    case IMB_FUNC_RESET_ALL_DATA_SECTIONS:
    {
      IMB_FACTORY_DATA_STRUCT2 stTempFactoryDataStruct;
      IMB_MIRROR_DATA_STRUCT2  stTempMirrorDataStruct;
      IMB_GLOBAL_DATA_STRUCT2  stTempGlobalDataStruct;

      const int c = 0XFF;  // init flash with 0XFF
      memset(& stTempFactoryDataStruct, c, sizeof(IMB_FACTORY_DATA_STRUCT2));
      memset(& stTempMirrorDataStruct, c, sizeof(IMB_MIRROR_DATA_STRUCT2));
      memset(& stTempGlobalDataStruct, c, sizeof(IMB_GLOBAL_DATA_STRUCT2));

      nRetVal = oIMBComm.SetFlashImage2( (unsigned char *) & stTempFactoryDataStruct, IMB_FACTORY_FLASH_SECTION, 0, sizeof(IMB_FACTORY_DATA_STRUCT2) );
      if(nRetVal < 0)
        printf("CIMBComm::SetFlashImage2() failed : %d !!!\n", nRetVal);

      if(nRetVal >= 0)
      {
        nRetVal = oIMBComm.SetFlashImage2( (unsigned char *) & stTempMirrorDataStruct, IMB_MIRROR_FLASH_SECTION, 0, sizeof(IMB_MIRROR_DATA_STRUCT2) );
        if(nRetVal < 0)
          printf("CIMBComm::SetFlashImage2() failed : %d !!!\n", nRetVal);
      }

      if(nRetVal >= 0)
      {
        nRetVal = oIMBComm.SetFlashImage2( (unsigned char *) & stTempGlobalDataStruct, IMB_GLOBAL_FLASH_SECTION, 0, sizeof(IMB_GLOBAL_DATA_STRUCT2) );
        if(nRetVal < 0)
          printf("CIMBComm::SetFlashImage2() failed : %d !!!\n", nRetVal);
      }

      if(nRetVal >= 0)
        printf("Reseting All Data Sections success. \n");

    }
    break;

    default:
      printf("Invalid function... try again.\n");
      fflush(stdin);
      break;
    }


    if( !bAutomatedTest )
    {
      printf( "press ENTER key to continue..." ); 
      fflush(stdin);
      int nDummy1 = getchar();
      int nDummy2 = getchar();

      nDummy1 = 0;   //to silence the warning
      nDummy2 = 0;   //to silence the warning
    }
    else
    {
      nAutomatedTestInd++;
      if( nAutomatedTestInd >= (int) v_stIMBCommands.size() )
      {
        if( bAutomatedReadTest )
        {
          bAutomatedReadTest = false;

          printf( "***************************************************\n" ); 
          printf( "***************************************************\n" ); 

          //reset the reserved fields (they are not set by the set or read commands!!!)
          // DEFAULT SECTION no longer used, wyu - May, 2014
          /*
            memset( stSetDefaultDataStruct.reserved, 0, RESERVED_FIELD_LENGTH);
            memset( stGetDefaultDataStruct.reserved, 0, RESERVED_FIELD_LENGTH);
            if( memcmp( & stSetDefaultDataStruct, 
            & stGetDefaultDataStruct, 
            sizeof(IMB_DEFAULT_DATA_STRUCT)) == 0 )
            {
            printf( "Default Data Section - Read and Verify Success :)\n" );
            }
            else
            {
            printf( "Default Data Section - Read and Verify Failure !!!\n" );
            TestIMBDefaultData(stSetDefaultDataStruct, stGetDefaultDataStruct);
            }
          */

          //reset the reserved fields (they are not set by the set or read commands!!!)
          //memset( stSetFactoryDataStruct.reserved, 0, RESERVED_FIELD_LENGTH);
          //memset( stGetFactoryDataStruct.reserved, 0, RESERVED_FIELD_LENGTH);
          if( memcmp( & stSetFactoryDataStruct,
                      & stGetFactoryDataStruct,
                      sizeof(IMB_FACTORY_DATA_STRUCT2)) == 0 )
          {
            printf( "Factory Data Section - Read and Verify Success :)\n" );
          }
          else
          {
            printf( "Factory Data Section - Read and Verify Failure !!!\n" );
            TestIMBFactoryData(stSetFactoryDataStruct, stGetFactoryDataStruct);
          }

          //reset the reserved fields (they are not set by the set or read commands!!!)
          //memset( stSetMirrorDataStruct.reserved, 0, RESERVED_FIELD_LENGTH);
          //memset( stGetMirrorDataStruct.reserved, 0, RESERVED_FIELD_LENGTH);
          if( memcmp( & stSetMirrorDataStruct,
                      & stGetMirrorDataStruct,
                      sizeof(IMB_MIRROR_DATA_STRUCT2)) == 0 )
          {
            printf( "Mirror Data Section - Read and Verify Success :)\n" );
          }
          else
          {
            printf( "Mirror Data Section - Read and Verify Failure !!!\n" );
            TestIMBMirrorData(stSetMirrorDataStruct, stGetMirrorDataStruct);
          }

          //if( bySetGlobalIMBInit == byGetGlobalIMBInit )
          if( memcmp( & stSetGlobalDataStruct,
                      & stGetGlobalDataStruct,
                      sizeof(IMB_GLOBAL_DATA_STRUCT2)) == 0 )
          {
            printf( "Global Data Section - Read and Verify Success :)\n" );
          }
          else
          {
            printf( "Global Data Section - Read and Verify Failure !!!\n" );
            TestIMBGlobalData(stSetGlobalDataStruct, stGetGlobalDataStruct);
          }
          printf( "***************************************************\n" ); 
          printf( "***************************************************\n" ); 
        }

        v_stIMBCommands.clear();
        bAutomatedTest = false;
        nAutomatedTestInd = 0;
      }
    }
  }

  nRetVal = oIMBComm.CloseHal();
  if (nRetVal < 0)
  {
    printf("CIMBComm.Close() failed: %d !!!\n", nRetVal);
  }
}


// DEFAULT SECTION no longer used, wyu - May, 2014
/*
  void TestIMBDefaultData(IMB_DEFAULT_DATA_STRUCT& stSetDataStruct, IMB_DEFAULT_DATA_STRUCT& stGetDataStruct)
  {

  if( memcmp( & stSetDataStruct.stMetaInfo, 
  & stGetDataStruct.stMetaInfo, 
  sizeof(IMB_SECTION_META_INFO)) == 0 )
  {
  printf( "Default Data Section, Meta Data - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, Meta Data - Read and Verify Failure !!!\n" );
  }

  //AVT
  if( memcmp( & stSetDataStruct.stAVTSettings, 
  & stGetDataStruct.stAVTSettings, 
  sizeof(IMB_AVT_SETTINGS_TABLE_STRUCT)) == 0 )
  {
  printf( "Default Data Section, AVT settings - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, AVT settings - Read and Verify Failure !!!\n" );
  }

  if( memcmp( & stSetDataStruct.stAVTValveSel, 
  & stGetDataStruct.stAVTValveSel, 
  sizeof(IMB_AVT_VALVE_SEL_TABLE_STRUCT)) == 0 )
  {
  printf( "Default Data Section, Valve Selection - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, Valse Selection - Read and Verify Failure !!!\n" );
  }

  //CDT
  if( memcmp( & stSetDataStruct.stCDT, 
  & stGetDataStruct.stCDT, 
  sizeof(IMB_CDT_TABLE_STRUCT)) == 0 )
  {
  printf( "Default Data Section, CDT - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, CDT - Read and Verify Failure !!!\n" );
  }

  if( memcmp( & stSetDataStruct.stAVTTimeEventAdj, 
  & stGetDataStruct.stAVTTimeEventAdj, 
  sizeof(IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT)) == 0 )
  {
  printf( "Default Data Section, Time Event Adj - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, Time Event Adj - Read and Verify Failure !!!\n" );
  }

  //TEV
  if( memcmp( & stSetDataStruct.stHwTEV, 
  & stGetDataStruct.stHwTEV, 
  sizeof(IMB_HW_TEV_TABLE_STRUCT)) == 0 )
  {
  printf( "Default Data Section, HW TEV - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, HW TEV - Read and Verify Failure !!!\n" );
  }

  if( memcmp( & stSetDataStruct.stSwTEV, 
  & stGetDataStruct.stSwTEV, 
  sizeof(IMB_SW_TEV_TABLE_STRUCT)) == 0 )
  {
  printf( "Default Data Section, SW TEV - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, SW TEV - Read and Verify Failure !!!\n" );
  }

  if( memcmp( & stSetDataStruct.stGainTEV, 
  & stGetDataStruct.stGainTEV, 
  sizeof(IMB_GAIN_TEV_TABLE_STRUCT)) == 0 )
  {
  printf( "Default Data Section, Gain TEV - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, Gain TEV - Read and Verify Failure !!!\n" );
  }

  if( memcmp( & stSetDataStruct.stAnlyTimeCfg, 
  & stGetDataStruct.stAnlyTimeCfg, 
  sizeof(IMB_ANALYSIS_TIME_CONFIG_STRUCT)) == 0 )
  {
  printf( "Default Data Section, Analy Time TEV - Read and Verify Success :)\n" );
  }
  else
  {
  printf( "Default Data Section, Analy Time TEV - Read and Verify Failure !!!\n" );
  }


  }
*/

void TestIMBFactoryData(IMB_FACTORY_DATA_STRUCT2 & stSetDataStruct, IMB_FACTORY_DATA_STRUCT2 & stGetDataStruct)
{

  if( memcmp( & stSetDataStruct.stMetaInfo, 
              & stGetDataStruct.stMetaInfo, 
              sizeof(IMB_SECTION_META_INFO)) == 0 )
  {
    printf( "Factory Data Section, Meta Data - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Meta Data - Read and Verify Failure !!!\n" );
  }

  //TEV
  if( memcmp( & stSetDataStruct.stHwTEV, 
              & stGetDataStruct.stHwTEV, 
              sizeof(IMB_HW_TEV_TABLE_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, HW TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, HW TEV - Read and Verify Failure !!!\n" );
  }

  // SW-TEV
  if( memcmp( & stSetDataStruct.stSwTEV, 
              & stGetDataStruct.stSwTEV, 
              sizeof(IMB_SW_TEV_TABLE_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, SW TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, SW TEV - Read and Verify Failure !!!\n" );
  }

  // EXT
  if( memcmp( & stSetDataStruct.stSwTEVExt,
              & stGetDataStruct.stSwTEVExt,
              sizeof(IMB_SW_TEV_TABLE_STRUCT_EXT)) == 0 )
  {
    printf( "Factory Data Section, SW TEV EXT - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, SW TEV EXT - Read and Verify Failure !!!\n" );
  }

  // Gain - TEV
  if( memcmp( & stSetDataStruct.stGainTEV, 
              & stGetDataStruct.stGainTEV, 
              sizeof(IMB_GAIN_TEV_TABLE_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, Gain TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Gain TEV - Read and Verify Failure !!!\n" );
  }

  if( memcmp( & stSetDataStruct.stAnlyTimeCfg, 
              & stGetDataStruct.stAnlyTimeCfg, 
              sizeof(IMB_ANALYSIS_TIME_CONFIG_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, Analy Time TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Analy Time TEV - Read and Verify Failure !!!\n" );
  }

  //CDT
  if( memcmp( & stSetDataStruct.stCDT, 
              & stGetDataStruct.stCDT, 
              sizeof(IMB_CDT_TABLE_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, CDT - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, CDT - Read and Verify Failure !!!\n" );
  }

  //EXT
  if( memcmp( & stSetDataStruct.stCDTExt,
              & stGetDataStruct.stCDTExt,
              sizeof(IMB_CDT_TABLE_STRUCT_EXT)) == 0 )
  {
    printf( "Factory Data Section, CDT EXT - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, CDT EXT - Read and Verify Failure !!!\n" );
  }

  // AVT Timed-Event ADJ
  if( memcmp( & stSetDataStruct.stAVTTimeEventAdj, 
              & stGetDataStruct.stAVTTimeEventAdj, 
              sizeof(IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2)) == 0 )
  {
    printf( "Factory Data Section, Time Event Adj - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Time Event Adj - Read and Verify Failure !!!\n" );
  }

  // EXT
  if( memcmp( & stSetDataStruct.stAVTTimeEventAdjExt,
              & stGetDataStruct.stAVTTimeEventAdjExt,
              sizeof(IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT)) == 0 )
  {
    printf( "Factory Data Section, Time Event Adj EXT - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Time Event Adj EXT - Read and Verify Failure !!!\n" );
  }


  //AVT Settings
  if( memcmp( & stSetDataStruct.stAVTSettings, 
              & stGetDataStruct.stAVTSettings, 
              sizeof(IMB_AVT_SETTINGS_TABLE_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, AVT settings - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, AVT settings - Read and Verify Failure !!!\n" );
  }

  //AVT Valve-Sel
  if( memcmp( & stSetDataStruct.stAVTValveSel, 
              & stGetDataStruct.stAVTValveSel, 
              sizeof(IMB_AVT_VALVE_SEL_TABLE_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, Valve Selection - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Valse Selection - Read and Verify Failure !!!\n" );
  }

  //Set points
  if( memcmp( & stSetDataStruct.fPresSetPoint,
              & stGetDataStruct.fPresSetPoint,
              sizeof(stSetDataStruct.fPresSetPoint) + sizeof(stSetDataStruct.fTempSetPoint)) == 0 )
  {
    printf( "Factory Data Section, Set Points - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Set Points - Read and Verify Failure !!!\n" );
  }

  //Carrier Calib Factors
  if( memcmp( & stSetDataStruct.fCarrierPressZeroCalFaq,
              & stGetDataStruct.fCarrierPressZeroCalFaq,
              sizeof(stSetDataStruct.fCarrierPressZeroCalFaq) + sizeof(stSetDataStruct.fCarrierPressSpanCalFaq)) == 0 )
  {
    printf( "Factory Data Section, Carrier Calib Factors - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Carrier Calib Factors - Read and Verify Failure !!!\n" );
  }

  //Sample Calib Factors
  if( memcmp( & stSetDataStruct.fSamplePressZeroCalFaq,
              & stGetDataStruct.fSamplePressZeroCalFaq,
              sizeof(stSetDataStruct.fSamplePressZeroCalFaq) + sizeof(stSetDataStruct.fSamplePressSpanCalFaq)) == 0 )
  {
    printf( "Factory Data Section, Sample Calib Factors - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Sample Calib Factors - Read and Verify Failure !!!\n" );
  }

  // Custom-Names
  if( memcmp( & stSetDataStruct.stCDTCustom,
              & stGetDataStruct.stCDTCustom,
              sizeof(IMB_CDT_CUSTOM_NAME_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, Custom Names - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Custom Names - Read and Verify Failure !!!\n" );
  }

  // Allowable-DEV
  if( memcmp( & stSetDataStruct.stMVAllowableDEV,
              & stGetDataStruct.stMVAllowableDEV,
              sizeof(IMB_MV_ALLOWABLE_DEV_STRUCT)) == 0 )
  {
    printf( "Factory Data Section, Allowable DEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Factory Data Section, Allowable DEV - Read and Verify Failure !!!\n" );
  }

}


void TestIMBMirrorData(IMB_MIRROR_DATA_STRUCT2 & stSetDataStruct, IMB_MIRROR_DATA_STRUCT2 & stGetDataStruct)
{

  if( memcmp( & stSetDataStruct.stMetaInfo, 
              & stGetDataStruct.stMetaInfo, 
              sizeof(IMB_SECTION_META_INFO)) == 0 )
  {
    printf( "Mirror Data Section, Meta Data - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, Meta Data - Read and Verify Failure !!!\n" );
  }

  //TEV
  if( memcmp( & stSetDataStruct.stHwTEV, 
              & stGetDataStruct.stHwTEV, 
              sizeof(IMB_HW_TEV_TABLE_STRUCT)) == 0 )
  {
    printf( "Mirror Data Section, HW TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, HW TEV - Read and Verify Failure !!!\n" );
  }

  // SW-TEV
  if( memcmp( & stSetDataStruct.stSwTEV, 
              & stGetDataStruct.stSwTEV, 
              sizeof(IMB_SW_TEV_TABLE_STRUCT)) == 0 )
  {
    printf( "Mirror Data Section, SW TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, SW TEV - Read and Verify Failure !!!\n" );
  }

  // EXT
  if( memcmp( & stSetDataStruct.stSwTEVExt,
              & stGetDataStruct.stSwTEVExt,
              sizeof(IMB_SW_TEV_TABLE_STRUCT_EXT)) == 0 )
  {
    printf( "Mirror Data Section, SW TEV EXT - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, SW TEV EXT - Read and Verify Failure !!!\n" );
  }


  if( memcmp( & stSetDataStruct.stGainTEV, 
              & stGetDataStruct.stGainTEV, 
              sizeof(IMB_GAIN_TEV_TABLE_STRUCT)) == 0 )
  {
    printf( "Mirror Data Section, Gain TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, Gain TEV - Read and Verify Failure !!!\n" );
  }

  if( memcmp( & stSetDataStruct.stAnlyTimeCfg, 
              & stGetDataStruct.stAnlyTimeCfg, 
              sizeof(IMB_ANALYSIS_TIME_CONFIG_STRUCT)) == 0 )
  {
    printf( "Mirror Data Section, Analy Time TEV - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, Analy Time TEV - Read and Verify Failure !!!\n" );
  }

  //CDT
  if( memcmp( & stSetDataStruct.stCDT, 
              & stGetDataStruct.stCDT, 
              sizeof(IMB_CDT_TABLE_STRUCT)) == 0 )
  {
    printf( "Mirror Data Section, CDT - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, CDT - Read and Verify Failure !!!\n" );
  }

  // EXT
  if( memcmp( & stSetDataStruct.stCDTExt,
              & stGetDataStruct.stCDTExt,
              sizeof(IMB_CDT_TABLE_STRUCT_EXT)) == 0 )
  {
    printf( "Mirror Data Section, CDT EXT - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, CDT EXT - Read and Verify Failure !!!\n" );
  }


  //Set points
  if( memcmp( & stSetDataStruct.fPresSetPoint,
              & stGetDataStruct.fPresSetPoint,
              sizeof(stSetDataStruct.fPresSetPoint) + sizeof(stSetDataStruct.fTempSetPoint)) == 0 )
  {
    printf( "Mirror Data Section, Set Points - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, Set Points - Read and Verify Failure !!!\n" );
  }

  //Carrier Calib Factors
  if( memcmp( & stSetDataStruct.fCarrierPressZeroCalFaq,
              & stGetDataStruct.fCarrierPressZeroCalFaq,
              sizeof(stSetDataStruct.fCarrierPressZeroCalFaq) + sizeof(stSetDataStruct.fCarrierPressSpanCalFaq)) == 0 )
  {
    printf( "Mirror Data Section, Carrier Calib Factors - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, Carrier Calib Factors - Read and Verify Failure !!!\n" );
  }

  //Sample Calib Factors
  if( memcmp( & stSetDataStruct.fSamplePressZeroCalFaq,
              & stGetDataStruct.fSamplePressZeroCalFaq,
              sizeof(stSetDataStruct.fSamplePressZeroCalFaq) + sizeof(stSetDataStruct.fSamplePressSpanCalFaq)) == 0 )
  {
    printf( "Mirror Data Section, Sample Calib Factors - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Mirror Data Section, Sample Calib Factors - Read and Verify Failure !!!\n" );
  }
}

void TestIMBGlobalData( IMB_GLOBAL_DATA_STRUCT2  & stSetDataStruct, IMB_GLOBAL_DATA_STRUCT2  & stGetDataStruct)
{
  // Init
  if( memcmp( & stSetDataStruct.byIMBInit,
              & stGetDataStruct.byIMBInit,
              sizeof(unsigned char)) == 0 )
  {
    printf( "Global Data Section, IMB Init - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Global Data Section, IMB Init - Read and Verify Failure !!!\n" );
  }

  // Module Serial Number
  if( memcmp( & stSetDataStruct.byModuleSerialNum,
              & stGetDataStruct.byModuleSerialNum,
              sizeof(unsigned char) * MODULE_SERIAL_NUMBER_LENGTH ) == 0 )
  {
    printf( "Global Data Section, Module Serial Number - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Global Data Section, Module Serial Number - Read and Verify Failure !!!\n" );
  }

  // Meta-Info
  if( memcmp( & stSetDataStruct.stMetaInfo,
              & stGetDataStruct.stMetaInfo,
              sizeof(IMB_SECTION_META_INFO2) ) == 0 )
  {
    printf( "Global Data Section, Meta-Info - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Global Data Section, Meta-Info - Read and Verify Failure !!!\n" );
  }

  // Application-Type
  if( memcmp( & stSetDataStruct.stApplicationType,
              & stGetDataStruct.stApplicationType,
              sizeof(IMB_APPLICATION_TYPE_STRUCT) ) == 0 )
  {
    printf( "Global Data Section, Application-Type - Read and Verify Success :)\n" );
  }
  else
  {
    printf( "Global Data Section, Application-Type - Read and Verify Failure !!!\n" );
  }

}


//Get the stream data from FFB board.
void GetRemoteRequest4mFFBBoard()
{
  int nRetVal = 0;
  char szFFBBoardName[]  = "FFB_COMMAND:FIELDBUS_SLOT:FFB_COMMAND_1";
  CFFBCmmd obFFBCmmd;
  printf("Opening FFB_COMMAND connection.\n");
  nRetVal = obFFBCmmd.OpenHal( szFFBBoardName );
  if( nRetVal >= 0 )
  {
    printf("Successfully opened FFB_COMMAND connection.\n");

    unsigned int unRxCRCErrCounter = 0;
    unsigned int unRxProtocolErrCounter = 0;
    unsigned int unRxUnknownErrCounter = 0;
    unsigned int unRxZeroDataCounter = 0;
    do
    {
      //Receive the data then check whether the same data or not and log errors.
      REMOTE_REQUEST_STRUCT stRemoteRequest;
      REMOTE_REQUEST_TYPE eRemoteRequestType;
      unsigned char byMssgSeqNum;
      int nRetVal = obFFBCmmd.GetRemoteRequest( stRemoteRequest, eRemoteRequestType, byMssgSeqNum, 500 );
    
      if( nRetVal > 0 )
      {
        bool bValidMssg = true;
        printf( "\n\n*************************************************************\n" );
        printf( "%d - Received Remote Request -\n", byMssgSeqNum );
        switch( eRemoteRequestType )
        {
        case GC_NETWORK_CONFIG:
          printf(" IP Address 1    - %.*s\n", GC_IP_ADDR_STR_LENGTH, stRemoteRequest.stNtwInfo.IPAddress1 );
          printf(" Net Mask 1      - %.*s\n", GC_IP_ADDR_STR_LENGTH, stRemoteRequest.stNtwInfo.NetMask1 );
          printf(" IP Address 2    - %.*s\n", GC_IP_ADDR_STR_LENGTH, stRemoteRequest.stNtwInfo.IPAddress2 );
          printf(" Net Mask 2      - %.*s\n", GC_IP_ADDR_STR_LENGTH, stRemoteRequest.stNtwInfo.NetMask2 );
          printf(" Default Gateway - %.*s\n", GC_IP_ADDR_STR_LENGTH, stRemoteRequest.stNtwInfo.DefaultGatewayIPAddr );
          break;
        case GC_CTRL_ACTION:
          printf(" Operation Number - %d\n", stRemoteRequest.stCtrlActInfo.byOperationNum );
          printf(" Stream Number    - %d\n", stRemoteRequest.stCtrlActInfo.byStreamNum );
          printf(" Control Action   - ");
          switch( (CTRL_ACTION) stRemoteRequest.stCtrlActInfo.byCtrlAction )
          {
          case NO_ACTION:
            printf("No Action\n");
            break; 
          case START_SINGLE_STREAM_CONTINUOUS:
            printf("Start Single Stream Continuous\n");
            break; 
          case START_SINGLE_STREAM_ONE_TIME:
            printf("Start Single Stream One Time\n");
            break; 
          case START_MANUAL_CALIBRATION:
            printf("Start Manual Calibration\n");
            break; 
          case START_MANUAL_VALIDATION:
            printf("Start Manual Validation\n");
            break; 
          case HALT_CURRENT_OPERATION:
            printf("Halt Current Operation\n");
            break; 
          case START_AUTO_SEQUENCE:
            printf("Start Auto Sequence\n");
            break; 
          case START_FORCED_MANUAL_CALIBRATION:
            printf("Start Forced Manual Calibration\n");
            break; 
          default:
            printf("Unknown\n");
            break; 
          }
          printf(" Purge Action     - ");
          switch( (PURGE_ACTION) stRemoteRequest.stCtrlActInfo.byPurgeAction )
          {
          case NOPURGE:
            printf("No Purge\n");
            break;
          case PURGE_BEFORE_CONTROL_ACTION:
            printf("Purge before control action\n");
            break;
          default:
            printf("Unknown\n");
            break;
          }
          break;
        default:
          bValidMssg = false;
          printf(" Unknown Request\n");
          break;
        }

        if( bValidMssg )
        {
          if( obFFBCmmd.SendRemoteRequestResponse( eRemoteRequestType,
                                                   ACK_FFB_NO_ERR,
                                                   0,
                                                   byMssgSeqNum ) > 0 )
          {
            printf("Failed to send Acknowledge message.\n");
          }
        }
      }
      else if( nRetVal == 0 )
      {
        unRxZeroDataCounter++;
      }
      else
      {
        switch( nRetVal )
        {
        case ERR_WRONG_CRC:
          unRxCRCErrCounter++;
          break;
        case ERR_PROTOCOL:
          unRxProtocolErrCounter++;
          break;
        case ERR_TIMEOUT:
          break;
        default:
          unRxUnknownErrCounter++;
          break;
        }
      }
    }while( g_nExitApp != 1 );

    printf( "Closing the FFB_COMMAND connection.\n" );
    obFFBCmmd.CloseHal();
  }
  else
  {
    printf( "Failed to open FFB_COMMAND connection.\n");
  }
}

//Returns the FFB time epoch.
int FFBTimeEpoch(time_t & epoch)
{

  //Fieldbus time epoch is Jan 1, 1972
  struct tm epoch_date;
  epoch_date.tm_sec = 0;
  epoch_date.tm_min = 0;
  epoch_date.tm_hour = 0;
  epoch_date.tm_mday = 1;
  epoch_date.tm_mon = 0;
  epoch_date.tm_year = 72;
  epoch_date.tm_isdst = 0;
  epoch = mktime(&epoch_date);

  return epoch;  // -1 if mktime failed.
}


//Send the GC status information to FFB Board.
void SendGCStatus2FFBBoard()
{
  int nRetVal = 0;
  //PV - Descs
  const char szPVDescs[ NUM_PV_CONFIG_INFO ][33] = 
    {  "C6+ 47/35/17 - GrossDryBTU",
       "C6+ 47/35/17 - NetDryBTU",
       "C6+ 47/35/17 - CalibConc",
       "C6+ 47/35/17 - HV Sup",
       "PROPANE - GrossDryBTU",
       "PROPANE - NetDryBTU",
       "PROPANE - CalibConc",
       "PROPANE - HV Sup",
       "i-BUTANE - GrossDryBTU",
       "i-BUTANE - NetDryBTU",
       "i-BUTANE - CalibConc",
       "i-BUTANE - HV Sup",
       "n-BUTANE - GrossDryBTU",
       "n-BUTANE - NetDryBTU",
       "n-BUTANE - CalibConc",
       "n-BUTANE - HV Sup",
       "NEOPENTANE - GrossDryBTU",
       "NEOPENTANE - NetDryBTU",
       "NEOPENTANE - CalibConc",
       "NEOPENTANE - HV Sup",
       "i-PENTANE - GrossDryBTU",
       "i-PENTANE - NetDryBTU",
       "i-PENTANE - CalibConc",
       "i-PENTANE - HV Sup",
       "n-PENTANE - GrossDryBTU",
       "n-PENTANE - NetDryBTU",
       "n-PENTANE - CalibConc",
       "n-PENTANE - HV Sup",
       "NITROGEN - GrossDryBTU",
       "NITROGEN - NetDryBTU",
       "NITROGEN - CalibConc",
       "NITROGEN - HV Sup",
       "METHANE - GrossDryBTU",
       "METHANE - NetDryBTU",
       "METHANE - CalibConc",
       "METHANE - HV Sup",
       "CARBON DIOXIDE - GrossDryBTU",
       "CARBON DIOXIDE - NetDryBTU",
       "CARBON DIOXIDE - CalibConc",
       "CARBON DIOXIDE - HV Sup",
       "ETHANE - GrossDryBTU",
       "ETHANE - NetDryBTU",
       "ETHANE - CalibConc",
       "ETHANE - HV Sup",
       "n-NONANE - GrossDryBTU",
       "n-NONANE - NetDryBTU",
       "n-NONANE - CalibConc",
       "n-NONANE - HV Sup",
       "n-HEXANE - GrossDryBTU",
       "n-HEXANE - NetDryBTU",
       "n-HEXANE - CalibConc",
       "n-HEXANE - HV Sup",
       "n-HEPTANE - GrossDryBTU",
       "n-HEPTANE - NetDryBTU",
       "n-HEPTANE - CalibConc",
       "n-HEPTANE - HV Sup",
       "n-OCTANE - GrossDryBTU",
       "n-OCTANE - NetDryBTU",
       "n-OCTANE - CalibConc",
       "n-OCTANE - HV Sup",
       "H2S - GrossDryBTU",
       "H2S - NetDryBTU",
       "H2S - CalibConc",
       "H2S - HV Sup"
    }; 

  const float fPVValues [ NUM_PV_DATA_IN_PV_UPDATE ] = 
    {    5288.700195,
         4900.600098,
         0.000000,
         196.979996,
         2522.000000,
         2320.399902,
         0.999600,
         93.935997,
         3259.500000,
         3006.899902,
         0.299500,
         121.400002,
         3269.899902,
         3018.000000,
         0.298500,
         121.790001,
         3993.899902,
         3691.399902,
         0.100400,
         148.759995,
         4010.199951,
         3707.600098,
         0.100100,
         149.360001,
         4018.000000,
         3715.600098,
         0.100300,
         149.660004,
         0.000000,
         0.000000,
         2.504000,
         0.000000,
         1012.299988,
         911.099976,
         89.570396,
         37.707001,
         0.000000,
         0.000000,
         0.996100,
         0.000000,
         1773.800049,
         1622.800049,
         4.999000,
         66.067001,
         7012.600098,
         6508.000000,
         0.000000,
         177.550003,
         4767.000000,
         4414.200195,
         0.303000,
         167.550003,
         5515.200195,
         5111.799805,
         0.000000,
         205.419998,
         6263.399902,
         5809.399902,
         0.000000,
         233.289993,
         638.580017,
         588.159973,
         0.000000,
         23.785000
    };

  char szFFBBoardComm[]    = "FFB_STATUS:FIELDBUS_SLOT:FFB_STATUS_1";
  CFFBStatus obFFBStatus; 
  printf("Opening FFB_STATUS connection.\n");
  nRetVal = obFFBStatus.OpenHal( szFFBBoardComm );
  if( nRetVal >= 0 )
  {
    printf("Successfully created FFB_STATUS connection.\n");
  }
  else
  {
    printf("Failed to open FFB_STATUS connection.\n");
  }

  if( nRetVal >= 0 )
  {
    unsigned int unTotalCounter  = 0;
    unsigned int unTxFailCounter = 0;
    unsigned int unTxSuccCounter = 0;
    unsigned int unTxRetryCounter = 0;

    //Flags to indicate whether to send continuous status mssg or just one.
    bool bSendContinuousStatusMssg = false;
    bool bSendContinuousPVUpdateMssg = false;
    bool bSendRandomPVUpdateDataSets = false;
    unsigned int unInterval4ContinuousPVUpdate = 1; //1 Sec

    while( g_nExitApp != 1 )
    {
      unTotalCounter++;
      printf("\n******************************************************************* \n");
      printf("Enter one of the following option - \n");
      printf("   0 - Exit.\n");
      printf("   1 - Send GC Configuration.\n");
      printf("   2 - Send GC User Tag.\n");
      printf("   3 - Send GC Network Configuration.\n");
      printf("   4 - Send GC Stream Configuration.\n");
      printf("   5 - Send GC PV Configuration.\n");
      printf("   6 - Send GC Detector Configuration.\n");
      printf("   7 - Send GC Device Mode.\n");
      printf("   8 - Send continuous GC Status at an interval of 1 sec.\n");
      printf("   9 - Send single GC Status.\n");
      printf("  10 - Send GC Operation Change information.\n");
      printf("  11 - Send continuous GC PV Update information.\n");
      printf("  12 - Send GC PV[SET-1] Update information.\n");
      printf("  13 - Send GC PV[SET-2] Update information.\n");
      printf("  14 - Send GC PV[SET-3] Update information.\n");
      printf("  15 - Request FFB time.\n" );
      printf("  16 - Query Device Capability Information.\n");
      printf("Input --> ");

      int nKey = 0;
      scanf("%d", & nKey);
      getchar();

      if( 0 == nKey )
      {
        break;
      }

      FFB_ACK_CODE eAckCode;
      switch( nKey )
      {
      case 1:
      {
        char szGCFirmRev[GC_FIRMWARE_REV_STR_LENGTH+1] = {0};
        char szGCSerialNum[GC_SERIAL_NUM_STR_LENGTH] = {0};
        int nBrand = 0;
          
        printf("Enter GC Firmware revision (Max length - %d)\n - ", GC_FIRMWARE_REV_STR_LENGTH);
        scanf("%s", szGCFirmRev);
        printf("Enter GC Serial Number (Max length - %d)\n - ", GC_SERIAL_NUM_STR_LENGTH);
        scanf("%s", szGCSerialNum);
        printf("Enter Brand Setting - \n (0) - Rosemount Analytical \n (1) - Daniel \n - ");
        scanf("%d", & nBrand );
        getchar();
        if( nBrand < 0 || nBrand > 1 )
        {
          nBrand = 0;
        }

        nRetVal = obFFBStatus.SendGCConfig( szGCFirmRev,
                                            szGCSerialNum,
                                            (BRAND_SETTING) nBrand,
                                            eAckCode );
        break;
      }
      case 2:
      {
        char szGCUserTag[GC_USER_TAG_STR_LENGTH+1] = {0};

        printf("Enter GC User Tag (Max length - %d)\n - ", GC_USER_TAG_STR_LENGTH);
        scanf("%s", szGCUserTag);

        nRetVal = obFFBStatus.SendGCUserTag( szGCUserTag, eAckCode );

        break;
      }
      case 3:
      {
        char szIPAddr1[GC_IP_ADDR_STR_LENGTH+1] = {0};
        char szNetMask1[GC_IP_ADDR_STR_LENGTH+1] = {0};
        char szIPAddr2[GC_IP_ADDR_STR_LENGTH+1] = {0};
        char szNetMask2[GC_IP_ADDR_STR_LENGTH+1] = {0};
        char szDefaultGateway[GC_IP_ADDR_STR_LENGTH+1] = {0};

        printf("Enter IP Address 1 (Max length - %d)\n - ", GC_IP_ADDR_STR_LENGTH);
        scanf("%s", szIPAddr1 );
        printf("Enter Net Mask 1 (Max length - %d)\n - ", GC_IP_ADDR_STR_LENGTH);
        scanf("%s", szNetMask1 );
        printf("Enter IP Address 2 (Max length - %d)\n - ", GC_IP_ADDR_STR_LENGTH);
        scanf("%s", szIPAddr2 );
        printf("Enter Net Mask 2 (Max length - %d)\n - ", GC_IP_ADDR_STR_LENGTH);
        scanf("%s", szNetMask2 );
        printf("Enter Default Gateway (Max length - %d)\n - ", GC_IP_ADDR_STR_LENGTH);
        scanf("%s", szDefaultGateway );

        nRetVal = obFFBStatus.SendNWConfig( szIPAddr1,
                                            szNetMask1,
                                            szIPAddr2,
                                            szNetMask2,
                                            szDefaultGateway,
                                            eAckCode );
        break;
      }
      case 4:
      {
        printf("Sending Stream Config information ...\n");
        GC_STREAM_CONFIG_STRUCT stStreamConfig;
        for( int nInd = 0; nInd < NUM_STREAM_CONFIG_INFO; nInd++ )
        {
          stStreamConfig.stStreamInfo[nInd].byStreamNum = (unsigned char) nInd + 1;
          stStreamConfig.stStreamInfo[nInd].byStreamUsage = (unsigned char) nInd % 4;
          stStreamConfig.stStreamInfo[nInd].byDetectorAssigned = (unsigned char) nInd % 4;
          sprintf( (char*) stStreamConfig.stStreamInfo[nInd].StreamName, "Stream %d", nInd+1);
        }

        nRetVal = obFFBStatus.SendStreamConfig( & stStreamConfig,
                                                eAckCode );
        break;
      }
      case 5:
      {
        printf("Sending PV Config information ...\n");
        GC_PV_CONFIG_STRUCT stPVConfig;
        for( int nInd = 0; nInd < NUM_PV_CONFIG_INFO; nInd++ )
        {
          stPVConfig.stPVInfo[nInd].byPVNum = (unsigned char) nInd + 1;
          strncpy( (char*) stPVConfig.stPVInfo[nInd].PVDescription, szPVDescs[nInd], GC_PV_DESC_STR_LENGTH );
        }

        nRetVal = obFFBStatus.SendPVConfig( & stPVConfig, eAckCode );

        break;
      }
      case 6:
      {
        printf("Sending Detector config data ...\n");
        GC_DET_CONFIG_STRUCT stDetConfig;
        for( int nInd = 0; nInd < NUM_DET_CONFIG_INFO; nInd++ )
        {
          stDetConfig.stDetInfo[nInd].byDetectorNum = (unsigned char) nInd + 1;
          stDetConfig.stDetInfo[nInd].byDetectorType = (unsigned char) nInd % 3;
        }

        nRetVal = obFFBStatus.SendDetConfig( & stDetConfig, eAckCode );

        break;
      }
      case 7:
      {
        int nDeviceMode;
        printf("Enter Device Mode -\n (1) 1 Stream, 1 Detector\n (2) 1 Stream, 2 Detector\n (3) 2 Stream 2 Detector\n - ");
        scanf("%d", & nDeviceMode );
        getchar();

        nRetVal = obFFBStatus.SendDeviceMode( (DEV_MODE) nDeviceMode, eAckCode );

        break;
      }
      case 8:
        bSendContinuousStatusMssg = true;
      case 9:
      {
        unsigned short usRunTimeCC1 = 1;
        unsigned short usRunTimeCC2 = 1;
        unsigned long long ullErrInfo = 0;
        unsigned long ulDetErrInfo = 0;
        CONFIG_ACTION eCfgAction;
        
        //When only a single status msssg to be send ask user to input the values.  
        if( ! bSendContinuousStatusMssg )
        {
          printf("Enter Run Time for Cycle Clock 1\n - ");
          scanf("%hu", & usRunTimeCC1);
          printf("Enter Run Time for Cycle Clock 2\n - ");
          scanf("%hu", & usRunTimeCC2);
          printf("Enter Error Information\n - ");
          scanf("%llu", & ullErrInfo);
          printf("Enter Detector Error Information\n - ");
          scanf("%lu", & ulDetErrInfo);
        }

        while( true )
        {
          nRetVal = obFFBStatus.SendGCStatus( usRunTimeCC1,
                                              usRunTimeCC2,
                                              ullErrInfo,
                                              ulDetErrInfo,
                                              eCfgAction,
                                              eAckCode );

          if( nRetVal > 0 )
          {
            printf("\nFFB Status : Config Action - %s\n", eCfgAction ? "Ready to Accept." : "Can't Acceptable.");
          }

          //If asked to send single status mssg then break off.
          if( ! bSendContinuousStatusMssg )
          {
            break;
          }
          //If user wants to come out of continuous loop of sending status mssg.(Ctrl+C)
          if( g_nExitApp )
          {
            g_nExitApp = 0;
            break;
          }

          //Update the TX success and fail count.
          if( nRetVal < 0 )
          {
            printf("Failed to send data !!!\n");
            unTxFailCounter++;
          }
          else
          {
            printf("Successfuly send data :)\n");
            unTxSuccCounter++;
          }

          unTotalCounter++;

          unTxRetryCounter += obFFBStatus.GetRetryAttempts();

          //Increment runtime, assumtion made - cycle time is of 300 sec.
          usRunTimeCC1 = ( usRunTimeCC1 % 300 ) + 1;
          usRunTimeCC2 = ( usRunTimeCC2 % 300 ) + 1;

          //Before trying to send the next status mssg wait 1 sec.
          sleep(1);
        }
          
        //Reset.
        bSendContinuousStatusMssg = false;
        break;
      }
      case 10:
      {
        int nOprNum = 0;
        int nCurrOprMode = 0;
        unsigned int nStreamNum = 0;
        unsigned int nNextStreamNum = 0;
        unsigned short usRunTime = 0;
        unsigned short usAnalysisTime = 0;
        unsigned short usCycleTime = 0;

        printf("Enter Operation Number\n - ");
        scanf("%d", & nOprNum);
        getchar();
        printf("Enter Current Operation Mode - \n");
        printf("   (0) - Unavailable\n");
        printf("   (1) - Warm Up\n");
        printf("   (2) - Idle\n");
        printf("   (3) - Auto Sequence\n");
        printf("   (4) - Auto Calibration\n");
        printf("   (5) - Auto Validation\n");
        printf("   (6) - Single Stream Continuous\n");
        printf("   (7) - Single Stream One Time\n");
        printf("   (8) - Manula Calibration\n");
        printf("   (9) - Forced Calibration\n");
        printf("  (10) - Manual Validation\n");
        printf("  (11) - Purge\n - ");
        scanf("%d", & nCurrOprMode);
        getchar();
        printf("Enter Stream Number ( any number between 1 to 24 )\n - ");
        scanf("%d", & nStreamNum);
        getchar();
        printf("Enter Next Stream Number ( any number between 1 to 24 )\n - ");
        scanf("%d", & nNextStreamNum);
        getchar();
        printf("Enter Run Time\n - ");
        scanf("%hu", & usRunTime);
        getchar();
        printf("Enter Analysis Time\n - ");
        scanf("%hu", & usAnalysisTime);
        getchar();
        printf("Enter Cycle Time\n - ");
        scanf("%hu", & usCycleTime);
        getchar();

        nRetVal = obFFBStatus.SendGCOperationChange( nOprNum,
                                                     (OPERATION_MODE) nCurrOprMode,
                                                     (unsigned char) nStreamNum,
                                                     (unsigned char) nNextStreamNum,
                                                     usRunTime,
                                                     usAnalysisTime,
                                                     usCycleTime,
                                                     eAckCode );

        break;
      }
      case 11:
        bSendContinuousPVUpdateMssg = true;
        printf("Enter the interval (In Seconds) between successive PV Update\n - ");
        scanf("%u", & unInterval4ContinuousPVUpdate);
        printf("Enter choice for PV Update set - \n");
        printf("   (0) - Send PV[SET-1] Update information continuously.\n");
        printf("   (1) - Send PV[SET-2] Update information continuously.\n");
        printf("   (2) - Send PV[SET-3] Update information continuously.\n");
        printf("   (3) - Each time randomly decide any of the above and send.\n - ");
        scanf("%d", & nKey );

        //Check if the entered value is valid entry or not.
        if( nKey < 0 || nKey > 3 )
        {
          printf("Invalid choice! Defaulting to option (0).\n");
          nKey = 12;
        }
        else if( 3 == nKey )
        {
          bSendRandomPVUpdateDataSets = true;
        }
        else
        {
          nKey += 12;
        }

      case 12:
      case 13:
      case 14:
      {
        while( true )
        {
          if( bSendRandomPVUpdateDataSets )
          {
            nKey = rand() % 3 + 12;;
          }

          PV_DATA_UPDATE_STRUCT stPVUpdate;

          if( 12 == nKey )
          { 
            printf("\nSending Process variable update (SET - 1) information ...\n");
            for( int nInd = 0; nInd < NUM_PV_DATA_IN_PV_UPDATE; nInd++ )
            {
              stPVUpdate.stPVData[nInd].byPVStatus = (unsigned char)GOOD;
              stPVUpdate.stPVData[nInd].fValue = fPVValues[nInd];
            }
          }
          else if( 13 == nKey )
          {
            printf("\nSending Process variable update (SET - 2) information ...\n");
            for( int nInd = 0; nInd < NUM_PV_DATA_IN_PV_UPDATE; nInd++ )
            {
              stPVUpdate.stPVData[nInd].byPVStatus = (unsigned char) ( nInd % 2 ? GOOD : BAD );
              stPVUpdate.stPVData[nInd].fValue = fPVValues[nInd] * 1.5;
            }
          }
          else
          { 
            printf("\nSending Process variable update (SET - 3) information ...\n");
            for( int nInd = 0; nInd < NUM_PV_DATA_IN_PV_UPDATE; nInd++ )
            {
              stPVUpdate.stPVData[nInd].byPVStatus = (unsigned char) BAD;
              stPVUpdate.stPVData[nInd].fValue = fPVValues[nInd] * 0.66;
            }
          }
  
          nRetVal = obFFBStatus.SendPVUpdate( & stPVUpdate, eAckCode, 600 );

          //If asked to send single status mssg then break off.
          if( ! bSendContinuousPVUpdateMssg )
          {
            break;
          }
          //If user wants to come out of continuous loop of sending status mssg.(Ctrl+C)
          if( g_nExitApp )
          {
            g_nExitApp = 0;
            break;
          }

          //Update the TX success and fail count.
          if( nRetVal < 0 )
          {
            printf("Failed to send data !!!\n");
            unTxFailCounter++;
          }
          else
          {
            printf("Successfuly send data :)\n");
            unTxSuccCounter++;
          }

          unTotalCounter++;

          unTxRetryCounter += obFFBStatus.GetRetryAttempts();

          //Sleep before sending the next PV Update.
          sleep( unInterval4ContinuousPVUpdate );
        }

        bSendContinuousPVUpdateMssg = false;
        bSendRandomPVUpdateDataSets = false;
        break;
      }
        
      case 15:
      {
        printf("Requesting FFB time ...\n");
        unsigned long long ullFFBTime = 0;
        nRetVal = obFFBStatus.RetrieveFFBTime( ullFFBTime, eAckCode );
        if( nRetVal < 0 )
        {
          printf( "Request failed !!!\n" );
          unTxFailCounter++;
        }
        else if( ACK_FFB_NO_ERR == eAckCode )
        {
          printf( "Received FFB reply :)\n");
          time_t tTime = 0;
          //Get the FFB time correction. FFB time referenced from 1st Jan 1972
          //instead of 1st Jan 1970.
          if( -1 == FFBTimeEpoch( tTime ) )
          {
            printf("Failed to get FFB epoch !!!\n");
            tTime = 0;
          }
          //Convert 64 bit ( 1/32 msec unit ) time to 32 bit ( 1 sec unit ) time.
          tTime += ullFFBTime / 32000;
          tm tmTime = *( gmtime( & tTime ) );
          printf( "FFB Time is (HH:MM:SS, MM/DD/YYYY) - ");
          printf( "%.2d:%.2d:%.2d, %.2d/%.2d/%.4d\n", 
                  tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec, 
                  tmTime.tm_mon + 1, tmTime.tm_mday, tmTime.tm_year + 1900 );
          unTxSuccCounter++;
        }
        else
        {
          printf( "FFB NAck-ed the request :(\n" );
          unTxSuccCounter++;
        }

        //Doing this to avoid printing the below message of sending success/failure.
        nKey = -1;
        break;
      }

      case 16:
      {
        printf("Querying FFB Device Capability information...\n");
        std::vector< FFB_DEV_CAP_DETAIL > v_stDevCapDetails;
        FIRMWARE_INFO stFirmInfo;
        nRetVal = obFFBStatus.QueryFFBCapabilityInfo( v_stDevCapDetails,
                                                      stFirmInfo,
                                                      eAckCode,
                                                      1000 );
        if( nRetVal < 0 )
        {
          printf( "Request failed !!!\n" );
          unTxFailCounter++;
        }
        else if( ACK_FFB_NO_ERR == eAckCode )
        {
          printf( "Received FFB reply :)\n");
          printf( "Rxd Device Capabilty Info -\n" );
          int nNumDev = (int) v_stDevCapDetails.size();
          for( int nInd = 0; nInd < nNumDev; nInd++ )
          {
            printf( " %d -> %s, %ld, %ld, %ld, %s\n", nInd+1, 
                    v_stDevCapDetails[nInd].strDeviceName.c_str(),
                    v_stDevCapDetails[nInd].nIOFunction,
                    v_stDevCapDetails[nInd].nSlotNum,
                    v_stDevCapDetails[nInd].nRevNum,
                    v_stDevCapDetails[nInd].strDevInfo.c_str() );
          }

          printf( "Firmware Revision - %.*s, Firmware Date - %.*s\n", 
                  FIRMWARE_VERSION_STR_SZ, 
                  stFirmInfo.szFirmwareVersion, 
                  FIRMWARE_DATE_STR_SZ, 
                  stFirmInfo.szFirmwareDate );
        }
        else
        {
          printf( "FFB NAck-ed the request :(\n" );
          unTxSuccCounter++;
        }

        //Doing this to avoid printing the below message of sending success/failure.
        nKey = -1;
        break;
      }

      default:
        printf("Function not supported ...\n");
        nKey = -1;
        break;
      }

      if( nRetVal < 0 && nKey > 0 ) //i.e nRetVal < 0 Error
      {
        printf("Failed to send data !!!\n");
        unTxFailCounter++;
      }
      else if( nKey > 0 ) //i.e nRetVal >= 0 No Error
      {
        printf("Successfuly send data :)\n");
        unTxSuccCounter++;
      }
      
      printf( "Total : Transmission attempts - %u\n", unTotalCounter );
      printf( "        Success - %u, Failure - %u", unTxSuccCounter, unTxFailCounter );

      unTxRetryCounter += obFFBStatus.GetRetryAttempts();
      printf( ", Retries - %u", unTxRetryCounter );

      printf( "\n\n" );

    }

    printf("Closing FFB_STATUS connection.\n");
    obFFBStatus.CloseHal();
  }
}

void TestIRKeyPad()
{
  CIRKeyPad obIRKeyPad;
  int nRetVal = obIRKeyPad.OpenHal( "GRAPHICAL_LOI:LOI_SLOT:GRAPHICAL_LOI_1" );
  if( nRetVal >= 0 )
  {
    while( g_nExitApp != 1 )
    {
      int nUserSelection = 0;
      printf( "\n\n\n*******************************************************************\n" );
      printf( "Enter one of the following options - \n" );
      printf( "0) Exit.\n");
      printf( "1) Set IR Key sensitivity.\n");
      printf( "2) Set all IR Key sensitivity.\n");
      printf( "3) Save all IR Key sensitivity.\n");
      printf( "4) Get IR Key sensitivity.\n");
      printf( "5) Get all IR Key sensitivity settings.\n");
      printf( "6) Load all previous IR Key sensitivity settings.\n");
      printf( "7) Restore all default IR Key sensitivity settings.\n - ");

      scanf( "%d", & nUserSelection );
      getchar();

      if( 0 == nUserSelection )
      {
        break;
      }

      //For individual Set/Get IR settings read the Key number.
      unsigned char ucIRKeyNum = 0;
      unsigned char ucIRKeySensitivity = 0;
      
      if( ( 1 == nUserSelection ) || ( 4 == nUserSelection ) )
      {
        printf( "Enter the IR Key Number ( 1 to 8 ) - \n " );
        int nData;
        scanf( "%d", & nData );
        getchar();

        if( nData < 1 || nData > 8 )
        {
          printf( "Invalid IR Key Number - %d\n", nData );
          continue;
        }

        ucIRKeyNum = (unsigned char) nData;
      }
      if( ( 1 == nUserSelection ) || ( 2 == nUserSelection ) )
      {
        printf( "Enter the Key Sensitivity ( 1 to 127 ) - \n " );
        int nData;
        scanf( "%d", & nData );
        getchar();

        if( nData < 1 || nData > 127 )
        {
          printf( "Invalid Sensitivity - %d\n", nData );
          continue;
        }
        ucIRKeySensitivity = (unsigned char) nData;
      }
      
      IRKeySettings KeySettings;
      switch( nUserSelection )
      {
      case 1:
        nRetVal = obIRKeyPad.SetKeySensitivity( ucIRKeyNum, ucIRKeySensitivity );
        if( nRetVal >= 0 )
        {
          printf( "Successfully set Key sensitivity\n" );
        }
        else
        {
          printf( "Failed to set Key sensitivity!\n" );
        }
        break;
      case 2:
        nRetVal = obIRKeyPad.SetAllKeySensitivity( ucIRKeySensitivity );
        if( nRetVal >= 0 )
        {
          printf( "Successfully set all Key sensitivity\n" );
        }
        else
        {
          printf( "Failed to set all Key sensitivity!\n" );
        }
        break;
      case 3:
        nRetVal = obIRKeyPad.SaveKeySensitivitySettings();
        if( nRetVal >= 0 )
        {
          printf( "Successfully saved all Key sensitivity\n" );
        }
        else
        {
          printf( "Failed to save all Key sensitivity!\n" );
        }
        break;
      case 4:
        nRetVal = obIRKeyPad.GetKeySensitivity( ucIRKeyNum, ucIRKeySensitivity );
        if( nRetVal >= 0 )
        {
          printf( "Retrieved Key %d sensitivity is %d\n", 
                  ucIRKeyNum, ucIRKeySensitivity );
        }
        else
        {
          printf( "Failed to retrieve Key sensitivity!\n" );
        }
        break;
      case 5:
        nRetVal = obIRKeyPad.GetAllKeySensitivity( KeySettings );
        if( nRetVal >= 0 )
        {
          for( int nInd = 0; nInd < 8; nInd++ )
          {
            printf( "Retrieved Key %d sensitivity is %d\n", 
                    nInd+1, KeySettings.ucSensitivity[nInd] );
          }
        }
        else
        {
          printf( "Failed to retrieve all Key sensitivity!\n" );
        }
        break;
      case 6:
        nRetVal = obIRKeyPad.LoadAllReviousKeySensitivity( KeySettings );
        if( nRetVal >= 0 )
        {
          for( int nInd = 0; nInd < 8; nInd++ )
          {
            printf( "Restored Key %d sensitivity is %d\n", 
                    nInd+1, KeySettings.ucSensitivity[nInd] );
          }
        }
        else
        {
          printf( "Failed to restore all Key sensitivity!\n" );
        }
        break;
      case 7:
        nRetVal = obIRKeyPad.RestoreAllDefaultKeySensitivity( KeySettings );
        if( nRetVal >= 0 )
        {
          for( int nInd = 0; nInd < 8; nInd++ )
          {
            printf( "Loaded default key %d sensitivity is %d\n", 
                    nInd+1, KeySettings.ucSensitivity[nInd] );
          }
        }
        else
        {
          printf( "Failed to load all default Key sensitivity!\n" );
        }
        break;
      default:
        printf( "Invalid choice !!!" );
        break;
      }
    }

    obIRKeyPad.CloseHal();
  }
  else
  {
    printf( "Failed to connect to LOI IR Keypad !" );
  }
}

void TestCPUADC()
{
  CCPU_ADC1_AD7908 obCPU_ADC1;
  CCPU_ADC2_AD7908 obCPU_ADC2;
  while( g_nExitApp != 1 )
  {
    float fADCVoltage = 0;
    int nRetVal = -1;
    
    for (int nChannelNumber = 1; nChannelNumber <= 8; ++nChannelNumber)
    {
      fADCVoltage = -1;
      nRetVal = obCPU_ADC1.GetReading( nChannelNumber, fADCVoltage );
      printf( "%7.3g, ", fADCVoltage );
      usleep (1000);
    }
    
    for (int nChannelNumber = 1; nChannelNumber <= 8; ++nChannelNumber)
    {
      fADCVoltage = -1;
      nRetVal = obCPU_ADC2.GetReading( nChannelNumber, fADCVoltage );
      printf( "%7.3g, ", fADCVoltage ) ;
      usleep (1000);
    }
    
    printf ("\n");
    usleep (1000000);
  }
}

// -10V to +10V, 16bit DAC
static const float fDACResolution = (float)(20.0 / 65535.0);
// 0V to +2.5V, 10bit DAC
// so    x    - 2.526 * x / 1024
static const float fADCResolution = (float)(2.5 / 1023);
void TestFID()
{
#ifdef MODEL_370XA
  return;
#else

  int nRetVal = 0;
  bool bExit = false;

  CFIDCtrl obFIDCtrl;
  if (Detect1500XAR2())
  {
    if (nIndex == 0)
      nRetVal = obFIDCtrl.OpenHal("FID:SLOT_BASE_IO:FID_1");
    else
      nRetVal = obFIDCtrl.OpenHal("FID:BACKPLANE:FID_1");
  }
  else
    nRetVal = obFIDCtrl.OpenHal("FID:SLOT_BASE_IO:FID_1");
  if (nRetVal >= 0)
  {
    printf("Successfully created FID connection.\n" );

    CFID_DAC1_AD5570ARSZ obFID_DAC1(nIndex + 1);
    CFID_DAC2_AD5570ARSZ obFID_DAC2(nIndex + 1);
    CFID_ADC_AD7811YRU obFID_ADC(nIndex + 1);
    CFIDOperations obFIDOpr(nIndex + 1);

    unsigned short usDACCoarseAdjValue = 10;
    unsigned short usDACFineAdjValue = 10;
    unsigned short usWaitInMsAfterGainChange = 100;

    float fDACVoltage2 = 0;

    bool bGainSet2High = false;
    while (g_nExitApp != 1)
    {
      char cUserSelection = 0;
    
      printf("Enter choice -> ");
      do
      {
        cUserSelection = (char)getchar();
      } while (cUserSelection == '\n' || cUserSelection == ' ');
    
      unsigned short usCount = 0;
      float fVolt = 0;
      int nChannelNum = 0;
      switch (cUserSelection)
      {
      case 'I':
        nRetVal = obFIDCtrl.TurnOnIgnition();
        break;
      case 'i':
        nRetVal = obFIDCtrl.TurnOffIgnition();
        break;
      case 'G':
        nRetVal = obFIDCtrl.SetGainHigh();
        bGainSet2High = true;
        break;
      case 'g':
        nRetVal = obFIDCtrl.SetGainLow();
        bGainSet2High = false;
        break;
      case '1':
        printf("Enter the DAC1 Voltage (-10V to +10V)\n - ");
        scanf("%f", &fVolt);
        fVolt += 10;
        usCount = (unsigned short)(fVolt / fDACResolution);
        printf("Equivalent count is - %hu\n", usCount);
        if (obFID_DAC1.SetCount(usCount) < 0)
          printf("Failed to set FID DAC1 value!\n");
        else
          printf("Successfully set FID DAC1 value.\n");
        break;
      case '!':
        if (obFID_DAC1.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC1 value!\n");
        }
        else
        {
          printf("FID DAC1 count - %hu\n", usCount);
          printf("FID DAC1 voltage - %f V\n", (((float)usCount * fDACResolution) - 10));
        }
        break;
      case '2':
        printf("Enter the DAC2 count value (-10V to +10V)\n - ");
        scanf("%f", &fVolt );
        fVolt += 10;
        usCount = (unsigned short)(fVolt / fDACResolution);
        printf("Equivalent count is - %hu\n", usCount);
        if (obFID_DAC2.SetCount(usCount) < 0)
          printf("Failed to set FID DAC2 value!\n");
        else
          printf("Successfully set FID DAC2 value.\n");
        fDACVoltage2 = fVolt;
        break;
      case '@':
        if (obFID_DAC2.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC2 value!\n");
        }
        else
        {
          printf("FID DAC2 count - %hu\n", usCount);
          printf("FID DAC2 voltage - %f V\n", (((float)usCount * fDACResolution) - 10));
        }
        break;
      case 'R':
        printf("Enter the channel number 1 or 2 or 3\n - ");
        scanf("%d", &nChannelNum);
        if (obFID_ADC.GetCount(nChannelNum, usCount) < 0)
        {
          printf("Failed to get FID ADC value!\n");
        }
        else
        {
          printf("FID ADC count - %hu\n", usCount);
          printf("FID ADC read voltage - %f V\n", fADCResolution * usCount);
        }
        break;
      case 'r':
        printf("Enter the AOUT i/p -> 1 or 2 or 3\n - ");
        scanf("%d", &nChannelNum);
        if (!obFID_ADC.GetCHVoltage(nChannelNum, fVolt))
          printf("Failed to get FID AOUT-%d value!\n", nChannelNum);
        else
          printf("FID AOUT-%d voltage - %f V\n", nChannelNum, fVolt);
        break;
      case 'n':
        if (obFIDOpr.NullElectrometer(bGainSet2High))
          printf("Successfully performed NULL Electorometer.\n");
        else
          printf("Failed to do NULL Electrometer!\n");
        break;
      case 'a':
        if (obFIDOpr.AutoZero())
          printf("Successfully performed AutoZero.\n");
        else
          printf("Failed to do AutoZero!\n");
        break;
      case 'j':
        printf("CH 1, CH 2, CH 3,\n");
        while (true)
        {
          for (int nInd = 0; nInd < 3 ; nInd++)
          {
            if (obFID_ADC.GetCount(nInd + 1, usCount) >= 0)
              printf("%.3f,", fADCResolution * usCount);
            else
              printf("Fail,");
          }
          printf("\n");

          if (nIndexFlag)
            usleep(nIndex * 1000);
          else
            usleep(1000000);

          if (g_nExitApp)
          {
            g_nExitApp = 0;
            break;
          }
        }
        break;

      case 'k':
        printf("CH 1, CH 2, CH 3,\n");
        while (true)
        {
          for (int nInd = 0; nInd < 3 ; nInd++)
          {
            if (obFID_ADC.GetCHVoltage(nInd + 1, fVolt) >= 0)
              printf("%.3f,", fVolt);
            else
              printf("Fail,");
          }
          printf("\n");

          if (nIndexFlag)
            usleep(nIndex * 1000);
          else
            usleep(1000000);

          if (g_nExitApp)
          {
            g_nExitApp = 0;
            break;
          }
        }
        break;

      case 'l':
        printf("CH 1, CH 2, CH 3,\n");
        while (true)
        {
          for (int nInd = 0; nInd < 3 ; nInd++)
          {
            if (obFID_ADC.GetCount(nInd + 1, usCount) >= 0)
              printf("%d,", usCount);
            else
              printf("Fail,");
          }
          printf("\n");

          if (nIndexFlag)
            usleep(nIndex * 1000);
          else
            usleep(1000000);

          if (g_nExitApp)
          {
            g_nExitApp = 0;
            break;
          }
        }
        break;

      case 'c':
        int nTemp;
        printf("Enter Coarse Adjustment value ( Take a value between 1 and 5000. Default is 10 ) \n - ");
        scanf("%d", &nTemp);
        if (nTemp < 1 || nTemp > 5000)
        {
          printf("Invalid i/p !\n");
        }
        else
        {
          usDACCoarseAdjValue = (unsigned short)nTemp;
          printf("Coarse Adjustment value - %d\n", usDACCoarseAdjValue);
        }
        break;

      case 'f':
        {
          int nTemp;
          printf("Enter Fine Adjustment value ( Take a value between 1 and 5000. Default is 10 ) \n - ");
          scanf("%d", &nTemp);
          if (nTemp < 1 || nTemp > 5000)
          {
            printf("Invalid i/p !\n");
          }
          else
          {
            usDACFineAdjValue = (unsigned short)nTemp;
            printf("Fine Adjustment value - %d\n", usDACFineAdjValue);
          }
        }
        break;

      case '>':
        if (obFID_DAC1.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC1 value!\n");
        }
        else
        {
          usCount = usCount + usDACCoarseAdjValue;
          if (obFID_DAC1.SetCount(usCount) < 0)
            printf("Failed to set FID DAC1 value!\n");
          else
            printf("DAC1 count is set to %hu.\n", usCount);
        }
        break;

      case '<':
        if (obFID_DAC1.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC1 value!\n");
        }
        else
        {
          usCount = usCount - usDACCoarseAdjValue;
          if (obFID_DAC1.SetCount(usCount) < 0)
            printf("Failed to set FID DAC1 value!\n");
          else
            printf("DAC1 count is set to %hu.\n", usCount);
        }
        break;

      case '.':
        if (obFID_DAC1.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC1 value!\n");
        }
        else
        {
          usCount = usCount + usDACFineAdjValue;
          if (obFID_DAC1.SetCount(usCount) < 0)
            printf("Failed to set FID DAC1 value!\n");
          else
            printf("DAC1 count is set to %hu.\n", usCount);
        }
        break;

      case ',':
        if (obFID_DAC1.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC1 value!\n");
        }
        else
        {
          usCount = usCount - usDACFineAdjValue;
          if (obFID_DAC1.SetCount(usCount) < 0)
            printf("Failed to set FID DAC1 value!\n");
          else
            printf("DAC1 count is set to %hu.\n", usCount);
        }
        break;

      case 'x':
        if (obFID_DAC1.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC1 value!\n");
        }
        else
        {
          while (g_nExitApp != 1)
          {
            usCount = usCount + usDACFineAdjValue;
            if (obFID_DAC1.SetCount(usCount) < 0)
              printf("Failed to set FID DAC1 value!\n");
            else
              printf("DAC1 count is set to %hu.\n", usCount);

            usleep(100 * 1000);
          }
          g_nExitApp = 0;
        }
        break;

      case 'z':
        if (obFID_DAC1.GetCount(usCount) < 0)
        {
          printf("Failed to get FID DAC1 value!\n");
        }
        else
        {
          while (g_nExitApp != 1)
          {
            usCount = usCount - usDACFineAdjValue;
            if (obFID_DAC1.SetCount(usCount) < 0)
              printf("Failed to set FID DAC1 value!\n");
            else
              printf("DAC1 count is set to %hu.\n", usCount);

            usleep(100 * 1000);
          }
          g_nExitApp = 0;
        }
        break;

      case 'T':
        {
          CPreampConfig obPreampCfg;
          int nRetVal = obPreampCfg.OpenHal((nIndex == 0) ? (char *)"PREAMP_CFG:SLOT_1:PREAMP_1" : (char *)"PREAMP_CFG:SLOT_1:PREAMP_2");
          if (nRetVal < 0)
          {
            printf("Error %d opening Preamp Channel: 2\n", nRetVal);
          }
          else
          {
            usCount = 0;
            long lLastValue = 0;
            unsigned long ulMaxOffset = 0;
            unsigned long ulMinOffset = (unsigned long)-1;
            printf("DAC1 Count, Preamp Det2 Reading, Change in Det2 Reading\n");
            do
            {
              long lValue = 0;

              if (obFID_DAC1.SetCount(usCount) < 0)
              {
                printf("Failed to set FID DAC1 value!\n");
                usleep(100 * 1000);
              }
              else
              {
                usleep(100 * 1000);
                if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
                {
                  printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
                }
                else
                {
                  long lOffset = 0;
                  if (lLastValue != 0)
                  {
                    lOffset = lLastValue - lValue;
                    if (lOffset < 0)
                      lOffset = -lOffset;
                    if ((unsigned long)lOffset < ulMinOffset)
                      ulMinOffset = (unsigned long)lOffset;
                    if ((unsigned long)lOffset > ulMaxOffset)
                      ulMaxOffset = (unsigned long)lOffset;
                  }
                  lLastValue = lValue;
                  printf("%hu, %ld, %ld\n",usCount, lValue, lOffset);
                }
              }

              usCount = usCount + usDACFineAdjValue;
              if (usCount < usDACFineAdjValue)
              {
                // One loop is completed from 0 to unsigned short MAX, time to exit
                break;
              }

            } while (g_nExitApp != 1);

            g_nExitApp = 0;

            printf("\n\nMax Offset - %lu, Min Offset - %lu\n", ulMaxOffset, ulMinOffset);

            nRetVal = obPreampCfg.CloseHal();
            if (nRetVal < 0)
              printf("Error %d closing Preamp channel: 2\n", nRetVal);
          }
        }
        break;

      case 'p':
        {
          CPreampConfig obPreampCfg;
          int nRetVal = obPreampCfg.OpenHal((nIndex == 0) ? (char *)"PREAMP_CFG:SLOT_1:PREAMP_1" : (char *)"PREAMP_CFG:SLOT_1:PREAMP_2");
          if (nRetVal < 0)
          {
            printf("Error %d opening Preamp Channel: 2\n", nRetVal);
          }
          else
          {
            long lValue = 0;
            if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
              printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
            else
              printf("Preamp Det 2 reading : %ld\n", lValue);

            nRetVal = obPreampCfg.CloseHal();
            if (nRetVal < 0)
              printf("Error %d closing Preamp channel: 2\n", nRetVal);
          }
        }
        break;

      case 'A':
        {
          CPreampConfig obPreampCfg;
          int nRetVal = obPreampCfg.OpenHal((nIndex == 0) ? (char *)"PREAMP_CFG:SLOT_1:PREAMP_1" : (char *)"PREAMP_CFG:SLOT_1:PREAMP_2");
          if (nRetVal < 0)
          {
            printf("Error %d opening Preamp Channel: 2\n", nRetVal);
          }
          else
          {
            long lValue = 0;
            if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
            {
              printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
            }
            else
            {
              printf("Before Adjusting Preamp Det 2 reading is  %ld\n", lValue);
              printf("Enter expected Preamp Det 2 reading : ");
              long lExpectedVal = 0;
              scanf("%ld", &lExpectedVal);
              long lOffset = lExpectedVal - lValue;
              if (lOffset < 0)
                lOffset = - lOffset;

              unsigned short usDACOffset = (unsigned short)(lOffset / 2457.25) ;
              if (obFID_DAC1.GetCount(usCount) < 0)
              {
                printf("Failed to get FID DAC1 value!\n");
              }
              else
              {
                if (lExpectedVal > lValue)
                  usCount = usCount - usDACOffset;
                else
                  usCount = usCount + usDACOffset;
                if (obFID_DAC1.SetCount( usCount ) < 0)
                  printf("Failed to set FID DAC1 value!\n");
                else
                  printf("DAC1 count is set to %hu.\n", usCount);

                usleep(100 * 1000);
                
                if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
                  printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
                else
                  printf("After Adjusting Preamp Det 2 reading is  %ld\n", lValue);
              }
            }

            nRetVal = obPreampCfg.CloseHal();
            if (nRetVal < 0)
              printf("Error %d closing Preamp channel: 2\n", nRetVal);
          }
        }
        break;

      case 'B':
        {
          printf("FID Gain is - %s\n", bGainSet2High ? "HIGH": "LOW");
          
          CPreampConfig obPreampCfg;
          int nRetVal = obPreampCfg.OpenHal((nIndex == 0) ? (char *)"PREAMP_CFG:SLOT_1:PREAMP_1" : (char *)"PREAMP_CFG:SLOT_1:PREAMP_2");
          if (nRetVal < 0)
          {
            printf("Error %d opening Preamp Channel: 2\n", nRetVal);
          }
          else
          {
            long lValue = 0;
            long lExpectedVal = 0;
            if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
            {
              printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
            }
            else
            {
              printf("Before Gain change Preamp Det 2 reading is  %ld\n", lValue);
              nRetVal = obFIDCtrl.SetGainHigh();
              bGainSet2High = true;
              lExpectedVal = lValue;
              printf("Wait %hu ms before reading preamp.\n", usWaitInMsAfterGainChange);
              usleep(usWaitInMsAfterGainChange * 1000);
              
              if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
                printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
              else
                printf("After Gain change Preamp Det 2 reading is  %ld\n", lValue);
            }
            if (nRetVal >= 0)
            {
              long lOffset = lExpectedVal - lValue;
              if (lOffset < 0)
                lOffset = -lOffset;
              unsigned short usDACOffset = (unsigned short)(lOffset / 2457.25);
              if (obFID_DAC1.GetCount(usCount) < 0)
              {
                printf("Failed to get FID DAC1 value!\n");
              }
              else
              {
                if (lExpectedVal > lValue)
                  usCount = usCount - usDACOffset;
                else
                  usCount = usCount + usDACOffset;
                if (obFID_DAC1.SetCount(usCount) < 0)
                  printf("Failed to set FID DAC1 value!\n");
                else
                  printf("DAC1 count is set to %hu.\n", usCount);

                usleep(100 * 1000);
                
                if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
                  printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
                else
                  printf("After Adjusting Preamp Det 2 reading is  %ld\n", lValue);
              }
            }

            nRetVal = obPreampCfg.CloseHal();
            if (nRetVal < 0)
              printf("Error %d closing Preamp channel: 2\n", nRetVal);
          }

          printf("FID Gain is - %s\n", bGainSet2High ? "HIGH": "LOW");
        }
        break;

      case 'b':
        {
          printf("FID Gain is - %s\n", bGainSet2High ? "HIGH": "LOW");
          
          CPreampConfig obPreampCfg;
          int nRetVal = obPreampCfg.OpenHal((nIndex == 0) ? (char *)"PREAMP_CFG:SLOT_1:PREAMP_1" : (char *)"PREAMP_CFG:SLOT_1:PREAMP_2");
          if (nRetVal < 0)
          {
            printf("Error %d opening Preamp Channel: 2\n", nRetVal);
          }
          else
          {
            long lValue = 0;
            long lExpectedVal = 0;
            if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
            {
              printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
            }
            else
            {
              printf("Before Gain change Preamp Det 2 reading is  %ld\n", lValue);
              nRetVal = obFIDCtrl.SetGainLow();
              bGainSet2High = false;
              lExpectedVal = lValue;
              printf("Wait %hu ms before reading preamp.\n", usWaitInMsAfterGainChange);
              usleep(usWaitInMsAfterGainChange * 1000);
              
              if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
                printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
              else
                printf("After Gain change Preamp Det 2 reading is  %ld\n", lValue);
            }
            if (nRetVal >= 0)
            {
              long lOffset = lExpectedVal - lValue;
              if (lOffset < 0)
                lOffset = -lOffset;
              unsigned short usDACOffset = (unsigned short)(lOffset / 2457.25);
              if (obFID_DAC1.GetCount(usCount) < 0)
              {
                printf("Failed to get FID DAC1 value!\n");
              }
              else
              {
                if (lExpectedVal > lValue)
                  usCount = usCount - usDACOffset;
                else
                  usCount = usCount + usDACOffset;
                if (obFID_DAC1.SetCount(usCount) < 0)
                  printf("Failed to set FID DAC1 value!\n");
                else
                  printf("DAC1 count is set to %hu.\n", usCount);

                usleep(100 * 1000);
                
                if ((nRetVal = obPreampCfg.GetPreampValue(&lValue)) < 0)
                  printf("Error %d getting preamp value from Channel: 2\n", nRetVal);
                else
                  printf("After Adjusting Preamp Det 2 reading is  %ld\n", lValue);
              }
            }

            nRetVal = obPreampCfg.CloseHal();
            if (nRetVal < 0)
              printf("Error %d closing Preamp channel: 2\n", nRetVal);
          }

          printf("FID Gain is - %s\n", bGainSet2High ? "HIGH": "LOW");
        }
        break;

      case 'y':
        {
          int nTemp;
          printf("Enter wait time in Milliseconds (10 to 65535) after a FID Gain change \n - ");
          scanf("%d", &nTemp);
          if (nTemp < 10 || nTemp > 65535)
          {
            printf("Invalid i/p !\n");
          }
          else
          {
            usWaitInMsAfterGainChange = (unsigned short)nTemp;
            printf("Wait time after FID Gain Change is  - %hu ms\n", usWaitInMsAfterGainChange);
          }
        }
        break;

      case 'N':
        nRetVal = obFIDCtrl.SetGainHigh();
        bGainSet2High = true;
        usleep(usWaitInMsAfterGainChange * 1000);
        obFIDOpr.NullElectrometer(bGainSet2High);
        usleep(usWaitInMsAfterGainChange * 1000);
        obFIDOpr.AutoZero();
        break;

      case 'h':
        printf( "\n*******************************************************************\n");
        printf("Enter one of the following choices - \n");
        printf(" I   ->   Turn ON FID Ignition.\n");
        printf(" i   ->   Trun OFF FID Ingnition.\n");
        printf(" G   ->   Set FID Gain High.\n");
        printf(" g   ->   Set FID Gain Low.\n");
        printf(" 1   ->   Set FID DAC1.\n");
        printf(" !   ->   Get FID DAC1.\n");
        printf(" 2   ->   Set FID DAC2.\n");
        printf(" @   ->   Get FID DAC2.\n");
        printf(" R   ->   Get FID ADC Readings.\n");
        printf(" r   ->   Get FID AOUT Readings.\n");
        printf(" N   ->   Change FID Gain to High, then do Null Electrometer and Auto Zero.\n");
        printf(" n   ->   NULL Electrometer FID.\n");
        printf(" a   ->   Auto Zero FID.\n");
        printf(" j   ->   Read continuously all 3 FID ADC channels.\n");
        printf(" k   ->   Read AOUT continuously all 3 FID ADC channels.\n");
        printf(" l   ->   Read ADC counts continuously for all 3 FID ADC channels.\n");
        printf(" c   ->   Set Coarse Adjustment Value.\n");
        printf(" f   ->   Set Fine Adjustment Value.\n");
        printf(" >   ->   Increase DAC1 Coarse Adjustment. (Change current DAC1 count to [DAC1 Count + Coarse Value])\n");
        printf(" <   ->   Decrease DAC1 Coarse Adjustment. (Change current DAC1 count to [DAC1 Count - Coarse Value])\n");
        printf(" .   ->   Increase DAC1 Fine Adjustment. (Change current DAC1 count to [DAC1 Count + Fine Value])\n");
        printf(" ,   ->   Decrease DAC1 Fine Adjustment. (Change current DAC1 count to [DAC1 Count - Fine Value])\n");
        printf(" x   ->   Increase DAC1 by Fine Adjustment continuously.\n");
        printf(" z   ->   Decrease DAC1 by Fine Adjustment continuously.\n");
        printf(" T   ->   Trace changes to Preamp Det2 with DAC1 incremental(Fine Adjustment) changes.\n");
        printf(" p   ->   Read current Preamp Det2 reading.\n");
        printf(" A   ->   Adjust Preamp Det2 reading.\n");
        printf(" B   ->   Set FID Gain HIGH and then Baseline Preamp Det2 reading.\n");
        printf(" b   ->   Set FID Gain LOW and then Baseline Preamp Det2 reading.\n");
        printf(" y   ->   Set delay time after FID Gain Change. (For option \'N\', \'B\' and \'b\')\n");
        printf("Enter any other key to exit application.\n");
        printf("*******************************************************************\n");
        break;
          
      default:
        bExit = true;
      }

      if (bExit)
        break;

      if (cUserSelection == 'I' || cUserSelection == 'i'
      ||  cUserSelection == 'G' || cUserSelection == 'g')
      {
        if (nRetVal >= 0)
          printf("Successfully sent FID command.\n");
        else
          printf("Failed to get response with error code - %d\n", nRetVal);
      }
    }

    printf("Closing FID connection.\n");
    obFIDCtrl.CloseHal();
  }
  else
    printf("Failed to open FID connection.\n");
#endif  //#ifdef MODEL_370XA
}

void TestFPD()
{
#ifdef MODEL_370XA
  return;
#else
  int nRetVal = 0;

  CFPDCtrl obFPDCtrl;
  if (Detect1500XAR2())
  {
    if (nIndex == 0)
      nRetVal = obFPDCtrl.OpenHal("FPD:SLOT_BASE_IO:FPD_1");
    else
      nRetVal = obFPDCtrl.OpenHal("FPD:BACKPLANE:FPD_1");
  }
  else
    nRetVal = obFPDCtrl.OpenHal("FPD:SLOT_BASE_IO:FPD_1");
  if (nRetVal >= 0)
  {
    printf("Successfully created FPD connection.\n");

    while (g_nExitApp != 1)
    {
      int nUserSelection = 0;
      printf("\n\n\n*******************************************************************\n");
      printf("Enter one of the following options - \n");
      printf(" 1) Do FPD Auto Zero.\n");
      printf(" 2) Read FPD Preamplifier voltage reading.\n");
      printf(" 3) Read FPD Auto Zero DAC reading.\n");
      printf(" 4) Read FPD Preamplifier and FPD Auto Zero DAC readings continiously.\n");
      printf(" 5) Read FPD Auto Zero DAC ADC reading.\n");
      printf(" 6) Read FPD Preamplifier ADC reading.\n");
      printf("Enter any other key to exit application...\n -> ");
      scanf( "%d", & nUserSelection );
      getchar();

      if (nUserSelection > 6 || nUserSelection < 1)
        break;

      switch (nUserSelection)
      {
        case 1:
          if (obFPDCtrl.AutoZero() >= 0)
            printf("Successfully performed FPD Auto Zero.\n");
          else
            printf("Failed to perform FPD Auto Zero\n");
          break;

        case 2:
          {
            CFPD_ADC_7705 obFPDADC(nIndex + 1);
            float fValue;
            if (obFPDADC.GetFPDPreampVoltage(fValue))
              printf("FPD Preamplifier voltage reading is %f\n", fValue);
            else
              printf("Failed to read FPD Preamplifier voltage\n");
          }
          break;

        case 3:
          {
            CFPD_ADC_7705 obFPDADC(nIndex + 1);
            float fValue;
            if (obFPDADC.GetFPDDACVoltage(fValue))
              printf("FPD Auto Zero DAC voltage reading is %f\n", fValue);
            else
              printf("Failed to read FPD Auto Zero DAC reading\n");
          }
          break;

        case 4:
          {
            CFPD_ADC_7705 obFPDADC(nIndex + 1);
            float fValue;
            printf("Preamplifier, Auto Zero DAC\n");
            while (g_nExitApp != 1)
            {
              fValue = 0;
              if (obFPDADC.GetFPDPreampVoltage(fValue))
                printf("%f,", fValue);
              else
                printf("Failed,");

              fValue = 0;
              if (obFPDADC.GetFPDDACVoltage(fValue))
                printf("%f\n", fValue);
              else
                printf("Failed\n");
            }
            g_nExitApp = 0;
          }
          break;

        case 5:
          {
            CFPD_ADC_7705 obFPDADC(nIndex + 1);
            unsigned short usCount;
            if (obFPDADC.GetFPDDACCounts(usCount))
              printf("FPD Auto Zero DAC ADC Counts is %d\n", usCount);
            else
              printf("Failed to read FPD Auto Zero DAC ADC Counts\n");
          }
          break;

        case 6:
          {
            CFPD_ADC_7705 obFPDADC(nIndex + 1);
            unsigned short usCount;
            if (obFPDADC.GetFPDPreampCounts(usCount))
              printf("FPD Preamplifier ADC Counts is %d\n", usCount);
            else
              printf("Failed to read FPD Preamplifier ADC Counts\n");
          }
          break;

        default:
          break;
      }
    }
  }
  obFPDCtrl.CloseHal();
#endif  //#ifdef MODEL_370XA
}

void TestFPD_G2()
{
#ifdef MODEL_370XA
  return;
#else
  int nRet = 0;
  long lVal = 0;
  char buf[11];
  FPD_G2_FLAME_STATUS eFlameStatus;
  FPD_G2_IGN_MODE eIgnMode;
  CFpdG2control oFpdG2Ctrl;
  if (nIndex == 0)
    nRet = oFpdG2Ctrl.OpenHal("FPD_G2:FPD_G2_1:FPD_G2_1");
  else
    nRet = oFpdG2Ctrl.OpenHal("FPD_G2:FPD_G2_2:FPD_G2_1");
  if (nRet >= 0)
  {
    printf("Successfully created FPD G2 connection.\n");

    while (g_nExitApp != 1)
    {
      int nUserSelection = 0;
      printf("\n\n\n*******************************************************************\n");
      printf("Enter one of the following options - \n");
      printf(" 1) Get Configuration\n");
      printf(" 2) Set Ignition Mode\n");
      printf(" 3) Set Ignition Attempts\n");
      printf(" 4) Set Wait Time Between Ignition Attempts\n");
      printf(" 5) Set Igniter ON Duration\n");
      printf(" 6) Set Flame ON Sense Temperature\n");
      printf(" 7) Set Flame OFF Sense Temperature\n");
      printf(" 8) Set TEC Temperature\n");
      printf(" 9) Ignite Flame\n");
      printf("Enter any other key to exit application...\n -> ");
      scanf("%d", &nUserSelection);
      getchar();

      if (nUserSelection < 1 || nUserSelection > 9)
        break;

      switch (nUserSelection)
      {
        case 1:   // Get Configuration
          printf(" TEC Setpoint           : ");
          nRet = oFpdG2Ctrl.GetTECsetpoint(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld degC\n", lVal);
          else
            printf("Error %d\n", nRet);

          printf(" TEC Temperature        : ");
          nRet = oFpdG2Ctrl.GetTECtemp(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld degC\n", lVal);
          else
            printf("Error %d\n", nRet);

          printf(" Flame Temperature      : ");
          nRet = oFpdG2Ctrl.GetFlameTemp(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld degC\n", lVal);
          else
            printf("Error %d\n", nRet);

          printf(" Flame Status           : ");
          nRet = oFpdG2Ctrl.GetFlameStatus(eFlameStatus);
          if (nRet == ERR_SUCCESS)
          {
            if (eFlameStatus == FPD_G2_FLAME_STATUS_OVERTEMP)
              strcpy(buf, "OVERTEMP");
            else if (eFlameStatus == FPD_G2_FLAME_STATUS_ON)
              strcpy(buf, "ON");
            else
              strcpy(buf, "OFF");
          }
          else
            sprintf(buf, "Error %d", nRet);
          printf("%s\n", buf);

          printf(" Ignition Mode          : ");
          nRet = oFpdG2Ctrl.GetIgnitionMode(eIgnMode);
          if (nRet == ERR_SUCCESS)
          {
            if (eIgnMode == FPD_G2_IGN_MODE_MANUAL)
              strcpy(buf, "Manual");
            else
              strcpy(buf, "Automatic");
          }
          else
            sprintf(buf, "Error %d", nRet);
          printf("%s\n", buf);

          printf(" Ignition Attempts      : ");
          nRet = oFpdG2Ctrl.GetIgnitionAttempts(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld\n", lVal);
          else
            printf("Error %d\n", nRet);

          printf(" Wait Time              : ");
          nRet = oFpdG2Ctrl.GetWaitTime(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld\n", lVal);
          else
            printf("Error %d\n", nRet);

          printf(" Igniter ON Duration    : ");
          nRet = oFpdG2Ctrl.GetIgniterOnTime(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld\n", lVal);
          else
            printf("Error %d\n", nRet);

          printf(" Flame ON Sense Temp    : ");
          nRet = oFpdG2Ctrl.GetFlameOnTemp(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld degC\n", lVal);
          else
            printf("Error %d\n", nRet);

          printf(" Flame OFF Sense Temp   : ");
          nRet = oFpdG2Ctrl.GetFlameOffTemp(lVal);
          if (nRet == ERR_SUCCESS)
            printf("%ld degC\n", lVal);
          else
            printf("Error %d\n", nRet);

          break;

        case 2:   // Set Ignition Mode
          printf("Enter Ignition Mode (1 = Manual, 2 = Automatic)-> ");
          fflush(stdin);
          scanf("%ld", &lVal);
          printf("\n");
          if (lVal < 1 || lVal > 2)
            printf("Invalid Ignition Mode\n");
          else
          {
            nRet = oFpdG2Ctrl.SetIgnitionMode((FPD_G2_IGN_MODE)lVal);
            if (nRet == ERR_SUCCESS)
              printf("...Ignition Mode changed to %s\n", (lVal == 1) ? "Manual" : "Automatic");
            else
              printf("...Failed to change Ignition Mode, error = %d\n", nRet);
          }
          break;

        case 3:   // Set Ignition Attempts
          printf("Enter Ignition Attempts (%d to %d)-> ", (int)MIN_FPD_G2_IGN_TRIES, (int)MAX_FPD_G2_IGN_TRIES);
          fflush(stdin);
          scanf("%ld", &lVal);
          printf("\n");
          if (lVal < MIN_FPD_G2_IGN_TRIES || lVal > MAX_FPD_G2_IGN_TRIES)
            printf("Invalid Ignition Attempts\n");
          else
          {
            nRet = oFpdG2Ctrl.SetIgnitionAttempts(lVal);
            if (nRet == ERR_SUCCESS)
              printf("...Ignition Attempts changed to %ld\n", lVal);
            else
              printf("...Failed to change Ignition Attempts, error = %d\n", nRet);
          }
          break;

        case 4:
          printf("Enter Wait Time (%d to %d sec)-> ", (int)MIN_FPD_G2_WAIT_TIME, (int)MAX_FPD_G2_WAIT_TIME);
          fflush(stdin);
          scanf("%ld", &lVal);
          printf("\n");
          if (lVal < MIN_FPD_G2_WAIT_TIME || lVal > MAX_FPD_G2_WAIT_TIME)
            printf("Invalid Wait Time\n");
          else
          {
            nRet = oFpdG2Ctrl.SetWaitTime(lVal);
            if (nRet == ERR_SUCCESS)
              printf("...Wait Time changed to %ld sec\n", lVal);
            else
              printf("...Failed to change Wait Time, error = %d\n", nRet);
          }
          break;

        case 5:
          printf("Enter Igniter ON Duration (%d to %d sec)-> ", (int)MIN_FPD_G2_IGN_ON_TIME, (int)MAX_FPD_G2_IGN_ON_TIME);
          fflush(stdin);
          scanf("%ld", &lVal);
          printf("\n");
          if (lVal < MIN_FPD_G2_IGN_ON_TIME || lVal > MAX_FPD_G2_IGN_ON_TIME)
            printf("Invalid Igniter ON Duration\n");
          else
          {
            nRet = oFpdG2Ctrl.SetIgniterOnTime(lVal);
            if (nRet == ERR_SUCCESS)
              printf("...Igniter ON Duration changed to %ld sec\n", lVal);
            else
              printf("...Failed to change Igniter ON Duration, error = %d\n", nRet);
          }
          break;

        case 6:
          printf("Enter Flame ON Temperature (%d to %d deg C)-> ", (int)MIN_FPD_G2_FLAME_SENSE, (int)MAX_FPD_G2_FLAME_SENSE);
          fflush(stdin);
          scanf("%ld", &lVal);
          printf("\n");
          if (lVal < MIN_FPD_G2_FLAME_SENSE || lVal > MAX_FPD_G2_FLAME_SENSE)
            printf("Invalid Flame ON Temperature\n");
          else
          {
            nRet = oFpdG2Ctrl.SetFlameOnTemp(lVal);
            if (nRet == ERR_SUCCESS)
              printf("...Flame ON Temperature changed to %ld degC\n", lVal);
            else
              printf("...Failed to change Flame ON Temperature, error = %d\n", nRet);
          }
          break;

        case 7:
          printf("Enter Flame OFF Temperature (%d to %d deg C)-> ", (int)MIN_FPD_G2_FLAME_SENSE, (int)MAX_FPD_G2_FLAME_SENSE);
          fflush(stdin);
          scanf("%ld", &lVal);
          printf("\n");
          if (lVal < MIN_FPD_G2_FLAME_SENSE || lVal > MAX_FPD_G2_FLAME_SENSE)
            printf("Invalid Flame OFF Temperature\n");
          else
          {
            nRet = oFpdG2Ctrl.SetFlameOffTemp(lVal);
            if (nRet == ERR_SUCCESS)
              printf("...Flame OFF Temperature changed to %ld degC\n", lVal);
            else
              printf("...Failed to change Flame OFF Temperature, error = %d\n", nRet);
          }
          break;

        case 8:
          printf("Enter TEC Setpoint (-100 to 100 deg C)-> ");
          fflush(stdin);
          scanf("%ld", &lVal);
          printf("\n");
          if (lVal < -100 || lVal > 100)
            printf("Invalid TEC Setpoint\n");
          else
          {
            nRet = oFpdG2Ctrl.SetTECsetpoint(lVal);
            if (nRet == ERR_SUCCESS)
              printf("...TEC Setpoint changed to %ld degC\n", lVal);
            else
              printf("...Failed to change TEC Setpoint, error = %d\n", nRet);
          }
          break;

        case 9:   // Ignite Flame
          nRet = oFpdG2Ctrl.TurnOnIgnition();
          if (nRet == ERR_SUCCESS)
            printf("...Flame Ignition Started\n");
          else
            printf("...Failed to start Flame Ignition, Error %d\n", nRet);
          break;

        default:
          break;
      }
    }
  }
  oFpdG2Ctrl.CloseHal();
#endif  //#ifdef MODEL_370XA
}

void TestPressure()
{
  CPressure oPressure;
  oPressure.OpenHal( "PRESSURE:ANALYZER_SLOT:PRESSURE_1" );

  while( g_nExitApp != 1 )
  {
    int nUserSelection = 0;
    printf( "\n\n\n*******************************************************************\n" );
    printf( "Enter one of the following options - \n" );
    printf( " 1) Read Analyzer board Sample pressure.\n" );
    printf( " 2) Read Analyzer board Sample pressure continuously.\n" );
    printf( "Enter any other key to exit application...\n -> ");
    scanf( "%d", & nUserSelection );
    getchar();

    if( nUserSelection > 2 || nUserSelection < 1 )
    {
      break;
    }

    unsigned long ulPressureMilliVolts = 0;
    switch( nUserSelection )
    {
    case 1:
      if( ERR_SUCCESS == oPressure.GetPressure( & ulPressureMilliVolts ) )
      {
        printf( "Sample Pressure is %ld mV\n", ulPressureMilliVolts );
      }
      else
      {
        printf( "Failed to get Sample Pressure reading!!!\n" );
      }
      break;

    case 2:
      while( g_nExitApp != 1 )
      {
        if( ERR_SUCCESS == oPressure.GetPressure( & ulPressureMilliVolts ) )
        {
          printf( "Sample Pressure is %ld mV\n", ulPressureMilliVolts );
        }
        else
        {
          printf( "Failed to get Sample Pressure reading!!!\n" );
        }
      }
      g_nExitApp = 0;
      break;

    default:
      break;
    }
  }

  oPressure.CloseHal();
}

void TestSerialMode()
{
  CSerialModeCtrl oSerialModeCtrl1, oSerialModeCtrl2;
  oSerialModeCtrl1.OpenHal( "CTRL:ANALYZER_SLOT:CTRL_1" );
  oSerialModeCtrl2.OpenHal( "CTRL:ANALYZER_SLOT:CTRL_2" );

  while( g_nExitApp != 1 )
  {
    int nUserSelection = 0;
    printf( "\n\n\n*******************************************************************\n" );
    printf( "Enter one of the following options - \n" );
    printf( " 1) Set CH1 Serial Mode to RS232.\n" );
    printf( " 2) Set CH1 Serial Mode to RS485.\n" );
    printf( " 3) Get CH1 Serial Mode.\n" );
    printf( " 4) Toggle CH1 Serial Mode continiously every 1 second.\n" );
    printf( " 5) Set CH2 Serial Mode to RS232.\n" );
    printf( " 6) Set CH2 Serial Mode to RS485.\n" );
    printf( " 7) Get CH2 Serial Mode.\n" );
    printf( " 8) Toggle CH2 Serial Mode continiously every 1 second.\n" );
    printf( "Enter any other key to exit application...\n -> ");
    scanf( "%d", & nUserSelection );
    getchar();

    if( nUserSelection > 8 || nUserSelection < 1 )
    {
      break;
    }

    SERIAL_MODE_ENUM eSerialMode;
    switch( nUserSelection )
    {
    case 1:
      if( ERR_SUCCESS == oSerialModeCtrl1.SetMode(SERIAL_MODE_RS232) )
      {
        printf( "CH1 Serial Mode is set to RS232.\n" );
      }
      else
      {
        printf( "Failed to set CH1 Serial Mode to RS232!!!\n" );
      }
      break;

    case 2:
      if( ERR_SUCCESS == oSerialModeCtrl1.SetMode(SERIAL_MODE_RS485) )
      {
        printf( "CH1 Serial Mode is set to RS485.\n" );
      }
      else
      {
        printf( "Failed to set CH1 Serial Mode to RS485!!!\n" );
      }
      break;
      
    case 3:
      if( ERR_SUCCESS == oSerialModeCtrl1.GetMode(&eSerialMode) )
      {
        printf( "CH1 Serial Mode is  %s.\n", (eSerialMode==SERIAL_MODE_RS232) ? "RS232":"RS485" );
      }
      else
      {
        printf( "Failed to get CH1 Serial Mode!!!\n" );
      }
      break;
      
    case 4:
      eSerialMode = SERIAL_MODE_RS232;
      while( g_nExitApp != 1 )
      {
        if( ERR_SUCCESS == oSerialModeCtrl1.SetMode(eSerialMode) )
        {
          printf( "CH1 Serial Mode is set to %s.\n", (eSerialMode==SERIAL_MODE_RS232) ? "RS232":"RS485" );
        }
        else
        {
          printf( "Failed to set CH1 Serial Mode to %s!!!\n", (eSerialMode==SERIAL_MODE_RS232) ? "RS232":"RS485" );
        }

        eSerialMode = (eSerialMode==SERIAL_MODE_RS232) ? SERIAL_MODE_RS485 : SERIAL_MODE_RS232;
      }
      g_nExitApp = 0;
      break;
      
    case 5:
      if( ERR_SUCCESS == oSerialModeCtrl2.SetMode(SERIAL_MODE_RS232) )
      {
        printf( "CH2 Serial Mode is set to RS232.\n" );
      }
      else
      {
        printf( "Failed to set CH2 Serial Mode to RS232!!!\n" );
      }
      break;

    case 6:
      if( ERR_SUCCESS == oSerialModeCtrl2.SetMode(SERIAL_MODE_RS485) )
      {
        printf( "CH2 Serial Mode is set to RS485.\n" );
      }
      else
      {
        printf( "Failed to set CH2 Serial Mode to RS485!!!\n" );
      }
      break;
      
    case 7:
      if( ERR_SUCCESS == oSerialModeCtrl2.GetMode(&eSerialMode) )
      {
        printf( "CH2 Serial Mode is  %s.\n", (eSerialMode==SERIAL_MODE_RS232) ? "RS232":"RS485" );
      }
      else
      {
        printf( "Failed to get CH2 Serial Mode!!!\n" );
      }
      break;
      
    case 8:
      eSerialMode = SERIAL_MODE_RS232;
      while( g_nExitApp != 1 )
      {
        if( ERR_SUCCESS == oSerialModeCtrl2.SetMode(eSerialMode) )
        {
          printf( "CH2 Serial Mode is set to %s.\n", (eSerialMode==SERIAL_MODE_RS232) ? "RS232":"RS485" );
        }
        else
        {
          printf( "Failed to set CH2 Serial Mode to %s!!!\n", (eSerialMode==SERIAL_MODE_RS232) ? "RS232":"RS485" );
        }

        eSerialMode = (eSerialMode==SERIAL_MODE_RS232) ? SERIAL_MODE_RS485 : SERIAL_MODE_RS232;
      }
      g_nExitApp = 0;
      break;
      
    default:
      break;
    }
  }

  oSerialModeCtrl1.CloseHal();
  oSerialModeCtrl2.CloseHal();
}

void RunDiagnostics()
{
  int nRetVal = 0;
  CDiagnostic obDiagnostic;
  
  while ( g_nExitApp != 1 )
  {
    int nUserSelection = 0;
    printf("\n\n\n*******************************************************************\n");
    printf("Enter one of the following options - \n");
    printf(" 1) Print Preamp board diagnostic details.\n");
    printf(" 2) Print Heater solenoid board diagnostic details.\n");
    printf(" 3) Print Base IO board diagnostic details.\n");
    printf(" 4) Print LOI IR Key diagnostic details.\n");
    printf(" 5) Print Analyzer board diagnostic details.\n");
    printf("Enter any other key to exit application...\n -> ");
    scanf( "%d", & nUserSelection );
    getchar();

    if( nUserSelection > 5 || nUserSelection < 1 )
    {
      break;
    }

    switch( nUserSelection )
    {
    case 1:
      nRetVal = obDiagnostic.OpenHal( "DIAGNOSTIC:SLOT_1:DIAGNOSTIC_1" );
      break;
    case 2:
      nRetVal = obDiagnostic.OpenHal( "DIAGNOSTIC:SLOT_2:DIAGNOSTIC_1" );
      break;
    case 3:
      nRetVal = obDiagnostic.OpenHal( "DIAGNOSTIC:SLOT_BASE_IO:DIAGNOSTIC_1" );
      break;
    case 4:
      nRetVal = obDiagnostic.OpenHal( "DIAGNOSTIC:LOI_SLOT:DIAGNOSTIC_1" );
      break;
    case 5:
      nRetVal = obDiagnostic.OpenHal( "DIAGNOSTIC:ANALYZER_SLOT:DIAGNOSTIC_1" );
      break;
    }

    if( nRetVal >= 0 )
    {
      printf("Successfully created Diagnostic connection.\n");
    }
    else
    {
      printf("Failed to open Diagnostic connection.\n");
    }

    if( nRetVal >= 0 )
    {
      DIAGNOSTIC_TYPE eDiagnoType;
      BOARD_DIAGNOSTICS objBoardDiagnostics;

      nRetVal = obDiagnostic.GetDiagnosticsDetails( eDiagnoType, objBoardDiagnostics );
      if( nRetVal > 0 )
      {
        switch( eDiagnoType )
        {
        case PREAMP_DIAGNO:
          printf("Preamp Diagnostic details : \n");
          printf(" Input 5 Volt reading  - %fV\n", objBoardDiagnostics.Preamp.Input5V );
          printf(" Input 16 Volt reading - %fV\n", objBoardDiagnostics.Preamp.Input16V );
          printf(" On board temparture   - %ld millidegC\n", objBoardDiagnostics.Preamp.OnBoardTemp );
          printf(" Board Revision        - %d\n", objBoardDiagnostics.Preamp.SysInfo.Revision );
          printf(" Firmware Revision     - %d.%d.%d\n", objBoardDiagnostics.Preamp.SysInfo.majorVer, 
                 objBoardDiagnostics.Preamp.SysInfo.minorVer,  objBoardDiagnostics.Preamp.SysInfo.buildVer );
          break;
        case HTRSOL_DIAGNO:
          printf("Heater Solenoid Diagnostic details : \n");
          printf(" System 3.3V Input      - %fV\n", objBoardDiagnostics.HtrSol.Input3_3V );
          printf(" System 5V Input        - %fV\n", objBoardDiagnostics.HtrSol.Input5V );
          printf(" On board temparture    - %ld millidegC\n", objBoardDiagnostics.HtrSol.OnBoardTemp );
          printf(" Board Revision         - %d\n", objBoardDiagnostics.HtrSol.SysInfo.Revision );
          printf(" Firmware Revision      - %d.%d.%d\n", objBoardDiagnostics.HtrSol.SysInfo.majorVer, 
                 objBoardDiagnostics.HtrSol.SysInfo.minorVer, objBoardDiagnostics.HtrSol.SysInfo.buildVer );
          break;
        case BASEIO_DIAGNO:
          printf("Base IO Diagnostic details : \n");
          printf(" System 3.3V Input            - %fV\n", objBoardDiagnostics.BaseIO.Input3_3V );
          printf(" System 5V Input          - %fV\n", objBoardDiagnostics.BaseIO.Input5V );
          printf(" System 24 Volt         - %fV\n", objBoardDiagnostics.BaseIO.Input24V );
          printf(" System 24 Volt Current Drawn - %fA\n", objBoardDiagnostics.BaseIO.Input24VCurrent );
          printf(" On board temparture        - %ld millidegC\n", objBoardDiagnostics.BaseIO.OnBoardTemp );
          printf(" FID Sensor voltage       - %fV\n", objBoardDiagnostics.BaseIO.FIDSenseVoltage );
          printf(" Board Revision               - %d\n", objBoardDiagnostics.BaseIO.SysInfo.Revision );
          printf(" Firmware Revision            - %d.%d.%d\n", objBoardDiagnostics.BaseIO.SysInfo.majorVer, 
                 objBoardDiagnostics.BaseIO.SysInfo.minorVer, objBoardDiagnostics.BaseIO.SysInfo.buildVer );
          break;
        case IRKEY_DIAGNO:
          printf("LOI IRKey Diagnostic details :\n");
          printf("  On board temperature         : %d millidegC\n", objBoardDiagnostics.IRKey.OnBoardTemp );
          printf("  Board revision               : %d\n",objBoardDiagnostics.IRKey.SysInfo.Revision );
          printf("  Firmware revision            : %d.%d.%d\n", objBoardDiagnostics.IRKey.SysInfo.majorVer,
                 objBoardDiagnostics.IRKey.SysInfo.minorVer, objBoardDiagnostics.IRKey.SysInfo.buildVer );
          break;
        case ANALYZER_DIAGNO:
          printf("Analyzer IO Diagnostic details :\n");
          printf("  System 10V input             : %fV\n", objBoardDiagnostics.Analyzer.Input10V);
          printf("  System 22V input             : %fV\n", objBoardDiagnostics.Analyzer.Input22V);
          printf("  On board temperature         : %d millidegC\n",objBoardDiagnostics.Analyzer.OnBoardTemp);
          printf("  Board revision               : %d\n", objBoardDiagnostics.Analyzer.SysInfo.Revision);
          printf("  Firmware revision            : %d.%d.%d\n", objBoardDiagnostics.Analyzer.SysInfo.majorVer,
                 objBoardDiagnostics.Analyzer.SysInfo.minorVer, objBoardDiagnostics.Analyzer.SysInfo.buildVer);
          break;
        default:
          break; 
        }
      }
      else
      {
        printf( "Failed to get response with error code - %d\n", nRetVal );
      }

      printf( "Closing Diagnostic connection.\n" );
      obDiagnostic.CloseHal();
    }
  }
}

void RunDiagsContinuous()
{
  int nRetVal = 0;
  CDiagnostic obDiagnostic;
  
  int nUserSelection = 0;
  printf("\n\n\n*******************************************************************\n");
  printf("Enter one of the following options - \n");
  printf(" 1) Print Preamp 1 board diagnostic details\n");
  printf(" 2) Print Preamp 2 board diagnostic details\n");
  printf(" 3) Print Preamp 3 board diagnostic details\n");
  printf(" 4) Print Heater solenoid 1 board diagnostic details\n");
  printf(" 5) Print Heater solenoid 2 board diagnostic details\n");
  printf("Enter any other key to exit application...\n -> ");
  scanf( "%d", & nUserSelection );
  getchar();

  if (nUserSelection > 5 || nUserSelection < 1)
    return;

  switch (nUserSelection)
  {
    case 1:
      nRetVal = obDiagnostic.OpenHal("DIAGNOSTIC:SLOT_1:DIAGNOSTIC_1" );
      break;
    case 2:
      nRetVal = obDiagnostic.OpenHal("DIAGNOSTIC:SLOT_3:DIAGNOSTIC_1" );
      break;
    case 3:
      nRetVal = obDiagnostic.OpenHal("DIAGNOSTIC:SLOT_5:DIAGNOSTIC_1" );
      break;
    case 4:
      nRetVal = obDiagnostic.OpenHal("DIAGNOSTIC:SLOT_2:DIAGNOSTIC_1" );
      break;
    case 5:
      nRetVal = obDiagnostic.OpenHal("DIAGNOSTIC:SLOT_4:DIAGNOSTIC_1" );
      break;
  }

  if (nRetVal >= 0)
    printf("Successfully created Diagnostic connection\n");
  else
  {
    printf("Failed to open Diagnostic connection\n");
    return;
  }

  while (g_nExitApp != 1) //TEST MODE
  {
    time_t curTime = time(NULL);
    struct tm *tm_curr = localtime(&curTime);
    
    // Print time when we acquired data...
    printf("%04d/%02d/%02d %02d:%02d:%02d, ", 
            tm_curr->tm_year + 1900, tm_curr->tm_mon + 1, tm_curr->tm_mday, 
            tm_curr->tm_hour, tm_curr->tm_min, tm_curr->tm_sec);

    DIAGNOSTIC_TYPE eDiagnoType;
    BOARD_DIAGNOSTICS objBoardDiagnostics;
    nRetVal = obDiagnostic.GetDiagnosticsDetails(eDiagnoType, objBoardDiagnostics);
    if (nRetVal > 0)
    {
      switch(eDiagnoType)
      {
        case PREAMP_DIAGNO:
          printf("%5ld mDegC, %.5fv, %.4fv\n", objBoardDiagnostics.Preamp.OnBoardTemp,
                 objBoardDiagnostics.Preamp.Input5V, objBoardDiagnostics.Preamp.Input16V);
          break;

        case HTRSOL_DIAGNO:
          printf("%5ld mDegC, %.5fv, %.5fv\n", objBoardDiagnostics.HtrSol.OnBoardTemp,
                 objBoardDiagnostics.HtrSol.Input3_3V, objBoardDiagnostics.HtrSol.Input5V);
          break;

        default:
          break; 
      }
    }
    else
      printf( "Failed to get response with error code - %d\n", nRetVal );

    sleep(3);
  }
  printf( "Closing Diagnostic connection.\n" );
  obDiagnostic.CloseHal();
}

#define APP_MODE_SOL          0
#define APP_MODE_RTD          1
#define APP_MODE_HTR          2
#define APP_MODE_PREAMP       3
#define APP_MODE_SERIAL       4
#define APP_MODE_DIG_IN       5
#define APP_MODE_DIG_OUT      6
#define APP_MODE_EPC          7
#define APP_MODE_ANA_IN       8
#define APP_MODE_ANA_OUT      9
#define APP_MODE_LTL          10
#define APP_MODE_EPC_RDBACK   11
#define APP_MODE_FFB_STATUS   12
#define APP_MODE_FFB_COMMAND  13
#define APP_MODE_DIAGNOSTICS  14
#define APP_MODE_FID          15
#define APP_MODE_CPU_ADC      16
#define APP_MODE_IRKEYPAD     17
#define APP_MODE_FPD          18
#define APP_MODE_RTD_PWM      19
#define APP_MODE_PRESSURE     20
#define APP_MODE_SERIAL_MODE  21
#define APP_MOD_DIG_IN_OUT    22
#define APP_MOD_ANA_IN_OUT    23
#define APP_MODE_IMB_COMMAND  24
#define APP_MODE_KEY_LOCK_SW  25
#define APP_MODE_KEY_LOCK_CHG 26
#define APP_MODE_FPD_G2       27
#define APP_MODE_DIAG_CONT    28

int main (int argc, char *argv[])
{
  int optVal = 0;
  int appMode = 0;

  if (argc < 2)
  {
    PrintHelp();
    return -1;
  }

  InstallDebug( argc, argv );

  srand( (unsigned)time(NULL) );

  while (argv[optind] != NULL)
  {
    optVal = getopt(argc, argv, "m:n:avc");

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
        case 'a':
          appMode = APP_MODE_CPU_ADC;
          break;

        case 'b':
          appMode = APP_MODE_FPD;
          break;

        case 'c':
          appMode = APP_MODE_FID;
          break;

        case 'd':
          appMode = APP_MODE_DIAGNOSTICS;
          break;

        case 'D':
          appMode = APP_MODE_DIAG_CONT;
          break;

        case 'e':
          appMode = APP_MODE_EPC;
          break;

        case 'E':
          appMode = APP_MODE_EPC_RDBACK;
          break;

        case 'f':
          appMode = APP_MODE_FFB_STATUS;
          break;

        case 'F':
          appMode = APP_MODE_FFB_COMMAND;
          break;

        case 'g':
          appMode = APP_MODE_IMB_COMMAND;
          break;

        case 'h':
          appMode = APP_MODE_HTR;
          break;

        case 'i':
          appMode = APP_MODE_DIG_IN;
          break;

        case 'I':
          appMode = APP_MOD_DIG_IN_OUT;
          break;

        case 'j':
          appMode = APP_MODE_FPD_G2;
          break;

        case 'k':
          appMode = APP_MODE_IRKEYPAD;
          break;

        case 'l':
          appMode = APP_MODE_LTL;
          break;
        
        case 'm':
          appMode = APP_MODE_PRESSURE;
          break;

        case 'n':
          appMode = APP_MODE_SERIAL_MODE;
          break;

        case 'o':
          appMode = APP_MODE_DIG_OUT;
          break;

        case 'p':
          appMode = APP_MODE_PREAMP;
          break;

        case 'r':
          appMode = APP_MODE_ANA_IN;
          break;

        case 'W':
          appMode = APP_MOD_ANA_IN_OUT;
          break;

        case 's':
          appMode = APP_MODE_SOL;
          break;

        case 't':
          appMode = APP_MODE_RTD;
          break;

        case 'u':
          appMode = APP_MODE_SERIAL;
          break;

        case 'w':
          appMode = APP_MODE_ANA_OUT;
          break;

        case 'x':
          appMode = APP_MODE_RTD_PWM;
          break;

        case 'z':
          appMode = APP_MODE_KEY_LOCK_SW;
          break;

        case 'Z':
          appMode = APP_MODE_KEY_LOCK_CHG;
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

    case 'c':
      g_nConfigure = 1;
      break;

    case 'a':
      g_b370XAIOBoards = true;
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
  case APP_MODE_KEY_LOCK_SW:
    TestKeyLockSW();
    break;

  case APP_MODE_KEY_LOCK_CHG:
    TrackKeyLockSWChanges();
    break;
    
  case APP_MODE_SOL:
    TestSol();
    break;

  case APP_MODE_RTD:
    TestRTD(nIndex);
    break;

  case APP_MODE_RTD_PWM:
    TestTempStability();
    break;

  case APP_MODE_HTR:
    TestHeaterCtrl();
    break;

  case APP_MODE_PREAMP:
    if (TestPreamp(nIndex) < 0)
    {
      printf("Invalid channel number.\n");
    }
    break;

  case APP_MODE_SERIAL:
    if (TestSerialIO(nIndex) < 0)
    {
      printf("Invalid channel number.\n");
    }
    break;
  
  case APP_MODE_DIG_IN:
    TestDigitalIn();
    break;

  case APP_MODE_DIG_OUT:
    TestDigitalOut();
    break;

  case APP_MOD_DIG_IN_OUT:
    TestLongRunDigital_InOut();
    break;

  case APP_MOD_ANA_IN_OUT:
    TestLongRunAnalog_InOut();
    break;

  case APP_MODE_ANA_IN:
    TestAnalogIn();
    break;

  case APP_MODE_ANA_OUT:
    TestAnalogOut();
    break;

  case APP_MODE_EPC:
    TestEPC();
    break;
  
  case APP_MODE_LTL:
    TestLtloi();
    break;

  case APP_MODE_EPC_RDBACK:
    TestEPCReadBack ();
    break;

  case APP_MODE_FFB_STATUS:
    SendGCStatus2FFBBoard();
    break;

  case APP_MODE_FFB_COMMAND:
    GetRemoteRequest4mFFBBoard();
    break;  

  case APP_MODE_DIAGNOSTICS:
    RunDiagnostics();
    break;

  case APP_MODE_DIAG_CONT:
    RunDiagsContinuous();
    break;

  case APP_MODE_FID:
    TestFID();
    break;

  case APP_MODE_CPU_ADC:
    TestCPUADC();
    break;

  case APP_MODE_IRKEYPAD:
    TestIRKeyPad();
    break;

  case APP_MODE_FPD:
    TestFPD();
    break;

  case APP_MODE_PRESSURE:
    TestPressure();
    break;

  case APP_MODE_SERIAL_MODE:
    TestSerialMode();
    break;

  case APP_MODE_IMB_COMMAND:
    TestIMBComm();
    break;

  case APP_MODE_FPD_G2:
    TestFPD_G2();
    break;
  }


  return 0;
  
}

// Read 5 RTDs and 4 Heater PWMs. Ensure that you kill xpheaterd before you run this.
void TestTempStability()
{
  static char * szRTDDevNames[5] = {NULL,NULL,NULL,NULL,NULL};
  int nNumRTDChannels = NR_RTD_CHANNELS;
  if( g_b370XAIOBoards )
  {
    nNumRTDChannels = ANALYZER_NR_RTD_CHANNELS;
    szRTDDevNames[0] = "RTD:ANALYZER_SLOT:RTD_1";
    szRTDDevNames[1] = "RTD:ANALYZER_SLOT:RTD_2";
  }
  else
  {
    szRTDDevNames[0] = "RTD:SLOT_2:RTD_1";
    szRTDDevNames[1] = "RTD:SLOT_2:RTD_2";
    szRTDDevNames[2] = "RTD:SLOT_2:RTD_3";
    szRTDDevNames[3] = "RTD:SLOT_2:RTD_4";
    szRTDDevNames[4] = "RTD:SLOT_2:RTD_5";
  };
  
  static CRTD obRTD[NR_RTD_CHANNELS];

  static char * szHtrDevNames[NR_HTR_CHANNELS]= {NULL,NULL,NULL,NULL};
  int nNumHtrChannels = NR_HTR_CHANNELS;
  if( g_b370XAIOBoards )
  {
    nNumHtrChannels = ANALYZER_NR_HTR_CHANNELS;
    szHtrDevNames[0] = "HTR_CTRL:ANALYZER_SLOT:HTR_CTRL_1";
    szHtrDevNames[1] = "HTR_CTRL:ANALYZER_SLOT:HTR_CTRL_2";
  }
  else
  {
    szHtrDevNames[0] = "HTR_CTRL:SLOT_2:HTR_CTRL_1";
    szHtrDevNames[1] = "HTR_CTRL:SLOT_2:HTR_CTRL_2";
    szHtrDevNames[2] = "HTR_CTRL:SLOT_2:HTR_CTRL_3";
    szHtrDevNames[3] = "HTR_CTRL:SLOT_2:HTR_CTRL_4";
  };
  
  static CHeaterCtrl obHtr[NR_HTR_CHANNELS];

  int nRetVal = 0;
  long nRTDVal = 0;
  unsigned int unValue = 0;

  // Open RTD Channels...
  for (int nRTDs = 0; nRTDs < nNumRTDChannels; nRTDs++)
  {
    nRetVal = obRTD[nRTDs].OpenHal(szRTDDevNames[nRTDs]);
    if (nRetVal < 0)
    {
      printf("Error %d opening RTD Channel: %d\n", nRetVal, nRTDs + 1);
      g_nExitApp = 1;
      break;
    }
  }

  // Open our heaters... Make sure you kill xpheaterrd before you run this program
  for (int nHeater = 0; nHeater < nNumHtrChannels; ++nHeater)
  {
    nRetVal = obHtr[nHeater].OpenHal(szHtrDevNames[nHeater]);
    
    if (nRetVal < 0)
    {
      DEBUG("Error %d opening Htr Channel: %d\n", nRetVal, nHeater);
      g_nExitApp = 1;
      break;
    }
  }

  while (g_nExitApp != 1) //TEST MODE
  {
    time_t curTime = time(NULL);
    struct tm *tm_curr = localtime( &curTime );
    
    // Print time when we acquired data...
    printf("%04d/%02d/%02d %02d:%02d:%02d, ", 
           tm_curr->tm_year + 1900, tm_curr->tm_mon + 1, tm_curr->tm_mday, 
           tm_curr->tm_hour, tm_curr->tm_min, tm_curr->tm_sec);
    
    // Read temperature values from the RTD, and print to the console
    for (int nRTDs = 0; nRTDs < nNumRTDChannels; nRTDs++)
    {
      
      nRetVal = obRTD[nRTDs].GetTempInMilliDegC(&nRTDVal);
      if (nRetVal < 0)
      {
        printf("CRTD[%d]..GetTempInMilliDegC() failed: %d\n", nRTDs + 1, nRetVal);
        g_nExitApp = 1;
      }
      else
      {
        printf ("%ld, ", nRTDVal);
      }
    }

    for (int nHeater = 0; nHeater < nNumHtrChannels; ++nHeater)
    {

      nRetVal = obHtr[nHeater].GetHtrChPWMMilliP(&unValue);
      if (nRetVal < 0)
      {
        printf("CHeaterCtrl[%d].GetHtrChPWMMilliP() failed.: %d\n", nHeater + 1, nRetVal);
        g_nExitApp = 1;
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
  
  // Close RTD Channels...
  for (int nRTDs = 0; nRTDs < nNumRTDChannels; nRTDs++)
  {
    nRetVal = obRTD[nRTDs].CloseHal();
    if (nRetVal < 0)
    {
      printf("Error %d closing RTD Channel: %d\n", nRetVal, nRTDs + 1);
    }
  }

  for (int nHtrs = 0; nHtrs < nNumHtrChannels; nHtrs++)
  {
    nRetVal = obHtr[nHtrs].CloseHal();
    
    if (nRetVal < 0)
    {
      printf("CHeaterCtrl[%d].Close() failed: %d\n", nHtrs + 1, nRetVal);
    }
  }
}

