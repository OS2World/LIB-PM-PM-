/* 


	pmsys.h (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#ifndef _PMSYS_H_
#define _PMSYS_H_

#define INCL_DOSPROCESS
#define INCL_DOSSEMAPHORES
#define INCL_WIN
#define OS2EMX_PLAIN_CHAR
#include <os2.h>

/////////////////////////////////////////////////////////////////////////////
// PMAnchorBlock ancapsulates a HAB and some useful functions
//
//

class PMMessageQueue;


class PMAnchorBlock {
protected:
	HAB hab;
public:
	PMAnchorBlock();
	PMAnchorBlock(HAB ab) { hab=ab; }
	virtual ~PMAnchorBlock();
	virtual void init(int flags=0);
	virtual void uninit(void);
	operator HAB () { return hab; }

	MRESULT dispachMsg(PQMSG pqmsg) {
		return WinDispatchMsg (hab, pqmsg);
	}
	BOOL getMsg(PQMSG pqmsg, HWND hwndFilter=0L, ULONG msgFilterFirst=0, ULONG msgFilterLast=0) {
		return WinGetMsg (hab,pqmsg,hwndFilter,msgFilterFirst,msgFilterLast);
	}
	BOOL peekMsg(PQMSG pqmsg, HWND hwndFilter=0L, ULONG msgFilterFirst=0, ULONG msgFilterLast=0, ULONG fl=0) {
		return WinPeekMsg (hab,pqmsg,hwndFilter,msgFilterFirst,msgFilterLast,fl);
	}

	POINTL msgPos() { 
		POINTL point;
		WinQueryMsgPos(hab,&point);
		return point;
	}
	ULONG msgTime() {
		return WinQueryMsgTime(hab);
	}

	LONG loadMessage(ULONG id,LONG cchMax,PSZ pchBuffer,HMODULE hmod=NULLHANDLE)
		{ return WinLoadMessage(hab,hmod,id,cchMax,pchBuffer); }
	LONG loadString(ULONG id,LONG cchMax,PSZ pchBuffer,HMODULE hmod=NULLHANDLE)
		{ return WinLoadString(hab,hmod,id,cchMax,pchBuffer); }
	BOOL intersectRect(PRECTL prclDst, RECTL *prclSrc1, RECTL *prclSrc2) 
		{ return WinIntersectRect (hab,prclDst,prclSrc1,prclSrc2); }
	BOOL ptInRect(RECTL *prcl, POINTL *pptl)
		{ return WinPtInRect (hab,prcl,pptl); }
	BOOL unionRect(PRECTL prclDst,RECTL *prclSrc1, RECTL *prclSrc2)
		{ return WinUnionRect (hab,prclDst,prclSrc1,prclSrc2); }

	BOOL releaseHook (HMQ hmq, LONG iHook, PFN pfnHook, HMODULE hmod)
		{ return WinReleaseHook (hab, hmq, iHook, pfnHook, hmod); }
	BOOL setHook (HMQ hmq, LONG iHook, PFN pfnHook, HMODULE hmod) 
		{ return WinSetHook (hab, hmq, iHook, pfnHook, hmod); }
	
};

//
//
//

class PMMessageQueue {
protected:
	HMQ hmq;
public:
	PMMessageQueue();
	operator HMQ () { return hmq; }
	void create(PMAnchorBlock ab) 
		{ hmq = WinCreateMsgQueue (ab, 0); }
	BOOL destroy(void) 
		{ return WinDestroyMsgQueue(hmq); }
};


/////////////////////////////////////////////////////////////////////////////

class PMApp {
	int argc;      // conserva anche argomenti | store command line args
	char** argv;   // della linea di comando   |
	PMAnchorBlock ab;
	PMMessageQueue mq;
public:
	PMApp(PMAnchorBlock iab,PMMessageQueue imq,int iargc=0,char** iargv=NULL);
	virtual void quit(int ret=0);	
	virtual void run(void);
};

/////////////////////////////////////////////////////////////////////////////
// PMIniProfile (profile access file)
//

class PMIniProfile {
protected:
	HINI hini;
public:
	PMIniProfile(HAB ab,PCSZ filename) {
		hini=PrfOpenProfile(ab,filename);
	}
	PMIniProfile(HINI ihini) {
		hini=ihini;
	}
	virtual ~PMIniProfile()	{
		PrfCloseProfile(hini);
	}
	operator HINI () { return hini; }
	LONG queryInt(PCSZ pszApp, PCSZ pszKey, LONG sDefault) {
		return PrfQueryProfileInt (hini,pszApp, pszKey, sDefault);}
	BOOL queryProfile(HAB ab,PPRFPROFILE pPrfProfile) {
		return PrfQueryProfile (ab,pPrfProfile);}
	BOOL queryData (PCSZ pszApp, PCSZ pszKey, PVOID pBuffer, PULONG pulBufferLength) {
		return PrfQueryProfileData (hini, pszApp, pszKey, pBuffer, pulBufferLength); }
	BOOL querySize (PCSZ pszApp, PCSZ pszKey, PULONG pulReqLen) {
		return PrfQueryProfileSize (hini, pszApp, pszKey, pulReqLen); }
	ULONG queryString (PCSZ pszApp, PCSZ pszKey,PCSZ pszDefault, PVOID pBuffer, ULONG ulBufferLength)	{	
		return PrfQueryProfileString (hini, pszApp, pszKey,pszDefault, pBuffer, ulBufferLength);	}
	BOOL reset (HAB ab,PRFPROFILE *pPrfProfile) {
		return PrfReset (ab,pPrfProfile); }
	BOOL writeData (PCSZ pszApp, PCSZ pszKey, CPVOID pData, ULONG ulDataLength) {
		return PrfWriteProfileData (hini, pszApp, pszKey, pData, ulDataLength); }
	BOOL writeString (PCSZ pszApp, PCSZ pszKey, PCSZ pszData) {
		return PrfWriteProfileString ( hini, pszApp, pszKey, pszData); }
};

class PMSystemProfile : public PMIniProfile {
public:
	PMSystemProfile() :	PMIniProfile(HINI_SYSTEM) {	}
	~PMSystemProfile() { } // do not close the system profile
};

class PMUserProfile : public PMIniProfile {
public:
	PMUserProfile() : PMIniProfile(HINI_USER) { }
	~PMUserProfile() { } // do not close the user profile
};

/////////////////////////////////////////////////////////////////////////////
// PMThread class.
// control starts in the main function. All thread member functions can
// use data stored in the thread class. (useful for thread instance data)

class PMThread {
protected:
	void* arg;
	int stacksize;
	unsigned long id;
public:
	PMThread(void* iarg=NULL,int istacksize=8192);
	virtual ~PMThread();
	virtual void main(void* arg=NULL);
	friend void threadstart(void* arg);

	unsigned long ID() { return id; }
	unsigned failed() { return id==(unsigned long)-1; }

	void threadStore(void *p);
	void* threadStore();
	ULONG kill() {
		return DosKillThread (id);
	}
	ULONG resume() {
		return DosResumeThread (id);
	}
	ULONG setPriority(ULONG ulScope, ULONG ulClass, LONG lDelta) {
		return DosSetPriority (ulScope,ulClass,lDelta,id);
	}		
	ULONG suspend() {
		return DosSuspendThread (id);
	}
	ULONG wait(ULONG ulWait) {
		return DosWaitThread (&id, ulWait);
	}
};

class PMWindowThread : public PMThread, public PMAnchorBlock {
protected:
	int flags;
public:
	PMWindowThread(void* iarg=NULL,int istacksize=32768,int iflags=0);
	~PMWindowThread();
	void main(void* arg=NULL);
};

/////////////////////////////////////////////////////////////////////////////
//
// Event Semaphore Class
// 12/19/1995
//

class PMEventSemaphore {
	PCSZ pszName;
	ULONG ulAttr;
	HEV hev;
	ULONG ulCount;
public:
	ULONG rc;
	// create semaphore
	PMEventSemaphore(ULONG iulAttr=DC_SEM_SHARED,BOOL32 fState=FALSE);
	PMEventSemaphore(PCSZ ipszName,ULONG iulAttr=DC_SEM_SHARED,BOOL32 fState=FALSE);
	// open semaphore
	PMEventSemaphore(PCSZ ipszName, PHEV phev); 
	// close semaphore
	~PMEventSemaphore();
	operator HEV () { return hev; }
	// other functions
	ULONG post() {
		return rc = DosPostEventSem (hev);
	}
	ULONG query() {
		rc = DosQueryEventSem (hev, &ulCount);
		return ulCount;
	}
	ULONG reset() {
		rc = DosResetEventSem (hev, &ulCount);
		return ulCount;
	}
	ULONG wait(ULONG ulTimeout=(ULONG)SEM_INDEFINITE_WAIT) {
		return rc = DosWaitEventSem (hev, ulTimeout);
	}
	ULONG count() {
		return ulCount;
	}
};

#endif

