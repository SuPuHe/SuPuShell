/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_main_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:45:49 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 17:58:28 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes an external command in the child process.
 *
 * Restores default signal handlers for SIGINT and SIGQUIT. Executes
 * the external command using run_external_command(). Cleans up
 * readline history and frees all tracked memory. Exits with status 127
 * if execution fails.
 *
 * @param command Pointer to the t_input structure representing the command.
 * @param shell Pointer to the shell structure.
 */
void	execute_external_child(t_input *command, t_shell *shell)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	run_external_command(command->args, shell->env);
	rl_clear_history();
	cf_free_all();
	exit(127);
}
