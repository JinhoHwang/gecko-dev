/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Mozilla Communicator client code, 
 * released March 31, 1998. 
 *
 * The Initial Developer of the Original Code is Netscape Communications 
 * Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 *     Samir Gehani <sgehani@netscape.com>
 */

#ifndef _NS_XINSTALLER_H_
#define _NS_XINSTALLER_H_

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "XIDefines.h"
#include "XIErrors.h"

#include "nsINIParser.h"
#include "nsLicenseDlg.h"
#include "nsXIContext.h"

extern nsXIContext *gCtx; 

class nsXInstaller
{
public:
    nsXInstaller();
    ~nsXInstaller();

    int ParseConfig();
    int RunWizard(int argc, char **argv);
    int Download();
    int Extract();
    int Install();

    static gint Kill(GtkWidget *widget, GtkWidget *event, gpointer data);

private:
    int InitContext();
    int DrawLogo();
    int DrawNavButtons();
};

int     main(int argc, char **argv);


#define CONFIG_INI "config.ini"

#if defined(DUMP)
#undef DUMP
#endif
#if defined(DEBUG_sgehani) || defined(DEBUG_druidd) || defined(DEBUG_root)
#define DUMP(_msg) printf("%s %d: %s \n", __FILE__, __LINE__, _msg);
#else
#define DUMP(_msg)
#endif


#endif /*_NS_XINSTALLER_H_ */
