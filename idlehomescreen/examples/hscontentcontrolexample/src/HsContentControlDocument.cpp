/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Control example
*
*/


// [[[ begin generated region: do not modify [Generated User Includes]
#include "HsContentControlDocument.h"
#include "HsContentControlAppUi.h"
// ]]] end generated region [Generated User Includes]

/**
 * @brief Constructs the document class for the application.
 * @param anApplication the application instance
 */
CHsContentControlDocument::CHsContentControlDocument( CEikApplication& anApplication )
    : CAknDocument( anApplication )
    {
    }

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here.  
 */ 
void CHsContentControlDocument::ConstructL()
    {
    }
    
/**
 * Symbian OS two-phase constructor.
 *
 * Creates an instance of CHsContentControlDocument, constructs it, and
 * returns it.
 *
 * @param aApp the application instance
 * @return the new CHsContentControlDocument
 */
CHsContentControlDocument* CHsContentControlDocument::NewL( CEikApplication& aApp )
    {
    CHsContentControlDocument* self = new ( ELeave ) CHsContentControlDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

/**
 * @brief Creates the application UI object for this document.
 * @return the new instance
 */    
CEikAppUi* CHsContentControlDocument::CreateAppUiL()
    {
    return new ( ELeave ) CHsContentControlAppUi;
    }
                
