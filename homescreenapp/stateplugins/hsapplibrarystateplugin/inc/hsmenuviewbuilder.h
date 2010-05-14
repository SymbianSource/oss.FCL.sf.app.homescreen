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
#include <QSet>
#include <QString>
#include <qnamespace.h>
#include <HbDocumentLoader>

class QActionGroup;
class HbAction;
class HbGroupBox;
class HbLineEdit;
class HbListView;
class HbSearchPanel;
class HbToolBar;
class HbView;
class HbWidget;
class HbToolBarExtension;

enum HsViewContext {
    HsAllAppsContext,
    HsAllCollectionsContext,
    HsInstalledAppsContext,
    HsCollectionContext,
};

enum HsOperationalContext {
    HsItemViewContext,
    HsSearchContext
};


class HsMenuViewBuilder
{
public:
    HsMenuViewBuilder();
    ~HsMenuViewBuilder();

    // mandatory context independent widgets accessors
    HbAction *allAppsAction() const;
    HbAction *allCollectionsAction() const;
    HbAction *searchAction() const;
    HbAction *oviStoreAction() const;
    HbAction *operatorAction() const;
    HbToolBar *toolBar() const;
    QActionGroup *toolBarActionGroup() const;
    HbToolBarExtension *toolBarExtension() const;

    // mandatory context dependent widgets accessors
    HbView *currentView() const;
    HbListView *currentListView() const;


    // optional widgets accessors
    HbGroupBox *currentViewLabel() const;
    HbSearchPanel *currentSearchPanel() const;


    void setViewContext(HsViewContext viewContext);
    void setOperationalContext(HsOperationalContext operationalContext);
    bool build();

private:
    void init();

    bool parseSection(const QString &sectionName = QString());


    bool readContextConfiguration(HsViewContext, HsOperationalContext);


    void searchPanelVisibilityChange(bool visible);

    HbLineEdit *searchPanelLineEdit() const;

    QMap<HsViewContext, QString > mViewContextToStringMap;
    QMap<HsOperationalContext, QString > mOperationalContextToStringMap;

    HbDocumentLoader mDocumentLoader;
    QSet<QObject *> mLoadedObjects;

    const QString DOCUMENT_FILE_NAME;
    const QString COMMON_SECTION_NAME;
    const QString ALL_APPS_ACTION_NAME;
    const QString ALL_COLLECTIONS_ACTION_NAME;
    const QString SEARCH_ACTION_NAME;
    const QString OVI_STORE_ACTION_NAME;
    const QString OPERATOR_ACTION_NAME;
    const QString SEARCH_PANEL_NAME;
    const QString TOOL_BAR_NAME;

    QActionGroup *mToolBarActionGroup;
    HbToolBar *mToolBar;
    HbToolBarExtension *mToolBarExtension;
    HbAction *mToolBarExtensionAction;

    HsViewContext mViewContext;
    HsOperationalContext mOperationalContext;
};

#endif // HSMENUVIEWBUILDER_H
