#include "get_next_line.h"

/*
Corrections vs broken version:
1. ft_strchr: stop on '\0' (avoid overflow)
2. ft_memcpy: forward copy loop (fixed reverse/off-by-one)
3. str_append_mem: handle *s1 == NULL safely
4. ft_memmove: proper overlap handling (no ft_strlen misuse / size_t underflow)
5. get_next_line: initial read if buffer empty + loop logic updated
6. On newline: append chunk, shift remainder in static buffer
7. EOF: return accumulated partial line else NULL
*/

static char *gnl_strchr(char *s, int c)
{
    while (*s && *s != (char)c)
        s++;
    if (*s == (char)c)
        return s;
    return NULL;
}

static size_t gnl_strlen(char *s)
{
    size_t n = 0; while (s && s[n]) n++; return n;
}

static void *gnl_memcpy(void *dst, const void *src, size_t n)
{
    size_t i = 0; while (i < n) { ((unsigned char*)dst)[i] = ((unsigned char*)src)[i]; i++; } return dst;
}

static void *gnl_memmove(void *dst, const void *src, size_t n)
{
    if (!dst || !src || dst == src || n == 0) return dst;
    unsigned char *d = (unsigned char*)dst; const unsigned char *s = (const unsigned char*)src;
    if (d < s || d >= s + n) return gnl_memcpy(dst, src, n);
    while (n--) d[n] = s[n];
    return dst;
}

static int str_append_mem(char **s1, char *s2, size_t n2)
{
    size_t n1 = *s1 ? gnl_strlen(*s1) : 0;
    char *tmp = (char*)malloc(n1 + n2 + 1);
    if (!tmp) return 0;
    if (*s1) gnl_memcpy(tmp, *s1, n1);
    gnl_memcpy(tmp + n1, s2, n2);
    tmp[n1 + n2] = '\0';
    free(*s1); *s1 = tmp; return 1;
}

static int str_append_str(char **s1, char *s2)
{ return str_append_mem(s1, s2, gnl_strlen(s2)); }

char *get_next_line(int fd)
{
    static char buf[BUFFER_SIZE + 1];
    char *line = NULL; char *nl; int r;

    if (buf[0] == '\0') { r = read(fd, buf, BUFFER_SIZE); if (r <= 0) return NULL; buf[r] = '\0'; }
    while (1)
    {
        nl = gnl_strchr(buf, '\n');
        if (nl)
        {
            if (!str_append_mem(&line, buf, (size_t)(nl - buf + 1))) { free(line); return NULL; }
            size_t rest = gnl_strlen(nl + 1);
            gnl_memmove(buf, nl + 1, rest + 1);
            return line;
        }
        if (!str_append_str(&line, buf)) { free(line); return NULL; }
        r = read(fd, buf, BUFFER_SIZE);
        if (r <= 0)
        {
            buf[0] = '\0';
            if (line && *line) return line;
            free(line); return NULL;
        }
        buf[r] = '\0';
    }
}
