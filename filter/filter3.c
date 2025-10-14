#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

int	main(int ac, char **av)
{
	char *line;
	char *buf;
	int i, j, k;
	int fd;

	if(ac != 2)
		return 1;

	buf = malloc(1);
	if(!buf)
	{
		perror("Error: ");
		return 1;
	}
	i = 0;
	while((fd = read(0, &buf[i], 1)) > 0)
	{
		line = realloc(buf, i + 2);
		if(!line)
		{
			perror("Error: ");
			free(buf);
			return 1;
		}
		buf = line;
		i++;
	}
	buf[i] = '\0';
	i = 0;
	while (buf[i])
	{
		j = 0;
		while(av[1][j] && buf[i + j] == av[1][j])
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