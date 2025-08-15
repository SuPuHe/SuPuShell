/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:30:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:23:56 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "types.h"
# include "stdbool.h"

//parser_command.c
bool		is_command_terminator(t_token *token);
void		init_input_structure(t_input *input, t_env *env, t_shell *shell);
t_ast_node	*parse_primary_subshell(t_list **tokens, t_shell *shell);
t_ast_node	*parse_primary_command(t_list **tokens, t_shell *shell);
t_input		parse_command_from_tokens(t_list **current_tokens,
				t_env *env, t_shell *shell);

//parser_main.c
t_ast_node	*parse_primary(t_list **tokens, t_shell *shell);
t_ast_node	*parse_pipeline(t_list **tokens, t_shell *shell);
t_ast_node	*parse_and_or(t_list **tokens, t_shell *shell);
t_ast_node	*parse_expression(t_list **tokens, t_shell *shell);
t_ast_node	*parse(const char *line, t_shell *shell);

//parser_redirection_handlers_1.c
void		apply_output_redirection(t_input *input,
				t_token_type redir_type, char *expanded_value);
void		apply_input_redirection(t_input *input, char *expanded_value);
bool		apply_redirections(t_input *input);
bool		apply_input_redirections(t_input *input);
//parser_redirection_handlers_2.c
bool		apply_output_redirections(t_input *input);

//parser_redirection_1.c
t_list		*collect_d_tokens_and_sb(t_list *start,
				t_string_builder *sb, bool *quoted);
char		*expand_filename_token(t_token *filename_token,
				t_env *env, t_shell *shell);
void		handle_heredoc_redirection(t_input *input, t_token *filename_token);
bool		validate_redirection_tokens(t_list **current_tokens,
				t_input *input);
void		handle_heredoc_redirection_with_delimiter(
				t_input *input, char *delimiter, bool quoted);

//parser_redirection_2.c
bool		handle_heredoc(t_input *input);
void		free_heredoc_delimiters(t_input *input);

//parser_redirection_3.c
void		heredoc_child_process(t_input *input, char *filename, int index);
bool		handle_redirection_token(t_input *input,
				t_list **current_tokens, t_env *env);

//parser_redirection_4.c
char		*collect_heredoc_delimiter(t_list **current_tokens, bool *quoted);

//parser_word_second_part.c
bool		is_adjacent_word_token(t_token *token);

//parser_word.c
void		handle_heredoc_fallback(t_input *input);
void		handle_regular_word(t_input *input, char *expanded_value);
void		handle_wildcard_expansion(t_input *input,
				t_token *current_tok, char *expanded_value);
char		*concatenate_adjacent_tokens(t_list **current_tokens,
				char *expanded_value, t_env *env, t_shell *shell);
bool		handle_word_token(t_input *input,
				t_list **current_tokens, t_env *env, t_shell *shell);

#endif
