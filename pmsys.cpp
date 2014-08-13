/* 


	pmsys.cpp (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#include <stdlib.h>

#include "pmsys.h"

/////////////////////////////////////////////////////////////////////////////

PMMessageQueue::PMMessageQueue()
{
	hmq=0;
}

PMAnchorBlock::PMAnchorBlock()
{
	hab=0;
}

PMAnchorBlock::~PMAnchorBlock()
{
}

void PMAnchorBlock::init(int flags)
{
	hab = WinInitialize(flags);
}

void PMAnchorBlock::uninit(void)
{
	WinTerminate(hab);
}


PMApp::PMApp(PMAnchorBlock iab,PMMessageQueue imq,int iargc,char** iargv)
{
	ab=iab; 
	mq=imq;
	argc=iargc;
	argv=iargv;
}

void PMApp::quit(int )
{
}

void PMApp::run(void)
{
	QMSG qmsg;
	while ( ab.getMsg( &qmsg ) ) 
		ab.dispachMsg ( &qmsg );
}

/////////////////////////////////////////////////////////////////////////////

PMThread::PMThread(void* iarg,int istacksize)
{
	arg=iarg;
	stacksize=istacksize;
	id=_beginthread(threadstart,NULL,stacksize,(void*)this);
}

PMThread::~PMThread()
{
}

void threadstart(void* arg)
{
	PMThread* pmt=(PMThread*)arg;
	pmt->threadStore(NULL);        // annulla il puntatore

	pmt->main(pmt->arg);

	delete pmt->threadStore();     // elimina la zona di memoria del thread
	delete pmt;
	_endthread();
}

void PMThread::threadStore(void *p)
{
	(*_threadstore())=p;	
}

void* PMThread::threadStore()
{
	return *_threadstore();
}

void PMThread::main(void* )
{
}

PMWindowThread::PMWindowThread(void* iarg,int istacksize,int iflags) : 
	PMThread(iarg,istacksize), PMAnchorBlock() 
{
	flags=iflags;
	init(flags);
}

PMWindowThread::~PMWindowThread()
{
}

void PMWindowThread::main(void* )
{
}

/////////////////////////////////////////////////////////////////////////////

PMEventSemaphore::PMEventSemaphore(ULONG iulAttr,BOOL32 fState)
{
	ulAttr=iulAttr;
	pszName=NULL;
	ulCount=0;
	rc=DosCreateEventSem(NULL,&hev,ulAttr,fState);
}

PMEventSemaphore::PMEventSemaphore(PCSZ ipszName,ULONG iulAttr,BOOL32 fState)
{
	ulAttr=iulAttr;
	pszName=ipszName;
	ulCount=0;
	rc=DosCreateEventSem(pszName,&hev,ulAttr,fState);
}

// open semaphore
PMEventSemaphore::PMEventSemaphore(PCSZ ipszName, PHEV phev)
{
	pszName=ipszName;
	hev=*phev;
	ulCount=0;
	rc=DosOpenEventSem(pszName,&hev);
	*phev=hev;
}
// close semaphore
PMEventSemaphore::~PMEventSemaphore()
{
	rc=DosCloseEventSem(hev);
}

