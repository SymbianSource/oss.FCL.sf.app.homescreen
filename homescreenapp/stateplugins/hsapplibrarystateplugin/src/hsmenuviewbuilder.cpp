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
#include <QGraphicsWidget>
#include <HbAction>
#include <HbGroupBox>
#include <HbListView>
#include <HbLineEdit>
#include <HbSearchPanel>
#include <HbToolBar>
#include <HbView>
#include <HbWidget>
#include <HbStaticVkbHost>
#include <HbMainWindow>
#include <HbInputMethod>

#include "hsmenuviewbuilder.h"
#include "hsmenustates_global.h"

// TODO this class is temprary solution, proper one should come from Orbit
class HsVkbHost : public HbStaticVkbHost {
public:
	explicit HsVkbHost(HbWidget *target):
		HbStaticVkbHost(target), mWidget(target) {}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT skipped: its very temporary TODO
	void openFinished() {		
		updateViewHeight(applicationArea().height());
	}

	void closeFinished(){				
		updateViewHeight(-1);
	}

	void updateViewHeight(qreal height) {		
		HbView* view = mWidget->mainWindow()->currentView();
		view->setMaximumHeight(height);
	}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
	
	HbWidget * mWidget;
};

/*!
    \class HsMenuViewBuilder
    \ingroup group_hsmenustateplugin

    \brief Menu View Builder

    Reads UI object from Application Library docml

    \sa HsMenuView
*/

/*!
  Sets view label visible or hidden.
  \param visible When true label is set visible, invisible otherwise.
  \return \a true on success, \a false otherwise.
 */
bool HsMenuViewBuilder::setLabelVisible(bool visible)
{
    bool result(true);

    if (visible != mViewOptions.testFlag(Label)) {
        mViewOptions ^= ViewOptions(Label);
        result = parseSection(mSections[mViewOptions]);
    }
    return result;
}


/*!
  Sets search panel visible or hidden.
  \param visible When true search panel is set visible,
  invisible otherwise.
  \return \a true on success, \a false otherwise.
 */
bool HsMenuViewBuilder::setSearchPanelVisible(bool visible)
{
    bool result(true);

    if (visible != mViewOptions.testFlag(Search)) {
        mViewOptions ^= ViewOptions(Search);
        result = parseSection(mSections[mViewOptions]);

        if (visible) {
			// TODO this is temprary solution, proper solution should come from Orbit
                        if (!HbVkbHost::getVkbHost(searchPanel()))
                                {
                                new HsVkbHost(searchPanel());
                                }
            HbLineEdit *const lineEdit(searchPanelLineEdit());

            lineEdit->setText("");
        }
        else {
           if (static_cast<HsVkbHost *>(HbVkbHost::getVkbHost(searchPanel()))) {
             static_cast<HsVkbHost *>(HbVkbHost::getVkbHost(searchPanel()))->
                     updateViewHeight(-1);
            }
       }
    }
    return result;
}

/*!
 \return \a true when label is visible, \a false otherwise.
 */
bool HsMenuViewBuilder::isLabelVisible() const
{
    return mViewOptions.testFlag(Label);
}

/*!
 \return \a true when search panel is visible, \a false otherwise.
 */
bool HsMenuViewBuilder::isSearchPanelVisible() const
{
    return mViewOptions.testFlag(Search);
}

/*!
 \return pointer to the main view.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbView *HsMenuViewBuilder::view() const
{
    return qobject_cast<HbView *>(mDocumentLoader.findWidget(VIEW_NAME));
}

/*!
 \return pointer to the list item view.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbListView *HsMenuViewBuilder::listView() const
{
    return qobject_cast<HbListView *>(
               mDocumentLoader.findWidget(LIST_VIEW_NAME));
}

/*!
 \return pointer to the label.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbGroupBox *HsMenuViewBuilder::label() const
{
    return qobject_cast<HbGroupBox *>(
               mDocumentLoader.findWidget(LABEL_NAME));
}

/*!
 \return pointer to the searchPanel.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbSearchPanel *HsMenuViewBuilder::searchPanel() const
{
    return qobject_cast<HbSearchPanel *>(mDocumentLoader.findWidget(
            SEARCH_PANEL_NAME));
}

/*!
 \return pointer to All Applications Action.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::allAppsAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        ALL_APPS_ACTION_NAME));
}

/*!
 \return pointer to All Collections Action.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::allCollectionsAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        ALL_COLLECTIONS_ACTION_NAME));
}

/*!
 \return pointer to Search Action.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::searchAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        SEARCH_ACTION_NAME));
}

/*!
 \return pointer to Ovi Store Action.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbAction *HsMenuViewBuilder::oviStoreAction() const
{
    return qobject_cast<HbAction *>(mDocumentLoader.findObject(
                                        OVI_STORE_ACTION_NAME));
}

/*!
 \return pointer to the main view toolbar.
 The pointer is valid until the HsMenuViewBuilder instance is destroyed.
 Memory ownership is not changed.
 */
