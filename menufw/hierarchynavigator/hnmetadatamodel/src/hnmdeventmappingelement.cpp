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


#include "hnmdeventmappingelement.h"
#include "hnmdaction.h"
#include "hnmdevent.h"
#include "hnconvutils.h"
#include "hnglobals.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdEventMappingElement::ConstructL( TXmlEngElement aElement )
    {
    TPtrC8 n = aElement.Name();
    
    //ASSERT( aElement.Name() == KEventElementName8 );
    //ASSERT( aElement.Prefix() == KNameSpacePrefix8 );
    ASSERT( aElement.HasAttributeL( KEventAttrName8 ) );
    ASSERT( aElement.HasChildNodes() );

    SetEventsL( aElement );
    SetActionsL( aElement );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
RPointerArray<CHnMdAction>& CHnMdEventMappingElement::Actions()
    {
    return iActions;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const RPointerArray<CHnMdEvent>& CHnMdEventMappingElement::Events() const
    {
    return iEvents;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdEventMappingElement::SetActionsL( TXmlEngElement aElement )
    {
    // retrieve action
    RXmlEngNodeList< TXmlEngElement > actions;
    CleanupClosePushL( actions );
    aElement.GetChildElements( actions );
    TInt count = actions.Count();
    for( TInt i( 0 ); i < count; i++ )
        {
        TXmlEngElement element = actions.Next();
        if(!element.Name().Compare( KActionElementName8 ) )
            {
            CHnMdAction* action = CHnMdAction::NewL( element );
            CleanupStack::PushL( action );
            iActions.AppendL( action );
            CleanupStack::Pop( action );
            }
        }
    CleanupStack::PopAndDestroy( &actions );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CHnMdEventMappingElement::SetEventsL( TXmlEngElement aElement )
    {
    // parsing events
    TPtrC8 events(aElement.AttributeValueL( KEventAttrName8 ));
    
    TLex8 lex( events );
    TChar ch;
    
    TInt length( events.Length() );
    TInt cnt = 0;
    while( ch = lex.Get() != 0 )
        {
        cnt++;
        while ( ( ch = lex.Peek() ) != '|' && cnt < length)
            {
            lex.Inc();
            cnt++;
            }

        // retrieve event id
        RBuf8 eventId;
        eventId.CreateL( lex.MarkedToken() );
        
        TLex8 lexEventId( eventId );
        TInt id = -1;
        lexEventId.Val( id );

        CHnMdEvent *event = CHnMdEvent::NewL( 
                id );

        AddEventL( event );
        eventId.Close();

        lex.Inc();
        cnt++;
        lex.Mark();
        if (cnt >= length)
            break;
        }
    }

// ---------------------------------------------------------------------------
// Takes ownership of an event!
// ---------------------------------------------------------------------------
//
void CHnMdEventMappingElement::AddEventL( CHnMdEvent* aEvent )
{
    iEvents.AppendL( aEvent );
}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdEventMappingElement* CHnMdEventMappingElement::NewL( 
                                    TXmlEngElement aElement )
    {
    CHnMdEventMappingElement* self = 
            CHnMdEventMappingElement::NewLC( aElement );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdEventMappingElement* CHnMdEventMappingElement::NewLC( 
                                    TXmlEngElement aElement )
    {
    CHnMdEventMappingElement* self = new( ELeave ) CHnMdEventMappingElement;
    CleanupStack::PushL( self );
    self->ConstructL( aElement );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdEventMappingElement::CHnMdEventMappingElement()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CHnMdEventMappingElement::~CHnMdEventMappingElement()
    {
    iEvents.ResetAndDestroy();
    iActions.ResetAndDestroy();
    }

