// Copyright (c) 2011-2014 The Straks Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef STRAKS_QT_STRAKSADDRESSVALIDATOR_H
#define STRAKS_QT_STRAKSADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class StraksAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit StraksAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Straks address widget validator, checks for a valid straks address.
 */
class StraksAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit StraksAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // STRAKS_QT_STRAKSADDRESSVALIDATOR_H
