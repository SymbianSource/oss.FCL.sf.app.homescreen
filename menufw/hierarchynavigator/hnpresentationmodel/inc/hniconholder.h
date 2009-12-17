/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MMICONHOLDER_H
#define MMICONHOLDER_H

#include <e32base.h>

class CGulIcon;

/**
 * A ref-counted icon holder that can be useful if you want to store
 * an icon from the model for an unspecified period of time.
 * By getting the icon holder instead of the icon you can share ownership
 * over the icon by calling Open() on the icon holder. Then Close() must
 * be called when the icon is no longer needed, otherwise a memory leak
 * will occur.
 */
NONSHARABLE_CLASS( CHnIconHolder ): public CObject
    {
public:
    /**
     * Sets the icon to be stored in this icon holder.
     * This method should be called only once. If you want to store another icon
     * simply call Close() on this icon holder and then create a new icon holder
     * to store the new icon.
     * 
     * @param aGulIcon Icon to store in this icon holder.
     */
    IMPORT_C void SetGulIcon( CGulIcon* aGulIcon );
    
    /**
     * Returns the icon held by this icon holder.
     * 
     * @return Icon held by this object (NULL value possible).
     */
    IMPORT_C CGulIcon* GetGulIcon() const;
    
    /**
     * Standard C++ destructor.
     */
    ~CHnIconHolder();
    
private: // data
    
    /**
     * A CGulIcon object owned by this icon holder.
     * Can possibly be NULL.
     * Own.
     */
    CGulIcon* iGulIcon;
    };

#endif // MMICONHOLDER_H
