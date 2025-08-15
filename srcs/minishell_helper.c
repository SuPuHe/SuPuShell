/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_helper.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 17:06:05 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:09:47 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	is_line_empty_or_whitespace(char *line, t_shell *shell)
{
	int	i;

	i = 0;
	while (line[i] && ft_isspace(line[i]))
		i++;
	if (!line[i])
		return (true);
	shell->last_exit_status = 2;
	return (true);
}

static bool	validate_ast_syntax(t_ast_node *ast, t_shell *shell)
{
	if (!ast || (ast->type != NODE_CMD && ast->type != NODE_SUBSHELL
			&& (!ast->left || !ast->right)))
	{
		write(2, "Billyshell: syntax error near unexpected token\n", 48);
		shell->last_exit_status = 2;
		if (ast)
			free_ast(ast);
		return (false);
	}
	return (true);
}

bool	process_one_command_iteration(char *line,
		t_shell *shell, int interactive)
{
	t_ast_node	*ast;

	ast = NULL;
	if (*line && interactive)
		add_history(line);
	if (!check_for_input(line))
		return (is_line_empty_or_whitespace(line, shell));
	ast = parse(line, shell);
	if (!validate_ast_syntax(ast, shell))
		return (true);
	execute_node(ast, shell);
	free_ast(ast);
	return (true);
}
