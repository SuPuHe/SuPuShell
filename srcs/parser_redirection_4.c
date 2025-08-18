/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_4.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 16:17:57 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/18 18:05:10 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static char	*get_matching_file(char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	char			*matching_file;
	int				match_count;

	match_count = 0;
	matching_file = NULL;
	dir = opendir(".");
	if (!dir)
		return (NULL);
	entry = readdir(dir);
	while (entry)
	{
		if (match_wildcard_pattern(pattern, entry->d_name))
		{
			if (match_count > 0)
			{
				ft_putstr_fd("Billyshell: ambiguous redirect\n", 2);
				cf_free_one(matching_file);
				closedir(dir);
				return (NULL);
			}
			matching_file = cf_strdup(entry->d_name);
			match_count++;
		}
		entry = readdir(dir);
	}
	closedir(dir);
	if (match_count == 0)
		return (cf_strdup(pattern));
	return (matching_file);
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
