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
* Description:  Utility functions of Xuikon.
*
*/


// System includes
#include <AknUtils.h>
#include <AknFontAccess.h>
#include <uri16.h>

// User includes
#include "c_xnutils.h"
#include "xnproperty.h"
#include "xnnodepluginif.h"
#include "xnuienginepluginif.h"
#include "xndomlist.h"
#include "xnresource.h"
#include "xnnodebreadthfirstiterator.h"
#include "xncontroladapter.h"
#include "xncontroladapterimpl.h"
#include "xncomponent.h"
#include "xncomponentnodeimpl.h"

// Constants
_LIT( KNormal, "normal" );
_LIT( KPrimaryFont , "EAknLogicalFontPrimaryFont" );
_LIT( KSecondaryFont , "EAknLogicalFontSecondaryFont" );
_LIT( KTitleFont, "EAknLogicalFontTitleFont" );
_LIT( KPrimarySmallFont , "EAknLogicalFontPrimarySmallFont" );
_LIT( KDigitalFont , "EAknLogicalFontDigitalFont" );

_LIT( KXnCommonWhiteSpaceCharacters, " \n\t\r" );
_LIT( KXnLineFeedTabCharacters, "\n\t\r" );
_LIT( KXnTabCharacters, "\t\r" );

_LIT( KSkin, "skin(" );
 
namespace XnColorNames
    {
    _LIT8( Kaliceblue, "aliceblue" );
    _LIT8( Kantiquewhite, "antiquewhite" );
    _LIT8( Kaqua, "aqua" );
    _LIT8( Kaquamarine, "aquamarine" );
    _LIT8( Kazure, "azure" );
    _LIT8( Kbeige, "beige" );
    _LIT8( Kbisque, "bisque" );
    _LIT8( Kblack, "black" );
    _LIT8( Kblanchedalmond, "blanchedalmond" );
    _LIT8( Kblue, "blue" );
    _LIT8( Kblueviolet, "blueviolet" );
    _LIT8( Kbrown, "brown" );
    _LIT8( Kburlywood, "burlywood" );
    _LIT8( Kcadetblue, "cadetblue" );
    _LIT8( Kchartreuse, "chartreuse" );
    _LIT8( Kchocolate, "chocolate" );
    _LIT8( Kcoral, "coral" );
    _LIT8( Kcornflowerblue, "cornflowerblue" );
    _LIT8( Kcornsilk, "cornsilk" );
    _LIT8( Kcrimson, "crimson" );
    _LIT8( Kcyan, "cyan" );
    _LIT8( Kdarkblue, "darkblue" );
    _LIT8( Kdarkcyan, "darkcyan" );
    _LIT8( Kdarkgoldenrod, "darkgoldenrod" );
    _LIT8( Kdarkgray, "darkgray" );
    _LIT8( Kdarkgreen, "darkgreen" );
    _LIT8( Kdarkgrey, "darkgrey" );
    _LIT8( Kdarkkhaki, "darkkhaki" );
    _LIT8( Kdarkmagenta, "darkmagenta" );
    _LIT8( Kdarkolivegreen, "darkolivegreen" );
    _LIT8( Kdarkorange, "darkorange" );
    _LIT8( Kdarkorchid, "darkorchid" );
    _LIT8( Kdarkred, "darkred" );
    _LIT8( Kdarksalmon, "darksalmon" );
    _LIT8( Kdarkseagreen, "darkseagreen" );
    _LIT8( Kdarkslateblue, "darkslateblue" );
    _LIT8( Kdarkslategray, "darkslategray" );
    _LIT8( Kdarkslategrey, "darkslategrey" );
    _LIT8( Kdarkturquoise, "darkturquoise" );
    _LIT8( Kdarkviolet, "darkviolet" );
    _LIT8( Kdeeppink, "deeppink" );
    _LIT8( Kdeepskyblue, "deepskyblue" );
    _LIT8( Kdimgray, "dimgray" );
    _LIT8( Kdimgrey, "dimgrey" );
    _LIT8( Kdodgerblue, "dodgerblue" );
    _LIT8( Kfirebrick, "firebrick" );
    _LIT8( Kfloralwhite, "floralwhite" );
    _LIT8( Kforestgreen, "forestgreen" );
    _LIT8( Kfuchsia, "fuchsia" );
    _LIT8( Kgainsboro, "gainsboro" );
    _LIT8( Kghostwhite, "ghostwhite" );
    _LIT8( Kgold, "gold" );
    _LIT8( Kgoldenrod, "goldenrod" );
    _LIT8( Kgray, "gray" );
    _LIT8( Kgreen, "green" );
    _LIT8( Kgreenyellow, "greenyellow" );
    _LIT8( Kgrey, "grey" );
    _LIT8( Khoneydew, "honeydew" );
    _LIT8( Khotpink, "hotpink" );
    _LIT8( Kindianred, "indianred" );
    _LIT8( Kindigo, "indigo" );
    _LIT8( Kivory, "ivory" );
    _LIT8( Kkhaki, "khaki" );
    _LIT8( Klavender, "lavender" );
    _LIT8( Klavenderblush, "lavenderblush" );
    _LIT8( Klawngreen, "lawngreen" );
    _LIT8( Klemonchiffon, "lemonchiffon" );
    _LIT8( Klightblue, "lightblue" );
    _LIT8( Klightcoral, "lightcoral" );
    _LIT8( Klightcyan, "lightcyan" );
    _LIT8( Klightgoldenrodyellow, "lightgoldenrodyellow" );
    _LIT8( Klightgray, "lightgray" );
    _LIT8( Klightgreen, "lightgreen" );
    _LIT8( Klightgrey, "lightgrey" );
    _LIT8( Klightpink, "lightpink" );
    _LIT8( Klightsalmon, "lightsalmon" );
    _LIT8( Klightseagreen, "lightseagreen" );
    _LIT8( Klightskyblue, "lightskyblue" );
    _LIT8( Klightslategray, "lightslategray" );
    _LIT8( Klightslategrey, "lightslategrey" );
    _LIT8( Klightsteelblue, "lightsteelblue" );
    _LIT8( Klightyellow, "lightyellow" );
    _LIT8( Klime, "lime" );
    _LIT8( Klimegreen, "limegreen" );
    _LIT8( Klinen, "linen" );
    _LIT8( Kmagenta, "magenta" );
    _LIT8( Kmaroon, "maroon" );
    _LIT8( Kmediumaquamarine, "mediumaquamarine" );
    _LIT8( Kmediumblue, "mediumblue" );
    _LIT8( Kmediumorchid, "mediumorchid" );
    _LIT8( Kmediumpurple, "mediumpurple" );
    _LIT8( Kmediumseagreen, "mediumseagreen" );
    _LIT8( Kmediumslateblue, "mediumslateblue" );
    _LIT8( Kmediumspringgreen, "mediumspringgreen" );
    _LIT8( Kmediumturquoise, "mediumturquoise" );
    _LIT8( Kmediumvioletred, "mediumvioletred" );
    _LIT8( Kmidnightblue, "midnightblue" );
    _LIT8( Kmintcream, "mintcream" );
    _LIT8( Kmistyrose, "mistyrose" );
    _LIT8( Kmoccasin, "moccasin" );
    _LIT8( Knavajowhite, "navajowhite" );
    _LIT8( Knavy, "navy" );
    _LIT8( Koldlace, "oldlace" );
    _LIT8( Kolive, "olive" );
    _LIT8( Kolivedrab, "olivedrab" );
    _LIT8( Korange, "orange" );
    _LIT8( Korangered, "orangered" );
    _LIT8( Korchid, "orchid" );
    _LIT8( Kpalegoldenrod, "palegoldenrod" );
    _LIT8( Kpalegreen, "palegreen" );
    _LIT8( Kpaleturquoise, "paleturquoise" );
    _LIT8( Kpalevioletred, "palevioletred" );
    _LIT8( Kpapayawhip, "papayawhip" );
    _LIT8( Kpeachpuff, "peachpuff" );
    _LIT8( Kperu, "peru" );
    _LIT8( Kpink, "pink" );
    _LIT8( Kplum, "plum" );
    _LIT8( Kpowderblue, "powderblue" );
    _LIT8( Kpurple, "purple" );
    _LIT8( Kred, "red" );
    _LIT8( Krosybrown, "rosybrown" );
    _LIT8( Kroyalblue, "royalblue" );
    _LIT8( Ksaddlebrown, "saddlebrown" );
    _LIT8( Ksalmon, "salmon" );
    _LIT8( Ksandybrown, "sandybrown" );
    _LIT8( Kseagreen, "seagreen" );
    _LIT8( Kseashell, "seashell" );
    _LIT8( Ksienna, "sienna" );
    _LIT8( Ksilver, "silver" );
    _LIT8( Kskyblue, "skyblue" );
    _LIT8( Kslateblue, "slateblue" );
    _LIT8( Kslategray, "slategray" );
    _LIT8( Kslategrey, "slategrey" );
    _LIT8( Ksnow, "snow" );
    _LIT8( Kspringgreen, "springgreen" );
    _LIT8( Ksteelblue, "steelblue" );
    _LIT8( Ktan, "tan" );
    _LIT8( Kteal, "teal" );
    _LIT8( Kthistle, "thistle" );
    _LIT8( Ktomato, "tomato" );
    _LIT8( Kturquoise, "turquoise" );
    _LIT8( Kviolet, "violet" );
    _LIT8( Kwheat, "wheat" );
    _LIT8( Kwhite, "white" );
    _LIT8( Kwhitesmoke, "whitesmoke" );
    _LIT8( Kyellow, "yellow" );
    _LIT8( Kyellowgreen, "yellowgreen" );
    }

