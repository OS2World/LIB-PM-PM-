/* 


	pmwin.cpp (emx+gcc) 

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
#include <stdarg.h>

#include "pmwin.h"
#include "pmgpi.h"
#include "pmdlg.h"
#include "pmhelp.h"
#include "pmstdres.h"


MRESULT PMWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	PMWin* win=NULL;
	
	if (msg==WM_CREATE) {
		WinSetWindowPtr(hwnd,0,mp1);  // mp1=this pointer from WinCreateWindow, line 125
		((PMWin*)mp1)->hwnd=hwnd;     // PMWin::hwnd is set to hwnd
	}

	win = (PMWin*)WinQueryWindowPtr(hwnd, 0);

	if (win) { // is there a C++ window object to process messages?
		PMEvent event(msg,mp1,mp2,CHARMSG(&msg),MOUSEMSG(&msg)); // yes
		MRESULT ret=win->msgProc(event);
		if (msg==WM_DESTROY) delete win; // automatically destroy the C++ object il the window is destroyed
		return ret;
	}
	return WinDefWindowProc(hwnd,msg,mp1,mp2); // no
}


/////////////////////////////////////////////////////////////////////////////
// PMWin class
// the constructors follow. Note that the constructors only fill in the
// createArgs structure. The window is actually created by the createWin 
// function

PMWin::PMWin(HAB iab)
{
	ab.PMAnchorBlock(iab);
	createArgs=NULL;
	hwnd=(HWND)0;
	hwndFrame=(HWND)0;
	oldproc=WinDefWindowProc; // for safety
}

PMWin::PMWin(PCSZ classname,HAB iab)
{
	ab.PMAnchorBlock(iab);
	createArgs=new WINCREATEARGS;
	hwnd=(HWND)0;
	hwndFrame=(HWND)0;
	oldproc=WinDefWindowProc; 
	
	createArgs->hwndParent=HWND_DESKTOP;
	createArgs->flStyle= WS_VISIBLE;
    createArgs->flCreateFlags=(FCF_TITLEBAR | FCF_SYSMENU |
                  FCF_SIZEBORDER | FCF_MINMAX |
                  FCF_SHELLPOSITION | FCF_TASKLIST);
    createArgs->pszClientClass=classname;
    createArgs->pszTitle=NULL;
    createArgs->styleClient=0L;
    createArgs->hmod=(HMODULE)0;
    createArgs->idResources=0;
    createArgs->phwndClient=&hwnd;

    createArgs->hab=iab;
    createArgs->pszClassName=classname;
    createArgs->pfnWndProc=PMWndProc;
    createArgs->flClassStyle=CS_SIZEREDRAW;
    createArgs->cbWindowData=sizeof(this);
}

PMWin::~PMWin()
{
	delete createArgs;
}

// I'm using the two-phase method of window creation (frame and client)
// beacuse I want to intercept the WM_CREATE message of the client window.
// (see the PMWndProc functions at the beginning of this file)

BOOL PMWin::createWin()
{
	HWND newhwnd;
	FRAMECDATA fcdata;

	CLASSINFO temp;
	if (!WinQueryClassInfo(ab,createArgs->pszClassName,&temp)) {
		BOOL ret=WinRegisterClass (createArgs->hab, createArgs->pszClassName, 
			createArgs->pfnWndProc,createArgs->flClassStyle, createArgs->cbWindowData);
		assert(ret);
	}
	
	oldproc=WinDefWindowProc;

	fcdata.cb = sizeof(FRAMECDATA); // fill in the frame control data structure
	fcdata.flCreateFlags = createArgs->flCreateFlags;
	fcdata.hmodResources = createArgs->hmod;
	fcdata.idResources   = createArgs->idResources;
	
	newhwnd = WinCreateWindow(createArgs->hwndParent,WC_FRAME,
							createArgs->pszTitle,0,
							0,0,0,0,    /* coordinates */ 0, // no owner 
							HWND_TOP,createArgs->idResources,&fcdata,NULL);
	
	assert(newhwnd);
	hwndFrame=newhwnd;
	newhwnd = WinCreateWindow(hwndFrame,createArgs->pszClassName,
							NULL,0,
							0,0,0,0,    /* coordinates */ 
							hwndFrame,
							HWND_TOP,FID_CLIENT,this,NULL);

	assert(newhwnd);
