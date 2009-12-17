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
* Description:  Xuikon volume control adapter observer
*
*/


#ifndef __XNVOLUMECONTROLOBSERVER_H
#define __XNVOLUMECONTROLOBSERVER_H

/**
*  @ingroup group_xnvolumecontrolfactory
*  @lib xnvolumecontrol.lib
*  @since Series 60 3.2
*/
class MXnVolumeControlObserver
    {
    public: // New functions
    
    /**
    * Called when volume value is changed.
    * @param aValue New volume value.
    */
    virtual void VolumeChanged( TInt aValue ) = 0;
    };

#endif // __XNVOLUMECONTROLOBSERVER_H
