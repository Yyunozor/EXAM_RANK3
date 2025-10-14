# broken_GNL — Tuto express

Objectif
- Réparer get_next_line pour retourner une ligne par appel (inclure '\n' s’il existe), sinon la dernière portion à l’EOF.
- Aucune fuite; seul le pointeur retourné reste alloué.
- Uniquement read, malloc, free. Support -D BUFFER_SIZE.

API et contraintes
- Prototype: char *get_next_line(int fd);
- Retour: NULL si erreur/EOF sans donnée; sinon chaîne allouée (incluant '\n' si présent).
- Pas d’allocation persistante entre appels.

Algorithme (modèle ULTIME)
- Buffer statique: static char buf[BUFFER_SIZE + 1];
- Accumulateur: char *line = NULL;
- Boucle:
  1) Si buf est vide: read(fd, buf, BUFFER_SIZE); si rd <= 0:
     - si line a du contenu -> retourner line; sinon -> NULL.
     - buf[rd] = '\0'.
  2) Chercher '\n' avec ft_strchr(buf, '\n').
     - Si trouvé: append [buf..nl] (inclus) à line; memmove(buf, nl+1, strlen(nl+1)+1); return line.
     - Sinon: append tout buf à line; buf[0] = '\0'; continuer.

Helpers (contrats)
- ft_strlen(s): s non NULL; retourne taille.
- ft_strchr(s,c): s peut être NULL; renvoie ptr sur c ou NULL.
- ft_memcpy(d,s,n): copie 0..n-1 (pas de chevauchement).
- ft_memmove(d,s,n): copie n octets en gérant le chevauchement.
- str_append_mem(&s1, s2, n): alloue size1+size2+1, copie s1 puis s2 (n), ajoute '\0', free(s1), met à jour s1.

Bugs typiques (broken_get_next_line.c)
- ft_strchr: boucle sans tester '\0' ni s == NULL -> overflow.
- ft_memcpy: while (--n > 0) ignore le 1er octet; n=0 UB.
- ft_strlen: OK, mais appel avec *s1 NULL provoque segfault ailleurs.
- str_append_mem: suppose *s1 non NULL; oublie size1=0 au départ.
- ft_memmove: mauvaise logique (strlen, underflow size_t, chevauchement mal géré).
- get_next_line:
  - Cherche '\n' avant de garantir que b est valide.
  - Gestion EOF/erreur incomplète (read -1/0).
  - Oublie de déplacer le “reste” après '\n'.
  - Fuites potentielles si échec après accumulation.

Exercices
1) Corriger ft_strchr pour gérer s NULL et l’arrêt sur '\0'.
2) Corriger ft_memcpy (boucle 0..n-1).
3) Corriger str_append_mem pour *s1 == NULL (size1=0).
4) Réimplémenter ft_memmove (copie avant/arrière selon d<s).
5) Réécrire la boucle de get_next_line selon l’algorithme ULTIME.

Tests
- Compile: cc -Wall -Wextra -Werror -D BUFFER_SIZE=1/42 get_next_line.c
- Cas:
  - Fichier vide.
  - Lignes > BUFFER_SIZE.
  - Dernière ligne sans '\n'.
  - Appels successifs jusqu’à NULL.
- Vérifier:
  - Chaque retour est freeable.
  - Pas de fuite (valgrind/leaks).
  - '\n' présent quand attendu.

Checklist
- [ ] EOF sans donnée -> NULL.
- [ ] EOF avec accumulation -> retourner la dernière portion.
- [ ] Inclure '\n' si présent.
- [ ] Aucune fuite; pas d’état dynamique entre appels.
- [ ] read/malloc/free uniquement.
- [ ] Insensible à BUFFER_SIZE (1, petit, grand).

Note
- Inspirez-vous de broken_get_next_lineULTIME.c pour une implémentation propre.
