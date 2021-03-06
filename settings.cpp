/*
Copyright 2011-2013 Lamarque Souza <lamarque@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "settings.h"
#include "settings_p.h"
#include "macros.h"
#include "connection.h"
#include "manager_p.h"

#include <QDBusObjectPath>

#include <nm-setting-connection.h>

#include "nmdebug.h"

NM_GLOBAL_STATIC(NetworkManager::SettingsPrivate, globalSettings)

NetworkManager::SettingsPrivate::SettingsPrivate()
    : iface(NetworkManagerPrivate::DBUS_SERVICE, NetworkManagerPrivate::DBUS_SETTINGS_PATH, QDBusConnection::systemBus())
{
    init();
    // This class is a friend of NetworkManagerPrivate thus initted there too
    // because of the init chain we must follow,
    // But if this class is used first we need to make sure the
    // NetworkManagerPrivate also get created so we have its signals for
    // when the daemon dies, we just can not call it directly here or
    // we will have a constructor infinite loop
    QTimer::singleShot(0, this, SLOT(initNotifier()));
}

void NetworkManager::SettingsPrivate::init()
{
    connect(&iface, SIGNAL(PropertiesChanged(QVariantMap)), this, SLOT(propertiesChanged(QVariantMap)));
    connect(&iface, SIGNAL(NewConnection(QDBusObjectPath)), this, SLOT(onConnectionAdded(QDBusObjectPath)));

    QDBusPendingReply<QList<QDBusObjectPath> > reply = iface.ListConnections();
    reply.waitForFinished();
    nmDebug() << "New Connections list";
    if (reply.isValid()) {
        foreach (const QDBusObjectPath &connection, reply.value()) {
            if (!connections.contains(connection.path())) {
                connections.insert(connection.path(), Connection::Ptr());
                emit connectionAdded(connection.path());
                nmDebug() << " " << connection.path();
            }
        }
    }
    if (m_canModify != iface.canModify()) {
        m_canModify = iface.canModify();
        emit canModifyChanged(m_canModify);
    }
    if (m_hostname != iface.hostname()) {
        m_hostname = iface.hostname();
        emit hostnameChanged(m_hostname);
    }
}

NetworkManager::Connection::List NetworkManager::SettingsPrivate::listConnections()
{
    NetworkManager::Connection::List list;
    QMap<QString, Connection::Ptr>::const_iterator i = connections.constBegin();
    while (i != connections.constEnd()) {
        NetworkManager::Connection::Ptr connection = findRegisteredConnection(i.key());
        if (connection) {
            list << connection;
        }
        ++i;
    }
    return list;
}

NetworkManager::Connection::Ptr NetworkManager::SettingsPrivate::findConnectionByUuid(const QString &uuid)
{
    QMap<QString, Connection::Ptr>::const_iterator i = connections.constBegin();
    while (i != connections.constEnd()) {
        NetworkManager::Connection::Ptr connection = findRegisteredConnection(i.key());
        if (connection && connection->uuid() == uuid) {
            return connection;
        }
        ++i;
    }

    return NetworkManager::Connection::Ptr();
}

QString NetworkManager::SettingsPrivate::hostname() const
{
    return m_hostname;
}

bool NetworkManager::SettingsPrivate::canModify() const
{
    return m_canModify;
}

QString NetworkManager::SettingsPrivate::addConnection(const NMVariantMapMap &connection)
{
    QDBusPendingReply<QDBusObjectPath> reply = iface.AddConnection(connection);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    QVariantMap connectionSettings = connection.value(QLatin1String(NM_SETTING_CONNECTION_SETTING_NAME));
    const QString id = connectionSettings.value(QLatin1String(NM_SETTING_CONNECTION_UUID)).toString();
    watcher->setProperty("libNetworkManagerQt_id", id);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(onConnectionAddArrived(QDBusPendingCallWatcher*)));
    return id;
}

void NetworkManager::SettingsPrivate::onConnectionAddArrived(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    const QString id = watcher->property("libNetworkManagerQt_id").toString();
    QString message;
    bool success = true;
    if (!reply.isValid()) {
        message = reply.error().message();
        success = false;
    } else {
        message = reply.argumentAt<0>().path();
    }
    emit connectionAddComplete(id, success, reply.error().message());
    watcher->deleteLater();
}

void NetworkManager::SettingsPrivate::initNotifier()
{
    notifier();
}

void NetworkManager::SettingsPrivate::saveHostname(const QString &hostname)
{
    iface.SaveHostname(hostname);
}

void NetworkManager::SettingsPrivate::propertiesChanged(const QVariantMap &properties)
{
    QVariantMap::const_iterator it = properties.constBegin();
    while (it != properties.constEnd()) {
        const QString property = it.key();
        if (property == QLatin1String("CanModify")) {
            m_canModify = it->toBool();
            emit canModifyChanged(m_canModify);
        } else if (property == QLatin1String("Hostname")) {
            m_hostname = it->toString();
            emit hostnameChanged(m_hostname);
        } else {
            qWarning() << Q_FUNC_INFO << "Unhandled property" << property;
        }
        ++it;
    }
}

void NetworkManager::SettingsPrivate::onConnectionAdded(const QDBusObjectPath &path)
{
    const QString id = path.path();
    if (connections.contains(id))
        return;
    connections.insert(id, Connection::Ptr());
    emit connectionAdded(id);
}

NetworkManager::Connection::Ptr NetworkManager::SettingsPrivate::findRegisteredConnection(const QString &path)
{
    Connection::Ptr ret;
    if (!path.isEmpty()) {
        bool contains = connections.contains(path);
        if (contains && connections.value(path)) {
            ret = connections.value(path);
        } else {
            ret = Connection::Ptr(new Connection(path), &QObject::deleteLater);
            connections[path] = ret;
            connect(ret.data(), SIGNAL(removed(QString)), this, SLOT(onConnectionRemoved(QString)));
            if (!contains) {
                emit connectionAdded(path);
            }
        }
    }
    return ret;
}

void NetworkManager::SettingsPrivate::onConnectionRemoved(const QString &path)
{
    connections.remove(path);
    emit connectionRemoved(path);
}

void NetworkManager::SettingsPrivate::daemonUnregistered()
{
    connections.clear();
}

NetworkManager::Connection::List NetworkManager::listConnections()
{
    return globalSettings->listConnections();
}

NetworkManager::Connection::Ptr NetworkManager::findConnectionByUuid(const QString &uuid)
{
    return globalSettings->findConnectionByUuid(uuid);
}

NetworkManager::Connection::Ptr NetworkManager::findConnection(const QString &path)
{
    return globalSettings->findRegisteredConnection(path);
}

QString NetworkManager::addConnection(const NMVariantMapMap &connection)
{
    return globalSettings->addConnection(connection);
}

void NetworkManager::saveHostname(const QString &hostname)
{
    globalSettings->saveHostname(hostname);
}

bool NetworkManager::canModify()
{
    return globalSettings->canModify();
}

QString NetworkManager::hostname()
{
    return globalSettings->hostname();
}

NetworkManager::SettingsNotifier *NetworkManager::settingsNotifier()
{
    return globalSettings;
}

#include "settings.moc"
