# get_next_line Broken ➜ Fixed Cheat Sheet

Ce mémo résume les corrections indispensables à apporter à `broken_get_next_line.c` en condition d'examen, sans aide externe.

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
    static char buf[BUFFER_SIZE + 1] = "";
    char *line = NULL;
    char *nl;
    int rd;

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
```

- Lecture initiale si le buffer est vide, gestion claire de `read <= 0`.
- Retour de la dernière ligne partielle et nettoyage du buffer statique.
- Décalage du leftover avec `ft_memmove` pour l'appel suivant.

---

## 3. Check-list finale

- [ ] Chaque helper corrigé (tests rapides sur brouillon).
- [ ] `str_append_mem` gère NULL + `\0` final.
- [ ] `get_next_line` fait : lecture initiale, boucle, gestion `rd <= 0`, leftover.
- [ ] Petit `main` de validation avec différents `BUFFER_SIZE`.
- [ ] Contrôle visuel : pas de `return` sans `free`, pas de variables inutilisées.

Avec cette fiche, tu peux corriger la version broken en ordre logique, sans rien oublier, et justifier chaque changement pendant l’examen.
