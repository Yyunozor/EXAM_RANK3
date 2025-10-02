#include "get_next_line.h"
/*
============================================================
GET_NEXT_LINE - FIXED VERSION (COMMENTED)
============================================================
Each FIX block explains exactly what was wrong in the original
broken version (see `broken_get_next_line.c`) and how it is
corrected here. No stylistic refactors beyond what is needed
for correctness & safety.
============================================================
*/

/*----------------------------------------------------------
FIX 1: ft_strchr
ORIGINAL BUG: loop `while (s[i] != c)` never checked end of string.
If `c` not present -> reads past '\0' (undefined behaviour).
SOLUTION: stop when *s == '\0'. Return NULL if not found.
----------------------------------------------------------*/
static char *ft_strchr_safe(char *s, int c)
{
    int i = 0;
    while (s[i] && s[i] != (char)c)
        i++;
    if (s[i] == (char)c)
        return s + i;
    return NULL;
}

/*----------------------------------------------------------
FIX 2: ft_memcpy
ORIGINAL BUG: loop copied backwards with `while (--n > 0)` and
used index `n-1`. This skipped copying byte 0 and produced
incorrect results for n == 1. Also unusual direction.
SOLUTION: standard forward copy from 0..n-1.
----------------------------------------------------------*/
static void *ft_memcpy_safe(void *dest, const void *src, size_t n)
{
    size_t i = 0;
    while (i < n)
    {
        ((unsigned char*)dest)[i] = ((const unsigned char*)src)[i];
        i++;
    }
    return dest;
}

/*----------------------------------------------------------
Utility: ft_strlen (unchanged logic, safe with non-NULL input
as in original usage; we still guard NULL before calling it elsewhere).
----------------------------------------------------------*/
static size_t ft_strlen_safe(char *s)
{
    size_t len = 0;
    while (s && s[len])
        len++;
    return len;
}

/*----------------------------------------------------------
FIX 3: str_append_mem - NULL dereference risk
ORIGINAL BUG: `size_t size1 = ft_strlen(*s1);` but *s1 may be NULL
(on first append) -> undefined behaviour.
SOLUTION: treat NULL as empty string.

FIX 4: Copy from NULL
ORIGINAL BUG: unconditionally `ft_memcpy(tmp, *s1, size1);`
when *s1 could be NULL.
SOLUTION: only copy if *s1 != NULL.
----------------------------------------------------------*/
static int str_append_mem_safe(char **s1, char *s2, size_t size2)
{
    size_t size1 = (*s1) ? ft_strlen_safe(*s1) : 0;
    char *tmp = (char*)malloc(size1 + size2 + 1);
    if (!tmp)
        return 0;
    if (*s1)
        ft_memcpy_safe(tmp, *s1, size1);
    ft_memcpy_safe(tmp + size1, s2, size2);
    tmp[size1 + size2] = '\0';
    free(*s1);
    *s1 = tmp;
    return 1;
}

static int str_append_str_safe(char **s1, char *s2)
{
    return str_append_mem_safe(s1, s2, ft_strlen_safe(s2));
}

/*----------------------------------------------------------
FIX 5: ft_memmove
ORIGINAL BUGS:
 - Used ft_strlen on raw memory (invalid if not NUL-terminated).
 - Calculated `size_t i = ft_strlen(src) - 1;` underflows if src == "".
 - Loop `while (i >= 0)` with unsigned size_t never ends (wrap-around).
 - Wrong overlap handling logic.
SOLUTION: standard memmove algorithm:
 - If regions do not overlap dangerously -> forward copy.
 - Else copy backwards.
----------------------------------------------------------*/
static void *ft_memmove_safe(void *dest, const void *src, size_t n)
{
    if (dest == src || n == 0)
        return dest;
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    if (d < s || d >= s + n) // no harmful overlap
        return ft_memcpy_safe(dest, src, n);
    while (n-- > 0)
        d[n] = s[n];
    return dest;
}

/*----------------------------------------------------------
FIX 6–8: get_next_line core logic
ORIGINAL ISSUES:
 - No initial read: started searching newline in an empty static buffer.
 - On EOF (read_ret == 0) treated as if data still pending -> possible infinite loop / missing last fragment.
 - Never shifted leftover buffer after returning a line -> subsequent calls reuse stale data.
 - Did not return partial last line if file ended without '\n'.
SOLUTION STEPS:
 1. Initial read if buffer empty.
 2. Loop: search '\n'. If found -> append portion, shift remainder (memmove), return line.
 3. If not found -> append whole buffer, read more.
 4. On read <= 0: return accumulated line if non-empty else NULL; clear buffer.
----------------------------------------------------------*/
char *get_next_line(int fd)
{
    static char buf[BUFFER_SIZE + 1] = {0};
    char *line = NULL;
    char *nl;
    int rd;

    if (buf[0] == '\0')
    {
        rd = read(fd, buf, BUFFER_SIZE);
        if (rd <= 0)
            return NULL;
        buf[rd] = '\0';
    }
    while (1)
    {
        nl = ft_strchr_safe(buf, '\n');
        if (nl)
        {
            if (!str_append_mem_safe(&line, buf, (size_t)(nl - buf + 1)))
            { free(line); return NULL; }
            size_t rest = ft_strlen_safe(nl + 1);
            ft_memmove_safe(buf, nl + 1, rest + 1); // include '\0'
            return line;
        }
        if (!str_append_str_safe(&line, buf))
        { free(line); return NULL; }
        rd = read(fd, buf, BUFFER_SIZE);
        if (rd <= 0)
        {
            buf[0] = '\0';
            if (line && *line)
                return line; // return last partial line
            free(line);
            return NULL;
        }
        buf[rd] = '\0';
    }
}
