# get_next_line Broken ➜ Fixed Cheat Sheet (Adapté au sujet broken_GNL)

## 0. Contraintes du sujet (rappel rapide examen)
- Fichiers à rendre: `get_next_line.c` `get_next_line.h`
- Fonctions autorisées: `read`, `malloc`, `free`
- Interdits dans le rendu: `open`, `close`, `printf`, autres libs non nécessaires
- Prototype: `char *get_next_line(int fd);`
- Aucune allocation conservée entre deux EOF (hors buffer statique)
- Retourne ligne (incluant `\n` si présent), sinon `NULL`
- Gestion dernière ligne sans `\n`
- Appels successifs lisent séquentiellement le fd
- Ajouter un guard: `if (fd < 0 || BUFFER_SIZE <= 0)` -> `return NULL;`

## 1. Plan d'action express

1. Cartographier les helpers (`ft_strlen`, `ft_strchr`, `ft_memcpy`, `ft_memmove`, `str_append_mem`, `get_next_line`).
2. Corriger chaque utilitaire dans l'ordre pour éliminer les UB (lecture hors limites, NULL deref, overlap).
3. Sécuriser `str_append_mem` (gestion malloc/free, pointeurs NULL).
4. Repenser `get_next_line` : lecture incrémentale, détection de `\n`, gestion EOF, leftover avec `memmove`.
5. Tester rapidement avec un petit `main`, différents `BUFFER_SIZE`.

## 2. Détails fonction par fonction

### ft_strchr

#### Avant (broken)
```c
char *ft_strchr(char *s, int c)
{
    int i = 0;
    while (s[i])
    {
        i++;
        if (s[i] == c)
            return s + i;
    }
    return NULL;
}
```

- Incrémente avant de comparer ➜ saute le premier caractère et lit s[i] hors limites si `c` absent.

#### Après (fix)
```c
char *ft_strchr(char *s, int c)
{
    int i = 0;
    while (s[i] && s[i] != c)
        i++;
    if (s[i] == c)
        return s + i;
    return NULL;
}
```

- Arrêt dès qu'on touche `\0` ou le caractère recherché.

---

### ft_memcpy

#### Avant (broken)
```c
void *ft_memcpy(void *dest, const void *src, size_t n)
{
    while (--n > 0)
        ((char*)dest)[n - 1] = ((char*)src)[n - 1];
    return dest;
}
```

- Ne copie jamais l'octet d'indice 0, rien n'est copié lorsque `n == 1`.

#### Après (fix)
```c
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
```

- Copie séquentielle de 0 à `n-1`, conforme au prototype standard.

---

### ft_strlen

#### Avant (broken)
```c
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
```

- Aucune protection si `s == NULL` ➜ crash dès le premier appel avec `*s1 == NULL`.

#### Après (fix)
```c
size_t ft_strlen(char *s)
{
    size_t ret = 0;
    while (s && *s)
    {
        s++;
        ret++;
    }
    return ret;
}
```

- Traite `NULL` comme chaîne vide, évite tout déréférencement illégal.

---

tmp[size1 + size2] = '\0';
### str_append_mem

#### Avant (broken)
```c
int str_append_mem(char **s1, char *s2, size_t size2)
{
    size_t size1 = ft_strlen(*s1);
    char *tmp = malloc(size2 + size1 + 1);
    if (!tmp)
        return 0;
    ft_memcpy(tmp, *s1, size1);
    ft_memcpy(tmp + size1, s2, size2);
    tmp[size1 + size2] = 0;
    free(*s1);
    *s1 = tmp;
    return 1;
}
```

- `ft_strlen(*s1)` et `ft_memcpy(tmp, *s1, ...)` déclenchent un crash si `*s1 == NULL`.
- Ne remet pas explicitement `\0` (utilise `0`, ok mais peu lisible).

#### Après (fix)
```c
int str_append_mem(char **s1, char *s2, size_t size2)
{
    size_t size1 = (*s1) ? ft_strlen(*s1) : 0;
    char *tmp = (char*)malloc(size1 + size2 + 1);
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
```

- Garde sur `*s1` avant tout accès.
- Ajoute systématiquement le terminateur `\0` et conserve la valeur de retour.

---

### ft_memmove

#### Avant (broken)
```c
void *ft_memmove(void *dest, const void *src, size_t n)
{
    if (dest > src)
        return ft_memcpy(dest, src, n);
    else if (dest == src)
        return dest;
    size_t i = ft_strlen((char*)src) - 1;
    while (i >= 0)
    {
        ((char*)dest)[i] = ((char*)src)[i];
        i--;
    }
    return dest;
}
```

- `ft_strlen((char*)src)` sous-dépasse si la zone ne se termine pas par `\0`.
- `size_t i` et boucle `while (i >= 0)` ➜ wrap infini.
- Ne copie pas correctement quand dest > src (utilise `ft_memcpy` buggué).

