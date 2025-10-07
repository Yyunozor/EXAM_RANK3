#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	int i = 0;
	while (s && s[i] && s[i] != c)
		i++;
	if (s && s[i] == c)
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

int str_append_str(char **s1, char *s2)
{
	return str_append_mem(s1, s2, ft_strlen(s2));
}

void *ft_memmove(void *dest, const void *src, size_t n)
{
	size_t				i;
	unsigned char	 	*d;
	const unsigned char *s;

	if(dest == src || n == 0)
		return dest;
	d = (unsigned char*)dest;
	s = (const unsigned char*)src;
	if(d < s)
	{
		i = 0;
		while (i < n)
		{
			d[i] = s[i];
			i++;
		}
	}
	else{
		while(n > 0)
		{
			n--;
			d[n] = s[n];
		}
	}
	return 				dest;
}

char *get_next_line(int fd)
{
	static char buf[BUFFER_SIZE + 1]; /* buffer persistant */
	char		*line = NULL;
	char		*nl;
	int			rd;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return NULL;
	while (1)
	{ /* Step 1: remplir le buffer si vide */
		if (buf[0] == '\0')
		{
			rd = read(fd, buf, BUFFER_SIZE);
			if (rd <= 0)
			{
				if (line && *line) /* dernière ligne sans \n */
					return line;
				free(line);
				return NULL;
			}
			buf[rd] = '\0';
		} /* Step 2: chercher newline */
		nl = ft_strchr(buf, '\n');
		if (nl)
		{
			if (!str_append_mem(&line, buf, (size_t)(nl - buf + 1)))
				return (free(line), NULL); /* Step 3: décaler le reste */
			ft_memmove(buf, nl + 1, ft_strlen(nl + 1) + 1);
			return line;
		} /* Step 4: pas de \n -> append tout le buffer et on relira */
		if (!str_append_mem(&line, buf, ft_strlen(buf)))
			return (free(line), NULL);
		buf[0] = '\0'; /* marquer consommé */
	}
}
