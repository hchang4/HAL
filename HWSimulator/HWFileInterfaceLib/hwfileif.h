/***********************************************************************
 * *                          Rosemount Analytical 
 * *                    10241 West Little York, Suite 200 
 * *                           Houston, TX 77040 
 * *                                             
 * *
 * *  Filename: hwfileif.h
 * *
 * *  Description: Hardware file interface library. Use this library
 * *               to create, access and modify 'tags', which represent
 * *               specific device functions.
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
 * *  $Id: hwfileif.h,v 1.5 2015/04/30 09:27:58 EMRSN\Dkad Exp $
 * *  $Log: hwfileif.h,v $
 * *  Revision 1.5  2015/04/30 09:27:58  EMRSN\Dkad
 * *  Fixed Bug 7550 - Need to change indentation style for firmware source files.
 * *  Fix : Updated the indentation in the source files(.h) as per below requirements.
 * *  -Set the indent style to C/C++.
 * *  -Set the indent size to 2, and replace tab with blank space.
 * *  -Turn off interpretation of tab
 * *
 * *  Revision 1.4  2014/06/26 09:59:02  EMRSN\sboyidi
 * *  Fixed Bug - Indentation style change for firmware source files under 'all/src'.
 * *  Fix : Updated the indentation in all the source files as per below requirements.
 * *        - Set the indent style to C/C++.
 * *        - Set the indent size to 2, and replace tab with blank space.
 * *        - Turn off interpretation of tab.
 * *
 * *  Revision 1.3  2012/03/21 06:48:32  EMRSN\jchekkala
 * *  File header was updated with Company Name and Address.
 * *
 * *  Revision 1.2  2007/03/01 07:10:42  EPM-NA\ssatpute
 * *  Increased tag value length to 255 characters. This is universal and applies to all data types (int/float/char *).
 * *
 * *
 * *************************************************************************/




#ifndef _HWFILEIF_H_
#define _HWFILEIF_H_


typedef long long tagcount;

#define MAX_FILE_PATH_LEN     255
#define FILE_PATH_STRING_LEN  (MAX_FILE_PATH_LEN + 1)

#define MAX_TAG_LEN       50
#define TAG_STRING_LEN    (MAX_TAG_LEN + 1)

//Length of the value represented in string format (including sign character)
#define MAX_TAG_VAL_LEN   255
//Length of the string = length of value (including sign) + NULL termination
#define TAG_VAL_STRING_LEN  (MAX_TAG_VAL_LEN + 1)
//For floating point data, the decimal separator
#define TAG_VAL_DECML_SEPR  15

//Length of the count represented in string format
#define MAX_TAG_CNT_LEN     30
//Length of the string = length of count (including sign) + NULL termination
#define TAG_CNT_STRING_LEN  (MAX_TAG_CNT_LEN + 1)

#define TAG_FIELD_SEPR    ':'
#define TAG_VALUE_SEPR    ','

//Initial count value for a new tag
#define TAG_INIT_COUNT    1LL


//Hardware file interface error codes
typedef enum
{
  //No error
  FILEIF_NO_ERROR = 0,
  //The user has not supplied a file name
  FILEIF_ERR_NO_FILENAME = -1,
  //File path is too long
  FILEIF_ERR_PATH_LONG = -2,
  //Error occurred when accessing the user specified file
  FILEIF_ERR_FILE_ACCESS = -3,
  //Error occurred when opening the user specified file
  FILEIF_ERR_FILE_OPEN = -4,
  //Error occurred when reading the user specified file
  FILEIF_ERR_FILE_RD = -5,
  //Error occurred when writing to the user specified file
  FILEIF_ERR_FILE_WR = -6,
  //Arguments to a function were invalid
  FILEIF_ERR_INVALID_ARGS = -7,
  //Tag too long
  FILEIF_ERR_LONG_TAG = -8,
  //Tag not found
  FILEIF_ERR_NO_TAG = -9,
  //Error occurred when parsing the file
  FILEIF_ERR_PARSE = -10,
  //Duplicate tag creation failed
  FILEIF_ERR_DUP_TAG = -11,
  //Not a valid tag name
  FILEIF_ERR_TAG_INVALID = -12,
  //Internal error occurred
  FILEIF_ERR_INT_ERR = -13,
  //Memory error
  FILEIF_ERR_MEMORY = -14,
  //Tag count error
  FILEIF_ERR_COUNT = -15,
  //Out of sequence function call
  FILEIF_ERR_SEQ = -16,
  //File lock error
  FILEIF_ERR_LOCK = -17,
  //Value too long (for string type)
  FILEIF_ERR_LONG_VAL = -18,
} E_FILEIF_ERRS;

