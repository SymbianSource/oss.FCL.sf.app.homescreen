/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   
*
*/

#include "tracer.h"

#ifdef __WINS__
// Yes, I know this does not compile for Arm, but at least we can use it in Wins
static TInt ii;
#endif

#ifdef FILE_LOGGING_SUPPORT
_LIT(KTracerLogFilePath, "c:\\logs\\xn3trace.log");
#endif

EXPORT_C TTracer::TTracer()
{
#ifdef FILE_LOGGING_SUPPORT
    RFs fs;
    TInt err(fs.Connect());
    if (KErrNone == err)
        {
        RFile file;
        file.Replace(fs, KTracerLogFilePath, EFileWrite);
    	file.Close();
    	fs.Close();
        }
#endif

#ifdef __WINS__
	ii = 0;
#endif
}

EXPORT_C TTracer::TTracer(TPtrC aString)
{
#ifdef FILE_LOGGING_SUPPORT
    TInt err (iFs.Connect());
    if (KErrNone == err)
        {
        RFile file;
        err (file.Open(iFs, KTracerLogFilePath, EFileWrite));
        if (KErrNone == err)
            {
            TFileText tfile;
            tfile.Set(file);
#endif

	TBuf<255> str=_L("");
	TTime currentTime; // set current time to now
	currentTime.HomeTime();
	TBuf<25> strCurTime;
	TRAP_IGNORE( currentTime.FormatL(strCurTime, _L("%H:%T:%S:%*C3 ")) );

	str.Copy(strCurTime);

#ifdef __WINS__
	for(int i=0;i<ii;i++)
	{
		str.Append(_L("  "));
	}
#else
	str.Append(_L(" "));
#endif

	iString = aString;
	str.Append(_L("Enter: "));
	str.Append(iString);

#ifdef FILE_LOGGING_SUPPORT
        	str.Append(_L("\r")); // carriage return
        	tfile.Seek(ESeekEnd);
        	tfile.Write(str);
            }
    	file.Close();
        }
#endif

#ifdef __WINS__
	ii++;
#endif
}


EXPORT_C TTracer::~TTracer()
{
#ifdef __WINS__
	if(ii > 0)
	{
		--ii;
#endif

#ifdef FILE_LOGGING_SUPPORT
        RFile file;
        TInt err (file.Open(iFs, KTracerLogFilePath, EFileWrite));
        if (KErrNone == err)
            {
            TFileText tfile;
            tfile.Set(file);
#endif

		TBuf<255> str=_L("");
		TTime currentTime; // set current time to now
		currentTime.HomeTime();
		TBuf<25> strCurTime;
		TRAP_IGNORE( currentTime.FormatL(strCurTime, _L("%H:%T:%S:%*C3 ")) );

		str.Copy(strCurTime);

#ifdef __WINS__
		for(int i=0;i<ii;i++)
		{
			str.Append(_L("  "));
		}
#else
		str.Append(_L(" "));
#endif
		str.Append(_L("Leave: "));
		str.Append(iString);

#ifdef FILE_LOGGING_SUPPORT
        	str.Append(_L("\r")); // carriage return
        	tfile.Seek(ESeekEnd);
        	tfile.Write(str);
            }
    	file.Close();
    	iFs.Close();
#endif
#ifdef __WINS__
	}
#endif
}

EXPORT_C void TTracer::Print(TPtrC aString)
{
#ifdef FILE_LOGGING_SUPPORT
    RFs fs;
    TInt err(fs.Connect());
    if (KErrNone == err)
        {
        RFile file;
        TInt err(file.Open(fs, KTracerLogFilePath, EFileWrite));
        if (KErrNone == err)
            {
            TFileText tfile;
            tfile.Set(file);
#endif
	TBuf<255> str=_L("");
	TTime currentTime; // set current time to now
	currentTime.HomeTime();
	TBuf<25> strCurTime;
	TRAP_IGNORE( currentTime.FormatL(strCurTime, _L("%H:%T:%S:%*C3 ")) );

	str.Copy(strCurTime);

	str.Append(_L(" "));
	str.Append(aString);
#ifdef FILE_LOGGING_SUPPORT
        	str.Append(_L("\r")); // carriage return
        	tfile.Seek(ESeekEnd);
        	tfile.Write(str);
            }
    	file.Close();
	    fs.Close();
        }
#endif
}

