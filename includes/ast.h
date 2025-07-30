/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 17:24:19 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 17:30:55 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
# define AST_H

# include "types.h"

//ast.c
t_ast_node		*create_cmd_node(t_input *cmd);
t_ast_node		*create_binary_node(t_node_type type,
					t_ast_node *left, t_ast_node *right);
t_ast_node		*create_subshell_node(t_ast_node *child);
void			free_ast(t_ast_node *node);

#endif
