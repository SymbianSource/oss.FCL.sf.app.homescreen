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
* widget manager plugin inline defintions
*
*/

#ifndef WMPLUGIN_INL_
#define WMPLUGIN_INL_

    /** 
     * the CAknViewAppUi reference
     * 
     * @return returns CAknViewAppUi 
     */
    inline CAknViewAppUi& CWmPlugin::ViewAppUi()
        { return *iViewAppUi; }
    
    /** 
     * the resource loader 
     * 
     * @return returns CWmResourceLoader
     */
    inline CWmResourceLoader& CWmPlugin::ResourceLoader()
        { return *iResourceLoader; }
    
    /** 
     * the content controller ref
     * 
     * @return returns MHsContentController
     */
    inline MHsContentController& CWmPlugin::ContentController()
        { return *iHsContentController; }

    /**
     * the shared file server reference
     */
    inline RFs& CWmPlugin::FileServer()
        { return *iFs; }
    
    

#endif /* WMPLUGIN_INL_ */
