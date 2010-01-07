/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies)..
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
* Description:
* Observer for changes in the widget data
*
*/


#ifndef WMWIDGETDATAOBSERVER_H
#define WMWIDGETDATAOBSERVER_H

// INCLUDES
#include <e32std.h>

// Forward declarations
class CWmWidgetData;

// CLASS DECLARATION

/**
 * Observer class for receiving notifications
 * when changes occur in widget data
 */
NONSHARABLE_CLASS( MWmWidgetDataObserver )
    {
    public:
        /**
         * A single widget data has changed.
         * @param aWidgetData pointer to the data where change
         *        has occurred
         */
        virtual void HandleWidgetDataChanged(
                            CWmWidgetData* aWidgetData ) = 0;

    };

#endif // WMWIDGETDATAOBSERVER_H