#### Après (fix)
```c
void *ft_memmove(void *dest, const void *src, size_t n)
{
    if (dest == src || n == 0)
        return dest;
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;
    if (d < s || d >= s + n)
        return ft_memcpy(dest, src, n);
    while (n-- > 0)
        d[n] = s[n];
    return dest;
}
```

- Respecte l'algorithme POSIX : copie backward uniquement si zones se chevauchent.

#### Variante pédagogique (même logique, plus explicite sans appel à ft_memcpy)

```c
void *ft_memmove(void *dest, const void *src, size_t n)
{
    size_t          i;
    unsigned char       *d;
    const unsigned char *s;

    if (dest == src || n == 0)
        return dest;
    d = (unsigned char*)dest;
    s = (const unsigned char*)src;
    if (d < s)
    {
        i = 0;
        while (i < n)
        {
            d[i] = s[i];
            i++;
        }
    }
    else
    {
        while (n > 0)
        {
            n--;
            d[n] = s[n];
        }
    }
    return dest;
}
```

Points clés rappel:
- Copie forward si dest avant src (pas de risque d’écrasement).
- Copie backward si overlap (dest dans la zone source).
- Pas d’appel externe: lisible à l’oral si tu dois “justifier ligne par ligne”.

---

### get_next_line

#### Avant (broken)
```c
char *get_next_line(int fd)
{
    static char b[BUFFER_SIZE + 1] = "";
    char *ret = NULL;

    char *tmp = ft_strchr(b, '\n');
    while (!tmp)
    {
        if (!str_append_str(&ret, b))
            return NULL;
        int read_ret = read(fd, b, BUFFER_SIZE);
        if (read_ret == -1)
            return NULL;
        b[read_ret] = 0;
    }
    if (!str_append_mem(&ret, b, tmp - b + 1))
    {
        free(ret);
        return NULL;
    }
    return ret;
}
```

- Ne lit jamais si le buffer statique est vide au début ➜ retourne `NULL` directement.
- Confond `read == 0` (EOF) avec cas normal ➜ boucle infinie, perte de la dernière ligne.
- N'effectue pas de `memmove` du reste après `\n` ➜ les appels suivants renvoient la même ligne.

#### Après (fix)
```c
char *get_next_line(int fd)
{
	static char buf[BUFFER_SIZE + 1];
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
				return (free(line), NULL);
			ft_memmove(buf, nl + 1, ft_strlen(nl + 1) + 1);
			return line;
		}
		if (!str_append_mem(&line, buf, ft_strlen(buf)))
			return (free(line), NULL);
		rd = read(fd, buf, BUFFER_SIZE);
		if (rd <= 0)
		{
			buf[0] = '\0';
			if (line && *line)
				return line;
			return (free(line), NULL);
		}
		buf[rd] = '\0';
	}
}
```

## 3. Points critiques validés
- `ft_strchr` ne dépasse pas / gère NULL
- `ft_memcpy` copie 0..n-1
- `ft_memmove` overlap-safe
- `str_append_mem` gère `*s1 == NULL`
- Aucune fuite: chaque malloc stocké -> transféré -> free précédent
- Dernière ligne sans `\n` retournée
- Retour NULL propre après EOF si aucune donnée
- Aucun `open/close/printf` dans la version à rendre

## 4. Mains de test (HORS RENDU)

Main détaillé:
```c
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 16
#endif
char *get_next_line(int fd);

int main(int ac, char **av)
{
	int fd = (ac == 2) ? open(av[1], O_RDONLY) : 0;
	char *s;
	if (fd < 0) return 1;
	while ((s = get_next_line(fd)))
	{
		printf("%s", s);
		free(s);
	}
	if (fd > 2) close(fd);
	return 0;
}
```

Main ultra minimal:
```c
int main(void)
{
	char *s;
	while ((s = get_next_line(0)))
	{
		write(1, s, ft_strlen(s));
		free(s);
	}
	return 0;
}
```

## 5. Routine test rapide
- `cc -Wall -Wextra -Werror -D BUFFER_SIZE=1 get_next_line.c test_main.c`
- Cas: fichier vide -> `NULL` direct
- Fichier terminant sans `\n`
- Ligne > BUFFER_SIZE (plusieurs lectures)
- Plusieurs appels consécutifs après EOF -> toujours `NULL`

## 6. Checklist finale (à cocher avant rendu)
- [ ] Aucun `printf`, `open`, `close` dans `get_next_line.c`
- [ ] Un seul fichier `.c` + header propre
- [ ] Guard fd / BUFFER_SIZE
- [ ] Pas d’allocation résiduelle après EOF
- [ ] Norme: pas de variables inutilisées / pas de leaks (testé)
- [ ] Retour inclut `\n` quand présent
- [ ] Compilation avec plusieurs `-D BUFFER_SIZE=` OK

Concis = crédible. Tu rends uniquement `get_next_line.c` + `get_next_line.h`.


