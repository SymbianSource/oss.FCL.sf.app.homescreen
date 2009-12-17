/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Menu Framework extension plugin
*
*/

#ifndef MMDIALOGMANAGER_H
#define MMDIALOGMANAGER_H

#include <AknQueryDialog.h>

class MMmDialogManager
    {
public:
    /**
     * Setter for dialog.
     * @param aDialog Dialog instance.
     * @return Result of the dialog.
     */
    virtual void SetDialogL( CAknQueryDialog* aDialog ) = 0;
    
    /**
     * Removes active dialog.
     */
    virtual void RemoveDialogL() = 0;
    };

#endif /* MMDIALOGMANAGER_H */

// end of file

