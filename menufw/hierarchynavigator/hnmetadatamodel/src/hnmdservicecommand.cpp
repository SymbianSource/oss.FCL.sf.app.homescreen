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

#include "hnmdkeyfactory.h"
#include "hnmdbasekey.h"
#include "hnmdservicecommand.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdServiceCommand* CHnMdServiceCommand::NewL( 
        TXmlEngElement aElement )
    {
    CHnMdServiceCommand* self = CHnMdServiceCommand::NewLC(aElement);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdServiceCommand* CHnMdServiceCommand::NewLC( 
        TXmlEngElement aElement )
    {
    CHnMdServiceCommand* self = new( ELeave ) CHnMdServiceCommand;
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TXmlEngElement CHnMdServiceCommand::RemoveRedundantKeyL( TXmlEngElement aElement )
    {
    TXmlEngElement ret = aElement;
    
    RXmlEngNodeList< TXmlEngElement > elements;
    CleanupClosePushL( elements );
    aElement.GetChildElements( elements );

    TInt elementsCount = elements.Count();
    if (elementsCount == 1)
        {
        TXmlEngElement dataElement = elements.Next();
        RXmlEngNodeList< TXmlEngAttr > nodeList;
        CleanupClosePushL( nodeList );
        dataElement.GetAttributes(nodeList);
        if (! dataElement.Name().Compare( KKeyElementItem8 ) && nodeList.Count() == 0)
            {
            ret = dataElement;
            }
        CleanupStack::PopAndDestroy( &nodeList );
        }
    
    CleanupStack::PopAndDestroy( &elements );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdServiceCommand::ConstructL( TXmlEngElement aElement )
    {
    iData = HnMdKeyFactory::CreateL( KCommandElementItem8(), KKeyTypeList(), 
            KNullDesC8() );
    
    RXmlEngNodeList< TXmlEngElement > elements;
    CleanupClosePushL( elements );
    aElement.GetChildElements( elements );

    TInt elementsCount = elements.Count();

    for (TInt i =0; i < elementsCount; i++ )
        { 
        TXmlEngElement dataElement = elements.Next();

        if ( !dataElement.Name().Compare( KCommandElementItem8 ) )
            {
            SetCommandNameL( dataElement.AttributeNodeL( KCommandNameAttr8 ) );
            SetCommandModeL( dataElement.AttributeNodeL( KCommandModeAttr8 ) );
            
            SetCommandDataL( RemoveRedundantKeyL( dataElement ) );
            break;
            }
        }
    CleanupStack::PopAndDestroy( &elements );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdServiceCommand::CHnMdServiceCommand()
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdServiceCommand::~CHnMdServiceCommand()
    {
    delete iData;
    iName.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TDesC8& CHnMdServiceCommand::GetCommandName() const
    {
    return iName;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdServiceCommand::SetCommandNameL( TXmlEngAttr aAttr )
    {
    iName.Close();
    iName.CreateL( aAttr.Value() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdBaseKey* CHnMdServiceCommand::GetCommandData() const
    {
    return iData;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdServiceCommand::SetCommandDataL( TXmlEngElement aElement )
    {
    
    RXmlEngNodeList< TXmlEngElement > keys;
    CleanupClosePushL( keys );
    aElement.GetChildElements( keys );

    TInt count = keys.Count();
    for (TInt j = 0; j < count; j++ )
        {
        TXmlEngElement keyElement = keys.Next();
        if ( !keyElement.Name().Compare( KKeyElementItem8 ) )
            {
            CHnMdBaseKey *key = HnMdKeyFactory::CreateL( keyElement );
            CleanupStack::PushL( key );
            iData->AddSubKeyL(key);
            CleanupStack::Pop( key );
            }
        }
    CleanupStack::PopAndDestroy( &keys );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TServiceMode CHnMdServiceCommand::GetCommandMode() const
    {
    return iMode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdServiceCommand::SetCommandModeL( TXmlEngAttr aAttr )
    {
    iMode = EServiceModeSynchronous;
    
    if ( !aAttr.Value().Compare( KCommandModeAsync8 ) )
        {
            iMode = EServiceModeAsynchronous;
        }
    }

