/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for navi pane animator callback.
*
*/


#ifndef M_AINAVIPANEANIMATORCALLBACK_H
#define M_AINAVIPANEANIMATORCALLBACK_H

#include <e32base.h>

namespace AiNativeUiController
{


/**
 *  Base class for navi pane animator callback.
 *
 *  @since S60 3.2
 */
class MAiNaviPaneAnimatorCallback
    {

    public:

        /**
         * Animation completed.
         * This method is called when navi pane animation is complete. 
         *
         * @since S60 3.2
         */
        virtual void AnimationCompleteL() = 0;
        
        
    protected:

        /**
         * Protected destructor prevents deletion through this interface.
         */
        MAiNaviPaneAnimatorCallback() {};

    };

} // namespace AiNativeUiController

#endif // M_AINAVIPANEANIMATORCALLBACK_H
