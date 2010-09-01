/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Image transaction element
*
*/


#include "xnbitmap.h"
#include "xntype.h"

#include <gulicon.h>

#include "imagetransactionelement.h"
#include "xmluicontrollerpanic.h"
#include "aixmluiutils.h"

using namespace AiXmlUiController;
using namespace XnImageInterface;

namespace AiXmlUiController
{

// ======== MEMBER FUNCTIONS ========

CImageTransactionElement::CImageTransactionElement(
    AiUtility::CContentPriorityMap& aContentPriorityMap)
    : CTransactionElement(aContentPriorityMap)
    {
    }

CImageTransactionElement* CImageTransactionElement::NewL(
    AiUtility::CContentPriorityMap& aContentPriorityMap)
    {
    CImageTransactionElement* self = 
        new( ELeave ) CImageTransactionElement( aContentPriorityMap );
                                                            
    return self;
    }

CImageTransactionElement::~CImageTransactionElement()
    {
    delete iIcon;    
    }

void CImageTransactionElement::InitializeL( CXnNodeAppIf& aTarget,
    CGulIcon* aIcon )
    {
    LeaveIfNull( aIcon, KErrArgument );
    
    CheckTypeL( aTarget );
    SetTarget( aTarget );

    iIcon = aIcon;
    iFilename = KNullDesC();
    }

void CImageTransactionElement::InitializeL( CXnNodeAppIf& aTarget,
    RFile& aFile )
    {    
    CheckTypeL( aTarget );   
    SetTarget( aTarget );

    iFilename = KNullDesC();
    
    aFile.FullName( iFilename );
    
    delete iIcon;
    iIcon = NULL;    
    }

void CImageTransactionElement::UpdateDataL()
    {           
    const TDesC8& type( Target().Type()->Type() );
    
    if ( type == XnImageInterface::MXnImageInterface::Type() )
        {
        MXnImageInterface* image( NULL );
        XnComponentInterface::MakeInterfaceL( image, Target() );
        
        LeaveIfNull( image, KErrNotSupported );

        if ( iIcon )
            {
            // Set new bitmaps. Ownership is transferred to MXnImageInterface
            iIcon->SetBitmapsOwnedExternally( ETrue );
            
            image->SetContentBitmaps( iIcon->Bitmap(), iIcon->Mask() );
            }
        else if ( iFilename != KNullDesC() )
            {
            image->SetContentBitmaps( iFilename );
            }        
        }
    
    UpdateContentPriorityL();
    }

void CImageTransactionElement::Reset()
    {
    CTransactionElement::Reset();

    iFilename = KNullDesC();    
    
    delete iIcon;
    iIcon = NULL;    
    }

TBool CImageTransactionElement::IsSupported( CXnNodeAppIf& aTarget )
    {
    // Get type info
    CXnType* typeInfo( aTarget.Type() );
    
    if ( !typeInfo )
        {
        return EFalse;
        }
    
    const TDesC8& type( typeInfo->Type() );
    
    // image element and newsticker supported
    return ( type == XnImageInterface::MXnImageInterface::Type() );
    }

void CImageTransactionElement::CheckTypeL( CXnNodeAppIf& aTarget )
    {
    if ( !IsSupported( aTarget ) )
        {
        User::Leave( KErrNotSupported );
        }
    }
} // ns

// End of file
