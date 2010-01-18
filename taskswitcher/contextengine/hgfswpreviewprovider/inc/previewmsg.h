/*
* ==============================================================================
*  Name        : %name: previewmsg.h %
*  Part of     : Hg fast swap preview provider plugin
*  Description : Message types for preview plugin
*  Version     : %version: sa1spcx1#2 %
*
*  Copyright (c) 2008 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
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
        EUnregisterReady
        };
    }

#endif // PREVIEWMSG_H
