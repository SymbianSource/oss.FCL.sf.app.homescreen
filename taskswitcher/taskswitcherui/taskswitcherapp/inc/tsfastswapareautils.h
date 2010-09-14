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
* Description:  Fast swap area utilities classes
 *
*/

#ifndef TSFASTSWAPAREAUTILS_H_
#define TSFASTSWAPAREAUTILS_H_

#include <e32base.h>

// Forwar class declarations
class CEikonEnv;
class TAknLayoutRect;
class CTsFastSwapTimer;
class CTsFastSwapArea;
class CTsFastSwapGrid;
class CTsFswEntry;
class CBitmapScaler;
class CTsFastSwapAreaExtension;


/**
 * Interface for observing timer class.
 */
class MTsFastSwapTimerObserver
    {
public:
    /**
     * Called when timer is completed
     */
    virtual void TimerCompletedL( CTsFastSwapTimer* aSource ) = 0;
    };



/**
 * Timer class for handling timed events
 */
class CTsFastSwapTimer : public CTimer
    {
public:
    // Constructor
    CTsFastSwapTimer( MTsFastSwapTimerObserver& aObserver );
    // Destructor
    ~CTsFastSwapTimer();
    // 2nd phase constructor
    void ConstructL();
    
private: // From CTimer
    void RunL();
    
private: // Data
    MTsFastSwapTimerObserver* iObserver; // not own
    };



/**
 * Asynchronous class for handling screenshots loading
 * and rescaling to achieve better quality. 
 */
class CTsBitmapScaleTask : public CActive
    {
public:
    CTsBitmapScaleTask( CTsFastSwapAreaExtension& aObserver );
    ~CTsBitmapScaleTask();
    void StartLD( CFbsBitmap* aSourceBitmap,
                  CFbsBitmap* aTargetBitmap );
private:
    void RunL();
    void DoCancel();
private:
    // Data
    CTsFastSwapAreaExtension& iObserver;
    CBitmapScaler* iScaler; // owned
    CFbsBitmap* iSourceBmp; // owned
    };



/**
 * Utility class containg methods for fast swap area handling.
 */
class CTsFastSwapAreaExtension: public CBase
    {

public:
    
    /**
     * Two-phase constructor.
     * 
     * @param aRect Area where fast swap info & list should draw itself.
     * @param aParent Parent CoeControl. 
     */
    static CTsFastSwapAreaExtension* NewL( CTsFastSwapArea& aFastSwapArea,
                                       CEikonEnv& aEnv);

    /**
     * @copydoc NewL
     */
    static CTsFastSwapAreaExtension* NewLC( CTsFastSwapArea& aFastSwapArea,
                                        CEikonEnv& aEnv );

    /**
     * Destructor.
     */
    ~CTsFastSwapAreaExtension();
    
private:

    /**
     * @copydoc NewL
     */
    void ConstructL();

    /**
     * C++ constructor.
     */
    CTsFastSwapAreaExtension( CTsFastSwapArea& aFastSwapArea,
                          CEikonEnv& aEnv );
    
public:
    
    /**
     * Returns current screen orientation:
     * 
     * @return  1 if landscape, 0 if portait 
     */
    TInt GetCurrentScreenOrientation();
    
    /**
     * Retrieves variety value, based on current screen resolution.
     * 
     * @param  aVariety  result of the function, 0 value means portrait
     *                   value of 1 indicates landscape
     * @return  ETrue if screen resolution differs from vale returned by
     *          layout meta data functions.
     */
    TBool GetVariety( TInt& aVariety );
    
    /**
     * Returns rectangles for fast swap area controls
     */
    void GetFastSwapAreaRects( RArray<TAknLayoutRect>& aRects );
    
    /**
     * Returns the position of the given item inside grid world rect.
     * Returns point in the middle of the view rectangle.
     * 
     * @param  aItemIdx  index of the item for which calculation will be done
     */
    TPoint ItemViewPosition( TInt aItemIdx );
    
    /**
     * Returns index of the shown item, based on the logical
     * position of the grids view. Item that is closest to the
     * middle of screen is returned.
     * 
     * @param  aViewPos  grids view position. This must be point in
     *                   the middle of the view rectangle
     * @return  index of the item
     */
    TInt ViewToVisibleItem( const TPoint aViewPos );
    
    /**
     * Launches increasing pop-up feedback.
     */
    void LaunchPopupFeedback();
    
    /**
     * Retrieves and returns size for image graphics.
     */
    TSize PreferredImageSize();
    
    /**
     * Calculates the correct size if aspect ratio needs to be preserved.
     */
    TSize CalculateSizePreserveRatio(const TSize& aTargetAreaSize,
            const TSize& aSourceSize);
    
    /**
     * Makes a copy of the given bitmap.
     * Also scaled to the given size, but maintains aspect ratio,
     * so the size of the returned bitmap may be less then aSize.
     */
    CFbsBitmap* CopyBitmapL( TInt aFbsHandle, TSize aSize, TBool aIsScreenshot );
    
    /**
     * Cancels ongoing scale tasks
     */
    void CancelScaleTasks();
    
    /**
     * Callback for updating task list.
     */
    void ScaleFinished( CTsBitmapScaleTask* iFinishedTask );
    
private: // Data
    
    CTsFastSwapArea& iFastSwapArea;
    
    CEikonEnv& iEnv;
    
    RPointerArray<CTsBitmapScaleTask> iScaleTasks;
    
    };

#endif /* TSFASTSWAPAREAUTILS_H_ */
