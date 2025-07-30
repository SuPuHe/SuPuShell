/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 11:58:51 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:47:02 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Helper function to skip whitespace
void	skip_whitespace(const char *line, int *i, bool *had_space)
{
	*had_space = false;
	while (ft_isspace(line[*i]))
	{
		*had_space = true;
		(*i)++;
	}
}

// Helper function to add token to list
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

t_list	*tokenize(const char *line)
{
	t_list	*tokens;
	int		i;
	bool	had_space;
	t_token	*new_token;
	char	*word_str;

	tokens = NULL;
	i = 0;
	while (line[i])
	{
		skip_whitespace(line, &i, &had_space);
		if (!line[i])
			break ;
		new_token = handle_operator_tokens(line, &i);
		if (!new_token)
			new_token = handle_redirection_tokens(line, &i);
		if (!new_token)
			new_token = handle_quote_tokens(line, &i, &tokens);
		if (!new_token)
		{
			word_str = extract_non_quoted_word(line, &i);
			new_token = create_token(TOKEN_WORD, word_str);
		}
		add_token_to_list(new_token, had_space, &tokens);
	}
	return (add_end_token(&tokens), tokens);
}
