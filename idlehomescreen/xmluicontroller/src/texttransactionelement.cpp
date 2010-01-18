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
* Description:  Text transaction element
*
*/


#include    "texttransactionelement.h"
#include    "aixmluiutils.h"

#include    "xntype.h"
#include    "xnproperty.h"
#include    "xntext.h"
#include    "xnmenuadapter.h"
#include    "xnnewsticker.h"
//#include    <xnmarquee.h>
#include    "mxncomponentinterface.h"
#include    "xnnodeappif.h"
#include    "xnuiengineappif.h"
#include    "xnproperty.h"
#include    "xndompropertyvalue.h"
#include    "xnvolumecontrol.h"
#include    "aistrcnv.h"

// ============================ MEMBER FUNCTIONS ===============================

using namespace AiXmlUiController;
using namespace XnTextInterface;

CTextTransactionElement::CTextTransactionElement(AiUtility::CContentPriorityMap& aContentPriorityMap)
    : CTransactionElement(aContentPriorityMap)
    {
    }    

CTextTransactionElement* CTextTransactionElement::NewL(AiUtility::CContentPriorityMap& aContentPriorityMap)
    {
    CTextTransactionElement* self = new( ELeave ) CTextTransactionElement(aContentPriorityMap);
    
    return self;
    }

CTextTransactionElement::~CTextTransactionElement()
    {
    if (iNewText)
        {
        delete iNewText;
        iNewText = NULL;
        }
    }

void CTextTransactionElement::InitializeL( CXnNodeAppIf& aTarget, const TDesC& aText )
    {
    CheckTypeL( aTarget );
    SetTarget( aTarget );
    
    if (iNewText)
        {
        delete iNewText;
        iNewText = NULL;
        }
    iNewText = aText.AllocL();    
    }

void CTextTransactionElement::UpdateDataL()
    {
    // Set new text
    SetTextL();
    
    // Update content priority
    UpdateContentPriorityL();
    }
    
void CTextTransactionElement::Reset()
    {
    CTransactionElement::Reset();
    
    
    if (iNewText)   
        {
        delete iNewText;
        iNewText = NULL;
        }
    }

TBool CTextTransactionElement::IsSupported( CXnNodeAppIf& aTarget )
    {
    CXnType* typeInfo = aTarget.Type();
    
    if ( !typeInfo )
        {
        return EFalse;
        }
    
    const TDesC8& type = typeInfo->Type();
    
    // Text element, menu item, newsticker, and marquee supported
    return ( type == XnTextInterface::MXnTextInterface::Type() ||
             type == KXnMenuItem ||
             type == KXnMenu ||
             type == XnPropertyNames::softkey::KNodeName || 
             type == XnPropertyNames::volumecontrol::KSlider /*||
             type == XnNewstickerInterface::MXnNewstickerInterface::Type() ||
             type == XnMarqueeInterface::MXnMarqueeInterface::Type() */);
    }

void CTextTransactionElement::CheckTypeL( CXnNodeAppIf& aTarget )
    {
    if ( !IsSupported( aTarget ) )
        {
        User::Leave( KErrNotSupported );
        }
    }

void CTextTransactionElement::SetTextL()
    {
    // Get type info
    const TDesC8& type = LeaveIfNull( Target().Type(), KErrNotSupported )->Type();
    
    if ( type == XnTextInterface::MXnTextInterface::Type() ) // text element
        {
        // Get control interface
        XnTextInterface::MXnTextInterface* textControl = NULL;
        XnComponentInterface::MakeInterfaceL( textControl, Target() );
        LeaveIfNull( textControl, KErrNotSupported );
        
        // Set text
        
        textControl->SetTextL( *iNewText );
        }
       
 // Menu softkey texts
    else if ( type == XnPropertyNames::softkey::KNodeName)
        {
        CXnNodeAppIf& target = Target();
        // Set the label of the softkey
        CXnDomPropertyValue* propertyValue =
            CXnDomPropertyValue::NewL( &target.UiEngineL()->StringPool() );
    
        CleanupStack::PushL( propertyValue );
    
        // Convert unicode to UTF-8
        HBufC8* textBuf = NULL;
        
        textBuf = AiUtility::CopyToBufferL( textBuf, *iNewText );
        CleanupStack::PushL( textBuf );
        
        propertyValue->SetStringValueL( CXnDomPropertyValue::EString,
                                        *textBuf );
    
        CleanupStack::PopAndDestroy( textBuf );
        
        CXnProperty* newProperty = CXnProperty::NewL( XnPropertyNames::menu::KLabel,
                                                      propertyValue,
                                                      target.UiEngineL()->StringPool() );
        CleanupStack::Pop( propertyValue );
        
        CleanupStack::PushL( newProperty );

        target.SetPropertyL( newProperty );
        
        CleanupStack::Pop( newProperty );
        target.UiEngineL()->RefreshMenuL(); 
       
        }            
        
    else if ( type == XnPropertyNames::volumecontrol::KSlider ) // slider
        {
        // Get volume control interface
        XnVolumeControlInterface::MXnVolumeControlInterface* volumeControl = NULL;
        XnComponentInterface::MakeInterfaceL( volumeControl, Target() );
        LeaveIfNull( volumeControl, KErrNotSupported );
        
        TInt32 volume = 0;
        
        User::LeaveIfError( AiUtility::ParseInt( volume, *iNewText ) );
        volumeControl->SetValue( volume );
        }
/*    else if ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() )
        {
        // Get Newsticker control interface
        XnNewstickerInterface::MXnNewstickerInterface* newsTicker = NULL;
        XnComponentInterface::MakeInterfaceL( newsTicker, Target() );
        LeaveIfNull( newsTicker, KErrNotSupported );
        
        // Append new title
        newsTicker->ClearTitles();
        newsTicker->AppendTitleL( iNewText );
        }
    else if ( type == XnMarqueeInterface::MXnMarqueeInterface::Type() )
        {
        // Get marquee interface
        XnMarqueeInterface::MXnMarqueeInterface* marquee = NULL;
        XnComponentInterface::MakeInterfaceL( marquee, Target() );
        LeaveIfNull( marquee, KErrNotSupported );
        
        // Set new text
        marquee->SetTextL( iNewText );
        }*/
    else
        {
        User::Leave( KErrNotSupported );
        }
    }
    
//  End of File
