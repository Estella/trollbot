/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "servers.h"
#include "scripts.h"

/*
void add_php_script(const char *script)
{
  struct scripts *tmp = NULL;

  if (php_scripts_head == NULL)
  {
    php_scripts                  = tmalloc(sizeof(struct servers));
    php_scripts->prev            = NULL;
    php_scripts_head             = php_scripts;
    php_scripts_tail             = php_scripts;
    tmp                          = php_scripts;
  } else {
    php_scripts_tail->next       = tmalloc(sizeof(struct servers));
    php_scripts_tail->next->prev = php_scripts_tail;
    php_scripts_tail             = php_scripts_tail->next;
    tmp                          = php_scripts_tail;
  }

  tmp->filename = tstrdup(script);
  tmp->status   = 0;

  tmp->next     = NULL;

  return;
}


void add_tcl_script(const char *script)
{
  struct scripts *tmp = NULL;

  if (tcl_scripts_head == NULL)
  {
    tcl_scripts                  = tmalloc(sizeof(struct servers));
    tcl_scripts->prev            = NULL;
    tcl_scripts_head             = tcl_scripts;
    tcl_scripts_tail             = tcl_scripts;
    tmp                          = tcl_scripts;
  } else {
    tcl_scripts_tail->next       = tmalloc(sizeof(struct servers));
    tcl_scripts_tail->next->prev = tcl_scripts_tail;
    tcl_scripts_tail             = tcl_scripts_tail->next;
    tmp                          = tcl_scripts_tail;
  }

  tmp->filename = tstrdup(script);
  tmp->status   = 0;

  tmp->next     = NULL;

  return;
}

*/
