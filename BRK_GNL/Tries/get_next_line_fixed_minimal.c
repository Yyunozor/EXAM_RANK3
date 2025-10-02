#include "get_next_line.h"

/* Minimal diff fixed version.
 * Only lines that required correction were changed.
 * Each changed block tagged with // FIX: ...
 */

char *ft_strchr(char *s, int c)
{
	int i = 0;
	while (s[i] && s[i] != c)           // FIX: add s[i] check to avoid reading past '\0'
		i++;
	if (s[i] == c)
		return s + i;
	return NULL;                        // FIX: else branch removed (simplify)
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t i = 0;                       // FIX: rewrite algorithm forward (original skipped first byte)
	while (i < n)
	{
		((unsigned char*)dest)[i] = ((const unsigned char*)src)[i];
		i++;
	}
	return dest;
}

size_t ft_strlen(char *s)
{
	size_t ret = 0;
	while (s && *s)                     // FIX: guard NULL (first append case)
	{
		s++;
		ret++;
	}
	return ret;
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
	size_t size1;
	if (*s1)
		size1 = ft_strlen(*s1);
	else
		size1 = 0;
	char *tmp = malloc(size2 + size1 + 1);
	if (!tmp)
		return 0;
	if (*s1)                                   // FIX: copy only if *s1 not NULL
		ft_memcpy(tmp, *s1, size1);
	ft_memcpy(tmp + size1, s2, size2);
	tmp[size1 + size2] = 0;
	free(*s1);
	*s1 = tmp;
	return 1;
}

int str_append_str(char **s1, char *s2)
{
	return str_append_mem(s1, s2, ft_strlen(s2));
}

void *ft_memmove(void *dest, const void *src, size_t n)
{
	if (dest == src || n == 0)                 // FIX: quick exits
		return dest;
	unsigned char *d = (unsigned char*)dest;   // FIX: proper overlap-safe implementation
	const unsigned char *s = (const unsigned char*)src;
	if (d < s || d >= s + n)
		return ft_memcpy(dest, src, n);
	while (n > 0)
	{
		n--;
		d[n] = s[n];
	}
	return dest;
}

char *get_next_line(int fd)
{
	static char b[BUFFER_SIZE + 1] = "";
	char *ret = NULL;
	char *tmp;
	int read_ret;

	if (b[0] == '\0')                          // FIX: perform initial read if buffer empty
	{
		read_ret = read(fd, b, BUFFER_SIZE);
		if (read_ret <= 0)
			return NULL;
		b[read_ret] = '\0';
	}
	tmp = ft_strchr(b, '\n');
	while (!tmp)
	{
		if (!str_append_str(&ret, b))          // accumulate chunk without newline
			return NULL;
		read_ret = read(fd, b, BUFFER_SIZE);
		if (read_ret <= 0)                     // FIX: handle EOF (<=0) and return partial line if any
		{
			b[0] = '\0';
			if (ret && *ret)
				return ret;
			free(ret);
			return NULL;
		}
		b[read_ret] = '\0';
		tmp = ft_strchr(b, '\n');
	}
	if (!str_append_mem(&ret, b, (size_t)(tmp - b + 1))) // append line including '\n'
	{
		free(ret);
		return NULL;
	}
	// FIX: shift leftover data in static buffer
	{
		size_t remain = ft_strlen(tmp + 1);
		ft_memmove(b, tmp + 1, remain + 1);     // include '\0'
	}
	return ret;
}
