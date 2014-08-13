/* 


	pmgpi.h (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#ifndef _PMGPI_H_
#define _PMGPI_H_

#include "pmsys.h"

class PMWin;
class PMPoint;
class PMRect;
class PMPresSpace;
class PMDevContext;

struct PMPoint : public POINTL {
	PMPoint() { };
	PMPoint(LONG ix,LONG iy=0) { x=ix; y=iy; }
	
	PMPoint& operator += (PMPoint& a) { x+=a.x; y+=a.y; return *this; }
	PMPoint& operator -= (PMPoint& a) { x-=a.x; y-=a.y; return *this; }
	PMPoint& operator = (POINTS a) { x=(LONG)a.x; y=(LONG)a.y; return *this; }
};

inline PMPoint operator + (PMPoint& a,PMPoint& b) { 
	PMPoint c; 
	c.x=a.x+b.x; c.y=a.y+b.y; 
	return c; 
}

inline PMPoint operator - (PMPoint& a,PMPoint& b) { 
	PMPoint c; 
	c.x=a.x-b.x; c.y=a.y-b.y; 
	return c; 
}

inline PMPoint operator * (PMPoint& a,int b) { 
	PMPoint c; 
	c.x=a.x*b; c.y=a.y*b; 
	return c; 
}

inline PMPoint operator / (PMPoint& a,int b) { 
	PMPoint c; 
	c.x=a.x/b; c.y=a.y/b; 
	return c; 
}

struct PMRect : public RECTL {
	PMRect(LONG x1, LONG y1, LONG x2, LONG y2) { 
		xLeft=x1; yBottom=y1; xRight=x2; yTop=y2;
	}
	PMRect(PMPoint &p1, PMPoint &p2) { 
		xLeft=p1.x; yBottom=p1.y; xRight=p2.x; yTop=p2.y;
	}
	PMRect(POINTL &p1, POINTL &p2) { 
		xLeft=p1.x; yBottom=p1.y; xRight=p2.x; yTop=p2.y;
	}
	PMRect() { };
	PMRect& operator = ( PMWin* );
	PMRect& operator += (PMPoint& a) { 
		xLeft+=a.x; xRight+=a.x;
		yBottom+=a.y; yTop+=a.y;
		return *this; 
	}
	PMRect& operator -= (PMPoint& a) { 
		xLeft-=a.x; xRight-=a.x;
		yBottom-=a.y; yTop-=a.y;
		return *this; 
	}
};

/////////////////////////////////////////////////////////////////////////////

class PMPointer {
	HPOINTER ptr;
public:
	PMPointer(ULONG idres) {
		ptr=WinLoadPointer (HWND_DESKTOP,NULLHANDLE,idres);
	}
	~PMPointer() {
		WinDestroyPointer(ptr);
	}
	void set() {
		WinSetPointer(HWND_DESKTOP,ptr);
	}
	void reset() { // reset system pointer
		WinSetPointer(HWND_DESKTOP,
			WinQuerySysPointer (HWND_DESKTOP, SPTR_ARROW, FALSE));
	}
};

/////////////////////////////////////////////////////////////////////////////
// logical fonts

class PMLogFont {
protected:
	FATTRS fat;
	LONG lLcid;
public:
	friend PMPresSpace;

	PMLogFont(PMPresSpace& ps,PCCH szFacename, LONG lMaxBaselineExt, LONG lAveCharWidth, USHORT usCodePage);
	PMLogFont(PMPresSpace& ps,PFATTRS pfAttrs);
};


/////////////////////////////////////////////////////////////////////////////

class PMDeviceContext {
protected:
	HDC hdc;
	HAB hab;
	BOOL canClose; // if FALSE the destructor does not close the DC.
public:
	PMDeviceContext(HAB ab,HDC ihdc); // you can also open the DC later, specifing NULLHANDLE here 
	PMDeviceContext(HAB ab,LONG lType, PCSZ pszToken="*", LONG lCount=0, PDEVOPENDATA pdopData=NULL, HDC hdcComp=NULLHANDLE); // oppure fai tutto subito.
	PMDeviceContext(HWND hwnd);
	virtual ~PMDeviceContext();
	HDC open(LONG lType, PCSZ pszToken="*", LONG lCount=0, PDEVOPENDATA pdopData=NULL, HDC hdcComp=NULLHANDLE);
	HMF close() { return DevCloseDC ( hdc); }
	LONG escape(LONG lCode, LONG lInCount, PBYTE pbInData, PLONG plOutCount=NULL, PBYTE pbOutData=NULL) {
		return DevEscape (hdc, lCode,lInCount, pbInData,plOutCount, pbOutData);
	}	
	BOOL queryCaps(LONG lStart, LONG lCount, PLONG alArray) {
		return DevQueryCaps (hdc,lStart,lCount,alArray);
	}
	operator HDC () { return hdc; }
};

class PMMemoryDC : public PMDeviceContext {
protected:
public:
	PMMemoryDC(HAB ab);  // as above
	PMMemoryDC(HAB ab,LONG lCount=0, HDC hdcComp=NULLHANDLE);
	~PMMemoryDC();
};

class PMPrinterDC : public PMDeviceContext {
protected:
	DEVOPENSTRUC dop;
	CHAR achPrnData[256],achDefPrnName[34];
	DRIVDATA driv;
public:
	PMPrinterDC(HAB ab); // as above
	PMPrinterDC(HAB ab,LONG lCount, PDEVOPENDATA pdopData, HDC hdcComp);
	~PMPrinterDC();
	HDC open();
	LONG startDoc(PSZ);
	LONG endDoc(PSZ);
	LONG newFrame();
	LONG abortDoc();
	BOOL getInformation(PSZ szDefPrnName,PSZ szPrnData,PDEVOPENSTRUC pdop);
};

class PMPresSpace {
protected:
	HPS hps;
public:
	LONG lFontIdcount;
public:
	PMPresSpace() { lFontIdcount=1L; }
	PMPresSpace(PMDeviceContext*,LONG width,LONG height,LONG flOptions,HAB ab);
	virtual ~PMPresSpace();
	operator HPS () { return hps; }

	BOOL setColor(LONG lColor) 
		{ 	return GpiSetColor(hps,lColor); }
	BOOL setLineType(LONG lLineType) 
		{ 	return GpiSetLineType (hps, lLineType);	}
	BOOL setDrawingMode (LONG lMode)
		{	return GpiSetDrawingMode ( hps, lMode); }
	BOOL openSegment (LONG lSegment) 
		{	return GpiOpenSegment ( hps, lSegment); }
	BOOL closeSegment () 
		{	return GpiCloseSegment ( hps); }
	BOOL drawSegment ( LONG lSegment)
		{	return GpiDrawSegment ( hps, lSegment); }
	BOOL setStopDraw (LONG lValue)
		{	return GpiSetStopDraw ( hps, lValue); }

	BOOL restore ( LONG lPSid)
		{	return GpiRestorePS ( hps, lPSid); }
	LONG save ()
		{	return GpiSavePS ( hps); }

	LONG drawText(LONG cchText, PCCH lpchText, PRECTL prcl, LONG clrFore, LONG clrBack, ULONG flCmd)
		{	return WinDrawText(hps,cchText,lpchText,prcl,clrFore,clrBack,flCmd); }
	BOOL fillRect(PRECTL rect, LONG lColor) 
		{	return WinFillRect(hps, rect, lColor); }
	LONG line(PPOINTL point) 
		{	return GpiLine (hps, point); }
	LONG move(PPOINTL point) 
		{	return GpiMove (hps, point); }
	BOOL setMix(LONG lMixMode) 
		{	return GpiSetMix (hps, lMixMode); }
	LONG box(LONG lControl, PPOINTL pptlPoint, LONG lHRound=0L, LONG lVRound=0L)
		{ 	return GpiBox (hps, lControl, pptlPoint, lHRound, lVRound); }

	LONG intersectClipRectangle (__const__ RECTL *prclRectangle) 
		{	return GpiIntersectClipRectangle (hps, prclRectangle); }

	HDC queryDevice() 
		{ 	return GpiQueryDevice (hps); }
	BOOL queryCharBox(PSIZEF psizfxSize) 
		{ 	return GpiQueryCharBox (hps, psizfxSize); }
	BOOL queryFontMetrics(PFONTMETRICS pfmMetrics) 
		{ 	return GpiQueryFontMetrics (hps, sizeof(FONTMETRICS), pfmMetrics); }
	BOOL queryPageViewport (PRECTL prclViewport)
		{ 	return GpiQueryPageViewport (hps,prclViewport);	}
	BOOL queryTextBox (LONG lCount1, PCH pchString, LONG lCount2, PPOINTL aptlPoints)
		{ 	return GpiQueryTextBox (hps, lCount1, pchString, lCount2, aptlPoints); }
	BOOL queryTextBox (PSZ pszString, LONG lCount2, PPOINTL aptlPoints);
	LONG queryTextBoxWidth (PSZ pszString);
	LONG queryTextBoxHeight (PSZ pszString);

	BOOL set(PSIZEL psizlsize,ULONG flOptions)
		{	return GpiSetPS(hps,psizlsize,flOptions); }
	BOOL setPageViewport (__const__ RECTL *prclViewport)
		{	return GpiSetPageViewport (hps, prclViewport); }

	BOOL setCharSet(LONG llcid) 
		{	return GpiSetCharSet (hps, llcid); }
	BOOL setCharAngle (__const__ GRADIENTL *pgradlAngle)
		{ 	return GpiSetCharAngle (hps,pgradlAngle); }
	BOOL setCharBox (__const__ SIZEF *psizfxBox) 
		{	return GpiSetCharBox (hps,psizfxBox); }
	BOOL setCharBreakExtra (FIXED BreakExtra)
		{	return GpiSetCharBreakExtra (hps,BreakExtra); }
	BOOL setCharDirection (LONG lDirection)
		{	return GpiSetCharDirection (hps, lDirection); }
	BOOL setCharExtra (FIXED Extra)
		{	return GpiSetCharExtra (hps, Extra); }
	BOOL setCharMode (LONG lMode) 
		{	return GpiSetCharMode (hps, lMode); }
	BOOL setCharSet(PMLogFont& lf) 
		{	return GpiSetCharSet (hps, lf.lLcid); }
	LONG charString (LONG lCount, PCCH pchString)
		{	return GpiCharString (hps, lCount, pchString); }
	LONG charStringAt (__const__ POINTL *pptlPoint, LONG lCount, PCCH pchString)
		{	return GpiCharStringAt (hps, pptlPoint, lCount, pchString); }
	LONG createLogFont (__const__ STR8 *pName, LONG lLcid, __const__ FATTRS *pfatAttrs)
		{	return GpiCreateLogFont (hps, pName, lLcid, pfatAttrs); }
};

class PMWindowPresSpace : public PMPresSpace {
public:
	PMWindowPresSpace(PMWin*);
	~PMWindowPresSpace();
};

class PMMicroCachedPresSpace : public PMPresSpace {
public:
	PMMicroCachedPresSpace(PMWin*);
	~PMMicroCachedPresSpace();
};

#endif

