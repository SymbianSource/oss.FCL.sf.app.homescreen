/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef CXN3TIMEMON_H
#define CXN3TIMEMON_H

//  INCLUDES
#include <e32debug.h>
#include <e32base.h>
#include <e32std.h>
#include <hal.h>
#include <e32cmn.h>

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CXn3TimeMon : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CXn3TimeMon * NewL();
        
        /**
        * Destructor.
        */
        virtual ~CXn3TimeMon ();

    public: // New functions
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        inline static TTime StartTiming( const TDesC& aMsg );

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        inline static void StopTiming( TTime aStartTime, const TDesC& aMsg );
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        inline static void PrintUserMem(const TDesC& aMsg);

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CXn3TimeMon ();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

inline TTime CXn3TimeMon::StartTiming(const TDesC& aMsg )
    {
    TTime startTime;
    startTime.HomeTime(); 
    RDebug::Print( aMsg );
    return startTime;
    }

inline void CXn3TimeMon::StopTiming( TTime aStartTime, const TDesC& aMsg )
    {
    // calculating service time
    TTime readyTime;
    readyTime.HomeTime();
    TTimeIntervalMicroSeconds delay = readyTime.MicroSecondsFrom( aStartTime );
    TTime transferTime(delay.Int64());
    TBuf<64> timeString;
    transferTime.FormatL(timeString,_L(" Time: %S%C microseconds"));
    TBuf<256> tmp;
    tmp.Append( aMsg );
    tmp.Append( timeString );
    RDebug::Print( tmp ); 
    }

inline void CXn3TimeMon::PrintUserMem(const TDesC& aMsg)
    {
    TBuf<512> buffer;
    TInt    freeRAM;
    HAL::Get( HALData::EMemoryRAMFree, freeRAM );
    RHeap heap = User::Heap();
    heap.Open();
    TInt _size = heap.Size();
    TInt largest = 0;
    TInt available = heap.Available( largest );
    heap.Close();
    _LIT( KMemoryFormat, "FreeRAM: %d kB, User: - heap %d kB, available %d kB, largest block %d kB" );
    buffer.Format( KMemoryFormat, ( freeRAM / 1024), (_size / 1024),
                                    (available / 1024), (largest / 1024 ) );

    RDebug::Print( _L("Xuikon:RAM:%S: %S"), &aMsg, &buffer ); 
    }

#endif      // CXN3TIMEMON_H
            
// End of File
