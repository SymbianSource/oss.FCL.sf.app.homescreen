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
* Description:  Handles of drawing text on navi and title panes.
*
*/


#ifndef C_AISTATUSPANEL_H
#define C_AISTATUSPANEL_H


#include <e32base.h>
#include <telephonydomainpstypes.h>

class CEikStatusPane;
class CAknTitlePane;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class MAknNaviDecoratorObserver;
class MAknTitlePaneObserver;

namespace AiNativeUiController
{

/**
 *  This class is used to draw text on navi and title pane.
 *  And to draw bitmap on title pane.
 *
 *  @since S60 3.2
 */
class CAiStatusPanel : public CBase
    {
public:

    static CAiStatusPanel* NewL();

    virtual ~CAiStatusPanel();

    /**
     * Get size for oplogo based on titlepane layout
     * @param aSize Operator logo size.
     * @return Error code.
     */
     TInt GetTitlePaneSize( TSize& aSize );

    /**
     * Updates operator indicator of phone application.
     * @param aDisplayInfo display info of operator indicator.
     */
     void UpdatePhoneDisplayInfo( const TTelTitleDisplay& aDisplayInfo );

     /**
      * Store text for navi pane.
      * Navi pane is not rendered until RenderNaviPaneL is called.
      *
      * @param aText Text for navi pane.
      */
     void SetNaviPaneTextL( const TDesC& aText );

     /**
      * Store text for title pane.
      * Title pane is not rendered until RenderTitlePaneL is called.
      *
      * @param aText Text for title pane.
      */
     void SetTitlePaneTextL( const TDesC& aText );

     /**
      * Store bitmap that is later set to title pane.
      * Title pane is not rendered until RenderTitlePaneL is called.
      *
      * @param aBitmapHandle Handle for bitmap.
      * @param aMaskHandle Handle for mask.
      */
     void SetTitlePaneBitmapL( TInt aBitmapHandle, TInt aMaskHandle );

     /**
      * Draw stored data to navi pane.
      */
     void RenderNaviPaneL();

     /**
      * Draw stored data to title pane.
      */
     void RenderTitlePaneL();

     /**
      * Stop titlepane text scrolling.
      */
     void StopTitlePaneScrollingL();

     /**
     * Resume titlepane text scrolling
     */
     void ScrollTitlePaneTextL();

     /**
      * Set navi decorator observer
      * @param aObserver navi decorator observer
      */
     void SetNaviDecoratorObserver( MAknNaviDecoratorObserver* aObserver );

     /**
      * Set title pane observer
      * @param aObserver title pane observer
      */
     void SetTitlePaneObserver( MAknTitlePaneObserver* aObserver );

     /**
      * Check keylock status
      */
     TBool IsKeyLockEnabled();


private:

    CAiStatusPanel();

    void ConstructL();

    /**
     * Triggers text rendering on Navi pane
     */
    void RenderTextOnNaviPaneL();

    /**
     * Triggers text rendering on Title pane
     */
    void RenderTextOnTitlePaneL();

    /**
     * Triggers image rendering on Title pane
     */
    void RenderBitmapOnTitlePaneL();



private: // data

    /**
     * Pointer to status pane.
     * Not own.
     */
    CEikStatusPane* iStatusPane;

    /**
     * Pointer to title pane.
     * Not own.
     */
    CAknTitlePane* iTitlePane;

    /**
     * Pointer to navi pane.
     * Not own.
     */
    CAknNavigationControlContainer* iNaviPane;

    /**
     * Pointer to navi decorator.
     * Own.
     */
    CAknNavigationDecorator* iNaviDecorator;

    /**
     * Telephony display information.
     * Not own.
     */
    TTelTitleDisplay iDisplayInfo;

    /**
     * Text that is written to navi pane.
     * Own.
     */
     HBufC* iNaviPaneText;

    /**
     * Text that is written to title pane.
     * Own if text is not rendered. See iTitlePaneTextTransferred member.
     */
     HBufC* iTitlePaneText;

    /**
     * Bitmap handle for setting image to title pane.
     */
     TInt iBitmapHandle;

    /**
     * Mask handle for setting image to title pane.
     */
     TInt iMaskHandle;

    /**
     *
     */
     MAknNaviDecoratorObserver* iNaviDecoratorObserver;

    };

} // namespace AiNativeUiController

#endif // C_AISTATUSPANEL_H
