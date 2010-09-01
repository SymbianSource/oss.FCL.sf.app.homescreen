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
 * Description:  
 *
 */

#ifndef HSCONTENTCONTROLUI_H
#define HSCONTENTCONTROLUI_H

// System includes
#include <e32base.h>
#include <ecom/ecom.h>

// Forward declarations
class MHsContentController;
class CAknView;

/**
 *  Content control UI interface
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hscontentcontrol.lib
 *  @since S60 v5.0
 */
class MHsContentControlUi
    {
public:
    /**
     *
     */
    virtual void Activate() = 0;

    /**
     *
     */
    virtual void DeActivate() = 0;

    /**
    *
    */    
    virtual void Views( RPointerArray<CAknView>& aViews ) = 0;
    
    /**
     *
     */
    virtual void NotifyWidgetListChanged() = 0;

    /**
     *
     */
    virtual void NotifyViewListChanged() = 0;

    /**
     *
     */
    virtual void NotifyAppListChanged() = 0;
    
    /**
    *
    */
    virtual void SetContentController( MHsContentController* aController ) = 0;
    };

/**
 *  Content control UI base class
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hscontentcontrol.lib
 *  @since S60 v5.0
 */
class CHsContentControlUi : public CBase, public MHsContentControlUi
    {
public:
    /**
     * Two-phased constructor.
     * @param aImplUid Requested implementation UID
     */
    IMPORT_C static CHsContentControlUi* NewL(
        TUid aImplUid );

    /**
     * Destructor.
     */
    inline ~CHsContentControlUi();

public:  // From MHsContentControlUi
    /**
     *
     */
    inline void Activate();

    /**
     *
     */
    inline void DeActivate();

    /**
     *
     */
    inline void Views( RPointerArray<CAknView>& aViews );
    
    /**
     *
     */
    inline void NotifyWidgetListChanged();

    /**
     *
     */
    inline void NotifyViewListChanged();

    /**
     *
     */
    inline void NotifyAppListChanged();
    
    /**
     *
     */
    void SetContentController(
        MHsContentController* aController );
    

public:  // New functions

    /**
     *
     */
    TUid ImplUid() const;

    /**
     *
     */
    void SetContentControlTypeL(
        const TDesC8& aContentControlType );

    /**
     *
     */
    const TDesC8& ContentControlType() const;

protected: // Data

    /**
     *
     */
    MHsContentController* iHsContentController;

private: // Data
    
    /**
     *
     */
    TUid iDestructKey;
    
    /**
     *
     */
    TUid iImplUid;

    /**
     *
     */
    HBufC8* iContentControlType;

    };

inline CHsContentControlUi::~CHsContentControlUi()
    {
    delete iContentControlType;
    REComSession::DestroyedImplementation( iDestructKey );
    }

inline void CHsContentControlUi::Activate()
    {
    // Default implementation of MHsContentControlUi method
    }
inline void CHsContentControlUi::DeActivate()
    {
    // Default implementation of MHsContentControlUi method
    }
inline void CHsContentControlUi::Views( 
        RPointerArray<CAknView>& /*aViews*/ )
    {
    // Default implementation of MHsContentControlUi method
    }
inline void CHsContentControlUi::NotifyWidgetListChanged()
    {
    // Default implementation of MHsContentControlUi method
    }
inline void CHsContentControlUi::NotifyViewListChanged()
    {
    // Default implementation of MHsContentControlUi method
    }
inline void CHsContentControlUi::NotifyAppListChanged()
    {
    // Default implementation of MHsContentControlUi method
    }

inline void CHsContentControlUi::SetContentController(
    MHsContentController* aController )
    {
    iHsContentController = aController;
    }

#endif // HSCONTENTCONTROLUI_H
// End of file
