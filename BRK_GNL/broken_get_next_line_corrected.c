#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	// FIX 1: Version simplifiée avec une seule boucle
	while (*s && *s != c)
		s++;
	if (*s == c)
		return s;
	return NULL;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	// BUG ORIGINAL: while (--n > 0) avec ((char*)dest)[n - 1] = ((char*)src)[n - 1]
	// PROBLÈMES: 1) Copie à l'envers 2) Saute le dernier élément (n-1 quand n devient 0)
	// CORRECTION: Copie séquentielle de 0 à n-1
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
	while (*s)
	{
		s++;
		ret++;
	}
	return (ret);
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
	// FIX 3&4: Combiné en une seule vérification + initialisation directe
	size_t size1;
	if (*s1)
		size1 = ft_strlen(*s1);
	else
		size1 = 0;
	char *tmp = malloc(size2 + size1 + 1);
	
	if (!tmp)
		return 0;
	if (*s1)
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
	// FIX 5: Version simplifiée - utilise memcpy si pas d'overlap dangereux
	if (dest == src || n == 0)
		return dest;
	if (dest < src || dest >= (char*)src + n)
		return ft_memcpy(dest, src, n);
	
	// Overlap: copie à l'envers
	char *d = dest;
	const char *s = src;
	while (n--)
		d[n] = s[n];
	return dest;
}

char *get_next_line(int fd)
{
	static char b[BUFFER_SIZE + 1] = "";
	char *ret = NULL;
	int read_ret;

	// FIX 6: Lecture initiale simplifiée
	if (!b[0])
	{
		read_ret = read(fd, b, BUFFER_SIZE);
		if (read_ret <= 0)
			return NULL;
		b[read_ret] = '\0';
	}

	char *tmp = ft_strchr(b, '\n');
	while (!tmp)
	{
		if (!str_append_str(&ret, b))
			return NULL;
		read_ret = read(fd, b, BUFFER_SIZE);
		if (read_ret <= 0)
		{
			// FIX 7: Sans ternaire
			if (ret && *ret)
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
	
	// FIX 8: Utiliser ft_memmove au lieu de boucle manuelle
	size_t remaining = ft_strlen(tmp + 1);
	ft_memmove(b, tmp + 1, remaining + 1);
	
	return ret;
}