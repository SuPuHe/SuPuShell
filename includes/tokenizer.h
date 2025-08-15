/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:30:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:26:21 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZER_H
# define TOKENIZER_H

# include "types.h"

//tokenizer_handlers.c
char	*extract_non_quoted_word(const char *line, int *i);
char	*extract_subsegment(const char *line, int *i, char delimiter);
t_token	*handle_quote_tokens(const char *line, int *i, t_list **tokens);
t_token	*handle_redirection_tokens(const char *line, int *i);
t_token	*handle_operator_tokens(const char *line, int *i);

//tokenizer_main_2.c
t_token	*process_next_token(const char *line,
			int *i, t_list **tokens_list, bool *had_space);
void	add_end_token(t_list **tokens);

//tokenizer_main_1.c
void	skip_whitespace(const char *line, int *i, bool *had_space);
void	add_token_to_list(t_token *new_token, bool had_space, t_list **tokens);
void	free_token_list(t_list *tokens);
t_token	*create_token(t_token_type type, const char *value);
t_list	*tokenize(const char *line);

#endif
