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


#include "hnmdeventmapping.h"
#include "hnmdeventmappingelement.h"
#include "hnmdevent.h"
#include "hneventhandler.h"
#include "hnsuitemodel.h"
#include "hnitemmodel.h"
#include "hnmdaction.h"
#include "hnactionmodel.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
RPointerArray<CHnMdAction> CHnMdEventMapping::GetActionsForEventId(
    TInt aEventId )
    {
    RPointerArray<CHnMdAction> emptyActions;
    CHnMdEventMappingElement* eventMappingElement = NULL;
    TBool eventIdFound( EFalse );

    TInt mappingCount( Count() );

    for ( TInt i = 0; i < mappingCount; i++ )
        {
        CHnMdEventMappingElement* eme = iMappings[i];

        RPointerArray<CHnMdEvent> events = eme->Events();

        TInt actionsCount = events.Count();

        for ( TInt j = 0; j < actionsCount; j++ )
            {
            CHnMdEvent* event = events[ j ];

            if (event->Id() == aEventId )
                {
                eventIdFound = ETrue;
                break;
                }
            }
        if ( eventIdFound )
            {
            eventMappingElement = eme;
            break;
            }
        }

    if ( eventIdFound )
        return eventMappingElement->Actions();
    else
        return emptyActions;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdEventMapping::ConstructL( TXmlEngElement aElement, THnMdCommonPointers* aCmnPtrs )
    {
    // construction...
    iCmnPtrs = aCmnPtrs;

    RXmlEngNodeList< TXmlEngElement > children;
    CleanupClosePushL(children);

    aElement.GetChildElements(children);
    TInt amount = children.Count();

    for (TInt i(0);i<amount;i++)
        {
        TXmlEngElement child = children.Next();
        if ( !child.Name().Compare( KEventElementName8 ) )
            {
            CHnMdEventMappingElement* eme =
                CHnMdEventMappingElement::NewL( child );
            CleanupStack::PushL( eme );
            iMappings.AppendL( eme );
            CleanupStack::Pop( eme );
            }
        }

    CleanupStack::PopAndDestroy( &children );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdEventMapping* CHnMdEventMapping::NewL( TXmlEngElement aElement,
        THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdEventMapping* self = CHnMdEventMapping::NewLC( aElement,
        aCmnPtrs );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CHnMdEventMapping::Count() const
    {
    return iMappings.Count();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdEventMapping* CHnMdEventMapping::NewLC( TXmlEngElement aElement,
        THnMdCommonPointers* aCmnPtrs )
    {
    CHnMdEventMapping* self = new( ELeave ) CHnMdEventMapping;
    CleanupStack::PushL( self );
    self->ConstructL( aElement, aCmnPtrs );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdEventMapping::CHnMdEventMapping()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CHnMdEventMapping::~CHnMdEventMapping()
    {
    iMappings.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdEventMapping::AddActionL(
        RHashMap<TInt,CArrayPtr<CHnActionModel> *> & aActions,
        CHnActionModel *& aActionModel, TInt aEventId )
    {
    CArrayPtr<CHnActionModel>** actionsContainer = aActions.Find( aEventId );
    if ( !actionsContainer )
        {
        CArrayPtr<CHnActionModel>* newActionsContainer = new(
            ELeave ) CArrayPtrFlat<CHnActionModel>( KDefaultGranularity );
        CleanupStack::PushL( newActionsContainer );
        aActions.InsertL( aEventId, newActionsContainer );
        actionsContainer = &newActionsContainer;
        CleanupStack::Pop( newActionsContainer );
        }
    ( *actionsContainer )->AppendL( aActionModel );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdEventMapping::FillActionsL(
        RHashMap< TInt, CArrayPtr<CHnActionModel>* >& aActions,
        const CLiwGenericParamList& aQueriesResultsList, TInt aPos )
    {
    RArray< TInt > eventIds;
    CleanupClosePushL( eventIds );
    GetEventIdsL( eventIds );

    for ( TInt i( 0 ); i < eventIds.Count(); i++ )
        {
        RPointerArray< CHnMdAction > actions = GetActionsForEventId(
                eventIds[ i ] );
        for ( TInt j( 0 ); j < actions.Count(); j++ )
            {
            if ( !actions[ j ] || !actions[ j ]->IsValidL(
                    aQueriesResultsList, aPos ) )
                {
                continue;
                }
            CHnActionModel* actionModel = CHnActionModel::NewL();
            CleanupStack::PushL( actionModel );
            actions[ j ]->EvaluateL( actionModel, aQueriesResultsList, aPos );
            AddActionL( aActions, actionModel, eventIds[ i ] );
            CleanupStack::Pop( actionModel );
            }
        }
    CleanupStack::PopAndDestroy( &eventIds );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CHnMdEventMapping::GetEventIdsL( RArray< TInt >& aIds ) const
    {
    TInt mappingCount = Count();

    for( TInt i = 0; i < mappingCount; i++ )
        {
        CHnMdEventMappingElement* eme = iMappings[ i ];
        const RPointerArray< CHnMdEvent >& events = eme->Events();
        TInt eventsCount = events.Count();
        for ( TInt j = 0; j < eventsCount; j++ )
            {
            CHnMdEvent* event = events[ j ];
            aIds.AppendL( event->Id() );
            }
        }
    }

