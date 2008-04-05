/* $NetBSD: elf_machdep.h,v 1.10 2001/12/09 23:05:56 thorpej Exp $ */

#ifndef	_ALPHA_ELF_MACHDEP_H_
#define	_ALPHA_ELF_MACHDEP_H_

/*
 * Alpha ELF uses different (non-standard) definitions of Elf64_Sword
 * and Elf64_Word.
 */
typedef	int64_t		Elf64_Sword;
#define	ELF64_FSZ_SWORD	8
typedef	uint64_t	Elf64_Word;
#define	ELF64_FSZ_WORD	8

#define	ELF32_MACHDEP_ENDIANNESS	XXX	/* break compilation */
#define	ELF32_MACHDEP_ID_CASES						\
		/* no 32-bit ELF machine types supported */

#define	ELF64_MACHDEP_ENDIANNESS	ELFDATA2LSB
#define	ELF64_MACHDEP_ID_CASES						\
		case EM_ALPHA:						\
		case EM_ALPHA_EXP:					\
			break;

#define	ELF64_MACHDEP_ID	EM_ALPHA_EXP	/* XXX */

#define ARCH_ELFSIZE		64	/* MD native binary size */

/*
 * Alpha Relocation Types
 */
#define	R_ALPHA_NONE		0	/* No reloc */
#define	R_ALPHA_REFLONG		1	/* Direct 32 bit */
#define	R_ALPHA_REFQUAD		2	/* Direct 64 bit */
#define	R_ALPHA_GPREL32		3	/* GP relative 32 bit */
#define	R_ALPHA_LITERAL		4	/* GP relative 16 bit w/optimization */
#define	R_ALPHA_LITUSE		5	/* Optimization hint for LITERAL */
#define	R_ALPHA_GPDISP		6	/* Add displacement to GP */
#define	R_ALPHA_BRADDR		7	/* PC+4 relative 23 bit shifted */
#define	R_ALPHA_HINT		8	/* PC+4 relative 16 bit shifted */
#define	R_ALPHA_SREL16		9	/* PC relative 16 bit */
#define	R_ALPHA_SREL32		10	/* PC relative 32 bit */
#define	R_ALPHA_SREL64		11	/* PC relative 64 bit */
#define	R_ALPHA_OP_PUSH		12	/* OP stack push */
#define	R_ALPHA_OP_STORE	13	/* OP stack pop and store */
#define	R_ALPHA_OP_PSUB		14	/* OP stack subtract */
#define	R_ALPHA_OP_PRSHIFT	15	/* OP stack right shift */
#define	R_ALPHA_GPVALUE		16
#define	R_ALPHA_GPRELHIGH	17
#define	R_ALPHA_GPRELLOW	18
#define	R_ALPHA_IMMED_GP_16	19
#define	R_ALPHA_IMMED_GP_HI32	20
#define	R_ALPHA_IMMED_SCN_HI32	21
#define	R_ALPHA_IMMED_BR_HI32	22
#define	R_ALPHA_IMMED_LO32	23
#define	R_ALPHA_COPY		24	/* Copy symbol at runtime */
#define	R_ALPHA_GLOB_DAT	25	/* Create GOT entry */
#define	R_ALPHA_JMP_SLOT	26	/* Create PLT entry */
#define	R_ALPHA_RELATIVE	27	/* Adjust by program base */

#define	R_TYPE(name)		__CONCAT(R_ALPHA_,name)

#endif /* _ALPHA_ELF_MACHDEP_H_ */
