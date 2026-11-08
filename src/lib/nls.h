/*
This file is part of fdbox
For license - read license.txt

Translations: _() marks a message, gettext gives it in the language of
the environment (LANGUAGE, LANG; on GonWare the system language). Built
with FDBOX_NLS and a gettext library (GonWare: its libintl with the
catalogues built into the program), else the English of the source.
xgettext -k_ -kN_ collects the messages into po/fdbox.pot.
*/

#ifndef FDBOX_NLS_H
#define FDBOX_NLS_H

#ifdef FDBOX_NLS
#include <libintl.h>
#define _(s) gettext(s)
#else
#define _(s) (s)
#endif
#define N_(s) (s)

#endif
