/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:30:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:29:58 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include <stdbool.h>
# include "types.h"

//executor_main_1.c
bool	is_builtin_command(const char *command);
int		execute_external_command(t_input *command, t_shell *shell);
int		execute_builtin_command(t_input *command, t_shell *shell);
int		execute_command_node(t_ast_node *node, t_shell *shell);
int		execute_node(t_ast_node *node, t_shell *shell);

//executor_main_2.c
void	execute_external_child(t_input *command, t_shell *shell);

//executor_other.c
int		execute_and_node(t_ast_node *node, t_shell *shell);
int		execute_or_node(t_ast_node *node, t_shell *shell);
void	execute_subshell_child(t_ast_node *node, t_shell *shell);
int		execute_subshell_node(t_ast_node *node, t_shell *shell);

//executor_pipe_1.c
int		execute_pipe_node(t_ast_node *node, t_shell *shell);

//executor_pipe_2.c
bool	handle_heredocs_in_pipeline(t_ast_node *node);

#endif
