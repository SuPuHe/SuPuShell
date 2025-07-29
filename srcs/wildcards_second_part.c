/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_second_part.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 16:43:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/29 11:46:00 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	matches_pattern(const char *str, const char *pattern)
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

int	count_matches(const char *pattern)
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

char	**create_result_array(int size)
{
	char	**result;

	result = cf_malloc(sizeof(char *) * (size + 1));
	if (!result)
		return (NULL);
	return (result);
}

char	**handle_no_wildcards(const char *pattern)
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
