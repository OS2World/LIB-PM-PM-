/* 


	pmwin.h (emx+gcc) 

	1994,95 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#ifndef _PMWIN_H_
#define _PMWIN_H_

#define INCL_WIN
#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define OS2EMX_PLAIN_CHAR
#include <os2.h>

#include <stdlib.h>
#include <stddef.h>

#include "pmsys.h"
#include "pmgpi.h"

#undef assert
#ifdef NODEBUG
#define assert(ignore) ((void)0)
#else
#define assert(x) ((void)((x) || (DoAssert((PCSZ)#x,(PCSZ)__FILE__,__LINE__),1)))
#endif


void DoAssert(PCSZ msg,PCSZ file,unsigned line);

void ErrBox(PCSZ msg,...);

struct PMEvent;
class PMRect;
class PMPoint;
class PMPresSpace;
class PMWin;
class PMMainWin;
class PMHelpWin;
class PMApp;
class PMThread;
class PMAnchorBlock;   // class that creates basic window message handling functions
class PMWindowThread;
class PMDialog;
class PMControl;
class PMListBox;
class PMEntryField;
class PMMenu;

struct WINCREATEARGS {
	HWND hwndParent;
	HWND hwndOwner;
	ULONG flStyle;
	ULONG flCreateFlags;
	PCSZ pszClientClass;
	PCSZ pszTitle;
	ULONG styleClient;
	HMODULE hmod; 
	ULONG idResources;
	PHWND phwndClient;
// additional WinCreateWindow parameters
	LONG x,y,cx,cy;
	HWND hwndInsertBehind;
	ULONG id;
	PVOID pCtlData,pPresParams;
// register class part
	HAB hab;
	PCSZ pszClassName;
	PFNWP pfnWndProc;
	ULONG flClassStyle;
	ULONG cbWindowData;
};

////////////////////////////////////////////////////////////////////////////

MRESULT PMWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT PMDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

////////////////////////////////////////////////////////////////////////////


struct PMEvent 
{
	ULONG msg;
	MPARAM mp1;
	MPARAM mp2;
	MRESULT ret;
	PCHRMSG charmsg;
	PMSEMSG mousemsg;
	PMEvent(ULONG m,MPARAM n,MPARAM l,PCHRMSG c=NULL,PMSEMSG ms=NULL) { 
		msg=m; mp1=n; mp2=l; charmsg=c; mousemsg=ms; ret=0;
	}

};

/////////////////////////////////////////////////////////////////////////////

class PMWin 
{
protected:
	PFNWP oldproc;
	WINCREATEARGS *createArgs;
	HWND hwnd,hwndFrame;
	PMAnchorBlock ab;
	virtual BOOL dispachEvent(PMEvent & event);
	friend MRESULT PMWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
	friend MRESULT PMDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

public:
	PMWin(PCSZ classname,HAB iab); // se non viene specificato nessun a.b., prendi App di default
	PMWin(HAB iab); // costruttore semplice per le finestre meno complicate
	virtual ~PMWin();

	HWND getHwnd() { return hwnd; }
	HWND operator () () { return hwnd; }
	operator HWND () { return hwnd; }
	HWND getHwndFrame() { return hwndFrame; }

	virtual BOOL createWin();	
	virtual BOOL destroyWin();	
	virtual MRESULT msgProc(PMEvent& event);

	ULONG msgBox(PCSZ title,PCSZ msg,...);

	BOOL isChild(HWND hwndParent) { return WinIsChild(hwnd,hwndParent); }
	BOOL isEnabled() { return WinIsWindowEnabled(hwnd); }
	BOOL isVisible() { return WinIsWindowVisible(hwnd); }

	virtual MRESULT sendMsg (ULONG msg, MPARAM mp1=0L, MPARAM mp2=0L) 
		{	return WinSendMsg (hwnd, msg, mp1, mp2); }
	BOOL postMsg (ULONG msg, MPARAM mp1=0L, MPARAM mp2=0L) 
		{ 	return WinPostMsg (hwnd, msg, mp1, mp2); }
	virtual MRESULT close(); // manda WM_CLOSE
	BOOL invalidate(BOOL includeChildren) { return WinInvalidateRect(hwnd,NULL,includeChildren); }

	HWND queryCapture(HWND hwndDesktop=HWND_DESKTOP) 
		{	return WinQueryCapture (hwndDesktop); }
	HWND queryFocus(HWND hwndDesktop=HWND_DESKTOP) 
		{ 	return WinQueryFocus (hwndDesktop); }
	BOOL setCapture(HWND hwndDesktop=HWND_DESKTOP) 
		{	return WinSetCapture (hwndDesktop,hwnd); }
	BOOL resetCapture(HWND hwndDesktop=HWND_DESKTOP) 
		{	return WinSetCapture (hwndDesktop,NULLHANDLE); }
	BOOL queryPos(PSWP pswp) 
		{ 	return WinQueryWindowPos (hwnd, pswp); }
	BOOL queryFramePos(PSWP pswp) 
		{ 	return WinQueryWindowPos (hwndFrame, pswp); }
	LONG queryWindowText(LONG cchBufferMax, PCH pchBuffer)
		{ 	return WinQueryWindowText (hwnd,cchBufferMax,pchBuffer); }
	LONG queryWindowTextLenght() 
		{ 	return WinQueryWindowTextLength (hwnd); }
	BOOL setOwner(HWND hwndNewOwner)
		{ 	return WinSetOwner (hwnd,hwndNewOwner); }
	BOOL setParent(HWND hwndNewParent, BOOL fRedraw=FALSE)
		{ 	return WinSetParent (hwnd,hwndNewParent,fRedraw); }
	BOOL setPos(HWND hwndInsertBehind, LONG x, LONG y, LONG cx, LONG cy, ULONG fl=0L)
	  	{ 	return WinSetWindowPos(hwnd,hwndInsertBehind,x,y,cx,cy,fl); }
	BOOL setFramePos(HWND hwndInsertBehind, LONG x, LONG y, LONG cx, LONG cy, ULONG fl=0L)
	  	{ 	return WinSetWindowPos(hwndFrame,hwndInsertBehind,x,y,cx,cy,fl); }
	BOOL setWindowText(PCSZ pszText) 
		{ 	return WinSetWindowText (hwnd,pszText); }
	BOOL setFrameWindowText(PCSZ pszText) 
		{ 	return WinSetWindowText (hwndFrame,pszText); }
	BOOL ptInClient(PMPoint& pt);
	BOOL restoreWindowPos (PCSZ pszAppName, PCSZ pszKeyName, HWND h=NULLHANDLE)
		{	
			if (h==NULLHANDLE) h=hwndFrame;
			return WinRestoreWindowPos (pszAppName, pszKeyName, h); 
		}
	BOOL storeWindowPos (PCSZ pszAppName, PCSZ pszKeyName, HWND h=NULLHANDLE)
		{	
			if (h==NULLHANDLE) h=hwndFrame;
			return WinStoreWindowPos (pszAppName, pszKeyName, h); 
		}
		
	BOOL associateHelpInstance (HWND hwndHelpInstance)
		{	return WinAssociateHelpInstance (hwndHelpInstance, hwndFrame); }
	HWND queryHelpInstance ()
		{	return WinQueryHelpInstance (hwnd); }


	virtual BOOL queryEnd() { return TRUE; } // always exit from window

// Standard message handling functions //////////////////////////////////////
// the return BOOL of the following functions should be FALSE if
// you want to call the default message handling procedure (ie the
// message was not processed, TRUE if you don't want to.
	virtual BOOL command(USHORT id,USHORT cmddev);
	virtual BOOL create(PMEvent &event);
	virtual BOOL destroy();
	virtual void saveApplication();		 // exception: PM requires that the default procedure always be called
	virtual BOOL paint();				 // generic paint WM_PAINT
	virtual BOOL kbd(PMEvent &event);	 // any keyboard message
	virtual BOOL mouse(PMEvent &event);	 // any mouse message
	virtual BOOL enable();
	virtual BOOL show();	
	virtual BOOL move();
	virtual BOOL size(SHORT cx,SHORT cy);
	virtual BOOL activate();
	virtual BOOL setfocus();
	virtual BOOL help(PMEvent &event);	 // WM_HELP message
	virtual BOOL helpmsg(PMEvent &event);// any WM_ help-related message
	virtual BOOL timer();
	virtual BOOL closed();               // received WM_CLOSE
	virtual BOOL other(PMEvent &event);	 // all other messages
	virtual BOOL initmenu(SHORT id,PMMenu *menu); // menu initialization
};


/////////////////////////////////////////////////////////////////////////////
// PMMainWin add automatic support for menus, and active file handling.

// PMMainWin File Flags
//
#define PMMWFF_NOTHING 0				 // main window does not use files
#define PMMWFF_NONE 1					 // no active file
#define PMMWFF_UNTITLED 2				 // untitled document
#define PMMWFF_FILE 3					 // open file

class PMMainWin : public PMWin 
{
protected:
	char filename[CCHMAXPATH];
	BOOL modified;
	int fileFlags;
	char *caption;
	char *fnFilter,*fsCaption,*flCaption;
	char *fileUntitled,*fileNone;
	BOOL fileKillConfirm();
// imposta il titolo della finestra da caption e filename
// set the title of the window using caption and filename
	void setTitleCaption(); 
	PMHelpWin* helpWin;		// finestra a cui chiedere help (help instance window)
public:
	PMMainWin(PCSZ classname,HAB ab,PMHelpWin *ihelpWin=NULL);
	~PMMainWin() { };
	BOOL paint();
	BOOL command(USHORT id,USHORT cmddev);
	BOOL createWin();	
	BOOL destroyWin();	
	virtual BOOL paint(PMPresSpace& ) { return FALSE; }
//	BOOL helpmsg(PMEvent &event); // ridefinisci la gestione dell'aiuto

	BOOL isFileModified() { return modified; }
	BOOL setModified(BOOL state) { BOOL temp=modified; modified=state; setTitleCaption(); return temp; }

	BOOL queryEnd() { return fileKillConfirm(); } // query file kill confirm
	BOOL closed(); // riceived WM_CLOSE, see if I can close the window

// standard menu selections
	virtual BOOL fileOpen(PCSZ /*filename*/) { return TRUE; }; // return TRUE if everything is Ok
	virtual BOOL fileSave(PCSZ /*filename*/) { return TRUE; };
	virtual BOOL filePrint() { return TRUE; };
	virtual BOOL fileNew() { return TRUE; };

	virtual BOOL editCut() { return TRUE; };
	virtual BOOL editCopy() { return TRUE; };
	virtual BOOL editPaste() { return TRUE; };
	virtual BOOL editDelete() { return TRUE; };
	virtual BOOL editUndo() { return TRUE; };
	virtual BOOL editRedo() { return TRUE; };
	virtual BOOL editClear() { return TRUE; };
};