//	hwnd=newhwnd; // not necessary, this->hwnd is set by PMWndProc
	WinShowWindow(hwndFrame, TRUE);

//	BOOL ret=WinSetWindowPtr(hwnd, 0, this); // set the window user pointer to the pointer to this class. see PMWndProc
//	assert(ret);
	return hwnd;
}
//	newhwnd = WinCreateStdWindow (createArgs->hwndParent,createArgs->flStyle,
//				&createArgs->flCreateFlags,createArgs->pszClientClass,
//				createArgs->pszTitle,createArgs->styleClient,createArgs->hmod,
//				createArgs->idResources,createArgs->phwndClient);

// a BOOL TRUE from dispachEvent means that we processed the message.
MRESULT PMWin::msgProc(PMEvent &event)
{
	event.ret=MRFROMSHORT(FALSE);
	// if we handle the message return the ret code, otherwise call the default message handler
	return dispachEvent(event) ? event.ret : (*oldproc)(hwnd,event.msg,event.mp1,event.mp2);
}

// dispach messages among the predefinde functions
BOOL PMWin::dispachEvent(PMEvent & event)
{
	if ((event.msg>=WM_MOUSEFIRST && event.msg<=WM_MOUSELAST) ||
			(event.msg>=WM_EXTMOUSEFIRST && event.msg<=WM_EXTMOUSELAST) ) {
		return mouse(event);
	} else if (event.msg>=HM_MSG_BASE && event.msg<=HM_CONTROL) {
		return helpmsg(event);
	} else
		switch (event.msg) {
		case WM_CREATE:
			return create(event);
		case WM_DESTROY:
			return destroy();
		case WM_SAVEAPPLICATION:
			saveApplication();
			return FALSE;
		case WM_CHAR: 
			return kbd(event);
		case WM_COMMAND:
			return command(SHORT1FROMMP (event.mp1),SHORT2FROMMP (event.mp2));
		case WM_PAINT:
			return paint();
		case WM_ENABLE:
			return enable();
		case WM_SHOW:
			return show();
		case WM_MOVE:
			return move();
		case WM_SIZE:
			return size(SHORT1FROMMP (event.mp2),SHORT2FROMMP (event.mp2));
		case WM_ACTIVATE:
			return activate();
		case WM_SETFOCUS:
			return setfocus();
		case WM_HELP:
			return help(event);
		case WM_TIMER:
			return timer();
		case WM_CLOSE:
			return closed();
		case WM_PRESPARAMCHANGED:
			invalidate(TRUE);
			return TRUE;
		case WM_INITMENU:
			PMMenu* menu=new PMMenu(HWNDFROMMP(event.mp2));
			return initmenu(SHORT1FROMMP(event.mp1),menu);
			delete menu;
		}		   	
	return other(event);
}

// predefinde message classes
BOOL PMWin::create(PMEvent &)
{
	return FALSE;
}

BOOL PMWin::destroy()
{
	return FALSE;
}

// exception to the BOOL return value rool: PM requires that the default procedure always be called
void PMWin::saveApplication()
{
}

BOOL PMWin::command(USHORT /*id*/,USHORT /*cmddev*/)  // nothandled
{
	return FALSE;
}

BOOL PMWin::paint()
{
	return FALSE;
}

BOOL PMWin::kbd(PMEvent &)
{
	return FALSE;
}

BOOL PMWin::mouse(PMEvent &)
{
	return FALSE;
}

