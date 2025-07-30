/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vpushkar <vpushkar@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:23:19 by vpushkar          #+#    #+#             */
/*   Updated: 2025/07/30 13:45:27 by vpushkar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ast_node	*create_cmd_node(t_input *cmd)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_CMD;
	node->left = NULL;
	node->right = NULL;
	node->command = cmd;
	return (node);
}

t_ast_node	*create_binary_node(t_node_type type,
	t_ast_node *left, t_ast_node *right)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->left = left;
	node->right = right;
	node->command = NULL;
	return (node);
}

t_ast_node	*create_subshell_node(t_ast_node *child)
{
	t_ast_node	*node;

	node = cf_malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = NODE_SUBSHELL;
	node->left = child;
	node->right = NULL;
	node->command = NULL;
	return (node);
}

void	free_ast(t_ast_node *node)
{
	if (!node)
		return ;
	if (node->left)
		free_ast(node->left);
	if (node->right)
		free_ast(node->right);
	if (node->type == NODE_CMD && node->command)
	{
		cf_free_one(node->command);
	}
	cf_free_one(node);
}
