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
* Description:  highlight timer class
*  Version     : %version: MM_11.1.4 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMHIGHLIGHTTIMER_H
#define C_MMHIGHLIGHTTIMER_H

#include <e32base.h>
#include <e32std.h>

class CMmWidgetContainer;

/**
 *  Used by Multimedia Menu to handle highlight behaviour.
 *  If the timer is set up highlight is due to disappera after set up
 *  time.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v3.0
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmHighlightTimer ) : public CActive
    {
public:
    /**
     * Two-phased constructor.
     * 
     * @since S60 v3.0
     * @param aContainer Widget container that will use timer.
     * @return Instance of highlight timer.
     */
    IMPORT_C static CMmHighlightTimer* NewL( 
            CMmWidgetContainer* aContainer );
    
    /**
     * Two-phased constructor.
     * 
     * @since S60 v3.0
     * @param aContainer Widget container that will use timer.
     * @return Instance of highlight timer.
     */
    IMPORT_C static CMmHighlightTimer* NewLC( 
            CMmWidgetContainer* aContainer );

    /**
     * Destructor.
     * 
     * @since S60 v3.0
     */
    virtual ~CMmHighlightTimer();

    /**
     * Starts the timer with the time specified. Also draws the highlight.
     * Highlight timer is the only element eligible to draw highlight with single click.
     * If aTime parameter is negative or null the highlight is set permanently until it is 
     * cancelled manually (e.g. by calling StopL). 
     *
     * @since S60 v5.0
     * @param aTime Time after which timer expires.
     * @param aPressDownState The highlight state to be drawn. Default is highlight pressed down.
     */
    void StartL( TInt aTime, TBool aPressDownState = ETrue );
    
    /**
     * Continues the highlight visibility for a specific time.
     * If highlight wasn't active (visible) then this method does nothing.
     *
     * @since S60 v5.0
     * @param aTime Time after which timer expires.
     */
    void ContinueL( TInt aTime );
    
    /**
     * Cancels the asynchronous request and draws over the highlighted item (if told to).
     *
     * @since S60 v5.0
     * @param aDrawOverHighlight Tells if previous highlight should be drawn over with no highlight.
     */
    void StopL( TBool aDrawOverHighlight = ETrue );
    
    
    /**
     * Returns the previously drawn highlight index.
     *
     * @since S60 v5.0
     * @return Previously drawn highlight index.
     */
    TInt TimerHighlightIndex() const;

private:
	
    /**
     * Constructor.
     * 
     * @since S60 v3.0
     * @param aContainer The widget for which highlight is handled by the timer.
     */
    CMmHighlightTimer( CMmWidgetContainer* aContainer );
    
    /**
     * 2nd phase constructor.
     * 
     * @since S60 v3.0
     */
    void ConstructL();

    /**
     * Disables highlight and redraws the view.
     *
     * @since S60 v3.0
     */
    void RunL();
    
    /**
     * Cancels timer.
     *
     * @since S60 v3.0
     */
    void DoCancel();
    
    /**
     * Run if error occurred.
     *
     * @since S60 v3.0
     * @param aError Error for which error handling should be run.
     * @return Error code.
     */
    TInt RunError( TInt aError );
    
    /**
     * Draws the highlight.
     *
     * @since S60 v5.0
     * @param aItemIndex The index of the highlight.
     */
    void DrawHighlightL( TInt aItemIndex );
    
    /**
     * Sets press down state on widget drawer to draw highlight as told.
     *
     * @since S60 v5.0
     * @param aPressDownState ETrue to set press down state. Otherwise press down state is disabled.
     */
    void SetPressDownState(TBool aPressDownState);
    
private:
	
    /**
     * Defines states of the CMmHighlightTimer.
     */    
    enum THighlightTimerState
        {
        EInactivityState,
        ETimerExpirationCompletionState,
        EExternalCompletionState
        };
    
    /**
     * Actual state of the CMmHighlightTimer.
     */
    TInt iState;
    
    /**
     * Timer class. CMmHighlightTimer requests notifications
     * from this member every 6 seconds.
     */
    RTimer iTimer;
    
    /**
     * Manages highlight. Receives notifications.
     * Not own.
     */
    CMmWidgetContainer* iContainer;
    
    /**
     * Index of drawn item.
     */
    TInt iTimerHighlightIndex;
    
    };
    
#endif // C_MMHIGHLIGHTTIMER_H
    
