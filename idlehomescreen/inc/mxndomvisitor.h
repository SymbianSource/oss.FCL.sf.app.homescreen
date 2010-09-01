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
* Description:  Visitor class interface 
*
*/



#ifndef MXN_DOM_VISITOR_H
#define MXN_DOM_VISITOR_H

// CLASS FORWARD
class CXnDomNode;

// CLASS DECLARATION

/**
*  Visitor class modifies DOM node element's data.
*
*  @lib xndomvisitor.dll
*  @since Series 60 3.1
*/
class MXnDomVisitor
    {
    public: // New functions
        
        /**
        * Check in-memory plugin version.
        * @return ETrue if version match
        */
        virtual TBool IsVersion( const TDesC8& aVersion )=0;
        
        /**
        * Modifies data
        * @param aData Data to modify
        */
        virtual void ModifyDataL( CXnDomNode& aNode )=0;
        
        /**
        * Sets element content type (TContentType) from XnDomNode.h
        * @param aNode Node to modify
        */
        virtual void SetElementTypeL( CXnDomNode& aNode )=0;
        
        /**
        * Ecom passes a destructor key at the time when plugin is created
        * and requires that key when plugin is destroyed.
        * @param aKey ECom required destructor key
        */ 
        virtual void SetDtorKey( const TUid aDtorKey )=0;
        
        /**
        * Release resources
        */
        virtual void Release()=0;
    
    protected:
        /**
        * Plugin can only be destroyed by calling Release().
        */
        ~MXnDomVisitor(){};

    };

#endif      // MXN_DOM_VISITOR_H  
            
// End of File
