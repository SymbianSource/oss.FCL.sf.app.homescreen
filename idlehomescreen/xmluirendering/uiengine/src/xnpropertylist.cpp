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
* Description:  Xuikon property list
*
*/

//  INCLUDES
#include "xnpropertylist.h"
#include "xnproperty.h"
#include "xnproperty.h"
#include "xnpropertycomparator.h"
#include "xncomparator.h"
#include "xnmap.h"
#include "xndomproperty.h"
#include "xndompropertyvalue.h"
#include "xnpanic.h"

// LOCAL CONSTANTS AND MACROS
_LIT8( KPseudoClassFocus, "focus" );
_LIT8( KPseudoClassPassiveFocus, "passivefocus" );
_LIT8( KPseudoClassHold, "hold" );
_LIT8( KPseudoClassHover, "hover" );
_LIT8( KPseudoClassLink, "link" );
_LIT8( KPseudoClassVisited, "visited" );
_LIT8( KPseudoClassActive, "active" );
_LIT8( KPseudoClassEdit, "edit" );
_LIT8( KPseudoClassPressedDown, "presseddown" );

// LOCAL FUNCTION PROTOTYPES
static const TDesC8& PseudoClassName(
    CXnDomProperty::TPseudoClass aPseudoClass );
static CXnDomProperty::TPseudoClass PseudoClassFromName(
    const TDesC8& aName );
static TInt FindPseudoClass(
    const RArray< CXnDomProperty::TPseudoClass >& aArray,
    const TDesC8& aPseudoClass );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PseudoClassName
