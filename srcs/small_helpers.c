/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   small_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:44:02 by omizin            #+#    #+#             */
/*   Updated: 2025/07/09 12:50:14 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

void	skip_spaces(t_input *input)
{
	while (input->line[input->i]
		&& (input->line[input->i] == ' ' || input->line[input->i] == '\t'))
		input->i++;
}

int	check_for_input(char *line)
{
	int	i;
	int	quot;
	int	d_quot;

	i = 0;
	quot = 0;
	d_quot = 0;
	while (line[i])
	{
		if (line[i] == '\\' || line[i] == ';')
			return (printf("Error input\n"), 0);
		else if (line[i] == '"')
			d_quot++;
		else if (line[i] == '\'')
			quot++;
		i++;
	}
	if (d_quot % 2 != 0 || quot % 2 != 0)
		return (printf("Error input\n"), 0);
	return (1);
}

static int	ft_arrlen(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return (0);
	while (arr[i])
		i++;
	return (i);
}

char	**append_arg(char **args, char *new_arg)
{
	int		len;
	char	**new_args ;
	int		i;

	len = ft_arrlen(args);
	new_args = malloc(sizeof(char *) * (len + 2));
	i = 0;
	if (!new_args)
		return (NULL);
	while (i < len)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[len] = new_arg;
	new_args[len + 1] = NULL;
	free(args);
	return (new_args);
}
