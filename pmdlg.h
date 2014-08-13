/* 


	pmdlg.h (emx+gcc) 

	1994,95 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#ifndef _PMDLG_H_
#define _PMDLG_H_

class PMControl;

typedef PMControl* pPMControl;
typedef pPMControl (*NEWCTRLFN) (HWND,void*,int,int);

class PMControl : public PMWin {
protected:
	void* obj;
	int len;		
	int id;
	// qui il hwndFrame e' l'hwnd della Dlg...
public:
	PMControl(HWND parentdlg,void* iobj,int ilen,int iid);
	BOOL createWin();	
	BOOL destroyWin();
	virtual void get()=0;
	virtual void set()=0;
	MRESULT sendMsg(ULONG msg, MPARAM mp1=0L, MPARAM mp2=0L) 
		{ return WinSendDlgItemMsg(hwndFrame,id,msg,mp1,mp2); }

	BOOL queryShort(PSHORT pResult, BOOL fSigned) {
		return WinQueryDlgItemShort(hwndFrame, id, pResult, fSigned);
	}
	ULONG queryText(LONG cchBufferMax,PSZ pchBuffer) {
		return WinQueryDlgItemText (hwndFrame, id, cchBufferMax, pchBuffer);
	}
	LONG queryTextLenght() {
		return WinQueryDlgItemTextLength (hwndFrame, id);
	}
	BOOL setShort( USHORT usValue, BOOL fSigned) {
		return WinSetDlgItemShort (hwndFrame, id, usValue, fSigned);
	}
	BOOL setText (PCSZ pszText) {
		return WinSetDlgItemText (hwndFrame, id, pszText);
	}
};

/////////////////////////////////////////////////////////////////////////////

class PMButton : public PMControl {
protected:
public:
	PMButton(HWND parentDlg,void* iobj,int ilen,int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMButton(parent,obj,len,id); }
	BOOL queryHilite() { return (BOOL)sendMsg(BM_QUERYHILITE); }
	BOOL setHilite(USHORT usHilite) { return (BOOL)sendMsg(BM_SETHILITE,(MPARAM)usHilite); }
	ULONG click(USHORT usUp) { return (ULONG)sendMsg(BM_CLICK,(MPARAM)usUp); }
	USHORT queryCheck() { return (USHORT)sendMsg(BM_QUERYCHECK); }
	USHORT setCheck(USHORT usCheck) { return (USHORT)sendMsg(BM_SETCHECK,(MPARAM)usCheck); }
	void get();
	void set();
};

class PMRadioButtonGroup : public PMButton {
protected:
public:
	PMRadioButtonGroup(HWND parentDlg,void* iobj,int ilen,int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMRadioButtonGroup(parent,obj,len,id); }
	SHORT queryIndex() { return (SHORT)sendMsg(BM_QUERYCHECKINDEX); }
	void get();
	void set();
};

class PMCheckBox : public PMButton {
protected:
public:
	PMCheckBox(HWND parentDlg,void* iobj,int ilen,int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMCheckBox(parent,obj,len,id); }
	void get();
	void set();
};

/////////////////////////////////////////////////////////////////////////////

class PMEntryField : public PMControl {
protected:
public:
	PMEntryField(HWND parentDlg,void* iobj,int ilen,int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMEntryField(parent,obj,len,id); }
	void get();
	void set();
};

class PMMultiLineEdit : public PMControl {
protected:
public:
	PMMultiLineEdit(HWND parentDlg,void* iobj,int ilen,int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMMultiLineEdit(parent,obj,len,id); }
	void get();
	void set();
};

/////////////////////////////////////////////////////////////////////////////

class PMValueSet : public PMControl {
protected:
public:
	PMValueSet(HWND parentDlg,void* iobj,int ilen,int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMValueSet(parent,obj,len,id); }
	MRESULT querySelectedItem() { return sendMsg(VM_QUERYSELECTEDITEM); }
	MRESULT queryItem() { return sendMsg(VM_QUERYITEM); }
	MRESULT selectItem(USHORT usX,USHORT usY) { return sendMsg(VM_SELECTITEM,MPFROM2SHORT( usX, usY)); }
	MRESULT selectItem(MPARAM item) { return sendMsg(VM_SELECTITEM,item); }
	MRESULT setItem(USHORT usX,USHORT usY,MPARAM data) { 
		return sendMsg(VM_SETITEM,MPFROM2SHORT( usX, usY),data); }
	void get();
	void set();
};

/////////////////////////////////////////////////////////////////////////////

class PMSpinButton : public PMControl {
protected:
public:
	PMSpinButton(HWND parentDlg,void* iobj,int ilen,int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMSpinButton(parent,obj,len,id); }

	BOOL queryLimits(ULONG ulUpLimit,ULONG ulLoLimit) {
		return (BOOL) sendMsg(SPBM_QUERYLIMITS,(MPARAM)ulUpLimit,(MPARAM)ulLoLimit); }

	BOOL queryValue(PVOID buf,USHORT size,USHORT value=SPBQ_UPDATEIFVALID) {
		return (BOOL) sendMsg(SPBM_QUERYVALUE,(MPARAM)buf,MPFROM2SHORT(size,value)); }

	BOOL setArray(PCHAR *array,USHORT size) {
		return (BOOL) sendMsg(SPBM_SETARRAY,(MPARAM)array,(MPARAM)size); }

	BOOL setCurrentValue(LONG value) {
		return (BOOL) sendMsg(SPBM_SETCURRENTVALUE,(MPARAM)value); }

	BOOL setLimits(ULONG ulUpLimit,ULONG ulLoLimit) {
		return (BOOL) sendMsg(SPBM_SETLIMITS,(MPARAM)ulUpLimit,(MPARAM)ulLoLimit); }

	BOOL setMaster(HWND master) {
		return (BOOL) sendMsg(SPBM_SETMASTER,(MPARAM)master); }
	
	BOOL setTextLimit(USHORT usCharLimit) {
		return (BOOL) sendMsg(SPBM_SETTEXTLIMIT,(MPARAM)usCharLimit); }

	BOOL spinDown(ULONG ulItems=1) {
		return (BOOL) sendMsg(SPBM_SPINDOWN,(MPARAM)ulItems); }

	BOOL spinUp(ULONG ulItems=1) {
		return (BOOL) sendMsg(SPBM_SPINUP,(MPARAM)ulItems); }

	void get();
	void set();
};

/////////////////////////////////////////////////////////////////////////////

class PMListBox : public PMControl {
protected:
public:
	PMListBox(HWND parentDlg,void* iobj,int ilen,int iid);
	~PMListBox();
	static PMControl* New(HWND parent,void* obj, int len , int id)
		{ return new PMListBox(parent,obj,len,id); }
	void get();
	void set();
	ULONG deleteItem(SHORT index) {
		return (ULONG)WinSendMsg(hwnd,LM_DELETEITEM,MPFROMLONG(index),(MPARAM)NULL);
	}
	BOOL deleteAll() {
		return (BOOL)WinSendMsg(hwnd,LM_DELETEALL,(MPARAM)NULL,(MPARAM)NULL);
	}
	ULONG insertItem(SHORT index,PSZ item) {
		return (ULONG)WinSendMsg(hwnd,LM_INSERTITEM,MPFROMLONG(index),MPFROMP(item));
	}
	ULONG queryCount() {
		return (ULONG)WinSendMsg(hwnd,LM_QUERYITEMCOUNT,(MPARAM)NULL,(MPARAM)NULL);
	}
	ULONG querySelection() {
		return (ULONG)WinSendMsg(hwnd,LM_QUERYSELECTION,MPFROMLONG(LIT_FIRST),(MPARAM)NULL);
	}
	ULONG queryItemText(SHORT index,PSZ buf,SHORT cchMax) {
		return (ULONG)WinSendMsg(hwnd,LM_QUERYITEMTEXT,MPFROM2SHORT((index), (cchMax)), MPFROMP (buf));
	}
	ULONG queryItemTextLenght(SHORT index) {
		return (ULONG)WinSendMsg(hwnd,LM_QUERYITEMTEXTLENGTH,MPFROMLONG(index),(MPARAM)NULL);
	}
};

/////////////////////////////////////////////////////////////////////////////

class PMScrollBar : public PMControl {
protected:
public:
	PMScrollBar(HWND parentDlg, void* iobj, int ilen, int iid);
	PMScrollBar(PMWin* win, int* iobj, int iid);
	static PMControl* New(HWND parent,void* obj, int len, int id) 
		{ return new PMScrollBar(parent,obj,len,id); }
	void get();
	void set();
	SHORT queryPos() {
		return (SHORT)WinSendMsg(hwnd,SBM_QUERYPOS,(MPARAM)NULL,(MPARAM)NULL);
	}
	SHORT queryRangeMin() {
		return SHORT1FROMMR(WinSendMsg(hwnd,SBM_QUERYRANGE,(MPARAM)NULL,(MPARAM)NULL));
	}
	SHORT queryRangeMax() {
		return SHORT2FROMMR(WinSendMsg(hwnd,SBM_QUERYRANGE,(MPARAM)NULL,(MPARAM)NULL));
	}
	BOOL setPos(SHORT sslider) {
		return (BOOL)WinSendMsg(hwnd,SBM_SETPOS,(MPARAM)sslider,(MPARAM)NULL);
	}
	BOOL setScrollBar(SHORT sslider,SHORT sfirst, SHORT slast) {
		return (BOOL)WinSendMsg(hwnd,SBM_SETSCROLLBAR,(MPARAM)sslider,MPFROM2SHORT(sfirst,slast));
	}
	BOOL setThumbSize(SHORT svisible,SHORT stotal) {
		return (BOOL)WinSendMsg(hwnd,SBM_SETTHUMBSIZE,MPFROM2SHORT(svisible,stotal),(MPARAM)NULL);
	}
};

class PMVertScrollBar : public PMScrollBar {
public:
	PMVertScrollBar(PMWin* win, int* iobj);
};

class PMHorizScrollBar : public PMScrollBar {
public:
	PMHorizScrollBar(PMWin* win, int* iobj);
};

/////////////////////////////////////////////////////////////////////////////

#ifndef offsetof
#define offsetof(typ,mbr) ((size_t)((char*)&((typ*)0)->(mbr)-(char*)0))
#endif

struct PMControlMap {
	NEWCTRLFN createfn;
	int id;
	int offset;
	int len;		
	PMControl* ctrl;
};

// macros to fill out the dialog control structure

#define cmCust(fn, id, typ, mbr) \
	{ fn,id,offsetof(typ,mbr),sizeof(((typ *)0)->mbr), NULL },
	
#define cmEntryField(id,typ,mbr) cmCust(PMEntryField::New,id,typ,mbr)
#define cmListBox(id,typ,mbr) cmCust(PMListBox::New,id,typ,mbr)
#define cmButton(id) { PMButton::New, id, 0, 0, NULL }
#define cmRadioButtonGroup(id,typ,mbr) cmCust(PMRadioButtonGroup::New,id,typ,mbr)
#define cmCheckBox(id,typ,mbr) cmCust(PMCheckBox::New,id,typ,mbr)
#define cmValueSet(id,typ,mbr) cmCust(PMValueSet::New,id,typ,mbr)
#define cmSpinButton(id,typ,mbr) cmCust(PMSpinButton::New,id,typ,mbr)
#define cmMultiLineEdit(id,typ,mbr) cmCust(PMMultiLineEdit::New,id,typ,mbr)
#define cmEnd(id) { NULL, id, 0, 0, NULL }

class PMDialog : public PMWin {
protected:
	BOOL dispachEvent(PMEvent & event);
public:
	PMDialog(HWND parent,HWND owner);
	~PMDialog();
	virtual void init();
	BOOL createWin();	
	BOOL destroyWin();	

	virtual BOOL initdlg();
	virtual BOOL control(SHORT id,SHORT ctrlmsg);	
	BOOL command(USHORT id,USHORT cmddev);	 // command ridefinito...
};

class PMModalDialog : public PMDialog {
protected:
	int delctlmap; // TRUE if the ctlmap is deleteable
	ULONG id;
	void* obj;
	PMControlMap* cm;
public:
	PMModalDialog(HWND parent,HWND owner,ULONG id,PMControlMap* cm,void* obj);
	~PMModalDialog();
	void init();
	BOOL createWin();	
	void updateControls();
	void updateObject();
	PMControl* controlFromID(int id); 
	BOOL command(USHORT id,USHORT cmddev);	 // command ridefinito...
};


class PMModelessDialog : public PMModalDialog {
public:
	PMModelessDialog(HWND parent,HWND owner,ULONG id,PMControlMap* cm,void* obj) :
		PMModalDialog(parent,owner,id,cm,obj) { };
	BOOL createWin();	
};

/////////////////////////////////////////////////////////////////////////////

class PMFontDialog : public PMDialog {
protected:
	FONTMETRICS *fm;
	FONTDLG fontdlg;
	friend MRESULT PMFontDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
public:
	PMFontDialog(HWND parent,HWND owner,FONTMETRICS *ifm,ULONG type,PSZ ititle,PSZ ipreview);
	~PMFontDialog();
	operator FONTDLG () { return fontdlg; } // cosi' si puo' accedere ai dati privati
	BOOL createWin();	
};

/////////////////////////////////////////////////////////////////////////////

class PMFileDialog : public PMDialog {
protected:
	FILEDLG filedlg;
	friend MRESULT PMFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
public:
	PMFileDialog(HWND parent,HWND owner,PSZ file,PSZ ititle,PSZ OKButton,ULONG flags);
	~PMFileDialog();
	operator FILEDLG () { return filedlg; } // cosi' si puo' accedere ai dati privati
	BOOL createWin();	
};


#endif

