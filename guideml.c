/***************************************************************
**                                                            **
**      GuideML -- Converts AmigaGuide into HTML              **
**                                                            **
***************************************************************/
/*
**  Copyright (C) 1997-98 Richard K�rber  --  All Rights Reserved
**    E-Mail: shred@eratosthenes.starfleet.de
**    URL:    http://shredzone.home.pages.de
**
** Updated 2001-2008 by Chris Young
** chris@unsatisfactorysoftware.co.uk
** https://www.unsatisfactorysoftware.co.uk
***************************************************************/
/* to compile:
stack 500000
gcc guideml.c -o guidemlppc -D__USE_INLINE__ -mcrt=newlib
strip guidemlppc
vc guideml.c -o guideml68k -c99 -lamiga -O3
*/
/*
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
**  The author (Richard K�rber) reserves the right to revoke the
**  GNU General Public License whenever he feels that it is necessary,
**  especially when he found out that the licence has been abused,
**  ignored or violated, and without prior notice.
**
**  You must not use this source code to gain profit of any kind!
**
***************************************************************/
/*
** If you use an editor that is capable of text folding, use
** "//>" and "//<" as begin and end fold markers, respectively.
*/

#include <stdio.h>
#include <string.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/locale.h>
#include <proto/utility.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/memory.h>
#include <exec/exec.h>
#include <dos/dos.h>
#include <libraries/dos.h>

#include <exec/types.h>
#include <exec/io.h>
#include <proto/asl.h>
#include <proto/intuition.h>
#include <proto/wb.h>
#include <proto/window.h>
#include <proto/layout.h>
#include <proto/checkbox.h>
#include <proto/button.h>
#include <proto/label.h>
#include <proto/chooser.h>
#include <proto/getfile.h>
#include <proto/clicktab.h>
#include <proto/string.h>
#include <proto/slider.h>
// #include <clib/integer_protos.h>
// #include <clib/listbrowser_protos.h>
#include <proto/icon.h>

#ifdef __amigaos4__
#include <dos/anchorpath.h>
#include <proto/getcolor.h>
#include <gadgets/getcolor.h>
#endif

#include <intuition/intuition.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>
#include <workbench/icon.h>
#include <workbench/workbench.h>

#include <stdlib.h>

#include <classes/window.h>
#include <gadgets/layout.h>
#include <gadgets/checkbox.h>
#include <gadgets/button.h>
#include <gadgets/chooser.h>
#include <gadgets/clicktab.h>
// #include <gadgets/integer.h>
#include <gadgets/string.h>
#include <gadgets/slider.h>
// #include <gadgets/listbrowser.h>
#include <gadgets/getfile.h>
#include <images/label.h>

//#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include "GuideML_rev.h"

#define LINELEN (1024)                    /* Maximum length of a line */

struct Library *GadToolsBase;
struct Library *WorkbenchBase;

#ifdef __amigaos4__
struct Library *GetColorBase;
struct GadToolsIFace *IGadTools;
struct LocaleIFace *ILocale;
struct UtilityIFace *IUtility;
struct IntuitionIFace *IIntuition;
struct WorkbenchIFace *IWorkbench;
struct WindowIFace *IWindow;
struct StringIFace *IString;
struct SliderIFace *ISlider;
struct GetFileIFace *IGetFile;
struct IconIFace *IIcon;
struct LayoutIFace *ILayout;
struct GetColorIFace *IGetColor;
struct ClickTabIFace *IClickTab;
struct ChooserIFace *IChooser;
struct ButtonIFace *IButton;
struct LabelIFace *ILabel;
struct CheckBoxIFace *ICheckBox;
struct AslIFace *IAsl;
//struct Library *DOSBase;

struct Library *LocaleBase;
struct Library *IntuitionBase;
struct Library *UtilityBase = NULL;
#endif

struct Library *AslBase;
struct Library *WindowBase;
struct Library *LayoutBase;
struct Library *ButtonBase;
struct Library *CheckBoxBase;
struct Library *LabelBase;
struct Library *ChooserBase;
struct Library *ClickTabBase;
// struct Library *IntegerBase;
struct Library *StringBase;
struct Library *SliderBase;
// struct Library *ListBrowserBase;
struct Library *GetFileBase;
struct Library *IconBase;

#ifdef __amigaos4__
  #define CurrentDir SetCurrentDir
#else
  #define GetColorObject      NewObject( GETCOLOR_GetClass(), NULL
#endif

void err(char *,char *,int);
#ifdef __HAIKU__
void err(char * a,char * b,int c)
{
	fprintf(stderr, "%s %s %d\n", a, b, c);
}
#endif
void ui();
void free_list(struct List *);
void freetablist(struct List *);
BOOL make_list(struct List *, UBYTE **);
BOOL maketablist(struct List *, UBYTE **);
void gettooltypes(); // struct WBArg *);
struct Menu *addmenu(struct Window*);
void cleanup(int);
int wbmain(struct WBStartup*);

enum
{
    GID_MAIN=0,
    GID_CONV,
    GID_FILE,
    GID_TO,
    GID_HOMEURL,
    GID_FINDURL,
    GID_PREV,
    GID_NEXT,
    GID_INDEX,
    GID_TOC,
    GID_HELP,
    GID_RETRACE,
    GID_HOME,
    GID_FIND,
    GID_BAR,
    GID_BODY,
    GID_LINKS,
    GID_MSDOS,
	GID_SINGLEFILE,
	GID_EXTLINKS,
	GID_LINENUMBERS,
	GID_NOHTML,
    GID_CSS,
	GID_LA,
    GID_HTMLHEADF,
    GID_HTMLFOOTF,
    GID_WRAP,
    GID_VARWIDTH,
    GID_NAVBAR,
    GID_MOZNAV,
    GID_IMAGES,
    GID_SHOWALL,
    GID_TABS,
    GID_PAGE,
	GID_CTEXT, /* vvv GID_CTEXT to GID_CHIGHLIGHT must stay together and in this order */
	GID_CSHINE,
	GID_CSHADOW,
	GID_CFILL,
	GID_CFILLTEXT,
	GID_CBACKGROUND,
	GID_CHIGHLIGHT, /* ^^^ */
	GID_LINDENT,
    GID_LAST
};

enum
{
    WID_MAIN=0,
    WID_LAST
};

enum
{
    OID_MAIN=0,
    OID_LAST
};

struct Gadget *gadgets[GID_LAST];

char VerStr[] = VERSTAG; //"\0$VER: GuideML "VERSIONSTR" ("VERSIONDATE")";
  static unsigned char defbar[] = " | ";
//	struct WBArg *wbarg;

#ifndef __HAIKU__
// On Haiku and other 64bit systems, IPTR is a 64bit integer (like intptr_t). This allows to use
// readargs and have each argument be either an integer or a pointer as needed.
#define IPTR LONG
#define UIPTR ULONG
#endif

static const UIPTR colour_themes[3][7] = {
	// text      shine     shadow     fill   fill text background highlight
	{ 0x000000, 0xffffff, 0x000000, 0x4a8abd, 0x000000, 0xcfcfcf, 0xffffff }, // AmigaGuide theme
	{ 0x000000, 0xff4444, 0xcccccc, 0x4a8abd, 0x000000, 0xffffff, 0x4444ff }, // Light theme
	{ 0xffffff, 0xff4444, 0xcccccc, 0x4a8abd, 0xffffff, 0x121212, 0x4444ff }, // Dark theme

};

struct Parameter                          /* Structure of Shell parameters */
{
  STRPTR from;
  STRPTR to;
  STRPTR homeurl;
  STRPTR findurl;
  STRPTR prev;
  STRPTR next;
  STRPTR index;
  STRPTR toc;
  STRPTR help;
  STRPTR retrace;
  STRPTR home;
  STRPTR find;
  STRPTR bar;
  STRPTR bodyext;
  IPTR   verbatim;
  IPTR   images;
  IPTR   footer;
  STRPTR linkadd;
  IPTR   nolink;
  IPTR   noemail;
  IPTR   nowarn;
  IPTR   msdos;
  IPTR   singlefile;
  IPTR   dotdotslash;
  IPTR   numberlines;
  IPTR   nonavbar;
  IPTR   moznav;
  IPTR   showall;
  STRPTR htmltoptxt;
  STRPTR htmlheadf;
  STRPTR htmlbottxt;
  STRPTR htmlfootf;
  IPTR   nohtml;
  STRPTR cssurl;
  IPTR   wordwrap;
  IPTR   smartwrap;
  IPTR   varwidth;
  IPTR   noauto;
  UIPTR  lindent;
  STRPTR   theme;
  UIPTR  colours[7];
}param = {NULL};

struct Tracker
{
	BOOL b;
	BOOL i;
	BOOL u;
	BOOL font;
	BOOL pre;
	BOOL p;
	BOOL blockquote;
}track = {FALSE};

  long *htmltop;
  long *htmlbot;

struct text
{
  STRPTR from;
  STRPTR to;
  STRPTR prev;
  STRPTR next;
  STRPTR index;
  STRPTR toc;
  STRPTR help;
  STRPTR retrace;
  STRPTR home;
  STRPTR find;
  STRPTR bar;
};

struct text textlabs;

struct Entry                              /* List of all nodes and links */
{
  struct MinNode link;
  ULONG Count;
  char Node[100];
  char Prev[100];
  char Next[100];
  char TOC[100];
  char Help[100];
  char Index[100];
  char Titl[100];
	STRPTR macro[10];
	STRPTR macrotext[10];
	int macros;
	char *fontname;
	int fontsize;
};
struct MinList entries;

struct LinkStr                            /* Link bar alternative texts */
{
  char Prev[100];
  char Next[100];
  char Index[100];
  char Toc[100];
  char Home[100];
  char Help[100];
  char Find[100];
  char Retrace[100];
};

char Index[100];                          /* Index page name */
char Help[100];
char TOC[100];

char temp[300];

//Some dummy memory-allocating rodents for tooltype porpoises.
char *ttfrom = 0;
char *ttto = 0;
char *tthomeurl = 0;
char *ttfindurl = 0;
char *ttprev = 0;
char *ttnext = 0;
char *ttindex = 0;
char *tttoc = 0;
char *tthelp = 0;
char *ttretrace = 0;
char *tthome = 0;
char *ttfind = 0;
char *ttbar = 0;
char *ttbody = 0;
char *ttlinkadd = 0;
char *tthtmlheadf = 0;
char *tthtmlfootf = 0;
char *ttcss = 0;
char *tttheme = NULL;

int wb=0;
int ok=0;

char defname[100];

