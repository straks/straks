// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef STRAKS_QT_MACDOCKICONHANDLER_H
#define STRAKS_QT_MACDOCKICONHANDLER_H

#include <QMainWindow>
#include <QObject>

QT_BEGIN_NAMESPACE
class QIcon;
class QMenu;
class QWidget;
QT_END_NAMESPACE

/** Macintosh-specific dock icon handler.
 */
class MacDockIconHandler : public QObject
{
    Q_OBJECT

public:
    ~MacDockIconHandler();

    QMenu *dockMenu();
    void setIcon(const QIcon &icon);
    void setMainWindow(QMainWindow *window);
    static MacDockIconHandler *instance();
    static void cleanup();
    void handleDockIconClickEvent();

Q_SIGNALS:
    void dockIconClicked();

private:
    MacDockIconHandler();

    QWidget *m_dummyWidget;
    QMenu *m_dockMenu;
    QMainWindow *mainWindow;
};

#endif // STRAKS_QT_MACDOCKICONHANDLER_H
