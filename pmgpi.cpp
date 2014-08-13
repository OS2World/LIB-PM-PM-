/* 


	pmgpi.cpp (emx+gcc) 

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
#include "pmgpi.h"
#include "pmstdres.h"

/////////////////////////////////////////////////////////////////////////////


PMLogFont::PMLogFont(PMPresSpace& ps,PCCH szFacename, LONG lMaxBaselineExt, LONG lAveCharWidth, USHORT usCodePage)
{
	lLcid=ps.lFontIdcount;
	ps.lFontIdcount++;
	fat.usRecordLength = sizeof(FATTRS); /* sets size of structure   */
	fat.fsSelection = 0;         /* uses default selection           */
	fat.lMatch = 0L;             /* does not force match             */
	fat.idRegistry = 0;          /* uses default registry            */
	fat.usCodePage = usCodePage;        
	fat.lMaxBaselineExt = lMaxBaselineExt;   
	fat.lAveCharWidth = lAveCharWidth;     
	fat.fsType = 0;              /* uses default type                */
	fat.fsFontUse = FATTR_FONTUSE_NOMIX;/* doesn't mix with graphics */
 
	strcpy(fat.szFacename ,szFacename);
 
	ps.createLogFont(NULL, lLcid, &fat);      
}

PMLogFont::PMLogFont(PMPresSpace& ps,PFATTRS pfAttrs)
{
	lLcid=ps.lFontIdcount;
	ps.lFontIdcount++;
	fat=*pfAttrs;
	
	ps.createLogFont(NULL, lLcid, &fat);      
}


/////////////////////////////////////////////////////////////////////////////

PMMenu::PMMenu()
{
}

PMMenu::PMMenu(PMWin* iwin)
{
	win=iwin;
	menu=WinWindowFromID(win->getHwnd(),FID_MENU);
}

PMMenu::PMMenu(HWND imenu)
{
	menu=imenu;
	win=NULL;
}

PMSystemMenu::PMSystemMenu(PMWin* iwin)
{
	win=iwin;
	menu=WinWindowFromID(win->getHwnd(),FID_SYSMENU);
}

PMPopupMenu::PMPopupMenu(PMWin* iwin,ULONG idres) 
{
	win=iwin;
	menu=WinLoadMenu(win->getHwnd(),NULLHANDLE,idres);
}

BOOL PMPopupMenu::popup(PMPoint& pt,ULONG sel,ULONG flags) 
{
	return WinPopupMenu(win->getHwnd(),win->getHwnd(),menu,pt.x,pt.y,sel,flags);
}

/////////////////////////////////////////////////////////////////////////////

PMRect& PMRect::operator = (PMWin* w) 
{
	WinQueryWindowRect (w->getHwnd(), this);
	return *this;
}

////////////////////////////////////////////////////////////////////////////


PMPresSpace::PMPresSpace(PMDeviceContext* dc,LONG width,LONG height,LONG flOptions,HAB hab)
{
	lFontIdcount=1L;
	SIZEL sizl;
	sizl.cx = width;
	sizl.cy = height;
	hps = GpiCreatePS(hab,*dc,&sizl,flOptions);
}

PMPresSpace::~PMPresSpace()
{
	GpiAssociate(hps,NULLHANDLE);
	GpiDestroyPS(hps);		
}

BOOL PMPresSpace::queryTextBox (PSZ pszString, LONG lCount2, PPOINTL aptlPoints)
{
	return GpiQueryTextBox ( hps, strlen(pszString), (PCH) pszString , lCount2, aptlPoints);
}

LONG PMPresSpace::queryTextBoxWidth (PSZ pszString)
{
	POINTL aptlPoints[TXTBOX_COUNT];
	GpiQueryTextBox ( hps, strlen(pszString), (PCH) pszString , TXTBOX_COUNT, aptlPoints);
	return aptlPoints[TXTBOX_TOPRIGHT].x;
}

LONG PMPresSpace::queryTextBoxHeight (PSZ pszString)
{
	POINTL aptlPoints[TXTBOX_COUNT];
	GpiQueryTextBox ( hps, strlen(pszString), (PCH) pszString , TXTBOX_COUNT, aptlPoints);
	return aptlPoints[TXTBOX_TOPRIGHT].y;
}


PMWindowPresSpace::PMWindowPresSpace(PMWin* w) : 
	PMPresSpace()
{
   	hps = WinBeginPaint (w->getHwnd(), 0L, 0L);
}

PMWindowPresSpace::~PMWindowPresSpace() 
{ 
	WinEndPaint (hps);
}

PMMicroCachedPresSpace::PMMicroCachedPresSpace(PMWin* w) : 
	PMPresSpace()
{
   	hps = WinGetPS (w->getHwnd());
}

PMMicroCachedPresSpace::~PMMicroCachedPresSpace() 
{ 
	WinReleasePS (hps);
}