//> UBYTE Img#?[] = {...};
/*
**  These are the navigation bar images, in GIF format
*/
UBYTE ImgHome[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x03,0x62,0x49,0x44,0x41,0x54,0x78,0x9c,0xe5,0x54,0x5b,0x4b,0x54,0x61,0x14,0x9d,0xe7,0x7e,0x82,0xef,0x3d,0xf4,0xfa,0x7d,0x46,0x09,0x45,0x50,0x60,0x20,0x92,0x9a,0x96,0x49,0x65,0x44,0x66,0x39,0x91,0x59,0x36,0x9a,0x8a,0x86,0xe3,0xa8,0x93,0x43,0xa3,0x03,0x31,0x66,0xa2,0x8e,0x89,0x61,0xde,0x75,0xcc,0xfb,0xe4,0x05,0xf3,0x8a,0x77,0x4d,0xbc,0x8b,0x77,0x1d,0xe7,0x31,0x7c,0x5c,0x9d,0xbd,0x75,0x2a,0x1c,0xc5,0x11,0x7c,0xeb,0x83,0xc5,0x39,0xc3,0x99,0xb3,0xd6,0xda,0x6b,0xef,0x7d,0x54,0xaa,0xff,0xea,0x34,0xea,0x24,0xbe,0x69,0x25,0xe8,0xfe,0x57,0x4f,0x0f,0x1c,0x75,0x75,0x58,0xc9,0xcb,0xc3,0xcf,0xd4,0x54,0xf4,0xa9,0xd5,0x68,0xf4,0xf5,0xa5,0x67,0x1e,0x0a,0xce,0x9c,0x98,0xbc,0x39,0x4d,0xa2,0x28,0x4a,0x22,0xff,0x99,0x44,0x75,0xa2,0x84,0x3d,0x33,0x13,0xb3,0x49,0x49,0xe8,0x8a,0x88,0x40,0x45,0x70,0x30,0x72,0xbc,0xbd,0x61,0xf0,0xf2,0x22,0x81,0x2b,0xfb,0x22,0xee,0x9f,0x56,0xbd,0xc4,0x97,0x68,0x09,0x5d,0x80,0x84,0xf6,0x86,0x44,0x4e,0xb8,0xc4,0x57,0x8d,0x84,0x55,0xaf,0xff,0x53,0x41,0xb6,0xbf,0x3f,0x91,0xbf,0x56,0xe0,0x73,0x22,0x01,0x5b,0x86,0x44,0x69,0xac,0x44,0xda,0x4d,0x85,0x5c,0xab,0x45,0x49,0x49,0x09,0x52,0x95,0xfb,0x0f,0x0f,0x25,0x2c,0xcf,0x95,0xab,0x9f,0x1f,0x57,0x10,0xed,0xe9,0x49,0x02,0x61,0x0a,0x3c,0x55,0xee,0x46,0xf4,0xdd,0x20,0x51,0xf6,0x46,0x22,0x3d,0x50,0xc2,0x64,0x32,0xa1,0xad,0xad,0x0d,0x93,0x93,0x93,0x68,0x69,0x69,0x41,0x46,0xb0,0x84,0xf1,0xae,0x44,0xf6,0x23,0x81,0xc2,0xb0,0x30,0x67,0x05,0x21,0x0a,0xce,0xba,0x4d,0x5e,0x1e,0x27,0xa1,0x0f,0x52,0x04,0xd2,0xd3,0x51,0x53,0x53,0x83,0x8e,0x8e,0x0e,0x8c,0x8d,0x8d,0xa1,0xa9,0xa9,0x09,0x16,0x8b,0x85,0x05,0xf4,0xb7,0x04,0xde,0x87,0x08,0x67,0x05,0xee,0x09,0x50,0x2c,0xe4,0x9c,0xc8,0xc9,0x39,0x11,0x4f,0x4d,0x4d,0x61,0x67,0x67,0x07,0x76,0xbb,0x1d,0x9b,0x9b,0x9b,0x18,0x19,0x19,0x41,0x43,0x43,0x03,0x4c,0xa1,0x02,0x5a,0x3f,0x81,0x14,0x7f,0xe9,0x9e,0x00,0x35,0x94,0x32,0xa7,0x58,0x0c,0x06,0x03,0xea,0xeb,0xeb,0xd1,0xd7,0xd7,0x87,0x99,0x99,0x19,0x4c,0x4c,0x4c,0x60,0x74,0x74,0x14,0xfd,0xfd,0xfd,0xe8,0xee,0xee,0x46,0x65,0x65,0x25,0xcc,0x66,0x33,0xf7,0x23,0xd1,0x47,0x22,0xde,0x9b,0x45,0x8e,0xee,0x01,0x8d,0x22,0x4d,0x0b,0x35,0x94,0x9c,0xb7,0xb7,0xb7,0xb3,0x73,0x87,0xc3,0xc1,0xee,0xb7,0xb7,0xb7,0xd9,0xfd,0xfa,0xfa,0x3a,0x56,0x57,0x57,0xb1,0xb2,0xb2,0xc2,0x62,0xb5,0xb5,0xb5,0xf8,0xf8,0x58,0x22,0xe6,0x9a,0xc4,0x8b,0x4b,0x82,0x44,0xce,0xb9,0x88,0xd0,0x12,0xd1,0x9c,0xd3,0x28,0x1a,0x8d,0x46,0xe4,0xe6,0xe6,0xc2,0x6a,0xb5,0xb2,0xfb,0xc1,0xc1,0x41,0x76,0xee,0xcc,0x9f,0x26,0xa9,0xac,0xac,0x0c,0xe5,0xe5,0xe5,0xa8,0xaa,0xaa,0x42,0x41,0x41,0xc1,0xde,0x3b,0x4f,0x24,0xa2,0x2e,0x0b,0x3c,0x3d,0x7f,0x88,0x08,0x2d,0x90,0xd6,0x4f,0xa2,0x47,0xd9,0xd0,0xfc,0xfc,0x7c,0x3c,0x08,0xd5,0x20,0x3c,0x4c,0x03,0xc3,0x1d,0xa5,0x81,0x2f,0x35,0x88,0x8b,0xd5,0x60,0x63,0x63,0x03,0x36,0x9b,0x0d,0xea,0x8b,0x82,0x89,0x34,0x57,0x05,0xe2,0xaf,0x0b,0xbc,0xf5,0x15,0x6c,0xa4,0xb5,0xb5,0x15,0x79,0x6a,0x01,0xf5,0x05,0x16,0x70,0xd9,0x07,0xfa,0x71,0x65,0x7a,0x7a,0x9a,0x73,0x8f,0x54,0x27,0xe0,0xfe,0xbb,0x1f,0x2c,0x40,0xd7,0xe4,0xc4,0x04,0xae,0x82,0x9c,0x13,0x39,0x11,0xeb,0x02,0x84,0x32,0x49,0x42,0x89,0x47,0x70,0x65,0x75,0xca,0xa7,0x43,0xb5,0xb7,0x6c,0xb4,0x0f,0x2e,0x1b,0x4d,0xe5,0x78,0x50,0xae,0x14,0x05,0x11,0x3b,0x41,0x04,0xc5,0xaf,0x04,0x67,0xdf,0xd5,0xd5,0xc5,0xce,0x89,0xfc,0x5f,0x81,0xc5,0xc5,0x45,0x36,0x70,0x80,0xdc,0xb5,0xd9,0xd4,0xd0,0xf1,0xf1,0x71,0x18,0x52,0xf4,0x48,0xfa,0xbc,0xc0,0x2f,0xf3,0x35,0x4b,0xcf,0x4b,0x46,0xfb,0x40,0xee,0xad,0x26,0x0d,0x0a,0x63,0x82,0xf8,0x79,0x51,0x94,0x40,0x67,0x67,0x27,0x2f,0xe0,0xb1,0xa3,0x4a,0x93,0x32,0x37,0x37,0x87,0x4f,0x59,0x46,0x06,0x39,0xb7,0x98,0x8d,0x8c,0xad,0xad,0x2d,0x0c,0x0c,0x0c,0x70,0xe6,0x44,0xec,0x24,0x27,0x2c,0x2f,0x2f,0x63,0x78,0x78,0xf8,0x78,0x01,0x8a,0x61,0x61,0x61,0x01,0x74,0x76,0x77,0x77,0x79,0x44,0x29,0x36,0x1a,0xd7,0xa5,0xa5,0x25,0x5e,0xae,0xd4,0x80,0xbf,0xc4,0x84,0x8a,0x78,0xc1,0xc2,0xcd,0xcd,0xcd,0xc7,0x0b,0x90,0x13,0x12,0xa0,0x66,0x0f,0x0d,0x0d,0x71,0xe9,0x44,0x5a,0x5d,0x5d,0x8d,0xd9,0xd9,0x59,0xce,0x39,0xe3,0xf6,0x1e,0xa9,0x13,0x0d,0x3a,0x81,0xb5,0xb5,0x35,0xfe,0xff,0x91,0x02,0xfa,0xf5,0x52,0x10,0xe6,0xe7,0xe7,0x99,0x9c,0xf2,0xa6,0x5e,0x50,0xd9,0xe4,0xae,0xb7,0xb7,0x97,0x1b,0x4c,0xa3,0x98,0x79,0x6f,0x8f,0xd4,0x89,0xce,0x4c,0xc9,0x5b,0x5e,0x5c,0x5c,0x0c,0x27,0x8f,0x4b,0x93,0x23,0xc7,0xcc,0x38,0x4d,0xa8,0x0e,0x8e,0x69,0xa0,0x2d,0x19,0xa7,0x89,0xc3,0xa2,0xf2,0xd8,0x57,0x0d,0x39,0x25,0x1c,0xbe,0x68,0xfb,0xaa,0xa7,0x01,0xee,0xc1,0x6f,0xfb,0xcf,0x63,0xb7,0x23,0x14,0x09,0x7f,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGHOME 965

UBYTE ImgIndex[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x05,0x2d,0x49,0x44,0x41,0x54,0x78,0x9c,0xad,0x55,0x5b,0x48,0x9c,0x57,0x10,0x9e,0x17,0x2b,0xe6,0x21,0x88,0xa9,0x4a,0x14,0xac,0x52,0x7f,0x88,0x68,0x48,0x02,0xa2,0xe0,0x83,0xf5,0x82,0x20,0x12,0xab,0xd1,0x8a,0xfd,0x89,0x04,0x0d,0x5e,0xd2,0xac,0xd6,0x4b,0x34,0x89,0xda,0x20,0x45,0x1f,0xd4,0x44,0xad,0x51,0x5a,0x34,0x44,0x63,0xa3,0x09,0xda,0xe8,0xd2,0x64,0xd1,0xdd,0x55,0xd7,0x5d,0xd7,0x75,0x2f,0xea,0xae,0xba,0x5e,0x13,0x2f,0x50,0x9b,0x06,0x7d,0x50,0x82,0xbe,0xf8,0x34,0x3d,0x73,0xd2,0xff,0x67,0x8b,0xa4,0x2f,0xe9,0xc3,0xf0,0xf3,0x9f,0x33,0xe7,0xfb,0xbe,0x99,0x33,0x33,0x07,0x8e,0x8f,0x8f,0xe1,0xe8,0xe8,0x08,0x0e,0x0e,0x0e,0x60,0x6f,0x6f,0x0f,0xde,0xbe,0x7d,0x0b,0x5b,0x5b,0x5b,0xb0,0xbe,0xbe,0x0e,0x4b,0x4b,0x4b,0xe0,0x70,0x38,0xc0,0x6a,0xb5,0xc2,0xd4,0xd4,0x14,0x4c,0x4c,0x4c,0x80,0x46,0xa3,0x01,0x95,0x4a,0x05,0x4a,0xa5,0x12,0x06,0x06,0x06,0xa0,0xb7,0xb7,0x17,0xba,0xbb,0xbb,0xa1,0xb3,0xb3,0x13,0xda,0xdb,0xdb,0xa1,0xa5,0xa5,0x05,0x1a,0x1a,0x1a,0xa0,0xae,0xae,0x0e,0x6a,0x6a,0x6a,0x00,0x0e,0x0f,0x0f,0xff,0x13,0xdc,0x66,0xb3,0x9d,0x00,0x1f,0x1a,0x1a,0xe2,0xe0,0x7d,0x7d,0x7d,0x1c,0xbc,0xa3,0xa3,0x43,0x06,0x6f,0x6c,0x6c,0x94,0xc1,0x2b,0x2b,0x2b,0x01,0xf6,0xf7,0xf7,0x65,0xf0,0xed,0xed,0xed,0x8f,0x82,0x6b,0xb5,0x5a,0x59,0x79,0x7f,0x7f,0xbf,0x0c,0xee,0xaa,0xdc,0x15,0xbc,0xaa,0xaa,0x0a,0xca,0xcb,0xcb,0x01,0x76,0x77,0x77,0x39,0xf8,0xe6,0xe6,0xe6,0x67,0x63,0x63,0x3a,0xed,0xc8,0x88,0x06,0xc7,0xc7,0xf5,0x38,0x31,0x61,0xc4,0xc9,0x49,0x33,0x1a,0x8d,0x16,0x34,0x99,0x6c,0x1f,0x35,0xda,0x27,0x3f,0xf2,0x57,0xa9,0xd4,0xef,0x58,0xca,0x0c,0x65,0x65,0x65,0x67,0x99,0xb9,0x17,0x17,0x17,0x03,0xec,0xec,0xec,0xf0,0xb4,0x8c,0x8e,0x8e,0x69,0xd5,0x6a,0x2d,0x3b,0x60,0x46,0xb3,0x79,0x16,0xad,0x56,0x07,0xce,0xce,0x2e,0xe2,0xdc,0x9c,0x13,0xed,0xf6,0x25,0x74,0x38,0x96,0x4f,0x18,0xad,0xd3,0x3e,0xf9,0x91,0xff,0xf4,0xf4,0x0c,0x13,0x67,0xc0,0xc7,0x8f,0xbb,0x6c,0x39,0x39,0x39,0x81,0x05,0x05,0x05,0xee,0x72,0xce,0x49,0xb9,0x04,0x6e,0xb3,0xcd,0xf3,0x43,0x12,0xf0,0xc2,0xc2,0x2a,0x2e,0x2e,0xae,0x31,0x5b,0x77,0xb1,0x35,0xbe,0x2e,0x11,0x91,0x3f,0x9d,0xa3,0xf3,0x94,0x81,0x94,0x94,0x94,0xd8,0xac,0xac,0x2c,0x6f,0x39,0xe7,0xa3,0xa3,0xe3,0x4c,0x81,0x4d,0x56,0x4e,0x07,0xe7,0xe7,0x57,0x38,0x98,0xd3,0xf9,0x06,0x97,0x96,0x36,0x4e,0x18,0xad,0xd3,0x3e,0xf9,0x91,0xbf,0x14,0x09,0xa5,0x2b,0x2e,0x2e,0x2e,0x23,0x3d,0x3d,0x3d,0x08,0x9c,0x4e,0x27,0xbf,0x50,0x0a,0xcd,0x6a,0xb5,0xcb,0xca,0x25,0x70,0x02,0x5a,0x5e,0xde,0xc4,0xd5,0xd5,0xed,0x13,0x46,0xeb,0xb4,0x2f,0x91,0x48,0x91,0xd0,0x9d,0x44,0x47,0x47,0x8b,0xc9,0xc9,0xc9,0x02,0xd8,0xed,0x76,0x5e,0x2d,0xc4,0x4a,0x21,0x52,0x4e,0x49,0x8d,0x04,0xbe,0xb2,0xb2,0xc5,0xc1,0x5e,0xbf,0xfe,0xe3,0x84,0xd1,0x3a,0xed,0x4b,0x24,0x74,0x8e,0xce,0xd3,0xc5,0x47,0x45,0x45,0x89,0x89,0x89,0x89,0x02,0x58,0x2c,0x16,0x5e,0x8a,0xc4,0x2a,0xa9,0xa7,0xdc,0x52,0xf8,0x92,0x72,0x02,0xdb,0xd8,0xf8,0x13,0x37,0x37,0xff,0x92,0x8d,0xfe,0x69,0x9d,0x7c,0xc8,0xc8,0x9f,0xce,0xd1,0x79,0xaa,0xae,0xc8,0xc8,0x48,0x31,0x3e,0x3e,0x5e,0x00,0xa3,0xd1,0xc8,0xeb,0x5c,0x22,0xf8,0xa0,0x7e,0x0d,0x7b,0x7b,0x07,0x30,0x2f,0xaf,0x80,0xfd,0xaf,0x72,0x95,0xa2,0x98,0x85,0x15,0x15,0x95,0x78,0xf3,0x66,0x11,0x0e,0x0f,0x8f,0x73,0xc0,0xf2,0xf2,0xbb,0x78,0xf7,0xee,0x0f,0x6c,0xad,0x10,0x07,0x07,0x55,0x38,0x33,0xb3,0x20,0x13,0x84,0x87,0x87,0x8b,0x2c,0x4d,0x02,0xe8,0x74,0x3a,0xde,0xa1,0xff,0x26,0x58,0xc7,0x5b,0xb7,0xee,0x60,0x55,0xd5,0x3d,0xec,0xef,0x57,0x72,0x12,0x85,0xe2,0x7b,0x9c,0x9a,0x9a,0xc5,0xce,0xce,0x27,0xd8,0xd8,0xd8,0x8c,0x0f,0x1e,0xb4,0xb2,0x72,0xfc,0x15,0x0d,0x06,0x0b,0xab,0x7f,0x2d,0x3e,0x7c,0xf8,0x33,0x4b,0x8d,0x8d,0x93,0x10,0xc1,0x85,0x0b,0x17,0x44,0x96,0x26,0x01,0xd4,0x6a,0x35,0xef,0x50,0x57,0x82,0xe9,0xe9,0x59,0xa6,0xee,0x0e,0xf6,0xf4,0x3c,0xc7,0xc2,0xc2,0x12,0xf6,0x6f,0xc7,0xd4,0xd4,0x34,0xbc,0x71,0x43,0x81,0x97,0x2f,0x7f,0x8d,0x1d,0x1d,0xdd,0x58,0x54,0x54,0xc2,0x55,0x8f,0x8d,0x19,0x51,0xaf,0xb7,0x70,0x22,0x93,0xe9,0x43,0x89,0x13,0x41,0x58,0x58,0x98,0x18,0x11,0x11,0x21,0xc0,0xab,0x57,0xaf,0x78,0xfb,0xbb,0x12,0xdc,0xbf,0xdf,0x82,0xa5,0xa5,0xe5,0x2c,0x8a,0xdb,0x98,0x96,0xf6,0x0d,0xaa,0xd5,0x7a,0xcc,0xcd,0xcd,0x47,0xad,0x76,0x12,0x95,0xca,0x11,0xcc,0xce,0xbe,0x8e,0xf7,0xee,0xfd,0x88,0xad,0xad,0xbf,0xb0,0x8a,0xb1,0x32,0xe5,0x56,0x7c,0xf4,0xe8,0x09,0xab,0xc2,0x79,0x99,0x20,0x24,0x24,0x44,0xbc,0x74,0xe9,0x92,0x20,0x0f,0x2e,0x57,0x02,0x02,0x1b,0x19,0xd1,0xe1,0xcb,0x97,0x1a,0xac,0xab,0xab,0xc7,0xfa,0xfa,0x26,0xae,0x3c,0x3f,0xff,0x3b,0xcc,0xc9,0xc9,0xc5,0x86,0x86,0x66,0x1c,0x1a,0x52,0x31,0xbf,0x02,0x2c,0x2b,0xbb,0x8d,0xd7,0xae,0x65,0x63,0x5f,0xdf,0x6f,0x9c,0x48,0x22,0x10,0x04,0x41,0x3c,0x7f,0xfe,0xbc,0xc0,0x07,0x17,0x8d,0x5c,0xd7,0x2a,0xa2,0x96,0xd7,0xe9,0x4c,0xdc,0x0c,0x06,0x2b,0x37,0xad,0x56,0x8f,0x1a,0x8d,0x9e,0x7d,0x0d,0xac,0x91,0xcc,0x3c,0x25,0x7a,0xbd,0x99,0x09,0x19,0xe7,0x97,0x4e,0xeb,0x06,0xc3,0x34,0x6f,0x34,0x22,0x08,0x0a,0x0a,0x12,0xcf,0x9d,0x3b,0x27,0xc0,0xd3,0xa7,0x4f,0xf9,0x54,0x74,0xed,0x03,0x9b,0xcd,0xc1,0x2e,0x8b,0xc8,0x56,0x78,0x8d,0xcf,0xcf,0xaf,0x32,0x72,0x27,0x1b,0x03,0x34,0x6f,0xe6,0xd8,0xfe,0x02,0x2b,0xc9,0x75,0x7e,0xf9,0x94,0x16,0x22,0x23,0xf5,0x66,0xf3,0x1c,0x17,0x49,0x7d,0x10,0x10,0x10,0x20,0x06,0x07,0x07,0x0b,0xd0,0xd5,0xd5,0xc5,0x47,0xee,0xff,0xdd,0xc9,0xfe,0xfe,0xfe,0x22,0x23,0x11,0xe4,0xc7,0x42,0xa5,0x1a,0x7e,0x67,0x32,0x59,0x3f,0x79,0x16,0x59,0x2c,0x76,0x7c,0xf1,0x42,0xf9,0xde,0xd7,0xd7,0x37,0xd3,0xcf,0xcf,0xef,0x4b,0x68,0x6b,0x6b,0xe3,0x8f,0xc5,0xb3,0x67,0xcf,0x0d,0x14,0xc5,0xa7,0x4c,0x53,0x12,0x47,0xf7,0xd0,0xd4,0xf4,0xd3,0x1b,0x6f,0x6f,0xef,0x34,0x1f,0x1f,0x9f,0x2f,0xa0,0xb9,0xb9,0x99,0xbf,0xa1,0xd5,0xd5,0xd5,0x67,0xbb,0xbb,0x7b,0x6c,0x34,0x6a,0xf5,0x7a,0x13,0x0f,0x93,0x2e,0x8b,0x2e,0x9c,0x48,0x29,0xbf,0x27,0x6d,0x96,0xef,0x93,0x1f,0xf9,0x0f,0x0e,0xfe,0xfe,0x9e,0x81,0xbf,0x66,0xc0,0x8a,0x33,0x67,0xce,0x7c,0xe5,0xe5,0xe5,0xf5,0x39,0xd4,0xd7,0xd7,0xf3,0x67,0x8e,0x11,0xb8,0x2b,0x14,0x8a,0xc0,0xcc,0xcc,0xcc,0xd8,0xa4,0xa4,0xa4,0x8c,0x84,0x84,0x04,0x31,0x36,0x36,0x96,0xda,0x9d,0x3a,0x52,0xa4,0xd9,0x42,0xed,0x4f,0x1d,0x4a,0x4d,0x44,0x75,0x4e,0xa5,0x48,0xd5,0x42,0x17,0x4a,0x39,0x67,0x69,0xf9,0x96,0x94,0xff,0x03,0x1e,0x78,0xfa,0xf4,0x69,0x77,0xa8,0xad,0xad,0x95,0x1f,0xe8,0xd2,0xd2,0x52,0xf7,0xfc,0xfc,0x7c,0x6f,0xf6,0x50,0x04,0x65,0x64,0x64,0x08,0x57,0xae,0x5c,0x11,0x68,0xe4,0xd2,0x54,0xa4,0xc1,0x45,0xb3,0x85,0xda,0x9f,0x3a,0x94,0x9a,0x88,0xea,0x9c,0x4a,0x91,0xaa,0x85,0x2e,0x94,0x72,0x4e,0x69,0x21,0xe5,0x04,0xee,0xe1,0xe1,0x01,0xf2,0x03,0x5d,0x51,0x51,0x01,0x25,0x25,0x25,0xc0,0xa2,0x80,0xbc,0xbc,0x3c,0xc8,0xce,0xce,0x86,0xab,0x57,0xaf,0x02,0x23,0x82,0xd4,0xd4,0x54,0x60,0x51,0x01,0x8b,0x0a,0x62,0x62,0x62,0x80,0x91,0x00,0x23,0x81,0x8b,0x17,0x2f,0x42,0x68,0x68,0x28,0xb0,0x48,0x20,0x30,0x30,0x10,0x58,0x14,0xc0,0x22,0x00,0x4f,0x4f,0x4f,0x38,0x75,0xea,0x14,0xb8,0xb9,0xb9,0xc1,0xdf,0x2a,0x61,0xd0,0x3b,0x13,0x4e,0x2e,0xc1,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGINDEX 1424

UBYTE ImgNext[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x01,0x63,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x60,0x18,0x31,0x60,0xf3,0x89,0x67,0xff,0xb3,0xa7,0x5c,0xf8,0x0f,0x64,0x4a,0x01,0x31,0x17,0xcd,0x2c,0xc8,0x9b,0x0a,0xb6,0x44,0x93,0xea,0x96,0xc0,0x2c,0x28,0x98,0x7e,0xf9,0x7f,0xde,0x94,0xb3,0xd4,0xb7,0x04,0xd9,0x82,0x92,0xd9,0xd7,0xff,0x17,0x4c,0x3a,0x45,0x5d,0x4b,0xd0,0x2d,0xa8,0x9c,0x77,0xeb,0x7f,0xc1,0x84,0x63,0xa4,0x59,0xb2,0x78,0xf7,0xad,0xff,0x4b,0xf7,0xde,0xf9,0xbf,0xfc,0xc0,0xbd,0xff,0xab,0x0e,0xde,0xff,0xbf,0xe6,0xf0,0xc3,0xff,0xeb,0x8e,0x3e,0xfe,0xbf,0xe1,0xd8,0x93,0xff,0xd8,0x2c,0xa8,0x5d,0x78,0xf7,0x7f,0x41,0xdf,0x21,0xe2,0x2d,0x01,0x59,0xe0,0x53,0xb3,0xe3,0x7f,0x40,0xc3,0x9e,0xff,0xa1,0xcd,0xfb,0xff,0x47,0xb5,0x1f,0xfe,0x1f,0xd7,0x75,0xec,0x7f,0x52,0xef,0xc9,0xff,0xe9,0x13,0xce,0x60,0xb5,0xa0,0x69,0xe9,0xc3,0xff,0x05,0x3d,0xfb,0x89,0xb3,0x64,0xf1,0x1e,0xf2,0x2c,0x68,0x5b,0xf1,0xe4,0x7f,0x41,0xd7,0x2e,0xc2,0x96,0x50,0x62,0x41,0xf7,0xea,0xe7,0xff,0xf3,0xda,0xb7,0xe1,0xb7,0x84,0x52,0x0b,0xfa,0xd7,0xbf,0xfe,0x9f,0xdf,0xb6,0x19,0xb7,0x25,0xe4,0xc6,0x01,0x16,0x0b,0xbc,0x18,0x20,0x39,0x9e,0xba,0x3e,0xc8,0x69,0xd9,0x08,0x32,0xbc,0x84,0x26,0x16,0x64,0x36,0xae,0x83,0x19,0x1e,0x8a,0x3b,0x88,0xc8,0xb4,0x20,0xb5,0x6e,0x0d,0x61,0xc3,0x41,0xc0,0xa3,0x72,0xeb,0x7f,0xcb,0xd8,0xee,0xff,0x36,0x09,0x7d,0xff,0x1d,0x92,0x27,0xfc,0x77,0x49,0x9b,0xfc,0xdf,0x3d,0x63,0xea,0x7f,0xaf,0xac,0xe9,0x38,0x2d,0x48,0xaa,0x59,0x45,0x9c,0xe1,0x50,0x00,0x0a,0x37,0x7b,0x20,0x8e,0x44,0xc7,0xd8,0x72,0x72,0x5c,0xe5,0x72,0x92,0x0c,0x67,0x80,0x2a,0x00,0x59,0xa2,0x8a,0x8e,0xd1,0x2d,0x88,0x2e,0x5f,0x42,0xb2,0xe1,0x78,0x01,0xb2,0x05,0xe1,0xa5,0x8b,0xa8,0x6b,0x38,0x08,0x6c,0x3a,0x0e,0xb1,0x20,0xa4,0x68,0x3e,0xf5,0x0d,0x07,0x01,0x50,0x24,0x07,0xe4,0xcf,0xa5,0x8d,0xe1,0x50,0x00,0x4b,0x00,0x5e,0xb4,0x30,0x9c,0x81,0x01,0x91,0x00,0x48,0xaa,0xf4,0x01,0xf4,0xeb,0x45,0xba,0xbc,0x77,0xc6,0xe8,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGNEXT 454

UBYTE ImgPrev[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x01,0x55,0x49,0x44,0x41,0x54,0x78,0x9c,0x63,0x60,0x18,0x71,0x60,0xfb,0xe9,0x97,0xff,0x4b,0x66,0x5f,0xff,0x0f,0x64,0x4a,0x01,0x31,0x17,0x55,0x0d,0xdf,0x7a,0xea,0xc5,0xff,0x82,0xe9,0x97,0xff,0x87,0x97,0x2e,0x02,0x59,0x60,0x0f,0xb5,0x84,0x3a,0x60,0xf3,0x89,0x67,0xff,0xb3,0xa7,0x5c,0x40,0xb6,0x20,0x12,0x88,0x55,0xa9,0x62,0xf8,0x86,0x63,0x4f,0xfe,0xa7,0x4f,0x38,0x43,0x1b,0x0b,0xd6,0x1d,0x7d,0xfc,0x3f,0xa9,0xf7,0x24,0x6d,0x2c,0x58,0x73,0xf8,0xe1,0xff,0xb8,0xae,0x63,0x18,0x16,0x80,0x22,0x79,0xcf,0xf9,0xb7,0xff,0xf7,0x5d,0x78,0xf7,0xff,0xe0,0xa5,0x0f,0xff,0x0f,0x5f,0xf9,0xf4,0xff,0xe8,0xd5,0xcf,0xff,0x8f,0x5f,0xff,0xfa,0x7f,0xf2,0xa6,0x77,0xc4,0x25,0x80,0x55,0x07,0xef,0xff,0x8f,0x6a,0x3f,0x8c,0xd3,0x82,0xca,0x79,0xb7,0xfe,0xd7,0x2e,0xbc,0xfb,0xbf,0x69,0xe9,0xc3,0xff,0x6d,0x2b,0x9e,0xfc,0xef,0x5e,0xfd,0xfc,0x7f,0xff,0xfa,0xd7,0xff,0xf3,0xdb,0x36,0x13,0x4e,0x00,0xcb,0x0f,0xdc,0xfb,0x1f,0xda,0xbc,0x9f,0x12,0x0b,0x70,0x07,0xdf,0xd2,0xbd,0x77,0xfe,0x07,0x34,0xec,0xa1,0x8d,0x05,0x8b,0xf7,0xdc,0xfa,0xef,0x53,0xb3,0x63,0x08,0x5b,0x00,0x8a,0x98,0x79,0x3b,0x6e,0xd2,0xd4,0x02,0x50,0xd2,0xd2,0x9c,0xba,0xf9,0x3a,0xed,0x22,0x19,0x66,0x49,0xdf,0xda,0xab,0x34,0xb3,0x00,0x6e,0x49,0xc7,0xca,0x2b,0x58,0x2d,0x00,0xe5,0xe4,0xe8,0xf2,0x25,0xff,0xe3,0x2a,0x97,0xff,0x4f,0xaa,0x59,0xf5,0x3f,0xb5,0x6e,0xcd,0xff,0xcc,0xc6,0x75,0xff,0x73,0x5a,0x36,0x12,0x6d,0x01,0xdc,0x92,0xc6,0xc5,0x97,0x71,0x15,0x15,0x25,0x40,0x9c,0x02,0x35,0x0c,0x1d,0x13,0x5d,0xd2,0x82,0x2d,0xa9,0x9a,0x7f,0x09,0x9b,0x05,0x29,0x50,0x83,0x54,0xb1,0x60,0x92,0xea,0x0a,0xb0,0x25,0xa5,0xb3,0x2f,0xd2,0xae,0xb8,0x86,0x59,0x52,0x30,0xe3,0x12,0xcd,0x2c,0x40,0x58,0x42,0x43,0x0b,0xe0,0x96,0x00,0xb1,0x17,0x03,0xb5,0xab,0x4c,0x34,0x4b,0xa4,0x18,0x88,0x8c,0x48,0x00,0x8f,0xb4,0x4b,0x1a,0x75,0x62,0xc3,0x0c,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGPREV 440

UBYTE ImgTOC[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x01,0xd2,0x49,0x44,0x41,0x54,0x78,0x9c,0xed,0x95,0xcb,0x4e,0xc2,0x40,0x14,0x86,0xa7,0x55,0x5a,0x28,0x05,0x52,0x6e,0x52,0x02,0xa1,0xd6,0x80,0x01,0x42,0x34,0x01,0xb9,0xc4,0x60,0x4d,0x24,0xb2,0x72,0x45,0x74,0xd5,0x25,0x2b,0x1e,0xc1,0xc4,0xe7,0x30,0x31,0x6e,0x64,0x63,0x8c,0x0b,0xe2,0xed,0x09,0x5c,0xf8,0x58,0xe3,0x3f,0x32,0x44,0x0c,0x14,0x1d,0xed,0xd2,0x93,0x7c,0xe9,0x70,0xda,0x9e,0xcb,0x9c,0x7f,0x0a,0x21,0xff,0x26,0x68,0x21,0x98,0x6b,0x18,0xc6,0x03,0xd6,0x51,0xa0,0x32,0x67,0x36,0x9b,0xa5,0x22,0xb0,0x57,0x80,0xb6,0x10,0x5d,0x96,0xe5,0xb8,0xa2,0x28,0xfb,0xc9,0x64,0xf2,0x19,0x3f,0x2d,0x60,0xf0,0x5b,0x31,0x50,0x04,0x8d,0x1f,0x52,0xe4,0xef,0x2c,0x98,0x04,0x82,0xe9,0x74,0xfa,0x05,0xd7,0x0a,0xc8,0x30,0xa7,0x6d,0xdb,0x54,0x04,0x9e,0x24,0xe5,0xb5,0x4d,0xb2,0x69,0x9a,0x4f,0xbc,0x8a,0x8c,0xdf,0x1d,0x7c,0x58,0x2e,0x97,0x63,0x33,0xb0,0x67,0x09,0xaa,0xd5,0x2a,0x5d,0x06,0x6e,0x39,0x60,0x97,0x07,0x9c,0x67,0xf9,0x0c,0xe6,0xcc,0xe0,0xc1,0xbf,0x9b,0xc1,0xea,0x4a,0x3d,0x4c,0x06,0x41,0x5e,0x81,0xca,0x7d,0x1a,0xaf,0x4a,0xb8,0xd2,0x2f,0x16,0x8d,0x46,0x8f,0x30,0xa4,0x31,0xb8,0x8d,0xc5,0x62,0xa7,0x70,0x85,0x59,0xc2,0x56,0xab,0x45,0x45,0xf0,0x4c,0x1c,0x89,0x44,0x0e,0x21,0xd5,0x3c,0xce,0x42,0xaf,0x50,0x28,0x5c,0xc1,0x65,0x82,0x10,0xf1,0x71,0xc8,0x8a,0x24,0x49,0xe9,0x52,0xa9,0x74,0xa7,0x69,0xda,0x09,0x99,0x0e,0x5a,0x77,0x1c,0x87,0x8a,0x40,0xbc,0x64,0x1a,0x08,0x04,0x52,0xb5,0x5a,0x6d,0x82,0xe0,0x0e,0xf9,0x54,0x51,0xd0,0xb7,0x0e,0x2a,0x95,0xca,0x75,0xa3,0xd1,0x78,0xc5,0xf5,0x1e,0x47,0xfe,0x02,0xae,0x0d,0x96,0xb7,0xdf,0xef,0xd3,0x65,0x90,0x5f,0xc8,0x94,0xed,0xb7,0x05,0x76,0xc0,0x36,0x88,0xb3,0x04,0x2b,0x3a,0x10,0x96,0xa9,0x02,0x12,0x64,0x3a,0x5c,0xb6,0x87,0x3a,0x58,0x23,0x7e,0xc9,0x14,0xdf,0xa0,0xe3,0x7a,0xbd,0x3e,0x6e,0x36,0x9b,0x13,0x28,0xaa,0xc3,0x3b,0x92,0x06,0x83,0x01,0x15,0xc1,0x33,0xb1,0xae,0xeb,0x65,0xc8,0xd4,0xc4,0x39,0x38,0xc7,0xe7,0x62,0x44,0xf8,0x0c,0x56,0x6c,0x91,0xb0,0x4c,0xd7,0xf1,0x5f,0x70,0xd0,0xeb,0xf5,0xde,0x12,0x89,0x84,0x4b,0xa6,0x4a,0x0a,0xba,0xae,0x4b,0x45,0x20,0x5e,0x32,0x45,0xd0,0x3d,0xb6,0x2d,0xaa,0xaa,0x96,0xbb,0xdd,0xee,0x23,0xd6,0x5b,0x20,0xe2,0x5b,0x07,0x96,0x65,0x9d,0xb5,0xdb,0xed,0xcb,0x4e,0xa7,0x73,0x93,0xcf,0xe7,0x47,0xb3,0x07,0x87,0xc3,0x21,0x5d,0x06,0xf9,0x85,0x4c,0x99,0x8a,0x98,0x34,0x73,0x60,0x93,0x4c,0xd5,0x14,0x5e,0xd1,0x81,0xb0,0x4c,0x67,0x92,0x64,0x49,0x0c,0xbe,0x96,0xc9,0x1f,0x64,0xfa,0x0e,0xba,0x95,0xd8,0xf9,0x75,0xcf,0xd9,0x1f,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGTOC 565

UBYTE ImgHelp[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x05,0x5e,0x49,0x44,0x41,0x54,0x78,0x9c,0xa5,0x95,0x6b,0x4c,0x93,0x67,0x14,0xc7,0x4f,0x8b,0x55,0x41,0x27,0xa8,0xa0,0x01,0xdc,0xc0,0x78,0x99,0x5c,0x65,0x50,0x02,0x05,0x85,0x20,0x0a,0xa2,0x13,0x70,0xc0,0x12,0x61,0x1b,0x8c,0x4b,0xc6,0x70,0x11,0x98,0xb0,0x29,0x23,0x56,0x2c,0x8b,0x5c,0x36,0xd4,0xe0,0x2e,0x32,0x85,0x38,0x45,0xc3,0x96,0x7d,0x60,0x41,0x4c,0xdc,0xcc,0x96,0x4c,0x71,0x8e,0x4d,0x5d,0xf0,0x6e,0x0b,0x08,0x14,0x10,0x90,0x51,0x28,0x02,0xed,0xfb,0x9e,0x67,0xe7,0xa9,0xa9,0x29,0xa6,0x6c,0x1f,0xfc,0xf0,0xcf,0xfb,0x42,0xc8,0xef,0x77,0xce,0x79,0x5e,0xce,0x03,0x8c,0x31,0x78,0xd1,0xd4,0xd5,0xd5,0x41,0x4d,0x4d,0x0d,0x1c,0x3d,0x7a,0x14,0x0e,0x1d,0x3a,0x04,0xe5,0xe5,0xe5,0x50,0x5a,0x5a,0x0a,0x4a,0xa5,0x12,0x5e,0x18,0xce,0x73,0xec,0xd8,0xb1,0x67,0xf0,0x8a,0x8a,0x8a,0x67,0xf0,0xbd,0x7b,0xf7,0xfe,0x9f,0x40,0x94,0x22,0xb6,0x05,0x18,0x8d,0x45,0x47,0x8c,0x46,0xff,0x5b,0xc2,0xd4,0x92,0x47,0xc2,0xb8,0xd3,0x80,0x41,0xb7,0xbc,0x5d,0x3f,0x94,0xf4,0xdd,0xc8,0xd0,0xf7,0x89,0xe3,0xfa,0xc7,0x8b,0x04,0x41,0xb0,0xe1,0x7f,0x6f,0x09,0x2f,0x2a,0x2a,0x82,0x82,0x82,0x82,0xff,0x12,0xa0,0x8d,0x28,0x5e,0xdd,0x2e,0x08,0xef,0xde,0x41,0xdc,0xc0,0x10,0xfd,0x29,0xb6,0x0c,0x05,0x60,0x38,0x41,0x19,0x03,0x36,0x39,0xb8,0x70,0x58,0xab,0xf9,0xb8,0x6c,0x70,0xa0,0xd3,0xcd,0x60,0x30,0xc8,0x2c,0xe1,0x85,0x85,0x85,0x90,0x9b,0x9b,0x3b,0xb3,0x00,0xf1,0x4e,0xb4,0x20,0x7c,0xd6,0x81,0x58,0x4c,0x60,0x25,0x8a,0x62,0x36,0x22,0x7a,0xa1,0x28,0xcc,0x46,0x9c,0x02,0x26,0x8e,0x49,0x10,0x87,0x81,0x4d,0x69,0x6d,0x9f,0x68,0xda,0x0a,0x2b,0xfa,0xfb,0xba,0x5e,0x2e,0x2e,0x2e,0x96,0xf1,0xb1,0xf0,0xca,0x39,0x3c,0x27,0x27,0x67,0x26,0xc1,0x80,0xab,0x20,0x1c,0x6c,0x41,0xfc,0x8a,0xe0,0xe7,0x29,0xf5,0x04,0x2f,0xa0,0x04,0x23,0x8a,0xb6,0x26,0x01,0xea,0x25,0x28,0x3e,0x96,0x8a,0xd8,0x0f,0x6c,0x4c,0xbd,0x78,0xe8,0xe6,0x5f,0x27,0xd2,0xf3,0xf3,0xf3,0x17,0xef,0xde,0xbd,0x5b,0x6a,0x86,0x67,0x66,0x66,0x5a,0x17,0x08,0x42,0x7d,0x3e,0x62,0xa6,0x88,0x78,0x9c,0xf2,0x0b,0x55,0xdf,0x40,0xc9,0xa7,0x04,0x51,0x07,0x76,0x24,0x20,0xf8,0xd8,0x53,0x81,0xd8,0x47,0x9d,0x74,0x03,0xbb,0xd3,0x12,0xdb,0x98,0x9e,0x9e,0xee,0xb1,0x73,0xe7,0x4e,0x5b,0x33,0x3c,0x35,0x35,0xd5,0xba,0xc0,0x68,0x4c,0xfd,0x15,0x31,0x95,0x89,0x62,0xb9,0x28,0x8a,0x9f,0x13,0xb8,0x92,0xb2,0x8b,0xde,0x03,0x48,0x30,0x57,0x14,0x27,0x2d,0x05,0xd4,0x45,0x0f,0xb0,0xfe,0x6b,0x6e,0x9d,0x89,0x89,0x89,0x61,0x24,0x59,0x6c,0x86,0x27,0x27,0x27,0xcf,0x24,0xf0,0xe8,0x44,0x0c,0xa4,0xea,0xdf,0xa4,0xb1,0xc4,0x13,0x3c,0x81,0xb2,0x81,0xb2,0x88,0x04,0x52,0x53,0x07,0xc8,0x05,0xc3,0x14,0xde,0x01,0x09,0x46,0x6f,0xda,0x8f,0x6c,0xd9,0xb2,0xe5,0xf5,0x1d,0x3b,0x76,0xb8,0x9a,0xe1,0x24,0x9c,0x41,0x60,0x70,0xd1,0x22,0xda,0x91,0xc0,0x85,0xe6,0xef,0xcc,0x43,0x22,0x07,0x9a,0x3f,0xc1,0x0c,0x24,0x18,0xa7,0x43,0x1e,0x21,0xf8,0x20,0x45,0x4b,0xbf,0xeb,0xa2,0x73,0xb8,0x39,0x7f,0x6c,0xe3,0xc6,0x8d,0x6f,0x24,0x24,0x24,0xb8,0x73,0x78,0x52,0x52,0x12,0xc4,0xc5,0xc5,0x59,0x17,0x18,0xc6,0x97,0x75,0xa3,0x91,0x5a,0x37,0x4a,0x98,0x45,0x10,0x27,0xe9,0x39,0xce,0xab,0xa7,0x43,0x1e,0xe2,0xd5,0xd3,0x88,0xba,0xe8,0xef,0xda,0xa9,0x83,0xbf,0x17,0xe8,0xc2,0xc3,0xc3,0x13,0x63,0x63,0x63,0x97,0x73,0x78,0x7c,0x7c,0x3c,0xc4,0xc4,0xc4,0x58,0x17,0x4c,0xfe,0x23,0x6f,0xc5,0x27,0x04,0x32,0x85,0x60,0xf4,0x14,0x39,0x98,0x7f,0x39,0x23,0x4f,0x3f,0x4f,0xf1,0x11,0xbd,0xf7,0x90,0xa0,0x93,0x04,0x1a,0x60,0x7d,0x57,0x9d,0x7b,0xd7,0xad,0x5b,0x97,0x40,0x50,0x77,0x5e,0x39,0x87,0x53,0x47,0xd6,0x05,0xa3,0x83,0x85,0x15,0xc8,0x41,0x3a,0x1e,0x12,0xe8,0xcc,0xef,0x1c,0x4e,0x19,0x20,0x01,0x8d,0x46,0x7c,0x48,0xd1,0xd0,0xcf,0xf7,0x80,0x5d,0x6f,0x56,0xb4,0x84,0x84,0x84,0x6c,0x8b,0x8a,0x8a,0x5a,0x66,0x86,0x53,0x47,0xd6,0x05,0xba,0xe1,0x56,0xb9,0xbe,0x67,0x85,0x1a,0x07,0x09,0x6e,0x0a,0x41,0xcc,0xa1,0xca,0x51,0x4b,0x82,0x4e,0x0e,0xa7,0xea,0xef,0x03,0xd3,0x5f,0x9f,0xa7,0x7f,0x2f,0x65,0x45,0x75,0x68,0x68,0x68,0x44,0x64,0x64,0xa4,0xa3,0x19,0x4e,0xc2,0x19,0x46,0x34,0x39,0x39,0xa7,0xaf,0xbd,0xb2,0x60,0xe2,0xe1,0x02,0x1d,0xf6,0x92,0xa0,0x97,0xa0,0x3c,0x3d,0xfc,0x40,0x29,0x1d,0x24,0x50,0xd3,0xf3,0xae,0x04,0x0d,0x6d,0xb3,0xa7,0xbe,0x28,0x71,0xfa,0x39,0x54,0xf1,0x5a,0xda,0xfa,0xf5,0xeb,0x7d,0x22,0x22,0x22,0xec,0xcc,0xf0,0xc0,0xc0,0x40,0x80,0x86,0x86,0x06,0x38,0x7d,0xfa,0xf4,0xb4,0x95,0x5b,0x55,0x55,0x25,0x29,0x55,0x15,0x3b,0x5f,0xfc,0xc1,0xf7,0x2c,0x76,0x12,0xac,0x1d,0x50,0xec,0x00,0x0e,0x46,0x54,0x03,0x32,0x35,0x49,0xef,0xd1,0xf3,0x36,0xb0,0xcb,0x67,0x5f,0x52,0x47,0x86,0x7b,0x7e,0x14,0x14,0x14,0x14,0x1e,0x16,0x16,0xb6,0x94,0x62,0xc3,0xe1,0x72,0xb9,0x1c,0xd6,0xae,0x5d,0x0b,0x70,0xea,0xd4,0x29,0x13,0xdc,0x72,0xe5,0xf2,0x7d,0xae,0x3a,0x70,0x40,0x76,0xe6,0x9b,0x64,0xe5,0x93,0xfb,0x5e,0x4c,0x50,0x3b,0xa2,0x70,0x77,0x16,0x8a,0x77,0xc1,0x14,0xe3,0x2d,0x19,0x8e,0xfe,0x39,0x1f,0x1f,0x5c,0x70,0x65,0x95,0x45,0x1e,0xd7,0x02,0xe5,0x7e,0x6f,0x2b,0x14,0x8a,0x57,0xa9,0x83,0x39,0x96,0x70,0x4f,0x4f,0x4f,0x80,0xda,0xda,0x5a,0xab,0x97,0xc5,0xfe,0xfd,0xca,0x59,0xc7,0xbf,0x3e,0xa8,0xd4,0xde,0xaf,0x65,0x53,0xfd,0x65,0x38,0xaa,0xce,0xc0,0xfe,0xeb,0x31,0xd8,0x75,0x25,0x0a,0xdb,0x7e,0x8a,0x61,0x4d,0xdf,0x6e,0x65,0x55,0x07,0x93,0x58,0xea,0x5b,0x9b,0x6f,0xc8,0x03,0xfc,0x93,0x82,0x83,0x83,0x97,0x91,0x44,0xca,0xc7,0x62,0x86,0xaf,0x5c,0xb9,0x12,0x66,0xbc,0x2c,0x68,0xe5,0x4a,0x8f,0x1c,0x3e,0x9c,0xd7,0xa1,0x79,0xc0,0x68,0xe7,0x63,0x4f,0xf7,0x6d,0xfc,0xe3,0x4a,0x33,0xbb,0xd8,0xdc,0x80,0xf5,0x27,0xbf,0x64,0xaa,0xd2,0x7d,0xec,0x83,0x0f,0x73,0xd9,0xb6,0xb8,0xad,0x2d,0x04,0x8c,0xa7,0xaa,0x5d,0x28,0x12,0x4b,0xb8,0x9b,0x9b,0x1b,0x40,0x75,0x75,0xb5,0xd5,0x9b,0x88,0xaf,0x5c,0x92,0x04,0xb7,0xb6,0x5e,0xd5,0xe8,0xc7,0x27,0xd8,0xc3,0xee,0x5e,0xbc,0xd4,0xf2,0x3b,0x9e,0x6b,0xba,0x80,0x27,0x8e,0x9f,0xc4,0x7d,0x25,0x07,0xd8,0xfb,0xbb,0x72,0x26,0x14,0x21,0x8a,0x1a,0x2f,0x2f,0xaf,0x0d,0x7e,0x7e,0x7e,0x4e,0xbe,0xbe,0xbe,0x12,0x4b,0xb8,0x8b,0x8b,0x0b,0xf0,0x03,0x9d,0x76,0x87,0x9a,0x6f,0x22,0xbe,0x72,0xb3,0xb3,0xb3,0xe7,0x96,0x94,0xec,0xcf,0x6a,0x6f,0xd7,0x3c,0xea,0xd6,0x76,0xb3,0x4b,0x97,0x7f,0xc3,0xf3,0xe7,0x9a,0xb1,0xae,0xb6,0x8e,0x7d,0xb2,0xaf,0x78,0x2a,0x72,0x53,0x64,0xe3,0xea,0xd5,0xab,0xdf,0x21,0xa8,0xaf,0xb7,0xb7,0xf7,0xfc,0xe7,0xe1,0x8e,0x8e,0x8e,0x00,0x65,0x65,0x65,0x60,0xed,0x26,0xe2,0x2b,0x37,0x23,0x23,0x43,0x42,0x7b,0x65,0xe1,0x9e,0x3d,0x7b,0xb2,0x1b,0x7f,0x6c,0xbc,0xd1,0xd4,0xd4,0x34,0x7c,0xb6,0xfe,0x8c,0xee,0x53,0x55,0x69,0x57,0x54,0x74,0xf4,0x19,0x82,0x67,0xac,0x59,0xb3,0x46,0x41,0x1d,0x2c,0xf1,0xf0,0xf0,0xb0,0xb1,0x84,0x3b,0x39,0x39,0x81,0xbd,0xbd,0x3d,0x80,0x4a,0xa5,0x9a,0x36,0x16,0xcb,0xcb,0x82,0x6f,0x45,0xda,0x8e,0x36,0xb4,0x5f,0x16,0xd1,0xf7,0xed,0x4d,0x07,0xb8,0x89,0xc6,0x10,0x4f,0xc0,0xed,0x04,0xdc,0x4c,0x15,0xfb,0x53,0x96,0xd2,0xbb,0xec,0xf9,0xca,0x39,0xdc,0xd6,0xd6,0x16,0xa6,0x55,0x9e,0x97,0x97,0x07,0x74,0x61,0x40,0x56,0x56,0x16,0xa4,0xa5,0xa5,0x41,0x4a,0x4a,0x8a,0x79,0x2b,0x4a,0xa3,0xa3,0xa3,0xe7,0xd1,0x3f,0xd0,0x52,0xfa,0x5a,0x5e,0x09,0x08,0x08,0x70,0xa3,0x99,0x3b,0xfb,0xf8,0xf8,0x2c,0x20,0xc1,0xac,0x55,0xab,0x56,0x81,0xbb,0xbb,0x3b,0xb8,0xba,0xba,0x9a,0x2a,0x77,0x70,0x70,0x00,0x3b,0x3b,0x3b,0x90,0xc9,0x64,0xf0,0x2f,0xad,0xf7,0xea,0x2e,0xa9,0xd1,0xe3,0xd7,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGHELP 1473

UBYTE ImgFind[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x06,0x21,0x49,0x44,0x41,0x54,0x78,0x9c,0x7d,0x55,0x69,0x4c,0x54,0x57,0x18,0xfd,0x24,0x41,0x23,0x6d,0x02,0x58,0x20,0xb2,0xc5,0x05,0x50,0x11,0x15,0x54,0x10,0x6a,0x55,0x6c,0x0a,0x12,0x20,0x65,0x31,0xec,0x2a,0x28,0x60,0x05,0x2b,0xb5,0x22,0xb4,0x81,0x5a,0xbd,0x14,0x43,0x90,0x10,0x24,0x11,0x49,0xb4,0x0d,0x84,0xa2,0x56,0xfd,0x31,0xa0,0xf2,0x03,0xb4,0x2e,0xc8,0x94,0x28,0x45,0x84,0xb2,0x0e,0x45,0x06,0x98,0x01,0x04,0x86,0x1d,0x17,0x46,0xf4,0xf6,0xdc,0xd7,0x74,0x82,0x62,0x9d,0xe4,0xe6,0xbe,0xbc,0x37,0x39,0xdf,0x77,0xcf,0x77,0xce,0xb9,0x34,0x3d,0x3d,0x4d,0x53,0x53,0x53,0x34,0x3a,0x3a,0x4a,0x83,0x83,0x83,0xd4,0xdb,0xdb,0x4b,0x9d,0x9d,0x9d,0xa4,0x50,0x28,0xa8,0xa9,0xa9,0x89,0x1e,0x3f,0x7e,0x4c,0x0f,0x1f,0x3e,0x24,0xb9,0x5c,0x4e,0x77,0xef,0xde,0xa5,0x8a,0x8a,0x0a,0x2a,0x2b,0x2b,0xa3,0x92,0x92,0x12,0xba,0x7a,0xf5,0x2a,0x5d,0xb8,0x70,0x81,0x0a,0x0b,0x0b,0xe9,0xfc,0xf9,0xf3,0x94,0x97,0x97,0x47,0xa7,0x4f,0x9f,0xa6,0x53,0xa7,0x4e,0xd1,0xc9,0x93,0x27,0xe9,0xc4,0x89,0x13,0x44,0x93,0x93,0x93,0x1f,0x04,0xaf,0xa9,0xa9,0x99,0x03,0x2e,0x93,0xc9,0x24,0xf0,0x8b,0x17,0x2f,0x4a,0xe0,0xe7,0xce,0x9d,0xd3,0x81,0x67,0x65,0x65,0xe9,0xc0,0x53,0x52,0x52,0x88,0x46,0x46,0x46,0x74,0xe0,0x4a,0xa5,0xf2,0xbd,0xe0,0x37,0x6e,0xdc,0x30,0x8b,0x8a,0x8a,0x4a,0xb0,0xb4,0xb4,0xbc,0x35,0x6f,0xde,0xbc,0xbf,0x89,0x48,0x8d,0xbd,0xc3,0xcc,0xcc,0xec,0x9e,0x97,0x97,0x57,0x6a,0x5a,0x5a,0x9a,0x5d,0x76,0x76,0xb6,0xfe,0x6c,0xf0,0xd4,0xd4,0x54,0x4a,0x4a,0x4a,0x22,0x1a,0x18,0x18,0xf8,0x20,0x2d,0x39,0x39,0x39,0x5e,0x00,0x53,0xb8,0xb9,0xb9,0xf1,0xbd,0x7b,0x23,0x79,0x6c,0x6c,0x34,0xf6,0xbd,0x3c,0x32,0x32,0x92,0x87,0x87,0x87,0x73,0x57,0x57,0x57,0xbe,0x60,0xc1,0x02,0x25,0x9e,0xa3,0x8e,0x1d,0x3b,0x66,0x78,0xfc,0xf8,0x71,0x3d,0x01,0x9e,0x9c,0x9c,0x4c,0x87,0x0f,0x1f,0x26,0x52,0xa9,0x54,0x1f,0x04,0x37,0x37,0x37,0x7f,0x19,0x13,0x13,0xc3,0x53,0x53,0x53,0x06,0x2a,0x2a,0xca,0x64,0xd5,0xd5,0x7f,0xe4,0x57,0x57,0x57,0xe7,0xde,0xb9,0x73,0x27,0x1f,0x34,0xc9,0x00,0xa2,0x09,0x09,0x09,0xe1,0x2b,0x57,0xae,0xd4,0xfa,0xf9,0xf9,0x1d,0x38,0x72,0xe4,0x88,0xf1,0xd1,0xa3,0x47,0xf5,0x04,0xf8,0xc1,0x83,0x07,0xe9,0x7f,0x39,0x17,0xe0,0x8b,0x17,0x2f,0x7e,0x19,0x17,0x77,0x80,0x97,0x96,0x96,0xca,0x7b,0x7a,0xba,0x32,0xc6,0xc6,0x06,0x58,0x57,0x57,0x27,0x6b,0x6e,0x6e,0x66,0x0d,0x0d,0x0d,0xac,0xbe,0xbe,0x9e,0xd5,0xd4,0x3c,0xcc,0x28,0x2a,0x2a,0x92,0xef,0xda,0xb5,0x8b,0xaf,0x5d,0xbb,0x56,0x0b,0xca,0x0e,0xc4,0xc5,0xc5,0x19,0xc7,0xc7,0xc7,0xeb,0xc5,0xc6,0xc6,0xd2,0xff,0x72,0x2e,0x68,0x11,0xe0,0xe5,0xe5,0x65,0x72,0x8d,0x66,0x30,0xed,0xf5,0xeb,0x19,0xc6,0xf9,0x1b,0x36,0x31,0x31,0xc6,0x30,0x2b,0xd6,0xd6,0xd6,0xc6,0x5a,0x5a,0x5a,0x58,0x63,0x63,0xa3,0x28,0x94,0x76,0xe9,0xd2,0x25,0xa9,0x88,0x91,0x91,0x91,0x2a,0x30,0x30,0xd0,0x1d,0x14,0x1a,0x62,0x6e,0x44,0xf8,0xc3,0x1c,0xb5,0xe0,0x63,0xc2,0xa6,0x4d,0x9b,0x78,0x4a,0xca,0xf7,0x03,0x4a,0xa5,0x22,0x63,0x66,0x66,0x1a,0xe0,0x5c,0x5a,0x6f,0xde,0xbc,0x66,0x1a,0xcd,0x90,0x54,0xa4,0xbd,0xbd,0x9d,0xb5,0xb6,0xb6,0x4a,0x27,0xaa,0xab,0xab,0xcb,0x48,0x4c,0x4c,0xd4,0xac,0x5b,0xb7,0x8e,0xdb,0xd9,0xd9,0xe5,0xed,0xdc,0xb9,0x73,0x79,0x58,0x58,0xd8,0x7c,0xc2,0x87,0x39,0x52,0x14,0x6a,0xc1,0x47,0x7e,0xed,0x5a,0xa9,0xac,0xa7,0xa7,0x9b,0xc1,0x2b,0xba,0x02,0x33,0x33,0x33,0x6c,0x6c,0x6c,0x88,0xa9,0xd5,0xdd,0x28,0xf2,0x84,0x75,0x74,0xb4,0xa3,0x90,0x02,0x27,0x6a,0x65,0x25,0x25,0x32,0x99,0x8f,0x8f,0x0f,0x37,0x30,0x30,0x78,0x0c,0xaa,0x5c,0x30,0x13,0x63,0x7a,0xf0,0xe0,0x81,0x0e,0xfc,0xe6,0xcd,0x9b,0x92,0xce,0x85,0x14,0x85,0x52,0x2a,0x2b,0x2b,0xf3,0x15,0x8a,0x36,0xa9,0xe3,0x57,0xaf,0xb4,0x4c,0xab,0x7d,0xc5,0xc6,0xc7,0xc7,0x59,0x7f,0x7f,0x3f,0x53,0xa9,0xba,0xb1,0x94,0xac,0xa7,0x47,0xc9,0xba,0xbb,0x3b,0x51,0xac,0x83,0xd5,0xd6,0xd6,0xe4,0x47,0x46,0xee,0xe1,0x90,0x71,0xef,0xb6,0x6d,0xdb,0xbe,0xd8,0xb1,0x63,0x87,0x15,0x55,0x55,0x55,0xcd,0x71,0xa8,0xd0,0xb9,0x90,0x61,0x55,0xd5,0xfd,0x5c,0xc1,0x71,0x67,0xe7,0x13,0x36,0x34,0xf4,0x14,0x4b,0xc3,0xfa,0xfa,0x04,0xb8,0x4a,0x5a,0x6a,0x75,0x2f,0xeb,0xeb,0xef,0x43,0xc1,0x5e,0xd6,0xdb,0xab,0xc6,0x4c,0x9a,0x73,0xa3,0xa3,0xf7,0x89,0x02,0x1a,0xc8,0xfa,0x4b,0x77,0x77,0x77,0x1b,0x82,0xdc,0xe6,0xd8,0x5f,0x98,0x68,0xf7,0xee,0xdd,0xfc,0xf6,0xed,0xdb,0xf9,0x42,0x29,0x6d,0x6d,0x2d,0x28,0xd2,0x81,0x4e,0xbb,0x75,0xe0,0xe2,0x14,0xc3,0xc3,0x43,0x6c,0x74,0x74,0x04,0xfb,0x30,0x8a,0x0f,0x61,0x1e,0x2d,0xf9,0x31,0xb1,0x31,0xa2,0x40,0xff,0x86,0x0d,0x1b,0x02,0x36,0x6f,0xde,0x6c,0x4b,0xe5,0xe5,0xe5,0x3a,0xfb,0x5f,0xb9,0x72,0x45,0xca,0x16,0x53,0x53,0xd3,0x7b,0x42,0xdb,0x97,0x2f,0x5f,0x96,0xd5,0xd5,0x3d,0xc2,0x10,0x1b,0x99,0xa0,0x0a,0x92,0x86,0x4c,0xbb,0xd0,0xb9,0x5a,0x02,0x9f,0x9a,0x1a,0x67,0x2f,0x5e,0x3c,0x63,0xcf,0x9e,0x4d,0xb1,0x89,0xf1,0x09,0x06,0x4a,0x65,0xc2,0x33,0x30,0x5e,0xa3,0xa3,0xa3,0xa3,0x1f,0x84,0x62,0x23,0x24,0xa9,0xeb,0xfc,0xbf,0x6c,0x11,0xf6,0x77,0x72,0x72,0xe2,0x09,0x09,0x09,0x1a,0xcc,0x28,0xa3,0xb1,0xf1,0x2f,0xd1,0x1d,0x06,0xda,0x01,0xae,0xbb,0x40,0x53,0x1f,0x3a,0x1f,0x66,0x2f,0x5f,0x4c,0x61,0xe8,0x5a,0x2c,0x69,0x36,0x19,0x99,0x99,0x99,0x43,0xe0,0x9e,0xa3,0xc1,0x5f,0xa1,0x26,0xcf,0x8d,0x1b,0x37,0x5a,0xcd,0x09,0x2e,0x91,0x8a,0x22,0x5b,0x40,0x93,0x32,0x38,0x38,0x98,0xe3,0xbd,0xbc,0xa1,0xa1,0x3e,0xad,0xa9,0xa9,0x19,0xa7,0x50,0x48,0xa7,0x40,0xb4,0x48,0xb4,0x3c,0x7f,0x3e,0xc5,0xfe,0xf5,0x07,0x4f,0x43,0x23,0x72,0x64,0x0f,0xb7,0xb6,0xb6,0xee,0x5b,0xb1,0x62,0xc5,0x57,0x30,0x9d,0xeb,0xfa,0xf5,0xeb,0x8d,0x75,0xb4,0xcc,0x8e,0x5c,0x04,0xd7,0xfc,0x5b,0xb2,0x42,0x0e,0x27,0x73,0x51,0x44,0x38,0xf5,0xd1,0xa3,0x47,0x19,0x42,0xf3,0xe2,0x14,0x6a,0x95,0x9a,0x69,0xc0,0xf9,0xc4,0xc4,0x04,0x68,0x9a,0xcc,0xb8,0x7f,0xff,0xbe,0x1c,0xd9,0xc3,0x61,0x30,0xed,0xb2,0x65,0xcb,0xb2,0xec,0xed,0xed,0xbd,0x51,0xc0,0x06,0xa7,0x98,0x4f,0xc5,0xc5,0xc5,0x73,0x22,0xf7,0x41,0x59,0x11,0x17,0xbf,0x6b,0xbf,0x15,0x70,0x78,0x82,0x0b,0x4f,0x08,0x13,0xe1,0xb4,0xb2,0x3f,0x6b,0x6a,0xf2,0x61,0xac,0x5c,0x14,0xcb,0xaf,0x92,0x57,0xc9,0x90,0xfd,0x43,0x88,0x65,0x11,0x7c,0x5a,0xe4,0x51,0xd6,0xaa,0x55,0xab,0xfc,0x57,0xaf,0x5e,0xed,0xb8,0x66,0xcd,0x1a,0x43,0xec,0x44,0x05,0x05,0x05,0x6f,0x5d,0x16,0xbf,0x27,0x5a,0xf1,0xb3,0xa1,0x26,0xbc,0xb6,0xb6,0x96,0x5f,0xbf,0x7e,0x9d,0xa7,0xff,0x90,0xcc,0x17,0x2d,0x32,0x7e,0x1a,0x1a,0x1a,0xca,0xf7,0xef,0xdf,0xcf,0xa3,0xa3,0xa3,0xa5,0x1d,0x79,0xc3,0x05,0x25,0x7b,0xf6,0xec,0xe1,0x98,0x59,0x9f,0xad,0xad,0x6d,0x36,0x0a,0x04,0x38,0x38,0x38,0x38,0x61,0x2d,0xc2,0x29,0xf4,0xf0,0x8e,0xde,0xea,0xbc,0x32,0x79,0x39,0xff,0x25,0xda,0x8a,0x67,0x86,0x98,0xf3,0xef,0xfc,0x2c,0x38,0xbe,0xf1,0xad,0x5b,0xb7,0x26,0x62,0x70,0xdf,0xd8,0xd8,0xd8,0xfc,0xbc,0x70,0xe1,0xc2,0x7a,0x48,0xb0,0x4f,0xe8,0x5c,0x48,0x11,0x6a,0x69,0xc2,0x40,0x8b,0x11,0x0d,0xf1,0x00,0xf4,0x05,0xb0,0xe3,0x6c,0xf0,0x25,0x4b,0x96,0x10,0x9d,0x39,0x73,0x46,0x02,0x3f,0x1b,0x61,0xcd,0xc5,0xfa,0xc9,0xdf,0x8a,0x7f,0xbb,0xc3,0x94,0x87,0xbb,0x98,0xf0,0x2d,0x5b,0xb6,0x24,0xc1,0x8d,0xc1,0xfe,0xfe,0xfe,0x2e,0x58,0x0e,0x78,0x76,0xc3,0x3b,0x2f,0xc8,0xcf,0x1f,0x3a,0x0f,0x84,0x14,0xfd,0xc1,0xb5,0x17,0xd6,0xa7,0x58,0xb6,0xa0,0xc5,0x68,0x36,0xb8,0x85,0x85,0x05,0x89,0x58,0x96,0xee,0xd0,0x9c,0x20,0x4b,0xfe,0xa3,0xc7,0x27,0xfc,0xeb,0xcf,0x8c,0x78,0xd8,0x86,0x8f,0x75,0xe0,0x41,0x41,0x41,0xf6,0x48,0xc9,0x8f,0x30,0x07,0x7d,0x14,0x31,0x16,0xf6,0x17,0x0e,0x15,0x26,0x12,0x3a,0x17,0x52,0x84,0xa4,0x8d,0x31,0x50,0x7d,0xc1,0xf9,0x6c,0x70,0x13,0x13,0x13,0x22,0x68,0x57,0xba,0xe6,0x30,0x44,0x8b,0x84,0xcf,0x4d,0x78,0x84,0xb3,0xd1,0xbb,0xe0,0x06,0x11,0x11,0x11,0x04,0x35,0x51,0x40,0x40,0x00,0x79,0x7b,0x7b,0x93,0x87,0x87,0x07,0xa1,0x08,0xa1,0x08,0x39,0x3b,0x3b,0x13,0x4e,0x42,0xef,0x82,0x83,0x3a,0x32,0x34,0x34,0x24,0x4a,0x4f,0x4f,0x97,0xee,0x50,0x0c,0xcc,0x60,0xdf,0xbe,0x7d,0xf6,0xe8,0xd2,0xc7,0xd7,0xd7,0xd7,0x67,0x36,0x38,0x9e,0x09,0xef,0xe7,0x80,0xbb,0xb8,0xb8,0xbc,0x17,0x5c,0x74,0x2e,0xc0,0x31,0x33,0xd2,0x5d,0xd0,0xe2,0x0e,0x3d,0x74,0xe8,0x90,0x01,0x52,0xd4,0x02,0xc0,0x16,0xc8,0x22,0x03,0xec,0xba,0xce,0x11,0xc3,0xe4,0xe9,0xe9,0x49,0xdb,0xb7,0x6f,0x97,0xc0,0x41,0x0f,0x81,0x1a,0xc2,0x50,0x09,0x43,0xa6,0xa5,0x4b,0x97,0x8a,0x98,0x97,0x3a,0xc7,0xa5,0x43,0x88,0x6c,0xd2,0xd7,0xd7,0xa7,0x7f,0x00,0xf1,0x2f,0x58,0x48,0x5c,0x65,0x78,0x87,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGFIND 1668

UBYTE ImgRetrace[]={0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x18,0x08,0x06,0x00,0x00,0x00,0xe0,0x77,0x3d,0xf8,0x00,0x00,0x00,0x09,0x70,0x48,0x59,0x73,0x00,0x00,0x1e,0xc2,0x00,0x00,0x1e,0xc2,0x01,0x6e,0xd0,0x75,0x3e,0x00,0x00,0x00,0x09,0x74,0x45,0x58,0x74,0x43,0x6f,0x6d,0x6d,0x65,0x6e,0x74,0x00,0x00,0x89,0x2a,0x8d,0x06,0x00,0x00,0x04,0x02,0x49,0x44,0x41,0x54,0x78,0x9c,0xad,0x95,0x5b,0x48,0x9b,0x67,0x1c,0xc6,0x9f,0x5d,0x84,0x61,0x68,0x51,0x86,0xde,0x44,0x86,0x4e,0x8c,0x8a,0x5a,0x94,0x79,0xc4,0x53,0xa2,0x78,0x8c,0x13,0x8f,0xd1,0x44,0xe7,0x22,0xa8,0xcb,0xa6,0x64,0x91,0x1a,0xc9,0x22,0x96,0x04,0xcc,0xa8,0x5a,0xa6,0x63,0x8a,0x43,0x69,0xb5,0x9b,0xda,0xa1,0x63,0xca,0x50,0x5a,0x23,0xab,0x0a,0x91,0x4d,0x65,0xe8,0xa8,0x6e,0x20,0x0c,0x1d,0x88,0x0c,0xbd,0xd0,0x0b,0xbd,0xf1,0xea,0xdd,0xff,0x0d,0x7c,0x21,0x89,0x49,0x6c,0xa1,0x81,0x8f,0x04,0xf2,0x7d,0xbf,0xe7,0x79,0x9f,0xff,0xe1,0xc3,0xf5,0xf5,0x35,0xae,0xae,0xae,0x70,0x71,0x71,0x81,0xb3,0xb3,0x33,0x9c,0x9c,0x9c,0xe0,0xf0,0xf0,0x10,0x07,0x07,0x07,0xd8,0xdf,0xdf,0xc7,0xee,0xee,0x2e,0xb6,0xb6,0xb6,0xb0,0xb1,0xb1,0x81,0xb5,0xb5,0x35,0xd8,0x6c,0x36,0x2c,0x2d,0x2d,0x61,0x61,0x61,0x01,0x73,0x73,0x73,0x98,0x9e,0x9e,0xc6,0xe4,0xe4,0x24,0xc6,0xc7,0xc7,0x31,0x32,0x32,0x82,0xa1,0xa1,0x21,0xf4,0xf7,0xf7,0xc3,0x6a,0xb5,0xc2,0x6c,0x36,0x03,0x97,0x97,0x97,0x7e,0xe1,0xdb,0xdb,0xdb,0x37,0xe0,0xf3,0xf3,0xf3,0x0e,0xf8,0xcc,0xcc,0x8c,0x03,0x3e,0x36,0x36,0xe6,0x84,0x0f,0x0c,0x0c,0x38,0xe1,0x26,0x93,0x09,0x38,0x3f,0x3f,0x77,0xc2,0x8f,0x8e,0x8e,0x7c,0xc2,0x57,0x56,0x56,0x9c,0xce,0x67,0x67,0x67,0x9d,0x70,0x57,0xe7,0xae,0xf0,0xee,0xee,0x6e,0x18,0x0c,0x06,0xe0,0xf4,0xf4,0xf4,0xad,0xc7,0xc2,0xe1,0x5d,0x5d,0x5d,0xd0,0xeb,0xf5,0xc0,0xf1,0xf1,0xf1,0x1b,0xc3,0x27,0x26,0x26,0xdc,0x62,0x71,0x85,0xf3,0x58,0xb8,0x73,0x0e,0x6f,0x6b,0x6b,0x83,0xdf,0xcc,0x5d,0x63,0xf1,0x95,0xb9,0xa7,0x73,0x57,0x78,0x4b,0x4b,0x0b,0x5e,0x1b,0xee,0x2d,0x16,0xcf,0xcc,0x85,0x58,0x04,0xb8,0x46,0xa3,0x01,0xf6,0xf6,0xf6,0x7c,0x76,0x8b,0x50,0xd0,0x27,0xb3,0x3f,0x44,0x58,0x7f,0x19,0xb5,0x7c,0xba,0xde,0xcf,0x9a,0x5f,0x3e,0x64,0x9f,0xd8,0x7a,0x59,0xfd,0x73,0x0b,0x53,0x2e,0x3e,0x60,0x9f,0x7d,0xd7,0x63,0xe9,0xb4,0x98,0x22,0x3c,0x9d,0x73,0x78,0x7d,0x7d,0x3d,0xb0,0xb3,0xb3,0xe3,0x13,0xce,0x9d,0x3f,0x7a,0xf1,0xd8,0xa2,0xff,0xed,0x1b,0x66,0x3c,0x78,0xc2,0xee,0xff,0x35,0xc6,0xbe,0x78,0x35,0xca,0x3e,0xdf,0xf9,0x96,0xb5,0x6c,0x0f,0x32,0xcd,0xef,0x8f,0x58,0x83,0xfd,0x21,0x2b,0xfb,0xd9,0xc8,0x54,0x7d,0xba,0x01,0xad,0x56,0xfb,0x6e,0x73,0x73,0xf3,0x3b,0x02,0xbc,0xa6,0xa6,0x06,0xd8,0xdc,0xdc,0x74,0x8b,0xc5,0xfa,0xf7,0x53,0x66,0x79,0xf5,0x98,0x71,0xb8,0xf1,0x8f,0x51,0x66,0xf9,0x77,0x9a,0xf5,0xfc,0xf3,0xd4,0xaf,0x40,0xed,0xba,0x95,0x55,0xfe,0x6a,0x61,0xf9,0xcf,0x3a,0x58,0x5d,0x5d,0x5d,0xb0,0x4a,0xa5,0x12,0x29,0x95,0x4a,0x94,0x97,0x97,0x03,0x76,0xbb,0xdd,0xcd,0x39,0x17,0x18,0x3a,0x9b,0x67,0x3d,0x7f,0x8e,0xb3,0xaf,0x8e,0x7f,0x7c,0x23,0x81,0x8f,0x6c,0x0f,0x58,0x6e,0xa7,0xea,0x7b,0x85,0x42,0xf1,0x41,0x59,0x59,0x99,0xb8,0xa4,0xa4,0x04,0x58,0x5d,0x5d,0x75,0x8b,0x45,0x10,0x18,0xf8,0xef,0xa7,0x1b,0x02,0xde,0x6a,0xe0,0x29,0x90,0xd5,0x59,0xf3,0x3c,0x3d,0x3d,0x5d,0x21,0x93,0xc9,0x22,0xf2,0xf2,0xf2,0x44,0x58,0x5e,0x5e,0x76,0x8e,0x3f,0x2f,0xa8,0x2f,0x81,0x76,0xfb,0xd7,0xac,0xc6,0xaa,0xb5,0x67,0x65,0x65,0x99,0x33,0x32,0x32,0xcc,0x69,0x69,0x69,0xe6,0xc2,0x2f,0x3f,0xb6,0x57,0x2d,0x98,0x9c,0x02,0x99,0xf7,0xab,0xd7,0xa3,0xa3,0xa3,0xcd,0x71,0x71,0x71,0xda,0x94,0x94,0x94,0x34,0xba,0xf7,0x0e,0x16,0x17,0x17,0xdd,0xfa,0xdc,0x9b,0x00,0x2f,0x32,0x39,0x32,0x67,0x67,0x67,0x9b,0xe9,0xa1,0x6e,0xfa,0xad,0x23,0x77,0x1a,0xba,0xd4,0x99,0x99,0x99,0x1a,0x79,0x87,0xf2,0x45,0x8e,0xa1,0xf6,0x65,0x4c,0x4c,0x8c,0x39,0x2a,0x2a,0xca,0x14,0x1b,0x1b,0xab,0xf9,0x90,0x3e,0x64,0xe2,0xce,0x8d,0xc5,0xe5,0xef,0x04,0x72,0xb9,0xdc,0x50,0x58,0x58,0xd8,0x48,0xc5,0x93,0x57,0x55,0x55,0xdd,0xab,0xac,0xac,0x94,0x52,0xde,0xf7,0x48,0x50,0x9e,0x94,0x94,0xa4,0x8a,0x8f,0x8f,0xd7,0x72,0x78,0x42,0x42,0x82,0x2c,0x39,0x39,0x59,0x42,0x97,0xc8,0x11,0x8b,0xeb,0x10,0x09,0x5d,0xc4,0x8b,0xcc,0xbb,0xa8,0x73,0x6b,0xd8,0x71,0x02,0x2e,0x90,0x93,0x93,0xa3,0x2f,0x2d,0x2d,0x2d,0xa1,0x2e,0x79,0xbf,0xb1,0xb1,0x51,0xc4,0x5b,0xb1,0xba,0xba,0x5a,0x44,0xc5,0x7c,0x8f,0xfe,0x8b,0xe6,0xb1,0x70,0xe7,0x1c,0x4e,0x82,0x22,0x12,0x02,0xa6,0xa6,0xa6,0x6e,0x8c,0x3f,0x4d,0xa4,0xb4,0xb6,0xb6,0x56,0xcd,0x63,0x71,0xcd,0x9c,0xbe,0x0d,0x45,0x45,0x45,0x0a,0x6a,0x41,0x89,0x5a,0xad,0x76,0xf4,0x39,0x6f,0x45,0xde,0x2d,0xbc,0xa0,0x74,0xef,0x5d,0xba,0xef,0xae,0x00,0xa7,0xd3,0xc0,0xb1,0xb8,0x3c,0xb7,0xa2,0xd1,0x68,0x94,0xd0,0xb0,0xc8,0xe8,0x61,0x35,0x39,0x56,0x17,0x17,0x17,0xab,0x0b,0x0a,0x0a,0xd4,0xb9,0xb9,0xb9,0x6a,0x12,0x90,0x51,0x34,0x12,0xa1,0xcf,0x39,0x3c,0x3f,0x3f,0x1f,0x64,0x06,0x64,0x00,0x74,0x0a,0x08,0xf0,0xc8,0xc8,0x48,0x78,0x7d,0x59,0xd0,0x5e,0x11,0xeb,0x74,0x3a,0x09,0x8d,0xbc,0x94,0x84,0xa4,0x14,0x85,0x94,0x80,0x52,0x9e,0x39,0x87,0xd3,0x25,0xae,0xa8,0xa8,0x00,0xe5,0xef,0x17,0x1e,0x16,0x16,0x06,0x0c,0x0f,0x0f,0x7b,0x7d,0x13,0xf9,0xda,0x2d,0xae,0xb1,0xdc,0x06,0x97,0x48,0x24,0xc0,0xe0,0xe0,0xe0,0xad,0x2b,0x57,0x80,0xfb,0x8a,0x85,0x8a,0xea,0x15,0x1e,0x1c,0x1c,0x0c,0xf4,0xf5,0xf5,0xdd,0xba,0x72,0x05,0x38,0x8f,0xe5,0x75,0xe1,0x21,0x21,0x21,0x08,0x0c,0x0c,0x04,0x7a,0x7b,0x7b,0xdf,0x7a,0x2c,0xdc,0x39,0x87,0x07,0x04,0x04,0xc0,0xcd,0x79,0x47,0x47,0x07,0xda,0xdb,0xdb,0xd1,0xda,0xda,0x8a,0xa6,0xa6,0x26,0x34,0x34,0x34,0x38,0x9d,0xf3,0x82,0x52,0x27,0x81,0x86,0xcd,0x01,0x4f,0x4d,0x4d,0x45,0x62,0x62,0x22,0x68,0x2d,0x40,0x2a,0x95,0x22,0x3c,0x3c,0x1c,0xa1,0xa1,0xa1,0x0e,0xe7,0x41,0x41,0x41,0x10,0x8b,0xc5,0x10,0x89,0x44,0xf8,0x1f,0x72,0x7b,0xcb,0xf7,0x4b,0x16,0x43,0x00,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82};

#define LEN_IMGRETRACE 1125

//<

//> STRPTR entity[96] = {...};
/*
** This are the full names of all HTML entities
*/
STRPTR entity[96] =
{
  "nbsp","iexcl","cent","pound","curren","yen","brvbar","sect","uml",
  "copy","ordf","laquo","not","shy","reg","macr","deg","plusmn","sup2",
  "sup3","acute","micro","para","middot","cedil","sup1","ordm","raquo",
  "frac14","frac12","frac34","iquest","Agrave","Aacute","Acirc","Atilde",
  "Auml","Aring","AElig","Ccedil","Egrave","Eacute","Ecirc","Euml","Igrave",
  "Iacute","Icirc","Iuml","ETH","Ntilde","Ograve","Oacute","Ocirc","Otilde",
  "Ouml","times","Oslash","Ugrave","Uacute","Ucirc","Uuml","Yacute","THORN",
  "szlig","agrave","aacute","acirc","atilde","auml","aring","aelig","ccedil",
  "egrave","eacute","ecirc","euml","igrave","iacute","icirc","iuml","eth",
  "ntilde","ograve","oacute","ocirc","otilde","ouml","divide","oslash",
  "ugrave","uacute","ucirc","uuml","yacute","thorn","yuml"
};
//<
//> STRPTR agpens[] , htmlpens[]
/*
** This are the pen names in AmigaGuide documents
*/
STRPTR agpens[] =
{
  "text}","shine}","shadow}","fill}","filltext}","background}","highlight}",
  NULL
};

/*
** And this are the appropriate HTML colors
superceded by param.colours[]
STRPTR htmlpens[] =
{
  "#000000","#ffffff","#000000","#3864a0","#000000","#949494","#ffffff"
};
*/

STRPTR cssclass[] =
{
  "agtext","agshine","agshadow","agfill","agfilltext","agbackground","aghighlight"
};
//<

/* Some globals for macro support */
char *macro[20];
char *macrotext[20];
int macros=0;

#ifndef __HAIKU__
/* <OS4 needs this to be set (OS4 is happy with NULL) */
struct Locale *locale = NULL;

#ifndef __amigaos4__
typedef   signed long  int32;
typedef   signed long long  int64;
#endif
#endif

int32 seek64(BPTR file, int64 posn, int32 mode)
{
#ifdef __amigaos4__
	return ChangeFilePosition(file, posn, mode);	
#else
	long oldposn = Seek(file, (long)posn, mode);
	if(oldposn == -1) return 0;
		else return 1;
#endif
}

#ifndef __HAIKU__
char *mystrlwr(string)
unsigned char *string;
{
    unsigned char *p=string;

    while ((*p = ConvToLower(locale,*p) ) != '\0') {
        ++p;
    }
    return(string);
}

#ifndef __amigaos4__
char *strdup(const char *old)
{
        char *t = malloc(strlen(old)+1);
        if (t != 0)
                strcpy(t, old);
        return t;
}
#endif

#ifndef __amigaos4__
static const ULONG REGARGS AslGuideHook(__reg("a0") struct Hook *mh,__reg("a2") struct FileRequester *fr,__reg("a1") struct AnchorPath *ap)
#else
static const ULONG AslGuideHook(struct Hook *mh,struct FileRequester *fr,struct AnchorPathOld *ap)
#endif
{
  BPTR file = 0;
  char buffer[10];
  char fname[1024];

  if(ap->ap_Info.fib_DirEntryType > 0) return(TRUE);

  strcpy(fname,fr->fr_Drawer);
  AddPart(fname,ap->ap_Info.fib_FileName,1024);

  if(file = Open(fname,MODE_OLDFILE))
	{
		if(Read(file,&buffer,9))
		{
			Close(file);
			if(!Strnicmp(buffer,"@database",9))
			{
				return(TRUE);
			}
			else
			{
				return(FALSE);
			}
		}
	}

	if(file) Close(file);
  return(FALSE);
} 
#endif

//> SaveImg()
/*------------------------------------------------------------*
*   SaveImg()        Saves one image                          *
*     -> file File Name                                       *
*     -> data Data buffer                                     *
*     -> len  Length                                          *
*     <-      Success                                         *
*/
LONG SaveImg(STRPTR file, UBYTE *data, ULONG len)
{
  BPTR lock;

#ifndef __HAIKU__
  if(lock = Lock(file,ACCESS_READ))       // Already existing?
  {
    UnLock(lock);
    return(1);                            // then leave with success
  }
#endif

  if(param.verbatim) printf("Creating image '%s'\n",file);  // Report

  if(lock = Open(file,MODE_NEWFILE))      // Open the image file
  {
    if(Write(lock, (void *)data, (LONG)len) == -1)        // Write the image
    {
      Close(lock);                        // Write failed, so
      return(0);                          // return without success
    }
    Close(lock);                          // Write succeeded, so
    return(1);                            // return with success
  }
  return(0);                              // No lock, no success
}
//<
//> SaveImages()
/*------------------------------------------------------------*
*   SaveImages()     Saves all required images                *
*     <-      Success                                         *
*/
LONG SaveImages(void)
{
  for(;;)
  {
    if(param.homeurl)
      if(!SaveImg("home.png" ,ImgHome ,LEN_IMGHOME )) break;
    if(param.retrace)
      if(!SaveImg("retrace.png" ,ImgRetrace ,LEN_IMGRETRACE )) break;
    if(param.help)
      if(!SaveImg("help.png" ,ImgHelp ,LEN_IMGHELP )) break;
    if(param.findurl)
      if(!SaveImg("find.png" ,ImgFind ,LEN_IMGFIND )) break;

//    if(*Index)
      if(!SaveImg("index.png",ImgIndex,LEN_IMGINDEX)) break;

    if(!SaveImg("next.png" ,ImgNext ,LEN_IMGNEXT )) break;
    if(!SaveImg("prev.png" ,ImgPrev ,LEN_IMGPREV )) break;
    if(!SaveImg("toc.png"  ,ImgTOC  ,LEN_IMGTOC  )) break;
    return(1);
  }
  return(0);
}
//<

//> CopyLink()
/*------------------------------------------------------------*
*   CopyLink()       Copies a link file name into a variable  *
*     -> link Link                                            *
*     -> var  String-Var                                      *
*/
void CopyLink(STRPTR link, STRPTR var)
{
  char ch;
  char *orig_var = var;
  char strconvtmp[100];
	char *tmp=NULL;

  var[0] = '\0';                          // Init

if(param.linkadd && !(param.singlefile && strchr(link,'/')))
{
	strcpy(var,param.linkadd);
	var=var+strlen(param.linkadd);
}

if(param.dotdotslash && !param.singlefile && strchr(link,'/'))
{
	strcat(var,"../");
	var+=3;
}

  while(*link && *link==' ') link++;      // Suppress spaces
  if(!*link) return;                      //   there were only spaces!

  if(*link != '\"')
  {                                       // Quick Mode
    while((ch = *link++)!=' ' && ch!='\n')
    {
      if(ch==':') ch='/'; // colon to slash again
      *var++ = ch;
    }
  }
  else
  {                                       // Quote Mode
    link++;                               //   we had a leading quote
    if(!*link) return;
    while((ch = *link++)!='\"' && ch!='\n')
    {
		if(ch=='/' && param.singlefile)
		{
			*var++ = '.';                           // Add html suffix
  			*var++ = 'h';
  			*var++ = 't';
  			*var++ = 'm';
  			if(!param.msdos) *var++ = 'l';
			ch = '#';
		}

      if(ch==' ') ch = '_'; // space converts to underscore
      if(ch==':') ch = '/'; // colon converts to slash
      *var++ = ch;
    }
  }

	if(!param.singlefile)
	{
  		*var++ = '.';                           // Add html suffix
  		*var++ = 'h';
  		*var++ = 't';
  		*var++ = 'm';
  		if(!param.msdos) *var++ = 'l';
	}
  *var = '\0';

#ifndef __HAIKU__
  StrConvert(locale, orig_var, strconvtmp, 100, SC_COLLATE1);
  mystrlwr(strconvtmp);
  strcpy(orig_var, strconvtmp);
#endif
}
//<
//> MyPutCh()
/*------------------------------------------------------------*
*   MyPutCh()       Writes single chars into HTML             *
*     -> fh   output file handle                              *
*     -> ch   Amiga char to be written                        *
*     <-      -1 for failure                                  *
*/
LONG MyPutCh(BPTR fh, unsigned char ch)
{
  if(ch<' ' && ch!='\n')                // Is it a control char?
  {
    return FPrintf(fh,"&#%03ld;",ch);   //   yes: return them as number
  }
  else if(ch < 128)                     // Is it an ASCII char?
  {
    if(ch=='\"') return FPuts(fh,"&quot;");     // Convert some exceptions
    else if(ch=='&') return FPuts(fh,"&amp;");
    else if(ch=='<') return FPuts(fh,"&lt;");   // #060
    else if(ch=='>') return FPuts(fh,"&gt;"); // #062
    else return FPutC(fh,ch);                   // else put char directly
  }
  else                                  // Chars >127 ?
  {
    if(ch>=160)                         // Do we have an entity name?
      return FPrintf(fh,"&%s;",entity[ch-160]); // yes: use this one
    else
      return FPrintf(fh,"&#%03ld;",ch);         // no: use it's number
  }
}
//<
//> MyPuts()
/*------------------------------------------------------------*
*   MyPuts()        Writes a string into HTML                 *
*     -> fh   Output file handle                              *
*     -> str  String to be output                             *
*     <-      -1 for failure                                  *
*/
LONG MyPuts(BPTR fh, STRPTR str)
{
  unsigned char ch;

  while(ch = *str++)                        // go through the string
  {
    if(ch=='\"' || ch=='<' || ch=='>' || ch=='&')      // do NOT convert them
    {                                       //  since they are also used
      if(-1 == FPutC(fh,ch)) return(-1);    //  in HTML tags
    }
    else
    {
      if(-1 == MyPutCh(fh,ch)) return(-1);
    }
  }
  return(0);
}

LONG MyPutHex(BPTR fh, ULONG num)
{
    return FPrintf(fh,"%06lx",num);
}
//<
//> ConvLine()
/*------------------------------------------------------------*
*   ConvLine()      Converts an AmigaGuide line               *
*     -> fh   output file handle                              *
*     -> buf  Source buffer                                   *
*     -> linenr line number                                   *
*     <-      Success                                         *
*/
LONG ConvLine(BPTR fh, STRPTR buf, ULONG linenr,ULONG nodelinenr,STRPTR filename)
{
  unsigned char ch;
  char linkstr[100], linkstr2[100];
  register UWORD dolink  = !param.nolink;
  register UWORD doemail = !param.noemail;
  ULONG linkline;

	if(param.numberlines)
	{
		if(!param.singlefile)
		{
			sprintf(linkstr,"<a name=\"%ld\"></a>",linenr-nodelinenr);
		}
		else
		{
			sprintf(linkstr,"<a name=\"%s_line%ld\"></a>",filename,linenr-nodelinenr);
		}

		if(-1 == FPuts(fh,linkstr)) return(0);
	}

if((param.smartwrap) && (strlen(buf)<2))
	{
	if(-1 == FPuts(fh,"<br>")) return(0);
	}

  while(ch = *buf++)                      // while chars available
  {
    if('\\'==ch)                          //   backslash escape character?
    {
      switch(*buf)
      {
        case '\\':                        // '\\' -> '\'
        case '@':                         // '\@' -> '@'
          if(-1 == MyPutCh(fh,*buf)) return(0);
          buf++;
          break;
      }
      continue;
    }

    if(doemail)                           // email recognition activated?
    {
      if(  (ch>='a' && ch<='z')           // Could this be a valid email?
         ||(ch>='A' && ch<='Z')
         ||(ch>='0' && ch<='9'))
      {
        UWORD length   = 0;   // length of the email string
        UWORD atfound  = 0;   // how many '@' were found?
        UWORD charcnt  = 0;   // how many characters in total?
        UWORD dotfound = 0;   // how many dots?
        UWORD atdot    = 0;   // how many dots after the '@'?
        UWORD chardot  = 0;   // how many chars after the last dot?
        STRPTR index   = --buf;
        UWORD i;
        unsigned char ech;

        while(ech = *index++)
        {
          if(   !(ech>='a' && ech<='z')   // still a valid email char?
             && !(ech>='A' && ech<='Z')
             && !(ech>='0' && ech<='9')
             && !(ech=='@' || ech=='.' || ech=='_' || ech=='-')) break;
          length++;
          if(ech=='@')                    // count '@'
          {
            if(*index=='{')               // is it an AmigaGuide attribute?
            {
              length--;                   //   exclude the '@'
              break;                      //   the address is finished
            }
            atfound++;
            atdot = 0;
            continue;
          }
          if(ech=='.')                    // count '.'
          {
            dotfound++;
            atdot++;
            chardot = 0;
            continue;
          }
          charcnt++;                      // one more char
          chardot++;
        }
        if(   atfound==1                  // exactly one '@'
           && dotfound>0                  // at least one '.' (domain.country)
           && atdot>0                     //   this dot must be after the '@'
           && chardot>=2 && chardot<=4    // domain must have two or three chars
           && charcnt>6)                  // and we have also some chars
        {
          if(-1 == FPuts(fh,"<a href=\"mailto:")) return(0);  // Generate email link
          for(i=0;i<length;i++)
          {
            if(-1 == FPutC(fh,buf[i])) return(0);
          }
          if(-1 == FPuts(fh,"\">")) return(0);
          for(;length;length--)
          {
            if(-1 == FPutC(fh,*buf++)) return(0);
          }
          if(-1 == FPuts(fh,"</a>")) return(0);
          continue;
        }
        buf++;
      }
    }

    if(dolink)                            // internet link recognition
    {
      if(  (!Strnicmp(buf-1,"ftp://"  ,6))    // ftp://... - Link ?
         ||(!Strnicmp(buf-1,"gopher:" ,7))    // gopher:... - Link ?
         ||(!Strnicmp(buf-1,"http://" ,7))    // http://... - Link ?
         ||(!Strnicmp(buf-1,"https://",8))    // https://... - Link ?
         ||(!Strnicmp(buf-1,"mailto:" ,7))    // mailto:... - Link ?
         ||(!Strnicmp(buf-1,"news:"   ,5))    // news:... - Link ?
         ||(!Strnicmp(buf-1,"nntp:"   ,5))    // nntp:... - Link ?
         ||(!Strnicmp(buf-1,"telnet:" ,7))    // telnet:... - Link ?
         ||(!Strnicmp(buf-1,"www."    ,4)))   // www. - Link?
      {
        STRPTR bufstart;

        if(-1 == FPuts(fh,"<a href=\"")) return(0);

        if(!Strnicmp(buf-1,"www.",4))
          if(-1 == FPuts(fh,"http://")) return(0);

        bufstart = --buf;
        while(ch = *buf++)
        {
          if(' '==ch || '\n'==ch || ')'==ch || ('@'==ch && '{'==*buf)) break;
          if(-1 == FPutC(fh,ch)) return(0);
        }
        if(-1 == FPuts(fh,"\">")) return(0);
        for(;bufstart<buf-1;bufstart++)     // followed by a verbatim copy
        {
          if(-1 == FPutC(fh,*bufstart)) return(0);
        }
        if(-1 == FPuts(fh,"</a>")) return(0);
      }
    }

    if('@'==ch && '{'==*buf)              // AmigaGuide command?
    {
      buf++;                              // read the command?
      for(;;)
      {
        if('\"' == *buf)                  // is it a link?
        {
          STRPTR link;
          UWORD  linkpos=0;
          BOOL   linkquot=0;

          buf++;                          // skip the quotation marks
          link = buf;
          while(*link && *link!='\"') link++;
          if(!*link) return(0);
          link++;
          while(*link && (*link==' ' || *link==',' || *link=='\t')) link++;
          if(!*link) return(0);
          if(Strnicmp(link,"link",4) == 0)
          {
              link+=4;
              while(*link && *link==' ') link++;
              if(!*link) return(0);
              if(*link == '\"')
              {
                  link++;
                  linkquot=1;
              }
              while(*link && *link!='}')
              {
                  ch = *link++;
                  if(ch=='\"' && linkquot) break;
                  if(ch==' ' && !linkquot) break;
                  if(ch>='A' && ch<='Z') ch+=32;
                  linkstr[linkpos++] = (ch==' ' || ch==':' ? '_' : ch);
              }

			  if(*link == ' ') link++;

			  if(*link >= '0' && *link <= '9')
			  {
				  linkline = atoi(link);
			  }
			  else
			  {
				  linkline=0;
			  }

			  while(*link && *link!='}') link++;
			  while(linkpos>0 && linkstr[linkpos-1] == '_') linkpos--;
			  if(linkstr[linkpos-1] == '\"')
			  {
				  linkpos--;
			  }
			  linkstr[linkpos] = '\0';
			  if(!*link) return(0);
			  link++;
			  if(-1 == FPuts(fh,"<a href=\"")) return(0);

			  if(param.singlefile && strchr(linkstr,'/'))
			  {
				  char *temp = strchr(linkstr,'/');
				  *temp = '\0';

				  if(-1 == FPuts(fh,linkstr)) return(0);

				  if(param.msdos)
				  {
					  if(-1 == FPuts(fh,".htm")) return(0);
				  }
				  else
				  {
					  if(-1 == FPuts(fh,".html")) return(0);
				  }

				  *temp = '#';
				  strcpy(linkstr,temp);
			  }
			  else if(param.linkadd)
			  {
				  if(-1 == FPuts(fh,param.linkadd)) return(0);
			  }

			  if(param.dotdotslash && !param.singlefile && strchr(linkstr,'/'))
			  {
				  if(-1 == FPuts(fh,"../")) return(0);
			  }
			  if(-1 == FPuts(fh,linkstr)) return(0);

			  if(!param.singlefile)
			  {
				  if(param.msdos)
				  {
					  if(-1 == FPuts(fh,".htm")) return(0);
				  }
				  else
				  {
					  if(-1 == FPuts(fh,".html")) return(0);
				  }
			  }

			  if(linkline)
			  {
				  if(!param.singlefile)
				  {
					  char temp[20];
					  sprintf(temp,"#%ld",linkline);
					  if(-1 == FPuts(fh,temp)) return(0);
				  }
				  else if(param.singlefile && param.numberlines)
				  {
					  char temp[20];
					  sprintf(temp,"_line%ld",linkline);
					  if(-1 == FPuts(fh,temp)) return(0);
				  }
			  }

			  if(-1 == FPuts(fh,"\">")) return(0);

			  while((ch = *buf++) != '\"')
            	if(-1 == MyPutCh(fh,ch)) return(0);
          	  if(-1 == FPuts(fh,"</a>")) return(0);
          	  buf = link;
		  } else if(strncmp(link, "system", 6) == 0) {
			link += 6;
			  while(*link && *link==' ') link++;
			  if(!*link) return(0);
			  if(*link == '\"')
			  {
				  link++;
				  linkquot=1;
			  }
			  while(*link && *link!='}')
			  {
				  ch = *link++;
				  if(ch=='\"' && linkquot) break;
				  if(ch==' ' && !linkquot) break;
				  linkstr[linkpos++] = ch;
			  }

			  if(*link == ' ') link++;

			  linkline=0;

			  while(*link && *link!='}') link++;
			  while(linkpos>0 && linkstr[linkpos-1] == '_') linkpos--;
			  if(linkstr[linkpos-1] == '\"')
			  {
				  linkpos--;
			  }
			  linkstr[linkpos] = '\0';
			  if(!*link) return(0);
			  link++;
			  if (strncmp(linkstr, "openurl", 7) == 0)
			  {
			    if(-1 == FPuts(fh,"<a href=\"")) return(0);

			    if(-1 == FPuts(fh,linkstr + 8)) return(0); // skip "openurl"

			    if(-1 == FPuts(fh,"\">")) return(0);
			    while((ch = *buf++) != '\"')
				if(-1 == MyPutCh(fh,ch)) return(0);
			    if(-1 == FPuts(fh,"</a>")) return(0);
			  } else {
			    if(!param.nowarn) printf("Line %ld: WARNING: '%s' system command skipped!\n",linenr,linkstr);
			    while((ch = *buf++) != '\"')
				if(-1 == MyPutCh(fh,ch)) return(0);
			  }

			  buf = link;
		  } else {
				if(-1 == FPuts(fh,"<u>")) return(0);
				while((ch = *buf++) != '\"')
					if(-1 == MyPutCh(fh,ch)) return(0);
				if(-1 == FPuts(fh,"</u>")) return(0);
				buf = link;
				while(*buf && *buf!=' ') buf++;
				*buf++ = '\0';
				if(!param.nowarn) printf("Line %ld: WARNING: '%s' command skipped!\n",linenr,link);
				while(*buf && *buf!='}') buf++;
				if(*buf =='}') buf++;
				break;
          } 
          break;
        }

        if(!Strnicmp(buf,"amigaguide}",11))         // @{amigaguide}
        {
          if(-1 == FPuts(fh,"<b>AmigaGuide&reg;</ub>")) return(0);
          buf += 11;
          break;
        }

        if(!Strnicmp(buf,"b}",2))         // @{B}
        {
          if(-1 == FPuts(fh,"<b>")) return(0);
          buf += 2;
			track.b=TRUE;
          break;
        }
        if(!Strnicmp(buf,"ub}",3))        // @{UB}
        {
          if(-1 == FPuts(fh,"</b>")) return(0);
          buf += 3;
			track.b=FALSE;
          break;
        }

        if(!Strnicmp(buf,"i}",2))         // @{I}
        {
          if(-1 == FPuts(fh,"<i>")) return(0);
          buf += 2;
			track.i=TRUE;
          break;
        }
        if(!Strnicmp(buf,"ui}",3))        // @{UI}
        {
          if(-1 == FPuts(fh,"</i>")) return(0);
          buf += 3;
			track.i=FALSE;
          break;
        }

        if(!Strnicmp(buf,"u}",2))         // @{U}
        {
          if(-1 == FPuts(fh,"<u>")) return(0);
          buf += 2;
			track.u=TRUE;
          break;
        }
        if(!Strnicmp(buf,"uu}",3))        // @{UU}
        {
          if(-1 == FPuts(fh,"</u>")) return(0);
          buf += 3;
			track.u=FALSE;
          break;
        }
        if(!Strnicmp(buf,"code}",5))        // @{CODE}
        {
          if(-1 == FPuts(fh,"<pre>")) return(0);
          buf += 5;
			track.pre=TRUE;
          break;
        }
        if(!Strnicmp(buf,"body}",5))        // @{BODY}
        {
          if(-1 == FPuts(fh,"</pre>")) return(0);
          buf += 5;
			track.pre=FALSE;
          break;
        }
        if(!Strnicmp(buf,"line}",5))        // @{LINE}
        {
          if(-1 == FPuts(fh,"<br>")) return(0);
          buf += 5;
          break;
        }
        if(!Strnicmp(buf,"par}",4))        // @{PAR}
        {
          if(-1 == FPuts(fh,"<br>")) return(0);
          buf += 4;
          break;
        }
        if(!Strnicmp(buf,"plain}",6))        // @{PLAIN}
        {
			if(track.b)
			{
		         if(-1 == FPuts(fh,"</b>")) return(0);
				track.b=FALSE;
			}
			if(track.i)
			{
		         if(-1 == FPuts(fh,"</i>")) return(0);
				track.i=FALSE;
			}
			if(track.u)
			{
		         if(-1 == FPuts(fh,"</u>")) return(0);
				track.u=FALSE;
			}

          buf += 6;
          break;
        }
        if(!Strnicmp(buf,"jcenter}",8))        // @{JCENTER}
        {
			if(track.p)
			{
		         if(-1 == FPuts(fh,"</p>")) return(0);
				track.p=FALSE;
			}

          if(-1 == FPuts(fh,"<p align=\"center\">")) return(0);
          buf += 8;
			track.p=TRUE;
          break;
        }
        if(!Strnicmp(buf,"jleft}",6))        // @{JLEFT}
        {
			if(track.p)
			{
		         if(-1 == FPuts(fh,"</p>")) return(0);
				track.p=FALSE;
			}

          if(-1 == FPuts(fh,"<p align=\"left\">")) return(0);
          buf += 6;
			track.p=TRUE;
          break;
        }
        if(!Strnicmp(buf,"jright}",7))        // @{JRIGHT}
        {
			if(track.p)
			{
		         if(-1 == FPuts(fh,"</p>")) return(0);
				track.p=FALSE;
			}
          if(-1 == FPuts(fh,"<p align=\"right\">")) return(0);
          buf += 7;
			track.p=TRUE;
          break;
        }

        if(!Strnicmp(buf,"lindent ",8))        // @{LINDENT ...}
        {
			if(track.blockquote)
			{
		         if(-1 == FPuts(fh,"</blockquote>")) return(0);
				track.blockquote=FALSE;
			}

          UWORD i=0;
          buf += 8;

          while(*buf == ' ') buf++;

			i=atoi(buf);

          while(*buf != '}') buf++;
			buf++;

			if(param.lindent && (param.lindent<=i))
			{
          		if(-1 == FPuts(fh,"<blockquote>")) return(0);
				track.blockquote=TRUE;
			}
			break;
		}

        if(!Strnicmp(buf,"fg ",3))        // @{FG ...}
        {
			if(track.font)
			{
		         if(-1 == FPuts(fh,"</font>")) return(0);
				track.font=FALSE;
			}

          UWORD i=0;
          buf += 3;

          while(*buf == ' ') buf++;

          while(agpens[i])
          {
            if(!Strnicmp(buf,agpens[i],strlen(agpens[i])))
            {
              if(-1 == FPrintf(fh,"<font color=\"#%06lx\" class=\"%s\">",param.colours[i],cssclass[i])) return(0);
              buf += strlen(agpens[i]);
				track.font=TRUE;
              break;
            }
            i++;
          }
          if(agpens[i]) break;
        }

        if(!Strnicmp(buf,"img ",4))         // @{img ...}
        {
          if(-1 == FPuts(fh,"<img src=\"")) return(0);
          buf += 4;

          while(*buf != '}') {
	          if(-1 == MyPutCh(fh,*buf)) return(0);
    	      buf++; 
          }

          if(-1 == FPuts(fh,"\">")) return(0);

        }

        while(*buf && *buf!='}') buf++;   // Skip an unknown command!
        buf++;
        break;
      }

    }
    else
      if(-1 == MyPutCh(fh,ch)) return(0); // Error!
  }
  return(1);
}
//<

//> NavBar()
/*------------------------------------------------------------*
*   NavBar()        Produces the navigation bar               *
*     -> tfh   output file handle                             *
*     -> enode node with link data                            *
*     <-      Success                                         *
*/
LONG NavBar(BPTR tfh, struct Entry *enode)
{
  short before = 0;
  char main[100];

  for(;;)
  {

    if(param.homeurl)
    {
      if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      if(-1 == FPrintf(tfh,"<a href=\"%s\">",param.homeurl)) break;
      if(-1 == MyPuts(tfh,param.home)) break;
      if(-1 == FPuts(tfh,"</a>")) break;
		before = 1;
    }

	if(param.toc)
	{
		if(enode->Count)  {          // main page does not have a TOC link
	    	if(*enode->TOC)
    		{
      			if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      			if(-1 == FPrintf(tfh,"<a href=\"%s\">",enode->TOC)) break;
      			if(-1 == MyPuts(tfh,param.toc)) break;
      			if(-1 == FPuts(tfh,"</a>")) break;
      			before = 1;
    		}
    		else if(*TOC)
    		{
      			if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      			if(-1 == FPrintf(tfh,"<a href=\"%s\">",TOC)) break;
      			if(-1 == MyPuts(tfh,param.toc)) break;
      			if(-1 == FPuts(tfh,"</a>")) break;
      			before = 1;
    		}
    		else if(param.showall)
    		{
      			if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      			if(-1 == MyPuts(tfh,param.toc)) break;
      			before = 1;
    		}
		} else if(param.showall) {
      			if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      			if(-1 == MyPuts(tfh,param.toc)) break;
      			before = 1;
		}
    }

	if(param.index)
	{
    	if(*enode->Index)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      		if(-1 == FPrintf(tfh,"<a href=\"%s\">",enode->Index)) break;
      		if(-1 == MyPuts(tfh,param.index)) break;
      		if(-1 == FPuts(tfh,"</a>")) break;
      		before = 1;
    	}
    	else if(*Index)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      		if(-1 == FPrintf(tfh,"<a href=\"%s\">",Index)) break;
      		if(-1 == MyPuts(tfh,param.index)) break;
      		if(-1 == FPuts(tfh,"</a>")) break;
      		before = 1;
    	}
    	else if(param.showall)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      		if(-1 == MyPuts(tfh,param.index)) break;
      		before = 1;
    	}
	}

	if(param.help)
	{
	    if(*enode->Help)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      		if(-1 == FPrintf(tfh,"<a href=\"%s\">",enode->Help)) break;
      		if(-1 == MyPuts(tfh,param.help)) break;
      		if(-1 == FPuts(tfh,"</a>")) break;
      		before = 1;
    	}
    	else if(*Help)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      		if(-1 == FPrintf(tfh,"<a href=\"%s\">",Help)) break;
      		if(-1 == MyPuts(tfh,param.help)) break;
      		if(-1 == FPuts(tfh,"</a>")) break;
      		before = 1;
    	}
    	else if(param.showall)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
//      if(-1 == FPrintf(tfh,"<a href=\"http://www.unsatisfactorysoftware.co.uk/guideml/help.guide/main.html\">",Help)) break;
      		if(-1 == MyPuts(tfh,param.help)) break;
//      if(-1 == FPuts(tfh,"</a>")) break;
      		before = 1;
    	}
	}

    if(param.retrace)
    {
      if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      if(-1 == MyPuts(tfh,"<a href=\"javascript:window.history.go(-1)\">")) break;
      if(-1 == MyPuts(tfh,param.retrace)) break;
      if(-1 == FPuts(tfh,"</a>")) break;
      before = 1;
    }

	if(param.prev)
	{
    	if(*enode->Prev)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      		if(-1 == FPrintf(tfh,"<a href=\"%s\">",enode->Prev)) break;
      		if(-1 == MyPuts(tfh,param.prev)) break;
      		if(-1 == FPuts(tfh,"</a>")) break;
      		before = 1;
    	}
    	else
    	{
      		struct Entry *pred = (struct Entry *)enode->link.mln_Pred;
      		if(pred->link.mln_Pred)
      		{
        		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
        		if(-1 == FPrintf(tfh,"<a href=\"%s\">",pred->Node)) break;
        		if(-1 == MyPuts(tfh,param.prev)) break;
        		if(-1 == FPuts(tfh,"</a>")) break;
        		before = 1;
      		}
      		else if(param.showall)
      		{
        		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
        		if(-1 == MyPuts(tfh,param.prev)) break;
        		before = 1;
      		}
		}
    }

	if(param.next)
	{
    	if(*enode->Next)
    	{
      		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      		if(-1 == FPrintf(tfh,"<a href=\"%s\">",enode->Next)) break;
      		if(-1 == MyPuts(tfh,param.next)) break;
      		if(-1 == FPuts(tfh,"</a>")) break;
      		before = 1;
    	}
    	else
    	{
      		struct Entry *next = (struct Entry *)enode->link.mln_Succ;
      		if(next->link.mln_Succ)
      		{
        		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
        		if(-1 == FPrintf(tfh,"<a href=\"%s\">",next->Node)) break;
        		if(-1 == MyPuts(tfh,param.next)) break;
        		if(-1 == FPuts(tfh,"</a>")) break;
        		before = 1;
      		}
      		else if(param.showall)
      		{
        		if(before) if(-1 == MyPuts(tfh,param.bar)) break;
        		if(-1 == MyPuts(tfh,param.next)) break;
        		before = 1;
      		}
		}
    }

    if(param.findurl && param.find)
    {
      if(before) if(-1 == MyPuts(tfh,param.bar)) break;
      if(-1 == FPrintf(tfh,"<a href=\"%s\">",param.findurl)) break;
      if(-1 == MyPuts(tfh,param.find)) break;
      if(-1 == FPuts(tfh,"</a>")) break;
      before = 1;
    }


    if(-1 == FPutC(tfh,'\n')) break;
    return(1);                            // ALL OK
  }
  return(0);                              // ERROR
}
//<

