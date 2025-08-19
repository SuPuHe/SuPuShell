/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:15:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 14:57:53 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if a token is quoted.
 *
 * Determines whether a token was originally enclosed in single or double quotes.
 *
 * @param token Pointer to the token to check.
 * @return true if the token is single- or double-quoted, false otherwise.
 */
bool	is_token_quoted(t_token *token)
{
	if (!token)
		return (false);
	return (token->type == TOKEN_SINGLE_QUOTE_WORD
		|| token->type == TOKEN_DOUBLE_QUOTE_WORD);
}

/**
 * @brief Creates a single token part from a token.
 *
 * Allocates a new t_token_part, sets start/end positions and
 * quotation status.
 *
 * @param token Pointer to the token.
 * @param start_pos Start position of the token in the combined string.
 * @return Pointer to the newly allocated token part,
 * 			or NULL on allocation failure.
 */
static t_token_part	*create_token_part(t_token *token, int start_pos)
{
	t_token_part	*part;

	part = cf_malloc(sizeof(t_token_part));
	if (!part)
		return (NULL);
	part->start_pos = start_pos;
	part->end_pos = start_pos + ft_strlen(token->value) - 1;
	part->is_quoted = is_token_quoted(token);
	part->next = NULL;
	return (part);
}

/**
 * @brief Appends a token part to the linked list.
 *
 * Updates the head and current_part pointers and returns the new current.
 *
 * @param head Pointer to the head of the list (may be updated if empty).
 * @param current_part Current last node of the list.
 * @param new_part New token part to append.
 * @return Updated current_part pointer after appending.
 */
static t_token_part	*append_token_part(t_token_part **head,
	t_token_part *current_part, t_token_part *new_part)
{
	if (!*head)
	{
		*head = new_part;
		return (new_part);
	}
	else
	{
		current_part->next = new_part;
		return (new_part);
	}
}

/**
 * @brief Creates a linked list of token parts from a range of tokens.
 *
 * Iterates from tokens_start up to (but not including) tokens_end,
 * creating t_token_part nodes for each token and linking them into a list.
 * Frees already allocated parts if any allocation fails.
 *
 * @param tokens_start Start of the token list.
 * @param tokens_end End of the token list (exclusive).
 * @return Pointer to the head of the linked list of token parts,
 *			or NULL if allocation failed.
 */
t_token_part	*create_token_parts_list(t_list *tokens_start,
	t_list *tokens_end)
{
	t_token_part	*head;
	t_token_part	*current_part;
	t_token_part	*new_part;
	t_list			*token_node;
	int				pos;

	head = NULL;
	current_part = NULL;
	token_node = tokens_start;
	pos = 0;
	while (token_node && token_node != tokens_end)
	{
		new_part = create_token_part((t_token *)token_node->content, pos);
		if (!new_part)
		{
			free_token_parts(head);
			return (NULL);
		}
		current_part = append_token_part(&head, current_part, new_part);
		pos += ft_strlen(((t_token *)token_node->content)->value);
		token_node = token_node->next;
	}
	return (head);
}

/**
 * @brief Frees a linked list of token parts.
 *
 * Iterates through the linked list of t_token_part structures and frees
 * each node using centralized memory management.
 *
 * @param parts Pointer to the head of the token parts list.
 */
void	free_token_parts(t_token_part *parts)
{
	t_token_part	*next;

	while (parts)
	{
		next = parts->next;
		cf_free_one(parts);
		parts = next;
	}
}

/**
 * @brief Checks if the character at index is inside a quoted token part.
 *
 * @param parts Linked list of token parts.
 * @param index Index of the character in the string.
 * @return true if the character is inside a quoted part, false otherwise.
 */
static bool	is_in_quoted_part(t_token_part *parts, int index)
{
	t_token_part	*current;

	current = parts;
	while (current)
	{
		if (index >= current->start_pos && index <= current->end_pos
			&& current->is_quoted)
			return (true);
		current = current->next;
	}
	return (false);
}

/**
 * @brief Checks if the string contains unquoted wildcards '*' or '?'.
 *
 * Iterates through the string and returns true if a wildcard is found
 * outside of quoted token parts.
 *
 * @param str The string to check.
 * @param parts Linked list of token parts with quotation info.
 * @return true if there is at least one unquoted wildcard, false otherwise.
 */
bool	has_unquoted_wildcards(char *str, t_token_part *parts)
{
	int	len;
	int	i;

	len = ft_strlen(str);
	i = 0;
	while (i < len)
	{
		if (str[i] == '*' || str[i] == '?')
		{
			if (!is_in_quoted_part(parts, i))
				return (true);
		}
		i++;
	}
	return (false);
}

/**
 * @brief Creates a Bash-compatible pattern string with escaped wildcards.
 *
 * Iterates through the input string and escapes wildcard characters (*, ?)
 * if they appear inside quoted token parts. Allocates a new string to hold
 * the pattern.
 *
 * @param str Input string.
 * @param parts Linked list of token parts.
 * @return Allocated string containing the pattern, or
 * 			NULL on allocation failure.
 */
