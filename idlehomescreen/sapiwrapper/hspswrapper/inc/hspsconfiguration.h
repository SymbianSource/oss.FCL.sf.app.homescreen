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
* Description:  Hsps configuration message
*
*/


#ifndef C_CHSPSCONFIGURATION_H
#define C_CHSPSCONFIGURATION_H



#include <e32base.h>


namespace hspswrapper{


class CPluginMap;
class CItemMap;
class CObjectMap;
class CPluginInfo;

/**
 *  Hsps configuration message
 *  
 *
 *  @code
 *  
 *  @endcode
 *
 *  @lib xnlayoutengine
 *  @since S60 v5.0
 */
class CHspsConfiguration : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CHspsConfiguration* NewL();
    IMPORT_C static CHspsConfiguration* NewLC();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CHspsConfiguration();

    /**
     * Set configuration id
     *
     * @param aId configuration id
     */
    IMPORT_C CHspsConfiguration& SetConfIdL(const TDesC8& aId);
    
    /**
     * Get configuration id
     *
     * @return Configuration id or KNullDesC8
     */
    IMPORT_C const TDesC8& ConfId()const;
    
    /**
     * Get plugin info 
     *
     * @return Reference to plugin info
     */
    IMPORT_C CPluginInfo& PluginInfo();
    
    /**
     * Append plugin map into plugin map array
     *
     * @param aPluginMap Plugin map. Takes ownership
     */
    IMPORT_C CHspsConfiguration& AddPluginMapL(CPluginMap* aPluginMap);
    
    /**
     * Get plugin maps in a array
     *
     * @return Reference to plugin map array
     */
    IMPORT_C RPointerArray<CPluginMap>& PluginMaps();
    
    /**
     * Append item map into settings array  map
     *
     * @param aItemMap Item map. Takes ownership
     */
    IMPORT_C CHspsConfiguration& AddItemMapL(CItemMap* aItemMap);
    
    /**
     * Get settings array. Contains item maps
     *
     * @return Reference to settings array
     */
    IMPORT_C RPointerArray<CItemMap>& Settings();
    
    /**
     * Append object map into resources array  map
     *
     * @param aObjectMap Object map. Takes ownership
     */
    IMPORT_C CHspsConfiguration& AddObjectMapL(CObjectMap* aObjectMap);
    
    /**
     * Get resource array. Contains object maps
     *
     * @return Reference to resource array
     */
    IMPORT_C RPointerArray<CObjectMap>& Resources();
private:

    CHspsConfiguration();

    void ConstructL();
private: // data

    /**
     * Own. Configuration id
     */
    HBufC8* iId;
    /**
     * Own. Plugin info
     */
    CPluginInfo*    iPluginInfo;
    /**
     * Own. Plugins
     */
    RPointerArray<CPluginMap>   iPlugins;
    /**
     * Own. Settings
     */
    RPointerArray<CItemMap>     iSettings;
    /**
     * Own. Resources
     */
    RPointerArray<CObjectMap>   iResources;
    };

}

#endif // C_CHSPSCONFIGURATION_H