//Tag data type enumeration
typedef enum
{
  DTYPE_INT = 0,
  DTYPE_FLOAT, 
  DTYPE_STRING,
  MAX_DATA_TYPE,
} E_DATA_TYPE;

//Tag fields enumeration
typedef enum
{
  TAG_FUNC_NAME = 0,
  TAG_TYPE_NAME,
  TAG_FIELD_VALUE,
  TAG_FIELD_COUNT,
  MAX_TAG_FIELDS,
} E_TAG_FIELD;

//Tag operation
typedef enum
{
  TAG_WR_DEV = 0,
  TAG_RD_DEV,
  MAX_TAG_OPER,
} E_TAG_OPER;

//File lock operation
typedef enum
{
  FLOCK_OPER_LOCK = 0,
  FLOCK_OPER_UNLOCK,
  MAX_LOCK_OPER,
} E_LOCK_OPER;


//Structure to maintain a list of successfully accessed
//  tags (through Create, Read or Write) and keep track
//  of the last read count. This count will be used to 
//  indicate to the reader if the value in the tag has
//  been updated compared to the last time the tag value
//  was read.
typedef struct 
{
  char szTagName[TAG_STRING_LEN];
  tagcount llLastReadCount;
} STagsAccessed;


class CHwFileIf
{
private:
  //Name of the file that stores the tags and their value
  char m_szFileName[FILE_PATH_STRING_LEN];

  //Internal error code
  int m_nErrorCode;

  //File descriptor
  int m_fdFile;

  //Set in SelectTagField(...). 
  E_TAG_FIELD m_eIsTagSelected;

  //Number of unique tags successfully accessed
  unsigned long m_ulTagsAccessedCount;

  int m_nEnableIncCount;

  STagsAccessed *m_pstTagsAccessed;

  //Check if the tag is valid
  int CheckTagValidity(char *pszTag);

  //Create and initialize a tag with value and count
  int CreateTag(char *pszTag, void *pData, E_DATA_TYPE eDataType);

  //Go to the specified tag - to be called before 
  // calling SetValue(...) or GetValue(...)
  int SelectTagField(char *pszTag, E_TAG_FIELD eField);
  
  //Store value for the specified tag (to be called after SelectTagField(...) )
  int SetValue(char *pszTag, void *pData, E_DATA_TYPE eDataType);

  //Get value of the specified tag (to be called after SelectTagField(...) )
  int GetValue(char *pszTag, void *pData, E_DATA_TYPE eDataType, int *pnIsNew);

  //Get write count for the selected tag (to be called after SelectTagField(...) )
  int GetCount(char *pszTag, tagcount *llCount);

  //Get write count for the selected tag (to be called from SetValue(...))
  int IncCount();

  //Check and updated the local list of successfully accessed tags
  int UpdateLocalTagList(char *pszTag);

  //Check if the data being read is new (by comparing the count field with the
  //  local count).
  int CheckNewData(char *pszTag, int *pnIsNew);

  //File access lock/unlock routine
  int FileLock(E_TAG_OPER eTagOper, E_LOCK_OPER eLockOper);

public:
  //Constructor
  CHwFileIf(char *pszFileName);
  //Destructor
  ~CHwFileIf();

  //Create & initialize a tag
  int CreateDevice(char *pszTag, int nInitVal);
  int CreateDevice(char *pszTag, float fInitVal);
  int CreateDevice(char *pszTag, char *szInitVal);

  //Write to the specified tag
  int WriteDevice(char *pszTag, int nValue);
  int WriteDevice(char *pszTag, float fValue);
  int WriteDevice(char *pszTag, char *pszValue);

  //Read from the specified tag
  int ReadDevice(char *pszTag, int *pnValue, int *pnIsNew);
  int ReadDevice(char *pszTag, float *pfValue, int *pnIsNew);
  int ReadDevice(char *pszTag, char *pszValue, int *pnIsNew);
};


#endif
