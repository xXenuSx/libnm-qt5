/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -N -m -i generic-types.h -p nm-settings-connectioninterface /home/jgrulich/projects/libnm-qt/dbus/introspection/nm-settings-connection.xml
 *
 * qdbusxml2cpp is Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef NMQT_SETTINGS_CONNECTIONINTERFACE_H
#define NMQT_SETTINGS_CONNECTIONINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "generic-types.h"

/*
 * Proxy class for interface org.freedesktop.NetworkManager.Settings.Connection
 */
class OrgFreedesktopNetworkManagerSettingsConnectionInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.freedesktop.NetworkManager.Settings.Connection"; }

public:
    OrgFreedesktopNetworkManagerSettingsConnectionInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgFreedesktopNetworkManagerSettingsConnectionInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> Delete()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("Delete"), argumentList);
    }

    inline QDBusPendingReply<QVariantMapMap> GetSecrets(const QString &setting_name)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(setting_name);
        return asyncCallWithArgumentList(QLatin1String("GetSecrets"), argumentList);
    }

    inline QDBusPendingReply<QVariantMapMap> GetSettings()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QLatin1String("GetSettings"), argumentList);
    }

    inline QDBusPendingReply<> Update(const QVariantMapMap &properties)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(properties);
        return asyncCallWithArgumentList(QLatin1String("Update"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void Removed();
    void Updated();
};

#endif
