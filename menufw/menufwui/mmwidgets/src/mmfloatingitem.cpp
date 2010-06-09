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
* Description:  floating item
*
*/

#include <e32math.h>
#include <eiklbv.h>
#include "mmfloatingitem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TMmFloatingItem::TMmFloatingItem( TInt aDrawnIndex, TPoint aStartPosition,
        TMmFloatingItemType aType, TInt aAnimationFrames, CListBoxView* aView )
    {
    iView = aView;
    iSizeStep = 0;
    iZoomRatio = 1;
    iManualDelete = EFalse;

    if ( iView )
        {
        // item position is relative
        iItemPosition = aStartPosition - iView->ItemPos( 0 );
        }
    else
        {
        // item position is absolute
        iItemPosition = aStartPosition;
        }

    iDrawnItemIndex = aDrawnIndex;
    iType = aType;
    iFrames = aAnimationFrames;
    iFrameCounter = 0;
    CalculateSteps(TPoint(0,0));
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TMmFloatingItem::SetPositionStep( TPoint aDiffetenceVector  )
     {
     ASSERT( iFrames );
     iFrameCounter = 0;
     CalculateSteps( aDiffetenceVector );
     }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TMmFloatingItem::CalculateSteps(TPoint aVector)
    {
    iPositionStep.Reset();

    TReal x = aVector.iX;
    TReal y = aVector.iY;

    TReal sx = 0.0;
    TReal sy = 0.0;

    for( TInt i = 0; i < iFrames; i++ )
      {
      TReal sin;
      Math::Sin( sin, (i+1) * KPi/iFrames + KPi * 3./2.  );
      TReal xx = x * (0.5*( sin + 1.0 ) ) - sx;
      TReal yy = y * (0.5*( sin + 1.0 ) ) - sy;

      sx += xx;
      sy += yy;

      iPositionStep.At(i) = TPoint( xx, yy );
      }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TMmFloatingItem::SetSizeStep( TReal aStartSize, TReal aFinalSize )
    {
    ASSERT( iFrames );
    iFrameCounter = 0;
    iZoomRatio = aStartSize;
    iSizeStep = (aFinalSize - aStartSize) / iFrames;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TBool TMmFloatingItem::MakeStep()
    {
    TBool ret(EFalse);

    if( iFrameCounter >= iFrames && iManualDelete == EFalse )
        {
        InvalidateFloatingItem();
        }
    else if( iFrameCounter < iFrames )
        {
        iItemPosition += iPositionStep[iFrameCounter];
        iZoomRatio += iSizeStep;
        ret = Abs(iPositionStep[iFrameCounter].iX) > 0
                || Abs(iPositionStep[iFrameCounter].iY > 0)
                || Abs(iSizeStep) > 0;
        iFrameCounter++;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TMmFloatingItem::SetManualDelete(TBool aManual)
    {
    iManualDelete = aManual;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TReal TMmFloatingItem::GetCurrentZoomRatio() const
    {
    return iZoomRatio;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TMmFloatingItemType TMmFloatingItem::GetFloatingItemType() const
    {
    return iType;
    }

TInt TMmFloatingItem::GetDrawnItemIndex() const
    {
    return iDrawnItemIndex;
    }

TPoint TMmFloatingItem::GetItemPosition() const
    {
    TPoint ret;
    if ( iView )
        {
        // item position is relative
        ret = iItemPosition + iView->ItemPos( 0 );
        }
    else
        {
        // item position is absolute
        ret = iItemPosition;
        }
    return ret;
    }

void TMmFloatingItem::InvalidateFloatingItem()
    {
    iDrawnItemIndex = KErrNotFound;
    }

TBool TMmFloatingItem::IsFloatingItemValid() const
    {
    return GetDrawnItemIndex() != KErrNotFound
    && iType != EPostDragRefreshItem
    && iType != EPostHighlightChangeRefreshItem;
    }

TBool TMmFloatingItem::IsManualDelete()
    {
    return iManualDelete;
    }

TInt TMmFloatingItem::GetZoomingStatus()
    {
    return iSizeStep;
    }
// End of file
