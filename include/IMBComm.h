/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: IMBComm.h
 * *
 * *  Description: IMB Communication layer.
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
 * *  $Id: IMBComm.h,v 1.18 2016/06/13 16:19:25 emrsn\wjyu Exp $
 * *  $Log: IMBComm.h,v $
 * *  Revision 1.18  2016/06/13 16:19:25  emrsn\wjyu
 * *  Revised for TestHAL application
 * *
 * *  Revision 1.17  2016/05/27 20:20:45  emrsn\wjyu
 * *  revision to support C7+ application
 * *
 * *  Revision 1.16  2015/04/30 09:32:32  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.15  2014/10/22 18:08:31  emrsn\wjyu
 * *  Revise the format of the imb serial number string in the format of (left to right) word 3, word 2, word 1, word 0
 * *  (It was in the format of word 0, word 1, word 2, word 3)
 * *
 * *  Revision 1.14  2014/10/03 21:02:04  emrsn\wjyu
 * *  Revised GetFlashImage() and SetFlashImage() API to support FACTORY and MIRROR section.
 * *  Before, those two functions only support the GLOBAL section
 * *  (For 6587 GC should not allow reading Module data if IMB is Incompatible )
 * *
 * *  Revision 1.13  2014/06/26 10:01:15  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.12  2014/05/18 23:46:25  emrsn\wjyu
 * *  IMBComm revision - Added GetAPIID() method
 * *
 * *  Revision 1.11  2014/05/14 10:03:01  emrsn\wjyu
 * *  For IMB Firmware 1.0.1
 * *  Revised IMBComm, added GetIMBInfo() function
 * *
 * *  Revision 1.10  2014/05/02 19:03:53  emrsn\wjyu
 * *  For BZ 7292 - Need to store the 12 digit Factory Assigned Module Serial Number in IMB
 * *  Revised GC IMBComm
 * *
 * *  Revision 1.9  2013/05/20 21:01:55  emrsn\wjyu
 * *  Added function to update IMB Flash
 * *
 * *  Revision 1.8  2013/03/07 14:43:27  emrsn\wjyu
 * *  Add function to read IMB Flash Image
 * *
 * *  Revision 1.7  2013/02/08 17:20:05  emrsn\rsaha
 * *  Revised IMB class interface to retrieve IMB Database ID. A new method GetDBID() is added to retrieve the same. Database identifier is only one, section wise database identifiers has been removed.
 * *
 * *  Revision 1.6  2013/01/03 11:59:53  emrsn\rsaha
 * *  IMB firmware is revised with the following -
 * *
 * *  - Database ID is moved from Global Data section. Now Factory, Mirror and Default data section all have their individual Database Identifier field.- Time Event Adjustment Data also made part of Mirror Data Section.
 * *  - Revised Project settings to use 512 bytes less for stack region, since due to TEV Adj Data structure inclusion in Mirror Data Section there is increase of about 385 bytes of RAM usage.
 * *
 * *  Revision 1.5  2012/11/30 08:53:19  emrsn\rsaha
 * *  - DB ID is now in Global data section, previously it was present in all the three - Default, Factory and Mirror secotions.
 * *  - Software TEV table row size is increased from 16 to 30 and Gain TEV table row size is increased from 2 to 3.
 * *  - Revised AVT settings and AVT Valve Sel - Removed Peak Area Change limit from AVT settings and added it in AVT Valve.
 * *  - Revised AVT Time Event Adjustment table -
 * *          1. Removed Default RT.
 * *          2. Added Ideal RF Ration.
 * *          3. Added Ideal RF Reference Component.
 * *          4. Added Ideal RF Limit.
 * *  - Added AVT Settings, Valve Selection and Time Event Adjustments to Factory section also, previously they were in Default section only.
 * *  - Carrier and Sample Zero and Span calibration factors are placed in Mirror sections also, previously they were in Factory section only.
 * *  - Added Analysis Time configurations in all three - Default, Factory and Mirror sections.
 * *
 * *  - Revised Communication interface with CPU HAL layer to Read/Write the newly revised fields.
 * *
 * *  Revision 1.4  2012/03/21 06:49:07  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.3  2012/02/07 11:34:47  emrsn\rsaha
 * *  Revised to support -
 * *  1. Stored rows for each tables in Section data.
 * *  2. Carrier/Sample Zero and Span calibration values for Factory section.
 * *
 * *  Revision 1.2  2012/01/31 15:06:54  emrsn\rsaha
 * *  Revised communication between HAL and IMB with smaller packet sizes.
 * *
 * *  Revision 1.1  2012/01/03 13:04:15  emrsn\rsaha
 * *  Added support to communicate with IMB device.
 * *
 * *************************************************************************/



