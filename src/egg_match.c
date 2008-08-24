/*
 * match.c
 *   wildcard matching functions
 *
 * $Id: match.c,v 1.11 2004-04-06 06:56:38 wcc Exp $
 *
 * Once this code was working, I added support for % so that I could
 * use the same code both in Eggdrop and in my IrcII client.
 * Pleased with this, I added the option of a fourth wildcard, ~,
 * which matches varying amounts of whitespace (at LEAST one space,
 * though, for sanity reasons).
 *
 * This code would not have been possible without the prior work and
 * suggestions of various sourced.  Special thanks to Robey for
 * all his time/help tracking down bugs and his ever-helpful advice.
 *
 * 04/09:  Fixed the "*\*" against "*a" bug (caused an endless loop)
 *
 *   Chris Fuller  (aka Fred1@IRC & Fwitz@IRC)
 *     crf@cfox.bchs.uh.edu
 *
 * I hereby release this code into the public domain
 *
 */
#include "main.h"
#include "egg_match.h"

int _wild_match_per(register unsigned char *m, register unsigned char *n)
{
  unsigned char *ma = m, *lsm = 0, *lsn = 0, *lpm = 0, *lpn = 0;
  int match = 1, saved = 0, space;
  register unsigned int sofar = 0;

  /* null strings should never match */
  if ((m == 0) || (n == 0) || (!*n))
    return NOMATCH;

  while (*n) {
    if (*m == WILDT) {          /* Match >=1 space */
      space = 0;                /* Don't need any spaces */
      do {
        m++;
        space++;
      }                         /* Tally 1 more space ... */
      while ((*m == WILDT) || (*m == ' '));     /*  for each space or ~ */
      sofar += space;           /* Each counts as exact */
      while (*n == ' ') {
        n++;
        space--;
      }                         /* Do we have enough? */
      if (space <= 0)
        continue;               /* Had enough spaces! */
    }
    /* Do the fallback       */
    else {
      switch (*m) {
      case 0:
        do
          m--;                  /* Search backwards */
        while ((m > ma) && (*m == '?'));        /* For first non-? char */
        if ((m > ma) ? ((*m == '*') && (m[-1] != QUOTE)) : (*m == '*'))
          return PERMATCH;      /* nonquoted * = match */
        break;
      case WILDP:
        while (*(++m) == WILDP);        /* Zap redundant %s */
        if (*m != WILDS) {      /* Don't both if next=* */
          if (*n != ' ') {      /* WILDS can't match ' ' */
            lpm = m;
            lpn = n;            /* Save '%' fallback spot */
            saved += sofar;
            sofar = 0;          /* And save tally count */
          }
          continue;             /* Done with '%' */
        }
        /* FALL THROUGH */
      case WILDS:
        do
          m++;                  /* Zap redundant wilds */
        while ((*m == WILDS) || (*m == WILDP));
        lsm = m;
        lsn = n;
        lpm = 0;                /* Save '*' fallback spot */
        match += (saved + sofar);       /* Save tally count */
        saved = sofar = 0;
        continue;               /* Done with '*' */
      case WILDQ:
        m++;
        n++;
        continue;               /* Match one char */
      case QUOTE:
        m++;                    /* Handle quoting */
      }
      if (toupper(*m) == toupper(*n)) { /* If matching */
        m++;
        n++;
        sofar++;
        continue;               /* Tally the match */
      }
#ifdef WILDT
    }
#endif
    if (lpm) {                  /* Try to fallback on '%' */
      n = ++lpn;
      m = lpm;
      sofar = 0;                /* Restore position */
      if ((*n | 32) == 32)
        lpm = 0;                /* Can't match 0 or ' ' */
      continue;                 /* Next char, please */
    }
    if (lsm) {                  /* Try to fallback on '*' */
      n = ++lsn;
      m = lsm;                  /* Restore position */
      saved = sofar = 0;
      continue;                 /* Next char, please */
    }
    return NOMATCH;             /* No fallbacks=No match */
  }
  while ((*m == WILDS) || (*m == WILDP))
    m++;                        /* Zap leftover %s & *s */
  return (*m) ? NOMATCH : PERMATCH;     /* End of both = match */
}

int _wild_match(register unsigned char *m, register unsigned char *n)
{
  unsigned char *ma = m, *na = n, *lsm = 0, *lsn = 0;
  int match = 1;
  register int sofar = 0;

  /* null strings should never match */
  if ((ma == 0) || (na == 0) || (!*ma) || (!*na))
    return NOMATCH;
  /* find the end of each string */
  while (*(++m));
  m--;
  while (*(++n));
  n--;

  while (n >= na) {
    /* If the mask runs out of chars before the string, fall back on
     * a wildcard or fail. */
    if (m < ma) {
      if (lsm) {
        n = --lsn;
        m = lsm;
        if (n < na)
          lsm = 0;
        sofar = 0;
      }
      else
        return NOMATCH;
    }

    switch (*m) {
    case WILDS:                /* Matches anything */
      do
        m--;                    /* Zap redundant wilds */
      while ((m >= ma) && (*m == WILDS));
      lsm = m;
      lsn = n;
      match += sofar;
      sofar = 0;                /* Update fallback pos */
      if (m < ma)
        return MATCH;
      continue;                 /* Next char, please */
    case WILDQ:
      m--;
      n--;
      continue;                 /* '?' always matches */
    }
    if (toupper(*m) == toupper(*n)) {   /* If matching char */
      m--;
      n--;
      sofar++;                  /* Tally the match */
      continue;                 /* Next char, please */
    }
    if (lsm) {                  /* To to fallback on '*' */
      n = --lsn;
      m = lsm;
      if (n < na)
        lsm = 0;                /* Rewind to saved pos */
      sofar = 0;
      continue;                 /* Next char, please */
    }
    return NOMATCH;             /* No fallback=No match */
  }
  while ((m >= ma) && (*m == WILDS))
    m--;                        /* Zap leftover %s & *s */
  return (m >= ma) ? NOMATCH : MATCH;   /* Start of both = match */
}
