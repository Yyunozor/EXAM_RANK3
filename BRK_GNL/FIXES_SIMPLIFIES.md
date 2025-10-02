## 🎯 VERSION SIMPLIFIÉE - SANS CONCEPTS COMPLEXES

### LES 6 FIXES ULTRA-SIMPLES (pour l'examen)

#### ✅ FIX 1: ft_strchr (ligne 6)
```c
// AVANT (cassé):
while (s[i] != c)

// APRÈS (simple):
while (s[i] != '\0' && s[i] != c)
```

#### ✅ FIX 2: ft_memcpy (lignes 15-17)
```c
// AVANT (cassé):
while (--n > 0)
    ((char*)dest)[n - 1] = ((char*)src)[n - 1];

// APRÈS (simple):
size_t i = 0;
while (i < n)
{
    ((char*)dest)[i] = ((char*)src)[i];
    i++;
}
```

#### ✅ FIX 3: str_append_mem (ligne 30)
```c
// AVANT (cassé):
size_t size1 = ft_strlen(*s1);

// APRÈS (simple):
size_t size1;
if (*s1 == NULL)
    size1 = 0;
else
    size1 = ft_strlen(*s1);
```

#### ✅ FIX 4: str_append_mem (ligne 35)
```c
// AVANT (cassé):
ft_memcpy(tmp, *s1, size1);

// APRÈS (simple):
if (*s1 != NULL)
    ft_memcpy(tmp, *s1, size1);
```

#### ✅ FIX 5: get_next_line (AJOUTER après "char *ret = NULL;")
```c
// AJOUTER:
if (b[0] == '\0')
{
    int read_ret = read(fd, b, BUFFER_SIZE);
    if (read_ret <= 0)
        return NULL;
    b[read_ret] = '\0';
}
```

#### ✅ FIX 6: get_next_line (AJOUTER avant "return ret;")
```c
// AJOUTER:
size_t remaining = ft_strlen(tmp + 1);
size_t i = 0;
while (i <= remaining)
{
    b[i] = tmp[1 + i];
    i++;
}
```

### 🚨 CONCEPTS ÉVITÉS (trop complexes pour examen):
- ❌ Opérateur ternaire `? :`
- ❌ Pointeurs arithmétiques avancés  
- ❌ Expressions complexes dans les conditions
- ❌ Optimisations de code

### ✅ STYLE ADOPTÉ (simple et lisible):
- ✅ if/else explicites
- ✅ Boucles while classiques
- ✅ Une instruction par ligne
- ✅ Variables intermédiaires claires

### 🎯 POURQUOI C'EST MIEUX POUR L'EXAMEN:
1. **Plus rapide à taper**
2. **Moins d'erreurs de syntaxe**
3. **Plus facile à débugger**
4. **Moins de stress mental**