#ifndef _IMB_COMM_H
#define _IMB_COMM_H

#include <string>                     // for std::string
#include "BaseDev.h"
#include "IMBProtocol.h"

#define IMB_SERIAL_NUMBER_LENGTH_IN_CHARS (32)

const size_t INVALID_OFFSET2 = 0xFFFF;

// Digital Input Device
class CIMBComm : public CBaseDev {

private:
  //int m_nAPIVer;

private:
  // private functions
  DB_INT32 setFactoryMirrorFlashImage2(const IMB_FLASH_SECTION_ENUM _eIMBSection, const size_t factoryOffset, const size_t mirrorOffset, unsigned int datalen, unsigned char * pstSwTEVTable);
  DB_INT32 getFactoryMirrorFlashImage2(const IMB_FLASH_SECTION_ENUM _eIMBSection, const size_t factoryOffset, const size_t mirrorOffset, unsigned int datalen, unsigned char * pstSwTEVTable);
  
public:
  CIMBComm(); // Default Constructor
  
  ~CIMBComm();  // Destructor

  // OpenHal the device. Returns 0 on success, negative error code on failure.
  int OpenHal (char* pszDevName);

  // Closes the device. Returns 0 on success, negative error code on failure.
  int CloseHal ();

  // This has to be set before using API-version-dependent functions !!!!
  //void SetAPIVer(const int nAPIVer);

  int GetStructureID( IMB_STRUCTURE_ID_STRUCT * pstIMBDbId );
  int GetAPIID( IMB_API_ID_STRUCT * pstIMBApiId );
  int ConvertIMBSerialNumberToHex(const IMB_IMB_META_INFO & refIMBInfo, std::string & strImbSerialNumInHex );

  int GetIMBInfo( IMB_IMB_META_INFO * pstMetaInfo );

  //######################################################################
  // get and set functions to deal with the IMB data (FACTORY and MIRROR)
  //######################################################################



