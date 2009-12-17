/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMmPostEvaluationProcessor declaration
* Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef C_MMPOSTEVALUATIONPROCESSOR_H
#define C_MMPOSTEVALUATIONPROCESSOR_H

#include <e32base.h>
#include <e32std.h>

class CMmListBoxItemDrawer;

/**
 *  Used to performa additional post evaluation operations connected with
 *  the ui, and preparation of a suite display from the mmwidgets side.
 *  All the operation are performed in an active object, since they
 *  are not time crucial, and are performed to adjust performance:
 *  such as preloading suite icons that are not visible in the current view.
 *
 *  @code
 *  @endcode
 *  @lib mmwidgets
 *  @since S60 v5.0
 *  @see CListBoxItemDrawer
 *  @ingroup group_mmwidgets
 */
NONSHARABLE_CLASS( CMmPostEvaluationProcessor ) : public CActive
    {
    
public:
    /**
     * Two-phased constructor.
     * 
     * @since S60 v5.0
     * @param aDrawer Item drawer used to draw item.
     */
    static CMmPostEvaluationProcessor* NewL( CMmListBoxItemDrawer& aDrawer );

    /**
     * Two-phased constructor.
     * 
     * @since S60 v5.0
     * @param aDrawer Item drawer used to draw item.
     */
    static CMmPostEvaluationProcessor* NewLC( CMmListBoxItemDrawer& aDrawer );
    
    /**
     * Destructor.
     * 
     * @since S60 v5.0
     */
    virtual ~CMmPostEvaluationProcessor();

    /**
     * Starts at predefiend index.
     *
     * @param aIndex Starts processing at index.
     * @since S60 v3.0
     */
    void StartAt( TInt aIndex = 0 );
        
private:
	
    /**
     * Default constructor.
     * 
     * @since S60 v3.0
     * @param aDrawer Object used to draw items in widgets.
     */
    CMmPostEvaluationProcessor( CMmListBoxItemDrawer& aDrawer );
    
    /**
     * 2nd phase constructor.
     *
     * @since S60 v3.0 
     */
    void ConstructL();
    
    

private: // From CActive

    /**
     * Draws next animation phase.
     *
     * @since S60 v3.0
     */
    void RunL();

    /**
     * Cancels animation timer. Stops animation.
     *
     * @since S60 v3.0
     */
    void DoCancel();

    /**
     * Handles error if error an error occured.
     *
     * @since S60 v3.0
     * @param aError Error code which occurred.
     * @return Error code for error handling.
     */
    TInt RunError( TInt aError );

private: // Data
	
    /**
    * Drawer to use for preload.
	 
    */
    CMmListBoxItemDrawer & iDrawer;
	
    /**
     * Index to be processed in next RunL.
     
    */
    TInt iIndexToProcess;
	
    };

#endif // MMDRAWERANIMATOR_H
