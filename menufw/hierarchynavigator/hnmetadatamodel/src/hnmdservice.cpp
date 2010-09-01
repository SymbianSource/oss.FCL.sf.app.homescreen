/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include <xmlengdom.h>
#include <liwservicehandler.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "hnmdservice.h"
#include "hnmdservicecommand.h"
#include "hnliwutils.h"
#include "hnglobals.h"
#include "hnmdbasekey.h"
#include "hnmdkeyfactory.h"
#include "hnservicehandler.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdService::ConstructL( TXmlEngElement aElement )
    {
    SetInterfaceL( aElement.AttributeNodeL( KInterfaceAttrName8 ) );
    SetServiceL( aElement.AttributeNodeL( KServiceAttrName8 ) );
    SetConstructorL( aElement );
    iServiceCommand = CHnMdServiceCommand::NewL( aElement );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdService* CHnMdService::NewL( TXmlEngElement aElement )
    {
    CHnMdService* self = CHnMdService::NewLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdService* CHnMdService::NewLC( TXmlEngElement aElement )
    {
    CHnMdService* self = new( ELeave ) CHnMdService();
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdService::CHnMdService()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdService::~CHnMdService()
    {
    iServiceName.Close();
    iInterfaceName.Close();
    delete iConstructor;
    delete iServiceCommand;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnMdService::GetServiceName() const
    {
    return iServiceName;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnMdService::GetInterfaceName() const
    {
    return iInterfaceName;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const CHnMdServiceCommand& CHnMdService::GetCommand() const
    {
    return *iServiceCommand;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdService::GetConstructor() const
    {
    return iConstructor;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdService::SetConstructorL( TXmlEngElement aElement )
    {
    iConstructor = HnMdKeyFactory::CreateL( KConstructorElementItem8(), KKeyTypeList(), 
            KNullDesC8() );
    RXmlEngNodeList< TXmlEngElement > elements;
    CleanupClosePushL( elements );
    aElement.GetChildElements( elements );

    TInt elementsCount = elements.Count();

    for (TInt i =0; i < elementsCount; i++ )
        {
        TXmlEngElement dataElement = elements.Next();

        if ( !dataElement.Name().Compare( KConstructorElementItem8 ) )
            {
            RXmlEngNodeList< TXmlEngElement > keys;
            CleanupClosePushL( keys );
            dataElement.GetChildElements( keys );

            for (TInt j = 0; j < keys.Count(); j++ )
                {
                TXmlEngElement keyElement = keys.Next();
                if ( !keyElement.Name().Compare( KKeyElementItem8 ) )
                    {
                    CHnMdBaseKey *key = HnMdKeyFactory::CreateL( keyElement );
                    CleanupStack::PushL( key );
                    iConstructor->AddSubKeyL( key );
                    CleanupStack::Pop( key );
                    }
                }
            CleanupStack::PopAndDestroy( &keys );
            }
        }
    CleanupStack::PopAndDestroy( &elements );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdService::SetInterfaceL( TXmlEngAttr aAttr )
    {
    iInterfaceName.Close();
    iInterfaceName.CreateL( aAttr.Value() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdService::SetServiceL( TXmlEngAttr aAttr )
    {
    iServiceName.Close();
    iServiceName.CreateL( aAttr.Value() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CLiwGenericParamList* CHnMdService::EvaluateConstructorL( 
        const CLiwGenericParamList& aList,
        TInt aPos)
    {
    CHnMdBaseKey* evalKeys = EvaluateConstructorAsKeyL( aList, aPos ); 
    CleanupStack::PushL( evalKeys );
    CLiwGenericParamList* ret = CLiwGenericParamList::NewL();
    CleanupStack::PushL( ret ); 
    if ( evalKeys )
        {
        evalKeys->ToGenericParamListL( *ret );
        }
    CleanupStack::Pop( ret );
    CleanupStack::PopAndDestroy( evalKeys );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdService::EvaluateConstructorAsKeyL( 
        const CLiwGenericParamList& aList,
        TInt aPos)
    {
    CHnMdBaseKey* consKeys = GetConstructor();
    CHnMdBaseKey* evalKeys = NULL;
    if (consKeys)
        {
        evalKeys =  consKeys->EvaluateLC( aList, aPos );
        CleanupStack::Pop( evalKeys );
        }
    return evalKeys;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CLiwGenericParamList* CHnMdService::EvaluateCommandL( 
        const CLiwGenericParamList& aList,
        TInt aPos
        )
    {
    CHnMdBaseKey* evalKeys = EvaluateCommandAsKeyL( aList, aPos );
    CleanupStack::PushL( evalKeys );
    CLiwGenericParamList* ret = CLiwGenericParamList::NewL();
    CleanupStack::PushL( ret );
    if ( evalKeys )
            {
            evalKeys->ToGenericParamListL( *ret );
            }
    CleanupStack::Pop( ret );
    CleanupStack::PopAndDestroy( evalKeys );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdService::EvaluateCommandAsKeyL( 
        const CLiwGenericParamList& aList,
        TInt aPos
        )
    {
    CHnMdBaseKey* keys = iServiceCommand->GetCommandData();
    CHnMdBaseKey* evalKeys = NULL;
    if (keys)
        {
        evalKeys = keys->EvaluateLC( aList, aPos );
        CleanupStack::Pop( evalKeys );
        
        }
    return evalKeys;
    }

