/*
* ==============================================================================
*  Name        : hglogutils.h
*  Part of     : Teleport / Teleport UI
*  Description : Logging behavior
*  Version     : %version: 3 %
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

#ifndef HGLOGUTILS_H
#define HGLOGUTILS_H

/**
 * NOTE: This file contains the logic related to logging. Change only if you
 * know what you're doing.
 */










#ifdef _DEBUG

#include <e32std.h>
#include <e32debug.h>

static const TUint HGLOGERR   = 2;  /**< Used to create an error message */

// These macros are real macros, that should be used. For temporary purposes, these
// are left out and the logging is done by simple entry logging
#define _HGLOG_UNCONTROLLED_INPUT_MSG  L"%s%d[%x:%x:%x]>%s UNCONTROLLED ENTRY!"
#define _HGLOG_MULTIPLE_ENTRY_MSG      L"%s%d[%x:%x:%x]%s  ADDITIONAL ENTRY!"
#define _HGLOG_UNCONTROLLER_EXIT_MSG   L"%s%d[%x:%x:%x]<%s UNCONTROLLED EXIT!"
#define _HGLOG_MULTIPLE_EXIT_MSG       L"%s%d[%x:%x:%x]%s  ADDITIONAL EXIT!"
#define _HGLOG_TRAP_HARNESS_ENTRY      L"_D%d[%x:%x:%x]TraceFramework: Entering trap harness"
#define _HGLOG_TRAP_HARNESS_EXIT       L"_D%d[%x:%x:%x]TraceFramework: Exiting trap harness"
#define _HGLOG_TRAP_HARNESS_LEAVE      L"_D%d[%x:%x:%x]TraceFramework: ---------- LEAVE OCCURRED !!! ---------- "
#define _HGLOG_API_PREFIX              L"_A"
#define _HGLOG_LOCAL_PREFIX            L"_L"

/**
* @file 
* trace_utils.h contains definitions needed for advanced tracing features.
* Tracing can be customized using the following compile time flags:
* - <b>_DEBUG</b>
*   - With this flag undefined, all traces are disabled
* - <b>__KERNEL_MODE__</b>
*   - if kernel mode flag is defined, kernel macro variants are used (no unicode or shared heap related stuff, faster)
* - <b>DISABLE_SYNTAX_CHECK</b>
*   - If this flag is defined, runtime syntax checking features are disabled from traces
*/
#ifndef DISABLE_SYNTAX_CHECK
    #define _MARK_ENTRY() _dc.inOk=ETrue
    #define _DOINCHK() _dc.DoInChk()
    #define _CHK_MULTIIN() _dc.ChkMultiIn()
    #define _CHK_MULTIOUT() _dc.ChkMultiOut()
    #define _MARK_EXIT() _dc.outOk=ETrue
#else
    #define _MARK_ENTRY()
    #define _DOINCHK()
    #define _CHK_MULTIIN()
    #define _CHK_MULTIOUT()
    #define _MARK_EXIT()
#endif // DISABLE_SYNTAX_CHECK
    
/** For tracing */
#ifdef _HGLOG_RDEBUG
    #define _IT(a) (TPtrC((const TText *)(a)))
    #define _HGLOGPRINTER RDebug::Print        
        
/** For filedebug */
#else   // _HGLOG_RDEBUG

    /** Includes */
    #include <e32base.h>
    #include <e32std.h>
    #include <e32def.h>
    #include <e32svr.h>
    #include <flogger.h>
    
    #define _IT(a) KHgLogPath, KHgLogFile, EFileLoggingModeAppend, (TPtrC((const TText *)(a)))    
    #define _HGLOGPRINTER RFileLogger::WriteFormat    
#endif // _HGLOG_RDEBUG    

class _THgLogContext
    {
    public:
        _THgLogContext(
            const TText* _fn, 
            const TUint _id, 
            const TUint _vis, 
            const TUint _addr, 
            const TUint _thdId, 
            const TUint _category=0 )
            : 
            iFn(_fn), 
            iId(_id), 
            iApi((TBool)_vis), 
            iAddr(_addr), 
            iThdId(_thdId), 
            iVis((_vis == 0 ? (TText*)_HGLOG_API_PREFIX : (TText*)_HGLOG_LOCAL_PREFIX)),
            iCategory(_category)            
        #ifndef DISABLE_SYNTAX_CHECK
            ,outOk(EFalse), inOk(EFalse)
        #endif
            {
            }
        ~_THgLogContext() 
            {
            #ifndef DISABLE_SYNTAX_CHECK
                do
                    {                   
                    DoInChk();
                    if (!outOk)
                        {
                        _HGLOGPRINTER(_IT(_HGLOG_UNCONTROLLER_EXIT_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);
                        }
                    } while (0);
            #endif // DISABLE_SYNTAX_CHECK
            }

        const TText* iFn;
        const TUint iId;
        const TText* iVis;
        const TUint iAddr;
        const TInt iThdId;
        const TBool iApi;
        const TUint iCategory;
        
        #ifndef DISABLE_SYNTAX_CHECK
            inline void DoInChk()
                {
                if (!inOk)
                    {
                    _HGLOGPRINTER(_IT(_HGLOG_UNCONTROLLED_INPUT_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);
                    inOk = ETrue;
                    }
                }
                        
            inline void ChkMultiIn()
                {
                if (inOk)
                    {
                    _HGLOGPRINTER(_IT(_HGLOG_MULTIPLE_ENTRY_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);                
                    }
                }
    
            inline void ChkMultiOut()
                {
                if (outOk)
                    {           
                    _HGLOGPRINTER(_IT(_HGLOG_MULTIPLE_EXIT_MSG), iVis, iCategory, iId, iThdId, iAddr, iFn);                                        
                    }
                }

            TBool inOk;
            TBool outOk;
        #endif // DISABLE_SYNTAX_CHECK
    };
    
class _THgLogTrapHandler: public TTrapHandler
    {
    public:

        _THgLogTrapHandler(_THgLogContext& _context) : _dc( _context ) 
            {
            RThread me;
            iThdId = (TInt)me.Id();
            }
    	void Trap() 
    	    {
    	    _HGLOGPRINTER(_IT(_HGLOG_TRAP_HARNESS_ENTRY), 0, HGLOGERR, iThdId, this);
    	    oldHandler->Trap();
    	    } 
    	void UnTrap()
    	    {
    	    _HGLOGPRINTER(_IT(_HGLOG_TRAP_HARNESS_EXIT), 0, HGLOGERR, iThdId, this);
    	    oldHandler->UnTrap();
    	    }	
    	void Leave(TInt aValue)
    	    {
    	    _HGLOGPRINTER(_IT(_HGLOG_TRAP_HARNESS_LEAVE), 0, HGLOGERR, iThdId, this);
    	    oldHandler->Leave(aValue);
    	    }	
    	TTrapHandler* oldHandler;
    private:
        _THgLogContext& _dc;
        TInt iThdId;
    };
    
#endif // _DEBUG    

#endif // HGLOGUTILS_H