char	*create_bash_compatible_pattern(char *str, t_token_part *parts)
{
	int		len;
	char	*pattern;
	int		pattern_pos;
	int		i;
	char	c;

	len = ft_strlen(str);
	pattern = cf_malloc(len * 2 + 1);
	if (!pattern)
		return (NULL);
	pattern_pos = 0;
	i = 0;
	while (i < len)
	{
		c = str[i];
		if ((c == '*' || c == '?') && is_in_quoted_part(parts, i))
			pattern[pattern_pos++] = '\\';
		pattern[pattern_pos++] = c;
		i++;
	}
	pattern[pattern_pos] = '\0';
	return (pattern);
}

/**
 * @brief Matches a single character in pattern with escape.
 *
 * Handles '\' escape in pattern, comparing the next character literally.
 *
 * @param str Current position in string.
 * @param pattern Current position in pattern (points to '\').
 * @return 1 if matched, 0 otherwise.
 */
static int	match_escape(const char *str, const char *pattern)
{
	if (*str != *(pattern + 1))
		return (0);
	return (matches_pattern_with_escape(str + 1, pattern + 2));
}

/**
 * @brief Matches a string against a '*' wildcard in the pattern.
 *
 * Skips consecutive '*' and recursively tries to match the remaining string
 * with the rest of the pattern.
 *
 * @param str Current position in string.
 * @param pattern Current position in pattern (points to '*').
 * @return 1 if matched, 0 otherwise.
 */
static int	match_star(const char *str, const char *pattern)
{
	while (*pattern == '*')
		pattern++;
	if (*pattern == '\0')
		return (1);
	while (*str)
	{
		if (matches_pattern_with_escape(str, pattern))
			return (1);
		str++;
	}
	return (matches_pattern_with_escape(str, pattern));
}

/**
 * @brief Matches a string against a pattern with wildcards and escapes.
 *
 * Supports:
 * - '*' matches any sequence of characters
 * - '?' matches any single character
 * - '\' escapes the next character in the pattern
 *
 * @param str Input string to match.
 * @param pattern Pattern string containing wildcards and escapes.
 * @return 1 if string matches pattern, 0 otherwise.
 */
int	matches_pattern_with_escape(const char *str, const char *pattern)
{
	if (*pattern == '\0')
		return (*str == '\0');
	if (*pattern == '\\' && *(pattern + 1) != '\0')
		return (match_escape(str, pattern));
	if (*pattern == '*')
		return (match_star(str, pattern));
	if (*str == '\0')
		return (0);
	if (*pattern == *str || *pattern == '?')
		return (matches_pattern_with_escape(str + 1, pattern + 1));
	return (0);
}

/**
 * @brief Returns a dynamically allocated array with the original pattern.
 *
 * Used when no wildcard expansion is needed or no matches found.
 *
 * @param pattern Original string.
 * @return Null-terminated array with pattern copy.
 */
static char	**return_single_pattern(const char *pattern)
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
static int	count_matching_files(const char *pattern)
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
static bool	append_match_to_result(char **result, int *i, const char *name)
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
static char	**collect_matching_files(const char *pattern, int count)
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

/**
 * @brief Expands a pattern with wildcards to matching filenames.
 *
 * If no wildcards or no matches, returns an array with the original pattern.
 *
 * @param pattern Pattern string.
 * @return Null-terminated array of matching filenames.
 */
char	**expand_wildcards_with_escape(const char *pattern)
{
	int	count;

	if (!ft_strchr(pattern, '*') && !ft_strchr(pattern, '?'))
		return (return_single_pattern(pattern));
	count = count_matching_files(pattern);
	if (count == 0)
		return (return_single_pattern(pattern));
	return (collect_matching_files(pattern, count));
}

static void	handle_wildcard_results(t_input *input, char *expanded_value,
	char *bash_pattern, t_token_part *parts)
{
	char	**wildcards;
	int		i;

	wildcards = expand_wildcards_with_escape(bash_pattern);
	if (wildcards && wildcards[0])
	{
		i = 0;
		while (wildcards[i])
		{
			input->args = append_arg(input->args, wildcards[i]);
			i++;
		}
		free_expanded_wildcards(wildcards);
	}
	else
		input->args = append_arg(input->args, expanded_value);
	cf_free_one(expanded_value);
	cf_free_one(bash_pattern);
	free_token_parts(parts);
}

static bool	handle_simple_expansion(t_input *input, char *expanded_value,
	t_token_part *parts, char **bash_pattern)
{
	if (!parts)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		return (true);
	}
	if (!has_unquoted_wildcards(expanded_value, parts))
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return (true);
	}
	*bash_pattern = create_bash_compatible_pattern(expanded_value, parts);
	if (!*bash_pattern)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
		free_token_parts(parts);
		return (true);
	}
	return (false);
}

