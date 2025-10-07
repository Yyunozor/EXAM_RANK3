#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	int i = 0;
	if (!s)
		return NULL;
	while (s[i] && s[i] != (char)c)
		i++;
	if (s[i] == (char)c)
		return s + i;
	return NULL;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t i = 0;
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
	while (s && *s)
	{
		s++;
		ret++;
	}
	return (ret);
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
	size_t size1 = (*s1) ? ft_strlen(*s1) : 0;
	char *tmp = (char *)malloc(size1 + size2 + 1);
	if (!tmp)
		return 0;
	if (*s1)
		ft_memcpy(tmp, *s1, size1);
	ft_memcpy(tmp + size1, s2, size2);
	tmp[size1 + size2] = '\0';
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
	if (dest == src || n == 0)
		return dest;
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;
	if (d < s || d >= s + n)
		return ft_memcpy(dest, src, n);
	while (n-- > 0)
		d[n] = s[n];
	return dest;
}

char *get_next_line(int fd)
{
	static char buf[BUFFER_SIZE + 1] = "";
	char *line = NULL;
	char *nl;
	int  rd;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return NULL;
	if (buf[0] == '\0')
	{
		rd = read(fd, buf, BUFFER_SIZE);
		if (rd <= 0)
			return NULL;
		buf[rd] = '\0';
	}
	while (1)
	{
		nl = ft_strchr(buf, '\n');
		if (nl)
		{
			if (!str_append_mem(&line, buf, (size_t)(nl - buf + 1)))
			{
				free(line);
				return NULL;
			}
			ft_memmove(buf, nl + 1, ft_strlen(nl + 1) + 1);
			return line;
		}
		if (!str_append_mem(&line, buf, ft_strlen(buf)))
		{
			free(line);
			return NULL;
		}
		rd = read(fd, buf, BUFFER_SIZE);
		if (rd <= 0)
		{
			buf[0] = '\0';
			if (line && *line)
				return line;
			free(line);
			return NULL;
		}
		buf[rd] = '\0';
	}
}

int main(void)
{
	int fd = open("test.txt", O_RDONLY);
	char *line;

	if (fd < 0)
		return 1;
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("%s", line);
		free(line);
	}
	close(fd);
	return 0;
}
