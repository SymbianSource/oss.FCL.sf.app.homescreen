/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  External rendering plugin adapter
*
*/

#ifndef XNEXTRENDERINGPLUGINADAPTER_H
#define XNEXTRENDERINGPLUGINADAPTER_H

// System includes
#include <e32base.h>
#include <ecom/ecom.h>
#include <coecntrl.h>

// User includes

class MXnExtEventHandler;

/**
 *  Base class for the external rendering plug-ins. Widget developer
 *  can implement own rendering plug-in if stock plug-ins do not
 *  provide sufficient functionality. Baddly written plug-in can 
 *  ruin the Homescreen performance as it runs in the Homescreen
 *  process. Power save mode must be obeyed in order to maximize
 *  the standby times of the device.
 *
 *  @code
 *  // External rendering plug-in mandatory functions
 *  T_RenderingPlugin* T_RenderingPlugin::NewL()
 *      {
 *      T_RenderingPlugin* self = T_RenderingPlugin::NewLC();
 *      CleanupStack::Pop( self );
 *      return self;
 *      }
 *  //
 *  T_RenderingPlugin* T_RenderingPlugin::NewLC()
 *      {
 *      T_RenderingPlugin* self = new( ELeave ) T_RenderingPlugin();
 *      CleanupStack::PushL( self );
 *      self->ConstructL();
 *      return self;
 *      }
 *  //
 *  T_RenderingPlugin::~T_RenderingPlugin()
 *      {
 *      }
 * //
 *  T_RenderingPlugin::T_RenderingPlugin()
 *      {
 *      // Do nothing
 *      }
 *  //
 *  void T_RenderingPlugin::ConstructL()
 *      {
 *      }
 *  //
 *  const TImplementationProxy KImplementationTable[] =
 *     {
 *  #ifdef __EABI__
 *      IMPLEMENTATION_PROXY_ENTRY( 0x22334455, T_RenderingPlugin::NewL )
 *  #else
 *      { { 0x22334455 }, T_RenderingPlugin::NewL }
 *  #endif
 *     };
 *  //
 *  EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
 *      TInt& aTableCount )
 *      {
 *      aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
 *      return KImplementationTable;
 *      }
 *  
 *  @endcode
 *
 *  @lib extrenderingplugin.lib
 *  @since S60 v5.2
 */
class CXnExtRenderingPluginAdapter : public CCoeControl
    {
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     *
     * @since Series 60 5.2
     */
    IMPORT_C static CXnExtRenderingPluginAdapter* NewL( TUid aImplUid );

    /**
     * Destructor.
     *
     * @since Series 60 5.2
     */
    IMPORT_C virtual ~CXnExtRenderingPluginAdapter();  

public: // New functions,

    /**
     * Returns the ECOM implementation uid.
     *
     * @since Series 60 5.2
     */
    IMPORT_C TUid ImplUid() const;

    /**
     * Is called when Homescreen enters power save mode.
     * In power save mode all the timers, outstanding requests and animations
     * must be cancelled in order to save battery.
     *
     * @since Series 60 5.2
     */
    IMPORT_C virtual void EnterPowerSaveModeL();

    /**
     * Is called when Homescreen exists power save mode.
     *
     * @since Series 60 5.2
     */    
    IMPORT_C virtual void ExitPowerSaveModeL();

    /**
     * Informs skin change. If plug-in is responsible of loading skin
     * graphics, all the graphics must be reloaded.
     *
     * @since Series 60 5.2
     */    
    IMPORT_C virtual void SkinChanged();
    
    /**
     * Informs that plug-in element has gained a focus and highlight must be
     * drawn if appropriate.
     *
     * @since Series 60 5.2
     * @param aDrawNow whether plug-in should draw itself.
     */
    IMPORT_C virtual void FocusChanged( TDrawNow aDrawNow );
    
    /**
     * Informs that plug-in element size has changed. Size can change if the
     * plug-in element is declared relatively to parent element.
     *
     * @since Series 60 5.2
     */
    IMPORT_C virtual void SizeChanged();
    
    /**
     * Sets the external event handler interface.
     *
     * @since Series 60 5.2
     * @param aEventHandler Event handler interface.
     */
    IMPORT_C virtual void SetEventHandler( MXnExtEventHandler* aEventHandler );
    
    /**
     * Routes the data stream for the external rendering plugin.
     *
     * @since Series 60 5.2
     * @param aData Data stream.
     * @param aType Type of the stream.
     * @param aIndex Index of the data.
     */
    IMPORT_C virtual void SetDataL( const TDesC8& aData, const TDesC& aType, TInt aIndex );

private: // Data

    /**
     * Destruct key
     */
    TUid iDestructKey;
    
    /**
     * Implementation UID
     */
    TUid iImplUid;
    };


#endif // XNEXTRENDERINGPLUGINADAPTER_H

// End of file
