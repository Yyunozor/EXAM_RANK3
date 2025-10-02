#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*
TEST SUMMARY:
Input: "abc123abc\n", pattern="abc" => "***123***"
Input: pattern not present => output identique
Input large (>1024): realloc OK (doublage), terminaison buf[pos]='\0'
Edge: pattern length 1 => toutes occurrences remplies
Limitation: O(n*m) accepté pour examen
*/

void	replace_pattern(char *str, char *pattern, int len)
{
	int i = 0;
	while (str[i])
	{
		int j = 0;
		while (str[i + j] == pattern[j] && pattern[j] && j < len)
			j++;
		if (j == len)
		{
			for (int k = 0; k < len; k++)
				str[i + k] = '*';
			i += len;
		}
		else
			i++;
	}
}

int main(int ac, char **av)
{
	if (ac != 2 || !av[1][0])
		return (1);
	char *pattern = av[1];
	int pat_len = strlen(pattern);
	int buf_size = 1024;
	int pos = 0;
	char *buf = malloc(buf_size);
	if (!buf)
		return (1);
	int bytes_read;
	while ((bytes_read = read(0, buf + pos, buf_size - pos)) > 0)
	{
		pos += bytes_read;
		if (pos >= buf_size - 1)
		{
			buf_size *= 2;
			char *new = realloc(buf, buf_size);
			if (!new)
			{
				free(buf);
				return (1);
			}
			buf = new;
		}
	}
	if (bytes_read < 0)
	{
		free(buf);
		return (1);
	}
	buf[pos] = '\0';
	replace_pattern(buf, pattern, pat_len);
	printf("%s", buf);
	free(buf);
	return (0);
}
