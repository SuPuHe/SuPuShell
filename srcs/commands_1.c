/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 11:39:33 by omizin            #+#    #+#             */
/*   Updated: 2025/08/15 17:51:35 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints the current working directory.
 *
 * Uses getcwd to get the current directory and prints it in green color.
 */
void	do_pwd(void)
{
	char	buf[512];

	getcwd(buf, sizeof(buf));
	printf("%s\n", buf);
}

/**
 * @brief Handles the logic of the built-in cd command.
 *
 * Changes the current working directory based on the given arguments:
 * - No argument or "~": changes to the home directory.
 * - "-": changes to the previous directory.
 * - Any other path: attempts to change to that directory.
 *
 * Updates shell->last_exit_status to 1 if chdir fails.
 *
 * @param shell Pointer to the shell structure (stores last exit status).
 * @param commands Array of command strings, where commands[0] is "cd".
 * @param env Pointer to the environment structure.
 * @param home String containing the home directory path.
 */
void	cd_logic(t_shell *shell, char **commands, t_env **env,
	const char *home)
{
	if (!commands[1] || ft_strncmp(commands[1], "~", 2) == 0)
		chdir(home);
	else if (ft_strncmp(commands[1], "-", 2) == 0)
		goto_prev_dir(env);
	else
	{
		if (chdir(commands[1]) == -1)
		{
			perror(BOLDRED"cd"RESET);
			shell->last_exit_status = 1;
		}
	}
}

/**
 * @brief Changes the current directory and updates environment variables.
 *
 * Handles cd with no arguments, '~', '-', and a specific path. Updates
 * OLDPWD and PWD environment variables after changing directory.
 *
 * @param commands Array of command arguments.
 * @param env Pointer to environment variable list.
 */
void	do_cd(t_shell *shell, char **commands, t_env **env)
{
	const char	*home;
	char		oldpwd[512];
	char		newpwd[512];

	if (commands[2])
	{
		ft_putstr_fd("Billyshell: cd: too many arguments\n", 2);
		shell->last_exit_status = 1;
		return ;
	}
	if (!getcwd(oldpwd, sizeof(oldpwd)))
		return ;
	home = getenv("HOME");
	cd_logic(shell, commands, env, home);
	if (getcwd(newpwd, sizeof(newpwd)))
	{
		update_or_add_env_var(env, "OLDPWD", oldpwd);
		update_or_add_env_var(env, "PWD", newpwd);
	}
}

/**
 * @brief Prints all environment variables in sorted order.
 *
 * Creates a sorted array of environment variable pointers, then prints
 * each variable in the format "declare -x KEY=\"VALUE\"" if VALUE exists.
 *
 * Uses cf_free_one to free the array after printing.
 *
 * @param env Pointer to the environment linked list.
 */
void	print_all_env_vars(t_env *env)
{
	t_env	**env_array;
	int		count;
	int		i;

	env_array = create_sorted_env_array(env, &count);
	if (!env_array)
		return ;
	i = 0;
	while (i < count)
	{
		printf("declare -x %s", env_array[i]->key);
		if (env_array[i]->value && env_array[i]->value[0])
			printf("=\"%s\"", env_array[i]->value);
		printf("\n");
		i++;
	}
	cf_free_one(env_array);
}

/**
 * @brief Handles a single parsed argument for the export built-in.
 *
 * Checks if the provided key is a valid variable name. If valid, it
 * updates the existing variable or adds a new one to the environment.
 * If invalid, prints an error message to stderr.
 *
 * @param key The name of the environment variable.
 * @param val The value to assign to the environment variable.
 * @param env Pointer to the environment linked list.
 *
 * @return true if the variable was valid and updated/added; false if
 * the key was invalid.
 */
bool	handle_parsed_export_arg(char *key, char *val, t_env **env)
{
	if (is_valid_var_name(key))
	{
		update_or_add_env_var(env, key, val);
		return (true);
	}
	else
	{
		ft_putstr_fd("Billyshell: export: not a valid identifier\n", 2);
		return (false);
	}
}
