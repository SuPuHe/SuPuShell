/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:27:16 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/28 16:27:36 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strsep(char **str, const char *sep)
{
	char		*start;
	char		*ptr;
	const char	*s;

	if (!str || !*str)
		return (NULL);
	start = *str;
	ptr = start;
	while (*ptr)
	{
		s = sep;
		while (*s)
		{
			if (*ptr == *s)
			{
				*ptr = '\0';
				*str = ptr + 1;
				return (start);
			}
			s++;
		}
		ptr++;
	}
	*str = NULL;
	return (start);
}

// helper functions for searching by PATH
static char	*find_in_paths(const char *cmd, char *paths)
{
	char	full_path[1024];
	char	*token;
	char	*result;

	token = ft_strsep(&paths, ":");
	while (token != NULL)
	{
		ft_strcpy(full_path, token);
		ft_strcat(full_path, "/");
		ft_strcat(full_path, cmd);
		if (access(full_path, X_OK) == 0)
		{
			result = ft_strdup(full_path);
			return (result);
		}
		token = ft_strsep(&paths, ":");
	}
	return (NULL);
}

char	*search_path(const char *cmd, t_env *env)
{
	char	*path_var;
	char	*paths;
	char	*save;
	char	*result;

	path_var = get_env_value(env, "PATH");
	if (!path_var)
		return (NULL);
	paths = ft_strdup(path_var);
	save = paths;
	result = find_in_paths(cmd, paths);
	free(save);
	return (result);
}
