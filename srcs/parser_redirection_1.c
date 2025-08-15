/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_redirection_1.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:10:29 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 18:02:33 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Collects consecutive word tokens into a string builder.
 *
 * Iterates through tokens starting from @p start, appends their
 * values to the provided string builder until a non-word token,
 * a token with a space (not at start), or a quote boundary is found.
 * Updates the @p quoted flag if any token is quoted.
 *
 * @param start Pointer to the first token in the list to process.
 * @param sb Pointer to the string builder where token values are
 *			appended.
 * @param quoted Pointer to a bool that is set to true if any of the
 *			collected tokens are quoted.
 * @return Pointer to the first token that was not collected.
 */
t_list	*collect_d_tokens_and_sb(t_list *start,
	t_string_builder *sb, bool *quoted)
{
	t_list	*cur;
	t_token	*tok;

	cur = start;
	*quoted = false;
	while (cur)
	{
		tok = (t_token *)cur->content;
		if (!tok)
			break ;
		if (tok->type != TOKEN_WORD
			&& tok->type != TOKEN_SINGLE_QUOTE_WORD
			&& tok->type != TOKEN_DOUBLE_QUOTE_WORD)
			break ;
		if (tok->has_space && cur != start)
			break ;
		if (tok->type == TOKEN_SINGLE_QUOTE_WORD
			|| tok->type == TOKEN_DOUBLE_QUOTE_WORD)
			*quoted = true;
		sb_append(sb, tok->value, false);
		cur = cur->next;
	}
	return (cur);
}

/**
 * @brief Expands a filename token, handling quotes and variables.
 *
 * Returns the expanded filename string for redirection, handling
 * single quotes and variable expansion.
 *
 * @param filename_token Pointer to the filename token.
 * @param env Pointer to the environment structure.
 * @param shell Pointer to the shell structure.
 * @return Expanded filename string (must be freed).
 */
char	*expand_filename_token(t_token *filename_token,
	t_env *env, t_shell *shell)
{
	if (filename_token->type == TOKEN_SINGLE_QUOTE_WORD)
		return (cf_strdup(filename_token->value));
	else
		return (expand_string_variables(filename_token->value,
				env, shell, false));
}

/**
 * @brief Resizes the arrays used for heredoc delimiters and quotes.
 *
 * Allocates new arrays with size increased by 1, copies existing
 * values, frees the old arrays, and updates the input structure
 * to point to the new arrays.
 *
 * @param input Pointer to the t_input structure containing heredoc
 *				arrays.
 */
static void	resize_heredoc_arrays(t_input *input)
{
	char	**new_delims;
	bool	*new_quoted;
	int		i;

	new_delims = cf_malloc(sizeof(char *) * (input->heredoc_count + 2));
	new_quoted = cf_malloc(sizeof(bool) * (input->heredoc_count + 2));
	i = 0;
	while (i < input->heredoc_count)
	{
		new_delims[i] = input->heredoc_delimiters[i];
		new_quoted[i] = input->heredoc_is_quoted[i];
		i++;
	}
	cf_free_one(input->heredoc_delimiters);
	cf_free_one(input->heredoc_is_quoted);
	input->heredoc_delimiters = new_delims;
	input->heredoc_is_quoted = new_quoted;
}

/**
 * @brief Handles heredoc redirection by storing the delimiter.
 *
 * Sets the heredoc field in the input structure to the delimiter value.
 *
 * @param input Pointer to the input structure.
 * @param filename_token Pointer to the heredoc delimiter token.
 */
void	handle_heredoc_redirection_with_delimiter(
	t_input *input, char *delimiter, bool quoted)
{
	if (!input->heredoc_delimiters)
	{
		input->heredoc_delimiters = cf_malloc(sizeof(char *) * 2);
		input->heredoc_is_quoted = cf_malloc(sizeof(bool) * 2);
		input->heredoc_count = 0;
	}
	else
	{
		resize_heredoc_arrays(input);
	}
	input->heredoc_delimiters[input->heredoc_count] = delimiter;
	input->heredoc_is_quoted[input->heredoc_count] = quoted;
	input->heredoc_count++;
	input->heredoc_delimiters[input->heredoc_count] = NULL;
}

/**
 * @brief Validates the presence of a token after a redirection operator.
 *
 * Advances the token list and checks if the next token exists and is valid.
 * If the next token is missing,
 * sets the syntax flag to false and returns false.
 *
 * @param current_tokens Pointer to the token list pointer.
 * @param input Pointer to the input structure to update the syntax flag.
 * @return true if the next token is valid, false otherwise.
 */
bool	validate_redirection_tokens(t_list **current_tokens, t_input *input)
{
	t_token	*tok;

	if (!*current_tokens || !(*current_tokens)->next)
		return (input->syntax_ok = false, false);
	tok = (t_token *)(*current_tokens)->next->content;
	if (tok->type == TOKEN_REDIR_OUT || tok->type == TOKEN_REDIR_IN
		|| tok->type == TOKEN_REDIR_APPEND || tok->type == TOKEN_HEREDOC
		|| !tok->value)
		return (input->syntax_ok = false, false);
	return (true);
}