namespace XnColorValues
    {
    const TInt Kaliceblue = 0xfff8f0;
    const TInt Kantiquewhite = 0xd7ebfa;
    const TInt Kaqua = 0xffff00;
    const TInt Kaquamarine = 0xd4ff7f;
    const TInt Kazure = 0xfffff0;
    const TInt Kbeige = 0xdcf5f5;
    const TInt Kbisque = 0xc4e4ff;
    const TInt Kblack = 0x000000;
    const TInt Kblanchedalmond = 0xcdebff;
    const TInt Kblue = 0xff0000;
    const TInt Kblueviolet = 0xe22b8a;
    const TInt Kbrown = 0x2a2aa5;
    const TInt Kburlywood = 0x87b8de;
    const TInt Kcadetblue = 0xa09e5f;
    const TInt Kchartreuse = 0x00ff7f;
    const TInt Kchocolate = 0x1e69d2;
    const TInt Kcoral = 0x507fff;
    const TInt Kcornflowerblue = 0xed9564;
    const TInt Kcornsilk = 0xdcf8ff;
    const TInt Kcrimson = 0x3c14dc;
    const TInt Kcyan = 0xffff00;
    const TInt Kdarkblue = 0x8b0000;
    const TInt Kdarkcyan = 0x8b8b00;
    const TInt Kdarkgoldenrod = 0x0b86b8;
    const TInt Kdarkgray = 0xa9a9a9;
    const TInt Kdarkgreen = 0x006400;
    const TInt Kdarkgrey = 0xa9a9a9;
    const TInt Kdarkkhaki = 0x6bb7bd;
    const TInt Kdarkmagenta = 0x8b008b;
    const TInt Kdarkolivegreen = 0x2f6b55;
    const TInt Kdarkorange = 0x008cff;
    const TInt Kdarkorchid = 0xcc3299;
    const TInt Kdarkred = 0x00008b;
    const TInt Kdarksalmon = 0x7a96e9;
    const TInt Kdarkseagreen = 0x8fbc8f;
    const TInt Kdarkslateblue = 0x8b3d48;
    const TInt Kdarkslategray = 0x4f4f2f;
    const TInt Kdarkslategrey = 0x4f4f2f;
    const TInt Kdarkturquoise = 0xd1ce00;
    const TInt Kdarkviolet = 0xd30094;
    const TInt Kdeeppink = 0x9314ff;
    const TInt Kdeepskyblue = 0xffbf00;
    const TInt Kdimgray = 0x696969;
    const TInt Kdimgrey = 0x696969;
    const TInt Kdodgerblue = 0xff901e;
    const TInt Kfirebrick = 0x2222b2;
    const TInt Kfloralwhite = 0xf0faff;
    const TInt Kforestgreen = 0x228b22;
    const TInt Kfuchsia = 0xff00ff;
    const TInt Kgainsboro = 0xdcdcdc;
    const TInt Kghostwhite = 0xfff8f8;
    const TInt Kgold = 0x00d7ff;
    const TInt Kgoldenrod = 0x20a5da;
    const TInt Kgray = 0x808080;
    const TInt Kgreen = 0x008000;
    const TInt Kgreenyellow = 0x2fffad;
    const TInt Kgrey = 0x808080;
    const TInt Khoneydew = 0xf0fff0;
    const TInt Khotpink = 0xb469ff;
    const TInt Kindianred = 0x5c5ccd;
    const TInt Kindigo = 0x82004b;
    const TInt Kivory = 0xf0ffff;
    const TInt Kkhaki = 0x8ce6f0;
    const TInt Klavender = 0xfae6e6;
    const TInt Klavenderblush = 0xf5f0ff;
    const TInt Klawngreen = 0x00fc7c;
    const TInt Klemonchiffon = 0xcdfaff;
    const TInt Klightblue = 0xe6d8ad;
    const TInt Klightcoral = 0x8080f0;
    const TInt Klightcyan = 0xffffe0;
    const TInt Klightgoldenrodyellow = 0xd2fafa;
    const TInt Klightgray = 0xd3d3d3;
    const TInt Klightgreen = 0x90ee90;
    const TInt Klightgrey = 0xd3d3d3;
    const TInt Klightpink = 0xc1b6ff;
    const TInt Klightsalmon = 0x7aa0ff;
    const TInt Klightseagreen = 0xaab220;
    const TInt Klightskyblue = 0xface87;
    const TInt Klightslategray = 0x998877;
    const TInt Klightslategrey = 0x998877;
    const TInt Klightsteelblue = 0xdec4b0;
    const TInt Klightyellow = 0xe0ffff;
    const TInt Klime = 0x00ff00;
    const TInt Klimegreen = 0x32cd32;
    const TInt Klinen = 0xe6f0fa;
    const TInt Kmagenta = 0xff00ff;
    const TInt Kmaroon = 0x000080;
    const TInt Kmediumaquamarine = 0xaacd66;
    const TInt Kmediumblue = 0xcd0000;
    const TInt Kmediumorchid = 0xd355ba;
    const TInt Kmediumpurple = 0xdb7093;
    const TInt Kmediumseagreen = 0x71b33c;
    const TInt Kmediumslateblue = 0xee687b;
    const TInt Kmediumspringgreen = 0x9afa00;
    const TInt Kmediumturquoise = 0xccd148;
    const TInt Kmediumvioletred = 0x8515c7;
    const TInt Kmidnightblue = 0x701919;
    const TInt Kmintcream = 0xfafff5;
    const TInt Kmistyrose = 0xe1e4ff;
    const TInt Kmoccasin = 0xb5e4ff;
    const TInt Knavajowhite = 0xaddeff;
    const TInt Knavy = 0x800000;
    const TInt Koldlace = 0xe6f5fd;
    const TInt Kolive = 0x008080;
    const TInt Kolivedrab = 0x238e6b;
    const TInt Korange = 0x00a5ff;
    const TInt Korangered = 0x0045ff;
    const TInt Korchid = 0xd670da;
    const TInt Kpalegoldenrod = 0xaae8ee;
    const TInt Kpalegreen = 0x98fb98;
    const TInt Kpaleturquoise = 0xeeeeaf;
    const TInt Kpalevioletred = 0x9370db;
    const TInt Kpapayawhip = 0xd5efff;
    const TInt Kpeachpuff = 0xb9daff;
    const TInt Kperu = 0x3f85cd;
    const TInt Kpink = 0xcbc0ff;
    const TInt Kplum = 0xdda0dd;
    const TInt Kpowderblue = 0xe6e0b0;
    const TInt Kpurple = 0x800080;
    const TInt Kred = 0x0000ff;
    const TInt Krosybrown = 0x8f8fbc;
    const TInt Kroyalblue = 0xe16941;
    const TInt Ksaddlebrown = 0x13458b;
    const TInt Ksalmon = 0x7280fa;
    const TInt Ksandybrown = 0x60a4f4;
    const TInt Kseagreen = 0x578b2e;
    const TInt Kseashell = 0xeef5ff;
    const TInt Ksienna = 0x2d52a0;
    const TInt Ksilver = 0xc0c0c0;
    const TInt Kskyblue = 0xebce87;
    const TInt Kslateblue = 0xcd5a6a;
    const TInt Kslategray = 0x908070;
    const TInt Kslategrey = 0x908070;
    const TInt Ksnow = 0xfafaff;
    const TInt Kspringgreen = 0x7fff00;
    const TInt Ksteelblue = 0xb48246;
    const TInt Ktan = 0x8cb4d2;
    const TInt Kteal = 0x808000;
    const TInt Kthistle = 0xd8bfd8;
    const TInt Ktomato = 0x4763ff;
    const TInt Kturquoise = 0xd0e040;
    const TInt Kviolet = 0xee82ee;
    const TInt Kwheat = 0xb3def5;
    const TInt Kwhite = 0xffffff;
    const TInt Kwhitesmoke = 0xf5f5f5;
    const TInt Kyellow = 0x00ffff;
    const TInt Kyellowgreen = 0x32cd9a;
    }

// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// MatchColor
// -----------------------------------------------------------------------------
//
static TBool MatchColor( const TDesC8& aColorName, TRgb& aValue )
    {
    // Following local macro is used to generate large if-table.
    #define E( name ) else if ( aColorName == XnColorNames::name ) \
        { \
        aValue = TRgb( XnColorValues::name ); \
        return ETrue; \
        }

    if ( EFalse )
        {
        }
    E( Kaliceblue )
    E( Kantiquewhite )
    E( Kaqua )
    E( Kaquamarine )
    E( Kazure )
    E( Kbeige )
    E( Kbisque )
    E( Kblack )
    E( Kblanchedalmond )
    E( Kblue )
    E( Kblueviolet )
    E( Kbrown )
    E( Kburlywood )
    E( Kcadetblue )
    E( Kchartreuse )
    E( Kchocolate )
    E( Kcoral )
    E( Kcornflowerblue )
    E( Kcornsilk )
    E( Kcrimson )
    E( Kcyan )
    E( Kdarkblue )
    E( Kdarkcyan )
    E( Kdarkgoldenrod )
    E( Kdarkgray )
    E( Kdarkgreen )
    E( Kdarkgrey )
    E( Kdarkkhaki )
    E( Kdarkmagenta )
    E( Kdarkolivegreen )
    E( Kdarkorange )
    E( Kdarkorchid )
    E( Kdarkred )
    E( Kdarksalmon )
    E( Kdarkseagreen )
    E( Kdarkslateblue )
    E( Kdarkslategray )
    E( Kdarkslategrey )
    E( Kdarkturquoise )
    E( Kdarkviolet )
    E( Kdeeppink )
    E( Kdeepskyblue )
    E( Kdimgray )
    E( Kdimgrey )
    E( Kdodgerblue )
    E( Kfirebrick )
    E( Kfloralwhite )
    E( Kforestgreen )
    E( Kfuchsia )
    E( Kgainsboro )
    E( Kghostwhite )
    E( Kgold )
    E( Kgoldenrod )
    E( Kgray )
    E( Kgreen )
    E( Kgreenyellow )
    E( Kgrey )
    E( Khoneydew )
    E( Khotpink )
    E( Kindianred )
    E( Kindigo )
    E( Kivory )
    E( Kkhaki )
    E( Klavender )
    E( Klavenderblush )
    E( Klawngreen )
    E( Klemonchiffon )
    E( Klightblue )
    E( Klightcoral )
    E( Klightcyan )
    E( Klightgoldenrodyellow )
    E( Klightgray )
    E( Klightgreen )
    E( Klightgrey )
    E( Klightpink )
    E( Klightsalmon )
    E( Klightseagreen )
    E( Klightskyblue )
    E( Klightslategray )
    E( Klightslategrey )
    E( Klightsteelblue )
    E( Klightyellow )
    E( Klime )
    E( Klimegreen )
    E( Klinen )
    E( Kmagenta )
    E( Kmaroon )
    E( Kmediumaquamarine )
    E( Kmediumblue )
    E( Kmediumorchid )
    E( Kmediumpurple )
    E( Kmediumseagreen )
    E( Kmediumslateblue )
    E( Kmediumspringgreen )
    E( Kmediumturquoise )
    E( Kmediumvioletred )
    E( Kmidnightblue )
    E( Kmintcream )
    E( Kmistyrose )
    E( Kmoccasin )
    E( Knavajowhite )
    E( Knavy )
    E( Koldlace )
    E( Kolive )
    E( Kolivedrab )
    E( Korange )
    E( Korangered )
    E( Korchid )
    E( Kpalegoldenrod )
    E( Kpalegreen )
    E( Kpaleturquoise )
    E( Kpalevioletred )
    E( Kpapayawhip )
    E( Kpeachpuff )
    E( Kperu )
    E( Kpink )
    E( Kplum )
    E( Kpowderblue )
    E( Kpurple )
    E( Kred )
    E( Krosybrown )
    E( Kroyalblue )
    E( Ksaddlebrown )
    E( Ksalmon )
    E( Ksandybrown )
    E( Kseagreen )
    E( Kseashell )
    E( Ksienna )
    E( Ksilver )
    E( Kskyblue )
    E( Kslateblue )
    E( Kslategray )
    E( Kslategrey )
    E( Ksnow )
    E( Kspringgreen )
    E( Ksteelblue )
    E( Ktan )
    E( Kteal )
    E( Kthistle )
    E( Ktomato )
    E( Kturquoise )
    E( Kviolet )
    E( Kwheat )
    E( Kwhite )
    E( Kwhitesmoke )
    E( Kyellow )
    E( Kyellowgreen )
    else
        {
        return EFalse;
        }
    #undef E
    return EFalse;
    }

