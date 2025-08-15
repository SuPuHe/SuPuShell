/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 15:30:25 by vpushkar          #+#    #+#             */
/*   Updated: 2025/08/15 16:55:43 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMANDS_H
# define COMMANDS_H

//commands_1
void	do_pwd(void);
void	do_cd(t_shell *shell, char **commands, t_env **env);
void	cd_logic(t_shell *shell, char **commands, t_env **env,
			const char *home);
bool	handle_parsed_export_arg(char *key, char *val, t_env **env);
void	print_all_env_vars(t_env *env);

//commands_2
void	print_error(char *argv);
void	do_echo(char **argv);
void	run_external_command(char **argv, t_env *env);
void	do_env(t_env *env, char **argv);

//commands_3
void	do_export(t_shell *shell, char **argv, t_env **env);
void	do_unset(char **argv, t_env **env);

//commands_helpers
void	goto_prev_dir(t_env **env);
char	**build_envp(t_env *env);
void	bubble_sort_env_array(t_env **array, int count);
t_env	**create_sorted_env_array(t_env *env, int *count);

#endif
