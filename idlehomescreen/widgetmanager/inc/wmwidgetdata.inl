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
* CWmWidgetData , RWidgetDataValues inline function definitions
*
*/

#ifndef WMWIDGETDATA_INL_
#define WMWIDGETDATA_INL_

    /** the HsContent info */
    inline CHsContentInfo& CWmWidgetData::HsContentInfo()
        { return *iHsContentInfo; }

    /** the HsContent info */
    inline const CHsContentInfo& CWmWidgetData::HsContentInfo() const
        { return *iHsContentInfo; }

    /** widget uid */
    inline TUid CWmWidgetData::Uid() const
        { return UidFromString( iHsContentInfo->Uid() ); };
    
    /** cps widget publisher id */
    inline const TDesC& CWmWidgetData::PublisherId() const
        { return iHsContentInfo->PublisherId(); }
    
    /** widget type */
    inline CWmWidgetData::TWidgetType CWmWidgetData::WidgetType() const
        { return iWidgetType; }

    /** wrt widget type */
    inline CWmWidgetData::TWrtType CWmWidgetData::WrtType() const
        { return iWrtType; }
    
    /** the logo bitmap */
    inline const CFbsBitmap* CWmWidgetData::LogoImage()
        { return iLogoImage; }

    /** the logo bitmap */
    inline const CFbsBitmap* CWmWidgetData::LogoImageMask()
        { return iLogoImageMask; }
  
    /** whether the widget data is valid or not */
    inline TBool CWmWidgetData::IsValid()
        { return iValid; }
    
    /** sets the widget data validity */
    inline void CWmWidgetData::SetValid( TBool aValid )
        { iValid = aValid; }
    
    //From  RWidgetDataValues
    
    /** returns number of items in the pointer array */
    inline TInt RWidgetDataValues::MdcaCount() const
        {
        return Count();
        }
    /** returns the pointed widget name */
    inline TPtrC16 RWidgetDataValues::MdcaPoint(TInt aIndex) const
        {
        const TPtrC16 ptr( (*this)[aIndex]->MdcaPoint() );
        return ptr;
        }

#endif /* WMWIDGETDATA_INL_ */