//> MozNavBar()
/*--------------------------------------------------------------------*
*   MozNavBar() Produces a Mozilla-compatible site navigation bar     *
*     -> tfh   output file handle                                     *
*     -> enode node with link data                                    *
*     <-      Success                                                 *
*/
LONG MozNavBar(BPTR tfh, struct Entry *enode)
{
  short before = 0;

  for(;;)
  {
    if(enode->Count)            // main page does not have a TOC link
    {
      if(param.msdos)
      {
        if(-1 == FPrintf(tfh,"<link rel=\"up\" href=\"%s\" title=\"%s\" />\n",(*enode->TOC ? enode->TOC : "main.htm"),textlabs.toc)) break;
      }
      else
      {
        if(-1 == FPrintf(tfh,"<link rel=\"up\" href=\"%s\" title=\"%s\" />\n",(*enode->TOC ? enode->TOC : "main.html"),textlabs.toc)) break;
      }
    }

    if(*enode->Index)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"index\" href=\"%s\" title=\"%s\" />\n",enode->Index,textlabs.index)) break;
    }
    else if(*Index)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"index\" href=\"%s\" title=\"%s\" />\n",Index,param.index)) break;
    }

    if(*enode->Prev)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"previous\" href=\"%s\" title=\"%s\" />\n",enode->Prev,textlabs.prev)) break;
    }
    else
    {
      struct Entry *pred = (struct Entry *)enode->link.mln_Pred;
      if(pred->link.mln_Pred)
      {
        if(-1 == FPrintf(tfh,"<link rel=\"previous\" href=\"%s\" title=\"%s\" />\n",pred->Node,textlabs.prev)) break;
      }
    }

    if(*enode->Next)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"next\" href=\"%s\" title=\"%s\" />\n",enode->Next,textlabs.next)) break;
    }
    else
    {
      struct Entry *next = (struct Entry *)enode->link.mln_Succ;
      if(next->link.mln_Succ)
      {
        if(-1 == FPrintf(tfh,"<link rel=\"next\" href=\"%s\" title=\"%s\" />\n",next->Node,textlabs.next)) break;
      }
    }

    if(*enode->Help)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"help\" href=\"%s\" title=\"%s\" />\n",enode->Help,textlabs.help)) break;
    }
    else if(*Help)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"help\" href=\"%s\" title=\"%s\" />\n",Help,textlabs.help)) break;
    }
	 else
	 {
//      if(-1 == FPuts(tfh,"<link rel=\"help\" href=\"http://www.unsatisfactorysoftware.co.uk/guideml/help.guide/main.html\" title=\"AmigaGuide Help\" />\n")) break;
	 }

    if(param.homeurl)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"top\" href=\"%s\" title=\"%s\" />\n",param.homeurl,textlabs.home)) break;
    }

    if(param.findurl)
    {
      if(-1 == FPrintf(tfh,"<link rel=\"search\" href=\"%s\" title=\"%s\" />\n",param.findurl,textlabs.find)) break;
    }

      if(-1 == FPuts(tfh,"<link rel=\"GuideML\" href=\"https://www.unsatisfactorysoftware.co.uk/\" title=\"Unsatisfactory Software\" />\n")) break;


