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
 * Description: Menu View Builder
 *
 */

#ifndef HSMENUVIEWBUILDER_H
#define HSMENUVIEWBUILDER_H

#include <QFlags>
#include <QMap>
#include <QString>
#include <qnamespace.h>
#include <hbdocumentloader.h>

class HbAction;
class HbGroupBox;
class HbLineEdit;
class HbListView;
class HbSearchPanel;
class HbToolBar;
class HbView;
class HbWidget;

class HsMenuViewBuilder
{
public:
    HsMenuViewBuilder();
    ~HsMenuViewBuilder();

    bool setLabelVisible(bool);
    bool setSearchPanelVisible(bool);
    bool isLabelVisible() const;
    bool isSearchPanelVisible() const;

    HbView *view() const;
    HbListView *listView() const;
    HbGroupBox *label() const;
    HbSearchPanel *searchPanel() const;
    HbAction *allAppsAction() const;
    HbAction *allCollectionsAction() const;
    HbAction *searchAction() const;
    HbAction *oviStoreAction() const;
    HbToolBar *toolBar() const;
    HbLineEdit *searchPanelLineEdit() const;
private:

    bool parseSection(const QString &sectionName);

    HbDocumentLoader mDocumentLoader;

    QObject mParent;

    enum ViewOption {
        Default = 0x0,
        Label = 0x1,
        Search = 0x2
    };

    Q_DECLARE_FLAGS(ViewOptions, ViewOption)
    ViewOptions mViewOptions;
    QMap<ViewOptions, QString> mSections;

    void buildSectionKeyMap();

    const QString DOCUMENT_FILE_NAME;
    const QString ALL_APPS_ACTION_NAME;
    const QString ALL_COLLECTIONS_ACTION_NAME;
    const QString SEARCH_ACTION_NAME;
    const QString OVI_STORE_ACTION_NAME;
    const QString VIEW_NAME;
    const QString CONTAINER_NAME;
    const QString LIST_VIEW_NAME;
    const QString TOOL_BAR_NAME;
    const QString SEARCH_PANEL_NAME;
    const QString LABEL_NAME;
    const QString LIST_VIEW_SECTION_NAME;
    const QString LIST_LABELED_VIEW_SECTION_NAME;
    const QString LIST_SEARCH_VIEW_SECTION_NAME;
    const QString LIST_SEARCH_LABELED_VIEW_SECTION_NAME;
};

#endif // HSMENUVIEWBUILDER_H
