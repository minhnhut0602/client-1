/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QByteArray>

class QWidget;

namespace Mirall {

namespace Utility
{
    QString formatFingerprint( const QByteArray& );
    void setupFavLink( const QString &folder );
    QString octetsToString( qint64 octets );
    QString platform();
    QByteArray userAgentString();
    void raiseDialog(QWidget *);
}

}
#endif // UTILITY_H
