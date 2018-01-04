// Copyright (c) 2009-2017 The Bitcoin Core developers 
// Copyright (c) 2017 The Dash developers 
// Copyright (c) 2017 STRAKS developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef STRAKS_QT_TEST_COMPATTESTS_H
#define STRAKS_QT_TEST_COMPATTESTS_H

#include <QObject>
#include <QTest>

class CompatTests : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void bswapTests();
};

#endif // STRAKS_QT_TEST_COMPATTESTS_H
