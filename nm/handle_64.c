/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_64.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ademenet <ademenet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 15:50:23 by ademenet          #+#    #+#             */
/*   Updated: 2018/01/10 18:49:12 by ademenet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/ft_nm.h"

static int						get_symtab_sec_64(t_sym *symtab,
								struct segment_command_64 *seg,
								struct section_64 *sec, int *k)
{
	uint32_t					j;

	j = -1;
	while (++j < seg->nsects)
	{
		if (ft_strcmp(sec->sectname, SECT_TEXT) == 0 &&
			ft_strcmp(sec->segname, SEG_TEXT) == 0)
			symtab->text = (*k) + 1;
		else if (ft_strcmp(sec->sectname, SECT_DATA) == 0 &&
			ft_strcmp(sec->segname, SEG_DATA) == 0)
			symtab->data = (*k) + 1;
		else if (ft_strcmp(sec->sectname, SECT_BSS) == 0 &&
			ft_strcmp(sec->segname, SEG_DATA) == 0)
			symtab->bss = (*k) + 1;
		sec = (void *)sec + sizeof(struct section_64);
		if (check(sec))
			return (set_type_error("2 File truncated or someway invalid."));
		(*k)++;
	}
	return (EXIT_SUCCESS);
}

static int						get_symtab_64(t_sym *symtab,
								struct mach_header_64 *header,
								struct load_command *lc)
{
	uint32_t					i;
	int							k;
	struct segment_command_64	*seg;
	struct section_64			*sec;

	i = -1;
	k = 0;
	while (++i < header->ncmds)
	{
		if (lc->cmd == LC_SEGMENT_64)
		{
			seg = (struct segment_command_64 *)lc;
			sec = (struct section_64 *)((void *)seg +
				sizeof(struct segment_command_64));
			if (get_symtab_sec_64(symtab, seg, sec, &k))
				return (EXIT_FAILURE);
		}
		lc = (void *)lc + lc->cmdsize;
		if (check(lc))
			return (set_type_error("3 File truncated or someway invalid."));
	}
	return (EXIT_SUCCESS);
}

int								print_output_64(struct symtab_command *sym,
								t_sym *symtab, char *ptr)
{
	char						*stringtable;
	struct nlist_64				*array;

	stringtable = (void *)ptr + sym->stroff;
	array = (void *)ptr + sym->symoff;
	if (check(stringtable) || check(array))
		return (set_type_error("4 File truncated or someway invalid."));
	if (sort_64(stringtable, array, sym->nsyms))
		return (EXIT_FAILURE);
	if (sort_value_64(stringtable, array, sym->nsyms))
		return (EXIT_FAILURE);
	if (display_64(sym, stringtable, array, symtab))
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

int								handle_64(char *ptr)
{
	uint32_t					i;
	struct mach_header_64		*header;
	struct load_command			*lc;
	struct symtab_command		*sym;
	t_sym						symtab;

	i = -1;
	header = (struct mach_header_64 *)ptr;
	lc = (void *)ptr + sizeof(*header);
	if (get_symtab_64(&symtab, header, lc))
		return (error_display("5 File truncated or someway invalid."));
	while (++i < header->ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			if (print_output_64(sym, &symtab, ptr))
				return (error_display("5a File truncated or someway invalid."));
			break ;
		}
		lc = (void *)lc + lc->cmdsize;
		if (check(lc))
			return (error_display("5b File truncated or someway invalid."));
	}
	return (EXIT_SUCCESS);
}
