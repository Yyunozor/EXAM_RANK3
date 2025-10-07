#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	int i = 0;
	while (s && s[i] && s[i] != c)
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
	size_t ret = 0;
	while (s[ret])
	{
		ret++;
	}
	return (ret);
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
	size_t size1 = (*s1) ? ft_strlen(*s1) : 0;
	char *tmp = malloc(size2 + size1 + 1);
	if (!tmp)
		return 0;
	if(*s1)	
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
	if(dest == src || n == 0)
		return dest;
	unsigned char *d = (unsigned char*)dest;
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
	static char buffer[BUFFER_SIZE + 1] = "";
	char	*ret = NULL;
	char	*tmp = NULL;
	int		read_ret;

	if(buffer[0] == '\0')
	{
		read_ret = read(fd, buffer, BUFFER_SIZE);
		if (read_ret <= 0)
			return NULL;
		buffer[read_ret] = '\0';
	}
	while(!tmp)
	{
		if(!str_append_str(&ret, buffer))
			return NULL;
		read_ret = read(fd, buffer, BUFFER_SIZE);
		if(read_ret <= 0)
		{
			buffer[0] = '\0';
			if(ret && *ret)
				return ret;
			free(ret);
			return NULL;
		}
		buffer[read_ret] = '\0';
		tmp = ft_strchr(buffer, '\n');
	}
	if (!str_append_mem(&ret, buffer, (size_t)(tmp - buffer + 1)))
	{
		free(ret);
		return NULL;
	}
	{
		size_t remain = ft_strlen(tmp + 1);
		ft_memmove(buffer, tmp + 1, remain + 1);
	}
	return ret;
}

#include <stdio.h>
#include <fcntl.h>

int main(void)
{
	int fd = open("text.txt", O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return 1;
	}
	char *line;
	while ((line = get_next_line(fd)))
	{
		printf("%s", line);
		free(line);
	}
	close(fd);
	return 0;
}
