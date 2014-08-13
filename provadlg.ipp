.* 
.*
.*
.*	provadlg.cpp (emx+gcc) 
.*
.*	1995 Giovanni Iachello
.*	This is freeware software. You can use or modify it as you wish,
.*	provided that the part of code that I wrote remains freeware.
.*	Freeware means that the source code must be available on request 
.*	to anyone.
.*	You must also include this notice in all files derived from this
.*	file.
.*
.*
.*********** This is NOT an IPF file! It must be preprocessed by the
.*********** IPFC program by Larry Salomon Jr. available on the hobbes
.*********** archive in the dev16 directory.

.include "pmstdres.h"
.include "provadlg.h"

:userdoc.
:title.Prova Dlg Sample Help File
:body.
:docprof toc=1234 ctrlarea=coverpage.

:h1 res=&HLP_EXT_PROVADLG..Extended Help
:p.help esteso

:h1 res=&HLP_KEYS_PROVADLG..Keys Help
:p.
F3 exits
Alt-F4 exits
blah blah blah

.***************************************************************************

:h1 res=&HLP_EXT_FDLG..Cicca
:p.help esteso per la finestra di fonts

:h1 res=&HLP_EXT_NDLG..Lippa
:p.help esteso per la finestra di nomi

:h1 res=&HLP_PNL_NDLG_NAME..Name
:p.inserire il nome

:h1 res=&DID_SIZE..Size
:p.set font size

:h1 res=&PMHLP_DLG_ABOUT..Help for About window
:p.help about window

:h1 res=&HLP_DLG_INFO..Information
:p.help for infor window

:h1 res=&DNAME_EF_NAME..Entry Field
:p.insert the name

.***************************************************************************

:h1 res=&PMHLP_FILE..Help for menuitem &odq.File&cdq.
:p.ciao

:h1 res=&PMHLP_FILENEW..Help for menuitem &odq.File New&cdq.
:p.ciao1

:h1 res=&HLP_CRASH..Hello!
:p.My name is Zaphod Beeblebrox!
:i1 .I'm president of the Galaxy!

:h1 res=&HLP_FONTS..Font dialog window
:p.Font dialog window
:i1 .Topolino is Mikey Mouse's name in Italian!

:euserdoc.

