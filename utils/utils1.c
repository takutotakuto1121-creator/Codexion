#include "codexion.h"

int	ft_strlen(const char *str)
{
	int	i;

	i = 0;
	while(str[i])
		i++;
	return (i)
}

void	print_error(const char *str)
{
	int	i;

	i = ft_strlen(str);
	write(2, "[ERROR]", 7);
	write(2, str, i);
}