// -----------------------------------------------------------------------------
//
static const TDesC8& PseudoClassName( CXnDomProperty::TPseudoClass aPseudoClass )
    {
    switch ( aPseudoClass )
        {
        case CXnDomProperty::EFocus:
            return KPseudoClassFocus;
        case CXnDomProperty::EPassiveFocus:
            return KPseudoClassPassiveFocus;
        case CXnDomProperty::EHold:
            return KPseudoClassHold;
        case CXnDomProperty::EHover:
            return KPseudoClassHover;
        case CXnDomProperty::ELink:
            return KPseudoClassLink;
        case CXnDomProperty::EVisited:
            return KPseudoClassVisited;
        case CXnDomProperty::EActive:
            return KPseudoClassActive;
        case CXnDomProperty::EEdit:
            return KPseudoClassEdit;
        case CXnDomProperty::EPressedDown:
            return KPseudoClassPressedDown;
        default:
            return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// PseudoClassFromName
// -----------------------------------------------------------------------------
//
static CXnDomProperty::TPseudoClass PseudoClassFromName( const TDesC8& aName )
    {
    if ( aName == KPseudoClassFocus )
        {
        return CXnDomProperty::EFocus;
        }
    else if ( aName == KPseudoClassPassiveFocus )
        {
        return CXnDomProperty::EPassiveFocus;
        }
    else if ( aName == KPseudoClassHold )
        {
        return CXnDomProperty::EHold;
        }
    else if ( aName == KPseudoClassHover )
        {
        return CXnDomProperty::EHover;
        }
    else if ( aName == KPseudoClassLink )
        {
        return CXnDomProperty::ELink;
        }
    else if ( aName == KPseudoClassVisited )
        {
        return CXnDomProperty::EVisited;
        }
    else if ( aName == KPseudoClassActive )
        {
        return CXnDomProperty::EActive;
        }
    else if ( aName == KPseudoClassEdit )
        {
        return CXnDomProperty::EEdit;
        }
    else if ( aName == KPseudoClassPressedDown )
        {
        return CXnDomProperty::EPressedDown;
        }
    return CXnDomProperty::ENone;
    }

// -----------------------------------------------------------------------------
// FindPseudoClass
// -----------------------------------------------------------------------------
//
static TInt FindPseudoClass(
    const RArray< CXnDomProperty::TPseudoClass >& aArray,
    const TDesC8& aPseudoClass )
    {
    for ( TInt i = aArray.Count() - 1; i >= 0; --i )
        {
        CXnDomProperty::TPseudoClass item = aArray[i];
        if ( PseudoClassName( item ) == aPseudoClass )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnPropertyList::NewL()
// -----------------------------------------------------------------------------
//
CXnPropertyList* CXnPropertyList::NewL()
    {
    CXnPropertyList* self = new ( ELeave ) CXnPropertyList;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();//self
    return self;
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::~CXnPropertyList()
// -----------------------------------------------------------------------------
//
CXnPropertyList::~CXnPropertyList()
    {
    delete iMap;

    iCurrentPseudoClasses.Reset();
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::CXnPropertyList()
// -----------------------------------------------------------------------------
//
CXnPropertyList::CXnPropertyList()
    {
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::ConstructL()
// -----------------------------------------------------------------------------
//
void CXnPropertyList::ConstructL()
    {
    iMap = CXnMap::NewL( new ( ELeave ) TXnPropertyComparator );
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::SetPropertyL
// Set a property.
// -----------------------------------------------------------------------------
//
void CXnPropertyList::SetPropertyL( CXnProperty* aProperty )
    {
    CXnPropertyKey key;
    key.iString = &( aProperty->Property()->Name() );
    key.iPseudoClass = aProperty->Property()->PseudoClass();

    CXnProperty* tmp = static_cast< CXnProperty* >( iMap->Get( key ) );
    if ( tmp == aProperty )
        {
        return;
        }
    TBool replaced = EFalse;
    for ( TInt i = ( iMap->Container() ).Count() - 1; i >= 0; --i )
        {
        tmp = static_cast< CXnProperty* >( ( iMap->Container() )[i] );
        if ( aProperty->Property()->PseudoClass() == CXnDomProperty::ENone )
            {
            const TDesC8& tmpName = tmp->Property()->Name();
            const TDesC8& propertyName = aProperty->Property()->Name();
            if ( tmpName == propertyName )
                {
                if ( tmp->Property()->PseudoClass() != CXnDomProperty::ENone )
                    {
                    TBool pseudoMatch = EFalse;
                    for ( TInt j = iCurrentPseudoClasses.Count() - 1; j >= 0; --j )
                        {
                        CXnDomProperty::TPseudoClass pseudoClass =
                            iCurrentPseudoClasses[j];
                        if ( tmp->Property()->PseudoClass() == pseudoClass )
                            {
                            pseudoMatch = ETrue;
                            break;
                            }
                        }
                    if ( !pseudoMatch )
                        {
                        continue;
                        }
                    CXnProperty* newProperty = aProperty->CloneL();
                    CleanupStack::PushL( newProperty );
                    newProperty->Property()->SetPseudoClass(
                        tmp->Property()->PseudoClass() );
                    // add new object
                    if ( ( iMap->Container()).Append( newProperty ) != KErrNone )
                        {
                        User::Leave( KXnErrAddingProperyToListFailed );
                        }
                    CleanupStack::Pop( newProperty );
                    }
                if ( ( iMap->Container() ).Append( aProperty )!= KErrNone )
                    {
                    User::Leave( KXnErrAddingProperyToListFailed_2 );
                    }
                replaced = ETrue;
                // remove old object
                delete tmp;
                ( iMap->Container() ).Remove( i );
                }
            }
        else
            {
            const TDesC8& tmpName = tmp->Property()->Name();
            const TDesC8& propertyName = aProperty->Property()->Name();
            if ( tmp->Property()->PseudoClass() != CXnDomProperty::ENone &&
                 tmp->Property()->PseudoClass() == aProperty->Property()->PseudoClass() &&
                 tmpName == propertyName )
                {
                if ( ( iMap->Container() ).Append( aProperty )!= KErrNone )
                    {
                    User::Leave( KXnErrAddingProperyToListFailed_3 );
                    }
                replaced = ETrue;
                // remove old object
                delete tmp;
                ( iMap->Container() ).Remove( i );
                }
            }
        }
    if ( replaced )
        {
        return;
        }
    if ( aProperty->Property()->PseudoClass() != CXnDomProperty::ENone )
        {
        for ( TInt i = iCurrentPseudoClasses.Count() - 1; i >= 0; --i )
            {
            CXnDomProperty::TPseudoClass pseudoClass = iCurrentPseudoClasses[i];
            CXnProperty* newProperty = aProperty->CloneL();
            CleanupStack::PushL( newProperty );
            newProperty->Property()->SetPseudoClass( pseudoClass );
            // add new object
            User::LeaveIfError( ( iMap->Container() ).Append( newProperty ) );
            if ( ( iMap->Container() ).Append( newProperty )!= KErrNone )
                {
                User::Leave( KXnErrAddingProperyToListFailed_4 );
                }
            CleanupStack::Pop( newProperty );
            }
        }
    if ( ( iMap->Container() ).Append( aProperty )!= KErrNone )
        {
        User::Leave( KXnErrAddingProperyToListFailed_5 );
        }
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::GetProperty
// Gets a property.
// -----------------------------------------------------------------------------
//
CXnProperty* CXnPropertyList::GetProperty( const TDesC8& aKey ) const
    {
    CXnPropertyKey key;
    key.iString = &aKey;
    for ( TInt i = iCurrentPseudoClasses.Count() - 1; i >= 0; --i )
        {
        key.iPseudoClass = iCurrentPseudoClasses[i];
        CXnProperty* property = static_cast< CXnProperty* >( iMap->Get( key ) );
        if ( property )
            {
            return property;
            }
        }

    key.iPseudoClass = CXnDomProperty::ENone;
    CXnProperty* property = static_cast< CXnProperty* >( iMap->Get( key ) );
    if ( property )
        {
        return property;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::SetPseudoClassL
// Set a pseudoclass
// -----------------------------------------------------------------------------
//
TBool CXnPropertyList::SetStateL( const TDesC8& aState )
    {
    if ( FindPseudoClass( iCurrentPseudoClasses, aState ) < 0 )
        {
        User::LeaveIfError( iCurrentPseudoClasses.Append(
            PseudoClassFromName( aState ) ) );
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::IsStateSet
// Check whether a state is set or not
// -----------------------------------------------------------------------------
//
TBool CXnPropertyList::IsStateSet( const TDesC8& aState )
    {
    if ( FindPseudoClass( iCurrentPseudoClasses, aState ) < 0 )
        {
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::UnsetPseudoClass
// Unset a pseudoclass
// -----------------------------------------------------------------------------
//
TBool CXnPropertyList::UnsetState( const TDesC8& aState )
    {
    TInt index = FindPseudoClass( iCurrentPseudoClasses, aState );
    if ( index < 0 )
        {
        return EFalse;
        }
    iCurrentPseudoClasses.Remove( index );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CXnPropertyList::CloneL
// Clone the list
// -----------------------------------------------------------------------------
//
CXnPropertyList* CXnPropertyList::CloneL()
    {
    CXnPropertyList* clone = CXnPropertyList::NewL();
    CleanupStack::PushL( clone );
    TInt count = iCurrentPseudoClasses.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        User::LeaveIfError( clone->iCurrentPseudoClasses.Append(
            iCurrentPseudoClasses[i] ) );
        }
    clone->iMap = CXnMap::NewL( new ( ELeave ) TXnPropertyComparator );
    count = iMap->Container().Count();
    for ( TInt i = 0; i < count; ++i )
        {
        User::LeaveIfError( clone->iMap->Container().Append(
            static_cast< CXnProperty* >( iMap->Container()[i] )->CloneL() ) );
        }
    CleanupStack::Pop( clone );
    return clone;
    }
