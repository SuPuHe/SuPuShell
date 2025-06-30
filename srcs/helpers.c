/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 19:38:55 by omizin            #+#    #+#             */
/*   Updated: 2025/06/30 19:43:07 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

char	*substr_dup(const char *start, const char *end)
{
	size_t	len;
	char	*s;

	len = end - start;
	s = malloc(len + 1);
	if (!s)
		return NULL;
	ft_memcpy(s, start, len);
	s[len] = '\0';
	return (s);
}

char	**split_args(const char *input)
{
	char		**args;
	int			i;
	const char	*p;
	const char	*start;

	args = malloc(64 * sizeof(char *));
	i = 0;
	p = input;
	while (*p)
	{
		while (*p && (*p == ' ' || *p == '\t' || *p == '\n'))
			p++;
		if (*p == '\0')
			break;
		start = p;
		while (*p && *p != ' ' && *p != '\t' && *p != '\n')
			p++;
		args[i++] = substr_dup(start, p);
		if (i >= 63)
			break;
	}
	args[i] = NULL;
	return (args);
}

void	free_args(char **args)
{
	int	i;

	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

void	free_env_list(t_env *env)
{
	t_env *tmp;

	while (env)
	{
		tmp = env->next;
		free(env->key);
		free(env->value);
		free(env);
		env = tmp;
	}
}
