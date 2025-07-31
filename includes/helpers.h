/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:15:11 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/31 10:30:10 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HELPERS_H
# define HELPERS_H

# include "types.h"

//helpers
char	*substr_dup(const char *start, const char *end);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_strjoin_free(char *s1, const char *s2);
char	*ft_strcpy(char *dest, const char *src);
char	*ft_strcat(char *dest, const char *src);

//env_helpers
void	update_or_add_env_var(t_env **env, char *key, char *val);
int		parse_export_argument(char *arg, char **key, char **val);
void	remove_env_var(t_env **env, const char *key);
t_env	*create_env(char **envp);
char	*get_env_value(t_env *env, const char *key);

//path_helpers
char	*ft_strsep(char **str, const char *sep);
char	*search_path(const char *cmd, t_env *env);

//small_helpers
int		check_for_input(char *line);
int		ft_isspace(char c);
char	**append_arg(char **args, char *new_arg);
int		last_status_assign(t_input *command, t_shell *shell);

#endif
