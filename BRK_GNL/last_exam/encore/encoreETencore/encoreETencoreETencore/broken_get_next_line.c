#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	int i = 0;
	unsigned char uc = c;
	while (s[i] != uc)
		i++;
	if (s[i] == uc)
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
		ret++;
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

void *ft_memmove(void *dest, const void *src, size_t n)
{
	if (dest == src || n == 0)
		return dest;
	unsigned char *d = dest;
	const unsigned char *s = src;
	size_t i = 0;
	if(d < s)
	{
		while(i < n)
		{
			d[i] = s[i];
			i++;
		}
	}
	else
		while(n > 0)
		{
			n--;
			d[n] = s[n];
		}
	return dest;
}

char *get_next_line(int fd)
{
	static char buf[BUFFER_SIZE + 1] = "";
	char *line = NULL;
	char *nl;
	int rd;

	if(fd < 0 || BUFFER_SIZE <= 0)
		return NULL;
	
	while(1)
	{
		if(buf[0] == '\0')
		{
			rd = read(fd, buf, BUFFER_SIZE);
			if(rd < 0)
			{
				free(line);
				return NULL;
			}
			if(rd == 0)
			{
				if(line && *line)
					return (line);
				free(line);
				return NULL;
			}
		}
		nl = ft_strchr(buf, '\n');
		if(nl)
		{
			if(!str_append_mem(&line, buf, (size_t)(nl - buf + 1)))
				return(free(line), NULL);
			ft_memmove(buf, nl + 1, ft_strlen(nl + 1) + 1);
			return line;
		}
		if(!str_append_mem(&line, buf, ft_strlen(buf)))
			return(free(line), NULL);
		buf[0] = '\0';
	}

}

int	main(int ac, char **av)
{
	if(ac != 2)
		return 1;
	int fd;
	char *line;

	fd = open(av[1], O_RDONLY);
	if(fd < 0)
		return 1;
	while((line = get_next_line(fd)))
	{
		printf("%s", line);
		free(line);
	}
	if (ac == 2)
		close(fd);
	return 0;
}