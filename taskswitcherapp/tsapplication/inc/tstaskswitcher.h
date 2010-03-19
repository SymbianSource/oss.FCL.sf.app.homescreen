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
* Description:
*
*/

#ifndef TSTASKSWITCHER_H
#define TSTASKSWITCHER_H

#include <QObject>

class T_TsTaskSwitcher;

class QStateMachine;

/**
 * @ingroup group_tsapplication
 * @brief TaskSwitcher application main class.
 *
 * Loads a runtime from a runtime provider plugin. Manages
 * the runtime execution.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
 
class TsTaskSwitcher : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @since S60 ?S60_version.
     * @param aParent Parent object.
     */
    TsTaskSwitcher(QObject* parent = 0);

private:

    Q_DISABLE_COPY(TsTaskSwitcher)

signals:
    /**
     * Emitted when the teleport application needs to exit.
     * @since S60 ?S60_version.
     */
    void exit();

public slots:
    /**
     * Starts the runtime.
     * @since S60 ?S60_version.
     */
    void start();

    /**
     * Stops the runtime.
     * @since S60 ?S60_version.
     */
    void stop();

private:
    /**
     * Runtime.
     */
    QStateMachine* mRuntime;

    friend class T_TsTaskSwitcher;
};

#endif // TSTASKSWITCHER_H
