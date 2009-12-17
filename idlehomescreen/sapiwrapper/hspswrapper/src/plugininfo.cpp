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
* Description:  Presents plugin info using states uid, interface, type and name
*
*/


#include "plugininfo.h"

const TInt32 KMultiInstanceDefaultValue = 1;
const TInt32 KMaxChildsDefaultValue = 6;

namespace hspswrapper{

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CPluginInfo::CPluginInfo()
    : iMultiInstance( KMultiInstanceDefaultValue )
    {
    iMaxChilds = KMaxChildsDefaultValue;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CPluginInfo::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo* CPluginInfo::NewL()
    {
    CPluginInfo* self = CPluginInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo* CPluginInfo::NewLC()
    {
    CPluginInfo* self = new( ELeave ) CPluginInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo::~CPluginInfo()
    {
    delete iUid;
    delete iType;
    delete iInterface;
    delete iName;
	delete iConfigurationState;
    delete iDescription;
    delete iLogoIcon;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetUidL(const TDesC8& aUid)
    {
    delete iUid;
    iUid = NULL;
    iUid = aUid.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginInfo::Uid()const
    {
    if(iUid)
        {
        return *iUid;
        }
    else
        {
        return KNullDesC8;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetTypeL(const TDesC8& aType)
    {
    delete iType;
    iType = NULL;
    iType = aType.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginInfo::Type()const
    {
    if(iType)
        {
        return *iType;
        }
    else
        {
        return KNullDesC8;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetInterfaceL(
    const TDesC8& aInterface)
    {
    delete iInterface;
    iInterface = NULL;
    iInterface = aInterface.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginInfo::Interface()const
    {
    if(iInterface)
        {
        return *iInterface;
        }
    else
        {
        return KNullDesC8;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetNameL(const TDesC8& aName)
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginInfo::Name()const
    {
    if(iName)
        {
        return *iName;
        }
    else
        {
        return KNullDesC8;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetConfigurationStateL(
    const TDesC8& aConfigurationState)
    {
    delete iConfigurationState;
    iConfigurationState = NULL;
    
    iConfigurationState = aConfigurationState.AllocL();
    
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CPluginInfo::ConfigurationState()const
    {
    if(iConfigurationState)
        {
        return *iConfigurationState;
        }
    else
        {
        return KNullDesC8;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetMultiInstanceL(
    const TInt32 aMultiInstance )
    {
    iMultiInstance = aMultiInstance;
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TInt32& CPluginInfo::MultiInstance()const
    {
    return iMultiInstance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TInt32& CPluginInfo::MaxChild()const
    {
    return iMaxChilds;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetMaxChild(
    const TInt32 aMaxChild )
    {
    iMaxChilds = aMaxChild;
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetDescriptionL( const TDesC& aDescription )
    {
    delete iDescription;
    iDescription = NULL;

    iDescription = aDescription.AllocL();
    return *this;
    }

    // ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetDescriptionL( const TDesC8& aDescription )
    {
    delete iDescription;
    iDescription = NULL;

    iDescription = HBufC::NewL( aDescription.Length() );
    iDescription->Des().Copy( aDescription );
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPluginInfo::Description() const
    {
    return iDescription ? *iDescription : KNullDesC();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetLogoIconL( const TDesC& aLogoIcon )
    {
    delete iLogoIcon;
    iLogoIcon = NULL;

    iLogoIcon = aLogoIcon.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CPluginInfo::SetLogoIconL( const TDesC8& aLogoIcon )
    {
    delete iLogoIcon;
    iLogoIcon = NULL;

    iLogoIcon = HBufC::NewL( aLogoIcon.Length() );
    iLogoIcon->Des().Copy( aLogoIcon );
    return *this;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPluginInfo::LogoIcon() const
    {
    return iLogoIcon ? *iLogoIcon : KNullDesC();
    }
    
} // namespace
