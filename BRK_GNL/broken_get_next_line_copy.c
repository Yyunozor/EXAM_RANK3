// VERSION BROKEN ANNOTEE : chaque bloc problématique est commenté
// Objectif: expliquer pourquoi ce code est incorrect / dangereux sans le modifier.
// Ne PAS utiliser cette version en production.
#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	int i = 0;
	while (s[i] != c)                 // BUG: aucune condition d'arrêt si le caractère n'existe pas -> lecture au-delà du '\0' (UB)
		i++;                            // Si 'c' n'est pas trouvé, on passe la fin de chaîne et on lit de la mémoire indéfinie.
	if (s[i] == c)
		return s + i;                  // Ce return suppose que s[i] est accessible; faux si on a dépassé la fin.
	else
		return NULL;                   // Jamais atteint pour le cas "non trouvé" si on déclenche un crash avant.
}

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	while (--n > 0)                    // BUG: ignore l'octet d'indice 0 : pour n == 1 la boucle ne s'exécute pas -> rien copié.
		((char*)dest)[n - 1] = ((char*)src)[n - 1]; // Copie en ordre inverse sans raison; pas faux en soi mais combiné à la condition ci-dessus => copie incomplète.
	return dest;                        // Comportement inattendu si l'appelant pense avoir une copie complète.
}

size_t ft_strlen(char *s)
{
	size_t ret = 0;
	while (*s)                          // BUG POTENTIEL: aucune vérification si s == NULL (arrive si première concat commence avec *s1 == NULL)
	{
		s++;
		ret++;
	}
	return (ret);
}

int str_append_mem(char **s1, char *s2, size_t size2)
{
	size_t size1 = ft_strlen(*s1);      // BUG: *s1 peut être NULL au premier appel -> deref NULL
	char *tmp = malloc(size2 + size1 + 1);
	if (!tmp)
		return 0;                       // OK: gestion allocation
	ft_memcpy(tmp, *s1, size1);         // BUG: même problème si *s1 == NULL (comportement indéfini)
	ft_memcpy(tmp + size1, s2, size2);  // Suppose que ft_memcpy copie correctement (ce qui est faux ici, perd 1 octet)
	tmp[size1 + size2] = 0;             // Null-terminate OK
	free(*s1);                          // Libère ancienne chaîne (OK si non NULL)
	*s1 = tmp;
	return 1;
}

int str_append_str(char **s1, char *s2)
{
	return str_append_mem(s1, s2, ft_strlen(s2)); // Hérite des bugs: ft_strlen(s2) ok si s2 non NULL, mais str_append_mem crash si *s1 == NULL
}

void *ft_memmove(void *dest, const void *src, size_t n)
{
	if (dest > src)
		return ft_memcpy(dest, src, n);   // BUG: si overlap et dest > src -> ok conceptuellement MAIS ft_memcpy est incorrect (perd 1 octet)
	else if (dest == src)
		return dest;                     // OK: rien à faire
	size_t i = ft_strlen((char*)src) - 1; // BUG CRITIQUE: utilise strlen pour déterminer n bytes à copier -> faux si zone mémoire binaire / si n < strlen / si src pas terminée
	while (i >= 0)                      // BUG: i est size_t (non signé) -> la condition i >= 0 est toujours vraie -> boucle infinie après underflow
	{
		((char*)dest)[i] = ((char*)src)[i];
		i--;                             // Quand i == 0 puis i-- -> i devient très grand (wrap) -> écriture hors limites massive
	}
	return dest;                        // Implémentation totalement UB pour beaucoup de cas
}

char *get_next_line(int fd)
{
	static char b[BUFFER_SIZE + 1] = ""; // Buffer statique jamais "décalé" après extraction d'une ligne -> leftovers non gérés
	char *ret = NULL;                     // Commence NULL mais sera utilisé sans garde dans str_append_str -> crash potentiel

	char *tmp = ft_strchr(b, '\n');      // Recherche d'un '\n' dans un buffer potentiellement vide (b == "") : ft_strchr va déréférencer b[0] ok, mais si jamais b vide et pas de lecture suivante -> logique bancale
	while (!tmp)
	{
		if (!str_append_str(&ret, b))      // Ajoute le contenu actuel même s'il est vide; si première itération ret==NULL -> crash dans str_append_mem
			return NULL;                    // En cas d'echec malloc -> leak du buffer statique ok mais ret NULL
		int read_ret = read(fd, b, BUFFER_SIZE); // Lecture tardive: on lit seulement APRÈS avoir tenté d'utiliser le buffer initial
		if (read_ret == -1)
			return NULL;                  // Si EOF (read_ret == 0) -> on ne sort pas explicitement pour renvoyer la dernière ligne partielle
		b[read_ret] = 0;                  // Terminaison OK, mais si read_ret == 0 (EOF) -> b[0] = 0 et on repart dans la boucle infinie (tmp toujours NULL)
		// ABSENCE: gestion cas read_ret == 0 (EOF): devrait renvoyer ret si non vide ou NULL sinon.
	}
	if (!str_append_mem(&ret, b, tmp - b + 1)) // Concat la portion jusqu'au '\n' inclus
	{
		free(ret);
		return NULL;
	}
	// ABSENCE: décaler (memmove) ce qu'il y a après le '\n' dans b pour l'appel suivant -> la prochaine ligne recommencera au même endroit -> duplications/boucle.
	return ret;                           // Retourne la ligne mais laisse les restes intacts -> appels suivants cassés.
}
