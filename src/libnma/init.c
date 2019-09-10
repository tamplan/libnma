// SPDX-License-Identifier: GPL-2.0+
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/*
 * Copyright 2014 Red Hat, Inc.
 */

#include "nm-default.h"

#include <libintl.h>

static void __attribute__((constructor))
_libnma_init (void)
{
	static gboolean initialized = FALSE;

	if (initialized)
		return;
	initialized = TRUE;

	bindtextdomain (GETTEXT_PACKAGE, NMALOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
}

