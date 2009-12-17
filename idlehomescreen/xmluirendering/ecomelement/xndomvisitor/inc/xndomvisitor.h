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
* Description:  Visitor class to modify DOM node element's data 
*
*/



#ifndef XN_DOM_VISITOR_H
#define XN_DOM_VISITOR_H

#include <e32base.h>
#include "mxndomvisitor.h"

// CLASS FORWARD


// CLASS DECLARATION

/**
*  Visitor class modifies DOM node element's data.
*
*  @lib xndomvisitor.dll
*  @since Series 60 3.1
*/
class CXnDomVisitor: public CBase, public MXnDomVisitor
    {
    public:
        /**
        * NewL construction
        */
        static MXnDomVisitor* NewL();
    
    public: // New functions
        
        /**
        * Documented in MXnDomVisitor::IsVersion
        */
        TBool IsVersion( const TDesC8& aVersion );
        
        /**
        * Documented in MXnDomVisitor::ModifyDataL
        */
        void ModifyDataL( CXnDomNode& aNode );
        
        /**
        * Documented in MXnDomVisitor::SetElementTypeL
        */
        void SetElementTypeL( CXnDomNode& aNode );
        
        /**
        * Documented in MXnDomVisitor::SetDtorKey
        */
        void SetDtorKey( const TUid aDtorKey );
        
        /**
        * Documented in MXnDomVisitor::Release
        */
        void Release();
    
    private:
        /**
        * Destructor
        */
        ~CXnDomVisitor();
        
        /**
        * C++ default constructor
        */
        CXnDomVisitor();
        
    private:
        //ECom destructor key
        TUid iDtorKey;
    };

#endif      // XN_DOM_VISITOR_H  
            
// End of File
