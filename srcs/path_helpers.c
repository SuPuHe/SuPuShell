/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_helpers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:27:16 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:31:15 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Splits a string by separator characters.
 *
 * Finds the next token in *str separated by any character in sep,
 * replaces the separator with null, and updates *str.
 *
 * @param str Pointer to string to split.
 * @param sep Separator characters.
 * @return Pointer to token, or NULL if no more tokens.
 */
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

/**
 * @brief Searches for a command in a colon-separated PATH string.
 *
 * Iterates through each path, builds the full path, and checks
 * if the command is executable.
 *
 * @param cmd Command name to search for.
 * @param paths Colon-separated PATH string.
 * @return Pointer to found path, or NULL if not found.
 */
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

/**
 * @brief Searches for a command in the PATH environment variable.
 *
 * Retrieves PATH from env, splits it, and searches for the command
 * using find_in_paths().
 *
 * @param cmd Command name to search for.
 * @param env Pointer to environment variable list.
 * @return Pointer to found path, or NULL if not found.
 */
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
