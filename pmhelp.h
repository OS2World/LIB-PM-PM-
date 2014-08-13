/* 


	pmhelp.h (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/
#ifndef _PMHELP_H_
#define _PMHELP_H_


class PMHelpWin : public PMWin 
{
	HELPINIT hiInit;
public:
	PMHelpWin(PSZ title,PSZ library,USHORT id,HAB hab);
	PMHelpWin(PSZ title,PSZ library,PHELPTABLE pht,HAB hab);
	~PMHelpWin();
	BOOL createWin();	
	BOOL destroyWin();
};

BOOL helpHook ( HAB habAnchor, SHORT sMode, USHORT usTopic, USHORT usSubTopic, PRECTL prclPos );


#endif

