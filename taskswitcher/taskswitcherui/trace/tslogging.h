/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines all available logging macros.
*
*/


#ifndef TSLOGLOGUTILS_H
#define TSLOGLOGUTILS_H

/**
 * @file
 * 
 * TAKING LOGGING INTO USE:
 * 
 * This step is needed to do once per component.
 * 
 * For each component that uses these common logging macros should specify
 * their own logging configuration file, which includes this file. In that
 * configuration file, following constants and macros must be defined.
 *  For example:
 * @code
 * 
 * _LIT( KTsLogFile, "text_file_for_logging.txt" );
 * _LIT( KTsLogPath, "folder_for_text_file" );
 * #define _TSLOG_LOG_COMPONENT_ID <some unique number here>
 * 
 * #include "tslogging.h"
 * 
 * @/code
 * 
 * KTsLogFile   : This is the name of the file, where all the logs for
 *                this components are being written.
 * 
 * KTsLogPath   : This is the folder name under c:\logs, where the file
 *                is to be stored. For example, if KTsLogPath is "test",
 *                log file is created into folder c:\logs\test.
 * 
 * _TSLOG_LOG_COMPONENT_ID  : Unique number id of the component. This is 
 *                            for filtering purposes.
 * 
 * _TSLOG_RDEBUG    : When defined tracing instead of file logging. 
 *                    Default is for file logging.
 * 
 * --------------------------------------------------------------------------
 * 
 * USING LOGGING:
 * 
 * Basically the use is simple, register function use with TSLOG_CONTEXT, 
 * then log function enter by any TSLOG_IN -macro, then possibly use TSLOG
 * -macros for function logging and finally TSLOG_OUT -macros for returning
 * from the function.
 * 
 * @code
 * TInt CGood::Example( TInt aSomething )
 *     {
 *     // Create log context class, which is maintained for lifetime of the 
 *     // method. 
 *     TSLOG_CONTEXT( Example, TSLOG_LOCAL );
 * 
 *     // Indicate we are entering the function. 
 *     TSLOG_IN1( "aSomething contains value %d", aSomething );
 * 
 *     // Your buggy code...
 * 
 *     // Before leaving, indicate function execution has ended.
 *     TSLOG_OUT();
 * 
 *     return 0;
 *     }
 * @/code
 */

#include <e32def.h>
#include <e32std.h>

#include "tslogutils.h"

#define TSLOG_API 0
#define TSLOG_LOCAL 1

#define TSLOG_INFO 0
#define TSLOG_WARNING 1
#define TSLOG_ERROR 2

#define TSLOG_ASSERT(_assertion)  __TSLOG_ASSERT_DBG(_assertion) 
#define TSLOG_TRACE_ASSERT(_assertion) __ASSERT_DEBUG((_assertion), User::Invariant() )

#ifdef _DEBUG
            
