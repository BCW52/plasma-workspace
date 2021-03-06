/*
    SPDX-FileCopyrightText: 2007-2010 John Tapsell <johnflux@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef Q_WS_WIN

#include "ksystemactivitydialog.h"

#include <QAction>
#include <QCloseEvent>
#include <QDBusConnection>
#include <QIcon>
#include <QLineEdit>
#include <QString>
#include <QVBoxLayout>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KWindowSystem>
#include <QDebug>

KSystemActivityDialog::KSystemActivityDialog(QWidget *parent)
    : QDialog(parent)
    , m_processList(nullptr)
{
    setWindowTitle(i18n("System Activity"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("utilities-system-monitor")));
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(&m_processList);
    m_processList.setScriptingEnabled(true);
    setSizeGripEnabled(true);

    // Since we kinda act like an application more than just a Window, map the usual ctrl+Q shortcut to close as well
    QAction *closeWindow = new QAction(this);
    closeWindow->setShortcut(QKeySequence::Quit);
    connect(closeWindow, &QAction::triggered, this, &KSystemActivityDialog::accept);
    addAction(closeWindow);

    KConfigGroup cg = KSharedConfig::openConfig()->group("TaskDialog");
    m_processList.loadSettings(cg);

    QDBusConnection con = QDBusConnection::sessionBus();
    con.registerObject(QStringLiteral("/"), this, QDBusConnection::ExportAllSlots);
}

void KSystemActivityDialog::run()
{
    show();
    raise();
    KWindowSystem::setOnDesktop(winId(), KWindowSystem::currentDesktop());
    KWindowSystem::forceActiveWindow(winId());
}

void KSystemActivityDialog::setFilterText(const QString &filterText)
{
    m_processList.filterLineEdit()->setText(filterText);
    m_processList.filterLineEdit()->setFocus();
}

QString KSystemActivityDialog::filterText() const
{
    return m_processList.filterLineEdit()->text();
}

void KSystemActivityDialog::closeEvent(QCloseEvent *event)
{
    saveDialogSettings();
    event->accept();
}

void KSystemActivityDialog::reject()
{
    saveDialogSettings();
    QDialog::reject();
}

void KSystemActivityDialog::saveDialogSettings()
{
    // When the user closes the dialog, save the process list setup
    KConfigGroup cg = KSharedConfig::openConfig()->group("TaskDialog");
    m_processList.saveSettings(cg);
    KSharedConfig::openConfig()->sync();
}

QSize KSystemActivityDialog::sizeHint() const
{
    return QSize(650, 420);
}

#endif // not Q_WS_WIN