//    if(-1 == FPutC(tfh,'\n')) break;
    return(1);                            // ALL OK
  }
  return(0);                              // ERROR
}
//<

STRPTR SubMacro(STRPTR line,struct Entry *enode)
{
	STRPTR buffer;
	unsigned char *ch,*bufch;
	int num,found=0;
	int allmacros=macros+enode->macros;

	if(allmacros==0) return(line);

	if(enode->macros)
	{
		for(num=macros;num<allmacros;num++)
		{
			macro[num]=enode->macro[num];
			macrotext[num]=enode->macrotext[num];
		}
	}

	do
	{
		buffer = AllocVec(LINELEN+1024,MEMF_ANY);
		bufch=buffer;
		found=0;
		ch=line;
		while(*ch!='\0')
		{
			*bufch = *ch;

			if(*ch == '\\') // escape char, don't check next character
			{
				ch++;
				bufch++;
				*bufch = *ch;
			}
			else
			{
		    	if(!Strnicmp(ch,"@{",2)) // got potential macro
				{
					for(num=allmacros-1;num>=0;num--)
					{
				    	if((!Strnicmp(ch+2,macro[num],strlen(macro[num]))) && ((*(ch+2+strlen(macro[num])) == ' ') || (*(ch+2+strlen(macro[num])) == '}')))
						{
							char *macroarg[10];
							int macroargs=0;
							char *temp;

      						//if(param.verbatim) printf("Substituting '%s' macro\n",macro[num]);

							ch+=(2+strlen(macro[num]));

							while(*ch!='}' && *ch!='\0') // we have arguments
							{
								int gotquote=0;

								while(*ch == ' ') ch++;   // Skip spaces
								if(*ch == '\"')
								{
									ch++;     // Skip quotation mark
									gotquote=1;
								}
								temp=ch;

								if(gotquote)
								{
	      							while(*ch != '\"')  // search quote
									{
										if(*ch== '\\') ch++;
										ch++;
									}
								}
								else
								{
	      							while(*ch != ' ' && *ch!='}')  // search space
									{
										//if(*ch== '\\') ch++;
										ch++;
									}
								}

								if(*ch!='}')
								{
									*ch='\0';
									ch++;
								}
								else
								{
									*ch='\0';
								}

								macroarg[macroargs]=(char *)strdup(temp);
								macroargs++;
								while(*ch == ' ') ch++;   // Skip trailing spaces
							}

							found=1;

							temp=macrotext[num];
							do
							{
								if(*temp=='$')
								{
									temp++;
									macroargs=(*temp)-'0'-1;
									strcpy(bufch,macroarg[macroargs]);
									bufch+=strlen(macroarg[macroargs]);
									temp++;
								}
								else
								{
									if(*temp=='\\')
									{
										*bufch=*temp;
										bufch++;
										temp++;
									}

									*bufch=*temp;
									bufch++;
									temp++;
								}
							}while(*temp != '\0');
							bufch--;

							if(macroargs)
							{
								int n;

								for(n=0;n<macroargs;n++)
									free(macroarg[n]);
							}

						}
					}
				}
			}
			ch++;
			bufch++;
		}

		*bufch=0;
		FreeVec(line);
		line=buffer;
	}while(found==1);

	return(buffer);
}

