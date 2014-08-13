/* 


	provadlg.cpp (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/

/*

	This example shows some of the library features:
	
	* building a PMMainWin and automatic file name management
	* using menus
	* using (smart) dialog boxes
	* calling system dialogs (font dialog)
	* calling message boxes
	* printing from a different thread
	* using the help system and a hook procedure for the message boxes help

*/
#include <stdio.h>
#include <string.h>
#include "pmwin.h"
#include "pmdlg.h"
#include "pmgpi.h"
#include "pmhelp.h"
#include "pmstdres.h"
#include "provadlg.h"

PMApp* App;

PMHelpWin *helpwin;

/////////////////////////////////////////////////////////////////////////////
// Must process all and only the help requests in message boxes
//

BOOL helpHook ( HAB habAnchor, SHORT sMode, USHORT usTopic, USHORT usSubTopic, PRECTL prclPos )
{
	if ( (sMode == HLPM_WINDOW ) && 
	     helpwin && 
		 ( (usTopic>=PMHLP_DLG_ERRMSG && usTopic<=PMHLP_DLG_ASSERTFAIL) || usTopic==HLP_DLG_INFO) 
	   ) {
		helpwin->sendMsg(HM_DISPLAY_HELP, 
			MPFROMLONG (MAKELONG (usTopic, 0)), MPFROMSHORT( HM_RESOURCEID) );
		return TRUE;
	} 
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Smart dialog box. When an item from the listbox is selected, the string
// is also inserted in the entry field
//

class NamesDialog : public PMModalDialog {
public:
	NamesDialog(HWND parent,HWND owner,ULONG id,PMControlMap* cm,void* obj) :
		PMModalDialog(parent,owner,id,cm,obj) {};
	BOOL initdlg() {  // dialog box initialization
		char buf[64];
		PMListBox* lb=(PMListBox*)controlFromID(DNAME_LB_NAMELIST);
		lb->insertItem(LIT_END,"Mario");
		lb->insertItem(LIT_END,"Luca");
		lb->insertItem(LIT_END,"Piero");
		lb->insertItem(LIT_END,"Marco");
		lb->insertItem(LIT_END,"Giovanni");
		ab.loadString( STR_PAOLA, 64, buf);	   // load strings from resources
		lb->insertItem( LIT_END, buf);
		ab.loadString( STR_CHIARA, 64, buf);
		lb->insertItem( LIT_END, buf);
		ab.loadString( STR_SILVIA, 64, buf);
		lb->insertItem( LIT_END, buf);
		return TRUE;
	}		
	BOOL control(SHORT id,SHORT ctrlmsg) {
		switch (id) {
		case DNAME_LB_NAMELIST: // control id= the LISTBOX
			switch (ctrlmsg) {
				case LN_SELECT: {
					char buf[64];
					SHORT sIndex;

					PMListBox* ctl=(PMListBox*)controlFromID(DNAME_LB_NAMELIST);

					sIndex=ctl->querySelection();
					ctl->queryItemText(sIndex,buf,64);
					
					controlFromID(DNAME_EF_NAME)->setText(buf); // put string in edit control
  	        		return TRUE;
				}
				case LN_ENTER: // if enter is pressed in the list box, the box quits and accepts.
					postMsg(WM_COMMAND,MPFROMSHORT(DID_OK),0);
					return TRUE;
			}
		}
		return FALSE;
	}		 
};

/////////////////////////////////////////////////////////////////////////////
//	Separate print thread (does not clog the application message queue)
//

class PMPrintThread : public PMWindowThread 
{
public:
	PMPrintThread() : PMWindowThread() {
	}
	void main(void* arg=NULL) {
		PMPrinterDC printer(hab);
		printer.open();
		printer.startDoc("provadlg.exe test document");

		PMPresSpace *ptr=new PMPresSpace(&printer,0,0,PU_LOMETRIC|PU_LOENGLISH|GPIF_DEFAULT|GPIT_NORMAL|GPIA_ASSOC,hab);
		PMPoint points[4]={ PMPoint(0,0), PMPoint(900,0), PMPoint(0,900), PMPoint(900,900) };
    	ptr->move(&points[0]);
    	ptr->line(&points[1]);
    	ptr->line(&points[2]);
    	ptr->line(&points[3]);
    	ptr->line(&points[0]);

		delete ptr;
		printer.endDoc("provadlg.exe test document");
	}
};

/////////////////////////////////////////////////////////////////////////////
// Pippo (this is Disney's Goofy name in Italian) is a PMMainWin with some
// more functions: a client paint function, a menu selection function.

class Pippo : public PMMainWin 
{
	struct pluto {
		int lbsel;
		char nome[64];
	} temp;
public:
	Pippo(HAB ab,PMHelpWin* hw) : PMMainWin("pippo.child",ab,hw) {
		createArgs->flCreateFlags|=FCF_MENU|FCF_ICON|FCF_ACCELTABLE;
		createArgs->idResources=ID_PROVADLG;
		createArgs->pszTitle="Pippo";
		strcpy(temp.nome,"Hello");
		temp.lbsel=1;
		caption="Pippo";
		fnFilter="*.*";
		flCaption="Open Pippo File";
		fsCaption="Save Pippo File";
		fileFlags=PMMWFF_UNTITLED;
	} ;
	BOOL paint(PMPresSpace& ps)
	{
		char buf[100];
		sprintf( buf, "ProvaDlg main window '%s'", temp.nome);
		PMRect rcl;
		rcl=this;
      	ps.setColor (CLR_DARKCYAN);
      	ps.drawText (-1, buf, &rcl, 0, 0, DT_TEXTATTRS | DT_CENTER | DT_VCENTER | DT_ERASERECT);
		return TRUE;
	}
	BOOL command(USHORT id,USHORT cmddev)
	{
		switch (id) {
	        case IDM_CRASH: {
				PMControlMap esempio1cm[] ={
					cmEntryField(D1_NOME,pluto,nome)
					cmEnd(D1_NOME)
				};
				// *must* be HWND_DESKTOP,hwnd for the help system to work!!!
				PMModalDialog mydialog(HWND_DESKTOP,hwnd,DLG_ESEMPIO1,esempio1cm,&temp);
				int ret=mydialog.createWin(); // run dialog
				if (ret) invalidate(TRUE);
				return TRUE;
			}
	        case IDM_NAMES: {
				PMControlMap plutocm[] ={
					cmListBox(DNAME_LB_NAMELIST , pluto,lbsel)
					cmEntryField(DNAME_EF_NAME,pluto,nome)
					cmEnd(DNAME_EF_NAME)
				};
				NamesDialog names(HWND_DESKTOP,hwnd,DLG_NAME,plutocm,&temp);

				int ret=names.createWin(); // run dialog
				if (ret) invalidate(TRUE);
				return TRUE;
			}
    	    case IDM_FONTS:	{
				FONTMETRICS fm;
				PMWindowPresSpace ps(this);
				ps.queryFontMetrics(&fm);
				PMFontDialog fd(HWND_DESKTOP,hwnd, &fm,0,"Titolo","Preview");
				int ret=fd.createWin();
				assert(ret);
				FONTDLG fi=fd;
// call this function to test the HELP system in message boxes
				return WinMessageBox(HWND_DESKTOP,hwnd,"aaa","Result",HLP_DLG_INFO,MB_OK|MB_INFORMATION|MB_HELP);
//				msgBox("Result","%d %s %ld %hd %hd %hd %hd",ret,fi.pszFamilyname,fi.fxPointSize,fi.usWeight,fi.usWidth,fi.x,fi.y);
//				return TRUE;
			}
		}		
		return PMMainWin::command(id,cmddev);
	}
// intercept keyboard (for debug purposes only!)
/*	BOOL kbd(PMEvent& event) {
		char buf[160];
		sprintf(buf,"fs: %u  repeat: %u   scan: %u  chr: %u  vkey: %u",
			event.charmsg->fs,event.charmsg->cRepeat,event.charmsg->scancode,
			event.charmsg->chr,event.charmsg->vkey);
		WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,buf,"KEYBOARDHIT",0,MB_OK);
		return TRUE;
	}*/
// what to do when the user asked to open a file
	BOOL fileOpen(PCSZ filename) {
		msgBox("Result","%s ",filename);
		
		return TRUE;
	}
// what to do when user asked to print the current file
	BOOL filePrint() {
		new PMPrintThread;
		return TRUE;
	}
// react to help messages: tell the keys help, and check for error messages
	BOOL helpmsg(PMEvent &event)
	{
		if (event.msg==HM_QUERY_KEYS_HELP) {
			event.ret=MRFROMSHORT(HLP_KEYS_PROVADLG); 
			return TRUE;
		}
		if (event.msg==HM_HELPSUBITEM_NOT_FOUND) {
            // Don't intercept general help 
            if( SHORT1FROMMP( event.mp1 ) != 1 && SHORT2FROMMP( event.mp2 ) != 2 )
            {
                ErrBox("IPF Help error.  Topic %lx (subtopic %lx) not found for %s.",
                    (ULONG) SHORT1FROMMP( event.mp2 ),
                    (ULONG) SHORT2FROMMP( event.mp2 ),
                    LONGFROMMP( event.mp1 ) == HLPM_WINDOW ? "application window" :
                        (LONGFROMMP( event.mp1 ) == HLPM_FRAME ? "frame window" : "menu window" ) );
            }

            /* For HLPM_WINDOW or HLPM_FRAME return FALSE
                for no action or TRUE for extended help */
            event.ret=(MRESULT) TRUE;
			return TRUE;
		}
		return FALSE;
	}
};


int main (int argc,char* argv[])
{
	PMAnchorBlock ab;  // create A.B.
	PMMessageQueue mq; // create M.Q.
	ab.init();         // initialize
	mq.create(ab);
	
	App=new PMApp(ab,mq,argc,argv);

	// use help file provadlg.hlp, and connect to ID_PROVADLG help resource definition
	helpwin=new PMHelpWin("Prova Dlg Sample Help File","provadlg.hlp",ID_PROVADLG,ab);
	helpwin->createWin();

	// create main window
	Pippo * pip=new Pippo(ab,helpwin);
	pip->createWin();

	// set hook for message windows
	ab.setHook(mq,HK_HELP,(PFN)helpHook,NULLHANDLE);
	
	// run the application main loop
	App->run();

	// reset hook
	ab.releaseHook(mq,HK_HELP,(PFN)helpHook,NULLHANDLE);

	pip->destroyWin();

	delete helpwin;

	mq.destroy();
	ab.uninit();

	return (0);
}

/*
 * Local variables:
 * compile-command: "dmake provadlg.exe"
 * end:
 */
