/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:58:51 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/12 16:42:09 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Skips whitespace characters in the input line.
 *
 * Advances the index past any whitespace and sets the had_space flag
 * if any spaces were found.
 *
 * @param line Input line string.
 * @param i Pointer to the current index.
 * @param had_space Pointer to the space flag.
 */
void	skip_whitespace(const char *line, int *i, bool *had_space)
{
	*had_space = false;
	while (ft_isspace(line[*i]))
	{
		*had_space = true;
		(*i)++;
	}
}

/**
 * @brief Adds a token to the token list.
 *
 * Sets the space flag for the token and appends it to the token list.
 *
 * @param new_token Token to add.
 * @param had_space Space flag.
 * @param tokens Pointer to the token list.
 */
void	add_token_to_list(t_token *new_token, bool had_space, t_list **tokens)
{
	t_list	*node;

	if (new_token)
	{
		new_token->has_space = had_space;
		node = cf_lstnew(new_token);
		ft_lstadd_back(tokens, node);
	}
}

/**
 * @brief Frees the memory of a token list and its contents.
 *
 * Iterates through the token list, freeing each token and node.
 *
 * @param tokens Token list to free.
 */
void	free_token_list(t_list *tokens)
{
	t_list	*current;
	t_token	*token;

	while (tokens)
	{
		current = tokens;
		tokens = tokens->next;
		token = (t_token *)current->content;
		if (token && token->value)
			cf_free_one(token->value);
		if (token)
			cf_free_one(token);
		current->next = NULL;
		cf_free_one(current);
	}
}

/**
 * @brief Creates a new token with the given type and value.
 *
 * Allocates and initializes a token structure for the parser.
 *
 * @param type Token type.
 * @param value Token value string.
 * @return Pointer to the new token structure.
 */
t_token	*create_token(t_token_type type, const char *value)
{
	t_token	*token;

	token = cf_malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = cf_strdup(value);
	else
		token->value = NULL;
	return (token);
}

/**
 * @brief Tokenizes the input line into a list of tokens.
 *
 * Iterates through the input string, extracting tokens using
 * process_next_token and adding them to the token list. Handles spaces
 * and appends an end token at the end of the list.
 *
 * @param line Input line to tokenize.
 * @return Pointer to the list of tokens.
 */
t_list	*tokenize(const char *line)
{
	t_list	*tokens;
	int		i;
	bool	had_space;
	t_token	*new_token;

	tokens = NULL;
	i = 0;
	while (line[i])
	{
		new_token = process_next_token(line, &i, &tokens, &had_space);
		if (!new_token && !line[i])
			break ;
		if (new_token)
			add_token_to_list(new_token, had_space, &tokens);
	}
	add_end_token(&tokens);
	return (tokens);
}
