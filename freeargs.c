/*
    Copyright © 1995-2007, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Free arguments structure from ReadArgs()
    Lang: english
*/
#include <proto/exec.h>
#include <dos/rdargs.h>
#include "dos/dos.h"

/*****************************************************************************

    NAME */
#include <proto/dos.h>

void FreeArgs(struct RDArgs * args)

/*  FUNCTION
        FreeArgs() will clean up after a call to ReadArgs(). If the
        RDArgs structure was allocated by the system in a call to 
        ReadArgs(), then it will be freed. If however, you allocated
        the RDArgs structure with AllocDosObject(), then you will
        have to free it yourself with FreeDosObject().

    INPUTS
        args            - The data used by ReadArgs(). May be NULL,
                          in which case, FreeArgs() does nothing.

    RESULT
        Some memory will have been returned to the system.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO
        ReadArgs()

    INTERNALS

*****************************************************************************/
{
    if(!args)
        return;

    /* ReadArgs() failed. Clean everything up. */
    if (args->RDA_DAList)
    {
        FreeVec(((struct DAList *)args->RDA_DAList)->ArgBuf);
        FreeVec(((struct DAList *)args->RDA_DAList)->StrBuf);
        FreeVec(((struct DAList *)args->RDA_DAList)->MultVec);
        FreeVec((struct DAList *)args->RDA_DAList);

        args->RDA_DAList = 0;
        
#if 0
        /*
            Why do I put this here. Unless the user has been bad and
            set RDA_DAList to something other than NULL, then this
            RDArgs structure was allocated by ReadArgs(), so we can
            free it. Otherwise the RDArgs was allocated by
            AllocDosObject(), so we are not allowed to free it.

            See the original AmigaOS autodoc if you don't believe me
        */

        FreeVec(args);
#endif
    }

    if (args->RDA_Flags & RDAF_ALLOCATED_BY_READARGS)
        FreeVec(args);

} /* FreeArgs */