  int SetSectionMetaInfo2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_SECTION_META_INFO2 & stMetaInfo );
  int GetSectionMetaInfo2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_SECTION_META_INFO2 * pstMetaInfo );

  int SetSectionAVTSettingsTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_SETTINGS_TABLE_STRUCT & stAVTSettingsTable );
  int GetSectionAVTSettingsTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_SETTINGS_TABLE_STRUCT * pstAVTSettingsTable );

  int SetSectionAVTValveSelTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_VALVE_SEL_TABLE_STRUCT & stAVTValveSelTable );
  int GetSectionAVTValveSelTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_VALVE_SEL_TABLE_STRUCT * pstAVTValveSelTable );

  int SetSectionAVTTimeEventAdjTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 & stAVTTEATable );
  int GetSectionAVTTimeEventAdjTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT2 * pstAVTTEATable );

  int SetSectionCDTTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_CDT_TABLE_STRUCT & stCDTTable );
  int GetSectionCDTTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_TABLE_STRUCT * pstCDTTable );

  //int SetSectionCDTUpdate( IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_UPDATE_STRUCT stCDTUpdate );
  //int GetSectionCDTUpdate( IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_UPDATE_STRUCT * pstCDTUpdate );

  int SetSectionHwTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_HW_TEV_TABLE_STRUCT & stHwTEVTable );
  int GetSectionHwTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_HW_TEV_TABLE_STRUCT * pstHwTEVTable );

  int SetSectionSwTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_SW_TEV_TABLE_STRUCT & stSwTEVTable );
  int GetSectionSwTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_SW_TEV_TABLE_STRUCT * pstSwTEVTable );

  int SetSectionGainTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_GAIN_TEV_TABLE_STRUCT & stGainTEVTable );
  int GetSectionGainTEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_GAIN_TEV_TABLE_STRUCT * pstGainTEVTable );

  int SetSectionAnalysisConfigTable2(const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_ANALYSIS_TIME_CONFIG_STRUCT & stAnlyTimeCfgTable );
  int GetSectionAnalysisConfigTable2(const IMB_FLASH_SECTION_ENUM eSectionType, IMB_ANALYSIS_TIME_CONFIG_STRUCT * pstAnlyTimeCfgTable );

  int SetSectionSetPoints2(const IMB_FLASH_SECTION_ENUM eSectionType, const float fPresSetPoint, const float fTempSetPoint );
  int GetSectionSetPoints2(const IMB_FLASH_SECTION_ENUM eSectionType, float * pfPresSetPoint, float *pfTempSetPoint );

  int SetMirrorAVTEnable2( const unsigned char byAVTEnable );
  int GetMirrorAVTEnable2( unsigned char * pbyAVTEnable );

  int GetMirrorNumAnalysis2( int * pMirrorNumAnalysis );
  int SetMirrorNumAnalysis2( const int nMirrorNumAnalysis );

  int SetSectionCarrierCalFactors2( const IMB_FLASH_SECTION_ENUM eSectionType, const float fZeroCalFaq, const float fSpanCalFaq );
  int GetSectionCarrierCalFactors2( const IMB_FLASH_SECTION_ENUM eSectionType, float * pfZeroCalFaq, float * pfSpanCalFaq );

  // NOT USED
  //int SetSectionSampleCalFaq( IMB_FLASH_SECTION_ENUM eSectionType, float fZeroCalFaq, float fSpanCalFaq );
  //int GetSectionSampleCalFaq( IMB_FLASH_SECTION_ENUM eSectionType, float * pfZeroCalFaq, float * pfSpanCalFaq );


  int SetFlashImage( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int unOffset, const unsigned int unLength);
  int GetFlashImage( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int unOffset, const unsigned int unLength, unsigned int& refActualLen );

  int SetFlashImage2( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int unOffset, const unsigned int unLength);
  int GetFlashImage2( unsigned char * pchFlashImage, const unsigned char bySection, const unsigned int unOffset, const unsigned int unLength, unsigned int& refActualLen );


  int UpdateFlash();

  //######################################################################
  // VERSION_2 stuff, new structures
  //######################################################################
  int SetSectionSwTEVTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_SW_TEV_TABLE_STRUCT_EXT & stSwTEVTableExt );
  int GetSectionSwTEVTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_SW_TEV_TABLE_STRUCT_EXT * pstSwTEVTableExt );

  int SetSectionCDTTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_CDT_TABLE_STRUCT_EXT & stCDTTableExt );
  int GetSectionCDTTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_TABLE_STRUCT_EXT * pstCDTTableExt );

  int SetSectionCDTCustomNames2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_CDT_CUSTOM_NAME_STRUCT & stCDTCustom );
  int GetSectionCDTCustomNames2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_CDT_CUSTOM_NAME_STRUCT * pstCDTCustom );

  int SetSectionAVTTimeEventAdjTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT & stAVTTEATableExt );
  int GetSectionAVTTimeEventAdjTableExt2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_AVT_TIME_EVENT_ADJ_TABLE_STRUCT_EXT * pstAVTTEATableExt );

  int SetSectionMVAllowableDEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, const IMB_MV_ALLOWABLE_DEV_STRUCT & stMVAllowableDEVTable );
  int GetSectionMVAllowableDEVTable2( const IMB_FLASH_SECTION_ENUM eSectionType, IMB_MV_ALLOWABLE_DEV_STRUCT * pstMVAllowableDEVTable );

  //######################################################################
  // functions to deal with GLOBAL data
  //######################################################################
  int SetGlobalDataModuleSerialNum2( unsigned char * pchData );
  int GetGlobalDataModuleSerialNum2( unsigned char * pchData );

  int SetGlobalDataIMBInit2( unsigned char byIMBInit );
  int GetGlobalDataIMBInit2( unsigned char * pbyIMBInit );

  int SetGlobalDataMetaInfo2( const IMB_SECTION_META_INFO2 & _stMetaInfo );
  int GetGlobalDataMetaInfo2( IMB_SECTION_META_INFO2 * pstMetaInfo );

  int SetGlobalDataApplicationType2( const IMB_APPLICATION_TYPE_STRUCT & stAppType );
  int GetGlobalDataApplicationType2( IMB_APPLICATION_TYPE_STRUCT * pstApplicationType );

};

#endif // #ifndef _IMB_COMM_H



