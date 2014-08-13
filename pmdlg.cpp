/* 


	pmdlg.cpp (emx+gcc) 

	1994,95 Giovanni Iachello
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

#define INCL_WINMLE
#include "pmwin.h"
#include "pmdlg.h"
#include "pmstdres.h"

/////////////////////////////////////////////////////////////////////////////

MRESULT PMDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	PMDialog* win=NULL;
	if (msg==WM_INITDLG) {
		win=(PMDialog*)mp2;
		if (win) { 
			WinSetWindowPtr(hwnd,0,win);
			win->hwnd=hwnd;				  // e naturalmente aggiorna hwnd ... ;)
			win->hwndFrame=hwnd;
			win->init();
			win->initdlg();
			return (MRESULT)TRUE;               // if focus changed
		}
	}

	win = (PMDialog*)WinQueryWindowPtr(hwnd, 0);
	if (win) {
		PMEvent event(msg,mp1,mp2,CHARMSG(&msg),MOUSEMSG(&msg));
		return win->msgProc(event);
	}
	return WinDefDlgProc(hwnd,msg,mp1,mp2);
}


/////////////////////////////////////////////////////////////////////////////

PMControl::PMControl(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMWin(WinQueryAnchorBlock(parentDlg))
{
	hwndFrame=parentDlg; obj=iobj; len=ilen; id=iid;
	hwnd=WinWindowFromID(hwndFrame,id);
	// this is not used because the library can determine the pointer to
	// the PMControl in another way, and it does not work for some controls
	// (MLE...). Anyway it is done for completeness.
	WinSetWindowPtr(hwnd,0,this); // okkey
}

BOOL PMControl::destroyWin()
{
	return WinDestroyWindow(hwnd);
}

// inhibit creating controls.
BOOL PMControl::createWin()
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////

PMButton::PMButton(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMControl(parentDlg,iobj,ilen,iid) 
{
}

void PMButton::get() 
{ 
}

void PMButton::set() 
{ 
}

/////////////////////////////////////////////////////////////////////////////

PMRadioButtonGroup::PMRadioButtonGroup(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMButton(parentDlg,iobj,ilen,iid) 
{
}

void PMRadioButtonGroup::get() 
{ 
	int ret=queryIndex();
	*((int*)obj)=ret;
}

void PMRadioButtonGroup::set() 
{ 
	HWND hwndctrl=WinWindowFromID(hwndFrame,id+(*(int*)obj)); // trova hwnd del controllo da attivare
	WinSendMsg(hwndctrl,BM_SETCHECK,(MPARAM)1,(MPARAM)0);
}

/////////////////////////////////////////////////////////////////////////////

PMCheckBox::PMCheckBox(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMButton(parentDlg,iobj,ilen,iid) 
{
}

void PMCheckBox::get() 
{ 
	int ret=queryCheck();
	*((int*)obj)=ret;
}

void PMCheckBox::set() 
{ 
	setCheck(*((int*)obj));
}

/////////////////////////////////////////////////////////////////////////////

PMEntryField::PMEntryField(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMControl(parentDlg,iobj,ilen,iid) 
{ 
	WinSendDlgItemMsg(hwndFrame,id,EM_SETTEXTLIMIT,MPFROMSHORT(len),0);
}

void PMEntryField::get() 
{ 
	WinQueryDlgItemText(hwndFrame,id,len,(PSZ)obj);
}

void PMEntryField::set() 
{ 
	WinSetDlgItemText(hwndFrame,id,(PSZ)obj);
}

/////////////////////////////////////////////////////////////////////////////

PMMultiLineEdit::PMMultiLineEdit(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMControl(parentDlg,iobj,ilen,iid) 
{ 
	WinSendDlgItemMsg(hwndFrame,id,MLM_SETTEXTLIMIT,MPFROMSHORT(len),0);
}

void PMMultiLineEdit::get() 
{ 
	WinQueryDlgItemText(hwndFrame,id,len,(PSZ)obj);
}

void PMMultiLineEdit::set() 
{ 
	WinSetDlgItemText(hwndFrame,id,(PSZ)obj);
}

////////////////////////////////////////////////////////////////////////////

PMListBox::PMListBox(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMControl(parentDlg,iobj,ilen,iid) 
{ 
}

PMListBox::~PMListBox()
{
}

void PMListBox::get()
{
	ULONG ret=(ULONG)WinSendMsg(hwnd,LM_QUERYSELECTION,MPFROMLONG(LIT_FIRST),(MPARAM)NULL);
	*((int*)obj)=ret;
}

void PMListBox::set()
{
	WinSendMsg(hwnd,LM_SELECTITEM,MPFROMLONG(*((int*)obj)),(MPARAM)NULL);
}

////////////////////////////////////////////////////////////////////////////

PMValueSet::PMValueSet(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMControl(parentDlg,iobj,ilen,iid) 
{ 
}

void PMValueSet::get()
{
	MRESULT ret=querySelectedItem();
	*((MRESULT*)obj)=ret;
}

void PMValueSet::set()
{
	selectItem(*(MPARAM*)obj);
}

////////////////////////////////////////////////////////////////////////////

PMSpinButton::PMSpinButton(HWND parentDlg,void* iobj,int ilen,int iid) :
	PMControl(parentDlg,iobj,ilen,iid) 
{ 
}

void PMSpinButton::get()
{
	queryValue((PVOID)obj,0,SPBQ_ALWAYSUPDATE);
}

void PMSpinButton::set()
{
	setCurrentValue(*(LONG*)obj);
}

////////////////////////////////////////////////////////////////////////////

PMScrollBar::PMScrollBar(HWND parentDlg, void* iobj, int ilen, int iid) :
	PMControl(parentDlg,iobj,ilen,iid)
{
}

PMScrollBar::PMScrollBar(PMWin* win, int* iobj, int iid)	:
	PMControl(win->getHwndFrame(),iobj,4,iid)
{
}

void PMScrollBar::get()
{
	*(int*)obj=int(queryPos());
}

void PMScrollBar::set()
{
	setPos(SHORT(*(int*)obj));
}

PMVertScrollBar::PMVertScrollBar(PMWin* win, int* iobj) :
	PMScrollBar(win,iobj,FID_VERTSCROLL)
{
}

PMHorizScrollBar::PMHorizScrollBar(PMWin* win, int* iobj) :
	PMScrollBar(win,iobj,FID_HORZSCROLL)
{
}


/////////////////////////////////////////////////////////////////////////////

PMDialog::PMDialog(HWND p,HWND o) : PMWin("",WinQueryAnchorBlock(o)) // chiama il costruttore complesso che riempie createargs
{
	createArgs->hwndParent=p;
	createArgs->hwndOwner=o;
	createArgs->hmod=NULLHANDLE;	
}

PMDialog::~PMDialog()
{
}

void PMDialog::init()
{
}

BOOL PMDialog::createWin()
{
	return FALSE;
}

BOOL PMDialog::destroyWin()
{
	return WinDismissDlg(hwnd,FALSE);
}


BOOL PMDialog::dispachEvent(PMEvent & event)
{
	switch (event.msg) {
		case WM_CONTROL: 
			return control(SHORT1FROMMP(event.mp1),SHORT2FROMMP(event.mp1));
		case WM_INITDLG:
			return initdlg();
	}		   	
	return PMWin::dispachEvent(event);
}

BOOL PMDialog::control(SHORT id,SHORT ctrlmsg)
{
	return FALSE;
}	

BOOL PMDialog::initdlg()
{
	return FALSE;
}	

BOOL PMDialog::command(USHORT id,USHORT cmddev)
{
	return FALSE;
}	

/////////////////////////////////////////////////////////////////////////////

PMModalDialog::PMModalDialog(HWND p,HWND o,ULONG i,PMControlMap *icm,void* iobj) : 
	PMDialog(p,o)
{
	id=i;
	cm=icm;
	obj=iobj;
	delctlmap=FALSE;
}

PMControl* PMModalDialog::controlFromID(int id)
{ 
	PMControl* ctl;
	ctl=(PMControl*)WinQueryWindowPtr(WinWindowFromID(hwnd,id),0); 
	if (!ctl) {
		PMControlMap* pcm=cm;
		for ( ; pcm->createfn!=NULL; pcm++ ) 
			if (pcm->id == id) { ctl=pcm->ctrl;	break; }
	}
	assert(ctl);
	return ctl;
}

PMModalDialog::~PMModalDialog()
{
	if (delctlmap) {
		PMControlMap* pcm=cm;
		for ( ; pcm->createfn!=NULL; pcm++ ) {
			delete pcm->ctrl; // elimina i controlli creati...
		}
		delctlmap=FALSE;
	}
}

void PMModalDialog::init()
{
	PMControlMap* pcm=cm;
	for ( ; pcm->createfn!=NULL; pcm++ ) {
		PMControl* ctl=pcm->createfn(hwnd,(char*)obj+pcm->offset,pcm->len,pcm->id);
		assert(ctl);
		ctl->set(); // imposta sullo schermo
		pcm->ctrl=ctl; // salva anche nella mappa dei controlli
	}
	delctlmap=TRUE;
	WinSetFocus(HWND_DESKTOP,WinWindowFromID(hwnd,pcm->id));
}	

void PMModalDialog::updateControls()
{
	PMControlMap* pcm=cm;
	for ( ; pcm->createfn!=NULL; pcm++ ) {
		pcm->ctrl->set(); // imposta sullo schermo
	}
}

void PMModalDialog::updateObject()
{
	PMControlMap* pcm=cm;
	for ( ; pcm->createfn!=NULL; pcm++ ) {
		pcm->ctrl->get(); // imposta nella struttura
	}
}

BOOL PMModalDialog::createWin()
{
	oldproc=WinDefDlgProc;
	ULONG ret=WinDlgBox(createArgs->hwndParent,createArgs->hwndOwner,PMDlgProc,createArgs->hmod,id,this);
	return ret;
}

BOOL PMModalDialog::command(USHORT id,USHORT cmddev)
{
	if (id==DID_OK) {
		WinDismissDlg(hwnd,TRUE);
		updateObject();		
	}
	if (id==DID_CANCEL) WinDismissDlg(hwnd,FALSE);
	return TRUE;
}	

BOOL PMModelessDialog::createWin()
{
	oldproc=WinDefDlgProc;
	ULONG ret=WinLoadDlg(createArgs->hwndParent,createArgs->hwndOwner,PMDlgProc,createArgs->hmod,id,this);
	return ret;
}
	

/////////////////////////////////////////////////////////////////////////////

PMFontDialog::PMFontDialog(HWND parent,HWND owner,FONTMETRICS *ifm,ULONG type,PSZ title,PSZ preview) : 
	PMDialog(parent,owner) 
{
	fm=ifm;
	memset((void*)&fontdlg, 0 , sizeof(FONTDLG));
	fontdlg.flType= type;
	fontdlg.pszTitle=title;
	fontdlg.pszPreview=preview;
	fontdlg.pszFamilyname=new CHAR[CCHMAXPATH]; // alloca zpazio per il nome della famiglia
}

PMFontDialog::~PMFontDialog()
{
	delete fontdlg.pszFamilyname;
}

BOOL PMFontDialog::createWin()
{
	fontdlg.cbSize=sizeof(FONTDLG);
	fontdlg.hpsScreen = WinGetScreenPS(HWND_DESKTOP);
	fontdlg.hpsPrinter= NULLHANDLE;
	fontdlg.pfnDlgProc= PMFontDlgProc; 
	fontdlg.ulUser= (ULONG)this;

	strcpy (fontdlg.pszFamilyname,fm->szFamilyname);
	fontdlg.usFamilyBufLen=CCHMAXPATH;
	fontdlg.fxPointSize= MAKEFIXED (fm->sNominalPointSize / 10 , 0);
	fontdlg.fl= FNTS_CENTER | FNTS_HELPBUTTON | FNTS_NOSYNTHESIZEDFONTS | FNTS_RESETBUTTON;
	fontdlg.clrFore=CLR_NEUTRAL;
	fontdlg.clrBack=CLR_BACKGROUND;
	fontdlg.usWeight=fm->usWeightClass;
	fontdlg.usWidth=fm->usWidthClass;

	oldproc=WinDefFontDlgProc;

	WinFontDlg (createArgs->hwndParent,createArgs->hwndOwner, &fontdlg);
	return fontdlg.lReturn;
}

MRESULT PMFontDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	PFONTDLG fdlg;
	PMFontDialog* win=NULL;  // il puntatore alla struttura fontdlg e' in WinPtr 0.

	fdlg = (PFONTDLG)WinQueryWindowPtr(hwnd, 0);
	win = (PMFontDialog*) fdlg->ulUser;
	if (win) {
		if (msg==WM_INITDLG) {
			win->hwnd=hwnd;				  // e naturalmente aggiorna hwnd ... ;)
			win->hwndFrame=hwnd;
			win->init();
		}
		PMEvent event(msg,mp1,mp2,CHARMSG(&msg),MOUSEMSG(&msg));
		return win->msgProc(event);
	}
	return WinDefFontDlgProc(hwnd,msg,mp1,mp2);
}


/////////////////////////////////////////////////////////////////////////////

PMFileDialog::PMFileDialog(HWND parent,HWND owner,PSZ file,PSZ title,PSZ OKButton,ULONG flags) : 
	PMDialog(parent,owner) 
{
	memset((void*)&filedlg, 0 , sizeof(FILEDLG));
	filedlg.pszTitle=title;
	filedlg.pszOKButton=OKButton;
	filedlg.fl=flags;
	strncpy(filedlg.szFullFile,file,CCHMAXPATH);
}

PMFileDialog::~PMFileDialog()
{
}

BOOL PMFileDialog::createWin()
{
	filedlg.cbSize=sizeof(FILEDLG);
	filedlg.pfnDlgProc= PMFileDlgProc; 
	filedlg.ulUser= (ULONG)this;
	filedlg.fl|= FDS_CENTER | FDS_PRELOAD_VOLINFO | FDS_HELPBUTTON;
	filedlg.pszIDrive="C:";

	oldproc=WinDefFileDlgProc;
	
	WinFileDlg (createArgs->hwndParent,createArgs->hwndOwner, &filedlg);
	return filedlg.lReturn;
}

MRESULT PMFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	PFILEDLG fdlg;
	PMFileDialog* win=NULL;  // il puntatore alla struttura fontdlg e' in WinPtr 0.

	fdlg = (PFILEDLG)WinQueryWindowPtr(hwnd, 0);
	win = (PMFileDialog*) fdlg->ulUser;
	if (win) {
		if (msg==WM_INITDLG) {
			win->hwnd=hwnd;				  // e naturalmente aggiorna hwnd ... ;)
			win->hwndFrame=hwnd;
			win->init();
		}
		PMEvent event(msg,mp1,mp2,CHARMSG(&msg),MOUSEMSG(&msg));
		return win->msgProc(event);
	}
	return WinDefFileDlgProc(hwnd,msg,mp1,mp2);
}