// -----------------------------------------------------------------------------
// FindResource
// Locates a resource from the resource list
// -----------------------------------------------------------------------------
//
EXPORT_C CXnResource* CXnUtils::FindResource(
    CArrayPtrSeg< CXnResource >& aArray,
    const TDesC& aResourceFileName,
    TInt& aBitmapIndex )
    {
    if ( aResourceFileName == KNullDesC )
        {
        return NULL;
        }

    TUriParser parser;
    parser.Parse( aResourceFileName );

    TPtrC uriWithoutFragment;
    parser.UriWithoutFragment( uriWithoutFragment );

    const TDesC& fragment( parser.Extract( EUriFragment ) );

    TLex lex( fragment );
    lex.Val( aBitmapIndex );

    for ( TInt i = aArray.Count() - 1; i >= 0; --i )
        {
        CXnResource* res( aArray.At( i ) );

        const TDesC& resId( res->ResourceId() );

        if ( resId.FindF( uriWithoutFragment ) != KErrNotFound )
            {
            return res;
            }
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// DecimalFromHexChar
// -----------------------------------------------------------------------------
//
static TBool DecimalFromHexChar( TChar aChar, TInt& aValue )
    {
    switch ( aChar )
        {
        case 'a':
        case 'A':
            aValue = 10;
            return ETrue;
        case 'b':
        case 'B':
            aValue = 11;
            return ETrue;
        case 'c':
        case 'C':
            aValue = 12;
            return ETrue;
        case 'd':
        case 'D':
            aValue = 13;
            return ETrue;
        case 'e':
        case 'E':
            aValue = 14;
            return ETrue;
        case 'f':
        case 'F':
            aValue = 15;
            return ETrue;
        case '0':
            aValue = 0;
            return ETrue;
        case '1':
            aValue = 1;
            return ETrue;
        case '2':
            aValue = 2;
            return ETrue;
        case '3':
            aValue = 3;
            return ETrue;
        case '4':
            aValue = 4;
            return ETrue;
        case '5':
            aValue = 5;
            return ETrue;
        case '6':
            aValue = 6;
            return ETrue;
        case '7':
            aValue = 7;
            return ETrue;
        case '8':
            aValue = 8;
            return ETrue;
        case '9':
            aValue = 9;
            return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// NumericOrHexValue
// -----------------------------------------------------------------------------
//
static TInt NumericOrHexValue( TReal& aValue, TLex8& aData )
    {
    TBool isHex = EFalse;
    TInt hexIndex = 0;
    TPtrC8 data = aData.Remainder();
    TInt separatorIndex = 0;
    if ( ( hexIndex = data.LocateF( '#' ) ) != KErrNotFound &&
         ( ( separatorIndex = data.LocateF( ',' ) ) != KErrNotFound ||
           ( separatorIndex = data.LocateF( ')' ) ) != KErrNotFound ) &&
         ( separatorIndex > hexIndex ) )
        {
        isHex = ETrue;
        }
    if ( !isHex )
        {
        return aData.Val( aValue );
        }
    else
        {
        TChar character = aData.Get();
        if ( character == 0 )
            {
            return KErrGeneral;
            }
        character = aData.Get();
        if ( character == 0 )
            {
            return KErrGeneral;
            }
        TChar secondCharacter = aData.Get();
        if ( secondCharacter == 0 )
            {
            return KErrGeneral;
            }
        TInt val;
        TInt secondVal;
        if ( DecimalFromHexChar( character, val ) &&
             DecimalFromHexChar( secondCharacter, secondVal ) )
            {
            aValue = 16 * val + secondVal;
            return KErrNone;
            }
        }
    return KErrGeneral;
    }

// -----------------------------------------------------------------------------
// DecimalValueOrPercentage
// -----------------------------------------------------------------------------
//
static TInt DecimalValueOrPercentage( TInt& aValue, TLex8& aData )
    {
    TBool isPercentage = EFalse;
    TInt separatorIndex = 0;
    TInt percentageIndex = 0;
    TPtrC8 data = aData.Remainder();
    if ( ( percentageIndex = data.LocateF( '%' ) ) != KErrNotFound &&
         ( ( separatorIndex = data.LocateF( ',' ) ) != KErrNotFound ||
           ( separatorIndex = data.LocateF( ')' ) ) != KErrNotFound ) &&
         ( separatorIndex > percentageIndex ) )
        {
        isPercentage = ETrue;
        }
    TReal tmp;
    TInt error = NumericOrHexValue( tmp, aData );
    if ( error != KErrNone )
        {
        return error;
        }
    if ( !isPercentage )
        {
        aValue = static_cast< TInt >( tmp );
        }
    else
        {
        aValue = static_cast< TInt >( ( tmp / 100.0 ) * 255.0 );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// GetRgbValueFromColorName
// -----------------------------------------------------------------------------
//
static TBool GetRgbValueFromColorName( TRgb& aValue, const TDesC8& aData )
    {
    return MatchColor( aData, aValue );
    }

// -----------------------------------------------------------------------------
// GetRgbValueFromDecimalValues
// -----------------------------------------------------------------------------
//
static TBool GetRgbValueFromDecimalValues( TRgb& aValue, const TDesC8& aData )
    {
    TLex8 lex( aData );
    TInt red;
    TInt green;
    TInt blue;
    lex.SkipSpace();
    TInt error = DecimalValueOrPercentage( red, lex );
    if ( error != KErrNone )
        {
        return EFalse;
        }
    lex.SkipSpace();
    while ( lex.Peek() == ',' || lex.Peek() == '%' )
        {
        lex.Get();
        lex.SkipSpace();
        }
    error = DecimalValueOrPercentage( green, lex );
    if ( error != KErrNone )
        {
        return EFalse;
        }
    lex.SkipSpace();
    while ( lex.Peek() == ',' || lex.Peek() == '%' )
        {
        lex.Get();
        lex.SkipSpace();
        }
    error = DecimalValueOrPercentage( blue, lex );
    if ( error != KErrNone )
        {
        return EFalse;
        }
    aValue.SetRed( red );
    aValue.SetGreen( green );
    aValue.SetBlue( blue );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// GetRgbValueFromRgbFunction
// -----------------------------------------------------------------------------
//
static TBool GetRgbValueFromRgbFunction( TRgb& aValue, const TDesC8& aData )
    {
    TInt functionNameIndex = 0;
    TInt openingParenthesisIndex = 0;
    TInt closingParenthesisIndex = 0;
    if ( ( functionNameIndex = aData.FindF( _L8( "rgb" ) ) ) != KErrNotFound &&
         ( openingParenthesisIndex = aData.FindF( _L8( "(" ) ) ) != KErrNotFound &&
         ( openingParenthesisIndex > functionNameIndex ) &&
         ( closingParenthesisIndex = aData.FindF( _L8( ")" ) ) ) != KErrNotFound &&
         ( closingParenthesisIndex > openingParenthesisIndex ) )
        {
        TPtrC8 functionString = aData.Mid(
            openingParenthesisIndex + 1,
            closingParenthesisIndex - openingParenthesisIndex );
        return GetRgbValueFromDecimalValues( aValue, functionString );
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// GetRgbValueFromHslFunction
// -----------------------------------------------------------------------------
//
static TBool GetRgbValueFromHslFunction( TRgb& aValue, const TDesC8& aData )
    {
    TInt functionNameIndex = 0;
    TInt openingParenthesisIndex = 0;
    TInt closingParenthesisIndex = 0;
    if ( ( functionNameIndex = aData.FindF( _L8( "hsl" ) ) ) != KErrNotFound &&
         ( openingParenthesisIndex = aData.FindF( _L8( "(" ) ) ) != KErrNotFound &&
         ( openingParenthesisIndex > functionNameIndex ) &&
         ( closingParenthesisIndex = aData.FindF( _L8( ")" ) ) ) != KErrNotFound &&
         ( closingParenthesisIndex > openingParenthesisIndex ) )
        {
        TPtrC8 functionString = aData.Mid(
            openingParenthesisIndex + 1,
            closingParenthesisIndex - openingParenthesisIndex );
        TRgb rgb;
        TBool success = GetRgbValueFromDecimalValues( rgb, functionString );

        TLex8 lex( functionString );
        TInt red;
        TInt green;
        TInt blue;
        lex.SkipSpace();
        TReal tmp;

        TInt error = NumericOrHexValue( tmp, lex );
        if ( error != KErrNone )
            {
            return EFalse;
            }
        red = static_cast< TInt >( tmp );
        lex.SkipSpace();
        while ( lex.Peek() == ',' || lex.Peek() == '%' )
            {
            lex.Get();
            lex.SkipSpace();
            }
        error = NumericOrHexValue( tmp, lex );
        if ( error != KErrNone )
            {
            return EFalse;
            }
        green = static_cast< TInt >( tmp );
        lex.SkipSpace();
        while ( lex.Peek() == ',' || lex.Peek() == '%' )
            {
            lex.Get();
            lex.SkipSpace();
            }
        error = NumericOrHexValue( tmp, lex );
        if ( error != KErrNone )
            {
            return EFalse;
            }
        blue = static_cast< TInt >( tmp );

        if ( success )
            {
            aValue = CXnUtils::ConvertHslToRgb( red, green, blue );
            }
        return success;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// GetRgbValueFromHexValue
// -----------------------------------------------------------------------------
//
static TBool GetRgbValueFromHexValue( TRgb& aValue, const TDesC8& aData )
    {
    TBool isHex = EFalse;
    if ( aData.LocateF( '#' ) != KErrNotFound )
        {
        isHex = ETrue;
        }
    if ( !isHex )
        {
        return EFalse;
        }
    else
        {
        TInt length = aData.Length();
        TLex8 lex( aData );
        lex.SkipSpace();
        if ( lex.Get() == 0 )
            {
            return EFalse;
            }
        TInt values[3];
        TBool flags[3] = { EFalse, EFalse, EFalse };
        TChar chars[6] = { 0, 0, 0, 0, 0, 0 };
        for ( TInt i = 5; i >= 0; --i )
            {
            TChar character = lex.Get();
            if ( character == 0 )
                {
                break;
                }
            chars[i] = character;
            if ( length == 4 )
                {
                i--;
                chars[i] = character;
                }
            }

        for ( TInt i = 5, j = 0; ( i >= 0 ) && ( j < 3 ); --i )
            {
            if ( chars[i] == 0 )
                {
                continue;
                }
            if ( i % 2 )
                {
                TInt secondVal = 0;
                TInt val = 0;
                TChar secondCharacter = chars[i];
                --i;
                TChar character = chars[i];
                if ( DecimalFromHexChar( character, val ) &&
                     DecimalFromHexChar( secondCharacter, secondVal ) )
                    {
                    values[j] = val + 16 * secondVal;
                    flags[j] = ETrue;
                    ++j;
                    }
                else
                    {
                    break;
                    }
                }
            else
                {
                TInt val = 0;
                TChar character = chars[i];
                if ( DecimalFromHexChar( character, val ) )
                    {
                    values[j] = val;
                    flags[j] = ETrue;
                    ++j;
                    }
                else
                    {
                    break;
                    }
                }
            }
        if ( flags[2] && flags[1] && flags[0] )
            {
            aValue.SetRed( values[0] );
            aValue.SetGreen( values[1] );
            aValue.SetBlue( values[2] );
            return ETrue;
            }
        else if ( flags[1] && flags[0] )
            {
            aValue.SetRed( 0 );
            aValue.SetGreen( values[0] );
            aValue.SetBlue( values[1] );
            return ETrue;
            }
        else if ( flags[0] )
            {
            aValue.SetRed( 0 );
            aValue.SetGreen( 0 );
            aValue.SetBlue( values[0] );
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Finds out AVKON font ID from fontname
// -----------------------------------------------------------------------------
//
static TInt AvkonFontId( const TDesC& aFontName )
    {
    TInt fontId( KErrNotFound );
    if ( aFontName == KPrimaryFont )
        {
        fontId = KAknFontCategoryPrimary;
        }
    else if ( aFontName == KSecondaryFont )
        {
        fontId = KAknFontCategorySecondary;
        }
    else if ( aFontName == KTitleFont )
        {
        fontId = KAknFontCategoryTitle;
        }
    else if ( aFontName == KPrimarySmallFont )
        {
        fontId = KAknFontCategoryPrimarySmall;
        }
    else if ( aFontName == KDigitalFont )
        {
        fontId = KAknFontCategoryDigital;
        }
    return fontId;
    }

// -----------------------------------------------------------------------------
// TRgb CXnUtils::ConvertHslToRgb( TInt aHue, TInt aSaturation,
// TInt aLightness )
// Perform the HSL to RGB conversion according to
// the rules defined in the CSS3 color module specification:
// http://www.w3.org/TR/2003/CR-css3-color-20030514/#hsl-color
// -----------------------------------------------------------------------------
//
EXPORT_C TRgb CXnUtils::ConvertHslToRgb(
    TInt aHue, TInt aSaturation, TInt aLightness )
    {
    TRgb rgb( 0, 0, 0 );
    TReal h( aHue );
    h /= 360;
    TReal s( aSaturation );
    s /= 100;
    TReal l( aLightness );
    l /= 100;

    if ( 0 == aSaturation )
        {
        rgb.SetRed( TInt( l * 255 ) );
        rgb.SetGreen( TInt( l * 255 ) );
        rgb.SetBlue( TInt( l * 255 ) );
        }
    else
        {
        TReal m1( 0 );
        TReal m2( 0 );

        // l<=0.5: PUT l*(s+1) IN m2
        if ( aLightness < 0.5 )
            {
            m2 = l * ( s + 1.0 );
            }
        // PUT l+s-l*s IN m2
        else
            {
            m2 = l + s - ( l * s );
            }

        // PUT l*2-m2 IN m1
        m1 = l * 2.0 - m2;

        TReal helperH( h );
        TReal helperM1( m1 );
        TReal helperM2( m2 );

        // calculate Red
        helperH = h + ( 1.0 / 3.0 );

        // IF h<0: PUT h+1 IN h
        if ( helperH < 0 )
            {
            helperH += 1.0;
            }

        // IF h>1: PUT h-1 IN h
        else if ( helperH > 1 )
            {
            helperH -= 1.0;
            }
        else
            {
            }

        TReal val( 0 );

        // IF h*6<1: RETURN m1+(m2-m1)*h*6
        if ( ( helperH * 6 ) < 1 )
            {
            val = ( 255 * ( helperM1 + ( helperM2 - helperM1 ) * helperH * 6.0 ) );
            }

        // IF h*2<1: RETURN m2 )
        else if ( ( helperH * 2.0 ) < 1.0 )
            {
            val = ( 255.0 * helperM2 );
            }

        // IF h*3<2: RETURN m1+(m2-m1)*(2/3-h)*6
        else if ( ( helperH * 3.0 ) < 2.0 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) *
                            ( 2.0 / 3.0 - helperH ) * 6.0 ) );
            }
        else
            {
            val = 255.0 * helperM1;
            }
        TInt tmpVal = TInt( val + 0.5 );
        rgb.SetRed( ( tmpVal >= 0 ) ? tmpVal : -tmpVal );

        // calculate Green
        // PUT hue.to.rgb(m1, m2, h    ) IN g
        helperH = h;
        helperM1 = m1;
        helperM2 = m2;

        // IF h<0: PUT h+1 IN h
        if ( helperH < 0 )
            {
            helperH += 1.0;
            }

        // IF h>1: PUT h-1 IN h
        else if ( helperH > 1 )
            {
            helperH -= 1.0;
            }
        else
            {
            }

        // IF h*6<1: RETURN m1+(m2-m1)*h*6
        if ( ( helperH * 6 ) < 1 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) * helperH * 6.0 ) );
            }

        // IF h*2<1: RETURN m2
        else if ( ( helperH * 2 ) < 1 )
            {
            val = ( 255.0 * helperM2 );
            }

        // IF h*3<2: RETURN m1+(m2-m1)*(2/3-h)*6
        else if ( ( helperH * 3 ) < 2 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) *
                            ( 2.0 / 3.0 - helperH ) * 6.0 ) );
            }
        else
            {
            val = 255.0 * helperM1;
            }

        tmpVal = TInt( val + 0.5 );
        rgb.SetGreen( ( tmpVal >= 0 ) ? tmpVal : tmpVal );

        // calculate Blue
        // PUT hue.to.rgb(m1, m2, h-1/3) IN b
        helperH = h - ( 1.0 / 3.0 );
        helperM1 = m1;
        helperM2 = m2;

        // IF h<0: PUT h+1 IN h
        if ( helperH < 0 )
            {
            helperH += 1.0;
            }

        // IF h>1: PUT h-1 IN h
        else if ( helperH > 1 )
            {
            helperH -= 1.0;
            }
        else
            {
            }

        // IF h*6<1: RETURN m1+(m2-m1)*h*6
        if ( ( helperH * 6 ) < 1 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) * helperH * 6.0 ) );
            }

        // IF h*2<1: RETURN m2
        else if ( ( helperH * 2 ) < 1 )
            {
            val = ( 255.0 * helperM2 );
            }

        // IF h*3<2: RETURN m1+(m2-m1)*(2/3-h)*6
        else if ( ( helperH * 3 ) < 2 )
            {
            val = ( 255.0 * ( helperM1 + ( helperM2 - helperM1 ) *
                            ( 2.0 / 3.0 - helperH ) * 6.0 ) );
            }
        else
            {
            val = 255.0 * helperM1;
            }

        tmpVal = TInt( val + 0.5 );
        rgb.SetBlue( ( tmpVal >= 0 ) ? tmpVal : -tmpVal );
        }
    return rgb;
    }

// -----------------------------------------------------------------------------
// GetRgbValue
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnUtils::GetRgbValue( TRgb& aValue, const TDesC8& aData )
    {
    return GetRgbValueFromRgbFunction( aValue, aData ) ||
           GetRgbValueFromHslFunction( aValue, aData ) ||
           GetRgbValueFromHexValue( aValue, aData ) ||
           GetRgbValueFromColorName( aValue, aData );
    }

// -----------------------------------------------------------------------------
// Retrieves the string property of the node.
// -----------------------------------------------------------------------------
//
const TDesC8& GetStringProperty(
    CXnNodePluginIf& aNode,
    const TDesC8& aProperty )
    {
    CXnProperty* prop( NULL );
    TRAP_IGNORE( prop = aNode.GetPropertyL( aProperty ) );
    if ( prop )
        {
        CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
            prop->Property()->PropertyValueList().Item( 0 ) );
        if ( CXnDomPropertyValue::EIdent == value->PrimitiveValueType() ||
             CXnDomPropertyValue::EString == value->PrimitiveValueType() )
            {
            return prop->StringValue();
            }
        }
    return KNullDesC8;
    }

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// Creates the font according to node information.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnUtils::CreateFontL(
    CXnNodePluginIf& aNode,
    CFont*& aFont,
    TInt& aReleaseFont )
    {
    // Get the font properties from node
    TFontSpec fontspec;

    //Font name
    const TDesC8& fontNameS = GetStringProperty(
        aNode, XnPropertyNames::appearance::common::KFontFamily );
    HBufC* fontName = HBufC::NewLC( fontNameS.Length() );
    TPtr namePtr = fontName->Des();
    namePtr.Copy( fontNameS ); // 8-bit to 16-bit
    StripQuotes( fontName );
    TInt fLenght = fontName->Length();
    TInt avkonFId = AvkonFontId( *fontName );

    //let's see if normal font needed
    CCoeEnv* coeEnv = CEikonEnv::Static();
    if ( *fontName == KNormal ||
         !fLenght ||
         ( KErrNotFound == avkonFId && fLenght > 24 ) )
        {
        aFont = const_cast< CFont* >( coeEnv->NormalFont() );
        aReleaseFont = EFalse;
        CleanupStack::PopAndDestroy( fontName );
        return;
        }

    //Font size in pixels
    CXnProperty* fontSizeProp = aNode.GetPropertyL(
        XnPropertyNames::appearance::common::KFontSize );
    TInt fontSizeInPixels( aNode.Rect().Height() );//default font size

    if ( fontSizeProp )
        { //font size from node if needed. If leaves uses default.
        TRAP_IGNORE( fontSizeInPixels = aNode.UiEngineL()->VerticalPixelValueL(
            fontSizeProp, aNode.Rect().Height()) );
        }

    //Font stroke
    const TDesC8& fontStroke = GetStringProperty(
        aNode, XnPropertyNames::appearance::common::KFontWeight );
    if ( fontStroke == XnPropertyNames::appearance::common::fontweight::KBold )
        {
        fontspec.iFontStyle.SetStrokeWeight( EStrokeWeightBold );
        }

    //Font posture
    const TDesC8& fontPosture = GetStringProperty(
        aNode, XnPropertyNames::appearance::common::KFontStyle );
    if ( fontPosture == XnPropertyNames::appearance::common::fontstyle::KItalic )
        {
        fontspec.iFontStyle.SetPosture( EPostureItalic );
        }

    //Font effects
    CXnProperty* textEffectsProp = aNode.GetPropertyL(
        XnPropertyNames::appearance::common::KTextEffects );
    if ( textEffectsProp )
        {
        CBitmapDevice* dev = coeEnv->ScreenDevice();
        fontspec.iHeight = fontSizeInPixels;

        CXnDomList& propertyValueList =
            textEffectsProp->Property()->PropertyValueList();
        TInt valueCount = propertyValueList.Length();
        TUint32 effectsFlag( 0 );

        for ( TInt i = 0; i < valueCount; ++i )
            {
            CXnDomPropertyValue* value = static_cast< CXnDomPropertyValue* >(
                propertyValueList.Item( i ) );

            if ( value->StringValueL() ==
                 XnPropertyNames::appearance::common::fonteffects::KEffectsAlgorithmicbold )
                {
                effectsFlag |= FontEffect::EAlgorithmicBold;
                }
            if ( value->StringValueL() ==
                 XnPropertyNames::appearance::common::fonteffects::KEffectsDropshadow )
                {
                effectsFlag |= FontEffect::EDropShadow;
                }
            if ( value->StringValueL() ==
                 XnPropertyNames::appearance::common::fonteffects::KEffectsOutline )
                {
                effectsFlag |= FontEffect::EOutline;
                }
            if ( value->StringValueL() == 
                 XnPropertyNames::appearance::common::fonteffects::KEffectsEmbossed )
                {
                effectsFlag |= FontEffect::EEmbossed;
                }
            if ( value->StringValueL() ==
                 XnPropertyNames::appearance::common::fonteffects::KEffectsEngraved )
                {
                effectsFlag |= FontEffect::EEngraved;
                }
            if ( value->StringValueL() == 
                 XnPropertyNames::appearance::common::fonteffects::KEffectsSoftedge )
                {
                effectsFlag |= FontEffect::ESoftEdge;
                }
            }
        // Must be antialiased for effects to work
        fontspec.iFontStyle.SetBitmapType( EAntiAliasedGlyphBitmap );
        fontspec.iFontStyle.SetEffects( effectsFlag );
        User::LeaveIfError( dev->GetNearestFontToDesignHeightInPixels(
            aFont, fontspec ) );
        aReleaseFont = ETrue;
        }
    else
        {
        CreateFontL( *fontName, fontSizeInPixels,
                     fontspec.iFontStyle, aFont, aReleaseFont );
        }

    CleanupStack::PopAndDestroy( fontName );
    }

