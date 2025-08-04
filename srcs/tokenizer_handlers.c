/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_handlers.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:01:04 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/04 17:38:14 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Extracts a non-quoted word from the input line.
 *
 * Reads characters until a space, operator, or quote is found.
 * Returns a newly allocated substring.
 *
 * @param line Input line string.
 * @param i Pointer to current index.
 * @return Pointer to extracted word.
 */
char	*extract_non_quoted_word(const char *line, int *i)
{
	int		start;
	char	*s;
	char	quote_char;

	quote_char = '\0';
	start = *i;
	if (line[*i] == '$' && (*i + 1 < (int)ft_strlen(line)) &&
		(line[*i + 1] == '\'' || line[*i + 1] == '\"'))
		{
			quote_char = line[*i + 1];
			(*i) += 2;
			while (line[*i] && line[*i] != quote_char)
				(*i)++;
			if (line[*i] == quote_char) 
				(*i)++;
	}
	else
	{
		while (line[*i] && !ft_isspace(line[*i])
			&& !is_operator(line[*i]) && line[*i] != '\'' && line[*i] != '\"')
			(*i)++;
	}
	s = cf_substr(line, start, *i - start);
	return (s);
}

/**
 * @brief Extracts a substring up to a delimiter character.
 *
 * Reads characters until the delimiter is found. Returns a newly
 * allocated substring.
 *
 * @param line Input line string.
 * @param i Pointer to current index.
 * @param delimiter Delimiter character.
 * @return Pointer to extracted substring.
 */
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

/**
 * @brief Handles quote tokens in the input line.
 *
 * Extracts quoted substrings and creates corresponding tokens.
 * Returns NULL on error or unmatched quotes.
 *
 * @param line Input line string.
 * @param i Pointer to current index.
 * @param tokens Pointer to token list.
 * @return Pointer to created token, or NULL on error.
 */
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

/**
 * @brief Handles redirection tokens in the input line.
 *
 * Detects and creates tokens for >, >>, <, << redirections.
 *
 * @param line Input line string.
 * @param i Pointer to current index.
 * @return Pointer to created token, or NULL if not matched.
 */
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

/**
 * @brief Handles operator tokens in the input line.
 *
 * Detects and creates tokens for &&, ||, |, (, ), & operators.
 *
 * @param line Input line string.
 * @param i Pointer to current index.
 * @return Pointer to created token, or NULL if not matched.
 */
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
