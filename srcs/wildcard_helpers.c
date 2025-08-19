/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/19 17:03:02 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 17:05:38 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Returns a dynamically allocated array with the original pattern.
 *
 * Used when no wildcard expansion is needed or no matches found.
 *
 * @param pattern Original string.
 * @return Null-terminated array with pattern copy.
 */
char	**return_single_pattern(const char *pattern)
{
	char	**result;

	result = cf_malloc(sizeof(char *) * 2);
	if (!result)
		return (NULL);
	result[0] = cf_strdup(pattern);
	result[1] = NULL;
	return (result);
}

/**
 * @brief Counts files in the current directory matching the pattern.
 *
 * @param pattern Pattern with wildcards.
 * @return Number of matching files.
 */
int	count_matching_files(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	int				count;

	dir = opendir(".");
	count = 0;
	if (!dir)
		return (0);
	entry = readdir(dir);
	while (entry != NULL)
	{
		if (entry->d_name[0] != '.'
			&& matches_pattern_with_escape(entry->d_name, pattern))
			count++;
		entry = readdir(dir);
	}
	closedir(dir);
	return (count);
}

/**
 * @brief Safely appends a matched filename to the result array.
 *
 * Allocates memory for the filename, updates the index, and frees all
 * previously allocated entries if allocation fails.
 *
 * @param result Array of strings.
 * @param i Pointer to current index in the array.
 * @param name Matched filename.
 * @return true if successful, false on allocation failure.
 */
bool	append_match_to_result(char **result, int *i, const char *name)
{
	result[*i] = cf_strdup(name);
	if (!result[*i])
	{
		while (--(*i) >= 0)
			cf_free_one(result[*i]);
		cf_free_one(result);
		return (false);
	}
	(*i)++;
	return (true);
}

/**
 * @brief Collects files matching a wildcard pattern in the current directory.
 *
 * Opens the directory, checks each file against the pattern, and stores
 * matches in a null-terminated array.
 *
 * @param pattern Pattern with wildcards.
 * @param count Number of matching files expected.
 * @return Null-terminated array of matching filenames, or NULL on failure.
 */
char	**collect_matching_files(const char *pattern, int count)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**result;
	int				i;

	dir = opendir(".");
	i = 0;
	if (!dir)
		return (NULL);
	result = cf_malloc(sizeof(char *) * (count + 1));
	if (!result)
		return (closedir(dir), NULL);
	entry = readdir(dir);
	while (entry != NULL && i < count)
	{
		if (entry->d_name[0] != '.'
			&& matches_pattern_with_escape(entry->d_name, pattern))
		{
			if (!append_match_to_result(result, &i, entry->d_name))
				return (closedir(dir), NULL);
		}
		entry = readdir(dir);
	}
	result[i] = NULL;
	return (closedir(dir), result);
}
