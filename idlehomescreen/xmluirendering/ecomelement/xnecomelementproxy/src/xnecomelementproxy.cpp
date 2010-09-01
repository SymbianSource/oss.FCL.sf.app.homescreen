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
* Description:  Entry point for accessing xmluiml language specific element plugins.
*
*/



// INCLUDE FILES
#include    "xnecomelementproxy.h"
#include    "utf.h"
#include    "xndomvisitor.rh"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TXnEComElementProxy::CreateImplementationL
// -----------------------------------------------------------------------------
//
EXPORT_C MXnDomVisitor * TXnEComElementProxy::CreateImplementationL( 
    const TDesC8& aNS )
	{
	TUid dtor={0};
    TUid interfaceUid = {KXnDomVisitorIId};
	// Let's find namespace e.g http://www.series60.com/xml/xmluiml/1
    TEComResolverParams resolverParams;
	resolverParams.SetDataType( aNS );
	resolverParams.SetWildcardMatch( ETrue );
	
	TAny * ecom = NULL;
#ifdef _DEBUG
	TRAPD(err, ecom= REComSession::CreateImplementationL( interfaceUid, dtor,
	                NULL, resolverParams));
	if( err != KErrNone )
	    {
	    REComSession::FinalClose();
	    User::Leave( err );
	    }
#else
    ecom= REComSession::CreateImplementationL( interfaceUid, dtor,
	                NULL, resolverParams);
#endif
    MXnDomVisitor* element = element = reinterpret_cast<MXnDomVisitor*>(ecom);	    
	element->SetDtorKey( dtor );
    
    return element;
	}
//  End of File  
