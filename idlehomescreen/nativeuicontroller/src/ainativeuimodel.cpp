/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common NativeUi definitions and utilities
*
*/


#include "ainativeuimodel.h"
#include <aidevicestatuscontentmodel.h>

namespace AiNativeUiController
    {

namespace AiNativeUiModel 
    {

struct TRenderingPriorityItem
	{
    TInt iContentId;
    TInt iRenderingPriority;	
	};

const TInt KHighestRenderingPriority = 20;

const TInt KPriorityOffset1 = 1;
const TInt KPriorityOffset2 = 2;
const TInt KPriorityOffset3 = 3;
const TInt KPriorityOffset4 = 4;
const TInt KPriorityOffset5 = 5;
const TInt KPriorityOffset6 = 6;


const TRenderingPriorityItem KNaviPaneRenderingPriorities[] =
    {
        { EAiDeviceStatusContentProfileName,            KHighestRenderingPriority },
        { EAiDeviceStatusContentSilentIndicator,        KHighestRenderingPriority },
        { EAiDeviceStatusContentTimedProfileIndicator,  KHighestRenderingPriority },
        { EAiDeviceStatusContentDate,                   KHighestRenderingPriority - KPriorityOffset1 },
        { EAiDeviceStatusContentGeneralProfileName,     KHighestRenderingPriority - KPriorityOffset2 },
        { EAiDeviceStatusContentVHZIndicator,           KHighestRenderingPriority - KPriorityOffset3 },
        { EAiDeviceStatusContentCUGIndicator,           KHighestRenderingPriority - KPriorityOffset4 },
        { EAiDeviceStatusContentMCNIndicator,           KHighestRenderingPriority - KPriorityOffset5 },
        { EAiDeviceStatusContentGeneralIndicator,       KHighestRenderingPriority - KPriorityOffset6 }
    };


TRenderingPriorities RenderingPriorities( const TDesC8& aUiElementId )
    {
    if( aUiElementId == KNaviPaneId )
        {
        return TRenderingPriorities( KNaviPaneRenderingPriorities, 
            sizeof(KNaviPaneRenderingPriorities) / sizeof(KNaviPaneRenderingPriorities[0]) );
        }
    else
        {
        return TRenderingPriorities( NULL, 0 );
        }
    }


TRenderingPriorities::TRenderingPriorities
        (const TRenderingPriorityItem* aRenderingPriorities, TInt aCount)
    : iRenderingPriorities(aRenderingPriorities), iCount(aCount)
    {
    }
    

TInt TRenderingPriorities::RenderingPriority( TInt aContentId )
    {
    if( !iRenderingPriorities )
        {
        return KErrNotFound;
        }
    for( TInt i = 0; i < iCount; ++i )
        {
        if( iRenderingPriorities[i].iContentId == aContentId )
            {
            return iRenderingPriorities[i].iRenderingPriority;
            }
        }
    return KErrNotFound;
    }


TBool TRenderingPriorities::GetNextContentId( TInt& aContentId, TInt& aNextPriority )
    {    
    if( !iRenderingPriorities )
        {
        return EFalse;
        }
    for( TInt i = 0; i < iCount; ++i )
        {
        if( iRenderingPriorities[i].iRenderingPriority < aNextPriority )
            {
            aContentId = iRenderingPriorities[i].iContentId;
            aNextPriority = iRenderingPriorities[i].iRenderingPriority;
            return ETrue;
            }
        }
    return EFalse;
    }

	}  // namespace AiNativeUi
    
    }