/////////////////////////////////////////////////////////////////////////////

class PMSubclassWin : public PMWin {
public:
	PMSubclassWin(HWND hwndParent, PCSZ pszClass, PCSZ pszName,
	    ULONG flStyle, LONG x, LONG y, LONG cx, LONG cy, HWND hwndOwner,
    	HWND hwndInsertBehind, ULONG id, PVOID pCtlData=NULL, PVOID pPresParams=NULL);
	~PMSubclassWin();

	BOOL createWin();	
};

/////////////////////////////////////////////////////////////////////////////
// Menus
//

class PMMenu 
{
protected:
	HWND menu;
	PMWin* win;
public:
	PMMenu(PMWin* iwin);  // menu' della finestra
	PMMenu(HWND menu); // menu qualsiasi
PMMenu(); // costruttore vuoto, per le classi derivate..
	BOOL checkItem(SHORT id,SHORT fcheck) {
		return WinCheckMenuItem(menu,id,fcheck);
	}
	BOOL enableItem(SHORT id,SHORT fEnable) {
		return WinEnableMenuItem(menu,id,fEnable);
	}
	BOOL isItemChecked(SHORT id) {
		return WinIsMenuItemChecked(menu,id);
	}
	BOOL isItemEnabled(SHORT id) {
		return WinIsMenuItemEnabled(menu,id);
	}
	BOOL isItemValid(SHORT id) {
		return WinIsMenuItemValid(menu,id);
	}                 
	BOOL setItemText(SHORT id,PSZ psz) {
		return WinSetMenuItemText(menu,id,psz);
	}
};


// window system menu //////////////////////////////////////////////////////
class PMSystemMenu : PMMenu 
{
public:
	PMSystemMenu(PMWin* iwin);  
};


// popup menu //////////////////////////////////////////////////////////////

class PMPopupMenu : PMMenu 
{
public:
	PMPopupMenu(PMWin* iwin,ULONG idres); 
	BOOL popup(PMPoint& pt,ULONG sel,ULONG flags=PU_POSITIONONITEM|PU_MOUSEBUTTON1|PU_MOUSEBUTTON2|PU_KEYBOARD);
};

/////////////////////////////////////////////////////////////////////////////

#endif

