/*!
\file acronym.c
*/

/*
This file is part of Algol68G - an Algol 68 interpreter.
Copyright (C) 2001-2006 J. Marcel van der Veer <algol68g@xs4all.nl>.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                      
*/

/* This code was contributed by Theo Vosse.  */

#include "algol68g.h"
#include "genie.h"
#include <ctype.h>

static BOOL_T is_vowel (char);
static BOOL_T is_consonant (char);
static int cmp (const void *, const void *);
static BOOL_T is_coda (char *, int);
static void get_init_sylls (char *, char *);
static void reduce_vowels (char *);
static int error_length (char *);
static BOOL_T remove_extra_coda (char *);
static void make_acronym (char *, char *);

/*!
\brief
\param
\return
**/

static BOOL_T is_vowel (char ch)
{
  return (strchr ("aeiouAEIOU", ch) != NULL);
}

/*!
\brief
\param
\return
**/

static BOOL_T is_consonant (char ch)
{
  return (strchr ("qwrtypsdfghjklzxcvbnmQWRTYPSDFGHJKLZXCVBNM", ch) != NULL);
}

static char *codas[] = {
  "BT", "CH", "CHS", "CHT", "CHTS", "CT", "CTS", "D", "DS", "DST",
  "DT", "F", "FD", "FDS", "FDST", "FDT", "FS", "FST", "FT", "FTS", "FTST", "G", "GD",
  "GDS", "GDST", "GDT", "GS", "GST", "GT", "H", "K", "KS", "KST", "KT",
  "KTS", "KTST", "L", "LD", "LDS", "LDST", "LDT", "LF", "LFD", "LFS", "LFT",
  "LG", "LGD", "LGT", "LK", "LKS", "LKT", "LM", "LMD", "LMS", "LMT", "LP", "LPS",
  "LPT", "LS", "LSD", "LST", "LT", "LTS",
  "LTST", "M", "MBT", "MBTS", "MD", "MDS", "MDST", "MDT", "MF",
  "MP", "MPT", "MPTS", "MPTST", "MS", "MST", "MT", "N",
  "ND", "NDR", "NDS", "NDST", "NDT", "NG", "NGD", "NGS",
  "NGST", "NGT", "NK", "NKS", "NKST", "NKT", "NS", "NSD", "NST",
  "NT", "NTS", "NTST", "NTZ", "NX", "P", "PS", "PST", "PT", "PTS", "PTST",
  "R", "RCH", "RCHT", "RD", "RDS", "RDST", "RDT",
  "RG", "RGD", "RGS", "RGT", "RK", "RKS", "RKT",
  "RLS", "RM", "RMD", "RMS", "RMT", "RN", "RND", "RNS", "RNST", "RNT",
  "RP", "RPS", "RPT", "RS", "RSD", "RST", "RT", "RTS",
  "S", "SC", "SCH", "SCHT", "SCS", "SD", "SK",
  "SKS", "SKST", "SKT", "SP", "SPT", "ST", "STS", "T", "TS", "TST", "W",
  "WD", "WDS", "WDST", "WS", "WST", "WT",
  "X", "XT"
};

/*!
\brief
\param
\return
**/

static int cmp (const void *key, const void *data)
{
  return (strcmp ((char *) key, *(char **) data));
}

/*!
\brief
\param
\return
**/

static BOOL_T is_coda (char *str, int len)
{
  char str2[8];
  strncpy (str2, str, len);
  str2[len] = '\0';
  return (bsearch (str2, codas, sizeof (codas) / sizeof (char *), sizeof (char *), cmp) != NULL);
}

/*!
\brief
\param
**/

static void get_init_sylls (char *in, char *out)
{
  char *coda;
  while (*in != '\0') {
    if (isalpha (*in)) {
      while (*in != '\0' && isalpha (*in) && !is_vowel (*in))
	*out++ = toupper (*in++);
      while (*in != '\0' && is_vowel (*in))
	*out++ = toupper (*in++);
      coda = out;
      while (*in != '\0' && is_consonant (*in)) {
	*out++ = toupper (*in++);
	*out = '\0';
	if (!is_coda (coda, out - coda)) {
	  out--;
	  break;
	}
      }
      while (*in != '\0' && isalpha (*in))
	in++;
      *out++ = '+';
    } else {
      in++;
    }
  }
  out[-1] = '\0';
}

/*!
\brief
\param
**/

static void reduce_vowels (char *str)
{
  char *next;
  while (*str != '\0') {
    next = strchr (str + 1, '+');
    if (next == NULL) {
      break;
    }
    if (!is_vowel (*str) && is_vowel (next[1])) {
      while (str != next && !is_vowel (*str))
	str++;
      if (str != next) {
	memmove (str, next, strlen (next) + 1);
      }
    } else {
      while (*str != '\0' && *str != '+')
	str++;
    }
    if (*str == '+')
      str++;
  }
}

/*!
\brief
\param
**/

static void remove_boundaries (char *str, int maxLen)
{
  int len = 0;
  while (*str != '\0') {
    if (len >= maxLen) {
      *str = '\0';
      return;
    }
    if (*str == '+') {
      memmove (str, str + 1, strlen (str + 1) + 1);
    } else {
      str++;
      len++;
    }
  }
}

/*!
\brief
\param
\return
**/

static int error_length (char *str)
{
  int len = 0;
  while (*str != '\0') {
    if (*str != '+')
      len++;
    str++;
  }
  return (len);
}

/*!
\brief
\param
\return
**/

static BOOL_T remove_extra_coda (char *str)
{
  int len;
  while (*str != '\0') {
    if (is_vowel (*str) && str[1] != '+' && !is_vowel (str[1]) && str[2] != '+' && str[2] != '\0') {
      for (len = 2; str[len] != '\0' && str[len] != '+'; len++);
      memmove (str + 1, str + len, strlen (str + len) + 1);
      return (A_TRUE);
    }
    str++;
  }
  return (A_FALSE);
}

/*!
\brief
\param
\param
**/

static void make_acronym (char *in, char *out)
{
  get_init_sylls (in, out);
  reduce_vowels (out);
  while (error_length (out) > 8 && remove_extra_coda (out));
  remove_boundaries (out, 8);
}

/*!
\brief
\param
**/

void genie_acronym (NODE_T * p)
{
  A68_REF z;
  int len;
  char *u, *v;
  POP_REF (p, &z);
  len = a68_string_size (p, z);
  u = malloc (len + 1);
  v = malloc (len + 1 + 8);
  a_to_c_string (p, u, z);
  if (u != NULL && u[0] != '\0' && v != NULL) {
    make_acronym (u, v);
    PUSH_REF (p, c_to_a_string (p, v));
  } else {
    PUSH_REF (p, empty_string (p));
  }
  if (u != NULL) {
    free (u);
  }
  if (v != NULL) {
    free (v);
  }
}
