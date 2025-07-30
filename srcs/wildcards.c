/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:00:00 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:32:46 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Cleans up memory and closes directory on error.
 *
 * Frees all strings in the result array and closes the directory if it is open.
 *
 * @param result Array of strings to free.
 * @param i Last index to free.
 * @param dir Directory pointer to close.
 */
static void	cleanup_on_error(char **result, int i, DIR *dir)
{
	while (--i >= 0)
		cf_free_one(result[i]);
	cf_free_one(result);
	if (dir)
		closedir(dir);
}

/**
 * @brief Prepares resources for wildcard expansion.
 *
 * Checks for '*' in pattern, counts matches, allocates result array,
 * and opens the current directory.
 * Handles cases with no matches or no wildcards.
 *
 * @param pattern Pattern string to expand.
 * @param result Pointer to result array.
 * @param dir Pointer to directory pointer.
 * @return 0 on success, -1 on error or no matches.
 */
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

/**
 * @brief Checks a directory entry for wildcard matching.
 *
 * If entry matches the pattern and is not hidden, copies its name to result.
 *
 * @param result Array to store matched names.
 * @param i Current index in result array.
 * @param entry Directory entry to check.
 * @param pattern Wildcard pattern to match.
 * @return 1 if entry added, 0 if not matched, -1 on allocation error.
 */
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

/**
 * @brief Expands wildcards in the pattern to matching filenames.
 *
 * Scans the directory for files matching the pattern. Returns an array of
 * matched filenames, or the pattern itself if no matches are found.
 *
 * @param pattern Pattern string with wildcards.
 * @return Array of matched filenames, or NULL on error.
 */
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

/**
 * @brief Frees memory for expanded wildcards array.
 *
 * Frees each string in the array, then frees the array itself.
 *
 * @param expanded Array of expanded wildcard strings to free.
 */
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
