/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_64.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ademenet <ademenet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 15:50:23 by ademenet          #+#    #+#             */
/*   Updated: 2017/12/27 15:59:16 by ademenet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/ft_nm.h"

int					check_addr_error(struct load_command *lc)
{
	// To be implemented
	return (0);
}

void				sort_value_64(char *stringtable, struct nlist_64 *sort,
					int nsyms)
{
	int				i;
	int				flag;
	struct nlist_64	temp;

	while (++i < nsyms - 1)
	{
		i = -1;
		if (ft_strcmp(stringtable + sort[i].n_un.n_strx,
		stringtable + sort[i].n_un.n_strx) == 0 && 
		(sort[i].n_un.n_strx != 0 || sort[i + 1].n_un.n_strx != 0))
		{
			if (sort[i].n_un.n_strx > sort[i + 1].n_un.n_strx)
			{
				temp = sort[i + 1];
				sort[i + 1] = sort[i];
				sort[i] = temp;
			}
		}
	}
	return ;
}

struct nlist_64		*init_sort_64(struct nlist_64 *array,
					int nsyms)
{
	int				i;
	struct nlist_64	*sort;
	
	i = -1;
	sort = (struct nlist_64 *)malloc(nsyms * sizeof(struct nlist_64));
	while (++i < nsyms)
		sort[i] = array[i];
	return (sort);
}

void	print_debug(char *stringtable, struct nlist_64 *sort, int nsyms) {
	int i = 0;
	while (i < nsyms)
	{
		fprintf(stderr, "sorted %d : %s\n", i, stringtable + sort[i].n_un.n_strx);
		i++;
	}

}

struct nlist_64		*sort_64(char *stringtable, struct nlist_64 *array, 
					int nsyms)
{
	int				i;
	int				j;
	struct nlist_64	temp;
	struct nlist_64	*sort;

	i = -1;

	sort = init_sort_64(array, nsyms);
	while (++i < nsyms)
	{
		j = -1;
		while (++j < nsyms)
		{
			if (ft_strcmp(stringtable + sort[i].n_un.n_strx,
			stringtable + sort[j].n_un.n_strx) < 0)
			{
				temp = sort[i];
				sort[i] = sort[j];
				sort[j] = temp;
			}
		}
	}
	return (sort);
}

void				print_output_64(int nsyms, int symoff, int stroff, 
					char *ptr)
{
	int					i;
	char				*stringtable;
	struct nlist_64		*array;

	array = (void *)ptr + symoff;
	stringtable = (void *)ptr + stroff;
	// TODO checker les ranges
	array = sort_64(stringtable, array, nsyms);
	sort_value_64(stringtable, array, nsyms);
	for (i = 0; i < nsyms; ++i)
	{
		const char			*n_type;
		n_type = NULL;
		uint8_t 			mask;
		mask = array[i].n_type & N_TYPE;
		if (mask == N_UNDF)
			n_type = "N_UNDF";
		else if (mask == N_ABS)
			n_type = "N_ABS";
		else if (mask == N_SECT)
		{
			// TODO faire get_section qui recupere la section
			// cf. nlist.h l.146
			// n_sect est un ordinal pour trouver dans quelle section
			// nous nous trouvons >>> Trouver un moyen de determiner
			// dans quelle section je me trouve vraiment (T, d, S, etc.)
			n_type = "N_SECT";
			// ft_printf("n_sect : %d\n", array[i].n_sect);
		}
		else if (mask == N_PBUD)
			n_type = "N_PBUD";
		else if (mask == N_INDR)
			n_type = "N_INDR";
		else
			return ;
		if (array[i].n_value == 0)
			ft_printf("%15llx ", array[i].n_value);
		else
			ft_printf("%015llx ", array[i].n_value);
		ft_printf("%s ", n_type);
		ft_printf("%s\n", stringtable + array[i].n_un.n_strx);
	}
}

void				handle_64(char *ptr)
{
	int						i;
	int						ncmds;
	struct mach_header_64	*header;
	struct load_command		*lc;
	struct symtab_command	*sym;

	header = (struct mach_header_64 *)ptr;
	ncmds = header->ncmds;
	lc = (void *)ptr + sizeof(*header);
	while (i++ < ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			print_output_64(sym->nsyms, sym->symoff, sym->stroff, ptr);
			break ;
		}
		lc = (void *)lc + lc->cmdsize;
		if (check_addr_error(lc))
		{
			error_display(ADDR_ERR);
			return ;
		}
	}
}