void	handle_bash_compatible_wildcard_expansion(t_input *input,
	char *expanded_value, t_list *tokens_start, t_list *tokens_end)
{
	t_token_part	*parts;
	char			*bash_pattern;

	parts = create_token_parts_list(tokens_start, tokens_end);
	if (handle_simple_expansion(input, expanded_value, parts, &bash_pattern))
		return ;
	handle_wildcard_results(input, expanded_value, bash_pattern, parts);
}

/**
 * @brief Handles fallback for heredoc if no arguments are present.
 *
 * Sets a default argument ':' if heredoc is used without any command
 * arguments, ensuring valid command execution.
 *
 * @param input Pointer to the input structure.
 */
void	handle_heredoc_fallback(t_input *input)
{
	if ((!input->args || !input->args[0]) && input->heredoc_count > 0)
	{
		input->args = cf_malloc(sizeof(char *) * 2);
		if (!input->args)
		{
			input->syntax_ok = false;
			return ;
		}
		input->args[0] = cf_strdup(":");
		input->args[1] = NULL;
	}
}

/**
 * @brief Handles a regular word token, adding it to command arguments.
 *
 * Adds the expanded word to the input arguments if not empty.
 * Frees the expanded value after use.
 *
 * @param input Pointer to the input structure.
 * @param expanded_value Expanded word string.
 */
void	handle_regular_word(t_input *input, char *expanded_value)
{
	if (ft_strlen(expanded_value) > 0)
	{
		input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
	}
	else if (expanded_value)
	{
		input->args = append_arg(input->args, cf_strdup(""));
		cf_free_one(expanded_value);
	}
	else
		cf_free_one(expanded_value);
}

/**
 * @brief Handles wildcard expansion for word tokens.
 *
 * Expands wildcards in the word and adds all matches to command arguments.
 * If no matches, adds the original word. Frees memory after use.
 *
 * @param input Pointer to the input structure.
 * @param current_tok Pointer to the current token.
 * @param expanded_value Expanded word string.
 */
void	handle_wildcard_expansion(t_input *input,
			t_token *current_tok, char *expanded_value)
{
	char	**wildcards;
	int		i;

	if (current_tok->type == TOKEN_WORD
		&& (ft_strchr(expanded_value, '*') || ft_strchr(expanded_value, '?')))
	{
		wildcards = expand_wildcards(expanded_value);
		if (wildcards && wildcards[0])
		{
			i = 0;
			while (wildcards[i])
			{
				input->args = append_arg(input->args, wildcards[i]);
				i++;
			}
			free_expanded_wildcards(wildcards);
		}
		else
			input->args = append_arg(input->args, expanded_value);
		cf_free_one(expanded_value);
	}
	else
		handle_regular_word(input, expanded_value);
}

/**
 * @brief Concatenates adjacent word tokens into a single string.
 *
 * Combines adjacent tokens without spaces, expanding each and joining
 * them into one argument string for the command.
 *
 * @param current_tokens Pointer to the token list pointer.
 * @param expanded_value Initial expanded value string.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Combined expanded string (must be freed).
 */
char	*concatenate_adjacent_tokens(t_list **current_tokens,
			char *expanded_value, t_env *env, t_shell *shell)
{
	t_list	*next_token;
	char	*next_expanded;
	char	*combined;

	next_token = (*current_tokens)->next;
	while (next_token && !((t_token *)next_token->content)->has_space
		&& is_adjacent_word_token((t_token *)next_token->content))
	{
		next_expanded = expand_token_value((t_token *)next_token->content,
				env, shell);
		if (next_expanded)
		{
			combined = ft_strjoin_free(expanded_value, next_expanded);
			cf_free_one(expanded_value);
			cf_free_one(next_expanded);
			expanded_value = combined;
			*current_tokens = next_token;
			next_token = next_token->next;
		}
		else
			break ;
	}
	return (expanded_value);
}

/**
 * @brief Handles a word token, expanding and adding to command arguments.
 *
 * Expands the token value, concatenates adjacent tokens, handles wildcards,
 * and advances the token list pointer.
 *
 * @param input Pointer to the input structure.
 * @param current_tokens Pointer to the token list pointer.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return true if successful, false otherwise.
 */
bool	handle_word_token(t_input *input,
		t_list **current_tokens, t_env *env, t_shell *shell)
{
	t_token	*current_tok;
	char	*expanded_value;
	t_list	*tokens_start;
	t_list	*tokens_end_next;

	current_tok = (t_token *)(*current_tokens)->content;
	tokens_start = *current_tokens;
	expanded_value = expand_token_value(current_tok, env, shell);
	if (!expanded_value)
		return (input->syntax_ok = false, false);
	expanded_value = concatenate_adjacent_tokens(current_tokens,
			expanded_value, env, shell);
	tokens_end_next = (*current_tokens)->next;
	handle_bash_compatible_wildcard_expansion(input, expanded_value,
		tokens_start, tokens_end_next);
	*current_tokens = (*current_tokens)->next;
	return (true);
}
