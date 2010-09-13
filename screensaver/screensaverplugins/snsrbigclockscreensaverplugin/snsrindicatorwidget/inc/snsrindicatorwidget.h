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

#ifndef SNSRINDICATORWIDGET_H
#define SNSRINDICATORWIDGET_H

#include <hbwidget.h>
#include "snsrtest_global.h"

SCREENSAVER_TEST_CLASS(T_SnsrIndicatorWidget)

class HbIconItem;
class SnsrIndicatorInfo;


class SnsrIndicatorWidget: public HbWidget
{
    Q_OBJECT
    
    Q_ENUMS(IndicatorLayoutType)
    Q_PROPERTY(IndicatorLayoutType layoutType READ layoutType WRITE setLayoutType)
   
public:
    
    enum IndicatorLayoutType
    {
        IndicatorsAlignedLeft,
        IndicatorsCentered
    };

public:

    explicit SnsrIndicatorWidget(QGraphicsItem *parent = 0);
    virtual ~SnsrIndicatorWidget();
    
    IndicatorLayoutType layoutType() const;
    void setLayoutType(IndicatorLayoutType type);

    void setPowerSaveModeColor(bool savemode);

public slots:
    
    void showIndicators(const QList<SnsrIndicatorInfo> &indicators);
    void removeAllIndicators();
    
private:

    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;
    void createPrimitives();
   
private:

    QList<HbIconItem*> mIcons;   
    IndicatorLayoutType mLayoutType;
    bool mPowerSaveModeColor;

    Q_DISABLE_COPY(SnsrIndicatorWidget)
    SCREENSAVER_TEST_FRIEND_CLASS(T_SnsrIndicatorWidget)
};

#endif // SNSRINDICATORWIDGET_H