// -----------------------------------------------------------------------------
// Creates the font according to given information.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnUtils::CreateFontL(
    const TDesC& aFontName,
    TInt aFontSizeInPixels,
    TFontStyle aFontStyle,
    CFont*& aFont,
    TInt& aReleaseFont )
    {
    CCoeEnv* coeEnv = CEikonEnv::Static();
    if ( aFontName == KNormal || !aFontName.Length() )
        {
        aFont = const_cast< CFont* >( coeEnv->NormalFont() );
        aReleaseFont = EFalse;
        }
    else
        {
        //Avkon font id used
        TInt fontId = AvkonFontId( aFontName );
        if ( fontId != KErrNotFound )
            {
            fontId += ( ( aFontSizeInPixels & 0x3ff ) << 21 ) +
                      ( ( aFontStyle.StrokeWeight() & 0x1 ) << 4 ) +
                      ( ( aFontStyle.Posture() & 0x1 ) << 5 ) + 0x80000000;
            aFont = const_cast< CFont* >( AknLayoutUtils::FontFromId( fontId ));
            aReleaseFont = EFalse;
            }
        else
            {
            //Get font
            CBitmapDevice* dev = coeEnv->ScreenDevice();
            TInt fontSizeInTwips = dev->VerticalPixelsToTwips( aFontSizeInPixels );
            aFont =  AknFontAccess::GetFont(
                *dev, aFontStyle, fontSizeInTwips, aFontName );
            if ( !aFont )
                {
                aFont = AknFontAccess::GetClosestFont( *dev, aFontStyle,
                    fontSizeInTwips, aFontName );
                }
            aReleaseFont = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// Collapses white spaces from the string.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnUtils::CollapseWhiteSpace(
    CXnNodePluginIf& aNode,
    TDes &aDes )
    {
    const TDesC8& value = GetStringProperty( aNode,
        XnPropertyNames::appearance::common::KWhiteSpaceCollapse );

    //Collapse -default
    if ( value == XnPropertyNames::appearance::common::whitespace::KCollapse ||
         !value.Length() )
        {
        TInt src = 0;
        TInt srclength = aDes.Length();
        while ( src < srclength )
            {
            TChar c = aDes[src];
            if ( KXnLineFeedTabCharacters().LocateF( c ) != KErrNotFound )
                {
                aDes[src] = KSpaceChar;
                }
            ++src;
            }
        aDes.TrimAll();
        }
    //Preserve breaks
    else if ( value ==
              XnPropertyNames::appearance::common::whitespace::KPreserveBreaks )
        {
        TInt src = 0;
        TInt srclength = aDes.Length();
        while ( src < srclength )
            {
            TChar c = aDes[src];
            if ( KXnTabCharacters().LocateF( c ) != KErrNotFound )
                {
                aDes[src] = KSpaceChar;
                }
            ++src;
            }
        aDes.TrimAll();
        }
    //discard
    else if ( value == XnPropertyNames::appearance::common::whitespace::KDiscard )
        {
        TInt target = 0;
        TInt src = 0;
        TInt srclength = aDes.Length();
        while ( src < srclength )
            {
            aDes[target] = aDes[src];
            TChar c = aDes[src];
            if ( KXnCommonWhiteSpaceCharacters().LocateF( c ) == KErrNotFound )
                {
                ++target;
                }
            ++src;
            }
        aDes.SetLength( target );
        }
    //Preserve
    //Do nothing.
    }

// -----------------------------------------------------------------------------
// Resolve TextAlignment property.
// -----------------------------------------------------------------------------
//
EXPORT_C TGulAlignmentValue CXnUtils::TextAlignment( CXnNodePluginIf& aNode )
    {
    TInt value;

    const TDesC8& alignment = GetStringProperty( aNode,
                  XnPropertyNames::appearance::common::KTextAlign );

    if ( alignment ==
         XnPropertyNames::appearance::common::textalign::KAlignLeft )
        {
        value = EHLeft;
        }
    else if ( alignment ==
              XnPropertyNames::appearance::common::textalign::KAlignRight )
        {
        value = EHRight;
        }
    else if ( alignment ==
              XnPropertyNames::appearance::common::textalign::KAlignCenter )
        {
        value = EHCenter;
        }
    else if ( alignment ==
              XnPropertyNames::appearance::common::textalign::KAlignJustify )
        {
        //We don't have EJustify
        value = EHCenter;
        }
    else
        {
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            value = EHRight;
            }
        else
            {
            value = EHLeft;
            }
        }

    const TDesC8& valignment = GetStringProperty( aNode,
        XnPropertyNames::appearance::common::KS60TextVAlign );

    if ( valignment ==
         XnPropertyNames::appearance::common::textvalign::KAlignTop )
        {
        value |= EVTop;
        }
    else if ( valignment ==
              XnPropertyNames::appearance::common::textvalign::KAlignBottom )
        {
        value |= EVBottom;
        }
    else
        {
        value |= EVCenter;
        }

    return ( TGulAlignmentValue )value;
    }

// -----------------------------------------------------------------------------
// ScaleBitmapL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnUtils::ScaleBitmapL(
    const TRect& aTrgRect,
    CFbsBitmap* aTrgBitmap,
    CFbsBitmap* aSrcBitmap )
    {
    CXnUtils::ScaleBitmapExtL( aTrgRect, aTrgBitmap, aSrcBitmap, EFalse );
    }

