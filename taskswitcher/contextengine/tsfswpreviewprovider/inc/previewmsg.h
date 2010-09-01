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
* Description:  Message types for preview plugin
*
*/


#ifndef PREVIEWMSG_H
#define PREVIEWMSG_H

namespace NPreviewMsg
    {
    /**
     * Messages that are sent between PreviewProviderClient and PreviewProvider
     * ECOM plugin via window server. 
     */
    enum 
        {
        // From PreviewProviderClient to PreviewProvider ECOM plugin. 
        ENone,
        ERegister,
        EUnregister,
        EChangeScreen,
        ESetPreviewParam,
        ETakePreview,
        
        // From PreviewProvider ECOM plugin to PreviewProviderClient. 
        EPreviewReady,
        EAckPreviewReady,
        EUnregisterReady,
        EBitmapRotationNeeded90,
        EBitmapRotationNeeded270
        };
    }

#endif // PREVIEWMSG_H