/////////////////////////////////////////////////////////////////////////////

PMDeviceContext::PMDeviceContext(HAB ab,HDC ihdc)
{
	hab=ab;
	hdc=ihdc;
	canClose=FALSE;
}

PMDeviceContext::PMDeviceContext(HAB ab,LONG lType, PCSZ pszToken, LONG lCount, PDEVOPENDATA pdopData, HDC hdcComp)
{
	hab=ab;
	hdc=DevOpenDC ( hab,lType,pszToken,lCount,pdopData,hdcComp);
	canClose=TRUE;
}

PMDeviceContext::PMDeviceContext(HWND hwnd)
{
	hab=WinQueryAnchorBlock(hwnd);
	hdc=WinOpenWindowDC(hwnd);
	canClose=FALSE;
}

PMDeviceContext::~PMDeviceContext()
{
	if (hdc!=NULLHANDLE && canClose) DevCloseDC(hdc);
}

HDC PMDeviceContext::open(LONG lType, PCSZ pszToken, LONG lCount, PDEVOPENDATA pdopData, HDC hdcComp)
{
	hdc=DevOpenDC ( hab,lType,pszToken,lCount,pdopData,hdcComp);
	return hdc;
}

/////////////////////////////////////////////////////////////////////////////

PMMemoryDC::PMMemoryDC(HAB hab) :
	PMDeviceContext(hab,NULLHANDLE) { }
	
PMMemoryDC::PMMemoryDC(HAB hab,LONG lCount, HDC hdcComp) :
	PMDeviceContext(hab,OD_MEMORY,"*",lCount,NULL,hdcComp) 
{ 
}

PMMemoryDC::~PMMemoryDC() 
{
}

/////////////////////////////////////////////////////////////////////////////

PMPrinterDC::PMPrinterDC(HAB ab) :
	PMDeviceContext(ab,NULLHANDLE) 
{
	driv.cb=sizeof (DRIVDATA);
}

PMPrinterDC::PMPrinterDC(HAB ab,LONG lCount, PDEVOPENDATA pdopData, HDC hdcComp) :
	PMDeviceContext(ab,OD_QUEUED,"*",lCount,pdopData,hdcComp) 
{
	driv.cb=sizeof (DRIVDATA);
}

PMPrinterDC::~PMPrinterDC()
{
}

BOOL PMPrinterDC::getInformation(PSZ szDefPrnName,PSZ szPrnData,PDEVOPENSTRUC pdop)
{
	char* p;
	PMIniProfile prf(HINI_PROFILE);

	// get def. printer
	prf.queryString("PM_SPOOLER","PRINTER",";",szDefPrnName,33L);

	if ( (p = strchr(szDefPrnName, ';')) != NULL ) *p='\0';
	if ( *szDefPrnName=='\0' ) return FALSE;

	// get def. prn information
	prf.queryString("PM_SPOOLER_PRINTER",szDefPrnName,";;;;",szPrnData,256L);

	if ( (p = strchr(szPrnData, ';')) == NULL )	return FALSE;
	
	pdop->pszDriverName = p + 1;
	
	if ( (p = strchr(pdop->pszDriverName, ';')) == NULL )	return FALSE;

	pdop->pszLogAddress = p + 1;

	*(pdop->pszLogAddress + strcspn (pdop->pszLogAddress, ",;")) = '\0' ;
	*(pdop->pszDriverName + strcspn (pdop->pszDriverName, ",;")) = '\0' ;
	
	return TRUE;	
}

HDC PMPrinterDC::open()
{
	char* p;
	if (!getInformation(achDefPrnName,achPrnData,&dop)) return DEV_ERROR;

	if ((p=strchr(dop.pszDriverName, '.')) != NULL) {
		*p='\0';
		strncpy (driv.szDeviceName, p + 1, sizeof(driv.szDeviceName));
		dop.pdriv=&driv;
	} else 
		dop.pdriv = NULL;
	dop.pszDataType = "PM_Q_STD";

	hdc=DevOpenDC( hab, OD_QUEUED, "*", 4L, (PDEVOPENDATA)&dop, 0L);
	return hdc;
}

LONG PMPrinterDC::startDoc(PSZ szDocName)
{
	return DevEscape(hdc,DEVESC_STARTDOC,strlen(szDocName),szDocName,NULL,NULL);
}

LONG PMPrinterDC::endDoc(PSZ szDocName)
{
	return DevEscape(hdc,DEVESC_ENDDOC,strlen(szDocName),szDocName,NULL,NULL);
}

LONG PMPrinterDC::newFrame()
{
	return DevEscape(hdc,DEVESC_NEWFRAME,0,NULL,NULL,NULL);
}

LONG PMPrinterDC::abortDoc()
{
	return DevEscape(hdc,DEVESC_ABORTDOC,0,NULL,NULL,NULL);
}