/*****************************************************************************
 LOGGING MACROS - LOGGING ON
*****************************************************************************/             

    /** 
     * Context initialization 
     * NOTE: TSLOG_STATIC_CONTEXT is meant for static methods.
     * 
     * @param _fn   Name of the function.
     * @param _vis  Visibility for the client, use values TSLOG_API or TSLOG_LOCAL
     * @param _thdId For static functions, thread id can be given here.
     */
    #define TSLOG_CONTEXT(_fn, _vis ) _TTsLogContext _dc((TText*)L ## #_fn, _TSLOG_LOG_COMPONENT_ID, _vis, (TUint)this, RProcess().SecureId().iId )
    #define TSLOG_STATIC_CONTEXT(_fn, _vis, _thdId) _TTsLogContext _dc((TText*)L ## #_fn, _TSLOG_LOG_COMPONENT_ID, _vis, _thdId, RProcess().SecureId().iId )

    /** 
     * Entering function 
     * 
     * @param string    Custom text. Example: TSLOG_IN0( "Yeah!!!" );
     * @param p1 - p5   For multiple variables in same string.
     */
    #define TSLOG_IN() do { _CHK_MULTIIN(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
    #define TSLOG0_IN(string) do { _CHK_MULTIIN(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
    #define TSLOG1_IN(string, p1) do { _CHK_MULTIIN(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); _MARK_ENTRY(); } while(0)
    #define TSLOG2_IN(string, p1, p2) do { _CHK_MULTIIN(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_ENTRY(); } while(0)
    #define TSLOG3_IN(string, p1, p2, p3) do { _CHK_MULTIIN(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_ENTRY(); } while(0)
    #define TSLOG4_IN(string, p1, p2, p3, p4) do { _CHK_MULTIIN(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_ENTRY(); } while(0)
    #define TSLOG5_IN(string, p1, p2, p3, p4, p5) do { _CHK_MULTIIN(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_ENTRY(); } while(0)

    /** Leaving function */
    #define TSLOG_OUT() do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
    #define TSLOG0_OUT(string) do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
    #define TSLOG1_OUT(string, p1) do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); _MARK_EXIT(); } while(0)
    #define TSLOG2_OUT(string, p1, p2) do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_EXIT(); } while(0)
    #define TSLOG3_OUT(string, p1, p2, p3) do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_EXIT(); } while(0)
    #define TSLOG4_OUT(string, p1, p2, p3, p4) do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_EXIT(); } while(0)
    #define TSLOG5_OUT(string, p1, p2, p3, p4, p5) do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0)

    /** Leaving function with return value */
    #define TSLOG0_RET(val, fmtstr) do { do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val); _MARK_EXIT(); } while(0); return val;} while(0)
    #define TSLOG1_RET(val, fmtstr, p1) do { do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1); _MARK_EXIT(); } while(0); return val;} while(0)
    #define TSLOG2_RET(val, fmtstr, p1, p2) do { do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2); _MARK_EXIT(); } while(0); return val;} while(0)
    #define TSLOG3_RET(val, fmtstr, p1, p2, p3) do { do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3); _MARK_EXIT(); } while(0); return val;} while(0)
    #define TSLOG4_RET(val, fmtstr, p1, p2, p3, p4) do { do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4); _MARK_EXIT(); } while(0); return val;} while(0)
    #define TSLOG5_RET(val, fmtstr, p1, p2, p3, p4, p5) do { do { _DOINCHK(); _CHK_MULTIOUT(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0); return val;} while(0)

    /** 
     * General log lines 
     * 
     * @param level     This can be used as internal information 
     *                  field, such as info, error, warning etc.
     * @param string    Custom string
     * @param p1 - p5   For multiple variables in same string.
     */
    #define TSLOG0(level, string) do { _DOINCHK(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); } while(0)
    #define TSLOG1(level, string, p1) do { _DOINCHK(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); } while(0)
    #define TSLOG2(level, string, p1, p2) do { _DOINCHK(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); } while(0)
    #define TSLOG3(level, string, p1, p2, p3) do { _DOINCHK(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); } while(0)
    #define TSLOG4(level, string, p1, p2, p3, p4) do { _DOINCHK(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); } while(0)
    #define TSLOG5(level, string, p1, p2, p3, p4, p5) do { _DOINCHK(); _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); } while(0)

    /** Error logging */
    #define __TSLOG_ASSERT_DBG( _assertion ) do { if( _assertion ) { break; } TFileName file; file.Copy( _L8( __FILE__ ) );   _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s Assert:%S:%d:" L ## #_assertion) , _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, &file, __LINE__ ); User::Invariant(); } while( 0 )
    #define __TSLOG_TRACE_ASSERT_DBG(_assertion, _textToPrint, _panicCode)  do { if (_assertion) { break; } _TSLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s ASSERTION FAILED!!! %s file: %s, line: %s"), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, _textToPrint, __FILE__, __LINE__); User::Panic(_L("AssertionFailed"), _panicCode} while(0)
    #define TSLOG_TRAPHANDLER() _TTsLogTrapHandler _traceTrapHandler; _traceTrapHandler.oldHandler = User::SetTrapHandler(&_traceTrapHandler)

/*****************************************************************************
 LOGGING MACROS - NO LOGGING
*****************************************************************************/ 
#else // _DEBUG

    #define TSLOG_CONTEXT(_fn, _vis )
    #define TSLOG_STATIC_CONTEXT(_fn, _vis, _thdId)

    #define TSLOG_IN()
    #define TSLOG0_IN(string)
    #define TSLOG1_IN(string, p1)
    #define TSLOG2_IN(string, p1, p2)
    #define TSLOG3_IN(string, p1, p2, p3)
    #define TSLOG4_IN(string, p1, p2, p3, p4)
    #define TSLOG5_IN(string, p1, p2, p3, p4, p5)

    #define TSLOG_OUT()
    #define TSLOG0_OUT(string)
    #define TSLOG1_OUT(string, p1)
    #define TSLOG2_OUT(string, p1, p2)
    #define TSLOG3_OUT(string, p1, p2, p3)
    #define TSLOG4_OUT(string, p1, p2, p3, p4)
    #define TSLOG5_OUT(string, p1, p2, p3, p4, p5)

    #define TSLOG0_RET(val, fmtstr) return val
    #define TSLOG1_RET(val, fmtstr, p1) return val
    #define TSLOG2_RET(val, fmtstr, p1, p2) return val
    #define TSLOG3_RET(val, fmtstr, p1, p2, p3) return val
    #define TSLOG4_RET(val, fmtstr, p1, p2, p3, p4) return val
    #define TSLOG5_RET(val, fmtstr, p1, p2, p3, p4, p5) return val

    #define TSLOG0(level, string)
    #define TSLOG1(level, string, p1)
    #define TSLOG2(level, string, p1, p2)
    #define TSLOG3(level, string, p1, p2, p3)
    #define TSLOG4(level, string, p1, p2, p3, p4)
    #define TSLOG5(level, string, p1, p2, p3, p4, p5)
    
    #define BIND_TRACE_TRAPHANDLER()
    #define TRACE_DECL() TInt _iTraceThreadId
    #define TRACE_FAST_CREATE(_thdId) _thdId++;
    #define TRACE_CREATE()

    #define __TSLOG_ASSERT_DBG(_assertion)
    #define __TRACE_ASSERT_DBG(_assertion, _message, _panicCode )

#endif // _DEBUG

#endif      // TSLOGLOGUTILS_H
            
// End of File
