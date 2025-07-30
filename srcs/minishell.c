/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 10:07:36 by omizin            #+#    #+#             */
/*   Updated: 2025/07/30 12:48:57 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// // Prototypes for helper functions
// static int  execute_builtin_command(t_input *command, t_shell *shell);
// static int  execute_external_command(t_input *command, t_shell *shell);
// static int  execute_command_node(t_ast_node *node, t_shell *shell);
// static int  execute_pipe_node(t_ast_node *node, t_shell *shell);
// static void execute_pipe_child_left(t_ast_node *node, t_shell *shell, int *pipefd);
// static void execute_pipe_child_right(t_ast_node *node, t_shell *shell, int *pipefd);
// static int  execute_subshell_node(t_ast_node *node, t_shell *shell);
// static void execute_subshell_child(t_ast_node *node, t_shell *shell);
// static int  execute_and_node(t_ast_node *node, t_shell *shell);
// static int  execute_or_node(t_ast_node *node, t_shell *shell);
// static bool is_builtin_command(const char *command);
// int	execute_node(t_ast_node *node, t_shell *shell);
// static void	init_input_structure(t_input *input, t_env *env, t_shell *shell);
// static bool	is_command_terminator(t_token *token);
// static bool	validate_redirection_tokens(t_list **current_tokens, t_input *input);
// static void	handle_heredoc_redirection(t_input *input, t_token *filename_token);
// static char	*expand_filename_token(t_token *filename_token, t_env *env, t_shell *shell);
// static void	apply_output_redirection(t_input *input, t_token_type redir_type, char *expanded_value);
// static void	apply_input_redirection(t_input *input, char *expanded_value);
// static bool	handle_redirection_token(t_input *input, t_list **current_tokens, t_env *env, t_shell *shell);
// static bool	is_adjacent_word_token(t_token *token);
// static char	*concatenate_adjacent_tokens(t_list **current_tokens, char *expanded_value, t_env *env, t_shell *shell);
// static void	handle_wildcard_expansion(t_input *input, t_token *current_tok, char *expanded_value);
// static void	handle_regular_word(t_input *input, char *expanded_value);
// static void	handle_heredoc_fallback(t_input *input);
// static bool	handle_word_token(t_input *input, t_list **current_tokens, t_env *env, t_shell *shell);
// bool	apply_redirections(t_input *input);
// bool	is_operator(char c);
// t_ast_node	*parse_expression(t_list **tokens, t_shell *shell);
// void	free_ast(t_ast_node *node);
// void	free_token_list(t_list *tokens);



int	main(int argc, char **argv, char **envp)
{
	char	*line;
	t_shell	shell;

	(void)argc;
	(void)argv;
	billy_print();
	shell.env = create_env(envp);
	shell.last_exit_status = 0;
	shell.should_exit = 0;
	disable_echoctl();
	int interactive = 0;
	t_ast_node	*ast = NULL;
	while (1)
	{
		setup_signal();
		if (isatty(STDIN_FILENO))
		{
			line = readline(SHELLNAME);
			interactive = 1;
		}
		else
			line = get_next_line(STDIN_FILENO);
		if (g_sigint_exit_status == 1)
		{
			shell.last_exit_status = 1;
			g_sigint_exit_status = 0;
		}
		if (line)
			ast = parse(line, &shell);
		if (!line)
		{
			rl_clear_history();
			cf_free_all();
			break ;
		}
		if (*line && interactive)
			add_history(line);
		if (!check_for_input(line))
		{
			free(line);
			continue ;
		}
		if (ast)
		{
			execute_node(ast, &shell);
			free_ast(ast);
			ast = NULL;
		}
		free(line);
		if (shell.should_exit)
		{
			rl_clear_history();
			cf_free_all();
			break ;
		}
	}
	shell.env = NULL;
	if (ast)
		free_ast(ast);
	ast = NULL;
	cf_free_all();
	return (0);
}

//valgrind --leak-check=full --show-leak-kinds=all --suppressions=valgrind_readline.supp ./minishell
// ➜  SuPuShell git:(master) ✗ export FILE=test.txt
// ➜  SuPuShell git:(master) ✗ echo hi > $FILE
// ➜  SuPuShell git:(master) ✗ echo hi $FILE > $FILE
// ➜  SuPuShell git:(master) ✗ echo hi '$FILE' >> $FILE
// ➜  SuPuShell git:(master) ✗ echo hi '$FILE' >> '$FILE'
// ➜  SuPuShell git:(master) ✗ cat << $FILE
