/* 


	bg_paint.cpp (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.

	Parts of this code are:

*/

/*---------------------------------------------------------
   THREADS3.C -- Demonstrates drawing from a second thread
				 (c) 1990, Ziff Communications Co.
				 PC Magazine * Charles Petzold, 2/90
  ---------------------------------------------------------*/

/*

	This example shows some of the library features:
	
	* using the background paint thread 
	* using semaphores

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define INCL_WIN
#include "pmwin.h"
#include "pmdlg.h"
#include "pmgpi.h"
#include "pmstdres.h"
#include "bg_paint.h"

#define CXIMAGE     150
#define CYIMAGE     150
#define SPLINE      100

#define min(a,b) ((a)>(b)? (b) : (a))


PMApp* App;

/////////////////////////////////////////////////////////////////////////////
//
// PMPaintThread: paint thread class: allows to dump lenghty paint
// operations on a secondary thread.
//
//
// What to do to use the class: 
// 1. create the class (you'll want to derive another class and redefine
//	  the paint function).
// 2. when creating the window that uses the class (in the create() func.)
//	  open a DC of the window, get a PS, create the paint thread, and if
//    you want, you may produce a segment on the PS. (openSegment with
//    drawing mode set to DM_RETAIN).
// 3. When WM_PAINT is sent to the window, do default processing (ie erasing
//    the window, then stop drawing in the paint thread, wait for ready flag
//    (readySem posted), and post the triggerSem to trigger drawing.
// 4. When exiting the window, kill the thread, (optionally calling
//    stop and waiting for ready flag).
// 
// note: it is not necessary to wait for the ready flag.
//
class PMPaintThread : public PMWindowThread 
{

	PMPresSpace *pps;	
	BOOL stopCalled;
	
public:
	PMEventSemaphore *readySem,*triggerSem;

	PMPaintThread(PMPresSpace *p) : PMWindowThread() 
	{
		readySem = new PMEventSemaphore;
		triggerSem = new PMEventSemaphore;
		pps=p;
	}
	void main(void* =NULL) {
		while (TRUE) {
			readySem->post();		   // we're ready...
			triggerSem->wait();		   // wait for trigger

			pps->setStopDraw(SDW_OFF); // default setting, do not stop drawing
			stopCalled=FALSE;

			readySem->reset();		   // we're not ready...
			triggerSem->reset();       // deactivate trigger...
			paint();
		}
	}
	PMPresSpace *ps() { return pps; }
	void stop() 	   
	// try stop drawing, by calling GpiSetStopDraw (if drawing from a 
	// segment) or by checking a flag in the paint function.
	{
		stopCalled=TRUE;
		pps->setStopDraw(SDW_ON);
	}
protected:
	void paint() 
	{
		pps->save();
		pps->drawSegment(1L);
		pps->restore(-1L);
		for (int j=0; j<8; j++) {
			for (int i=0; i<200 && !stopCalled; i++) {
				PMRect rcl(j*100,0,(j+1)*100,i);
				pps->fillRect(&rcl,CLR_BLUE+j);
			}
		}
	}	

};

class BG_PaintWin : public PMWin 
{
	PMPaintThread *dthread;
	PMDeviceContext *dc;
	PMPresSpace *ps;
	ULONG prtclass;
	LONG delta;
public:
	BG_PaintWin(HAB ab,ULONG pc,LONG d) : PMWin("bgpaintwin",ab) {
		createArgs->flCreateFlags|=FCF_ICON;
		createArgs->idResources=ID_BGPAINT;
		createArgs->pszTitle="bgpaint";
		delta=d;
		prtclass=pc;
	} ;
	BOOL create(PMEvent &)
	{
		if (!dthread) {
			dc= new PMDeviceContext( hwnd );
	        ps = new PMPresSpace(dc, 0, 0, PU_LOENGLISH | GPIF_DEFAULT | GPIT_NORMAL | GPIA_ASSOC, ab) ;
			dthread = new PMPaintThread(ps);

			dthread->setPriority(PRTYS_THREAD,prtclass,delta);
			// produce a simple segment to show how it is done
			// segments are collections of Gpi commands which are not
			// executed directly but are kept in the PS. When the 
			// drawSegment command is invoked the whole segment is
			// drawn in one pass. It is possible to stop the lenghty 
			// drawing operation with setStopDraw.
	        ps->setDrawingMode ( DM_RETAIN) ;
    	    ps->openSegment (1L) ;
	        drawImage () ;
	        ps->closeSegment () ;
		}
		return TRUE;
	}

	// parts of this code was taken from an example by Charles Petzold, quote:
/*---------------------------------------------------------
   THREADS3.C -- Demonstrates drawing from a second thread
				 (c) 1990, Ziff Communications Co.
				 PC Magazine * Charles Petzold, 2/90
  ---------------------------------------------------------*/
	
	BOOL size(SHORT sx,SHORT sy)
	{
		dthread->stop();
		dthread->readySem->wait();
		SIZEL sizlClient ;
		// get new size of client window

		sizlClient.cx = sx ;
		sizlClient.cy = sy ;

		// set the new default view matrix

		GpiConvert (*ps, CVTC_DEVICE, CVTC_PAGE, 1L, (PPOINTL) &sizlClient);
		FIXED    afxScale[2] ;
		MATRIXLF matlf ;
		POINTL   ptl ;

		afxScale[0] = afxScale[1] = min (65536L * sizlClient.cx / CXIMAGE,
									  65536L * sizlClient.cy / CYIMAGE) ;
		ptl.x = 0 ;
		ptl.y = 0 ;

		GpiScale (*ps, &matlf, TRANSFORM_REPLACE, afxScale, &ptl) ;

		ptl.x = (sizlClient.cx - afxScale[0] *CXIMAGE / 65536L) / 2 ;
		ptl.y = (sizlClient.cy - afxScale[1] * CYIMAGE / 65536L) / 2 ;

		GpiTranslate (*ps, &matlf, TRANSFORM_ADD, &ptl) ;

		GpiSetDefaultViewMatrix (*ps, 9L, &matlf, TRANSFORM_REPLACE) ;
		return TRUE;
	}
	BOOL destroy()
	{
		if (dthread) {
			dthread->stop();
			dthread->kill();
			delete dthread;
			delete ps;
		}
		return TRUE;
	}
	BOOL paint()
	{
		PMRect rcl;
		dthread->stop();
		dthread->readySem->wait();
        WinBeginPaint (hwnd, *ps, NULL) ;
		rcl=this;
        GpiErase (*ps) ;
		ps->fillRect(&rcl,CLR_BLUE);
        WinEndPaint (*ps) ;
		dthread->triggerSem->post();
		return TRUE;
	}
	BOOL other(PMEvent &event)
	{
		if (event.msg == WM_ERASEBACKGROUND) {
			event.ret=(MRESULT)TRUE;
			return TRUE;
		}
		return FALSE;
	}

	void drawImage() 
	{
//		PMRect rcl;
//		rcl=this;
//      	ps->setColor (CLR_DARKCYAN);
//		ps->fillRect(&rcl,CLR_WHITE);
//      	ps->drawText (-1, "Background Paint main window ", &rcl, 0, 0, DT_TEXTATTRS | DT_CENTER | DT_VCENTER | DT_ERASERECT);
	 MATRIXLF matlf ;
	 POINTL   ptl, aptl[4] ;
	 SHORT    iAngle ;

			   // Draw outline of letters

	 drawLetters () ;

			   // Use letters outline as clipping path

	 GpiBeginPath (*ps, 1L) ;
	 drawLetters () ;
	 GpiEndPath (*ps) ;
	 GpiSetClipPath (*ps, 1L, SCP_AND | SCP_ALTERNATE) ;

	 for (iAngle = 0 ; iAngle < 360 ; iAngle++)
		  {
					// Find matrix for rotation around origin

		  ptl.x = 0 ;
		  ptl.y = 0 ;
		  GpiRotate (*ps, &matlf, TRANSFORM_REPLACE,
					 MAKEFIXED (iAngle, 0), &ptl) ;

					// Append matrix for translation to center of image

		  ptl.x = CXIMAGE / 2 ;
		  ptl.y = CYIMAGE / 2 ;
		  GpiTranslate (*ps, &matlf, TRANSFORM_ADD, &ptl) ;

					// Set model transform using composite matrix

		  GpiSetModelTransformMatrix (*ps, 9L, &matlf, TRANSFORM_REPLACE) ;

					// Draw spline curve

		  aptl[0].x = 0 ;
		  aptl[0].y = 0 ;

		  aptl[1].x = SPLINE / 3 ;
		  aptl[1].y = SPLINE / 2 ;

		  aptl[2].x = 2 * SPLINE / 3 ;
		  aptl[2].y = - SPLINE / 2 ;

		  aptl[3].x = SPLINE ;
		  aptl[3].y = 0 ;

		  GpiMove (*ps, aptl) ;
		  GpiPolySpline (*ps, 3L, aptl + 1) ;
		  }
	}
	void drawLetters ()
	 {
	 POINTL ptl, aptl[3] ;

					// Outside of 'P'

	 ptl.x     =   4 ;  ptl.y     =   4 ;  GpiMove (*ps, &ptl) ;
	 ptl.x     =   4 ;  ptl.y     = 146 ;  GpiLine (*ps, &ptl) ;
	 ptl.x     =  45 ;  ptl.y     = 146 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x =  72 ;  aptl[0].y = 146 ;
	 aptl[1].x =  72 ;  aptl[1].y = 119 ;  GpiPolyFillet (*ps, 2L, aptl) ;
	 ptl.x     =  72 ;  ptl.y     =  85 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x =  72 ;  aptl[0].y =  58 ;
	 aptl[1].x =  45 ;  aptl[1].y =  58 ;  GpiPolyFillet (*ps, 2L, aptl) ;
	 ptl.x     =  35 ;  ptl.y     =  58 ;  GpiLine (*ps, &ptl) ;
	 ptl.x     =  35 ;  ptl.y     =   4 ;  GpiLine (*ps, &ptl) ;
	 ptl.x     =   4 ;  ptl.y     =   4 ;  GpiLine (*ps, &ptl) ;

					// Inside of 'P'

	 ptl.x     =  35 ;  ptl.y     =  77 ;  GpiMove (*ps, &ptl) ;
	 ptl.x     =  35 ;  ptl.y     = 125 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x =  43 ;  aptl[0].y = 125 ;
	 aptl[1].x =  43 ;  aptl[1].y = 117 ;  GpiPolyFillet (*ps, 2L, aptl) ;
	 ptl.x     =  43 ;  ptl.y     =  85 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x =  43 ;  aptl[0].y =  77 ;
	 aptl[1].x =  35 ;  aptl[1].y =  77 ;  GpiPolyFillet (*ps, 2L, aptl) ;

					// Outline of 'C'

	 ptl.x     = 116 ;  ptl.y     =  63 ;  GpiMove (*ps, &ptl) ;
	 ptl.x     = 145 ;  ptl.y     =  63 ;  GpiLine (*ps, &ptl) ;
	 ptl.x     = 145 ;  ptl.y     =  31 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x = 145 ;  aptl[0].y =   4 ;
	 aptl[1].x = 118 ;  aptl[1].y =   4 ;  GpiPolyFillet (*ps, 2L, aptl) ;
	 ptl.x     = 103 ;  ptl.y     =   4 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x =  76 ;  aptl[0].y =   4 ;
	 aptl[1].x =  76 ;  aptl[1].y =  31 ;  GpiPolyFillet (*ps, 2L, aptl) ;
	 ptl.x     =  76 ;  ptl.y     = 119 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x =  76 ;  aptl[0].y = 146 ;
	 aptl[1].x = 103 ;  aptl[1].y = 146 ;  GpiPolyFillet (*ps, 2L, aptl) ;
	 ptl.x     = 118 ;  ptl.y     = 146 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x = 145 ;  aptl[0].y = 146 ;
	 aptl[1].x = 145 ;  aptl[1].y = 119 ;  GpiPolyFillet (*ps, 2L, aptl) ;
	 ptl.x     = 145 ;  ptl.y     =  88 ;  GpiLine (*ps, &ptl) ;
	 ptl.x     = 116 ;  ptl.y     =  88 ;  GpiLine (*ps, &ptl) ;
	 ptl.x     = 116 ;  ptl.y     = 123 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x = 116 ;  aptl[0].y = 127 ;
	 aptl[1].x = 108 ;  aptl[1].y = 127 ;
	 aptl[2].x = 108 ;  aptl[2].y = 123 ;  GpiPolyFillet (*ps, 3L, aptl) ;
	 ptl.x     = 108 ;  ptl.y     =  30 ;  GpiLine (*ps, &ptl) ;
	 aptl[0].x = 108 ;  aptl[0].y =  26 ;
	 aptl[1].x = 116 ;  aptl[1].y =  26 ;
	 aptl[2].x = 116 ;  aptl[2].y =  30 ;  GpiPolyFillet (*ps, 3L, aptl) ;
	 ptl.x     = 116 ;  ptl.y     =  63 ;  GpiLine (*ps, &ptl) ;
	 }
};


int main (int argc,char* argv[])
{
	ULONG pc;
	LONG d;

	if (argc>1) {
		if (argc!=3 || strcmp(argv[1],"-?")==0) {
			fprintf(stderr,"bg_paint <priority class 1..4> <delta level -31..31>\n");
			exit(0);
		} else {
			pc=atol(argv[1]);
			d=atol(argv[2]);
		}				
	} else {
		pc=1;
		d=16;
	}

	PMAnchorBlock ab;
	PMMessageQueue mq;
	ab.init();
	mq.create(ab);


	App=new PMApp(ab,mq,argc,argv);

	BG_PaintWin * bgpw=new BG_PaintWin(ab,pc,d);
	bgpw->createWin();

	App->run();

	bgpw->destroyWin();

	mq.destroy();
	ab.uninit();

	return (0);
}

/*
 * Local variables:
 * compile-command: "dmake bg_paint.exe"
 * end:
 */
