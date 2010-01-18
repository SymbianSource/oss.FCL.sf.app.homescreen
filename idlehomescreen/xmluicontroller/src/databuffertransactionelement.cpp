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
* Description:  Data buffer transaction element
*
*/


#include    "databuffertransactionelement.h"
#include    "aixmluiutils.h"

#include    "xntype.h"
#include    "xntext.h"
#include    "xnnewsticker.h"
#include    "xnmenuadapter.h"
#include    "mxncomponentinterface.h"
#include    "xnvolumecontrol.h"
#include    "xnnodeappif.h"
#include    "xnuiengineappif.h"
#include    "xnproperty.h"
#include    "xndompropertyvalue.h"
#include    "aistrcnv.h"
#include    "aixmluiconstants.h"

// ============================ MEMBER FUNCTIONS ===============================

using namespace AiXmlUiController;
using namespace XnTextInterface;

CDataBufferTransactionElement::CDataBufferTransactionElement(AiUtility::CContentPriorityMap& aContentPriorityMap)
    : CTransactionElement(aContentPriorityMap)
    {
    }    

CDataBufferTransactionElement* CDataBufferTransactionElement::NewL(AiUtility::CContentPriorityMap& aContentPriorityMap)
    {
    CDataBufferTransactionElement* self = new( ELeave ) CDataBufferTransactionElement(aContentPriorityMap);
    
    return self;
    }

CDataBufferTransactionElement::~CDataBufferTransactionElement()
    {
    }

void CDataBufferTransactionElement::InitializeL( CXnNodeAppIf& aTarget, const TDesC8& aData )
    {
    CheckTypeL( aTarget );
    SetTarget( aTarget );
    iNewData.Set( aData );
    }

void CDataBufferTransactionElement::UpdateDataL()
    {
    // Set new text
    SetDataL();
    
    // Update content priority
    UpdateContentPriorityL();
    }
    
void CDataBufferTransactionElement::Reset()
    {
    CTransactionElement::Reset();
    
    iNewData.Set( KNullDesC8);
    }

TBool CDataBufferTransactionElement::IsSupported( CXnNodeAppIf& aTarget,
                                                  const TDesC8& aContentType )
    {
    // Get target type info
    CXnType* typeInfo = aTarget.Type();
    
    if ( !typeInfo )
        {
        return EFalse;
        }
    
    const TDesC8& type = typeInfo->Type();
    
    if ( aContentType == KContentTypeText ) // Text
        {
        // Text element and menu item supports
        return ( type == XnTextInterface::MXnTextInterface::Type() ||
                 type == KXnMenuItem || 
                 type == KXnMenu ||
                 type == XnPropertyNames::softkey::KNodeName ||
                 type == XnPropertyNames::volumecontrol::KSlider );
        }
    else if ( aContentType == KContentTypeImageSvg ) // SVG image
        {
        // Newsticker support
        return ( type == XnNewstickerInterface::MXnNewstickerInterface::Type() );
        }
        
    return EFalse;
    }

void CDataBufferTransactionElement::CheckTypeL( CXnNodeAppIf& aTarget )
    {
    // Get type info
    const TDesC8& type = LeaveIfNull( aTarget.Type(), KErrNotSupported )->Type();
    
    // Text element, menu item, and newsticker supported
    if ( type != XnTextInterface::MXnTextInterface::Type() &&
         type != KXnMenuItem &&
         type != KXnMenu &&
         type != XnPropertyNames::softkey::KNodeName && 
         type != XnPropertyNames::volumecontrol::KSlider &&
         type != XnNewstickerInterface::MXnNewstickerInterface::Type() )
        {
        User::Leave( KErrNotSupported );
        }
    }

void CDataBufferTransactionElement::SetDataL()
    {
    // Get type info
    const TDesC8& type = LeaveIfNull( Target().Type(), KErrNotSupported )->Type();
    
    if ( type == XnTextInterface::MXnTextInterface::Type() ) // Text element
        {
        // Set character data directly to target
        Target().SetPCDataL( iNewData );
        }
    else if (  type == XnPropertyNames::softkey::KNodeName ||
            type == KXnMenuItem || 
            type == KXnMenu ) // menu item
        {
        // Update label attribute value.
        
        // 
        CXnNodeAppIf& target = Target();
        
        // Create new new label attribute value
        CXnDomPropertyValue* propertyValue =
            CXnDomPropertyValue::NewL( &target.UiEngineL()->StringPool() );
    
        CleanupStack::PushL( propertyValue );
    
        propertyValue->SetStringValueL( CXnDomPropertyValue::EString,
                                        iNewData );
    
        CXnProperty* newProperty = CXnProperty::NewL( XnPropertyNames::menu::KLabel,
                                                      propertyValue,
                                                      target.UiEngineL()->StringPool() );
        CleanupStack::Pop( propertyValue );
        
        CleanupStack::PushL( newProperty );

        // Set label attribute
        target.SetPropertyL( newProperty );
        
        CleanupStack::Pop( newProperty );
        
        // Refresh menu
        target.UiEngineL()->RefreshMenuL();
        }
    else if ( type == XnPropertyNames::volumecontrol::KSlider ) // slider
        {
        // Get volume control interface
        XnVolumeControlInterface::MXnVolumeControlInterface* volumeControl = NULL;
        XnComponentInterface::MakeInterfaceL( volumeControl, Target() );
        LeaveIfNull( volumeControl, KErrNotSupported );
        
        TInt32 volume = 0;
        User::LeaveIfError( AiUtility::ParseInt( volume, iNewData ) );
        volumeControl->SetValue( volume );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }
    
//  End of File