BOOL PMWin::enable()
{
	return FALSE;
}
BOOL PMWin::show()	
{
	return FALSE;
}
BOOL PMWin::move()
{
	return FALSE;
}
BOOL PMWin::size(SHORT,SHORT)
{
	return FALSE;
}
BOOL PMWin::activate()
{
	return FALSE;
}
BOOL PMWin::setfocus()
{
	return FALSE;
}
BOOL PMWin::help(PMEvent &)
{
	return FALSE;
}
BOOL PMWin::helpmsg(PMEvent &)
{
	return FALSE;
}

BOOL PMWin::timer()
{
	return FALSE;
}
BOOL PMWin::closed()
{
	return FALSE;
}

BOOL PMWin::other(PMEvent &)
{
	return FALSE;
}

MRESULT PMWin::close() 
{ 
	return sendMsg(WM_CLOSE); 
}

BOOL PMWin::destroyWin()
{
	return WinDestroyWindow(hwndFrame);
}

BOOL PMWin::initmenu(SHORT /*id*/,PMMenu* /*menu*/)
{
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// additional function

BOOL PMWin::ptInClient(PMPoint& pt)
{
	PMRect rcl;
	rcl=this;
	return WinPtInRect (ab, &rcl, &pt);
}

/////////////////////////////////////////////////////////////////////////////
// PMMain Window.
// Adds automatic support to file handling, title bar text and menus and help

PMMainWin::PMMainWin(PCSZ classname,HAB ab,PMHelpWin *ihelpWin) : PMWin(classname,ab) 
{ 
	helpWin=ihelpWin;
	caption=NULL;
	modified=FALSE;
	filename[0]='\0';
	fileFlags=PMMWFF_NOTHING;
	fileNone="(none)"; fileUntitled="(untitled)";
	flCaption=fsCaption=fnFilter=NULL;
}


// inline help support (this create uses the standard create function
// and adds the automatic association to the help instance)

// supporto per help inline (usa tutto create della classe base, e aggiunge
// l'associazione automatica alla finestra di help!)

BOOL PMMainWin::createWin()
{
	BOOL ret;
	ret=PMWin::createWin();
	if (helpWin) ret&=associateHelpInstance(helpWin->getHwnd());
	return ret;
}

BOOL PMMainWin::destroyWin()
{
	BOOL ret;
	if (helpWin) {
		ret=associateHelpInstance(NULLHANDLE);
		ret&=PMWin::destroyWin();
	} else ret=PMWin::destroyWin();
	return ret;
}

// automatically create a WindowPresSpace (WinBeginPaint) and pass it to 
// the official paint function
BOOL PMMainWin::paint() 
{
	PMWindowPresSpace ps(this);
	return paint(ps);
}

// the window wants to be closed. Ask to the user if it is possible (queryEnd)
// and do it.
BOOL PMMainWin::closed()
{
	if (queryEnd()) postMsg(WM_QUIT);
	return TRUE;
}

// used by queryEnd to ask the user if it is possible to close the file
BOOL PMMainWin::fileKillConfirm()
{
	if (!modified) return TRUE;
	char buf[160];
	strcpy(buf,"Save Changes");
	if (fileFlags==PMMWFF_FILE) {
		strcat(buf," to ");
		strcat(buf,filename);
	}
	strcat(buf,"?");
	int ret=WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,(PCSZ)buf,(PCSZ)caption,0,MB_YESNOCANCEL|MB_ICONHAND|MB_APPLMODAL);
	if (ret==MBID_YES) command(PMIDM_FILESAVE,0);
	if (ret==MBID_CANCEL) return FALSE;
	return TRUE;		
}

