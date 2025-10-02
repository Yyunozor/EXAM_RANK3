#include "get_next_line.h"

char *ft_strchr(char *s, int c)
{
	while (*s && *s != c)
		s++;
	return (*s == c) ? s : NULL;
}

size_t ft_strlen(char *s)
{
	size_t len = 0;
	while (s[len])
		len++;
	return len;
}

char *ft_strjoin(char *s1, char *s2, size_t len2)
{
	size_t len1 = s1 ? ft_strlen(s1) : 0;
	char *result = malloc(len1 + len2 + 1);
	if (!result)
		return NULL;
	
	size_t i = 0;
	while (i < len1)
	{
		result[i] = s1[i];
		i++;
	}
	while (i < len1 + len2)
	{
		result[i] = s2[i - len1];
		i++;
	}
	result[i] = '\0';
	free(s1);
	return result;
}

char *get_next_line(int fd)
{
	static char buffer[BUFFER_SIZE + 1] = "";  // Initialiser explicitement à vide
	char *line = NULL;
	char *newline;
	int bytes_read;

	while (1)
	{
		newline = ft_strchr(buffer, '\n');
		if (newline)
		{
			line = ft_strjoin(line, buffer, newline - buffer + 1);
			// Shift remaining content to start of buffer
			size_t remaining = ft_strlen(newline + 1);
			size_t i = 0;
			while (i <= remaining)
			{
				buffer[i] = newline[1 + i];
				i++;
			}
			return line;
		}
		
		line = ft_strjoin(line, buffer, ft_strlen(buffer));
		buffer[0] = '\0'; // Clear buffer after consuming it - FIX CRITIQUE
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read <= 0)
		{
			if (line && *line)
				return line;
			free(line);
			return NULL;
		}
		buffer[bytes_read] = '\0';
	}
}

int main(void)
{
	int fd;
	char *line;

	fd = open("test_long_lines.txt", O_RDONLY);

	while ((line = get_next_line(fd)))
		printf("%s", line);
	
	close(fd);
	return 0;
}