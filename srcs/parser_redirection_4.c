/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_4.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:17:57 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 13:48:03 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Recursively matches a text string against a wildcard pattern.
 *
 * Supports the '*' wildcard, which can match zero or more characters.
 * Other characters must match exactly. The function returns true if
 * the entire pattern matches the entire text.
 *
 * @param pattern The wildcard pattern to match (supports '*').
 * @param text The text string to test against the pattern.
 * @return true if the text matches the pattern, false otherwise.
 */
static bool	match_wildcard_pattern(char *pattern, char *text)
{
	if (*pattern == '\0' && *text == '\0')
		return (true);
	if (*pattern == '*')
	{
		if (*(pattern + 1) == '\0')
			return (true);
		while (*text)
		{
			if (match_wildcard_pattern(pattern + 1, text))
				return (true);
			text++;
		}
		return (false);
	}
	if (*pattern == *text)
		return (match_wildcard_pattern(pattern + 1, text + 1));
	return (false);
}

/**
 * @brief Handles a potential match in the directory.
 *
 * Checks for ambiguous matches and returns an
 * allocated string for a single match.
 * @param matching_file Current matching file (may be NULL).
 * @param match_count Number of matches found so far.
 * @param entry_name Name of the directory entry being checked.
 * @return Allocated string of the match or NULL if ambiguous.
 */
static char	*handle_dir_entry(char *matching_file,
	int *match_count, char *entry_name)
{
	if ((*match_count) > 0)
	{
		ft_putstr_fd("Billyshell: ambiguous redirect\n", 2);
		cf_free_one(matching_file);
		return (NULL);
	}
	return (cf_strdup(entry_name));
}

/**
 * @brief Searches the current directory for a file matching a pattern.
 *
 * Opens the directory, iterates entries, and
 * returns a single matching filename.
 * Handles ambiguous matches and frees memory if needed.
 *
 * @param pattern Wildcard pattern to match.
 * @return Allocated string of matching filename or
 * 			NULL if ambiguous/no directory.
 */
static char	*find_matching_in_dir(char *pattern, int *match_count)
{
	DIR				*dir;
	struct dirent	*entry;
	char			*matching_file;
	char			*tmp;

	dir = opendir(".");
	if (!dir)
		return (NULL);
	matching_file = NULL;
	entry = readdir(dir);
	while (entry)
	{
		if (match_wildcard_pattern(pattern, entry->d_name))
		{
			tmp = handle_dir_entry(matching_file, match_count, entry->d_name);
			if (!tmp)
				return (closedir(dir), NULL);
			matching_file = tmp;
			(*match_count)++;
		}
		entry = readdir(dir);
	}
	closedir(dir);
	return (matching_file);
}

/**
 * @brief Returns a filename matching a wildcard pattern.
 *
 * Calls find_matching_in_dir to search for matches in the current directory.
 * Returns NULL if ambiguous matches are found, the exact match if one is found,
 * or the original pattern if no matches exist.
 *
 * @param pattern Wildcard pattern to match.
 * @return Pointer to allocated matching filename or original pattern duplicated.
 */
static char	*get_matching_file(char *pattern)
{
	int		match_count;
	char	*matching_file;

	match_count = 0;
	matching_file = find_matching_in_dir(pattern, &match_count);
	if (matching_file)
		return (matching_file);
	if (match_count == 0)
		return (cf_strdup(pattern));
	return (NULL);
}

/**
 * @brief Validates and expands a filename token for redirection.
 *
 * Ensures the current token exists and expands it using environment
 * variables. If validation or expansion fails, marks syntax as invalid.
 *
 * @param input Input structure holding parsing state.
 * @param current_tokens Pointer to the current token list iterator.
 * @param env Environment variable list for expansion.
 * @return Expanded filename string, or NULL on error.
 */
char	*get_expanded_filename(t_input *input,
	t_list **current_tokens, t_env *env)
{
	t_token	*filename_token;
	char	*expanded_value;
	char	*final_value;

	filename_token = (t_token *)(*current_tokens)->content;
	if (!filename_token || !filename_token->value)
	{
		input->syntax_ok = false;
		return (NULL);
	}
	expanded_value = expand_filename_token(filename_token, env, input->shell);
	if (!expanded_value)
	{
		input->syntax_ok = false;
		return (NULL);
	}
	final_value = get_matching_file(expanded_value);
	cf_free_one(expanded_value);
	if (!final_value)
	{
		input->syntax_ok = false;
		return (NULL);
	}
	return (final_value);
}

/**
 * @brief Collects the delimiter for a heredoc (<<) operator.
 *
 * Starting from the current token list node, this function gathers all tokens
 * that form the heredoc delimiter into a string builder. It also determines
 * whether the delimiter was quoted, which affects whether variable expansion
 * should occur in the heredoc body.
 *
 * @param current_tokens Pointer to the token list iterator; will be advanced
 *						past the delimiter tokens.
 * @param quoted Output flag set to true if the delimiter is quoted,
 * 					false otherwise.
 * @return A newly allocated string containing the heredoc delimiter,
 *			or NULL if allocation failed or tokens are missing.
 */
char	*collect_heredoc_delimiter(t_list **current_tokens, bool *quoted)
{
	t_list				*start;
	t_string_builder	*sb;

	if (!current_tokens || !*current_tokens || !(*current_tokens)->content)
		return (NULL);
	start = *current_tokens;
	sb = sb_create();
	if (!sb)
		return (NULL);
	*current_tokens = collect_d_tokens_and_sb(start, sb, quoted);
	return (sb_build_and_destroy(sb));
}
