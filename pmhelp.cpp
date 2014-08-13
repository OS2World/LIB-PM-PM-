/* 


	pmhelp.cpp (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pmwin.h"
#include "pmhelp.h"
#include "pmstdres.h"

/////////////////////////////////////////////////////////////////////////////

PMHelpWin::PMHelpWin(PSZ title,PSZ library,USHORT id,HAB hab) : PMWin(hab)
{
	hiInit.cb = sizeof (HELPINIT);
	hiInit.ulReturnCode = 0;
	hiInit.pszTutorialName = NULL;
	hiInit.phtHelpTable = ( PHELPTABLE ) MAKEULONG (id, 0xffff);
	hiInit.hmodHelpTableModule = NULLHANDLE;
	hiInit.hmodAccelActionBarModule = NULLHANDLE;
	hiInit.idAccelTable = 0;
	hiInit.idActionBar = 0;
	hiInit.pszHelpWindowTitle = title;
	hiInit.fShowPanelId = CMIC_HIDE_PANEL_ID;
	hiInit.pszHelpLibraryName = library;
}

PMHelpWin::PMHelpWin(PSZ title,PSZ library,PHELPTABLE pht,HAB hab) : PMWin(hab)
{
	hiInit.cb = sizeof (HELPINIT);
	hiInit.ulReturnCode = 0;
	hiInit.pszTutorialName = NULL;
	hiInit.phtHelpTable = pht;
	hiInit.hmodHelpTableModule = NULLHANDLE;
	hiInit.hmodAccelActionBarModule = NULLHANDLE;
	hiInit.idAccelTable = 0;
	hiInit.idActionBar = 0;
	hiInit.pszHelpWindowTitle = title;
	hiInit.fShowPanelId = CMIC_HIDE_PANEL_ID;
	hiInit.pszHelpLibraryName = library;
}

BOOL PMHelpWin::createWin()
{
	hwnd= WinCreateHelpInstance( ab, &hiInit );
	if (!hwnd) { // if we could not create the help instance display a message
		char buf[460];
		sprintf(buf,"Could not start help system. (Probably the help file \"%s\" is not in the current directory.)\nTo continue, press OK, but help will not be available.\nTo quit the application press Cancel.\n",hiInit.pszHelpLibraryName);
		if (WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,buf,"Error!",0,MB_OKCANCEL|MB_ICONHAND|MB_APPLMODAL)!=MBID_OK) exit(-1);
		return FALSE;
	}
	return TRUE;
}

BOOL PMHelpWin::destroyWin()
{
	return WinDestroyHelpInstance(hwnd);
}

PMHelpWin::~PMHelpWin()
{
	destroyWin();
}



