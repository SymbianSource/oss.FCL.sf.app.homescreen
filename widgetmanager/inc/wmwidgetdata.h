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

public: // construction
    /**
     * Two-phased constructor.
     * @param aHsContentInfo
     * @param aRegistryClientSession  
     */
    static CWmWidgetData* NewL( 
            CHsContentInfo& aHsContentInfo,
            RWidgetRegistryClientSession* aRegistryClientSession );

    /**
     * Two-phased constructor.
     * @param aHsContentInfo
     * @param aRegistryClientSession  
     */
    static CWmWidgetData* NewLC( 
            CHsContentInfo& aHsContentInfo,
            RWidgetRegistryClientSession* aRegistryClientSession );
        
    /** Destructor. */
    ~CWmWidgetData();

private: // private construction
    /** Constructor for performing 1st stage construction */
    CWmWidgetData();

    /** 2nd phase constructor */
    void ConstructL( 
            CHsContentInfo& aHsContentInfo,
            RWidgetRegistryClientSession* aRegistryClientSession );
     
public: // external handles

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
     * set logo rect size
     */
    void SetLogoSize( const TSize& aSize );

    /**
     * Init logo re-creation
     */
    void ReCreateLogo( const TSize& aSize );
    
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
    
    /** the HsContent info */
    inline CHsContentInfo& HsContentInfo();

    /** the HsContent info */
    inline const CHsContentInfo& HsContentInfo() const;

    /** widget name */
    inline const TDesC& Name() const;

    /** widget description */
    inline const TDesC& Description() const;

    /** widget uid */
    inline TUid Uid() const;
    
    /** cps widget publisher id */
    inline const TDesC& PublisherId() const;
    
    /** cps widget publisher uid */
    TUid PublisherUid();
    
    /** widget type */
    inline TWidgetType WidgetType() const;

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
    
protected: // from MConverterObserver

    /** image conversin completed */
    void NotifyCompletion( TInt aError );

private: // new functions

    /** fetches publisher uid from widget registry*/
    void FetchPublisherUidL( 
            RWidgetRegistryClientSession* aRegistryClientSession );

    /** Converts uid to TUid from TDesC8 */
    TUid UidFromString( const TDesC8& aUidString ) const;
    
    /** Logo icon string handling */
    static TInt HandleAsyncIconString( TAny* aPtr );
    void HandleIconStringL( const TDesC& aIconStr );
    void FireDataChanged();

private: // data members
    
    /* instance of the CIdle class for async iconStr handling*/
    CIdle*              iIdle;

    /* the image converter utility */
    CWmImageConverter*    iImageConverter;

    /* observes this widget representation (NOT OWNED) */
    MWmWidgetDataObserver* iObserver;

    /* rotating animation index */
    TInt                iInstallAnimationIndex;

    /** the widget logo bitmap */
    CFbsBitmap*         iLogoImage;

    /** the widget logo mask */
    CFbsBitmap*         iLogoImageMask;
    
    /** The CHsContentInfo that corresponds to this list row */
    CHsContentInfo*     iHsContentInfo;

    /** type oif the widget */
    TWidgetType         iWidgetType;

    /** persistent order of widgets (used in sorting) */
    const CWmPersistentWidgetOrder* iPersistentWidgetOrder;

    /** wrt widget publisher uid */
    TUid                iPublisherUid;
    
    /** size of logo rect in list item */
    TSize               iLogoSize;
    
    /** validity of the widget - used during list refresh */
    TBool               iValid;
    
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
