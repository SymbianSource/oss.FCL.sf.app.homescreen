/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Indicator Widget.
*
*/

#include "snsrindicatorwidget.h"
#include "snsrindicatorinfo.h"

#include <HbColorScheme>
#include <QDebug>
#include <QSizePolicy>
#include <HbStyleLoader>
#include <HbIconItem>
#include <HbIndicatorInterface>


const char *gIndicatorCssFilePath =":/style/snsrindicatorwidget.css";
const char *gIndicatorWidgetMLFilePath = ":/style/snsrindicatorwidget.widgetml";
const char *gIndicatorColorCssFilePath = ":/style/snsrindicatorwidget_color.css";

const int gNumberOfIcons(6);

/*!
    \class SnsrIndicatorWidget
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver indicator widget.
 */

/*!
    Constructs a new SnsrIndicatorWidget.
    \param parent Parent object.
 */
SnsrIndicatorWidget::SnsrIndicatorWidget(QGraphicsItem* parent):
    HbWidget(parent), mLayoutType(IndicatorsCentered)
{
    HbStyleLoader::registerFilePath(gIndicatorCssFilePath);
    HbStyleLoader::registerFilePath(gIndicatorWidgetMLFilePath);
    
    // TODO: will some color variables be added into hbcolorgroup.css so
    // that one don't need to use hard-coded values? If not, then
    // changes needed in this widget.
    HbStyleLoader::registerFilePath(gIndicatorColorCssFilePath);
   
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
          
    createPrimitives();
}

/*!
    Destructs the class.
 */
SnsrIndicatorWidget::~SnsrIndicatorWidget()
{    
    // icons within the list are deleted by parent
    mIcons.clear();
    
    HbStyleLoader::unregisterFilePath( gIndicatorCssFilePath );
    HbStyleLoader::unregisterFilePath( gIndicatorWidgetMLFilePath );
    HbStyleLoader::unregisterFilePath(gIndicatorColorCssFilePath);
}


SnsrIndicatorWidget::IndicatorLayoutType SnsrIndicatorWidget::layoutType() const
{
    return mLayoutType;
}

/*!
    Sets the name of layout definition \a layoutName for selecting 
    the layout of view items of this view from css/xml files.

    This layout is accessible from css file as layout property
    of the view item.

 */
void SnsrIndicatorWidget::setLayoutType(IndicatorLayoutType type)
{
    if (mLayoutType != type ) {
        mLayoutType = type;
        QSizePolicy::Policy hSizePolicy(QSizePolicy::Expanding);
        if ( mLayoutType == IndicatorsAlignedLeft ) {
            hSizePolicy = QSizePolicy::Preferred;
        }
        setSizePolicy(hSizePolicy,QSizePolicy::Preferred);
    }
} 

/*!
    This is called whenever indicators' status changes: parameter list
    tells which indicators should be shown at the given moment. All
    previously shown indicators should be hidden and show only these
    indicators passed as a parameter. 
 */
void SnsrIndicatorWidget::showIndicators(const QList<SnsrIndicatorInfo> &indicators)
{
    // Update icon paths so that shown iconItems have
    // valid icon path and rest have empty path (->invisible)
    
    int indicatorAmount = indicators.size();
    
    removeAllIndicators();

    // TODO: after wk18 color is lost after icon name is set again - bug or what?
    // workaround: ask color from theme and set it again here
    // If this must be left here, then change back-up color to white?
    QColor iconColor = HbColorScheme::color("snsrforeground"); // TODO: final variable name?
    if (iconColor.spec() == QColor::Invalid ) {
        iconColor = Qt::magenta;
    }
    // update icon paths for items that are gonna be shown
    for (int i=0; i < indicatorAmount && i < mIcons.size(); ++i) {
        mIcons.at(i)->setIconName(indicators.at(i).iconPath);
        mIcons.at(i)->setFlags(HbIcon::Colorized);
        mIcons.at(i)->setColor(iconColor);        
    }

    // To recalculate layouts
    updateGeometry();
    if ( parentLayoutItem() ) {
        parentLayoutItem()->updateGeometry(); // clear the cache of the parent
    }
}

/*!
    Remove all indicators from UI.
 */
void SnsrIndicatorWidget::removeAllIndicators()
{
    for (int i=0; i < mIcons.size(); ++i) {
            mIcons.at(i)->setIconName(QString());
    }
}

/*!
    If indicator pane is going to be centered, then calculate width so that it's
    exactly the same as visible icons are taking space (+spacers between them).
    If indicator pane is left aligned, then actual width doesn't matter.
    Assumption is that icons are fixed sized.
    \reimp
*/
QSizeF SnsrIndicatorWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    QSizeF hint = HbWidget::sizeHint(which, constraint);
    
    // TODO: seems to work if icons are fixed-sized (units)
    // TODO: even though size policy is preferred, should we give also
    // minimum and maximum sizes?
    if (which == Qt::PreferredSize && layoutType() == IndicatorsCentered ) {
        qreal width(0);
        qreal itemWidth(0);
        qreal iconSpace(0);
        int visibleIconCount(0);
        
        // TODO: use geometry or size?
        for (int i=0; i < mIcons.size(); ++i) {
               if (!mIcons.at(i)->iconName().isEmpty() ) {
                   visibleIconCount++;
                   itemWidth = mIcons.at(i)->iconItemSize().width();
                   width = width + itemWidth;
               }
        }
        if (visibleIconCount > 1) {
            style()->parameter("hb-param-margin-gene-middle-horizontal", iconSpace);
            width = width + ((visibleIconCount-1)*iconSpace);
        }
        
        hint.setWidth(width);
    }
    
    return hint;
}

/*!
    Creates all widget primitives.
    Coloring is handled via CSS.
 */
void SnsrIndicatorWidget::createPrimitives()
{
    if (!mIcons.isEmpty()) {
        return;
    }
    
    HbIconItem *icon(0);
    QLatin1String itemNameBase("icon");
    int itemNameEnd(1);
    QString itemName("");

    while (mIcons.count() < gNumberOfIcons) {
        // When created, icon names are left empty -> nothing shown in UI.
        icon = new HbIconItem(this);
        icon->setFlags(HbIcon::Colorized);
        icon->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
            
        // Create items that are named icon1..icon6.
        itemName.clear();
        itemName.append(itemNameBase);
        itemName.append(QString::number(itemNameEnd));
        HbStyle::setItemName(icon, itemName);
        ++itemNameEnd;
        
        mIcons.append(icon);
    }
}
