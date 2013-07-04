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

#include "utility.h"

#include "mirall/version.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QUrl>
#include <QWidget>

#include <QDebug>

#ifdef Q_OS_MAC
#include <CoreServices/CoreServices.h>
#endif
#ifdef Q_OS_WIN
#include <shlobj.h>
#endif

namespace Mirall {

QString Utility::formatFingerprint( const QByteArray& fmhash )
{
    QByteArray hash;
    int steps = fmhash.length()/2;
    for (int i = 0; i < steps; i++) {
        hash.append(fmhash[i*2]);
        hash.append(fmhash[i*2+1]);
        hash.append(' ');
    }

    QString fp = QString::fromAscii( hash.trimmed() );
    fp.replace(QChar(' '), QChar(':'));

    return fp;
}

void Utility::setupFavLink(const QString &folder)
{
#ifdef Q_OS_WIN
    // Windows Explorer: Place under "Favorites" (Links)
    wchar_t path[MAX_PATH];
    SHGetSpecialFolderPath(0, path, CSIDL_PROFILE, FALSE);
    QString profile =  QDir::fromNativeSeparators(QString::fromWCharArray(path));
    QDir folderDir(QDir::fromNativeSeparators(folder));
    QString linkName = profile+QLatin1String("/Links/") + folderDir.dirName() + QLatin1String(".lnk");
    if (!QFile::link(folder, linkName))
        qDebug() << Q_FUNC_INFO << "linking" << folder << "to" << linkName << "failed!";
#elif defined (Q_OS_MAC)
    // Finder: Place under "Places"/"Favorites" on the left sidebar
    CFStringRef folderCFStr = CFStringCreateWithCString(0, folder.toUtf8().data(), kCFStringEncodingUTF8);
    CFURLRef urlRef = CFURLCreateWithFileSystemPath (0, folderCFStr, kCFURLPOSIXPathStyle, true);

    LSSharedFileListRef placesItems = LSSharedFileListCreate(0, kLSSharedFileListFavoriteItems, 0);
    if (placesItems) {
        //Insert an item to the list.
        LSSharedFileListItemRef item = LSSharedFileListInsertItemURL(placesItems,
                                                                     kLSSharedFileListItemLast, 0, 0,
                                                                     urlRef, 0, 0);
        if (item)
            CFRelease(item);
    }
    CFRelease(placesItems);
    CFRelease(folderCFStr);
    CFRelease(urlRef);
#elif defined (Q_OS_UNIX)
    // Nautilus: add to ~/.gtk-bookmarks
    QFile gtkBookmarks(QDir::homePath()+QLatin1String("/.gtk-bookmarks"));
    QByteArray folderUrl = "file://" + folder.toUtf8();
    if (gtkBookmarks.open(QFile::ReadWrite)) {
        QByteArray places = gtkBookmarks.readAll();
        if (!places.contains(folderUrl)) {
            places += folderUrl;
            gtkBookmarks.reset();
            gtkBookmarks.write(places + '\n');
        }


    }

#endif
}

QString Utility::octetsToString( qint64 octets )
{
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;

    if (octets >= gb) {
        return QString::number(octets/gb) + QLatin1String(" GB");
    } else if (octets >= mb) {
        return QString::number(octets/mb) + QLatin1String(" MB");
    } else if (octets >= kb) {
        return QString::number(octets/kb) + QLatin1String(" KB");
    } else {
        return octets + QLatin1String(" bytes");
    }
}

// Qtified version of get_platforms() in csync_owncloud.c
QString Utility::platform()
{
#if defined(Q_OS_WIN32)
    return QLatin1String("Windows");
#elif defined(Q_OS_MAC)
    return QLatin1String("Macintosh");
#elif defined(Q_OS_LINUX)
    return QLatin1String("Linux");
#elif defined(__DragonFly__) // Q_OS_FREEBSD also defined
    return "DragonFlyBSD";
#elif defined(Q_OS_FREEBSD)
    return QLatin1String("FreeBSD");
#elif defined(Q_OS_NETBSD)
    return QLatin1String("NetBSD");
#elif defined(Q_OS_OPENBSD)
    return QLatin1String("OpenBSD");
#elif defined(Q_OS_SOLARIS)
    return "Solaris";
#else
    return "Unknown OS"
#endif
}

QByteArray Utility::userAgentString()
{
    return QString::fromLatin1("Mozilla/5.0 (%1) mirall/%2")
            .arg(Utility::platform())
            .arg(QLatin1String(MIRALL_STRINGIFY(MIRALL_VERSION)))
            .toLatin1();
}

void Utility::raiseDialog( QWidget *raiseWidget )
{
    // viel hilft viel ;-)
    if( raiseWidget ) {
#if defined(Q_WS_WIN) || defined (Q_OS_MAC)
        Qt::WindowFlags eFlags = raiseWidget->windowFlags();
        eFlags |= Qt::WindowStaysOnTopHint;
        raiseWidget->setWindowFlags(eFlags);
        raiseWidget->show();
        eFlags &= ~Qt::WindowStaysOnTopHint;
        raiseWidget->setWindowFlags(eFlags);
#endif
        raiseWidget->show();
        raiseWidget->raise();
        raiseWidget->activateWindow();
    }
}

} // namespace Mirall
