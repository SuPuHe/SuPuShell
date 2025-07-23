/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:00:00 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/23 17:42:46 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <dirent.h>

static int	matches_pattern(const char *str, const char *pattern)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '*')
	{
		while (*pattern == '*')
			pattern++;
		if (*pattern == '\0')
			return (1);
		while (*str)
		{
			if (matches_pattern(str, pattern))
				return (1);
			str++;
		}
		return (matches_pattern(str, pattern));
	}
	if (*str == '\0')
		return (0);
	if (*pattern == *str || *pattern == '?')
		return (matches_pattern(str + 1, pattern + 1));
	return (0);
}

static int	count_matches(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	count = 0;
	dir = opendir(".");
	if (!dir)
		return (0);
	entry = readdir(dir);
	while (entry != NULL)
	{
		if (entry->d_name[0] != '.' && matches_pattern(entry->d_name, pattern))
			count++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (count);
}

static char	**create_result_array(int size)
{
	char	**result;

	result = cf_malloc(sizeof(char *) * (size + 1));
	if (!result)
		return (NULL);
	return (result);
}

static char	**handle_no_wildcards(const char *pattern)
{
	char	**result;

	result = create_result_array(1);
	if (!result)
		return (NULL);
	result[0] = cf_strdup(pattern);
	if (!result[0])
	{
		cf_free_one(result);
		return (NULL);
	}
	result[1] = NULL;
	return (result);
}

static void	cleanup_on_error(char **result, int i, DIR *dir)
{
	while (--i >= 0)
		cf_free_one(result[i]);
	cf_free_one(result);
	if (dir)
		closedir(dir);
}

static int	init_wildcards(const char *pattern, char ***result, DIR **dir)
{
	int	count;

	if (!ft_strchr(pattern, '*'))
	{
		*result = handle_no_wildcards(pattern);
		return (-1);
	}
	count = count_matches(pattern);
	if (count == 0)
	{
		*result = handle_no_wildcards(pattern);
		return (-1);
	}
	*result = create_result_array(count);
	if (!*result)
		return (-1);
	*dir = opendir(".");
	if (!*dir)
	{
		cf_free_one(*result);
		return (-1);
	}
	return (0);
}

static int	process_entry(char **result, int i, struct dirent *entry,
		const char *pattern)
{
	if (entry->d_name[0] != '.' && matches_pattern(entry->d_name, pattern))
	{
		result[i] = cf_strdup(entry->d_name);
		if (!result[i])
			return (-1);
		return (1);
	}
	return (0);
}

char	**expand_wildcards(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**result;
	int				i;
	int				ret;

	if (init_wildcards(pattern, &result, &dir) == -1)
		return (result);
	i = 0;
	entry = readdir(dir);
	while (entry != NULL)
	{
		ret = process_entry(result, i, entry, pattern);
		if (ret == -1)
		{
			cleanup_on_error(result, i, dir);
			return (NULL);
		}
		i += ret;
		entry = readdir(dir);
	}
	result[i] = NULL;
	closedir(dir);
	return (result);
}

void	free_expanded_wildcards(char **expanded)
{
	int	i;

	if (!expanded)
		return ;
	i = 0;
	while (expanded[i])
	{
		cf_free_one(expanded[i]);
		i++;
	}
	cf_free_one(expanded);
}
