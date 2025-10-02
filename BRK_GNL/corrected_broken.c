#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	int i = 0;
	// FIX 1: ORIGINAL BUG - while (s[i] != c) pouvait dépasser la fin de chaîne
	// CORRECTION: Ajout de vérification s[i] != '\0' pour éviter buffer overflow
	while (s[i] != '\0' && s[i] != c)
		i++;
	if (s[i] == c)
		return s + i;
	else
		return NULL;
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	// FIX 2: ORIGINAL BUG - while (--n > 0) avec ((char*)dest)[n - 1] copiait à l'envers et sautait le dernier élément
	// CORRECTION: Logique de copie séquentielle correcte de 0 à n-1
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
	size_t size1;
	// FIX 3: ORIGINAL BUG - ft_strlen(*s1) appelé sans vérifier si *s1 est NULL
	// CORRECTION: Vérification si s1 est NULL avant d'appeler ft_strlen
	if (*s1 == NULL)
		size1 = 0;
	else
		size1 = ft_strlen(*s1);
	char *tmp = malloc(size2 + size1 + 1);
	if (!tmp)
		return 0;
	// FIX 4: ORIGINAL BUG - ft_memcpy(tmp, *s1, size1) appelé même si *s1 est NULL
	// CORRECTION: Ne copier que si s1 n'est pas NULL
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
	// FIX BONUS: ORIGINAL BUG - Logique complètement incorrecte avec ft_strlen sur src
	// CORRECTION: Implémentation correcte de memmove avec gestion des overlaps
	unsigned char *d = dest;
	const unsigned char *s = src;

	if (d == s || n == 0)
		return dest;
	if (d < s)
	{
		size_t i = 0;
		while (i < n)
		{
			d[i] = s[i];
			i++;
		}
	}
	else // Copy backwards
	{
		size_t i = n;
		while (i > 0)
		{
			d[i - 1] = s[i - 1];
			i--;
		}
	}
	return dest;
}

char *get_next_line(int fd)
{
	static char b[BUFFER_SIZE + 1] = "";
	char *ret = NULL;

	// FIX 5: ORIGINAL BUG - Pas de lecture initiale, ft_strchr appelé sur buffer vide
	// CORRECTION: Lecture initiale si buffer vide
	if (b[0] == '\0')
	{
		int read_ret = read(fd, b, BUFFER_SIZE);
		if (read_ret <= 0)
			return NULL;
		b[read_ret] = '\0';
	}

	char *tmp = ft_strchr(b, '\n');
	while (!tmp)
	{
		if (!str_append_str(&ret, b))
			return NULL;
		int read_ret = read(fd, b, BUFFER_SIZE);
		// FIX 5b: ORIGINAL BUG - if (read_ret == -1) seulement, pas de gestion EOF
		// CORRECTION: Gestion correcte EOF (read_ret <= 0) et retour ligne partielle
		if (read_ret <= 0)
		{
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
	
	// FIX 6: ORIGINAL BUG - Buffer statique jamais mis à jour, données perdues
	// CORRECTION: Décaler le contenu restant du buffer après la ligne lue
	size_t remaining = ft_strlen(tmp + 1);
	size_t i = 0;
	while (i <= remaining)
	{
		b[i] = tmp[1 + i];
		i++;
	}
	
	return ret;
}

int main(int argc, char *argv[])
{
	int fd;
	char *line;
	(void)argc;

	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		return (1);
	}
	while ((line = get_next_line(fd)) != NULL)
	{
		printf("LINE: %s", line);
		free(line);
	}
	close(fd);
	return (0);
}