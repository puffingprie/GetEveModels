gr2 to obj converter

unstuff the gr2 file and run evegr2toobj <src> <dst>
example: evegr2toobj gb2.gr2 gb2.obj

It should now work with all gr2 files from eve that
don't crash the dll.


source has some comments

if you want to compile the source you have to tell
the linker in mingw gcc to use granny2.dll or else
you have to either make an import library out of
the dll or do the LoadLibrary, GetProcAddress,
FreeLibrary stuff.


I am using mingw gcc 4.2.1 tech preview dw2 and
when i use an optimization level of -O2 or higher
something goes wrong and half_to_float returns
wrong results. My guess is that something goes
wrong when it tries to inline functions.


granny.h and main.cpp is majorly based on the code of grnreader
taken from http://aoe3.heavengames.com/cgi-bin/forums/display.cgi?action=ct&f=14,25313,,all
on 2007.12.25

half.h and half.cpp is taken from
http://www.cellperformance.com/articles/2006/07/branchfree_implementation_of_h_1.html
on 2007.12.26

granny2.dll is taken from the Granny Crowd Scene Demo at
http://www.radgametools.com/granny/download.html
on 2007.12.25


USE THIS AT YOUR OWN RISK. IF YOU USE THIS, YOU
AGREE THAT THE AUTHOR CANNOT BE HELD LIABLE FOR
ANY CONSEQUENCES THAT MIGHT OCCUR BECAUSE OF
ITS USAGE.