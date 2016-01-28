/*
 * dbus_actions.h
 *
 * Copyright (C) 2015 Kano Computing Ltd.
 * License: http://www.gnu.org/licenses/gpl-2.0.txt GNU General Public License v2
 *
 *
 * Based on an autogenerated XML
 * To generate the XML
 *
 * qdbuscpp2xml dbus.h -o dbus.xml
 *
 * To generate this file again from that
 *
 * qdbusxml2cpp dbus.xml -a dbus.h
 *
 */

#ifndef DBUS_ACTIONS_H
#define DBUS_ACTIONS_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

/*
 * Adaptor class for interface me.kano.dbusTest
 */
class DbusActionsAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "me.kano.sonicpi.Actions")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"me.kano.sonicpi.Actions\">\n"
"    <method name=\"save\"/>\n"
"    <method name=\"share\"/>\n"
"    <method name=\"load\"/>\n"
"    <method name=\"make\"/>\n"
"  </interface>\n"
        "")
public:
    DbusActionsAdaptor(QObject *parent);
    virtual ~DbusActionsAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
    void load();
    void make();
    void save();
    void share();
Q_SIGNALS: // SIGNALS
};

#endif