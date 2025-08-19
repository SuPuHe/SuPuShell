/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:15:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/19 17:21:35 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
