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
* Inline function definitions for CWmListBox
*
*/

#ifndef WMLISTBOX_INL_
#define WMLISTBOX_INL_

/** 
 * Findbox visibility
 * 
 * @param aVisibility sets find pane visibility
 */
 inline void CWmListBox::SetFindPaneIsVisible( TBool aVisibility )
        { iFindPaneIsVisible = aVisibility; }
 
 /** 
  * number of items in the widget data array (also items on the list) 
  * 
  * @return count of widget data array
  */
 inline TInt CWmListBox::WidgetDataCount()
     { return iVisibleWidgetArray.Count(); }
 
 /**
  * gets the constant widget data array
  */
 inline const RWidgetDataValues& CWmListBox::WidgetDataArray()
     { return iVisibleWidgetArray; }

 inline const ROrderArray& CWmListBox::OrderDataArray()
      { return iOrderDataArray; }
 
#endif /* WMLISTBOX_INL_ */
