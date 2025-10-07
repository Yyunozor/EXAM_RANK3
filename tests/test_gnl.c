#include "../BRK_GNL/get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

static size_t slen(const char *s)
{
    size_t n = 0; while (s && s[n]) n++; return n;
}

int main(void)
{
    int fd = open("tests/test_input_gnl.txt", O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }
    char *s; int i = 0;
    while ((s = get_next_line(fd)))
    {
        printf("[%d]: %s", i, s);
        size_t l = slen(s);
        if (l && s[l-1] != '\n')
            printf("<NO_NL>\n");
        i++;
        free(s);
    }
    close(fd);
    return 0;
}
