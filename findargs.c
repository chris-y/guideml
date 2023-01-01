/*
    Copyright © 1995-2007, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: english
*/
#include <proto/utility.h>
#include "dos/dos.h"

/*****************************************************************************

    NAME */
#include <proto/dos.h>
#include <proto/exec.h>

LONG FindArg(CONST_STRPTR template, CONST_STRPTR keyword)

/*  FUNCTION
        Search for keyword in the template string.
        Abbreviations are handled.

    INPUTS
        template - template string to be searched
        keyword  - keyword to search for

    RESULT
        Index of the keyword or -1 if not found.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    LONG count=0;
    CONST_STRPTR key;

    /* Loop over template */
    for(;;)
    {
        /* Compare key to template */
        key=keyword;
        for(;;)
        {
            UBYTE lkey;

            /* If the keyword has ended check the template */
            if(!*key)
            {
                if(!*template||*template=='='||*template=='/'||*template==',')
                    /* The template has ended, too. Return count. */
                    return count;
                /* The template isn't finished. Stop comparison. */
                break;
            }
            /* If the two differ stop comparison. */
            lkey=ToLower(*key);
            if(lkey!=ToLower(*template))
                break;
            /* Go to next character */
            key++;
            template++;
        }
        /* Find next keyword in template */
        for(;;)
        {
            if(!*template)
                return -1;
            if(*template=='=')
            {
                /* Alias found */
                template++;
                break;
            }
            if(*template==',')
            {
                /* Next item found */
                template++;
                count++;
                break;
            }
            template++;
        }
    }
} /* FindArg */
