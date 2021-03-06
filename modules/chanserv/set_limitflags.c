/*
 * SPDX-License-Identifier: ISC
 * SPDX-URL: https://spdx.org/licenses/ISC.html
 *
 * Copyright (C) 2003-2004 E. Will, et al.
 * Copyright (C) 2006-2009 Atheme Project (http://atheme.org/)
 *
 * This file contains routines to handle the CService SET LIMITFLAGS command.
 */

#include <atheme.h>

static mowgli_patricia_t **cs_set_cmdtree = NULL;

static void
cs_cmd_set_limitflags(struct sourceinfo *si, int parc, char *parv[])
{
	struct mychan *mc;

	if (!(mc = mychan_find(parv[0])))
	{
		command_fail(si, fault_nosuch_target, STR_IS_NOT_REGISTERED, parv[0]);
		return;
	}

	if (!parv[1])
	{
		command_fail(si, fault_needmoreparams, STR_INSUFFICIENT_PARAMS, "SET LIMITFLAGS");
		return;
	}

	if ((chanacs_source_flags(mc, si) & CA_HIGHPRIVS) != CA_HIGHPRIVS)
	{
		command_fail(si, fault_noprivs, STR_NOT_AUTHORIZED);
		return;
	}

	if (!strcasecmp("ON", parv[1]))
	{
		if (MC_LIMITFLAGS & mc->flags)
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is already set for \2%s\2."), "LIMITFLAGS", mc->name);
			return;
		}

		logcommand(si, CMDLOG_SET, "SET:LIMITFLAGS:ON: \2%s\2", mc->name);
		verbose(mc, "\2%s\2 enabled the LIMITFLAGS flag", get_source_name(si));

		mc->flags |= MC_LIMITFLAGS;

		command_success_nodata(si, _("The \2%s\2 flag has been set for \2%s\2."), "LIMITFLAGS", mc->name);

		return;
	}

	else if (!strcasecmp("OFF", parv[1]))
	{
		if (!(MC_LIMITFLAGS & mc->flags))
		{
			command_fail(si, fault_nochange, _("The \2%s\2 flag is not set for \2%s\2."), "LIMITFLAGS", mc->name);
			return;
		}

		logcommand(si, CMDLOG_SET, "SET:LIMITFLAGS:OFF: \2%s\2", mc->name);
		verbose(mc, "\2%s\2 disabled the LIMITFLAGS flag", get_source_name(si));

		mc->flags &= ~MC_LIMITFLAGS;

		command_success_nodata(si, _("The \2%s\2 flag has been removed for \2%s\2."), "LIMITFLAGS", mc->name);

		return;
	}

	else
	{
		command_fail(si, fault_badparams, STR_INVALID_PARAMS, "SET LIMITFLAGS");
		return;
	}
}

static struct command cs_set_limitflags = {
	.name           = "LIMITFLAGS",
	.desc           = N_("Limits the power of the +f flag."),
	.access         = AC_NONE,
	.maxparc        = 2,
	.cmd            = &cs_cmd_set_limitflags,
	.help           = { .path = "cservice/set_limitflags" },
};

static void
mod_init(struct module *const restrict m)
{
	MODULE_TRY_REQUEST_SYMBOL(m, cs_set_cmdtree, "chanserv/set_core", "cs_set_cmdtree")

	command_add(&cs_set_limitflags, *cs_set_cmdtree);

	use_limitflags++;
}

static void
mod_deinit(const enum module_unload_intent ATHEME_VATTR_UNUSED intent)
{
	command_delete(&cs_set_limitflags, *cs_set_cmdtree);

	use_limitflags--;
}

SIMPLE_DECLARE_MODULE_V1("chanserv/set_limitflags", MODULE_UNLOAD_CAPABILITY_OK)
