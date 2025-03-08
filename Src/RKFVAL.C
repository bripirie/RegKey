/*





                                                                    R E G K E Y

 ------------------------------------------------------------------------------


                                                                   Version 3.20


                                    The Registration Key System For Programmers


                                                                    Source Code




          (C) Copyright Brian Pirie, 1993 - 1998. All Rights Reserved.

*/


/*
        FILE : rkfval.c

     VERSION : 3.20

     PURPOSE : Source code for the RegKeyFileValidate() function

  NOTICE - The conditions of use of this source code are specified in the
           RegKey documentation. Use of the RegKey source code is subject
           to a non-disclosure agreement. Please remember that you are not
           permitted to redistribute this source code.
*/


#define BUILDING_REGKEY
#include "regkey.h"
#include "rkintern.h"


/* RegKeyFileValidate()                                                      */
/*                                                                           */
/* Checks whether the specified registration key file is valid for a         */
/* particular application, using the application-specified validation code   */
/* that was generated by RegKeyNewCodeSet(). The RKVALID pointed to by       */
/* peRegistered is set to either RK_REGISTERED or RK_UNREGISTERED,           */
/* indicating whether or not the registration key and registration string    */
/* stored in the registration key file are valid. The szFileName parameter   */
/* may include wildcards. If you have registered RegKey, your own name and   */
/* RegKey registration key should be passed to this function to diable the   */
/* RegKey "unregistered" message.                                            */
/*                                                                           */
/* This function is called from within your application each time it         */
/* executes, in order to determine whether it should operate in registered   */
/* or unregistered mode. This function is used with file-based registration  */
/* keys; compare with RegKeyValidate().                                      */
/*                                                                           */
/* szFileName          INPUT: Registration key file name                     */
/* szValidationCode    INPUT: App's validation code                          */
/* szYourName          INPUT: Your name (if registered)                      */
/* szYourKey           INPUT: Your key (if registered)                       */
/* cbMaxStringSize     INPUT: Characters in reg. string                      */
/* szRegString         OUTPUT: Registration string                           */
/* peRegistered        OUTPUT: Is key valid                                  */

RKFUNCDEF RKRETURN RKCALL RegKeyFileValidate(
   CONST char *szFileName,
   CONST char *szValidationCode,
   CONST char *szYourName,
   unsigned long int nYourKey,
   char *szRegString,
   unsigned short int cbMaxStringSize,
   RKVALID *peRegistered)
{
   RKRETURN toReturn;
   char szRegBuffer[INTERNAL_STRING_CHARS+1];
   char szRegKey[21];
   int cbSize;
   int cbCut;
   BOOL *pbModInited;
   char *cp;

   /* Validate Parameters */
   if(!szValidationCode || !peRegistered)
   {
      return(RK_FAILURE);
   }

   /* If szFilename is NULL or empty */
   if(!szFileName || *szFileName == '\0')
   {
      /* Default to first file found with ".KEY" extension */
      szFileName = (char *) "*.KEY";
   }

   /* If no registration string was supplied */
   if(!szRegString || cbMaxStringSize == 0)
   {
      /* Then use internal registration string */
      szRegString = szRegBuffer;
      cbMaxStringSize = INTERNAL_STRING_CHARS+1;
   }

   /* Read contents of registration key file */
   cbSize = cbMaxStringSize - 1;
   if(!fileGet(szFileName, szRegKey, szRegString, &cbSize))
   {
      *peRegistered = RK_UNREGISTERED;
      return(RK_SUCCESS);
   }

   cp = szRegString;
   while(*cp) ++cp;

   for(cbCut = 0; cbCut < cbSize; ++cbCut)
   {
      --cp;
      if(*cp == '\n' || *cp == '\r')
      {
         *cp = '\0';
      }
      else
      {
         break;
      }
   }

   /* Determine whether registration file is valid using RegKeyValidate() */
#ifdef NOT_C_TARGET
   toReturn=rkv(szRegString, szRegKey, szValidationCode, szYourName,
      nYourKey, peRegistered);
#else
   toReturn=RegKeyValidate(szRegString, szRegKey, szValidationCode, szYourName,
      nYourKey, peRegistered);
#endif

   /* If RegKeyValidate was successful, and user is not registered */
   if(toReturn == RK_SUCCESS && *peRegistered != RK_REGISTERED)
   {
      /* Truncate registration string */
      *szRegString = '\0';
   }

   /* Return appropriate value */
   return(toReturn);
}
