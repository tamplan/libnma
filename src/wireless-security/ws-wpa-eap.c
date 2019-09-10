// SPDX-License-Identifier: GPL-2.0+
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* NetworkManager Applet -- allow user control over networking
 *
 * Dan Williams <dcbw@redhat.com>
 *
 * Copyright 2007 - 2014 Red Hat, Inc.
 */

#include "nm-default.h"

#include <ctype.h>
#include <string.h>

#include "wireless-security.h"
#include "eap-method.h"

struct _WirelessSecurityWPAEAP {
	WirelessSecurity parent;

	GtkSizeGroup *size_group;
};


static void
destroy (WirelessSecurity *parent)
{
	WirelessSecurityWPAEAP *sec = (WirelessSecurityWPAEAP *) parent;

	if (sec->size_group)
		g_object_unref (sec->size_group);
}

static gboolean
validate (WirelessSecurity *parent, GError **error)
{
	return ws_802_1x_validate (parent, "wpa_eap_auth_combo", error);
}

static void
add_to_size_group (WirelessSecurity *parent, GtkSizeGroup *group)
{
	WirelessSecurityWPAEAP *sec = (WirelessSecurityWPAEAP *) parent;

	if (sec->size_group)
		g_object_unref (sec->size_group);
	sec->size_group = g_object_ref (group);

	ws_802_1x_add_to_size_group (parent,
	                             sec->size_group,
	                             "wpa_eap_auth_label",
	                             "wpa_eap_auth_combo");
}

static void
fill_connection (WirelessSecurity *parent, NMConnection *connection)
{
	NMSettingWirelessSecurity *s_wireless_sec;

	ws_802_1x_fill_connection (parent, "wpa_eap_auth_combo", connection);

	s_wireless_sec = nm_connection_get_setting_wireless_security (connection);
	g_assert (s_wireless_sec);

	g_object_set (s_wireless_sec, NM_SETTING_WIRELESS_SECURITY_KEY_MGMT, "wpa-eap", NULL);
}

static void
auth_combo_changed_cb (GtkWidget *combo, gpointer user_data)
{
	WirelessSecurity *parent = WIRELESS_SECURITY (user_data);
	WirelessSecurityWPAEAP *sec = (WirelessSecurityWPAEAP *) parent;

	ws_802_1x_auth_combo_changed (combo,
	                              parent,
	                              "wpa_eap_method_vbox",
	                              sec->size_group);
}

static void
update_secrets (WirelessSecurity *parent, NMConnection *connection)
{
	ws_802_1x_update_secrets (parent, "wpa_eap_auth_combo", connection);
}

WirelessSecurityWPAEAP *
ws_wpa_eap_new (NMConnection *connection,
                gboolean is_editor,
                gboolean secrets_only,
                const char *const*secrets_hints)
{
	WirelessSecurity *parent;
	GtkWidget *widget;

	parent = wireless_security_init (sizeof (WirelessSecurityWPAEAP),
	                                 validate,
	                                 add_to_size_group,
	                                 fill_connection,
	                                 update_secrets,
	                                 destroy,
	                                 "/org/freedesktop/network-manager-applet/ws-wpa-eap.ui",
	                                 "wpa_eap_notebook",
	                                 NULL);
	if (!parent)
		return NULL;

	parent->adhoc_compatible = FALSE;
	parent->hotspot_compatible = FALSE;

	widget = ws_802_1x_auth_combo_init (parent,
	                                    "wpa_eap_auth_combo",
	                                    "wpa_eap_auth_label",
	                                    (GCallback) auth_combo_changed_cb,
	                                    connection,
	                                    is_editor,
	                                    secrets_only,
	                                    secrets_hints);
	auth_combo_changed_cb (widget, parent);

	return (WirelessSecurityWPAEAP *) parent;
}

