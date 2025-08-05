/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:15:20 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/05 17:55:31 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		input->args = append_arg(input->args, "\0");
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
		if (wildcards)
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
	t_list	**current_tokens, t_env *env, t_shell *shell)
{
	t_token	*current_tok;
	char	*expanded_value;

	current_tok = (t_token *)(*current_tokens)->content;
	expanded_value = expand_token_value(current_tok, env, shell);
	if (!expanded_value)
		return (input->syntax_ok = false, false);
	expanded_value = concatenate_adjacent_tokens(current_tokens,
			expanded_value, env, shell);
	handle_wildcard_expansion(input, current_tok, expanded_value);
	*current_tokens = (*current_tokens)->next;
	return (true);
}