//> Convert()
/*------------------------------------------------------------*
*   Convert()       Reads guide and creates HTML files        *
*     -> fh   input file handle                               *
*     <-      Success                                         *
*/
LONG Convert(BPTR fh)
{
  STRPTR buffer = AllocVec(LINELEN,MEMF_ANY);     // allocate line buffer
  char copyright[100] = "\0";
  char author[100] = "\0";
  char version[100] = "\0";
  char master[100] = "\0";
  char database[100] = "\0";
  char keywords[100] = "\0";
  char description[200] = "\0";
  STRPTR node, title, line;
  char nodename[100];
  char filename[100];
	char fontname[100] = "\0";
	int fontsize=0;
  BPTR tfh = (BPTR)NULL;
  ULONG linenr = 0,nodelinenr=0;
  short firstpage = 1;
  struct Entry *enode = (struct Entry *)entries.mlh_Head;
  register UWORD donavbar = !param.nonavbar;
  LONG wrap = 0;
  long mode;

  if(param.wordwrap || param.smartwrap) wrap = 1;

  if(!buffer) return(0);

  if(!wb) PutStr("Converting into HTML\n");

   /* search for @node */
  do
  {
    linenr++;
    if(FGets(fh,buffer,LINELEN) == NULL) {
#ifndef __HAIKU__
	   if((!wb) && (IoErr() != 0)) printf("IO Error: %ld\n", IoErr());
#endif
	   goto Done;
	}

   if(!param.noauto)
   {
    if(Strnicmp(buffer,"@wordwrap",9)==0)
    {
    if(!wb) PutStr("@wordwrap detected!\n");
	 param.wordwrap = 1;
	 wrap = 1;
	 }
    if(Strnicmp(buffer,"@smartwrap",10)==0)
    {
    if(!wb) PutStr("@smartwrap detected!\n");
	 param.smartwrap = 1;
	 wrap = 1;
	 }
   }

	 if(Strnicmp(buffer,"@macro ",7)==0)
	 {
		int foundquote=0;
		char *temp;
	   line = buffer+7;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"')
		{
			 line++;     // Skip quotation mark
			foundquote=1;
		}
		temp=line;

		if(foundquote==1)
		{
      		while(*line != '\"') line++;  // search quote
		}
		else
		{
      		while(*line != ' ') line++;  // search whitespace
		}

		*line=0;
		macro[macros]=(char *)strdup(temp);
		line++;

      while(*line==' ') line++; // skip trailing spaces
      if(*line == '\"') line++;     // skip quotation mark
		temp=line;
      while(*line != '\"') line++;  // search quote
		*line=0;
		macrotext[macros]=(char *)strdup(temp);

		if(param.verbatim) printf("Macro %ld: %s => %s\n",macros+1,macro[macros],macrotext[macros]);

		macros++;

    }

	 if(Strnicmp(buffer,"@font ",6)==0)
	 {
		int foundquote=0;
		char *temp;
	   line = buffer+6;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"')
		{
			 line++;     // Skip quotation mark
			foundquote=1;
		}
		temp=line;

		while(*line != '.') line++; // search for a dot
		*line=0; // this is a bit of a cheat as the font must end in ".font".
		strcpy(fontname,temp);

		if(foundquote==1)
		{
      		while(*line != '\"') line++;  // search quote
		}
		else
		{
      		while(*line != ' ') line++;  // search whitespace
		}

		line++;

      while(*line==' ') line++; // skip trailing spaces
      if(*line == '\"') line++;     // skip quotation mark
		temp=line;
      while(*line != '\"') line++;  // search quote
		*line=0;
		fontsize=atoi(temp);
    }

   // the below would be seriously better off as subroutines...

	 if(Strnicmp(buffer,"@(c) ",5)==0)
	 {
	   line = buffer+5;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
      strcpy(copyright,title);
    }

	 if(Strnicmp(buffer,"@author ",8)==0)
	 {
	   line = buffer+8;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
	  strcpy(author,title);
	 }

	 if(Strnicmp(buffer,"@$ver:",6)==0)
	 {
	 	   line = buffer+6;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
	  strcpy(version,title);
	}

	 if(Strnicmp(buffer,"@database ",10)==0)
	 {
	   line = buffer+10;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
	  strcpy(database,title);
	}

	 if(Strnicmp(buffer,"@master ",8)==0)
	 {
	 	   line = buffer+8;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
	  strcpy(master,title);
	}

	 if(Strnicmp(buffer,"@keywords ",10)==0)
	 {
	 	   line = buffer+10;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
	 strcpy(keywords,title);
	}

	 if(Strnicmp(buffer,"@description ",13)==0)
	 {
	 	   line = buffer+13;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
	 strcpy(description,title);
	}

  }
  while(Strnicmp(buffer,"@node ",strlen("@node ")));

   /* and go... */
  for(;;)
  {
#ifndef __HAIKU__
    if(SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) goto ErrorBrk;
#endif

     /* search the node name */
    node = buffer+strlen("@node ");
    while(*node == ' ') node++;     // skip leading spaces
    if(*node == '\"')
    {                               // Quote Mode
      node++;
      line = node;
      while(*line!='\"' && *line!='\n')
      {
        if(*line==' ' || *line==':') *line='_';  // space -> '_'
        line++;
      }
      *line++ = '\0';
    }
    else
    {
      line = node;
      while(*line!=' ' && *line!='\n' && *line!='\t')
      {
        if(*line==':') *line='/';               // ':' -> '_'
        line++;
      }
      *line++ = '\0';
    }

     /* search node title */
    if(*line == '\0' || *line == '\n')
    	{
      title = line;                 // Node does not have a title
    	if(*enode->Titl)
    		{
    			title = enode->Titl;

//printf("%s\n",title);

    		}
    	}
    else
    {
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"') line++;     // Skip quotation mark
      title = line;
      while(*line != '\n') line++;  // search EOL
      do line--; while(*line==' '); // skip trailing spaces
      if(*line == '\"') line--;     // skip trailing quotation mark
      *++line = '\0';               // terminate the line
    }

#ifdef __HAIKU__
	strcpy(nodename, node);
#else
    StrConvert(locale, node, nodename, 100, SC_COLLATE1);
    mystrlwr(nodename);                   // node name to lowercase
#endif

	if(param.singlefile)
	{
		if(Stricmp(nodename,"main") == 0)
		{
			mode = MODE_NEWFILE;
		}
		else
		{
			mode = MODE_OLDFILE;
		}

#ifndef __HAIKU__
		mystrlwr(param.from);
#endif
		strcpy(filename,FilePart(param.from));
	    if(param.msdos)
    	{
      		strcat(filename,".htm");
    	}
    	else
    	{
      		strcat(filename,".html");
    	}

	}
	else
	{
	    if(param.msdos)
    	{
      		strcat(nodename,".htm");
    	}
    	else
    	{
      		strcat(nodename,".html");
    	}

		mode = MODE_NEWFILE;

		strcpy(filename,nodename);
	}


    if(tfh = Open(filename,mode))
    {

		if(param.singlefile)
		{
			seek64(tfh, 0, OFFSET_END);
		}


      if(param.verbatim) printf("Creating page '%s' (Title: '%s')\n",nodename,title);

		nodelinenr=linenr;

if(((param.singlefile & firstpage) | (!param.singlefile)) & (!param.nohtml))
//if(!param.nohtml)
{
      if(-1 == FPuts(tfh,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n\"http://www.w3.org/TR/html4/loose.dtd\">\n<html>\n<head>\n<title>")) goto ErrorFile;
      if(-1 == MyPuts(tfh,title)) goto ErrorFile;
      if(-1 == FPuts(tfh,"</title>\n"
                         "<meta name=\"Generator\" content=\"" VERS " (https://www.unsatisfactorysoftware.co.uk/guideml/)\">\n"
                         "<meta http-equiv=\"Content-Type\" content=\"text/html\">\n")) goto ErrorFile;
		// start custom meta
		if(strlen(copyright) > 2)
		{
		if(-1 == FPuts(tfh,"<meta name=\"Copyright\" content=\"")) goto ErrorFile;
		if(-1 == MyPuts(tfh,copyright)) goto ErrorFile;
		if(-1 == FPuts(tfh,"\">\n")) goto ErrorFile;
		}
		// end custom meta
		// start custom meta
		if(strlen(author) > 2)
		{
		if(-1 == FPuts(tfh,"<meta name=\"Author\" content=\"")) goto ErrorFile;
		if(-1 == MyPuts(tfh,author)) goto ErrorFile;
		if(-1 == FPuts(tfh,"\">\n")) goto ErrorFile;
		}
		// end custom meta
		// start custom meta
		if(strlen(version) > 2)
		{
		if(-1 == FPuts(tfh,"<meta name=\"Version\" content=\"")) goto ErrorFile;
		if(-1 == MyPuts(tfh,version)) goto ErrorFile;
		if(-1 == FPuts(tfh,"\">\n")) goto ErrorFile;
		}
		// end custom meta
		// start custom meta
		if(strlen(database) > 2)
		{
		if(-1 == FPuts(tfh,"<meta name=\"Database\" content=\"")) goto ErrorFile;
		if(-1 == MyPuts(tfh,database)) goto ErrorFile;
		if(-1 == FPuts(tfh,"\">\n")) goto ErrorFile;
		}
		// end custom meta
		// start custom meta
		if(strlen(master) > 2)
		{
		if(-1 == FPuts(tfh,"<meta name=\"Master\" content=\"")) goto ErrorFile;
		if(-1 == MyPuts(tfh,master)) goto ErrorFile;
		if(-1 == FPuts(tfh,"\">\n")) goto ErrorFile;
		}
		// end custom meta
		// start custom meta
		if(strlen(keywords) > 2)
		{
			if(-1 == FPuts(tfh,"<meta name=\"Keywords\" content=\"")) goto ErrorFile;
			if(-1 == MyPuts(tfh,keywords)) goto ErrorFile;
			if(-1 == FPuts(tfh,"\">\n")) goto ErrorFile;
		}
		// end custom meta
		// start custom meta
		if(strlen(description) > 2)
		{
			if(-1 == FPuts(tfh,"<meta name=\"Description\" content=\"")) goto ErrorFile;
			if(-1 == MyPuts(tfh,description)) goto ErrorFile;
			if(-1 == FPuts(tfh,"\">\n")) goto ErrorFile;
		}
		// end custom meta

		if(param.cssurl)
		{
			if(-1 == FPuts(tfh,"<link rel=\"stylesheet\" href=\"")) goto ErrorFile;
			if(-1 == MyPuts(tfh,param.cssurl)) goto ErrorFile;
			if(-1 == FPuts(tfh,"\" type=\"text/css\">")) goto ErrorFile;
		}

		if(!param.moznav)
		{
      if(!MozNavBar(tfh,enode)) goto ErrorFile;
      }


		if(-1 == FPuts(tfh,"</head>\n<body")) goto ErrorFile;

      if(param.bodyext)
      {
        if(-1 == FPutC(tfh,' ')) goto ErrorFile;
        if(-1 == MyPuts(tfh,param.bodyext)) goto ErrorFile;
      }
		else
		{
			if(-1 == FPuts(tfh," bgcolor=\"#")) goto ErrorFile;
	        if(-1 == MyPutHex(tfh,param.colours[5])) goto ErrorFile;
			if(-1 == FPuts(tfh,"\" text=\"#")) goto ErrorFile;
	        if(-1 == MyPutHex(tfh,param.colours[0])) goto ErrorFile;
			if(-1 == FPuts(tfh,"\"")) goto ErrorFile;
		}

      if(-1 == FPuts(tfh,">\n")) goto ErrorFile;

}

		if(param.singlefile)
		{
			if(-1 == FPuts(tfh,"<a name=\"")) goto ErrorFile;
	        if(-1 == MyPuts(tfh,nodename)) goto ErrorFile;
			if(-1 == FPuts(tfh,"\">")) goto ErrorFile;
		      if(-1 == MyPuts(tfh,title)) goto ErrorFile;
			if(-1 == FPuts(tfh,"</a><p>")) goto ErrorFile;
			firstpage=0;

		}

      if(param.htmltoptxt)
      {
        if(-1 == FPutC(tfh,' ')) goto ErrorFile;
        if(-1 == MyPuts(tfh,param.htmltoptxt)) goto ErrorFile;
      }
      if(param.htmlheadf)
      {
        if(-1 == FPutC(tfh,' ')) goto ErrorFile;
        if(-1 == MyPuts(tfh,(STRPTR)htmltop)) goto ErrorFile;
      }

		if(donavbar)
		{
      if(!NavBar(tfh,enode)) goto ErrorFile;
      if(-1 == FPuts(tfh,"<hr>\n")) goto ErrorFile;
      }

		if(enode->fontsize)
		{
			sprintf(temp,"<font face=\"%s\" size=\"%ld\">",enode->fontname,enode->fontsize);
        	if(-1 == MyPuts(tfh,temp)) goto ErrorFile;
		}
		else if(fontsize)
		{
			sprintf(temp,"<font face=\"%s\" size=\"%ld\">",fontname,fontsize);
        	if(-1 == MyPuts(tfh,temp)) goto ErrorFile;
		}

		if(!wrap)
		{
	      if(-1 == FPuts(tfh,"<pre>\n")) goto ErrorFile;
		}
		else
		{
			if(!param.varwidth)
      	{
      		if(-1 == FPuts(tfh,"<tt>\n")) goto ErrorFile;
      	}
		}

      for(;;)
      {
        linenr++;
        if(FGets(fh,buffer,LINELEN) == NULL) goto ErrorEOF;

// TEMPORARY FOR TEST PORPOISES!
//printf("%s\n",buffer);

        if(buffer[0]=='@' && buffer[1]!='{')    // is it an AmigaGuide command
        {
          if(!Strnicmp(buffer,"@node ",strlen("@node "))) break;
          if(!Strnicmp(buffer,"@endnode",strlen("@endnode"))) break;
          continue;                           // unknown command
        }

		buffer = SubMacro(buffer,enode);

        if(!ConvLine(tfh,buffer,linenr,nodelinenr,nodename))
        {
			sprintf(temp,"Line %ld: Couldn't convert!\n",linenr);
          err(temp,"OK",0);
          goto Error;
        }
        		if(param.wordwrap)
				{
	  				if(-1 == FPuts(tfh,"<br>")) return(0);
				}

      }

		if(!wrap)
		{
      	if(-1 == FPuts(tfh,"</pre>\n")) goto ErrorFile;
		}
		else
		{
			if(!param.varwidth)
      	{
	      	if(-1 == FPuts(tfh,"</tt>\n")) goto ErrorFile;
			}
		}

			if(track.b)
			{
		         if(-1 == FPuts(tfh,"</b>")) return(0);
				track.b=FALSE;
			}
			if(track.i)
			{
		         if(-1 == FPuts(tfh,"</i>")) return(0);
				track.i=FALSE;
			}
			if(track.u)
			{
		         if(-1 == FPuts(tfh,"</u>")) return(0);
				track.u=FALSE;
			}
			if(track.blockquote)
			{
		         if(-1 == FPuts(tfh,"</blockquote>")) return(0);
				track.blockquote=FALSE;
			}
			if(track.pre)
			{
		         if(-1 == FPuts(tfh,"</pre>")) return(0);
				track.pre=FALSE;
			}
			if(track.font)
			{
		         if(-1 == FPuts(tfh,"</font>")) return(0);
				track.font=FALSE;
			}
			if(track.p)
			{
		         if(-1 == FPuts(tfh,"</p>")) return(0);
				track.p=FALSE;
			}

			if(enode->fontsize || fontsize)
			{
		         if(-1 == FPuts(tfh,"</font>")) return(0);
			}

	if(param.singlefile)
	{
        if(-1 == FPuts(tfh,"<hr>\n")) goto ErrorFile;
	}
	else
	{

      if(param.footer)
      {
        if(-1 == FPuts(tfh,"<hr>\n")) goto ErrorFile;
        if(donavbar)
        	{
        	if(!NavBar(tfh,enode)) goto ErrorFile;
        	}
      }

      if(param.htmlbottxt)
      {
        if(-1 == FPutC(tfh,' ')) goto ErrorFile;
        if(-1 == MyPuts(tfh,param.htmlbottxt)) goto ErrorFile;
      }
      else if(param.htmlfootf)
      {
        if(-1 == FPutC(tfh,' ')) goto ErrorFile;
        if(-1 == MyPuts(tfh,(STRPTR)htmlbot)) goto ErrorFile;
      }
		else
		{
	      if(firstpage)
	      {
			firstpage=0;
      	  if(-1 == FPuts(tfh,"<hr>\n<address>Converted using <a href=\"https://www.unsatisfactorysoftware.co.uk/guideml/\">" VERS "</a></address>\n")) goto ErrorFile;
      	}
      }

	if(!param.nohtml)
	{
      if(-1 == FPuts(tfh,"</body>\n</html>\n")) goto ErrorFile;
	}
}

      enode = (struct Entry *)enode->link.mln_Succ;
      Close(tfh);
      tfh = (BPTR)NULL;
    }
    else
    {
sprintf(temp,"Line %ld: Couldn't open %s for writing!\n",linenr,nodename);
      err(temp,"OK",0);
      goto Error;
    }

    while(Strnicmp(buffer,"@node ",strlen("@node ")))
    {
      linenr++;
      if(FGets(fh,buffer,LINELEN) == NULL) goto Done;
    }
  }

Done:
	if(param.singlefile)
	{

    if(tfh = Open(filename,MODE_OLDFILE))
    {
		seek64(tfh, 0, OFFSET_END);

      if(param.footer)
      {
        if(donavbar)
        	{
        	if(!NavBar(tfh,enode)) goto ErrorFile;
        	}
      }

      if(param.htmlbottxt)
      {
        if(-1 == FPutC(tfh,' ')) goto ErrorFile;
        if(-1 == MyPuts(tfh,param.htmlbottxt)) goto ErrorFile;
      }
      else if(param.htmlfootf)
      {
        if(-1 == FPutC(tfh,' ')) goto ErrorFile;
        if(-1 == MyPuts(tfh,(STRPTR)htmlbot)) goto ErrorFile;
      }
		else
		{
      	  if(-1 == FPuts(tfh,"<address>Converted using <a href=\"https://www.unsatisfactorysoftware.co.uk/guideml/\">" VERS "</a></address>\n")) goto ErrorFile;
      }

if(!param.nohtml)
	{
      if(-1 == FPuts(tfh,"</body>\n</html>\n")) goto ErrorFile;
	}

      Close(tfh);
      tfh = (BPTR)NULL;

	}
}


  FreeVec(buffer);
  return(1);

ErrorBrk:
sprintf(temp,"Line %ld: <CTRL-C> pressed!\n",linenr);
  err(temp,"OK",0);
  goto Error;
ErrorFile:
sprintf(temp,"Line %ld: Error writing file!\n",linenr);
  err(temp,"OK",0);
  goto Error;
ErrorEOF:
sprintf(temp,"Line %ld: Unexpected EOF!\n",linenr);
  err(temp,"OK",0);
Error:
  if(tfh) Close(tfh);
  FreeVec(buffer);
  return(0);
}
//<
//> PreScan()
/*------------------------------------------------------------*
*   PreScan()       Scans the AmigaGuide file                 *
*     -> fh   input file handle                               *
*     <-      Success                                         *
*/
LONG PreScan(BPTR fh)
{
  STRPTR buffer = AllocVec(LINELEN,MEMF_ANY);     // allocate line buffer
  STRPTR parse;
  STRPTR title;
  ULONG linenr  = 0;
  ULONG nodectr = 0;
  struct Entry *currentry = NULL;

  if(!buffer) return(0);

  if(!wb) PutStr("Scanning AmigaGuide document\n");
   /* Is it an AmigaGuide document ? */
  linenr++;
  if(NULL == FGets(fh,buffer,LINELEN)) goto ErrorEOF;
  if(Strnicmp(buffer,"@database",strlen("@database")))
  {
    strcpy(temp,"This is not a valid AmigaGuide file!");
    err(temp,"OK",0);
    goto Error;
  }

  CopyLink("main\n", TOC);

   /* Search for commands */
  for(;;)
  {
#ifndef __HAIKU__
    if(SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) goto ErrorBrk;
#endif
    linenr++;
    if(!FGets(fh,buffer,LINELEN)) break;      // Scan all lines
    if(buffer[0]!='@') continue;              // no command line
    if(buffer[1]=='{') continue;              // a text command at first column?

    parse = buffer+1;

    if(!Strnicmp(parse,"node ",strlen("node ")))      // @node <name>
    {
      if(currentry) AddTail((struct List *)&entries,(struct Node *)currentry);
      currentry = AllocVec(sizeof(struct Entry),MEMF_ANY|MEMF_CLEAR);
      if(!currentry) goto ErrorMem;
      currentry->Count = nodectr++;
      CopyLink(parse+strlen("node "), currentry->Node);
    }

    if(!Strnicmp(parse,"index ",strlen("index ")))    // @index <index>
    {
      if(currentry)
      {
        CopyLink(parse+strlen("index "), currentry->Index);
      }
      else
      {
	      CopyLink(parse+strlen("index "), Index);
      }
    }

    if(!Strnicmp(parse,"prev ",strlen("prev ")))      // @prev <name>
    {
      if(currentry)
      {
        CopyLink(parse+strlen("prev "), currentry->Prev);
      }
    }

    if(!Strnicmp(parse,"next ",strlen("next ")))      // @next <name>
    {
      if(currentry)
      {
        CopyLink(parse+strlen("next "), currentry->Next);
      }
    }

    if(!Strnicmp(parse,"toc ",strlen("toc ")))        // @toc <name>
    {
      if(currentry)
      {
        CopyLink(parse+strlen("toc "), currentry->TOC);
      }
    }

    if(!Strnicmp(parse,"title ",strlen("title ")))        // @title <name>
    {
      if(currentry)
      {
      parse = parse+strlen("title ");
      while(*parse == ' ') parse++;   // Skip spaces
      if(*parse == '\"') parse++;     // Skip quotation mark
      title=parse;
      while(*parse != '\n') parse++;  // search EOL
      do parse--; while(*parse==' '); // skip trailing spaces
      if(*parse == '\"') parse--;     // skip trailing quotation mark
      *++parse = '\0';               // terminate the line
      strcpy(currentry->Titl,title);

        //CopyLink(parse+strlen("title "), currentry->Titl);
		}
    }

    if(!Strnicmp(parse,"help ",strlen("help ")))        // @help <name>
    {
      if(currentry)
      {
        CopyLink(parse+strlen("help "), currentry->Help);
      }
      else
      {
	      CopyLink(parse+strlen("help "), Help);
      }
    }

    if(!Strnicmp(parse,"endnode",strlen("endnode")))  // @endnode
    {
      if(currentry)
      {
        AddTail((struct List *)&entries,(struct Node *)currentry);
        currentry = NULL;
      }
    }

	 if(Strnicmp(parse,"font ",5)==0)
	 {
		if(currentry)
		{
		int foundquote=0;
		char *temp,*line;
	   line = parse+5;
      while(*line == ' ') line++;   // Skip spaces
      if(*line == '\"')
		{
			 line++;     // Skip quotation mark
			foundquote=1;
		}
		temp=line;

		while(*line != '.') line++; // search for a dot
		*line=0;
		currentry->fontname=(char *)strdup(temp);

		if(foundquote==1)
		{
      		while(*line != '\"') line++;  // search quote
		}
		else
		{
      		while(*line != ' ') line++;  // search whitespace
		}

		line++;

      while(*line==' ') line++; // skip trailing spaces
      if(*line == '\"') line++;     // skip quotation mark
		temp=line;
      while(*line != '\"') line++;  // search quote
		*line=0;
		currentry->fontsize=atoi(temp);
		}
    }


	 if(Strnicmp(parse,"macro ",6)==0)
	 {
      if(currentry)
      {
		int foundquote=0;
		char *temp,*temp2;
	   temp2 = parse+6;
      while(*temp2 == ' ') temp2++;   // Skip spaces
      if(*temp2 == '\"')
		{
			foundquote=1;
			temp2++;     // Skip quotation mark
		}

		temp=temp2;

		if(foundquote==1)
		{
      		while(*temp2 != '\"') temp2++;  // search quote
		}
		else
		{
      		while(*temp2 != ' ') temp2++;  // search whitespace
		}

		*temp2=0;
		currentry->macro[currentry->macros]=(char *)strdup(temp);
		temp2++;

      while(*temp2==' ') temp2++; // skip trailing spaces
      if(*temp2 == '\"') temp2++;     // skip quotation mark
		temp=temp2;
      while(*temp2 != '\"') temp2++;  // search quote
		*temp2=0;
		currentry->macrotext[currentry->macros]=(char *)strdup(temp);

		if(param.verbatim) printf("Local macro %ld: %s => %s\n",currentry->macros+1,currentry->macro[currentry->macros],currentry->macrotext[currentry->macros]);

		currentry->macros++;
	  }
    }


  }
  if(currentry)
  {
    AddTail((struct List *)&entries,(struct Node *)currentry);
    if(!param.nowarn) printf("Line %ld: WARNING: No closing @endnode found!\n",linenr);
//*    currentry = NULL;
  }

  FreeVec(buffer);
  return(1);

ErrorMem:
sprintf(temp,"Line %ld: Not enough memory!\n",linenr);
  err(temp,"OK",0);
  goto Error;
ErrorBrk:
sprintf(temp,"Line %ld: <CTRL-C> pressed!\n",linenr);
  err(temp,"OK",0);
  goto Error;
ErrorEOF:
sprintf(temp,"Line %ld: Unexpected EOF!\n",linenr);
  err(temp,"OK",0);
Error:
  FreeVec(buffer);
  return(0);
}
//<

