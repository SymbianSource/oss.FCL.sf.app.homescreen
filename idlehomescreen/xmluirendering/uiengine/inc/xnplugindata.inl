/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data class to hold widget info - inline functions
*
*/


#ifndef _XNPLUGINDATA_INL_
#define _XNPLUGINDATA_INL_

// ---------------------------------------------------------------------------
// Sets plugindata node
// ---------------------------------------------------------------------------
//
inline void CXnPluginData::SetNode( CXnDomNode* aNode )
    {
    iNode = aNode;
    };

// ---------------------------------------------------------------------------
// Returns plugindata node
// ---------------------------------------------------------------------------
//
inline CXnDomNode* CXnPluginData::Node() const
    {
    return iNode;
    };

// ---------------------------------------------------------------------------
// Sets Owner
// ---------------------------------------------------------------------------
//
inline void CXnPluginData::SetOwner( CXnDomNode* aOwner )
    {
    iOwner = aOwner;
    };

// ---------------------------------------------------------------------------
// Returns Owner
// ---------------------------------------------------------------------------
//
inline CXnDomNode* CXnPluginData::Owner() const
    {
    return iOwner;
    };

// -----------------------------------------------------------------------------
// Return the parent of this plugin data
// -----------------------------------------------------------------------------
//
inline CXnPluginData* CXnPluginData::Parent() const
    {
    return iParent;
    };

// -----------------------------------------------------------------------------
// Return view manager
// -----------------------------------------------------------------------------
//
inline CXnViewManager& CXnPluginData::ViewManager() const
    {
    return iManager;
    };

// ---------------------------------------------------------------------------
// Queries whether this plugin is in active view
// ---------------------------------------------------------------------------
//
inline TBool CXnPluginData::Active() const
    {
    TBool retval( EFalse );
    
    if( iParent )
        {
        retval = iParent->Active();
        }
    
    return retval;
    };

// ---------------------------------------------------------------------------
// Returns Config. ID
// ---------------------------------------------------------------------------
//
inline const TDesC8& CXnPluginData::ConfigurationId() const
    {
    return iConfigurationId ? *iConfigurationId : KNullDesC8();
    };

// ---------------------------------------------------------------------------
// Returns plugin ID
// ---------------------------------------------------------------------------
//
inline const TDesC8& CXnPluginData::PluginId() const
    {
    return iPluginId ? *iPluginId : KNullDesC8();
    };

// ---------------------------------------------------------------------------
// Returns plugin UID
// ---------------------------------------------------------------------------
//
inline const TDesC8& CXnPluginData::PluginUid() const
    {
    return iPluginUid ? *iPluginUid : KNullDesC8();
    };

// ---------------------------------------------------------------------------
// Returns plugin name
// ---------------------------------------------------------------------------
//
inline const TDesC8& CXnPluginData::PluginName() const
    {
    return iPluginName ? *iPluginName : KNullDesC8();
    };

// ---------------------------------------------------------------------------
// Returns plugin type
// ---------------------------------------------------------------------------
//
inline const TDesC8& CXnPluginData::Type() const
    {
    return iPluginType ? *iPluginType : KNullDesC8();
    };

// ---------------------------------------------------------------------------
// Returns plugin state
// ---------------------------------------------------------------------------
//
inline const TDesC8& CXnPluginData::PluginState() const
    {
    return iPluginState ? *iPluginState : KNullDesC8();
    };
    
// ---------------------------------------------------------------------------
// Returns the publisher name
// ---------------------------------------------------------------------------
//
inline const TDesC& CXnPluginData::PublisherName() const 
    { 
    return iPublisherName ? *iPublisherName : KNullDesC16(); 
    };

// -----------------------------------------------------------------------------
// Sets this plugin occupied, i.e. holding widget
// -----------------------------------------------------------------------------
//
inline void CXnPluginData::SetOccupied()
    {
    iFlags.Set( EIsOccupied );    
    };

// -----------------------------------------------------------------------------
// Queries whether the plugin is occupied
// -----------------------------------------------------------------------------
//
inline TBool CXnPluginData::Occupied() const
    {
    return ( iFlags.IsSet( EIsOccupied ) ? ETrue : EFalse );    
    };

// ---------------------------------------------------------------------------
// Sets removable
// ---------------------------------------------------------------------------
//
inline void CXnPluginData::SetRemovable( TBool aRemovable )
    {    
    if( aRemovable )
        {
        iFlags.Set( EIsRemovable );        
        }
    else        
        {
        iFlags.Clear( EIsRemovable );        
        }
    };

// ---------------------------------------------------------------------------
// Returns removable
// ---------------------------------------------------------------------------
//
inline TBool CXnPluginData::Removable() const
    {
    return ( iFlags.IsSet( EIsRemovable ) ? ETrue : EFalse );    
    };

// ---------------------------------------------------------------------------
// Returns list of plugindata
// ---------------------------------------------------------------------------
//
inline RPointerArray< CXnPluginData >& CXnPluginData::PluginData() const
    {
    return iPluginsData;
    };

#endif // _XNPLUGINDATA_INL_
