/*
* ==============================================================================
*  Name        : hglogging.h
*  Part of     : Teleport / Context Services
*  Description : Defines all available logging macros.
*  Version     : %version: 2 %
*
*  Copyright © 2008 Nokia. All rights reserved.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia. All rights are reserved. Copying, including 
*  reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia.
* ==============================================================================
*/

#ifndef HGLOGLOGUTILS_H
#define HGLOGLOGUTILS_H

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
 * _LIT( KHgLogFile, "text_file_for_logging.txt" );
 * _LIT( KHgLogPath, "folder_for_text_file" );
 * #define _HGLOG_LOG_COMPONENT_ID <some unique number here>
 * 
 * #include "hglogging.h"
 * 
 * @/code
 * 
 * KHgLogFile   : This is the name of the file, where all the logs for
 *                this components are being written.
 * 
 * KHgLogPath   : This is the folder name under c:\logs, where the file
 *                is to be stored. For example, if KHgLogPath is "test",
 *                log file is created into folder c:\logs\test.
 * 
 * _HGLOG_LOG_COMPONENT_ID  : Unique number id of the component. This is 
 *                            for filtering purposes.
 * 
 * _HGLOG_RDEBUG    : When defined tracing instead of file logging. 
 *                    Default is for file logging.
 * 
 * --------------------------------------------------------------------------
 * 
 * USING LOGGING:
 * 
 * Basically the use is simple, register function use with HGLOG_CONTEXT, 
 * then log function enter by any HGLOG_IN -macro, then possibly use HGLOG
 * -macros for function logging and finally HGLOG_OUT -macros for returning
 * from the function.
 * 
 * @code
 * TInt CGood::Example( TInt aSomething )
 *     {
 *     // Create log context class, which is maintained for lifetime of the 
 *     // method. 
 *     HGLOG_CONTEXT( Example, HGLOG_LOCAL );
 * 
 *     // Indicate we are entering the function. 
 *     HGLOG_IN1( "aSomething contains value %d", aSomething );
 * 
 *     // Your buggy code...
 * 
 *     // Before leaving, indicate function execution has ended.
 *     HGLOG_OUT();
 * 
 *     return 0;
 *     }
 * @/code
 */

#include <e32def.h>
#include <e32std.h>

#include "hglogutils.h"

#define HGLOG_API 0
#define HGLOG_LOCAL 1

#define HGLOG_INFO 0
#define HGLOG_WARNING 1
#define HGLOG_ERROR 2

#define HGLOG_ASSERT(_assertion)  __HGLOG_ASSERT_DBG(_assertion) 
#define HGLOG_TRACE_ASSERT(_assertion) __ASSERT_DEBUG((_assertion), User::Invariant() )

#ifdef _DEBUG
            