//> main()
/*------------------------------------------------------------*
*   main()          M A I N   P R O G R A M                   *
*/
int main(int argc, char **argv)
{

  struct RDArgs *args;
  static char template[] = "FILE/A,TO/K,URL=HOMEURL/K,FINDURL=SEARCHURL/K,PREV/K,NEXT/K,INDEX/K,TOC/K,HELP/K,RETRACE/K,HOME/K,FIND=SEARCH/K,BAR/K,BODY/K,VER=VERBATIM/S,IMG=IMAGES/S,FTR=FOOTER/S,LA=LINKADD/K,NL=NOLINKS/S,NE=NOEMAIL/S,NW=NOWARN/S,MSDOS/S,SF=SINGLEFILE/S,PEL=PARENTEXTLINKS/S,LN=NUMBERLINES/S,NONAVBAR/S,MOZNAV/S,SHOWALL/S,HTMLHEAD/K,HTMLHEADF/K,HTMLFOOT/K,HTMLFOOTF/K,NOHTML/S,CSS/K,WORDWRAP/S,SMARTWRAP/S,VARWIDTH/S,NOAUTO/S,LINDENT/K/N,THEME=NAME/K";
  BPTR fh;
  BPTR hhf;
  BPTR oldlock = (BPTR)NULL;
  struct Entry *node, *next;
  struct LinkStr *lstr = NULL;
  struct FileInfoBlock *fib = NULL;
  long hfs = 4096;
  long hhs = 4096;
STRPTR oldlinkadd;
        BPTR destlock;

#ifndef __HAIKU__
	if((UtilityBase = OpenLibrary("utility.library",37))==0)
		{
		 err("Unable to open utility.library v37","OK",20);
		}

#ifdef __amigaos4__

		IUtility=(struct UtilityIFace *)GetInterface(UtilityBase,"main",1,NULL);

#endif

	if((LocaleBase = OpenLibrary("locale.library",38))==0)
		{
    				strcpy(temp,"Unable to open locale.library v38");
 err(temp,"OK",20);
		}
#ifdef __amigaos4__
		else
		{
			ILocale=(struct GadToolsIFace *)GetInterface(LocaleBase,"main",1,NULL);
		}
#endif

   if(0 == argc) return(wbmain((struct WBStartup *)argv));

	locale = OpenLocale(NULL);
#endif

  NewList((struct List *)&entries);

if(!wb)
{
  param.prev  = "&lt; Browse";
  param.next  = "Browse &gt;";
  param.index = "Index";
  param.toc   = "Contents";
  param.home  = "Home";
  param.help  = "Help";
  param.find  = "Search";
  param.bar   = defbar;
  param.retrace = NULL;

  if(args = (struct RDArgs *)ReadArgs(template,(IPTR*)&param,NULL))
	{
		if(param.lindent)
		{
			param.lindent = *(ULONG *)param.lindent;
		}

		ok=1;
	}

  int theme = 0;
  if (param.theme) {
    if (strcmp(param.theme, "LIGHT") == 0)
      theme = 1;
    else if (strcmp(param.theme, "DARK") == 0)
      theme = 2;
  }


  int i;
  for (i = 0; i < 7; i++) {
	param.colours[i] = colour_themes[theme][i];
  }

}

if(ok)
  {
   if(!wb) PutStr(VERS "\n");

  textlabs.prev = param.prev;
  textlabs.next = param.next;
  textlabs.index = param.index;
  textlabs.toc = param.toc;
  textlabs.home = param.home;
  textlabs.help = param.help;
  textlabs.find = param.find;
  textlabs.retrace = param.retrace;
  textlabs.bar = param.bar;

    if(param.images)
    {
      lstr = AllocVec(sizeof(struct LinkStr),MEMF_ANY);
      if(!lstr)
      {
    strcpy(temp,"**ERROR: Out of memory.");
    err(temp,"OK",0);
        goto Flush;
      }

      sprintf(lstr->Prev ,"<img src=\"prev.png\" alt=\"%s\" border=0>" ,param.prev );
      sprintf(lstr->Next ,"<img src=\"next.png\" alt=\"%s\" border=0>" ,param.next );
      sprintf(lstr->Index,"<img src=\"index.png\" alt=\"%s\" border=0>",param.index);
      sprintf(lstr->Retrace,"<img src=\"retrace.png\" alt=\"%s\" border=0>",param.retrace);
      sprintf(lstr->Toc  ,"<img src=\"toc.png\" alt=\"%s\" border=0>"  ,param.toc  );
      sprintf(lstr->Home ,"<img src=\"home.png\" alt=\"%s\" border=0>" ,param.home );
      sprintf(lstr->Help ,"<img src=\"help.png\" alt=\"%s\" border=0>" ,param.help );
      sprintf(lstr->Find ,"<img src=\"find.png\" alt=\"%s\" border=0>" ,param.find );
      if(param.prev) param.prev  = lstr->Prev;
      if(param.next) param.next  = lstr->Next;
      if(param.index) param.index = lstr->Index;
      if(param.toc) param.toc   = lstr->Toc;
      if(param.home) param.home  = lstr->Home;
      if(param.help) param.help  = lstr->Help;
      if(param.find) param.find  = lstr->Find;
      if(param.retrace) param.retrace  = lstr->Retrace;
      if(!strcmp(param.bar,defbar)) param.bar = "";
    }

	if(param.singlefile)
	{
		oldlinkadd = param.linkadd;
		param.linkadd = "#";
	}

	if(param.htmlheadf)
	{
		if(hhf=Open(param.htmlheadf,MODE_OLDFILE))
		{
//		if(ExamineFH(hhf,fib))
//			{
//				hhs=fib->fib_Size;
				if(htmltop = AllocMem(hhs,MEMF_CLEAR))
					{
						Read(hhf,htmltop,hhs);

						param.htmltoptxt=NULL;
						if(!wb) printf("%s loaded ok\n",param.htmlheadf);

					}
			Close(hhf);
		}
	}

	if(param.htmlfootf)
	{
		if(hhf=Open(param.htmlfootf,MODE_OLDFILE))
		{
				if(htmlbot = AllocMem(hfs,MEMF_CLEAR))
					{
						Read(hhf,htmlbot,hfs);
						param.htmlbottxt=NULL;
						if(!wb) printf("%s loaded ok\n",param.htmlfootf);
					}
			Close(hhf);
		}
	}

    if(fh = Open(param.from,MODE_OLDFILE))
    {
      if(param.to)
      {

        destlock = Lock(param.to,ACCESS_READ);
        if(!destlock)
        {
				strcpy(temp,"**ERROR: Couldn't lock destination drawer.");
 err(temp,"OK",0);
          Close(fh);
          goto Flush;
        }
        oldlock = CurrentDir(destlock);
      }

      if(!PreScan(fh))
      {
				strcpy(temp,"**ERROR: Scanning failed.");
 err(temp,"OK",0);
        Close(fh);
        goto Flush;
      }
      seek64(fh, 0, OFFSET_BEGINNING);
      if(!Convert(fh))
      {
				strcpy(temp,"**ERROR: Conversion failed.");
 err(temp,"OK",0);
        Close(fh);
        goto Flush;
      }
      Close(fh);
    }
    else
	{
sprintf(temp,"**ERROR: couldn't open file %s!\n",param.from);
      err(temp,"OK",0);
	}

    if(param.images)
    {
      if(!wb) PutStr("Saving images\n");
      if(!SaveImages())
		{
      				strcpy(temp,"**ERROR: couldn't save image files");
					 err(temp,"OK",0);
		}
    }

	if(!wb) printf("All finished!\n");

	if(param.singlefile)
	{
		param.linkadd = oldlinkadd;
	}


Flush:
    for (node = (struct Entry *)entries.mlh_Head;
         next = (struct Entry *)node->link.mln_Succ;
         node = next)
    {
		if(node->fontname) free(node->fontname);

		if(node->macros)
		{
			int n;
			for(n=0;n<(node->macros);n++)
			if(node->macro[n]) free(node->macro[n]);
			if(node->macrotext[n]) free(node->macrotext[n]);
		}

      Remove((struct Node *)node);
      FreeVec(node);
    }

    if(lstr) FreeVec(lstr);
    if(htmltop) FreeMem(htmltop,hhs);
    if(htmlbot) FreeMem(htmlbot,hfs);

	//if(destlock) UnLock(destlock);
    if(oldlock) UnLock(CurrentDir(oldlock));

    if(!wb) FreeArgs(args);
  }
  else
    PutStr(VERS "\n"
           "� 1997-98 by Richard K�rber <shred@eratosthenes.starfleet.de>\n"
           "Updated in 2001-08 by Chris Young <chris@unsatisfactorysoftware.co.uk>\n"
           "Yet Another (tm) AmigaGuide to HTML Converter!\n\n"
           "USAGE:\tFILE/A\t\t\tAmigaGuide file to be converted\n"
           "\tTO/K\t\t\tDestination drawer for the html pages\n"
           "\tURL=HOMEURL/K\t\tURL accessed by the 'HOME' button\n"
           "\tFINDURL=SEARCHURL/K\tURL accessed by the 'SEARCH' button\n"
           "\tPREV/K\t\t\t'PREV' button text/image\n"
           "\tNEXT/K\t\t\t'NEXT' button text/image\n"
           "\tINDEX/K\t\t\t'INDEX' button text/image\n"
           "\tTOC/K\t\t\t'CONTENTS' button text/image\n"
           "\tHELP/K\t\t\t'HELP' button text/image\n"
           "\tRETRACE/K\t\t'RETRACE' button text/image (displays only if specified)\n"
           "\tHOME/K\t\t\t'HOME' button text/image\n"
           "\tFIND=SEARCH/K\t\t'SEARCH' button text/image\n"
           "\tBAR/K\t\t\tBar text between navigation buttons\n"
           "\tBODY/K\t\t\tAddition to the body tag of every page\n"
           "\tVER=VERBATIM/S\t\tList of created files\n"
           "\tIMG=IMAGES/S\t\tCreate image navigation bar\n"
           "\tFTR=FOOTER/S\t\tNavigation bar also at end of page\n"
           "\tLA=LINKADD/K\t\tText to add to the start of each page URLs\n"
           "\tNL=NOLINKS/S\t\tIgnore internet links\n"
           "\tNE=NOEMAIL/S\t\tIgnore e-mail addresses\n"
           "\tNW=NOWARN/S\t\tDon't show warnings\n"
           "\tMSDOS/S\t\t\tCreate MS-DOS filenames (*.htm)\n"
           "\tSF=SINGLEFILE/S\t\tPut all nodes into a single HTML file\n"
           "\tPEL=PARENTEXTLINKS/S\t\tPrepends ../ to external AmigaGuide links\n"
           "\tLN=NUMBERLINES/S\t\tEnables line-numbered links to work\n"
           "\tNONAVBAR/S\t\tDo not create the HTML navigation bar\n"
           "\tMOZNAV/S\t\tCreate Mozilla-compatible Site Navigation Bar (obsolete)\n"
           "\tSHOWALL/S\t\tKeep navbar consistent between pages\n"
           "\tHTMLHEAD/K\t\tHTML to put in front of guide text\n"
           "\tHTMLHEADF/K\t\tFile containing HTMLHEAD (overrides HTMLHEAD)\n"
           "\tHTMLFOOT/K\t\tHTML to put underneath guide text\n"
           "\tHTMLFOOTF/K\t\tFile containing HTMLFOOT (overrides HTMLFOOT)\n"
           "\tNOHTML/K\t\tDo not auto-generate HTML headers or footers\n"
           "\tCSS/K\t\t\tURL of Cascading Style Sheet to link to\n"
           "\tWORDWRAP/S\t\tWord-wraps text (converts @wordwrap)\n"
           "\tSMARTWRAP/S\t\tSmartwraps text (converts @smartwrap)\n"
           "\tVARWIDTH/S\t\tDo not use fixed width font (#?WRAP only)\n"
           "\tNOAUTO/S\t\tDo not auto-detect wrap mode\n"
           "\tLINDENT/S\t\tThreshold over which @{lindent} converts to <blockquote>\n"
           "\tTHEME=NAME/K\t\tColor theme to use (AMIGAGUIDE (default), LIGHT or DARK)\n"

           "\n");
  return(0);
}
//<

/**************************************************************/

/* New GuiML stuff starts here.. code might be a bit messy *
 * as it is largely cut'n'paste from a different project   *
 * --- Starts with GuideML 3.1 ---                         */

#ifndef __HAIKU__
int wbmain(struct WBStartup *WBenchMsg)
{
struct WBArg *wbarg;
	LONG i;
	int olddir;
int name = 0;

wb=1;
ok=1;

param.lindent=4;

  param.prev  = "&lt; Browse";
  param.next  = "Browse &gt;";
  param.index = "Index";
  param.toc   = "Contents";
  param.home  = "Home";
  param.help  = "Help";
  param.find  = "Search";
  param.bar   = defbar;
  param.retrace = NULL;

  for (i = 0; i < 7; i++) {
	param.colours[i] = colour_themes[0][i];
  }

param.nowarn = TRUE;


/*
	if((UtilityBase = OpenLibrary("utility.library",37))==0)
		{
		 err("Unable to open utility.library v37","OK",20);
		}

#ifdef __amigaos4__

		IUtility=(struct UtilityIFace *)GetInterface(UtilityBase,"main",1,NULL);

#endif
*/

	if((IntuitionBase = OpenLibrary("intuition.library",37))==0)
		{
     				strcpy(temp,"Unable to open intuition.library v37");
 err(temp,"OK",20);
		}
#ifdef __amigaos4__
		else
		{
			IIntuition=(struct IntuitionIFace *)GetInterface(IntuitionBase,"main",1,NULL);
		}
#endif

	if((WorkbenchBase = OpenLibrary("workbench.library",37))==0)
		{
     				strcpy(temp,"Unable to open workbench.library v37");
 err(temp,"OK",20);
		}
#ifdef __amigaos4__
		else
		{
			IWorkbench=(struct WorkbenchIFace *)GetInterface(WorkbenchBase,"main",1,NULL);
		}
#endif

	if((DOSBase = OpenLibrary("dos.library",37))==0)
		{
    				strcpy(temp,"Unable to open dos.library v37");
 err(temp,"OK",20);
		}
#ifdef __amigaos4__
		else
		{
			IDOS=(struct DOSIFace *)GetInterface(DOSBase,"main",1,NULL);
		}
#endif

	if((AslBase = OpenLibrary("asl.library",38))==0)
		{
    				strcpy(temp,"Unable to open asl.library v38");
 err(temp,"OK",20);
		}
#ifdef __amigaos4__
		else
		{
			IAsl=(struct AslIFace *)GetInterface(AslBase,"main",1,NULL);
		}
#endif

	if((GadToolsBase = OpenLibrary("gadtools.library",37))==0)
		{
    				strcpy(temp,"Unable to open gadtools.library v37");
 err(temp,"OK",20);
		}
#ifdef __amigaos4__
		else
		{
			IGadTools=(struct GadToolsIFace *)GetInterface(GadToolsBase,"main",1,NULL);
		}
#endif

     if((WindowBase = OpenLibrary("window.class",44))==0) err("Unable to open window.class v44","OK",20);
#ifdef __amigaos4__
	if(WindowBase)	IWindow=(struct WindowIFace *)GetInterface(WindowBase,"main",1,NULL);
#endif

     if((LayoutBase = OpenLibrary("gadgets/layout.gadget",44))==0) err("Unable to open layout.gadget v44","OK",20);
#ifdef __amigaos4__
	if(LayoutBase)	ILayout=(struct LayoutIFace *)GetInterface(LayoutBase,"main",1,NULL);
#endif

     if((ButtonBase = OpenLibrary("gadgets/button.gadget",44))==0) err("Unable to open button.gadget v44","OK",20);
#ifdef __amigaos4__
	if(ButtonBase)	IButton=(struct ButtonIFace *)GetInterface(ButtonBase,"main",1,NULL);
#endif

     if((CheckBoxBase = OpenLibrary("gadgets/checkbox.gadget",44))==0) err("Unable to open checkbox.gadget v44","OK",20);
#ifdef __amigaos4__
	if(CheckBoxBase)	ICheckBox=(struct CheckBoxIFace *)GetInterface(CheckBoxBase,"main",1,NULL);
#endif

     if((LabelBase = OpenLibrary("images/label.image",44))==0) err("Unable to open label.image v44","OK",20);
#ifdef __amigaos4__
	if(LabelBase)	ILabel=(struct LabelIFace *)GetInterface(LabelBase,"main",1,NULL);
#endif

     if((ChooserBase = OpenLibrary("gadgets/chooser.gadget",44))==0) err("Unable to open chooser.gadget v44","OK",20);
#ifdef __amigaos4__
	if(ChooserBase)	IChooser=(struct ChooserIFace *)GetInterface(ChooserBase,"main",1,NULL);
#endif

     if((ClickTabBase = OpenLibrary("gadgets/clicktab.gadget",44))==0) err("Unable to open clicktab.gadget v44","OK",20);
#ifdef __amigaos4__
	if(ClickTabBase)	IClickTab=(struct ClickTabIFace *)GetInterface(ClickTabBase,"main",1,NULL);
#endif

#ifdef __amigaos4__
     if((GetColorBase = OpenLibrary("gadgets/getcolor.gadget",51))==0)
	{
	 err("Unable to open getcolor.gadget v51","OK",20);
	}
	else
	{
		IGetColor=(struct GetColorIFace *)GetInterface(GetColorBase,"main",1,NULL);
	}
#endif

//     if((ListBrowserBase = OpenLibrary("gadgets/listbrowser.gadget",44))==0) err("Unable to open listbrowser.gadget v44","OK",20);

     if((GetFileBase = OpenLibrary("gadgets/getfile.gadget",44))==0) err("Unable to open getfile.gadget v44","OK",20);
#ifdef __amigaos4__
	if(GetFileBase)	IGetFile=(struct GetFileIFace *)GetInterface(GetFileBase,"main",1,NULL);
#endif

     if((StringBase = OpenLibrary("gadgets/string.gadget",44))==0) err("Unable to open string.gadget v44","OK",20);
#ifdef __amigaos4__
	if(StringBase)	IString=(struct StringIFace *)GetInterface(StringBase,"main",1,NULL);
#endif

     if((SliderBase = OpenLibrary("gadgets/slider.gadget",44))==0) err("Unable to open slider.gadget v44","OK",20);
#ifdef __amigaos4__
	if(SliderBase)	ISlider=(struct SliderIFace *)GetInterface(SliderBase,"main",1,NULL);
#endif


	IconBase = OpenLibrary("icon.library",44);
	if(!IconBase)
	{
	 err("Cannot open icon.library v44","OK",20);
	}
	else
	{
#ifdef __amigaos4__
	IIcon=(struct IconIFace *)GetInterface(IconBase,"main",1,NULL);
#endif
	}

	for(i=0,wbarg=WBenchMsg->sm_ArgList;i<WBenchMsg->sm_NumArgs;i++,wbarg++)
		{
		olddir =-1;
		if((wbarg->wa_Lock)&&(*wbarg->wa_Name))
			olddir = CurrentDir(wbarg->wa_Lock);
if(!name)
{
strcpy(defname,"PROGDIR:");
 AddPart(defname,wbarg->wa_Name,100);
 name=1;
}

		gettooltypes(wbarg);

		if(olddir !=-1) CurrentDir(olddir);
		}

ui();

}

void gettooltypes(struct WBArg *wbarg)
{
	struct DiskObject *dobj;
	STRPTR *toolarray;
	char *s;

	ttfrom = (char *)AllocVec(1024,MEMF_CLEAR);

	if((*wbarg->wa_Name) && (dobj=GetIconTags(wbarg->wa_Name,NULL)))
		{
		toolarray = dobj->do_ToolTypes;
		if(s = (char *)FindToolType(toolarray,"FILE"))
			{
				if(ttfrom)
					{
			 			strcpy(ttfrom,s);
			 			param.from = ttfrom;
					}
			}
		if(s = (char *)FindToolType(toolarray,"TO"))
			{
				ttto = AllocMem(strlen(s)+1,0);
				if(ttto)
					{
					 	strcpy(ttto,s);
					 	param.to = ttto;
					}
			}

		if(s = (char *)FindToolType(toolarray,"HOMEURL"))
			{
				tthomeurl = AllocMem(strlen(s)+1,0);
				if(tthomeurl)
					{
					 	strcpy(tthomeurl,s);
					 	param.homeurl = tthomeurl;
					}
			}

		if(s = (char *)FindToolType(toolarray,"SEARCHURL"))
			{
				ttfindurl = AllocMem(strlen(s)+1,0);
				if(ttfindurl)
					{
					 	strcpy(ttfindurl,s);
					 	param.findurl = ttfindurl;
					}
			}

		if(s = (char *)FindToolType(toolarray,"PREV"))
			{
				ttprev = AllocMem(strlen(s)+1,0);
				if(ttprev)
					{
					 	strcpy(ttprev,s);
					 	param.prev = ttprev;
					}
			}

		if(s = (char *)FindToolType(toolarray,"NEXT"))
			{
				ttnext = AllocMem(strlen(s)+1,0);
				if(ttnext)
					{
					 	strcpy(ttnext,s);
					 	param.next = ttnext;
					}
			}

		if(s = (char *)FindToolType(toolarray,"INDEX"))
			{
				ttindex = AllocMem(strlen(s)+1,0);
				if(ttindex)
					{
					 	strcpy(ttindex,s);
					 	param.index = ttindex;
					}
			}

		if(s = (char *)FindToolType(toolarray,"TOC"))
			{
				tttoc = AllocMem(strlen(s)+1,0);
				if(tttoc)
					{
					 	strcpy(tttoc,s);
					 	param.toc = tttoc;
					}
			}

		if(s = (char *)FindToolType(toolarray,"HELP"))
			{
				tthelp = AllocMem(strlen(s)+1,0);
				if(tthelp)
					{
					 	strcpy(tthelp,s);
					 	param.help = tthelp;
					}
			}

		if(s = (char *)FindToolType(toolarray,"RETRACE"))
			{
				ttretrace = AllocMem(strlen(s)+1,0);
				if(ttretrace)
					{
					 	strcpy(ttretrace,s);
					 	param.retrace = ttretrace;
					}
			}

		if(s = (char *)FindToolType(toolarray,"HOME"))
			{
				tthome = AllocMem(strlen(s)+1,0);
				if(tthome)
					{
					 	strcpy(tthome,s);
					 	param.home = tthome;
					}
			}

		if(s = (char *)FindToolType(toolarray,"SEARCH"))
			{
				ttfind = AllocMem(strlen(s)+1,0);
				if(ttfind)
					{
					 	strcpy(ttfind,s);
					 	param.find = ttfind;
					}
			}

		if(s = (char *)FindToolType(toolarray,"BAR"))
			{
				ttbar = AllocMem(strlen(s)+2,0);
				if(ttbar)
					{
						strcpy(ttbar," ");
					 	strcat(ttbar,s);
					 	param.bar = ttbar;
					}
			}

		if(s = (char *)FindToolType(toolarray,"BODY"))
			{
				ttbody = AllocMem(strlen(s)+1,0);
				if(ttbody)
					{
					 	strcpy(ttbody,s);
					 	param.bodyext = ttbody;
					}
			}

		if(s = (char *)FindToolType(toolarray,"HTMLHEADF"))
			{
				tthtmlheadf = AllocMem(strlen(s)+1,0);
				if(tthtmlheadf)
					{
					 	strcpy(tthtmlheadf,s);
					 	param.htmlheadf = tthtmlheadf;
					}
			}

		if(s = (char *)FindToolType(toolarray,"HTMLFOOTF"))
			{
				tthtmlfootf = AllocMem(strlen(s)+1,0);
				if(tthtmlfootf)
					{
					 	strcpy(tthtmlfootf,s);
					 	param.htmlfootf = tthtmlfootf;
					}
			}

		if(s = (char *)FindToolType(toolarray,"LINKADD"))
			{
				ttlinkadd = AllocMem(strlen(s)+1,0);
				if(ttlinkadd)
					{
					 	strcpy(ttlinkadd,s);
					 	param.linkadd = ttlinkadd;
					}
			}

		if(s = (char *)FindToolType(toolarray,"CSS"))
			{
				ttcss = AllocMem(strlen(s)+1,0);
				if(ttcss)
					{
					 	strcpy(ttcss,s);
					 	param.cssurl = ttcss;
					}
			}
			
		if(s = (char *)FindToolType(toolarray,"THEME"))
			{
				tttheme = AllocMem(strlen(s)+1,0);
				if(tttheme)
					{
					 	strcpy(tttheme,s);
					 	param.theme = tttheme;
					}
			}

		if(s = (char *)FindToolType(toolarray,"COLOUR0"))
		{
			param.colours[0]=strtol(s,NULL,16);
		}

		if(s = (char *)FindToolType(toolarray,"COLOUR1"))
		{
			param.colours[1]=strtol(s,NULL,16);
		}

		if(s = (char *)FindToolType(toolarray,"COLOUR2"))
		{
			param.colours[2]=strtol(s,NULL,16);
		}

		if(s = (char *)FindToolType(toolarray,"COLOUR3"))
		{
			param.colours[3]=strtol(s,NULL,16);
		}

		if(s = (char *)FindToolType(toolarray,"COLOUR4"))
		{
			param.colours[4]=strtol(s,NULL,16);
		}

		if(s = (char *)FindToolType(toolarray,"COLOUR5"))
		{
			param.colours[5]=strtol(s,NULL,16);
		}

		if(s = (char *)FindToolType(toolarray,"COLOUR6"))
		{
			param.colours[6]=strtol(s,NULL,16);
		}

		if(s = (char *)FindToolType(toolarray,"INDENT"))
		{
			param.lindent=atoi(s);
		}

		if(s = (char *)FindToolType(toolarray,"SINGLEFILE")) param.singlefile = TRUE;
		if(s = (char *)FindToolType(toolarray,"PARENTEXTLINKS")) param.dotdotslash = TRUE;
		if(s = (char *)FindToolType(toolarray,"NUMBERLINES")) param.numberlines = TRUE;
		if(s = (char *)FindToolType(toolarray,"NOHTML")) param.nohtml = TRUE;
		if(s = (char *)FindToolType(toolarray,"IMAGES")) param.images = TRUE;
		if(s = (char *)FindToolType(toolarray,"FOOTER")) param.footer = TRUE;
		if(s = (char *)FindToolType(toolarray,"NOLINKS")) param.nolink = TRUE;
		if(s = (char *)FindToolType(toolarray,"NOEMAIL")) param.noemail = TRUE;
		if(s = (char *)FindToolType(toolarray,"MSDOS")) param.msdos = TRUE;
		if(s = (char *)FindToolType(toolarray,"NONAVBAR")) param.nonavbar = TRUE;
		if(s = (char *)FindToolType(toolarray,"MOZNAV")) param.moznav = TRUE;
		if(s = (char *)FindToolType(toolarray,"SHOWALL")) param.showall = TRUE;
		if(s = (char *)FindToolType(toolarray,"VARWIDTH")) param.varwidth = TRUE;
		if(s = (char *)FindToolType(toolarray,"NOAUTO")) param.noauto = TRUE;
		if(s = (char *)FindToolType(toolarray,"WORDWRAP")) param.wordwrap = TRUE;
		if(s = (char *)FindToolType(toolarray,"SMARTWRAP")) param.smartwrap = TRUE;
		FreeDiskObject(dobj);
		}

}

