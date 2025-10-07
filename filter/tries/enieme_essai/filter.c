#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char	*readthatshit(char *buff)
{
	int i = 0;
	char *tmp;
	int rd;

	while ((rd = read(0, &buff[i], 1)) > 0)
	{
		tmp = realloc(buff, i + 2);
		if(!tmp)
		{
			perror("Error: ");
			free(buff);
			return NULL;
		}
		buff = tmp;
		i++;
	}
	buff[i] = '\0';
	return buff;
}

int	main(int ac, char **av)
{
	char *buf;
	int i, j, k;
	
	if (ac != 2)
		return 1;
	if(av[1][0] = '\0')
	buf = malloc(1);
	if(!buf)
	{
		perror("Error: ");
		return 1;
	}
	buf = readthatshit(buf);
	i = 0;
	while(buf[i])
	{
		j = 0;
		while (av[1][j] && buf[i + j] == av[1][j])
			j++;
		if(av[1][j] == '\0')
		{
			k = 0;
			while(k < j)
			{
				buf[i + k] = '*';
				k++;
			}
			i += j;
		}
		else
			i++;
	}
	printf("%s", buf);
	free(buf);
	return 0;
}
