#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>

int	main(int ac, char **av)
{
	int i, j, k;
	char *buf;
	char *tmp;
	int rd;
// ----------------------------------------- VERIF
	if (ac != 2)
		return 1;
	buf = malloc(1);
	if(!buf)
	{
		perror("Error: ");
		return 1;
	}
	i = 0;
	// ----------------------------------------- BOUCLE 1
	while ((rd = read(0, &buf[i], 1)) > 0)
	{
		tmp = realloc(buf, i + 2);
		if(!tmp)
		{
			perror("Error: ");
			free(buf);
			return 1;
		}
		buf = tmp;
		i++;
	}
	buf[i] = '\0';
	i = 0;
		// ----------------------------------------- BOUCLE 2
	while(buf[i])
	{
		j = 0;
		while (av[1][j] && buf[i + j] == av[1][j])
			j++;
		if(av[1][j] == '\0')
		{
			k = 0;
			while (k < j)
			{
				buf[i + k] = '*';
				k++;
			}
			i += j;
		}
		else
			i++;
	}
			// ----------------------------------------- Print and Free
	free(buf);
	return 0;
}