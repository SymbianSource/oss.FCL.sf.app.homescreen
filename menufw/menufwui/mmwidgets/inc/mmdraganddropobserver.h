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
* Description:  receives key pressed notifications
*
*/


#ifndef M_MMDRAGANDDROPOBSERVER_H
#define M_MMDRAGANDDROPOBSERVER_H

/**
 *  Used by Multimedia Menu widgets to notify about dragging events.
 *  Whenever a drag begins HandleDragStartL() is invoked.
 *  When the highlight changes during drag, HandleDragOverL() is invoked.
 *  When item is dropped, HandleDragStopL() is invoked.
 *  The set of the three methods fully notifies the observer about the state
 *  of the drag.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
class MMmDragAndDropObserver
    {
public:
    /**
     * Method is invoked on the observer, when the dragging starts.
     *
     * @since S60 v5.0
     * @param aModelItemIndex Index of item that has started dragging.
     */
    virtual void HandleDragStartL( TInt aModelItemIndex ) = 0;

    /**
     * Method is invoked on the observer, when highlight changes
     * while dragging.
     *
     * @since S60 v5.0
     * @param aModelItemIndex Index of item that the dragged item hovers over.
     */
    virtual void HandleDragOverL( TInt aModelItemIndex ) = 0;

    /**
     * Method is invoked on the observer, when the item is dropped.
     *
     * @since S60 v5.0
     * @param aModelItemIndex Index of item on which the draging stops.
     */
    virtual void HandleDragStopL( TInt aModelItemIndex ) = 0;

    /**
     * Method is invoked on the observer, when the dragged item index changes.
     *
     * @since S60 v5.0
     * @param aModelItemIndex Index of item on which the draging stops.
     */
    virtual void HandleDraggedIndexUpdatedL( TInt aModelItemIndex ) = 0;

    /**
     * Method is invoked on the observer, when the move item event is needed.
     *
     * @since S60 v5.0
     * @param aRecipientId Index of item on which the move event invokes.
     * @param aEventParameters Event parameters.
     */
    virtual void HandleTriggerMoveItemL( const TInt aRecipientId,
                CLiwGenericParamList* aEventParameters) = 0;

    };

#endif // M_MMDRAGANDDROPOBSERVER_H
