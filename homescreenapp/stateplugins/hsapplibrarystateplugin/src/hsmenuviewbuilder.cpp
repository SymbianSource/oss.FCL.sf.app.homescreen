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

#include <QDebug>
#include <QActionGroup>
#include <QGraphicsWidget>
#include <HbAction>
#include <HbGroupBox>
#include <HbListView>
#include <HbLineEdit>
#include <HbSearchPanel>
#include <HbPushButton>
#include <HbToolBar>
#include <HbView>
#include <HbWidget>
#include <HbStaticVkbHost>
#include <HbMainWindow>
#include <HbInputMethod>
#include <HbToolBarExtension>

#include "hsmenuviewbuilder.h"
#include "hsmenustates_global.h"


/*!
    \class HsMenuViewBuilder
    \ingroup group_hsmenustateplugin

    \brief Menu View Builder

    Reads UI object from Application Library docml

    \sa HsMenuView
*/


/*!
  Cleanup search text on showing search panel. Ensures vkb host instance
  is appears on show and disappears on hide search panel.
  
  \param visible When true search panel is to show up,
  hide otherwise.
 */
void HsMenuViewBuilder::searchPanelVisibilityChange(bool visible)
{
    if (visible) {

        HbLineEdit *const lineEdit(searchPanelLineEdit());

        lineEdit->setText("");
        lineEdit->setFocus();
    } 

}

/*!
 \return pointer to the view resulting from last \a build call or NULL if 
 the \a build has not yet been called.
 Memory ownership is not changed.
 */
HbView *HsMenuViewBuilder::currentView() const
{
    const QString VIEW_NAME =
        mViewContextToStringMap[mViewContext]
        + "View";

    HbView *const view =
        qobject_cast<HbView *>(mDocumentLoader.findWidget(VIEW_NAME));

    if (view != NULL && mViewContext != HsInstalledAppsContext) {
        view->setToolBar(mToolBar);
    }
    return view;
}

/*!
 \return pointer to list view resulting from last \a build call or NULL if 
 the \a build has not yet been called.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbListView *HsMenuViewBuilder::currentListView() const
{
    QString LIST_VIEW_NAME = mViewContextToStringMap[mViewContext];
    if (mOperationalContext == HsSearchContext) {
        LIST_VIEW_NAME.append(mOperationalContextToStringMap[mOperationalContext]);
    }
    LIST_VIEW_NAME.append("ListView");

    return qobject_cast<HbListView *>(
               mDocumentLoader.findWidget(LIST_VIEW_NAME));
}

/*!
 \return pointer to the view label resulting from last \a build call. It is 
 guaranteed to be not NULL if the \a build was called for the context
 related to view including label.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbGroupBox *HsMenuViewBuilder::currentViewLabel() const
{
    const QString LABEL_NAME = mViewContextToStringMap[mViewContext]
                               + "Label";

    return qobject_cast<HbGroupBox *>(
               mDocumentLoader.findWidget(LABEL_NAME));
}

/*!
 \return pointer to search panel.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbSearchPanel *HsMenuViewBuilder::currentSearchPanel() const
{
    return qobject_cast<HbSearchPanel *>(mDocumentLoader.findWidget(
            SEARCH_PANEL_NAME));
}

/*!
 \return pointer to a button
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbPushButton *HsMenuViewBuilder::collectionButton() const
{
    return qobject_cast<HbPushButton *>(mDocumentLoader.findWidget(
            BUTTON_NAME));
}

/*!
 \return pointer to All Applications Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::allAppsAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        ALL_APPS_ACTION_NAME));
}

/*!
 \return pointer to All Collections Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::allCollectionsAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        ALL_COLLECTIONS_ACTION_NAME));
}

/*!
 \return pointer to Search Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::searchAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        SEARCH_ACTION_NAME));
}

/*!
 \return pointer to Ovi Store Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::oviStoreAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        OVI_STORE_ACTION_NAME));
}

/*!
 \return pointer to Operator Action. Guaranteed to be not NULL.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::operatorAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        OPERATOR_ACTION_NAME));
}

/*!
 \return pointer to the main view toolbar.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbToolBar *HsMenuViewBuilder::toolBar() const
{
    return mToolBar;
}

/*!
 \return pointer to the toolbar extension.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbToolBarExtension *HsMenuViewBuilder::toolBarExtension() const
{
    return mToolBarExtension;
}

/*!
 \return action group for \a allAppsState and \a allCollectionsState action.
 */
QActionGroup *HsMenuViewBuilder::toolBarActionGroup() const
{
    return mToolBarActionGroup;
}

/*!
 Reads docml configuration corresponding to current context.
 */
bool HsMenuViewBuilder::build()
{
    return readContextConfiguration(mViewContext, mOperationalContext);
}



/*!
 Constructor.
 Makes object be initialized with with hidden but existing
 search panel and view label.
 */
