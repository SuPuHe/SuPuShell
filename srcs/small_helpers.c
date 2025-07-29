/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   small_helpers.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 12:44:02 by omizin            #+#    #+#             */
/*   Updated: 2025/07/29 14:05:04 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isspace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

static void	check_for_par_and(char line, t_input_check *check)
{
	if (line == '(')
		check->l_par++;
	else if (line == ')')
		check->r_par++;
}

int	check_for_input(char *line)
{
	t_input_check	check;
	int				i;

	i = 0;
	check.d_quot = 0;
	check.s_quot = 0;
	check.l_par = 0;
	check.r_par = 0;
	while (line[i])
	{
		if (line[i] == '\\' || line[i] == ';')
			return (printf("Error input\n"), 0);
		else if (line[i] == '"')
			check.d_quot++;
		else if (line[i] == '\'')
			check.s_quot++;
		else
			check_for_par_and(line[i], &check);
		i++;
	}
	if ((check.d_quot % 2 != 0 || check.s_quot % 2 != 0)
		|| (check.r_par != check.l_par))
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
	new_args = cf_malloc(sizeof(char *) * (len + 2));
	i = 0;
	if (!new_args)
		return (NULL);
	while (i < len)
	{
		new_args[i] = args[i];
		i++;
	}
	new_args[len] = cf_strdup(new_arg);
	new_args[len + 1] = NULL;
	return (new_args);
}
