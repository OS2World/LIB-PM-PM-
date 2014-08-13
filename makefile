#
#
# pmpp.dll, pmpp.a, pmpp.lib, provadlg.exe makefile
# version 0.16 3-10-1995
# to be used with dmake & gcc/emx 0.9a patchlevel 3
#
CFLAGS=-Zmtd -Zcrtdll -v -Wall -m486          # do NOT put debugging info in dll!
CC=gcc $(CFLAGS)
B=/emx/bin/
I=/emx/include/
IP=/emx/include.cpp/
L=/emx/lib/
DLL=/emx/dll/
LIBS=$(L)crt0.o $(L)mt/c.a $(L)gcc.a
LIBP=$(L)iostream.a
LIBOS2=$(L)os2.a
LIBBSD=$(L)bsd.a

.SUFFIXES: .o .c .cc .cpp .s .exe .obj

.c.o: 
			$(CC) -c $<

.cc.o:
			$(CC) -c $< 

.cpp.o:
			$(CC) -c $< 

.c.s:
			$(CC) -S $< 

.o.exe:
			$(CC) -o $*.exe $< 


default:        lib

all:            examples lib

lib:			pmpp.dll pmpp.a

examples:		provadlg.exe provadlg.hlp bg_paint.exe

clean:
			-del *.o $(DELOPT)
			-del *.exe $(DELOPT)
			-del *.dll $(DELOPT)
			-del *.obj $(DELOPT)
			-del *.map $(DELOPT)
			-del *.msg $(DELOPT)
			-del *.lib $(DELOPT)
			-del *.a $(DELOPT)

#############################################################################

pmsys.o:          pmsys.cpp pmwin.h pmsys.h $(I)stdlib.h $(I)os2emx.h
		$(CC) -Zdll -c pmsys.cpp

pmwin.o:          pmwin.cpp pmwin.h pmgpi.h pmdlg.h pmsys.h $(I)stdio.h $(I)stdlib.h $(I)os2emx.h
		$(CC) -Zdll -c pmwin.cpp

pmdlg.o:          pmdlg.cpp pmwin.h pmdlg.h pmsys.h $(I)stdio.h $(I)os2emx.h
		$(CC) -Zdll -c pmdlg.cpp

pmgpi.o:          pmgpi.cpp pmwin.h pmgpi.h pmsys.h $(I)stdio.h $(I)os2emx.h
		$(CC) -Zdll -c pmgpi.cpp

pmhelp.o:          pmgpi.cpp pmwin.h pmgpi.h pmsys.h pmhelp.h $(I)stdio.h $(I)os2emx.h
		$(CC) -Zdll -c pmhelp.cpp

#############################################################################

pmpp.def: pmsys.o pmwin.o pmgpi.o pmdlg.o pmhelp.o pmpp.df
		cp pmpp.df pmpp.def
		emxexp -o1 pmsys.o pmwin.o pmgpi.o pmdlg.o pmhelp.o >> pmpp.def

pmpp.dll: pmsys.o pmwin.o pmdlg.o pmgpi.o pmhelp.o pmpp.def 
		$(CC) -Zdll -o pmpp.dll pmsys.o pmwin.o pmdlg.o pmgpi.o pmhelp.o pmpp.def
		cp pmpp.dll $(DLL)

pmpp.a: pmpp.def
		emximp -o pmpp.imp pmpp.def
		emximp -o pmpp.a pmpp.imp
		cp pmpp.a $(L)

# create omf library, for linking with omf obj files. (reduces overhead)
pmpp.lib: pmpp.a
		emxomf -s -l pmpp.a
		cp pmpp.lib $(L)

#############################################################################

provadlg.o:       provadlg.cpp provadlg.h pmwin.h pmdlg.h pmgpi.h pmsys.h pmhelp.h $(I)stdio.h $(I)os2emx.h
		$(CC) -g -c provadlg.cpp

provadlg.res:     provadlg.rc provadlg.h 
		rc -r -i \emx\include provadlg.rc 

provadlg:         provadlg.o pmpp.a provadlg.def $(LIBS) $(LIBOS2)
		$(CC) -g -o provadlg provadlg.o pmpp.a provadlg.def
        
provadlg.exe:     provadlg provadlg.res 
		emxbind -bpq -s -rprovadlg.res $(B)emxl provadlg 

#
#  You'll need the IPF file pre-processor by Larry Salomon to do this.
#  It can be found on the hobbes archive (ftp-os2.cdrom.com)
#
provadlg.ipf:     provadlg.ipp pmstdres.h provadlg.h
		ipfcpp -i provadlg.ipp -o provadlg.ipf
#
#  You'll need the IPF compiler by IBM. It can be found on the DEVCON CD's
#  or in the Dev. Toolkits.
#
provadlg.hlp:     provadlg.ipf
		ipfc provadlg.ipf

#############################################################################

bg_paint.o:       bg_paint.cpp bg_paint.h pmwin.h pmdlg.h pmgpi.h pmsys.h pmhelp.h $(I)stdio.h $(I)os2emx.h
		$(CC) -g -c bg_paint.cpp

bg_paint.res:     bg_paint.rc bg_paint.h 
		rc -r -x2 -i \emx\include bg_paint.rc 

bg_paint:         bg_paint.o pmpp.a bg_paint.def $(LIBS) $(LIBOS2)
		$(CC) -o bg_paint bg_paint.o pmpp.a bg_paint.def
        
bg_paint.exe:     bg_paint bg_paint.res 
		emxbind -bpq -s -rbg_paint.res $(B)emxl bg_paint 

