/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_main_second_part.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:45:49 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 18:23:36 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes an external command in a child process.
 *
 * Sets default signal handlers, runs the external command, clears history,
 * frees all memory, and exits with code 127 if execve fails.
 *
 * @param command Pointer to input command structure.
 * @param shell Pointer to shell structure.
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
