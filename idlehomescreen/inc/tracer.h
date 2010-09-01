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
#ifndef __TTRACER_H__
#define __TTRACER_H__

#include <e32std.h>
#include <e32svr.h>
#include <f32file.h>

//#ifdef _DEBUG
#define TRACER_ENABLED
//#else
//#undef TRACER_ENABLED
//#endif

/** Macro to initialise the tracer. Set the indentation level to zero. */
#ifdef TRACER_ENABLED
#define INITTRACE TTracer t
#else
#define INITTRACE
#endif

/** Macro to trace the function calls.
  * @param a Text to display. Usually class::function */
#ifdef TRACER_ENABLED
#define TRACE(a) TTracer t(_L(a))
#else
#define TRACE(a)
#endif

/** Macro to print a string to the trace window.
  * @param a Text to print.
  */
#ifdef TRACER_ENABLED
#define TRACESTR(a) TTracer::Print(_L(a))
#else
#define TRACESTR
#endif

/** 
 * @ingroup group_xnutils
 * 
 * Class for tracing function calls. Indentation shows the call tree.
 * Also the entry and leave points are time labeled, show timing info is available. 
 */
class TTracer
{
public:
	/** Initialising constructor. */
	IMPORT_C TTracer();

	/** Tracing constructor.
	  * Writes time lable and "Enter: " + the given string.
	  */
	IMPORT_C TTracer(TPtrC aString);

	/** Destructor.
	  * Writes time lable and "Leave: " + the given string.
	  */
	IMPORT_C ~TTracer();

	/** Prints the given string to the trace window.
	  * @param a Text to print.
	  */
    IMPORT_C static void Print(TPtrC aString);
//    static TInt Print(TRefByValue<const TDesC> aFmt,...);

private:
    RFs iFs;

	TBuf<255> iString;
//	RFileLogger iFLogger;
};


#endif // __TTRACER_H__