// -----------------------------------------------------------------------------
// Strips the " and ' quotes from the string.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnUtils::StripQuotes( HBufC*& aString )
    {
    TInt pos1 = aString->Locate( '\"' );
    TInt pos2 = aString->Locate( '\'' );
    // check, if the first character is " or '
    if ( pos1 == 0 || pos2 == 0 )
        {
        TInt len = aString->Length() - 2;
        TPtr ptr = aString->Des();
        TPtrC16 newString( aString->Mid( 1, len ).Ptr(), len );
        ptr = newString;
        }
    }

// -----------------------------------------------------------------------------
// Resolves skin id (major+minor) and index from SKIN() declaration.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnUtils::ResolveSkinItemIDL(
    const TDesC& aSkinId,
    TAknsItemID& aItemID,
    TInt& aIndex )
    {
    HBufC* str = aSkinId.AllocL();
    TPtr ptrSkin = str->Des();
    ptrSkin.TrimAll();
    TInt pos( ptrSkin.FindF( KSkin ) );
    if ( KErrNotFound != pos )
        {
        TInt majorID( 0 );
        TInt minorID( 0 );
        pos += KSkin().Length();
        TPtr ptr = ptrSkin.MidTPtr( pos, ( ptrSkin.Length() - pos - 1 ) );
        ptr.TrimAll();
        TInt offset( ptr.Locate( KSpaceChar ) );
        if ( KErrNotFound != offset )
            {
            TLex lex( ptr.Left( offset ) );
            lex.SkipSpace();
            lex.Val( majorID ); // ok until this.

            TInt offset2( ptr.LocateReverse( KSpaceChar ) );
            if ( offset2 == 0 || offset2 == offset )
                {
                lex.Assign( ptr.Mid( offset ) );
                lex.SkipSpace();
                lex.Val( minorID );
                }
            else
                {
                TPtrC ptrDebug = ptr.Mid( offset + 1, offset2 - offset - 1 );
                lex.Assign( ptr.Mid( offset + 1, offset2 - offset -1 ) );
                lex.Val( minorID );

                TPtrC ptrDebug2 = ptr.Mid( offset2 + 1, ptr.Length() - offset2 - 1 );
                lex.Assign( ptr.Mid( offset2 + 1, ptr.Length() - offset2 - 1 ) );
                lex.Val( aIndex );
                }
            aItemID.Set( majorID, minorID );
            delete str;
            return ETrue;
            }
        }
    delete str;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// Propagates powersave mode to adapters
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CXnUtils::HandlePowerSaveModeL(
    CXnNode* aNode,
    TBool aEnter,
    TInt aEvent )
    {

    CXnNodeBreadthFirstIterator< CXnNode >* iterator = 
            CXnNodeBreadthFirstIterator< CXnNode >::NewL( *aNode );

    CleanupStack::PushL( iterator );

    for ( CXnNode* currentNode = iterator->Value(); currentNode; currentNode = iterator->NextL() )
        {
        if ( !currentNode->ComponentNodeImpl() )
            {
            continue;
            }

        CXnComponent* component = currentNode->ComponentNodeImpl()->Component();

        if ( !component )
            {
            continue;
            }

        CXnControlAdapter* adapter = component->ControlAdapter();

        if ( !adapter )
            {
            continue;
            }

        if ( aEnter )
            {
            adapter->EnterPowerSaveModeL(
                ( CXnControlAdapter::TModeEvent ) aEvent );
            }
        else
            {
            adapter->ExitPowerSaveModeL(
                ( CXnControlAdapter::TModeEvent ) aEvent );
            }
        }

    CleanupStack::PopAndDestroy( iterator );

    return aEnter;
    }

// -----------------------------------------------------------------------------
// Resolves skin id (major+minor) from SKIN() declaration.
// -----------------------------------------------------------------------------
//
TBool CXnUtils::ResolveSkinItemIDL( const TDesC& aSkinId, TAknsItemID& aItemID )
    {
    HBufC* str = aSkinId.AllocL();
    TPtr ptrSkin = str->Des();
    ptrSkin.TrimAll();
    TInt pos( ptrSkin.FindF( KSkin ));
    if ( KErrNotFound != pos )
        {
        TInt majorID( 0 );
        TInt minorID( 0 );
        pos += KSkin().Length();
        TPtr ptr = ptrSkin.MidTPtr( pos, ( ptrSkin.Length() - pos - 1 ) );
        ptr.TrimAll();
        TInt offset( ptr.Locate( KSpaceChar ) );
        if ( KErrNotFound != offset )
            {
            TLex lex( ptr.Left( offset ) );
            lex.SkipSpace();
            lex.Val( majorID );
            lex.Assign( ptr.Mid( offset ) );
            lex.SkipSpace();
            lex.Val( minorID );
            aItemID.Set( majorID, minorID );
            delete str;
            return ETrue;
            }
        }
    delete str;
    return EFalse;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CXnUtils::DoesScaleBitmapUseFallBack( CFbsBitmap* aSrcBitmap )
    {
    if ( !aSrcBitmap )
        {
        return EFalse;
        }

    TDisplayMode displayMode = aSrcBitmap->DisplayMode();
    TBool fallbackOnly = EFalse;

    switch ( displayMode )
        {
        case EGray2:
        case EGray4:
        case EGray16:
        case EColor16:
        case EColor16M:
        case ERgb:
        case EColor16MA:
            fallbackOnly = ETrue;
            break;
        case EGray256:
        case EColor4K:
        case EColor64K:
        case EColor256:
        case EColor16MU:
            // These are the supported modes
            break;
        default:
            fallbackOnly = ETrue;
        }

    return fallbackOnly;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CXnUtils::ScaleBitmapExtL( const TRect& aTrgRect, CFbsBitmap* aTrgBitmap,
        CFbsBitmap* aSrcBitmap, TBool aForceFallBack )
    {
    if ( !aSrcBitmap ) User::Leave( KErrArgument );
    if ( !aTrgBitmap ) User::Leave( KErrArgument );
    if ( aSrcBitmap->DisplayMode() != aTrgBitmap->DisplayMode() )
        {
        User::Leave( KErrArgument );
        }

    TSize trgBitmapSize = aTrgBitmap->SizeInPixels();

    // calculate the valid drawing area
    TRect drawRect = aTrgRect;
    drawRect.Intersection( TRect( TPoint( 0, 0 ), trgBitmapSize ) );

    if( drawRect.IsEmpty() || aSrcBitmap->SizeInPixels().iHeight <= 0 || 
            aSrcBitmap->SizeInPixels().iWidth <= 0 )
        {
        return;
        }

    TSize srcSize = aSrcBitmap->SizeInPixels();

    TBool srcTemporary = EFalse;
    if ( aSrcBitmap->IsRomBitmap() )
        {
        srcTemporary = ETrue;
        }

    TDisplayMode displayMode = aSrcBitmap->DisplayMode();
    TBool fallbackOnly =
        aForceFallBack ||
        DoesScaleBitmapUseFallBack( aSrcBitmap );

    if ( fallbackOnly )
        {
        CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL( aTrgBitmap );
        CleanupStack::PushL( dev );
        CFbsBitGc* gc = NULL;
        User::LeaveIfError( dev->CreateContext( gc ) );
        CleanupStack::PushL( gc );

        // write alpha information if it exists
        if ( aSrcBitmap->DisplayMode() == EColor16MA )
            {
            gc->SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
            }

        // aTrgRect is used because DrawBitmap handles clipping automatically
        gc->DrawBitmap( aTrgRect, aSrcBitmap );
        CleanupStack::PopAndDestroy( 2 ); // dev, gc
        return;
        }

    // Heap lock for FBServ large chunk to prevent background
    // compression of aSrcBitmap after if IsCompressedInRAM returns EFalse
    aSrcBitmap->LockHeapLC( ETrue ); // fbsheaplock
    TBool fbsHeapLock = ETrue;
    if ( aSrcBitmap->IsCompressedInRAM() )
        {
        srcTemporary = ETrue;
        }

    CFbsBitmap* realSource = aSrcBitmap;
    if ( srcTemporary )
        {
        CleanupStack::PopAndDestroy(); // fbsheaplock
        fbsHeapLock = EFalse;

        realSource = new ( ELeave ) CFbsBitmap();
        CleanupStack::PushL( realSource );
        User::LeaveIfError(
            realSource->Create( srcSize, aSrcBitmap->DisplayMode() ) );
        CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL( realSource );
        CleanupStack::PushL( dev );
        CFbsBitGc* gc = NULL;
        User::LeaveIfError( dev->CreateContext( gc ) );
        CleanupStack::PushL( gc );
        gc->BitBlt( TPoint( 0, 0 ), aSrcBitmap );
        CleanupStack::PopAndDestroy( 2 ); // dev, gc
        }

    if ( !fbsHeapLock )
        {
        // Heap lock for FBServ large chunk is only needed with large bitmaps.
        if ( realSource->IsLargeBitmap() || aTrgBitmap->IsLargeBitmap() )
            {
            aTrgBitmap->LockHeapLC( ETrue ); // fbsheaplock
            }
        else
            {
            CleanupStack::PushL( ( TAny* )NULL );
            }
        }

    TUint32* srcAddress = realSource->DataAddress();
    TUint32* trgAddress = aTrgBitmap->DataAddress();

    const TInt xSkip = ( srcSize.iWidth << 8 ) / aTrgRect.Width();
    const TInt ySkip = ( srcSize.iHeight << 8 ) / aTrgRect.Height();

    const TInt drawWidth  = drawRect.Width();
    const TInt drawHeight = drawRect.Height();

    TRect offsetRect( aTrgRect.iTl, drawRect.iTl );
    const TInt yPosOffset = ySkip * offsetRect.Height();
    const TInt xPosOffset = xSkip * offsetRect.Width();

    if ( ( displayMode == EGray256 ) || ( displayMode == EColor256 ) )
        {
        TInt srcScanLen8 = CFbsBitmap::ScanLineLength(
            srcSize.iWidth, displayMode );
        TInt trgScanLen8 = CFbsBitmap::ScanLineLength(
            trgBitmapSize.iWidth, displayMode );

        TUint8* trgAddress8 = reinterpret_cast< TUint8* >( trgAddress );

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress8 += trgScanLen8 * drawRect.iTl.iY + drawRect.iTl.iX;

        for ( TInt y = 0; y < drawHeight; y++ )
            {
            TUint8* srcAddress8 = reinterpret_cast< TUint8* >( srcAddress ) +
                ( srcScanLen8 * ( yPos >> 8 ) );

            TInt xPos = xPosOffset;
            for ( TInt x = 0; x < drawWidth; x++ )
                {
                *( trgAddress8++ ) = srcAddress8[xPos >> 8];
                xPos += xSkip;
                }

            yPos += ySkip;

            trgAddress8 += trgScanLen8 - drawWidth;
            }
        }
    else if ( displayMode == EColor4K || displayMode == EColor64K )
        {
        TInt srcScanLen16 = CFbsBitmap::ScanLineLength(
            srcSize.iWidth, displayMode ) /2;
        TInt trgScanLen16 = CFbsBitmap::ScanLineLength(
            trgBitmapSize.iWidth, displayMode ) /2;

        TUint16* trgAddress16 = reinterpret_cast< TUint16* >( trgAddress );

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress16 += trgScanLen16 * drawRect.iTl.iY + drawRect.iTl.iX;

        for ( TInt y = 0; y < drawHeight; y++ )
            {
            TUint16* srcAddress16 = reinterpret_cast< TUint16* >( srcAddress ) +
                ( srcScanLen16 * ( yPos >> 8 ) );

            TInt xPos = xPosOffset;
            for ( TInt x = 0; x < drawWidth; x++ )
                {
                *( trgAddress16++ ) = srcAddress16[xPos >> 8];
                xPos += xSkip;
                }

            yPos += ySkip;

            trgAddress16 += trgScanLen16 - drawWidth;
            }
        }
    else if ( displayMode == EColor16MU )
        {
        TInt srcScanLen32 = CFbsBitmap::ScanLineLength(
            srcSize.iWidth, displayMode ) /4;
        TInt trgScanLen32 = CFbsBitmap::ScanLineLength(
            trgBitmapSize.iWidth, displayMode ) /4;

        TUint32* trgAddress32 = reinterpret_cast< TUint32* >( trgAddress );

        TInt yPos = yPosOffset;
        // skip left and top margins in the beginning
        trgAddress32 += trgScanLen32 * drawRect.iTl.iY + drawRect.iTl.iX;

        for ( TInt y = 0; y < drawHeight; y++ )
            {
            TUint32* srcAddress32 = reinterpret_cast< TUint32* >( srcAddress ) +
                ( srcScanLen32 * ( yPos >> 8 ) );

            TInt xPos = xPosOffset;
            for ( TInt x = 0; x < drawWidth; x++ )
                {
                *( trgAddress32++ ) = srcAddress32[xPos >> 8];
                xPos += xSkip;
                }

            yPos += ySkip;

            trgAddress32 += trgScanLen32 - drawWidth;
            }
        }
    else
        {
        User::Leave( KErrUnknown );
        }

    CleanupStack::PopAndDestroy(); // fbsheaplock

    if ( srcTemporary )
        {
        CleanupStack::PopAndDestroy(); // realSource
        }
    }