/*****************************************************************************
 LOGGING MACROS - LOGGING ON
*****************************************************************************/             

    /** 
     * Context initialization 
     * NOTE: HGLOG_STATIC_CONTEXT is meant for static methods.
     * 
     * @param _fn   Name of the function.
     * @param _vis  Visibility for the client, use values HGLOG_API or HGLOG_LOCAL
     * @param _thdId For static functions, thread id can be given here.
     */
    #define HGLOG_CONTEXT(_fn, _vis ) _THgLogContext _dc((TText*)L ## #_fn, _HGLOG_LOG_COMPONENT_ID, _vis, (TUint)this, RProcess().SecureId().iId )
    #define HGLOG_STATIC_CONTEXT(_fn, _vis, _thdId) _THgLogContext _dc((TText*)L ## #_fn, _HGLOG_LOG_COMPONENT_ID, _vis, _thdId, RProcess().SecureId().iId )

    /** 
     * Entering function 
     * 
     * @param string    Custom text. Example: HGLOG_IN0( "Yeah!!!" );
     * @param p1 - p5   For multiple variables in same string.
     */
    #define HGLOG_IN() do { _CHK_MULTIIN(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
    #define HGLOG0_IN(string) do { _CHK_MULTIIN(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_ENTRY(); } while(0)
    #define HGLOG1_IN(string, p1) do { _CHK_MULTIIN(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); _MARK_ENTRY(); } while(0)
    #define HGLOG2_IN(string, p1, p2) do { _CHK_MULTIIN(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_ENTRY(); } while(0)
    #define HGLOG3_IN(string, p1, p2, p3) do { _CHK_MULTIIN(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_ENTRY(); } while(0)
    #define HGLOG4_IN(string, p1, p2, p3, p4) do { _CHK_MULTIIN(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_ENTRY(); } while(0)
    #define HGLOG5_IN(string, p1, p2, p3, p4, p5) do { _CHK_MULTIIN(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]>%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_ENTRY(); } while(0)

    /** Leaving function */
    #define HGLOG_OUT() do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s "), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
    #define HGLOG0_OUT(string) do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); _MARK_EXIT(); } while(0)
    #define HGLOG1_OUT(string, p1) do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); _MARK_EXIT(); } while(0)
    #define HGLOG2_OUT(string, p1, p2) do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); _MARK_EXIT(); } while(0)
    #define HGLOG3_OUT(string, p1, p2, p3) do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); _MARK_EXIT(); } while(0)
    #define HGLOG4_OUT(string, p1, p2, p3, p4) do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); _MARK_EXIT(); } while(0)
    #define HGLOG5_OUT(string, p1, p2, p3, p4, p5) do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## string), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0)

    /** Leaving function with return value */
    #define HGLOG0_RET(val, fmtstr) do { do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val); _MARK_EXIT(); } while(0); return val;} while(0)
    #define HGLOG1_RET(val, fmtstr, p1) do { do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1); _MARK_EXIT(); } while(0); return val;} while(0)
    #define HGLOG2_RET(val, fmtstr, p1, p2) do { do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2); _MARK_EXIT(); } while(0); return val;} while(0)
    #define HGLOG3_RET(val, fmtstr, p1, p2, p3) do { do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3); _MARK_EXIT(); } while(0); return val;} while(0)
    #define HGLOG4_RET(val, fmtstr, p1, p2, p3, p4) do { do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4); _MARK_EXIT(); } while(0); return val;} while(0)
    #define HGLOG5_RET(val, fmtstr, p1, p2, p3, p4, p5) do { do { _DOINCHK(); _CHK_MULTIOUT(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s " L ## fmtstr), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, val, p1, p2, p3, p4, p5); _MARK_EXIT(); } while(0); return val;} while(0)

    /** 
     * General log lines 
     * 
     * @param level     This can be used as internal information 
     *                  field, such as info, error, warning etc.
     * @param string    Custom string
     * @param p1 - p5   For multiple variables in same string.
     */
    #define HGLOG0(level, string) do { _DOINCHK(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn); } while(0)
    #define HGLOG1(level, string, p1) do { _DOINCHK(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1); } while(0)
    #define HGLOG2(level, string, p1, p2) do { _DOINCHK(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2); } while(0)
    #define HGLOG3(level, string, p1, p2, p3) do { _DOINCHK(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3); } while(0)
    #define HGLOG4(level, string, p1, p2, p3, p4) do { _DOINCHK(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4); } while(0)
    #define HGLOG5(level, string, p1, p2, p3, p4, p5) do { _DOINCHK(); _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]%s " L ## string), _dc.iVis, level, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, p1, p2, p3, p4, p5); } while(0)

    /** Error logging */
    #define __HGLOG_ASSERT_DBG( _assertion ) do { if( _assertion ) { break; } TFileName file; file.Copy( _L8( __FILE__ ) );   _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s Assert:%S:%d:" L ## #_assertion) , _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, &file, __LINE__ ); User::Invariant(); } while( 0 )
    #define __HGLOG_TRACE_ASSERT_DBG(_assertion, _textToPrint, _panicCode)  do { if (_assertion) { break; } _HGLOGPRINTER(_IT(L ## "%s%d[%x:%x:%x]<%s ASSERTION FAILED!!! %s file: %s, line: %s"), _dc.iVis, _dc.iCategory, _dc.iId, _dc.iThdId, _dc.iAddr, _dc.iFn, _textToPrint, __FILE__, __LINE__); User::Panic(_L("AssertionFailed"), _panicCode} while(0)
    #define HGLOG_TRAPHANDLER() _THgLogTrapHandler _traceTrapHandler; _traceTrapHandler.oldHandler = User::SetTrapHandler(&_traceTrapHandler)

/*****************************************************************************
 LOGGING MACROS - NO LOGGING
*****************************************************************************/ 
#else // _DEBUG

    #define HGLOG_CONTEXT(_fn, _vis )
    #define HGLOG_STATIC_CONTEXT(_fn, _vis, _thdId)

    #define HGLOG_IN()
    #define HGLOG0_IN(string)
    #define HGLOG1_IN(string, p1)
    #define HGLOG2_IN(string, p1, p2)
    #define HGLOG3_IN(string, p1, p2, p3)
    #define HGLOG4_IN(string, p1, p2, p3, p4)
    #define HGLOG5_IN(string, p1, p2, p3, p4, p5)

    #define HGLOG_OUT()
    #define HGLOG0_OUT(string)
    #define HGLOG1_OUT(string, p1)
    #define HGLOG2_OUT(string, p1, p2)
    #define HGLOG3_OUT(string, p1, p2, p3)
    #define HGLOG4_OUT(string, p1, p2, p3, p4)
    #define HGLOG5_OUT(string, p1, p2, p3, p4, p5)

    #define HGLOG0_RET(val, fmtstr) return val
    #define HGLOG1_RET(val, fmtstr, p1) return val
    #define HGLOG2_RET(val, fmtstr, p1, p2) return val
    #define HGLOG3_RET(val, fmtstr, p1, p2, p3) return val
    #define HGLOG4_RET(val, fmtstr, p1, p2, p3, p4) return val
    #define HGLOG5_RET(val, fmtstr, p1, p2, p3, p4, p5) return val

    #define HGLOG0(level, string)
    #define HGLOG1(level, string, p1)
    #define HGLOG2(level, string, p1, p2)
    #define HGLOG3(level, string, p1, p2, p3)
    #define HGLOG4(level, string, p1, p2, p3, p4)
    #define HGLOG5(level, string, p1, p2, p3, p4, p5)
    
    #define BIND_TRACE_TRAPHANDLER()
    #define TRACE_DECL() TInt _iTraceThreadId
    #define TRACE_FAST_CREATE(_thdId) _thdId++;
    #define TRACE_CREATE()

    #define __HGLOG_ASSERT_DBG(_assertion)
    #define __TRACE_ASSERT_DBG(_assertion, _message, _panicCode )

#endif // _DEBUG

#endif      // HGLOGLOGUTILS_H
            
// End of File
