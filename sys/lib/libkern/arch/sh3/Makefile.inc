#	$NetBSD: Makefile.inc,v 1.16 2007/03/14 22:22:53 uwe Exp $

SRCS+=	__main.c imax.c imin.c lmax.c lmin.c max.c min.c ulmax.c ulmin.c \
	ffs.S scanc.c skpc.c \
	strcat.c strcmp.c strcpy.c strlen.c strcasecmp.c \
	strncasecmp.c strncmp.c strtoul.c \
	strncpy.c random.c __assert.c memchr.c memcmp.c memset.S \
	memmove.S memcpy.S byte_swap_2.S byte_swap_4.S byte_swap_8.S \
	ashiftrt.S ashlsi3.S ashrsi3.S lshrsi3.S movstr.S movstr_i4.S \
	movstrSI.S movstrSI12_i4.S mulsi3.S sdivsi3.S udivsi3.S