HbToolBar *HsMenuViewBuilder::toolBar() const
{
    return qobject_cast<HbToolBar *>(
               mDocumentLoader.findWidget(TOOL_BAR_NAME));
}

/*!
 Constructor.
 Makes object be initialized with with hidden but existing
 search panel and view label.
 */
HsMenuViewBuilder::HsMenuViewBuilder():
    mViewOptions(0),
    DOCUMENT_FILE_NAME(":/xml/applibrary.docml"),
    ALL_APPS_ACTION_NAME("allAppsAction"),
    ALL_COLLECTIONS_ACTION_NAME("allCollectionsAction"),
    SEARCH_ACTION_NAME("searchAction"),
    OVI_STORE_ACTION_NAME("oviStoreAction"),
    VIEW_NAME("view"),
    CONTAINER_NAME("container"),
    LIST_VIEW_NAME("listView"),
    TOOL_BAR_NAME("toolBar"),
    SEARCH_PANEL_NAME("searchPanel"),
    LABEL_NAME("label"),
    LIST_VIEW_SECTION_NAME("list_view"),
    LIST_LABELED_VIEW_SECTION_NAME("list_labeled_view"),
    LIST_SEARCH_VIEW_SECTION_NAME("list_search_view"),
    LIST_SEARCH_LABELED_VIEW_SECTION_NAME("list_search_labeled_view")
{
    buildSectionKeyMap();

    // parse common section and the one specified by view options
    bool result = parseSection("") && parseSection(mSections[mViewOptions]);

    Q_ASSERT_X(result,
               "HsMenuViewBuilder::HsMenuViewBuilder()",
               "construction failed");
}

/*!
 Destructor.
 Deletes list view, search panel, view label, All Applications
 Action, All Collections Action, Search Action, Ovi Store Action, container
 widget, main view toolbar and the main view.

 The destructor needs to be checked agains any change in related docml file.
 */
HsMenuViewBuilder::~HsMenuViewBuilder()
{
    QObjectList objectList;

    objectList << listView();

    objectList << searchPanel();

    objectList << label();

    objectList << allAppsAction();
    objectList << allCollectionsAction();
    objectList << searchAction();
    objectList << mDocumentLoader.findWidget(CONTAINER_NAME);
    objectList << toolBar();
    objectList << view();

    foreach(QObject *obj, objectList) {
        obj->deleteLater();
    }
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

    QObjectList loadedObjects = mDocumentLoader.load(DOCUMENT_FILE_NAME,
                                sectionName,
                                &loadStatusOk);

    Q_ASSERT_X(loadStatusOk,
               DOCUMENT_FILE_NAME.toLatin1().data(),
               "Error while loading docml file.");
    HSMENUTEST_FUNC_EXIT("HsMenuViewBuilder::parseSection");

    return loadStatusOk;
}

/*!
 \retval Line edit of the searchPanel on success, 0 otherwise.
 */
HbLineEdit *HsMenuViewBuilder::searchPanelLineEdit() const
{
    HSMENUTEST_FUNC_ENTRY("HsMenuViewBuilder::searchPanelLineEdit");

    HbLineEdit *result(0);

    foreach(QGraphicsItem *obj, searchPanel()->childItems()) {

        QGraphicsWidget *const widget =
            static_cast<QGraphicsWidget *>(obj);

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
    Sets up mapping between ViewOptions and section names.
 */
void HsMenuViewBuilder::buildSectionKeyMap()
{
    mSections[ViewOptions()]  =
        LIST_VIEW_SECTION_NAME;

    mSections[ViewOptions(Search)]   =
        LIST_SEARCH_VIEW_SECTION_NAME;

    mSections[ViewOptions(Label)]  =
        LIST_LABELED_VIEW_SECTION_NAME;

    mSections[ViewOptions(Label | Search)]   =
        LIST_SEARCH_LABELED_VIEW_SECTION_NAME;
}
