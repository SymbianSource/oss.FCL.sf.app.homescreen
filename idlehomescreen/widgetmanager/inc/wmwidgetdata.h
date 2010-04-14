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
* CWmWidgetData declaration
*
*/


#ifndef WMWIDGETDATA_H
#define WMWIDGETDATA_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <bamdesca.h>
#include <hscontentinfo.h>

#include "wmimageconverter.h"

// FORWARD DECLARATIONS
class CDictionaryFileStore;
class MWmWidgetDataObserver;
class CFbsBitmap;
class CHsContentInfo;
class CWmPersistentWidgetOrder;
class RWidgetRegistryClientSession;
class CWmResourceLoader;

// CLASS DECLARATION
/**
 * CWmWidgetData is a representation of a widget in CWmListBox. it
 * stores all data required for drawing the item in a list box, and
 * handling the list item.
 */
NONSHARABLE_CLASS( CWmWidgetData )
    : public CBase
    , public MConverterObserver
    {

public: // types

    /** widget type */
    enum TWidgetType
            {
            EUnknown,
            ENative,
            ECps // wrt widgets included in this category
            };

    /** wrt widget type */
    enum TWrtType
            {
            EUnIdentified,
            EWgz,                    
            EWgt
            };

public: // construction
    /**
     * static constructor
     * @param aLogoSize size of logo image.
     * @param aHsContentInfo (takes ownership)
     * @param aRegistryClientSession (does not take ownership)
     */
    static CWmWidgetData* NewL(
            const TSize& aLogoSize,
            CWmResourceLoader& aWmResourceLoader,
            CHsContentInfo* aHsContentInfo,
            RWidgetRegistryClientSession* aRegistryClientSession );

    /**
     * static constructor, leaves object in cleanup stack
     * @param aLogoSize size of logo image.
     * @param aHsContentInfo (takes ownership)
     * @param aRegistryClientSession (does not take ownership)
     */
    static CWmWidgetData* NewLC(
            const TSize& aLogoSize,
            CWmResourceLoader& aWmResourceLoader,
            CHsContentInfo* aHsContentInfo,
            RWidgetRegistryClientSession* aRegistryClientSession );
        
    /** Destructor. */
    ~CWmWidgetData();

private: // private construction
    /** Constructor for performing 1st stage construction */
    CWmWidgetData( const TSize& aLogoSize, 
            CWmResourceLoader& aWmResourceLoader );

    /** 2nd phase constructor */
    void ConstructL(
            CHsContentInfo* aHsContentInfo,
            RWidgetRegistryClientSession* aRegistryClientSession );

    /**
     * Initialisation routine
     * called from ConstructL and whenever the content type changes
     */
    void InitL(
            CHsContentInfo* aHsContentInfo,
            RWidgetRegistryClientSession* aRegistryClientSession );

public: // external handles

    /** current uninstall animation bitmap */
    const CFbsBitmap* AnimationBitmap( const TSize& aSize );

    /** current uninstall animation mask */
    const CFbsBitmap* AnimationMask( const TSize& aSize );
    
    /**
     * @return ETrue if widget is being uninstalled.
     */
    TBool IsUninstalling();
    
    /**
     * @return ETrue if logo is being created,false otherwise.
     */
    TBool IsPrepairingLogo();
    
    /** 
     * sets an observer for callbacks 
     *
     * @param aObserver notifications when changes occur in widget data
     */
    void SetObserver( MWmWidgetDataObserver* aObserver );

    /**
     * persistent widget order handle, which is used in comparing
     */
    void SetPersistentWidgetOrder(
            const CWmPersistentWidgetOrder* aPersistentWidgetOrder );
    
    /**
     * Init logo re-creation
     */
    void ReCreateLogo( const TSize& aSize );
    
    /**
     * returns widget description
     */
    const TDesC& Description() const;
    
    /**
     * Replaces the content of this widget data. The method is called
     * during widget update. If widget values change, a change event is
     * fired for the observer.
     * 
     * @param aHsContentInfo new content (takes ownership)
     * @return ETrue if content was changed
     */
    TBool ReplaceContentInfo(
            CHsContentInfo* aHsContentInfo );
    
public: // comparing / sorting
    /** whether this object represents the same widget as aContentInfo */
    TBool EqualsTo( CHsContentInfo& aContentInfo );
    
    /** comparator function for sort by name (compare two names)*/    
    static TInt CompareByName(
                        const CWmWidgetData& aDataOne,
                        const CWmWidgetData& aDataTwo );

    /**
     * comparator function for sorting by stored order
     **/
    static TInt CompareByPersistentWidgetOrder(
            const CWmWidgetData& aDataOne,
            const CWmWidgetData& aDataTwo );

public: // methods to read the content

    /** Mdcapoint for listbox("\t0widget name") */    
    const TDesC& MdcaPoint() const;
    
    /** the HsContent info */
    inline CHsContentInfo& HsContentInfo();

    /** the HsContent info */
    inline const CHsContentInfo& HsContentInfo() const;

    /** widget name */
    const TDesC& Name() const;

    /** widget uid */
    inline TUid Uid() const;
    
    /** cps widget publisher id */
    inline const TDesC& PublisherId() const;
    
    /** cps widget publisher uid */
    TUid PublisherUid();
    
    /** widget type */
    inline TWidgetType WidgetType() const;

    /** wrt widget type */
    inline TWrtType WrtType() const;
    
    /** running install animation index */
    inline TInt InstallAnimationIndex() const;

    /** the logo bitmap */
    inline const CFbsBitmap* LogoImage();

    /** the logo bitmap */
    inline const CFbsBitmap* LogoImageMask();

    /** whether the widget data is valid or not */
    inline TBool IsValid();
    
    /** sets the widget data validity */
    inline void SetValid( TBool aValid );
    
	/** start animation for uninstallation*/
    void VisualizeUninstallL();
	
    /** stop uninstallation animation */
    void StopUninstallAnimation();
    
protected: // from MConverterObserver

    /** image conversin completed */
    void NotifyCompletion( TInt aError );

private: // new functions
    
    /** uninstall animation related*/
    void PrepairAnimL();
    void DestroyAnimData();
    static TInt AnimationTick( TAny* aPtr );
    
    /** fetches publisher uid from widget registry*/
    void FetchPublisherUidL( 
            const TDesC& aPublisherId,
            RWidgetRegistryClientSession* aRegistryClientSession );

    /** Converts uid to TUid from TDesC8 */
    TUid UidFromString( const TDesC8& aUidString ) const;
    
    /** Logo icon string handling */
    void HandleIconString( const TDesC& aIconStr );
    void FireDataChanged();
    static TInt TimeoutTick( TAny* aPtr );

private: // data members
    
    /* reference to resource loader */
    CWmResourceLoader& iWmResourceLoader;
    
    /* the image converter utility */
    CWmImageConverter*    iImageConverter;

    /* observes this widget representation (NOT OWNED) */
    MWmWidgetDataObserver* iObserver;

    /** the widget logo bitmap */
    CFbsBitmap*         iLogoImage;

    /** the widget logo mask */
    CFbsBitmap*         iLogoImageMask;

    /** The CHsContentInfo that corresponds to this list row */
    CHsContentInfo*     iHsContentInfo;

    /** type of the widget */
    TWidgetType         iWidgetType;
    
    /** type of the wrt widget */    
    TWrtType            iWrtType;

    /** persistent order of widgets (used in sorting) */
    const CWmPersistentWidgetOrder* iPersistentWidgetOrder;
    
    /** "0\t" + widget name for listbox*/
    HBufC*              iMdcaPoint;
    
    /** wrt widget publisher uid */
    TUid                iPublisherUid;
    
    /** size of logo rect in list item */
    TSize               iLogoSize;
    
    /** validity of the widget - used during list refresh */
    TBool               iValid;

    /* Array of uninstall animation bitmaps*/
    RArray<CFbsBitmap*> iUninstallAnimIcons;
    
    /** timer for updating animation */
    CPeriodic*          iAnimationTimer;
    
    /** timer for canceling image convertion */
    CPeriodic*          iTimeoutTimer;
    
    /* uninstall animation index */
    TInt                iAnimationIndex;

    /** uninstallation switch */
    TBool               iAsyncUninstalling;
    
    /** logo changed switch */
    TBool               iFireLogoChanged;

    /**
     * ActiveSchedulerWait used to wait while logo image
     * is being prepaired.
     */
    CActiveSchedulerWait* iWait;
    
    /**
     * Holds widget name. Used for restoring widget name 
     * if error has occurred during uninstallation.
     */
    HBufC* iWidgetName;
    };


/**
 * Pointer array of widget data objects.
 * Also acts as an adapter to a descriptor array, providing the
 * widget names as descriptor array elements.
 */
NONSHARABLE_CLASS( RWidgetDataValues ) : public RPointerArray<CWmWidgetData>,
    public MDesCArray
    {
    protected: // from MDesCArray
        /** returns number of items in the pointer array */
        inline TInt MdcaCount() const;

        /** returns the pointed widget name */
        inline TPtrC16 MdcaPoint(TInt aIndex) const;
    };

#include "wmwidgetdata.inl"
#endif // WMWIDGETDATA_H
