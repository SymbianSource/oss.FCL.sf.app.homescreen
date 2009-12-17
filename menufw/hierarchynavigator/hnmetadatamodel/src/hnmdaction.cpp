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
* Description:  action model
*
*/


#include <liwcommon.h>

#include "hnmdaction.h"
#include "hnmdkeyfactory.h"
#include "hnmdbasekey.h"
#include "hnconditionfactory.h"
#include "hnmdservicecommand.h"
#include "hnactionmodel.h"
#include "hnmdservice.h"
#include "hnconditioninterface.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdAction::ConstructL( TXmlEngElement aElement )
    {
    ASSERT( aElement.Name() == KActionElementName8 );

    iService = CHnMdService::NewL( aElement );
    
    if( aElement.AttributeValueL( KMenuConditionAttrName8 ).Length() )
        {
        SetConditionL( aElement.AttributeNodeL( KMenuConditionAttrName8 ) );
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdAction* CHnMdAction::NewL( TXmlEngElement aElement )
    {
    CHnMdAction* self = CHnMdAction::NewLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdAction* CHnMdAction::NewLC( TXmlEngElement aElement )
    {
    CHnMdAction* self = new( ELeave ) CHnMdAction;
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    return self;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdAction::CHnMdAction()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdAction::~CHnMdAction()
    {
    delete iService;
    delete iCondition; 
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdAction::SetConditionL( TXmlEngAttr aAttr )
    {
    iCondition = HnConditionFactory::NewL( aAttr );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CHnMdAction::EvaluateL( CHnActionModel* aActionModel,
            const CLiwGenericParamList& aQueriesResultsList, TInt aPos)
    {
    TInt ret( KErrNone );
    
    aActionModel->SetServiceL( iService->GetServiceName() );
    aActionModel->SetInterfaceL( iService->GetInterfaceName() );
    aActionModel->SetCommandNameL( iService->GetCommand().GetCommandName() );
    aActionModel->SetServiceModeL( iService->GetCommand().GetCommandMode() );
    aActionModel->SetConstructorL( 
            iService->EvaluateConstructorAsKeyL( aQueriesResultsList, aPos ) );
    aActionModel->SetCommand( 
            iService->EvaluateCommandAsKeyL( aQueriesResultsList, aPos ) );
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CHnMdAction::IsValidL( const CLiwGenericParamList& aQueryResults, 
        TInt aPos )    
    {
    TBool ret( EFalse );
    if( !iCondition || iCondition->ResultL( aQueryResults, aPos ) )
        {
        ret = ETrue;
        }
    return ret;
    }