HsMenuViewBuilder::HsMenuViewBuilder():
    DOCUMENT_FILE_NAME(":/xml/applibrary.docml"),
    ALL_APPS_ACTION_NAME("allAppsAction"),
    ALL_COLLECTIONS_ACTION_NAME("allCollectionsAction"),
    SEARCH_ACTION_NAME("searchAction"),
    OVI_STORE_ACTION_NAME("oviStoreAction"),
    OPERATOR_ACTION_NAME("operatorAction"),
    SEARCH_PANEL_NAME("searchPanel"),
    BUTTON_NAME("collectionButton"),
    TOOL_BAR_NAME("toolBar"),
    mToolBar(new HbToolBar),
    mToolBarExtension(new HbToolBarExtension),
    mViewContext(HsAllAppsContext),
    mOperationalContext(HsItemViewContext)
{
    init();

    // parse common section and the one specified by view options
    const bool result = parseSection();

    Q_ASSERT_X(result,
               "HsMenuViewBuilder::HsMenuViewBuilder()",
               "construction failed");

    mToolBar->addAction(allAppsAction());
    mToolBar->addAction(allCollectionsAction());
    mToolBar->addAction(searchAction());

    mToolBarActionGroup = new QActionGroup(allAppsAction());

    allAppsAction()->setActionGroup(mToolBarActionGroup);
    allCollectionsAction()->setActionGroup(mToolBarActionGroup);
}

/*!
 Destructor.
 Deletes widgets owned by the Menu View Builder.
 */
HsMenuViewBuilder::~HsMenuViewBuilder()
{
    delete mToolBarExtension;
    delete mToolBar;
    qDeleteAll(mLoadedObjects);
}

/*!
 Parses requested docml file section and reflects its contents in the
 object state.
 \param sectionName Name of the section to parse.
 \retval \a true on success, \a false otherwise.
 */
bool HsMenuViewBuilder::parseSection(const QString &sectionName)
{
    HSMENUTEST_FUNC_ENTRY("HsMenuViewBuilder::parseSection");

    bool loadStatusOk = false;
    const QObjectList loadedObjects =
        mDocumentLoader.load(DOCUMENT_FILE_NAME,
                             sectionName,
                             &loadStatusOk); 
    mLoadedObjects |= loadedObjects.toSet();

    Q_ASSERT_X(loadStatusOk,
               DOCUMENT_FILE_NAME.toLatin1().data(),
               "Error while loading docml file.");

    HSMENUTEST_FUNC_EXIT("HsMenuViewBuilder::parseSection");

    return loadStatusOk;
}


/*!
 \return Line edit of the searchPanel on success, NULL otherwise.
 */
HbLineEdit *HsMenuViewBuilder::searchPanelLineEdit() const
{
    HSMENUTEST_FUNC_ENTRY("HsMenuViewBuilder::searchPanelLineEdit");

    HbLineEdit *result(0);

    foreach(QGraphicsItem *obj, currentSearchPanel()->childItems()) {

        QGraphicsWidget *const widget = static_cast<QGraphicsWidget *>(obj);

        if (widget != NULL) {

            HbLineEdit *const lineEdit = qobject_cast<HbLineEdit *>(widget);

            if (lineEdit != NULL) {
                result = lineEdit;
                break;
            }
        }
    }
    HSMENUTEST_FUNC_EXIT("HsMenuViewBuilder::searchPanelLineEdit");

    return result;
}

/*!
    Builds mapping between context and docml name buidling blocks.
 */
void HsMenuViewBuilder::init()
{
    mViewContextToStringMap[HsAllAppsContext] = "allApps";
    mViewContextToStringMap[HsAllCollectionsContext] = "allCollections";
    mViewContextToStringMap[HsInstalledAppsContext] = "installedApps";
    mViewContextToStringMap[HsCollectionContext] = "collection";
    mOperationalContextToStringMap[HsItemViewContext] = "";
    mOperationalContextToStringMap[HsSearchContext] = "Search";
    mOperationalContextToStringMap[HsButtonContext] = "Button";
    mOperationalContextToStringMap[HsEmptyLabelContext] = "EmptyLabel";

}

/*!
 Sets view context. Not reflected in widgets returned by the builder
 until \a build is not run.
 */
void HsMenuViewBuilder::setViewContext(HsViewContext viewContext)
{
    mViewContext = viewContext;
}

/*!
 Sets operational context. Not reflected in widgets returned by the builder
 until \a build is not run.
 */
void HsMenuViewBuilder::setOperationalContext(
    HsOperationalContext operationalContext)
{
    mOperationalContext = operationalContext;
}

/*!
 Reads configuration for requested context and ensures search panel and corresponding
 vkb host are managed properly.
 */
bool HsMenuViewBuilder::readContextConfiguration(HsViewContext viewContext,
        HsOperationalContext operationalContext)
{
    const QString sectionName = mViewContextToStringMap[viewContext]
        + mOperationalContextToStringMap[operationalContext] 
        + "ViewDefinition";

    const bool result = parseSection(sectionName);

    if (currentSearchPanel()) {
        searchPanelVisibilityChange(mOperationalContext == HsSearchContext);
    }

    return result;
}

