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
* Description:  Hsps configuration message
*
*/


#include "hspsconfiguration.h"
#include "pluginmap.h"
#include "objectmap.h"
#include "itemmap.h"
#include "plugininfo.h" 

namespace hspswrapper{


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CHspsConfiguration::CHspsConfiguration()
    {
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHspsConfiguration::ConstructL()
    {
    iPluginInfo = CPluginInfo::NewL();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration* CHspsConfiguration::NewL()
    {
    CHspsConfiguration* self = CHspsConfiguration::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration* CHspsConfiguration::NewLC()
    {
    CHspsConfiguration* self = new( ELeave ) CHspsConfiguration;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration::~CHspsConfiguration()
    {
    delete iId;
    delete iPluginInfo;
    iPlugins.ResetAndDestroy();
    iSettings.ResetAndDestroy();
    iResources.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration& CHspsConfiguration::SetConfIdL(const TDesC8& aId)
    {
    delete iId;
    iId = NULL;
    iId = aId.AllocL();
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CHspsConfiguration::ConfId()const
    {
    if(iId)
        {
        return *iId;
        }
    else
        {
        return KNullDesC8;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CPluginInfo& CHspsConfiguration::PluginInfo()
    {
    return *iPluginInfo;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration& CHspsConfiguration::AddPluginMapL(CPluginMap* aPluginMap)
    {
    iPlugins.AppendL(aPluginMap);
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CPluginMap>& CHspsConfiguration::PluginMaps()
    {
    return iPlugins;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration& CHspsConfiguration::AddItemMapL(CItemMap* aItemMap)
    {
    iSettings.AppendL(aItemMap);
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CItemMap>& CHspsConfiguration::Settings()
    {
    return iSettings;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfiguration& CHspsConfiguration::AddObjectMapL(CObjectMap* aObjectMap)
    {
    iResources.AppendL(aObjectMap);
    return *this;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CObjectMap>& CHspsConfiguration::Resources()
    {
    return iResources;
    }

}   // namespace end

