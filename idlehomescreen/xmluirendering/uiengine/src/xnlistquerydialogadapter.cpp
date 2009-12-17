/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  List query dialog adapter and data interface
*
*/


//  System includes
#include <e32base.h>
#include <aknlistquerydialog.h>
#include <utf.h>
#include <xnuiengine.rsg>

// User includes
#include "xnappuiadapter.h"
#include "xnlistquerydialogadapter.h"
#include "xnnodepluginif.h"
#include "xntype.h"
#include "xnproperty.h"
#include "xnuiengine.h"
#include "xnodt.h"
#include "xndomdocument.h"
#include "xndomnode.h"
#include "xndomlist.h"
#include "xndomattribute.h"

// ======== LOCAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// Finds recursively node by type
// @return    returns pointer to desired node, NULL if nothing found 
// ---------------------------------------------------------------------------
//
CXnDomNode* FindNodeByType( CXnDomNode* aNode, const TDesC8& aName )
    {
    if ( aNode == NULL )
        {
        return NULL;
        }

    if ( aNode->Name() == aName )
        {
        return aNode;
        }

    CXnDomList& list( aNode->ChildNodes() );

    for ( TInt i = 0; i < list.Length() ; i++ )
        {
        CXnDomNode* retval( FindNodeByType(
                static_cast< CXnDomNode* >( list.Item( i ) ), aName ) );

        if ( retval )
            {
            return retval;
            }
        }

    return NULL;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Symbian static 1st phase constructor
// -----------------------------------------------------------------------------
//
CXnListQueryDialog* CXnListQueryDialog::NewL()
    {
    CXnListQueryDialog* self = new( ELeave ) CXnListQueryDialog;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;    
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnListQueryDialog::ConstructL()
    {
    CXnComponent::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CXnListQueryDialog::CXnListQueryDialog()
    {
    }

// -----------------------------------------------------------------------------
// C++ destructor
// -----------------------------------------------------------------------------
//
CXnListQueryDialog::~CXnListQueryDialog()
    {
    }
    
// ---------------------------------------------------------
// Replaces or appends item into list
// ---------------------------------------------------------
//
void CXnListQueryDialog::ReplaceItemL( const TDesC& aText, TInt aIndex )
    {
    (static_cast<CXnListQueryDialogAdapter*>(ControlAdapter()))->ReplaceItemL( aText, aIndex );
    }

// ---------------------------------------------------------
// Inserts or appends item into list
// ---------------------------------------------------------
//
void CXnListQueryDialog::InsertItemL( const TDesC& aText, TInt aIndex )
    {
    (static_cast<CXnListQueryDialogAdapter*>(ControlAdapter()))->InsertItemL( aText, aIndex );
    }

// ---------------------------------------------------------
// Deletes item from the list
// ---------------------------------------------------------
//
void CXnListQueryDialog::DeleteItem( TInt aIndex )
    {
    return (static_cast<CXnListQueryDialogAdapter*>(ControlAdapter()))->DeleteItem( aIndex );   
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
XnComponentInterface::MXnComponentInterface* CXnListQueryDialog::MakeInterfaceL(const TDesC8& aType)
    {
    if (aType != XnListQueryDialogInterface::KType)
        {
        return NULL;
        }
    XnListQueryDialogInterface::MXnListQueryDialogInterface* interface =
        static_cast<XnListQueryDialogInterface::MXnListQueryDialogInterface*>( this );
    return interface;
    }  


// -----------------------------------------------------------------------------
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
//
CXnListQueryDialogAdapter* CXnListQueryDialogAdapter::NewL( CXnNodePluginIf& aNode )
    {
    CXnListQueryDialogAdapter* self = 
        new ( ELeave ) CXnListQueryDialogAdapter( aNode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CXnListQueryDialogAdapter::~CXnListQueryDialogAdapter()
    {
    delete iItemArray;
    iStaticItems.Close();
    }

// -----------------------------------------------------------------------------
// C++ default constructor. Must not leave.
// -----------------------------------------------------------------------------
//
CXnListQueryDialogAdapter::CXnListQueryDialogAdapter( CXnNodePluginIf& aNode )
    : iNode( aNode )
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor. Can leave.
// -----------------------------------------------------------------------------
//    
void CXnListQueryDialogAdapter::ConstructL()
    {
    CXnControlAdapter::ConstructL( iNode );
    iItemArray = new ( ELeave ) CDesCArrayFlat( 8 );
    // find normal menu items and dynamic menu items
    // Only one dynamic item allowed
    RPointerArray< CXnNodePluginIf > children( iNode.ChildrenL() );
    CleanupClosePushL( children );
    for ( TInt i = 0; i < children.Count(); i++ )
        {
        CXnNodePluginIf* node( children[i] );
        if ( node->Type()->Type() == XnPropertyNames::menu::KMenuItem)
            {
            iStaticItems.AppendL( node );
            CXnProperty* labelProp = node->GetPropertyL( XnPropertyNames::menu::KLabel );
            if( labelProp )
                {
                HBufC* label( labelProp->StringValueL());
                CleanupStack::PushL( label );
                iItemArray->AppendL( *label );
                CleanupStack::PopAndDestroy( label );
                }
            }
        else if( node->Type()->Type() == XnPropertyNames::menu::KDynMenuItem )
            {
            iDynamicItem = node;
            }
        }
    CleanupStack::PopAndDestroy( &children );
    }

// -----------------------------------------------------------------------------
// Replaces or appends item into list
// -----------------------------------------------------------------------------
// 
void CXnListQueryDialogAdapter::ReplaceItemL(const TDesC& aText, TInt aIndex )
    {
    aIndex += iStaticItems.Count();
    TInt count( iItemArray->Count());
    if( aIndex >= count )
        {
        iItemArray->AppendL( aText );
        }
    else
        {
        iItemArray->Delete( aIndex );
        iItemArray->InsertL( aIndex, aText );
        }
    }

// -----------------------------------------------------------------------------
// Inserts or appends item into list
// -----------------------------------------------------------------------------
// 
void CXnListQueryDialogAdapter::InsertItemL(const TDesC& aText, TInt aIndex )
    {
    aIndex += iStaticItems.Count();
    TInt count( iItemArray->Count());
    if( count < aIndex )
        {
        aIndex = count;
        }
    iItemArray->InsertL( aIndex, aText );
    }

// -----------------------------------------------------------------------------
// Deletes item from the list
// -----------------------------------------------------------------------------
// 
void CXnListQueryDialogAdapter::DeleteItem( TInt aIndex )
    {
    aIndex += iStaticItems.Count();
    if( aIndex < iItemArray->Count())
        {
        iItemArray->Delete( aIndex );
        }    
    }

// -----------------------------------------------------------------------------
// Displays the list query dialog
// -----------------------------------------------------------------------------
//
void CXnListQueryDialogAdapter::TryDisplayingDialogL( )
    {
    CXnAppUiAdapter& appui( static_cast< CXnAppUiAdapter& >( *iAvkonAppUi ) );
    
    appui.HideFocus();
    
    TInt selectedIndex( 0 );
    
    CAknListQueryDialog* query =
       new ( ELeave ) CAknListQueryDialog( &selectedIndex );

    query->PrepareLC( R_XML_LISTQUERY );
/* Not tested
    CAknPopupHeadingPane* heading( query->QueryHeading() );
    if ( heading )
        {
        CXnProperty* prop( iNode.GetPropertyL(
            XnPropertyNames::listquerydialog::KS60Heading ) );

        if ( prop )
            {
            HBufC* header( prop->StringValueL() );
            CleanupStack::Push( header );
            heading->SetTextL( *header );
            CleanupStack::PopAndDestroy( header );
            }
        else
            {
            heading->SetTextL( KNullDesC() );
            }
        }
*/
    query->SetItemTextArray( iItemArray );
    query->SetOwnershipType( ELbmDoesNotOwnItemArray );

    if ( query->RunLD() )
        {
        ActivateItemL( selectedIndex );
        }
    }

// -----------------------------------------------------------------------------
// Runs the activate event for the item node 
// -----------------------------------------------------------------------------
//
void CXnListQueryDialogAdapter::ActivateItemL( TInt aIndex )
    {
    if( aIndex < iStaticItems.Count())
        {
        iStaticItems[aIndex]->Node().SetStateL( 
                XnPropertyNames::style::common::KActive );
        iStaticItems[aIndex]->Node().UnsetStateL( 
                XnPropertyNames::style::common::KActive );
        }
    else
        {
        ModifyDynamicEventL( aIndex - iStaticItems.Count());
        iDynamicItem->Node().SetStateL( 
                        XnPropertyNames::style::common::KActive );
        iDynamicItem->Node().UnsetStateL( 
                XnPropertyNames::style::common::KActive );
        }
    }

// -----------------------------------------------------------------------------
// Replaces the '#' char by index in event. Syntax "(#"
// -----------------------------------------------------------------------------
//
void CXnListQueryDialogAdapter::ModifyDynamicEventL( TInt aIndex )
    {
    CXnDomNode* eventNode = FindNodeByType( iDynamicItem->Node().DomNode(), XnPropertyNames::action::KEvent );

    CXnDomAttribute* attribute = static_cast<CXnDomAttribute*> 
        (eventNode->AttributeList().FindByName( XnPropertyNames::action::event::KName ));
    
    HBufC8* nameStr( attribute->Value().AllocLC());
    TPtr8 namePtr = nameStr->Des();
    
    //Find '(' char
    TInt pos = namePtr.Locate('(');
    TBuf8<4> index;
    index.AppendNum( aIndex );
    namePtr.Replace( pos+1, 1, index );
    
    attribute->SetValueL( namePtr );
    CleanupStack::PopAndDestroy( nameStr );
    }

//  End of File
