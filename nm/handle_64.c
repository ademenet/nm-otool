/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_64.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ademenet <ademenet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/21 15:50:23 by ademenet          #+#    #+#             */
/*   Updated: 2018/01/09 17:03:27 by ademenet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./inc/ft_nm.h"

static void						get_symtab_sec_64(t_sym *symtab,
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
		(*k)++;
	}
	return ;
}

static void						get_symtab_64(t_sym *symtab,
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
			get_symtab_sec_64(symtab, seg, sec, &k);
		}
		lc = (void *)lc + lc->cmdsize;
	}
	return ;
}

int								print_output_64(struct symtab_command *sym,
								t_sym *symtab, char *ptr)
{
	char						*stringtable;
	struct nlist_64				*array;

	stringtable = (void *)ptr + sym->stroff;
	array = (void *)ptr + sym->symoff;
	if (check(stringtable) || check(array))
		return (error_display("File truncated or someway invalid."));
	array = sort_64(stringtable, array, sym->nsyms);
	sort_value_64(stringtable, array, sym->nsyms);
	return (display_64(sym, stringtable, array, symtab));
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
	get_symtab_64(&symtab, header, lc);
	while (++i < header->ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			print_output_64(sym, &symtab, ptr);
			break ;
		}
		lc = (void *)lc + lc->cmdsize;
		if (check(lc))
			return (error_display("Invalid file."));
	}
	return (0);
}
