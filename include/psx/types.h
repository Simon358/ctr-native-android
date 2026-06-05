/*
 * Derived from REDRIVER2/PsyCross MIT source:
 * externals/PsyCross/include/psx/types.h
 * See THIRD_PARTY_NOTICES.md for copyright and license details.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>
#if defined(CTR_NATIVE)
#include <sys/types.h>
#endif

#if !defined(__APPLE__)
/* major part of a device */
#define	major(x)	((int)(((unsigned)(x)>>8)&0377))

/* minor part of a device */
#define	minor(x)	((int)((x)&0377))

/* make a device number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))

#endif

#ifndef _UCHAR_T
#define _UCHAR_T
typedef	unsigned char	u_char;
#endif
#ifndef _USHORT_T
#define _USHORT_T
typedef	unsigned short	u_short;
#endif
#ifndef _UINT_T
#define _UINT_T
typedef	unsigned int	u_int;
#endif
#ifndef _ULONG_T
#define _ULONG_T
// NOTE(aalhendi): PsyQ `u_long` is a PS1 32-bit word, not a host-sized
// C `unsigned long`. Keep GPU/OT/CD ABI-shaped data fixed-width for native.
#if defined(CTR_NATIVE)
#define u_long uint32_t
#else
typedef unsigned long u_long;
#endif
#endif
typedef char psx_u_long_must_be_32_bits[(sizeof(u_long) == 4) ? 1 : -1];
#ifndef _SYSIII_USHORT
#define _SYSIII_USHORT
typedef	unsigned short	ushort;		/* sys III compat */
#endif
#ifndef __psx__
#ifndef _SYSV_UINT
#define _SYSV_UINT
typedef	unsigned int	uint;		/* sys V compat */
#endif
#ifndef _SYSV_ULONG
#define _SYSV_ULONG
typedef	unsigned long	ulong;		/* sys V compat */
#endif
#endif /* ! __psx__ */

#define	NBBY	8

#endif
