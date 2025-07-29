/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:00:00 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/29 11:44:50 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