void savetooltypes(char *fname,int def)
{
	struct DiskObject *dobj;
	UBYTE **oldtooltypes;
	UBYTE *olddefaulttool;
	UBYTE oldtype;
	long olddir=-1;
	int i=0;
	UBYTE *newtooltypes[46];
	char tttmp1[100];
	char tttmp2[100];
	char tttmp3[100];
	char tttmp4[100];
	char tttmp5[100];
	char tttmp6[100];
	char tttmp7[100];
	char tttmp8[100];
	char tttmp9[100];
	char tttmp10[100];
	char tttmp11[100];
	char tttmp12[100];
	char tttmp13[100];
	char tttmp14[100];
	char tttmp15[100];
	char tttmp16[100];
	char tttmp17[100];
	char tttmp18[100];
	char tttmp19[100];
	char *defaulttool = "guideml\0";

	if(!def) {
		dobj=GetIconTags(fname,ICONGETA_FailIfUnavailable,FALSE,
							ICONGETA_GetDefaultName,defaulttool,
							ICONGETA_GetDefaultType,WBPROJECT,TAG_DONE);
	} else {
		dobj=GetIconTagList(fname, NULL);
	}

	if(dobj) {
			oldtooltypes = (UBYTE **)dobj->do_ToolTypes;
			olddefaulttool = dobj->do_DefaultTool;
			oldtype = dobj->do_Type;

			if(param.smartwrap) newtooltypes[0] = "SMARTWRAP";
								else newtooltypes[0] = "(SMARTWRAP)";
			if(param.wordwrap) newtooltypes[1] = "WORDWRAP";
								else newtooltypes[1] = "(WORDWRAP)";
			if(param.noauto) newtooltypes[2] = "NOAUTO";
								else newtooltypes[2] = "(NOAUTO)";
			if(param.varwidth) newtooltypes[3] = "VARWIDTH";
								else newtooltypes[3] = "(VARWIDTH)";
			if(param.showall) newtooltypes[4] = "SHOWALL";
								else newtooltypes[4] = "(SHOWALL)";
			if(param.moznav) newtooltypes[5] = "MOZNAV";
								else newtooltypes[5] = "(MOZNAV)";
			if(param.nonavbar) newtooltypes[6] = "NONAVBAR";
								else newtooltypes[6] = "(NONAVBAR)";
			if(param.msdos) newtooltypes[7] = "MSDOS";
								else newtooltypes[7] = "(MSDOS)";
			if(param.noemail) newtooltypes[8] = "NOEMAIL";
								else newtooltypes[8] = "(NOEMAIL)";
			if(param.nolink) newtooltypes[9] = "NOLINKS";
								else newtooltypes[9] = "(NOLINKS)";
			if(param.footer) newtooltypes[10] = "FOOTER";
								else newtooltypes[10] = "(FOOTER)";
			if(param.images) newtooltypes[11] = "IMAGES";
								else newtooltypes[11] = "(IMAGES)";
			if(param.cssurl)
			{
				strcpy(tttmp1,"CSS=");
				newtooltypes[12] = strcat(tttmp1,param.cssurl);
			}
				else
			{
				newtooltypes[12]="(CSS=)";
			}

			if(param.htmlfootf)
			{
				strcpy(tttmp2,"HTMLFOOTF=");
				newtooltypes[13] = strcat(tttmp2,param.htmlfootf);
			}
				else
			{
				newtooltypes[13]="(HTMLFOOTF=)";
			}

			if(param.htmlheadf)
			{
				strcpy(tttmp3,"HTMLHEADF=");
				newtooltypes[14] = strcat(tttmp3,param.htmlheadf);
			}
				else
			{
				newtooltypes[14]="(HTMLHEADF=)";
			}

			if(param.bodyext)
			{
				strcpy(tttmp4,"BODY=");
				newtooltypes[15] = strcat(tttmp4,param.bodyext);
			}
				else
			{
				newtooltypes[15]="(BODY=)";
			}

			if(param.bar)
			{
				strcpy(tttmp5,"BAR=");
				newtooltypes[16] = strcat(tttmp5,param.bar);
			}
				else
			{
				newtooltypes[16]="(BAR=)";
			}

			if(param.find)
			{
				strcpy(tttmp6,"SEARCH=");
				newtooltypes[17] = strcat(tttmp6,param.find);
			}
				else
			{
				newtooltypes[17]="(SEARCH=)";
			}

			if(param.home)
			{
				strcpy(tttmp7,"HOME=");
				newtooltypes[18] = strcat(tttmp7,param.home);
			}
				else
			{
				newtooltypes[18]="(HOME=)";
			}

			if(param.retrace)
			{
				strcpy(tttmp8,"RETRACE=");
				newtooltypes[19] = strcat(tttmp8,param.retrace);
			}
				else
			{
				newtooltypes[19]="(RETRACE=)";
			}

			if(param.help)
			{
				strcpy(tttmp9,"HELP=");
				newtooltypes[20] = strcat(tttmp9,param.help);
			}
				else
			{
				newtooltypes[20]="(HELP=)";
			}

			if(param.toc)
			{
				strcpy(tttmp10,"TOC=");
				newtooltypes[21] = strcat(tttmp10,param.toc);
			}
				else
			{
				newtooltypes[21]="(TOC=)";
			}

			if(param.index)
			{
				strcpy(tttmp11,"INDEX=");
				newtooltypes[22] = strcat(tttmp11,param.index);
			}
				else
			{
				newtooltypes[22]="(INDEX=)";
			}

			if(param.next)
			{
				strcpy(tttmp12,"NEXT=");
				newtooltypes[23] = strcat(tttmp12,param.next);
			}
				else
			{
				newtooltypes[23]="(NEXT=)";
			}

			if(param.prev)
			{
				strcpy(tttmp13,"PREV=");
				newtooltypes[24] = strcat(tttmp13,param.prev);
			}
				else
			{
				newtooltypes[24]="(PREV=)";
			}

			if(param.findurl)
			{
				strcpy(tttmp14,"SEARCHURL=");
				newtooltypes[25] = strcat(tttmp14,param.findurl);
			}
				else
			{
				newtooltypes[25]="(SEARCHURL=)";
			}

			if(param.homeurl)
			{
				strcpy(tttmp15,"HOMEURL=");
				newtooltypes[26] = strcat(tttmp15,param.homeurl);
			}
				else
			{
				newtooltypes[26]="(HOMEURL=)";
			}

			if(param.linkadd)
			{
				strcpy(tttmp18,"LINKADD=");
				newtooltypes[27] = strcat(tttmp18,param.linkadd);
			}
				else
			{
				newtooltypes[27]="(LINKADD=)";
			}

			if(param.nohtml) newtooltypes[28] = "NOHTML";
								else newtooltypes[28] = "(NOHTML)";

			if(param.singlefile) newtooltypes[29] = "SINGLEFILE";
								else newtooltypes[29] = "(SINGLEFILE)";

			for(i=0;i<7;i++)
			{
				newtooltypes[30+i] = AllocVec(100,MEMF_CLEAR);
				sprintf(newtooltypes[30+i],"COLOUR%d=%06lx",i,param.colours[i]);
			}

			newtooltypes[37] = AllocVec(100,MEMF_CLEAR);
			sprintf(newtooltypes[37],"INDENT=%d",param.lindent);

			if(param.dotdotslash) newtooltypes[38] = "PARENTEXTLINKS";
								else newtooltypes[38] = "(PARENTEXTLINKS)";

			if(param.numberlines) newtooltypes[39] = "NUMBERLINES";
								else newtooltypes[39] = "(NUMBERLINES)";

			if(param.theme)
			{
				strcpy(tttmp19,"THEME=");
				newtooltypes[40] = strcat(tttmp19,param.theme);
			}
				else
			{
				newtooltypes[40]="(THEME=AMIGAGUIDE)";
			}

		if(!def)
		{

			if(param.to)
			{
				strcpy(tttmp16,"TO=");
				newtooltypes[41] = strcat(tttmp16,param.to);
			}
				else
			{
				newtooltypes[41]="(TO=)";
			}

			if(param.from)
			{
				strcpy(tttmp17,"FILE=");
				newtooltypes[42] = strcat(tttmp17,param.from);
			}
				else
			{
				newtooltypes[42]="(FILE=)";
			}

			dobj->do_DefaultTool = defaulttool;
			dobj->do_Type = WBPROJECT;

		}
		else
		{
			newtooltypes[41]=NULL;
			newtooltypes[42]=NULL;
		}


			newtooltypes[43] = NULL;
			dobj->do_ToolTypes = (STRPTR *)&newtooltypes;
			PutIconTags(fname,dobj,NULL); //			PutDiskObject(fname,dobj); // PutDiskObject(wbarg->wa_Name,dobj);
			dobj->do_ToolTypes=(STRPTR *)oldtooltypes;
			dobj->do_DefaultTool=olddefaulttool;
			dobj->do_Type = oldtype;
			FreeDiskObject(dobj);
		}

			for(i=0;i<8;i++)
			{
				FreeVec(newtooltypes[30+i]);
			}
// if(olddir!=-1) CurrentDir(olddir);

}

int saveas()
{
	BPTR fp;
	int t;
	char *filename[2048];
	char guidemlcmd[4096];
	struct FileRequester *req;
	struct TagItem asltags[] = {ASLFR_TitleText,(ULONG)"Save As...",
                             ASLFR_InitialShowVolumes,TRUE,
                             ASLFR_DoSaveMode,TRUE,TAG_DONE};

	req = (struct FileRequester *)AllocAslRequest(ASL_FileRequest,NULL);
	t = AslRequest(req,asltags);

	if (t==FALSE)
   {
		// user cancelled
		return(0);
	}

	strcpy((STRPTR)filename,req->fr_Drawer);
	AddPart((STRPTR)filename,req->fr_File,2048);

	FreeAslRequest(req);

	fp = Open((STRPTR)filename,MODE_NEWFILE);

	strcpy(guidemlcmd,"guideml \"");
	if(param.from) strcat(guidemlcmd,param.from);
	if(param.from) strcat(guidemlcmd,"\" ");
	if(param.to) strcat(guidemlcmd,"TO=\"");
	if(param.to) strcat(guidemlcmd,param.to);
	if(param.to) strcat(guidemlcmd,"\" ");
	if(param.homeurl) strcat(guidemlcmd,"URL=\"");
	if(param.homeurl) strcat(guidemlcmd,param.homeurl);
	if(param.homeurl) strcat(guidemlcmd,"\" ");
	if(param.prev) strcat(guidemlcmd,"PREV=\"");
	if(param.prev) strcat(guidemlcmd,param.prev);
	if(param.prev) strcat(guidemlcmd,"\" ");
	if(param.next) strcat(guidemlcmd,"NEXT=\"");
	if(param.next) strcat(guidemlcmd,param.next);
	if(param.next) strcat(guidemlcmd,"\" ");
	if(param.index) strcat(guidemlcmd,"INDEX=\"");
	if(param.index) strcat(guidemlcmd,param.index);
	if(param.index) strcat(guidemlcmd,"\" ");
	if(param.toc) strcat(guidemlcmd,"TOC=\"");
	if(param.toc) strcat(guidemlcmd,param.toc);
	if(param.toc) strcat(guidemlcmd,"\" ");
	if(param.help) strcat(guidemlcmd,"HELP=\"");
	if(param.help) strcat(guidemlcmd,param.help);
	if(param.help) strcat(guidemlcmd,"\" ");
	if(param.retrace) strcat(guidemlcmd,"RETRACE=\"");
	if(param.retrace) strcat(guidemlcmd,param.retrace);
	if(param.retrace) strcat(guidemlcmd,"\" ");
	if(param.home) strcat(guidemlcmd,"HOME=\"");
	if(param.home) strcat(guidemlcmd,param.home);
	if(param.home) strcat(guidemlcmd,"\" ");
	if(param.find) strcat(guidemlcmd,"FIND=\"");
	if(param.find) strcat(guidemlcmd,param.find);
	if(param.find) strcat(guidemlcmd,"\" ");
	if(param.bar) strcat(guidemlcmd,"BAR=\"");
	if(param.bar) strcat(guidemlcmd,param.bar);
	if(param.bar) strcat(guidemlcmd,"\" ");
	if(param.bodyext) strcat(guidemlcmd,"BODY=\"");
	if(param.bodyext) strcat(guidemlcmd,param.bodyext);
	if(param.bodyext) strcat(guidemlcmd,"\" ");
	if(param.htmlheadf) strcat(guidemlcmd,"HTMLHEADF=\"");
	if(param.htmlheadf) strcat(guidemlcmd,param.htmlheadf);
	if(param.htmlheadf) strcat(guidemlcmd,"\" ");
	if(param.htmlfootf) strcat(guidemlcmd,"HTMLFOOTF=\"");
	if(param.htmlfootf) strcat(guidemlcmd,param.htmlfootf);
	if(param.htmlfootf) strcat(guidemlcmd,"\" ");
	if(param.cssurl) strcat(guidemlcmd,"CSS=\"");
	if(param.cssurl) strcat(guidemlcmd,param.cssurl);
	if(param.cssurl) strcat(guidemlcmd,"\" ");
	if(param.nohtml) strcat(guidemlcmd,"NOHTML ");
	if(param.images) strcat(guidemlcmd,"IMG ");
	if(param.footer) strcat(guidemlcmd,"FTR ");
	if(param.linkadd) strcat(guidemlcmd,"LA=\"");
	if(param.linkadd) strcat(guidemlcmd,param.linkadd);
	if(param.linkadd) strcat(guidemlcmd,"\" ");
	if(param.nolink) strcat(guidemlcmd,"NL ");
	if(param.noemail) strcat(guidemlcmd,"NE ");
	if(param.msdos) strcat(guidemlcmd,"MSDOS ");
	if(param.singlefile) strcat(guidemlcmd,"SF ");
	if(param.dotdotslash) strcat(guidemlcmd,"PEL ");
	if(param.numberlines) strcat(guidemlcmd,"LN ");
	if(param.nonavbar) strcat(guidemlcmd,"NONAVBAR ");
	if(param.moznav) strcat(guidemlcmd,"MOZNAV ");
	if(param.showall) strcat(guidemlcmd,"SHOWALL ");
	if(param.wordwrap) strcat(guidemlcmd,"WORDWRAP ");
	if(param.smartwrap) strcat(guidemlcmd,"SMARTWRAP ");
	if(param.varwidth) strcat(guidemlcmd,"VARWIDTH ");
	if(param.noauto) strcat(guidemlcmd,"NOAUTO ");
	if(param.theme) strcat(guidemlcmd,"THEME=\"");
	if(param.theme) strcat(guidemlcmd,param.theme);
	if(param.theme) strcat(guidemlcmd,"\" ");
	strcat(guidemlcmd,"\n");


	FPuts(fp,guidemlcmd);

	Close(fp);

	savetooltypes((STRPTR)filename,0);
}


