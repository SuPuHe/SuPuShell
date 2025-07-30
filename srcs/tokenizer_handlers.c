/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_handlers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:01:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:32:24 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*extract_non_quoted_word(const char *line, int *i)
{
	int		start;
	char	*s;

	start = *i;
	while (line[*i] && !ft_isspace(line[*i])
		&& !is_operator(line[*i]) && line[*i] != '\'' && line[*i] != '"')
		(*i)++;
	s = cf_substr(line, start, *i - start);
	return (s);
}

char	*extract_subsegment(const char *line, int *i, char delimiter)
{
	int		start;
	char	*s;

	start = *i;
	while (line[*i] && line[*i] != delimiter)
		(*i)++;
	s = cf_substr(line, start, *i - start);
	return (s);
}

// Helper function to handle quote tokens
t_token	*handle_quote_tokens(const char *line, int *i, t_list **tokens)
{
	char	*value;

	if (line[*i] == '\'')
	{
		(*i)++;
		value = extract_subsegment(line, i, '\'');
		if (line[*i] == '\'')
			(*i)++;
		else
			return (free_token_list(*tokens), NULL);
		return (create_token(TOKEN_SINGLE_QUOTE_WORD, value));
	}
	else if (line[*i] == '"')
	{
		(*i)++;
		value = extract_subsegment(line, i, '"');
		if (line[*i] == '"')
			(*i)++;
		else
			return (free_token_list(*tokens), NULL);
		return (create_token(TOKEN_DOUBLE_QUOTE_WORD, value));
	}
	return (NULL);
}

// Helper function to handle redirection tokens
t_token	*handle_redirection_tokens(const char *line, int *i)
{
	if (ft_strncmp(&line[*i], ">>", 2) == 0)
	{
		*i += 2;
		return (create_token(TOKEN_REDIR_APPEND, NULL));
	}
	else if (ft_strncmp(&line[*i], "<<", 2) == 0)
	{
		*i += 2;
		return (create_token(TOKEN_HEREDOC, NULL));
	}
	else if (line[*i] == '>')
	{
		(*i)++;
		return (create_token(TOKEN_REDIR_OUT, NULL));
	}
	else if (line[*i] == '<')
	{
		(*i)++;
		return (create_token(TOKEN_REDIR_IN, NULL));
	}
	return (NULL);
}

// Helper function to handle operator tokens
t_token	*handle_operator_tokens(const char *line, int *i)
{
	if (ft_strncmp(&line[*i], "&&", 2) == 0)
		return (*i += 2, create_token(TOKEN_AND, NULL));
	else if (ft_strncmp(&line[*i], "||", 2) == 0)
		return (*i += 2, create_token(TOKEN_OR, NULL));
	else if (line[*i] == '|')
		return ((*i)++, create_token(TOKEN_PIPE, NULL));
	else if (line[*i] == '(')
		return ((*i)++, create_token(TOKEN_LPAREN, NULL));
	else if (line[*i] == ')')
		return ((*i)++, create_token(TOKEN_RPAREN, NULL));
	else if (line[*i] == '&')
		return ((*i)++, create_token(TOKEN_AMPERSAND, NULL));
	return (NULL);
}
