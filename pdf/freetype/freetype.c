/* This file is part of the FreeType project */

/* Single file library component for the ANSI target */
#define TT_MAKE_OPTION_SINGLE_OBJECT

/* first include common core components */

#ifdef __BORLANDC__

#pragma warn -8057 /* Parameter never used ... */
#pragma warn -8071 /* Conversion may lose ... */

#endif

#include "ttapi.c"
#include "ttcache.c"
#include "ttcalc.c"
#include "ttcmap.c"
#include "ttdebug.c"
#include "ttgload.c"
#include "ttinterp.c"
#include "ttload.c"
#include "ttobjs.c"
#include "ttraster.c"

/* then system-specific (or ANSI) components */

#include "ttfile.c"
#include "ttmemory.c"
#include "ttmutex.c"

/* the extensions are compiled separately, but we need to */
/* include the file ttextend.c if we want to support them */

#ifdef TT_CONFIG_OPTION_EXTEND_ENGINE
#include "ttextend.c"
#endif

/* END */
