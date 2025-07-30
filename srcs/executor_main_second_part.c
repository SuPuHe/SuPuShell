/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_main_second_part.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:45:49 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:35:04 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execute_external_child(t_input *command, t_shell *shell)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	run_external_command(command->args, shell->env);
	rl_clear_history();
	cf_free_all();
	exit(127);
}