void ui()
{
	long i=0; // temp variable
	struct List optlist;
	struct List navoptlist;
	struct List imgoptlist;
	struct List linkoptlist;
	struct List tablist;
    struct MsgPort *AppPort, *appwinport;
	struct AppWindow *appwin = NULL;
	struct AppMessage *appwinmsg;
	struct WBArg *appwinargs;
    struct Window *windows[WID_LAST];
    Object *objects[OID_LAST];

	struct Screen *scrn = NULL;
	struct Hook aslhookfunc;

	aslhookfunc.h_Entry = &AslGuideHook;
	aslhookfunc.h_SubEntry = NULL;
	aslhookfunc.h_Data = NULL;

	UBYTE *opts[] = {"Auto","Wordwrap","Smartwrap","None",NULL};
	UBYTE *navopts[] = {"Header","Both","None",NULL};
	UBYTE *linkopts[] = {"Convert All","Not Email","Email Only","None",NULL};
	UBYTE *imgopts[] = {"Text","Images",NULL};
	UBYTE *tabs[] = {"Convert","Options","NavBar",NULL};
		struct Menu *menustrip;
	UWORD menunum,itemnum,subnum; //menusel;
	struct MenuItem *item;
	int wrapm =0;
	int navm=0;
	int linkm=0;

if(param.noauto) wrapm = 3;
if(param.wordwrap) wrapm = 1;
if(param.smartwrap) wrapm = 2;
if(!param.noauto) wrapm = 0;

if(param.footer) navm = 1;
if(param.nonavbar) navm=2;

if(param.noemail) linkm=1;
if(param.nolink) linkm=linkm+2;

make_list(&optlist,opts);
make_list(&navoptlist,navopts);
make_list(&imgoptlist,imgopts);
make_list(&linkoptlist,linkopts);
maketablist(&tablist,tabs);

if(scrn = LockPubScreen(NULL)) UnlockPubScreen(NULL,scrn);

    if ( AppPort = CreateMsgPort() )
    {
        /* Create the window object.
         */
        objects[OID_MAIN] = WindowObject,
            WA_ScreenTitle, VERS " (" DATE ")",
            WA_Title, "GuideML",
            WA_Activate, TRUE,
            WA_DepthGadget, TRUE,
            WA_DragBar, TRUE,
            WA_CloseGadget, TRUE,
            WA_SizeGadget, TRUE,
            WA_SmartRefresh,TRUE,
            WA_IDCMP,IDCMP_MENUPICK,
            WINDOW_IconifyGadget, FALSE,
            WINDOW_IconTitle, "GuideML",
            WINDOW_AppPort, AppPort,
            WINDOW_Position, WPOS_CENTERMOUSE,
            WINDOW_ParentGroup, gadgets[GID_MAIN] = VGroupObject,
                LAYOUT_SpaceOuter, TRUE,
                LAYOUT_AddChild, gadgets[GID_TABS] = ClickTabObject,
                	GA_ID, GID_TABS,
                  GA_RelVerify, TRUE,
                  CLICKTAB_Labels,&tablist,
	              	CLICKTAB_PageGroup, PageObject,
                		PAGE_Add,LayoutObject,
                			LAYOUT_AddChild,VGroupObject,
									LAYOUT_SpaceOuter,TRUE,
                				LAYOUT_AddChild, gadgets[GID_FILE] = GetFileObject,
                    				GA_ID, GID_FILE,
                    				GA_RelVerify, TRUE,
                    				GETFILE_TitleText,"GuideML",
										GETFILE_ReadOnly,TRUE,
										GETFILE_FullFile,param.from,
										GETFILE_FilterFunc,&aslhookfunc,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "_Input File",
                    			LabelEnd,
                				CHILD_NominalSize, TRUE,
                				LAYOUT_AddChild, gadgets[GID_TO] = GetFileObject,
                    				GA_ID, GID_TO,
                    				GA_RelVerify, TRUE,
                    				GETFILE_TitleText,"GuideML",
										GETFILE_DoSaveMode,TRUE,
										GETFILE_DrawersOnly,TRUE,
										GETFILE_ReadOnly,TRUE,
										GETFILE_Drawer,param.to,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "_Output Path",
                    			LabelEnd,
                				CHILD_NominalSize, TRUE,
                				LAYOUT_AddChild, gadgets[GID_HOMEURL] = StringObject,
                    				GA_ID, GID_HOMEURL,
                    				GA_RelVerify, TRUE,
                    				STRINGA_TextVal,param.homeurl,
                    				STRINGA_MaxChars,100,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "_Home URL",
                    			LabelEnd,
                				CHILD_NominalSize, TRUE,
                				LAYOUT_AddChild, gadgets[GID_FINDURL] = StringObject,
                    				GA_ID, GID_FINDURL,
                    				GA_RelVerify, TRUE,
                    				STRINGA_TextVal,param.findurl,
                    				STRINGA_MaxChars,100,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "_Search URL",
                    			LabelEnd,
                				CHILD_NominalSize, TRUE,
                				LAYOUT_AddChild, gadgets[GID_CSS] = StringObject,
                    				GA_ID, GID_CSS,
                    				GA_RelVerify, TRUE,
                    				STRINGA_TextVal,param.cssurl,
                    				STRINGA_MaxChars,100,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "CSS _URL",
                    			LabelEnd,
                				CHILD_NominalSize, TRUE,
                				LAYOUT_AddChild, gadgets[GID_LA] = StringObject,
                    				GA_ID, GID_LA,
                    				GA_RelVerify, TRUE,
                    				STRINGA_TextVal,param.linkadd,
                    				STRINGA_MaxChars,100,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "_Link Prefix",
                    			LabelEnd,
                				CHILD_NominalSize, TRUE,
                				LAYOUT_AddChild, gadgets[GID_BODY] = StringObject,
                    				GA_ID, GID_BODY,
                    				GA_RelVerify, TRUE,
                    				STRINGA_TextVal,param.bodyext,
                    				STRINGA_MaxChars,100,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "_Body Attribs",
                    			LabelEnd,
                				CHILD_NominalSize, TRUE,
                				LAYOUT_AddChild, gadgets[GID_HTMLHEADF] = GetFileObject,
                    				GA_ID, GID_HTMLHEADF,
                    				GA_RelVerify, TRUE,
                    				GETFILE_TitleText,"GuideML",
										GETFILE_Pattern,"#?.(s|%)htm(l|%)",
										GETFILE_DoPatterns,TRUE,
										GETFILE_ReadOnly,TRUE,
										GETFILE_FullFile,param.htmlheadf,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "HTML H_eader",
                    			LabelEnd,
                				LAYOUT_AddChild, gadgets[GID_HTMLFOOTF] = GetFileObject,
                    				GA_ID, GID_HTMLFOOTF,
                    				GA_RelVerify, TRUE,
                    				GETFILE_TitleText,"GuideML",
										GETFILE_Pattern,"#?.(s|%)htm(l|%)",
										GETFILE_DoPatterns,TRUE,
										GETFILE_ReadOnly,TRUE,
										GETFILE_FullFile,param.htmlfootf,
                				End,
                    			CHILD_Label, LabelObject,
                        		LABEL_Text, "HTML _Footer",
                    			LabelEnd,

								LayoutEnd,
							LayoutEnd,
                		PAGE_Add,LayoutObject,
	               		LAYOUT_AddChild,VGroupObject,
               				LAYOUT_AddChild,HGroupObject,
	  		               		LAYOUT_AddChild,VGroupObject,
											LAYOUT_SpaceOuter,TRUE,
		                				LAYOUT_AddChild, gadgets[GID_WRAP] = ChooserObject,
                    						GA_ID, GID_WRAP,
                  		  				GA_RelVerify, TRUE,
						                  CHOOSER_PopUp,TRUE,
      		      				      CHOOSER_Labels,&optlist,
		  				                  CHOOSER_Selected,wrapm,
            			        			CHOOSER_AutoFit,TRUE,
                						ChooserEnd,
                  		  			CHILD_Label, LabelObject,
            		            		LABEL_Text, "_Wrap mode",
      		              			LabelEnd,
		                				CHILD_NominalSize, TRUE,



		                				LAYOUT_AddChild, gadgets[GID_LINKS] = ChooserObject,
      	              					GA_ID, GID_LINKS,
         	           					GA_RelVerify, TRUE,
					            	      CHOOSER_PopUp,TRUE,
            						      CHOOSER_Labels,&linkoptlist,
  				      		            CHOOSER_Selected,linkm,
            					        	CHOOSER_AutoFit,TRUE,
            	    					ChooserEnd,
         	           				CHILD_Label, LabelObject,
      	                  			LABEL_Text, "_Links",
	   	                 			LabelEnd,
		                				CHILD_NominalSize, TRUE,

		                				LAYOUT_AddChild, gadgets[GID_LINDENT] = SliderObject,
      	              					GA_ID, GID_LINDENT,
         	           					GA_RelVerify, TRUE,
											SLIDER_Level,param.lindent,
											SLIDER_Min,0,
											SLIDER_Max,20,
											SLIDER_Orientation,SLIDER_HORIZONTAL,
											SLIDER_Ticks,9,
											SLIDER_ShortTicks,TRUE,
#ifdef __amigaos4__
											SLIDER_LevelPlace,PLACETEXT_RIGHT,
											SLIDER_LevelFormat,"%ld",
#endif
            	    					SliderEnd,
         	           				CHILD_Label, LabelObject,
      	                  			LABEL_Text, "_Indent at",
	   	                 			LabelEnd,
		                				CHILD_NominalSize, TRUE,

		                				LAYOUT_AddChild, gadgets[GID_VARWIDTH] = CheckBoxObject,
      	              					GA_ID, GID_VARWIDTH,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"_Variable width font",
  				      		            GA_Selected,param.varwidth,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,

									LayoutEnd,
         		     				CHILD_WeightedHeight, 0,

			               		LAYOUT_AddChild,VGroupObject,
		                				LAYOUT_AddChild, gadgets[GID_MSDOS] = CheckBoxObject,
      	              					GA_ID, GID_MSDOS,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"MS-_DOS compatible filenames",
  				      		            GA_Selected,param.msdos,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,


		                				LAYOUT_AddChild, gadgets[GID_NOHTML] = CheckBoxObject,
      	              					GA_ID, GID_NOHTML,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"Do not _generate HTML headers",
  				      		            GA_Selected,param.nohtml,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,

		                				LAYOUT_AddChild, gadgets[GID_SINGLEFILE] = CheckBoxObject,
      	              					GA_ID, GID_SINGLEFILE,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"Si_ngle HTML file",
  				      		            GA_Selected,param.singlefile,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,

		                				LAYOUT_AddChild, gadgets[GID_EXTLINKS] = CheckBoxObject,
      	              					GA_ID, GID_EXTLINKS,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"Prepend _external links with ../",
  				      		            GA_Selected,param.dotdotslash,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,

		                				LAYOUT_AddChild, gadgets[GID_LINENUMBERS] = CheckBoxObject,
      	              					GA_ID, GID_LINENUMBERS,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"_Number lines in HTML",
  				      		            GA_Selected,param.numberlines,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,
									LayoutEnd,
								LayoutEnd,
          		     			CHILD_WeightedHeight, 0,
#ifdef __amigaos4__ /* TODO: Check getcolor for OS3.2 */
				               	LAYOUT_AddChild,HGroupObject,
									LAYOUT_BevelStyle,BVS_GROUP,
									LAYOUT_Label,"Colours",

				               	LAYOUT_AddChild,VGroupObject,
	   	             				LAYOUT_AddChild, gadgets[GID_CTEXT] = GetColorObject,
   	   	              				GA_ID, GID_CTEXT,
      	   	           				GA_RelVerify, TRUE,
										GETCOLOR_Screen,scrn,
										GETCOLOR_TitleText,"Text",
										GETCOLOR_Color,param.colours[0],
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "_Text",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,


	   	             				LAYOUT_AddChild, gadgets[GID_CSHINE] = GetColorObject,
   	   	              				GA_ID, GID_CSHINE,
      	   	           				GA_RelVerify, TRUE,
										GETCOLOR_Screen,scrn,
										GETCOLOR_TitleText,"Shine",
										GETCOLOR_Color,param.colours[1],
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "_Shine",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,

	   	             				LAYOUT_AddChild, gadgets[GID_CSHADOW] = GetColorObject,
   	   	              				GA_ID, GID_CSHADOW,
      	   	           				GA_RelVerify, TRUE,
										GETCOLOR_Screen,scrn,
										GETCOLOR_TitleText,"Shadow",
										GETCOLOR_Color,param.colours[2],
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "Shado_w",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,


	   	             				LAYOUT_AddChild, gadgets[GID_CFILL] = GetColorObject,
   	   	              				GA_ID, GID_CFILL,
      	   	           				GA_RelVerify, TRUE,
										GETCOLOR_Screen,scrn,
										GETCOLOR_TitleText,"Fill",
										GETCOLOR_Color,param.colours[3],
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "_Fill",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,

								LayoutEnd,
       		     					CHILD_WeightedHeight, 0,
				               	LAYOUT_AddChild,VGroupObject,

	   	             				LAYOUT_AddChild, gadgets[GID_CFILLTEXT] = GetColorObject,
   	   	              				GA_ID, GID_CFILLTEXT,
      	   	           				GA_RelVerify, TRUE,
										GETCOLOR_Screen,scrn,
										GETCOLOR_TitleText,"F_illText",
										GETCOLOR_Color,param.colours[4],
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "F_illText",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,


	   	             				LAYOUT_AddChild, gadgets[GID_CBACKGROUND] = GetColorObject,
   	   	              				GA_ID, GID_CBACKGROUND,
      	   	           				GA_RelVerify, TRUE,
										GETCOLOR_Screen,scrn,
										GETCOLOR_TitleText,"Background",
										GETCOLOR_Color,param.colours[5],
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "_Background",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,

	   	             				LAYOUT_AddChild, gadgets[GID_CHIGHLIGHT] = GetColorObject,
   	   	              				GA_ID, GID_CHIGHLIGHT,
      	   	           				GA_RelVerify, TRUE,
										GETCOLOR_Screen,scrn,
										GETCOLOR_TitleText,"_Highlight",
										GETCOLOR_Color,param.colours[6],
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "_Highlight",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,

										LayoutEnd,
	       		     					CHILD_WeightedHeight, 0,
									LayoutEnd,
       		     					CHILD_WeightedHeight, 0,
#endif
								LayoutEnd,
							LayoutEnd,

		               		PAGE_Add,LayoutObject,
	    		           		LAYOUT_AddChild,HGroupObject,
	    		           			LAYOUT_AddChild,VGroupObject,
									LAYOUT_BevelStyle,BVS_GROUP,
									LAYOUT_Label,"Labels",
   		             				LAYOUT_AddChild, gadgets[GID_PREV] = StringObject,
      		              				GA_ID, GID_PREV,
         		           				GA_RelVerify, TRUE,
            		        				STRINGA_TextVal,param.prev,
               		     				STRINGA_MaxChars,100,
                						End,
                    					CHILD_Label, LabelObject,
                        				LABEL_Text, "_Previous",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,
	   	             				LAYOUT_AddChild, gadgets[GID_NEXT] = StringObject,
   	   	              				GA_ID, GID_NEXT,
      	   	           				GA_RelVerify, TRUE,
         	   	        				STRINGA_TextVal,param.next,
            	   	     				STRINGA_MaxChars,100,
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "Nex_t",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,
	   	             				LAYOUT_AddChild, gadgets[GID_INDEX] = StringObject,
   	   	              				GA_ID, GID_INDEX,
      	   	           				GA_RelVerify, TRUE,
         	   	        				STRINGA_TextVal,param.index,
            	   	     				STRINGA_MaxChars,100,
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "Inde_x",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,
   	             					LAYOUT_AddChild, gadgets[GID_TOC] = StringObject,
	      	              				GA_ID, GID_TOC,
   	      	           				GA_RelVerify, TRUE,
      	      	        				STRINGA_TextVal,param.toc,
         	      	     				STRINGA_MaxChars,100,
            	    					End,
               	     				CHILD_Label, LabelObject,
                  	      			LABEL_Text, "C_ontents",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,

   		             				LAYOUT_AddChild, gadgets[GID_HELP] = StringObject,
      		              				GA_ID, GID_HELP,
         		           				GA_RelVerify, TRUE,
            		        				STRINGA_TextVal,param.help,
               		     				STRINGA_MaxChars,100,
                						End,
                    					CHILD_Label, LabelObject,
                        				LABEL_Text, "H_elp",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,

	   	             				LAYOUT_AddChild, gadgets[GID_RETRACE] = StringObject,
   	   	              				GA_ID, GID_RETRACE,
      	   	           				GA_RelVerify, TRUE,
         	   	        				STRINGA_TextVal,param.retrace,
            	   	     				STRINGA_MaxChars,100,
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "_Retrace",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,
	   	             				LAYOUT_AddChild, gadgets[GID_HOME] = StringObject,
   	   	              				GA_ID, GID_HOME,
      	   	           				GA_RelVerify, TRUE,
         	   	        				STRINGA_TextVal,param.home,
            	   	     				STRINGA_MaxChars,100,
               	 					End,
                  	  				CHILD_Label, LabelObject,
                     	   			LABEL_Text, "H_ome",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,
   	             					LAYOUT_AddChild, gadgets[GID_FIND] = StringObject,
	      	              				GA_ID, GID_FIND,
   	      	           				GA_RelVerify, TRUE,
      	      	        				STRINGA_TextVal,param.find,
         	      	     				STRINGA_MaxChars,100,
            	    					End,
               	     				CHILD_Label, LabelObject,
                  	      			LABEL_Text, "S_earch",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,
   	             					LAYOUT_AddChild, gadgets[GID_BAR] = StringObject,
	      	              				GA_ID, GID_BAR,
   	      	           				GA_RelVerify, TRUE,
      	      	        				STRINGA_TextVal,param.bar,
         	      	     				STRINGA_MaxChars,10,
            	    					End,
               	     				CHILD_Label, LabelObject,
                  	      			LABEL_Text, "_Bar",
                    					LabelEnd,
           		     					CHILD_NominalSize, TRUE,

									LayoutEnd,
	    		           			LAYOUT_AddChild,VGroupObject,

		                				LAYOUT_AddChild, gadgets[GID_NAVBAR] = ChooserObject,
      	              					GA_ID, GID_NAVBAR,
         	           					GA_RelVerify, TRUE,
					            	      CHOOSER_PopUp,TRUE,
            						      CHOOSER_Labels,&navoptlist,
  				      		            CHOOSER_Selected,navm,
            					        	CHOOSER_AutoFit,TRUE,
            	    					ChooserEnd,
         	           				CHILD_Label, LabelObject,
      	                  			LABEL_Text, "Pos_ition",
	   	                 			LabelEnd,
		                				CHILD_NominalSize, TRUE,

		                				LAYOUT_AddChild, gadgets[GID_IMAGES] = ChooserObject,
      	              					GA_ID, GID_IMAGES,
         	           					GA_RelVerify, TRUE,
					            	      CHOOSER_PopUp,TRUE,
            						      CHOOSER_Labels,&imgoptlist,
  				      		            CHOOSER_Selected,param.images,
            					        	CHOOSER_AutoFit,TRUE,
            	    					ChooserEnd,
         	           				CHILD_Label, LabelObject,
      	                  			LABEL_Text, "_Style",
	   	                 			LabelEnd,
		                				CHILD_NominalSize, TRUE,

		                				LAYOUT_AddChild, gadgets[GID_MOZNAV] = CheckBoxObject,
      	              					GA_ID, GID_MOZNAV,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"_Mozilla site navigation bar",
  				      		            GA_Selected,param.moznav,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,
		                				LAYOUT_AddChild, gadgets[GID_SHOWALL] = CheckBoxObject,
      	              					GA_ID, GID_SHOWALL,
         	           					GA_RelVerify, TRUE,
         	           					GA_Text,"Consistent _across pages",
  				      		            GA_Selected,param.showall,
            					        	CHECKBOX_TextPlace,PLACETEXT_LEFT,
            	    					CheckBoxEnd,
		                				CHILD_NominalSize, TRUE,

									LayoutEnd,
          		     					CHILD_WeightedHeight, 0,

								LayoutEnd,
							PageEnd,

						PageEnd,

       			ClickTabEnd,

                LAYOUT_AddChild, HGroupObject,
                    GA_BackFill, NULL,
                    LAYOUT_SpaceOuter, FALSE,
                    LAYOUT_VertAlignment, LALIGN_CENTER,
                    LAYOUT_HorizAlignment, LALIGN_CENTER,
		              	LAYOUT_AddChild, gadgets[GID_CONV] = ButtonObject,
      	              	GA_ID, GID_CONV,
         	           	GA_RelVerify, TRUE,
            	        	GA_Text,"_Convert",
                		ButtonEnd,
                LayoutEnd,
                CHILD_WeightedHeight, 0,
            EndGroup,
        EndWindow;


         /*  Object creation sucessful?
          */
        if (objects[OID_MAIN])
        {
            /*  Open the window.
             */
            if (windows[WID_MAIN] = (struct Window *) RA_OpenWindow(objects[OID_MAIN]))
            {
			    if ( appwinport = CreateMsgPort() )
				{
					appwin = AddAppWindowA(0,0,windows[WID_MAIN],appwinport,NULL);
				}

                ULONG wait, signal, app = (1L << AppPort->mp_SigBit), appwinsig = (1L << appwinport->mp_SigBit);
                ULONG done = FALSE;
                ULONG result;
                UWORD code;

					menustrip = addmenu(windows[WID_MAIN]);

                 /* Obtain the window wait signal mask.
                 */
                GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);

                /* Input Event Loop
                 */
                while (!done)
                {
                    wait = Wait( signal | SIGBREAKF_CTRL_C | app | appwinsig);

							if(wait & SIGBREAKF_CTRL_C)
								{
                        done = TRUE;
								}
							else if(wait & appwinsig)
							{
								while(appwinmsg = (struct AppMessage *)GetMsg(appwinport))
								{
									switch(appwinmsg->am_Type)
									{
										case AMTYPE_APPWINDOW:
											if(appwinargs = appwinmsg->am_ArgList)
											{
												if(appwinargs->wa_Lock)
												{
													NameFromLock(appwinargs->wa_Lock,ttfrom,1024);
												}
												AddPart(ttfrom,appwinargs->wa_Name,1024);
												param.from=ttfrom;
                                        		SetGadgetAttrs(gadgets[GID_FILE],windows[WID_MAIN],NULL,GETFILE_FullFile,param.from,TAG_DONE);
											}
										break;
									}
								}
								//ReplyMsg(appwinmsg);
							}
							else
								{
                        while ( (result = RA_HandleInput(objects[OID_MAIN], &code) ) != WMHI_LASTMSG )
                        {
                            switch (result & WMHI_CLASSMASK)
                            {
                                case WMHI_CLOSEWINDOW:
                                    windows[WID_MAIN] = NULL;
                                    done = TRUE;
                                    break;

                                case WMHI_MENUPICK:
                                		while (code !=MENUNULL)
                                		{
                                			//menusel = code;
                                			item = ItemAddress(menustrip,code);
                                			menunum = MENUNUM(code);
                                			itemnum = ITEMNUM(code);
                                			subnum = SUBNUM(code);

                                			switch(menunum)
                                			{
                                				case 0:  // Project
                                					switch(itemnum)
                                					{

                                						case 0: // save
																	SetWindowPointer(windows[WID_MAIN],WA_BusyPointer,TRUE,WA_PointerDelay,TRUE,TAG_DONE);
																 	saveas();
																	SetWindowPointer(windows[WID_MAIN],TAG_DONE);
                                						break;

                                						case 2: // about

                                							err(VERS " (" DATE ")\n\n� 1997-8 Richard K�rber\nhttp://www.shredzone.de\n\n� 2001-8 Chris Young\nhttps://www.unsatisfactorysoftware.co.uk","OK",0);
                                						break;

                                						case 4: // quit

					                                    windows[WID_MAIN] = NULL;
               					                     done = TRUE;

                                						break;

                                					}
                                				break;

                                				case 1: // Settings
                                					switch(itemnum)
                                					{
														int i;

														case 0:
															for (i = 0; i < 7; i++) {
																int theme = subnum;
																param.colours[i] = colour_themes[theme][i];
																#ifdef __amigaos4__
																RefreshSetGadgetAttrs(gadgets[GID_CTEXT + i], windows[WID_MAIN], NULL,
																	GETCOLOR_Color, param.colours[i],
																	TAG_DONE);
																#endif
															}
  
														break;
                                						case 1:
                                						 // save defaults
															SetWindowPointer(windows[WID_MAIN],WA_BusyPointer,TRUE,WA_PointerDelay,TRUE,TAG_DONE);
															savetooltypes(defname,1);
															SetWindowPointer(windows[WID_MAIN],TAG_DONE);
                                						break;
                                					}
                                				break;


                                			}


                                			code = item->NextSelect;
                                		}
                                		break;

                                case WMHI_GADGETUP:
                                    switch (result & WMHI_GADGETMASK)
                                    {
#ifdef __amigaos4__
                                        case GID_CTEXT:
                                        	if(DoMethod((Object *)gadgets[GID_CTEXT],GCOLOR_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETCOLOR_Color,gadgets[GID_CTEXT],(ULONG*)&param.colours[0]);
                                        	}

                                          break;
                                        case GID_CSHINE:
                                        	if(DoMethod((Object *)gadgets[GID_CSHINE],GCOLOR_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETCOLOR_Color,gadgets[GID_CSHINE],(ULONG*)&param.colours[1]);
                                        	}

                                          break;
                                        case GID_CSHADOW:
                                        	if(DoMethod((Object *)gadgets[GID_CSHADOW],GCOLOR_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETCOLOR_Color,gadgets[GID_CSHADOW],(ULONG*)&param.colours[2]);
                                        	}

                                          break;
                                        case GID_CFILL:
                                        	if(DoMethod((Object *)gadgets[GID_CFILL],GCOLOR_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETCOLOR_Color,gadgets[GID_CFILL],(ULONG*)&param.colours[3]);
                                        	}

                                          break;
                                        case GID_CFILLTEXT:
                                        	if(DoMethod((Object *)gadgets[GID_CFILLTEXT],GCOLOR_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETCOLOR_Color,gadgets[GID_CFILLTEXT],(ULONG*)&param.colours[4]);
                                        	}

                                          break;
                                        case GID_CBACKGROUND:
                                        	if(DoMethod((Object *)gadgets[GID_CBACKGROUND],GCOLOR_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETCOLOR_Color,gadgets[GID_CBACKGROUND],(ULONG*)&param.colours[5]);
                                        	}

                                          break;
                                        case GID_CHIGHLIGHT:
                                        	if(DoMethod((Object *)gadgets[GID_CHIGHLIGHT],GCOLOR_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETCOLOR_Color,gadgets[GID_CHIGHLIGHT],(ULONG*)&param.colours[6]);
                                        	}

                                          break;
#endif

                                        case GID_FILE:
                                        	if(DoMethod((Object *)gadgets[GID_FILE],GFILE_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETFILE_FullFile,gadgets[GID_FILE],(ULONG*)&param.from);
                                        	}

                                          break;

                                        case GID_TO:
                                        	if(DoMethod((Object *)gadgets[GID_TO],GFILE_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETFILE_Drawer,gadgets[GID_TO],(ULONG*)&param.to);
                                        	}

                                          break;
                                        case GID_HTMLHEADF:
                                        	if(DoMethod((Object *)gadgets[GID_HTMLHEADF],GFILE_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETFILE_FullFile,gadgets[GID_HTMLHEADF],(ULONG*)&param.htmlheadf);
                                        	}
                                      		if(param.htmlheadf)
                                      			{
                                      				if(strlen(param.htmlheadf)==0) param.htmlheadf=0;
                                      			}

                                          break;
                                        case GID_HTMLFOOTF:
                                        	if(DoMethod((Object *)gadgets[GID_HTMLFOOTF],GFILE_REQUEST,windows[WID_MAIN]))
                                        	{
                                        		GetAttr(GETFILE_FullFile,gadgets[GID_HTMLFOOTF],(ULONG*)&param.htmlfootf);
                                        	}
                                      		if(param.htmlfootf)
                                      			{
                                      				if(strlen(param.htmlfootf)==0) param.htmlfootf=0;
															}
                                          break;

                                        case GID_BODY:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_BODY],(ULONG*)&param.bodyext);
                                      		if(strlen(param.bodyext)==0) param.bodyext=0;
                                        break;
                                        case GID_CSS:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_CSS],(ULONG*)&param.cssurl);
                                      		if(strlen(param.cssurl)==0) param.cssurl=0;
                                        break;
                                        case GID_LA:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_LA],(ULONG*)&param.linkadd);
                                      		if(strlen(param.linkadd)==0) param.linkadd=0;
                                        break;
                                        case GID_HOMEURL:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_HOMEURL],(ULONG*)&param.homeurl);
                                      		if(strlen(param.homeurl)==0) param.homeurl=0;
                                        break;

                                        case GID_FINDURL:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_FINDURL],(ULONG*)&param.findurl);
                                      		if(strlen(param.findurl)==0) param.findurl=0;
                                        break;

                                        case GID_RETRACE:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_RETRACE],(ULONG*)&param.retrace);
                                      		if(strlen(param.retrace)==0) param.retrace=0;
                                        break;

                                        case GID_PREV:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_PREV],(ULONG*)&param.prev);
                                      		if(strlen(param.prev)==0) param.prev=0;
                                        break;

                                        case GID_NEXT:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_NEXT],(ULONG*)&param.next);
                                      		if(strlen(param.next)==0) param.next=0;
                                        break;

                                        case GID_INDEX:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_INDEX],(ULONG*)&param.index);
                                      		if(strlen(param.index)==0) param.index=0;
                                        break;

                                        case GID_TOC:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_TOC],(ULONG*)&param.toc);
                                      		if(strlen(param.toc)==0) param.toc=0;
                                        break;

                                        case GID_HELP:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_HELP],(ULONG*)&param.help);
                                      		if(strlen(param.help)==0) param.help=0;
                                        break;

                                        case GID_FIND:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_FIND],(ULONG*)&param.find);
                                      		if(strlen(param.find)==0) param.find=0;
                                        break;

                                        case GID_HOME:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_HOME],(ULONG*)&param.home);
                                      		if(strlen(param.home)==0) param.home=0;
                                        break;

                                        case GID_BAR:
                                      		GetAttr(STRINGA_TextVal,gadgets[GID_BAR],(ULONG*)&param.bar);
                                        break;

                                        case GID_CONV:
                                        	if(!param.from)
                                        		{
                                        			err("You must select an AmigaGuide\ndocument to convert!","OK",0);
                                        		}
                                        		else
                                        		{
																SetWindowPointer(windows[WID_MAIN],WA_BusyPointer,TRUE,TAG_DONE);
                                        			main(1,0);
		  														param.prev = textlabs.prev;
  																param.next = textlabs.next;
  																param.index = textlabs.index;
  																param.toc = textlabs.toc;
  																param.home = textlabs.home;
  																param.help = textlabs.help;
  																param.find = textlabs.find;
  																param.retrace = textlabs.retrace;
		  														param.bar = textlabs.bar;
																SetWindowPointer(windows[WID_MAIN],TAG_DONE);
                                        		}
                                        break;

                                        case GID_WRAP:

														switch(code)
														{
															case 0:
																param.noauto=0;
																param.wordwrap=0;
																param.smartwrap=0;
																SetAttrs(gadgets[GID_VARWIDTH],GA_Disabled,FALSE,TAG_DONE);
													  			RethinkLayout(gadgets[GID_VARWIDTH],windows[WID_MAIN],NULL,TRUE);
															break;

															case 1:
																param.wordwrap=TRUE;
																param.smartwrap=FALSE;
																param.noauto=TRUE;
																SetAttrs(gadgets[GID_VARWIDTH],GA_Disabled,FALSE,TAG_DONE);
													  			RethinkLayout(gadgets[GID_VARWIDTH],windows[WID_MAIN],NULL,TRUE);
															break;

															case 2:
																param.wordwrap=FALSE;
																param.smartwrap=TRUE;
																param.noauto=TRUE;
																SetAttrs(gadgets[GID_VARWIDTH],GA_Disabled,FALSE,TAG_DONE);
													  			RethinkLayout(gadgets[GID_VARWIDTH],windows[WID_MAIN],NULL,TRUE);
															break;

															case 3:
																param.wordwrap=FALSE;
																param.smartwrap=FALSE;
																param.noauto=TRUE;
																SetAttrs(gadgets[GID_VARWIDTH],GA_Disabled,TRUE,TAG_DONE);
													  			RethinkLayout(gadgets[GID_VARWIDTH],windows[WID_MAIN],NULL,TRUE);
															break;

														}

                                        break;

                                        case GID_VARWIDTH:
                                        	param.varwidth=code;
                                        break;

                                        case GID_LINDENT:
                                        	param.lindent=code;
                                        break;

                                        case GID_LINENUMBERS:
                                        	param.numberlines=code;
                                        break;

                                        case GID_SINGLEFILE:
                                        	param.singlefile=code;
											if(param.singlefile)
											{
												SetAttrs(gadgets[GID_LA],GA_Disabled,TRUE,TAG_DONE);
												SetAttrs(gadgets[GID_EXTLINKS],GA_Disabled,TRUE,TAG_DONE);
												//RethinkLayout(gadgets[GID_LA],windows[WID_MAIN],NULL,TRUE);
											}
											else
											{
												SetAttrs(gadgets[GID_LA],GA_Disabled,FALSE,TAG_DONE);
												SetAttrs(gadgets[GID_EXTLINKS],GA_Disabled,FALSE,TAG_DONE);
												//RethinkLayout(gadgets[GID_LA],windows[WID_MAIN],NULL,TRUE);
											}
											RethinkLayout(gadgets[GID_EXTLINKS],windows[WID_MAIN],NULL,TRUE);
                                        break;

                                        case GID_EXTLINKS:
                                        	param.dotdotslash=code;
                                        break;

                                        case GID_NOHTML:
                                        	param.nohtml=code;
                                        break;

                                        case GID_NAVBAR:

														switch(code)
														{
															case 0:
																param.nonavbar=FALSE;
																param.footer=FALSE;
																SetAttrs(gadgets[GID_IMAGES],GA_Disabled,FALSE,TAG_DONE);
													  			RethinkLayout(gadgets[GID_IMAGES],windows[WID_MAIN],NULL,TRUE);
															break;

															case 1:
																param.nonavbar=FALSE;
																param.footer=TRUE;
																SetAttrs(gadgets[GID_IMAGES],GA_Disabled,FALSE,TAG_DONE);
													  			RethinkLayout(gadgets[GID_IMAGES],windows[WID_MAIN],NULL,TRUE);
															break;

															case 2:
																param.nonavbar=TRUE;
																param.footer=FALSE;
																SetAttrs(gadgets[GID_IMAGES],GA_Disabled,TRUE,TAG_DONE);
													  			RethinkLayout(gadgets[GID_IMAGES],windows[WID_MAIN],NULL,TRUE);
															break;
														}

                                        break;

                                        case GID_LINKS:

														switch(code)
														{
															case 0:
																param.noemail=FALSE;
																param.nolink=FALSE;
															break;

															case 1:
																param.noemail=TRUE;
																param.nolink=FALSE;
															break;

															case 2:
																param.noemail=FALSE;
																param.nolink=TRUE;
															break;

															case 3:
																param.noemail=TRUE;
																param.nolink=TRUE;
															break;
														}
                                        break;

                                        case GID_IMAGES:
                                        	param.images=code;
                                        break;

                                        case GID_MOZNAV:
                                        	param.moznav=code;
                                        break;

                                        case GID_SHOWALL:
                                        	param.showall=code;
                                        break;

                                        case GID_MSDOS:
                                        	param.msdos=code;
                                        break;

                                    }
                                    break;

                                case WMHI_ICONIFY:
                                    RA_Iconify(objects[OID_MAIN]);
                                    windows[WID_MAIN] = NULL;
                                    break;

                                case WMHI_UNICONIFY:
                                    windows[WID_MAIN] = (struct Window *) RA_OpenWindow(objects[OID_MAIN]);

                                    if (windows[WID_MAIN])
                                    {
                                        GetAttr(WINDOW_SigMask, objects[OID_MAIN], &signal);
                                    }
                                    else
                                    {
                                        done = TRUE;    // error re-opening window!
                                    }
                                     break;
                            }
                        }
                    }
                }
            }

            /* Disposing of the window object will also close the window if it is
             * already opened, and it will dispose of the layout object attached to it.
             */
            DisposeObject(objects[OID_MAIN]);
        }

		RemoveAppWindow(appwin);
        DeleteMsgPort(appwinport);

        DeleteMsgPort(AppPort);
    }
free_list(&optlist);
freetablist(&tablist);
FreeMenus(menustrip); // menu
cleanup(0);

/*
    closelibs();

    return(0);
*/
 }


void free_list(struct List *list)
{
	struct Node *node,*nextnode;
	node = list ->lh_Head;
	while (nextnode = node->ln_Succ)
	{
		FreeChooserNode(node);
		node=nextnode;
	}
	NewList(list);
}

void freetablist(struct List *list)
{
	struct Node *node,*nextnode;
	node = list ->lh_Head;
	while (nextnode = node->ln_Succ)
	{
		FreeClickTabNode(node);
		node=nextnode;
	}
	NewList(list);
}

BOOL make_list(struct List *list, UBYTE **labels1)
{
	struct Node *node;
	WORD i = 0;
	int ro=FALSE;

	NewList(list);

	while (*labels1)
	{

		if (node = AllocChooserNode(
							CNA_Text, *labels1,
							CNA_ReadOnly, ro,
						TAG_DONE))
		{
			AddTail(list, node);
		}
		else
			break;

		labels1++;
		i++;
		ro=FALSE;
	}
	return(TRUE);
}

BOOL maketablist(struct List *list, UBYTE **labels1)
{
	struct Node *node;
	WORD i = 0;

	NewList(list);

	while (*labels1)
	{

		if (node = AllocClickTabNode(
							TNA_Text, *labels1,
							TNA_Number, i,
						TAG_DONE))
		{
			AddTail(list, node);
		}
		else
			break;

		labels1++;
		i++;
	}
	return(TRUE);
}

struct Menu *addmenu(struct Window *win)
{


APTR vi;
	struct Menu *menustrip;
	struct NewMenu menu[] = {
								{NM_TITLE,"Project"           , 0 ,0,0,0,},
									{ NM_ITEM,"Save As..."        ,"S",0,0,0,},
									{ NM_ITEM,NM_BARLABEL         , 0 ,0,0,0,},
									{ NM_ITEM,"About..."          ,"A",0,0,0,},
									{ NM_ITEM,NM_BARLABEL         , 0 ,0,0,0,},
									{ NM_ITEM,"Quit"              ,"Q",0,0,0,},
								{NM_TITLE,"Settings"          , 0 ,0,0,0,},
									{ NM_ITEM,"Theme Presets"     ,0,0,0,0},
										{ NM_SUB,"AmigaGuide", "0",0,0,0},
										{ NM_SUB,"Light", "1",0,0,0},
										{ NM_SUB,"Dark", "2",0,0,0},
									{ NM_ITEM,"Save As Defaults"   ,"D",0,0,0,},
								{  NM_END,0,0,0,0,0,},
							};

	menustrip = CreateMenus(menu,GTMN_FullMenu,TRUE,TAG_DONE);
	vi = GetVisualInfoA(win->WScreen,TAG_DONE);
	LayoutMenus(menustrip,vi,GTMN_NewLookMenus,TRUE,TAG_DONE);
//				SetAttrs(win,WINDOW_MenuStrip,menustrip,TAG_DONE);
  SetMenuStrip(win,menustrip);

return(menustrip);
}

 void err(STRPTR errtxt,STRPTR gadgtxt,int fail)
{

	int rc;

	if(wb)
	{
		struct EasyStruct errorreq =
			{
			sizeof(struct EasyStruct),
			0,
			"GuideML",
			NULL,
			NULL,
			};

			errorreq.es_TextFormat = errtxt;
			errorreq.es_GadgetFormat = gadgtxt;

		rc = EasyRequest(NULL,&errorreq,NULL);
	}
	else
	{
		printf("%s\n",errtxt);
	}
	if (fail) cleanup(fail);

	// return(rc);


}
#endif

void cleanup(int fail)
{
	if(ttfrom) FreeVec(ttfrom);
#ifndef __HAIKU__
	if(ttto) FreeMem(ttto,strlen(ttto)+1);
	if(tthomeurl) FreeMem(tthomeurl,strlen(tthomeurl)+1);
	if(ttfindurl) FreeMem(ttfindurl,strlen(ttfindurl)+1);
	if(ttprev) FreeMem(ttprev,strlen(ttprev)+1);
	if(ttnext) FreeMem(ttnext,strlen(ttnext)+1);
	if(ttindex) FreeMem(ttindex,strlen(ttindex)+1);
	if(tttoc) FreeMem(tttoc,strlen(tttoc)+1);
	if(tthelp) FreeMem(tthelp,strlen(tthelp)+1);
	if(ttretrace) FreeMem(ttretrace,strlen(ttretrace)+1);
	if(tthome) FreeMem(tthome,strlen(tthome)+1);
	if(ttfind) FreeMem(ttfind,strlen(ttfind)+1);
	if(ttbar) FreeMem(ttbar,strlen(ttbar)+1);
	if(ttbody) FreeMem(ttbody,strlen(ttbody)+1);
	if(tthtmlheadf) FreeMem(tthtmlheadf,strlen(tthtmlheadf)+1);
	if(tthtmlfootf) FreeMem(tthtmlfootf,strlen(tthtmlfootf)+1);
	if(ttcss) FreeMem(ttcss,strlen(ttcss)+1);
	if(tttheme) FreeMem(tttheme,strlen(tttheme)+1);

	if(LocaleBase) {
		CloseLocale(locale);

#ifdef __amigaos4__
		DropInterface((struct Interface *)ILocale);
#endif

		CloseLibrary(LocaleBase);
	}

if(GadToolsBase)
{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IGadTools);
	#endif

	CloseLibrary(GadToolsBase);
}
if(UtilityBase)
{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IUtility);
	#endif

	CloseLibrary(UtilityBase);
}

     if(WindowBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IWindow);
	#endif
 CloseLibrary(WindowBase);
	}
     if(LayoutBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)ILayout);
	#endif
 CloseLibrary(LayoutBase);
	}

     if(ButtonBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IButton);
	#endif
 CloseLibrary(ButtonBase);
	}

     if(CheckBoxBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)ICheckBox);
	#endif
 CloseLibrary(CheckBoxBase);
	}

     if(LabelBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)ILabel);
	#endif
 CloseLibrary(LabelBase);
	}

     if(ChooserBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IChooser);
	#endif
 CloseLibrary(ChooserBase);
	}

     if(ClickTabBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IClickTab);
	#endif
 CloseLibrary(ClickTabBase);
	}

#ifdef __amigaos4__
     if(GetColorBase)
	{
	DropInterface((struct Interface *)IGetColor);
	 CloseLibrary(GetColorBase);
	}
#endif

     if(GetFileBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IGetFile);
	#endif
 CloseLibrary(GetFileBase);
	}

     if(StringBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IString);
	#endif
 CloseLibrary(StringBase);
	}

     if(SliderBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)ISlider);
	#endif
 CloseLibrary(SliderBase);
	}

     if(GadToolsBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IGadTools);
	#endif
 CloseLibrary(GadToolsBase);
	}

     if(IconBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IIcon);
	#endif
 CloseLibrary(IconBase);
	}

     if(AslBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IAsl);
	#endif
 CloseLibrary(AslBase);
	}

     if(DOSBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IDOS);
	#endif
 CloseLibrary(DOSBase);
	}

     if(WorkbenchBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IWorkbench);
	#endif
 CloseLibrary(WorkbenchBase);
	}

     if(IntuitionBase)
	{
	#ifdef __amigaos4__
	DropInterface((struct Interface *)IIntuition);
	#endif
 CloseLibrary(IntuitionBase);
	}

#endif

 exit(fail);

 }
