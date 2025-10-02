#include "get_next_line.h"
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

char *ft_strchr(char *s, int c)
{
	int i = 0;
	if (!s)
		return NULL;
	while (s[i] && s[i] != c)
		i++;
	if (s[i] == c)
		return s + i;
	return NULL;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t i = 0;
	while (i < n)
	{
		((char*)dest)[i] = ((char*)src)[i];
		i++;		
	}
	return dest;
}

size_t ft_strlen(char *s)
{
	size_t i = 0;
	if (!s)
		return 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
	
	size_t size1;
	if(*s1)
		size1 = ft_strlen(*s1);
	else
		size1 = 0;
	char *tmp = malloc(size2 + size1 + 1);
	if (!tmp)
		return 0;
	if(*s1)
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
	if (dest == src || n == 0)
		return dest;
	unsigned char *d = (unsigned char*)dest;
	const unsigned char *s = (const unsigned char *)src;
	if(d < s || d >= s + n)
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

	if (fd < 0 || BUFFER_SIZE <= 0)
		return NULL;

	char *tmp = ft_strchr(b, '\n');
	while (!tmp)
	{
		if (ft_strlen(b) > 0)
		{
			if (!str_append_str(&ret, b))
			{
				free(ret);
				return NULL;
			}
		}
		int read_ret = read(fd, b, BUFFER_SIZE);
		if (read_ret == -1)
		{
			free(ret);
			b[0] = '\0';  // Clear buffer on error
			return NULL;
		}
		if (read_ret == 0)
		{
			b[0] = '\0';  // Clear buffer when EOF reached
			if (ret && ft_strlen(ret) > 0)
				return ret;
			free(ret);
			return NULL;
		}
		b[read_ret] = '\0';
		tmp = ft_strchr(b, '\n');
	}
	if (!str_append_mem(&ret, b, tmp - b + 1))
	{
		free(ret);
		return NULL;
	}
	ft_memmove(b, tmp + 1, ft_strlen(tmp + 1) + 1);
	return ret;
}

int	main(void)
{
	int fd = open("text.txt", O_RDONLY);
	if(fd == -1)
	{
		perror("Error opening file");
		return 1;
	}
	char *line;
	int line_num = 1;
	while ((line = get_next_line(fd)))
	{
		printf("line %d: %s", line_num++, line);
		free(line);
	}
	close(fd);
	return 0;
}