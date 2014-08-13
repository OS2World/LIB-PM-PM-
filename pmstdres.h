/*

	PM ++ Standard Resources defines

	pmstdres.h (emx+gcc) 

	1995 Giovanni Iachello
	This is freeware software. You can use or modify it as you wish,
	provided that the part of code that I wrote remains freeware.
	Freeware means that the source code must be available on request 
	to anyone.
	You must also include this notice in all files derived from this
	file.


*/


#ifndef _PMSTDRES_H_
#define _PMSTDRES_H_

// standard menu ID items

#define PMIDM_FILE 100
#define PMIDM_FILENEW 101
#define PMIDM_FILEOPEN 102
#define PMIDM_FILESAVE 103
#define PMIDM_FILESAVEAS 104
#define PMIDM_FILEPRINT 105
#define PMIDM_FILEEXIT 106

#define PMIDM_EDIT 200
#define PMIDM_EDITUNDO 201
#define PMIDM_EDITREDO 202
#define PMIDM_EDITCUT 203
#define PMIDM_EDITCOPY 204
#define PMIDM_EDITPASTE 205
#define PMIDM_EDITCLEAR 206
#define PMIDM_EDITDUPLICATE 207
#define PMIDM_EDITSELECTALL 208

#define PMIDM_HELP 300
#define PMIDM_HELPINDEX 301
#define PMIDM_HELPGENERAL 302
#define PMIDM_HELPUSING 303
#define PMIDM_HELPKEYS 304
#define PMIDM_HELPABOUT 305

// standard dialog IDs

#define PMDLG_ABOUT 500

// standard help panel IDs (menus)

#define PMHLP_FILE 100
#define PMHLP_FILENEW 101
#define PMHLP_FILEOPEN 102
#define PMHLP_FILESAVE 103
#define PMHLP_FILESAVEAS 104
#define PMHLP_FILEPRINT 105
#define PMHLP_FILEEXIT 106

#define PMHLP_EDIT 200
#define PMHLP_EDITUNDO 201
#define PMHLP_EDITREDO 202
#define PMHLP_EDITCUT 203
#define PMHLP_EDITCOPY 204
#define PMHLP_EDITPASTE 205
#define PMHLP_EDITCLEAR 206
#define PMHLP_EDITDUPLICATE 207
#define PMHLP_EDITSELECTALL 208

#define PMHLP_HELP 300
#define PMHLP_HELPINDEX 301
#define PMHLP_HELPGENERAL 302
#define PMHLP_HELPUSING 303
#define PMHLP_HELPKEYS 304
#define PMHLP_HELPABOUT 305

// standard help panel IDs (dialogs)

#define PMHLP_DLG_ABOUT 500
#define PMHLP_DLG_ERRMSG 501
#define PMHLP_DLG_MSGWIN 502
#define PMHLP_DLG_ASSERTFAIL 503

// standard font/file dialogs IDs  (those IDs double as help panel IDs)
#ifndef INCL_WINSTDFILE

#define DID_FILE_DIALOG			256
#define DID_FILENAME_TXT		257
#define DID_FILENAME_ED			258
#define DID_DRIVE_TXT			259
#define DID_DRIVE_CB			260
#define DID_FILTER_TXT			261
#define DID_FILTER_CB			262
#define DID_DIRECTORY_TXT		263
#define DID_DIRECTORY_LB		264
#define DID_FILES_TXT			265
#define DID_FILES_LB			266
#define DID_HELP_PB			267
#define DID_APPLY_PB			268

#endif

#ifndef INCL_WINSTDFONT

#define DID_FONT_DIALOG			300
#define DID_NAME			301
#define DID_STYLE			302
#define DID_DISPLAY_FILTER		303
#define DID_PRINTER_FILTER		304
#define DID_SIZE			305
#define DID_SAMPLE			306
#define DID_OUTLINE			307
#define DID_UNDERSCORE			308
#define DID_STRIKEOUT			309
#define DID_HELP_BUTTON			310
#define DID_APPLY_BUTTON		311
#define DID_RESET_BUTTON		312
#define DID_NAME_PREFIX			313
#define DID_STYLE_PREFIX		314
#define DID_SIZE_PREFIX			315
#define DID_SAMPLE_GROUPBOX		316
#define DID_EMPHASIS_GROUPBOX		317
#define DID_FONT_ISO_SUPPORT		318
#define DID_FONT_ISO_UNTESTED		319

#endif

#endif

