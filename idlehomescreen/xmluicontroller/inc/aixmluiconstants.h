/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  XML UI specific UI definitions and constants
*
*/


#ifndef AIXMLUICONSTANTS_H
#define AIXMLUICONSTANTS_H

#include <e32std.h>
#include <aicontentmodel.h>

namespace AiUiDef
    {
    namespace xml
        {
        namespace id
            {
            _LIT( KContentIdSeparator, "/" );
            _LIT( KSettingsIdSeparator, "/" );
            }
        
        namespace property
            {
            _LIT8( KClass, "class" );
            _LIT8( KType, "type" );
            _LIT8( KName, "name" );
            _LIT8( KValue, "value" );
            _LIT8( KLoadOrdinal, "load_ordinal" );
            _LIT8( KId, "id" );
            _LIT8( KUid, "uid" );
            _LIT8( KViewId, "viewid" );
            _LIT8( KMessageId, "messageid" );
            _LIT8( KMessage, "message" );
            }
            
        namespace propertyClass
            {
            _LIT8( KContentSource, "ContentSource" );
            _LIT( KSettings, "Settings" );
            }
        namespace general
            {
            _LIT ( KGeneral, "General" );
            _LIT ( KDontOpenLogsOnSendKey, "DontOpenLogsOnSendKey" );
            }
        namespace policy
            {
            _LIT8( KContent, "policy/Content" );
            _LIT8( KEmptyContent, "policy/emptyContent" );
            _LIT8( KResource, "policy/Resource" );
            _LIT8( KVisibilityPolicy, "policy/Visibility" );
            _LIT8( KContentChanged, "policy/ContentChanged" );
            _LIT8( KCondition, "if" );
            }   // End of namespace policy
                    
        namespace name
            {
            _LIT8( KOrdinal, "ordinal" );
            _LIT8( KPriority, "priority" );
            _LIT8( KTarget, "target" );
            
            }   // End of namespace name   
                    
        namespace value
            {
            _LIT8( KShowTooltips, "ShowTooltips" );
            }
            
        namespace event
            {
            _LIT8( KUiEventPrefix, "ui/" );
            _LIT8( KEventHandlerPrefix, "EventHandler/" );
            _LIT8( KFocusGained, "focusgained" );
            _LIT8( KSetElementSizeToCPS, "setelementsizetocps" );
            _LIT( KNewstickerTitleScrolled, "TitleScrolled" );
            _LIT( KNewstickerTitleToScroll, "TitleToScroll" );
            _LIT8( KEventShowHelp, "ShowHelp" );
            _LIT8( KEventSetOnline, "SetOnline" );
            _LIT8( KEventSetOffline, "SetOffline" );
            _LIT8( KEventLaunchApp, "LaunchApp");
            _LIT8( KEventActivateView, "ActivateView");
            _LIT8( KEventActivatePhoneView, "ActivatePhoneView");
            }
           
        namespace css
            {
            _LIT8( KSeparator, ":" );
            _LIT8( KValueTerminator, ";" );
            _LIT8( KPercentage, "%" );
            _LIT8( KPixel, "px" );
            _LIT8( KUnitValue, "u" );
            }
        namespace element
            {
            _LIT8( KPlugin, "plugin" );
            _LIT( K16Plugin, "plugin" );
            _LIT8( KImage, "image" );
            _LIT8( KText, "text" );
            _LIT8( KConfiguration, "configuration" );
            _LIT8( KNewsTicker, "newsticker" );            
            _LIT8( KAnimation, "animation" ); 
            }
        
         }   // End of namespace xml
    
    }   // End of namespace AiUiDef

#define KContentTypeText TPtrC8( reinterpret_cast< const TText8* >( KAiContentTypeText ) )
#define KContentTypeBitmap TPtrC8( reinterpret_cast< const TText8* >( KAiContentTypeBitmap ) )
_LIT8( KContentTypeImage, "image/" );
_LIT8( KContentTypeImageSvg, "image/svg+xml" );
_LIT( KTargetSliderVolumeValue, "slider::volume" );
const TText KComma = ',';
const TText KLeftParenthesis = '(';
const TText KRightParenthesis = ')';
const TText KNotOperator = '!';
const TText KAndOperator = '+';
const TText KWhiteSpace = ' ';


#endif // AIXMLUICONSTANTS_H