// automatic title text setter. (automagically adds an * for modified files)
void PMMainWin::setTitleCaption()
{
	char temp[256];
	if (caption==NULL) temp[0]='\0';
	else {
		switch (fileFlags) {
		case PMMWFF_NOTHING:
			sprintf(temp,"%s",caption);
			break;
		case PMMWFF_NONE:
			if (modified)
				sprintf(temp,"%s - *%s",caption,fileNone);
			else
				sprintf(temp,"%s - %s",caption,fileNone);
			break;
		case PMMWFF_UNTITLED:
			if (modified)
				sprintf(temp,"%s - *%s",caption,fileUntitled);
			else
				sprintf(temp,"%s - %s",caption,fileUntitled);
			break;
		case PMMWFF_FILE:
			if (modified)
				sprintf(temp,"%s - *%s",caption,filename);
			else
				sprintf(temp,"%s - %s",caption,filename);
			break;
		}
	}
	setFrameWindowText(temp);
}

// handle automatically menu selections.
BOOL PMMainWin::command(USHORT id,USHORT /*cmddev*/)
{
	switch(id) {
	case PMIDM_FILENEW:
		if (fileKillConfirm()) {
			if (fileNew()) { fileFlags=PMMWFF_UNTITLED; setModified(FALSE);	}
		}
		return TRUE;
	case PMIDM_FILEOPEN: {
		if (fileKillConfirm()) {
			PMFileDialog fd(HWND_DESKTOP, hwnd, fnFilter, flCaption, "Open", FDS_OPEN_DIALOG);
			int ret=fd.createWin();
			if (ret==1) {
				FILEDLG fi=fd;
				if (fileOpen(fi.szFullFile)) { // if load clompleted successfully, change the caption
	  	  			strcpy(filename,fi.szFullFile);
					fileFlags=PMMWFF_FILE;
				}
				setModified(FALSE);
			}
		}
		return TRUE;
		}	
	case PMIDM_FILESAVE: 
		if (filename[0]) { // if there's already a filename, save the file
			fileSave(filename);
			setModified(FALSE);
			return TRUE;
		} // otherwise fallback to SAVEAS
	case PMIDM_FILESAVEAS: {
		PMFileDialog fd(HWND_DESKTOP, hwnd, fnFilter, fsCaption, "Save", FDS_SAVEAS_DIALOG);
		int ret=fd.createWin();
		if (ret==1) {
			FILEDLG fi=fd;
			if (fileSave(fi.szFullFile)) { // if correctly saved, change caption
				strcpy(filename,fi.szFullFile);
				fileFlags=PMMWFF_FILE;
			}
			setModified(FALSE);
		}
		return TRUE;
		}	
	case PMIDM_FILEPRINT :
		filePrint();
		return TRUE;
	case PMIDM_FILEEXIT :
		close();
		return TRUE;

	case PMIDM_EDITUNDO :
		editUndo();
		return TRUE;
	case PMIDM_EDITREDO :
		editRedo();
		return TRUE;
	case PMIDM_EDITCUT :
		editCut();
		return TRUE;
	case PMIDM_EDITCOPY :
		editCopy();
		return TRUE;
	case PMIDM_EDITPASTE:
		editPaste();
		return TRUE;
	case PMIDM_EDITCLEAR:
		editClear();
		return TRUE;
	case PMIDM_EDITDUPLICATE :
	case PMIDM_EDITSELECTALL :
		return FALSE;

	case PMIDM_HELPINDEX :
		helpWin->sendMsg(HM_HELP_INDEX);
		return TRUE;
	case PMIDM_HELPGENERAL :
		helpWin->sendMsg(HM_EXT_HELP);
		return TRUE;
	case PMIDM_HELPUSING :
		helpWin->sendMsg(HM_DISPLAY_HELP);
		return TRUE;
	case PMIDM_HELPKEYS :
		helpWin->sendMsg(HM_KEYS_HELP);
		return TRUE;

	case PMIDM_HELPABOUT :
        WinDlgBox(HWND_DESKTOP,hwnd,(PFNWP)PMDlgProc,NULLHANDLE,PMDLG_ABOUT,NULL);
		return TRUE;

	}
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////

//static MRESULT PMSubclassWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
//{
//	PMWin* win=NULL;
//
//	win = (PMWin*)WinQueryWindowPtr(hwnd, 0);
//
//	if (win) {
//		PMEvent event(msg,mp1,mp2,CHARMSG(&msg),MOUSEMSG(&msg));
//		MRESULT ret=win->msgProc(event);
//		if (msg==WM_DESTROY) delete win; 
//		return ret;
//	}
//	return WinDefWindowProc(hwnd,msg,mp1,mp2);
//}

PMSubclassWin::PMSubclassWin(HWND hwndParent, PCSZ pszClass, PCSZ pszName,
    ULONG flStyle, LONG x, LONG y, LONG cx, LONG cy, HWND hwndOwner,
   	HWND hwndInsertBehind, ULONG id, PVOID pCtlData, PVOID pPresParams) : PMWin(NULLHANDLE)
{
	createArgs=new WINCREATEARGS;
	
	createArgs->hwndParent=hwndParent;
    createArgs->pszClientClass=pszClass;
    createArgs->pszTitle=pszName;
	createArgs->flStyle=flStyle;
    createArgs->x=x;
    createArgs->y=y;
    createArgs->cx=cx;
    createArgs->cy=cy;
	createArgs->hwndOwner=hwndOwner;
    createArgs->hwndInsertBehind=hwndInsertBehind;
    createArgs->id=id;
    createArgs->pCtlData=pCtlData;
    createArgs->pPresParams=pPresParams;
    
}

PMSubclassWin::~PMSubclassWin()
{
	delete createArgs;
}

BOOL PMSubclassWin::createWin()
{
	hwnd=WinCreateWindow(createArgs->hwndParent,
						createArgs->pszClientClass,
						createArgs->pszTitle,
						createArgs->flStyle,
						createArgs->x, createArgs->y,
						createArgs->cx, createArgs->cy,
						createArgs->hwndParent,
						createArgs->hwndInsertBehind,
						createArgs->id,
						createArgs->pCtlData,
						createArgs->pPresParams);
	BOOL ret=WinSetWindowPtr(hwnd, 0, this);	// the first few messages (WM_CREATE...) are not sent to the massage handling routine
	assert(ret);
	ab.PMAnchorBlock( WinQueryAnchorBlock(hwnd) );
	oldproc=WinSubclassWindow(hwnd,PMWndProc);			// PMWndProc gets active only at this point
	return hwnd;
}


/////////////////////////////////////////////////////////////////////////////
// Support functions:
// DoAssert executes the assert macro code, displaying the offending condition, linenumber and source file
// ErrBox shows a critical error box
// MsgBox shows a message box (with optional sprintf-like parameter list)

void DoAssert(PCSZ msg,PCSZ file,unsigned line)
{
	char buf[160];
	sprintf(buf,"at line %u in file %s: \"%s\"\nOK to continue, Cancel to close application.\n",line,file,msg);
	if (WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,buf,"ASSERTION FAILURE",PMHLP_DLG_ASSERTFAIL,MB_OKCANCEL|MB_ICONHAND|MB_APPLMODAL|MB_HELP)!=MBID_OK) exit(-1);
}

void ErrBox(PCSZ msg,...)
{
	char buf[256];

	va_list arg_ptr;
	va_start(arg_ptr,msg);
	vsprintf(buf,msg,arg_ptr);
	va_end(arg_ptr);

	WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,buf,"Error!",PMHLP_DLG_ERRMSG,MB_CUACRITICAL|MB_APPLMODAL|MB_HELP);
}

ULONG PMWin::msgBox(PCSZ title,PCSZ msg,...)
{
	char buf[256];

	va_list arg_ptr;
	va_start(arg_ptr,msg);
	vsprintf(buf,msg,arg_ptr);
	va_end(arg_ptr);
	
	return WinMessageBox(HWND_DESKTOP,hwnd,buf,title,PMHLP_DLG_MSGWIN,MB_OK|MB_HELP);
}

/*
 * Local variables:
 * compile-command: "dmake pmwin.o"
 * end:
 */
