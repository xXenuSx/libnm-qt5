/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -N -m -i generic-types.h -p nm-dhcp4-configinterface /home/jgrulich/projects/libnm-qt/dbus/introspection/nm-dhcp4-config.xml
 *
 * qdbusxml2cpp is Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "nm-dhcp4-configinterface.h"

/*
 * Implementation of interface class OrgFreedesktopNetworkManagerDHCP4ConfigInterface
 */

OrgFreedesktopNetworkManagerDHCP4ConfigInterface::OrgFreedesktopNetworkManagerDHCP4ConfigInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

OrgFreedesktopNetworkManagerDHCP4ConfigInterface::~OrgFreedesktopNetworkManagerDHCP4ConfigInterface()
{
}


#include "nm-dhcp4-configinterface.moc"
