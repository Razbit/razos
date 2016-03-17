/* Copyright (c) 2016 Eetu "Razbit" Pesonen
 *
 * This file is part of RazOS.
 *
 * RazOS is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * RazOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with RazOS. If not, see <http://www.gnu.org/licenses/>. */

#ifndef STDDEF_H
#define STDDEF_H

#define _NEED_NULL
#define _NEED_PTRDIFF_T
#define _NEED_SIZE_T
#define _NEED_WCHAR_T

#include <sys/types.h>

#define offsetof(st, m) __builtin_offsetof(st, m)

#endif /* STDDEF_H */
