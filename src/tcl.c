#ifdef HAVE_TCL

#include <stdio.h>
#include <tcl.h>

#include "main.h"
#include "irc.h"
#include "triggers.h"
#include "main.h"
#include "scripts.h"

static int tcl_eval(struct irc_data *data, const char *line)
{
  char *serialize = NULL,
       *tmpbuf    = NULL;
  int size = 0;
  struct params *tmp;

  if (strcmp(data->prefix->nick,"poutine"))
    return 1;

  serialize = tmalloc0(2048);

  tmp = data->rest_head->next;

  while (tmp != NULL)
  {
    tmpbuf = tmalloc0(strlen(tmp->param) + 2);

    sprintf(tmpbuf,"%s ",tmp->param);
    strcat(serialize,tmpbuf);
    free(tmpbuf);

    tmp = tmp->next;
  }

  Tcl_Eval(tcl_interpreter,serialize);  

  free(serialize);

  return 1;
}

/* Startup the TCL global interpreter */
int tcl_startup(void)
{
  struct scripts *tmp = tcl_scripts_head;

  /* Create the interpreter */
  tcl_interpreter = Tcl_CreateInterp();

  /* Add Commands see tcl_lib.c */
  Tcl_CreateObjCommand(tcl_interpreter,
                       "putserv",
                       tcl_putserv,
                       "putserv",
                       NULL);

  Tcl_CreateObjCommand(tcl_interpreter,
                       "bind",
                       tcl_bind,
                       "bind",
                       NULL);

  add_handler("tcl_eval",
              NULL,
              tcl_eval,
              NULL,
              1);

  add_trigger(PUB,
              "@tcl",
              "tcl_eval",
              NULL,
              "foo");
             

  /* Load the currently installed scripts */
  while (tmp != NULL)
  {
    if (Tcl_EvalFile(tcl_interpreter,tmp->filename) == TCL_OK)
      tmp->status = 1;
    else 
      tmp->status = 0;
   
    tmp = tmp->next;
  }
    
  return 1;
}

int tcl_handler(struct irc_data *data, const char *line)
{
/*  char *serialize = NULL,
       *tmpbuf    = NULL;
  int size = 0;
  char *cmd = tmalloc0(sizeof(char) * 2048);
  struct params *tmp;

  if (data->trigger->type == PUB)
  {
    sprintf(cmd,"%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\"",
                                    data->trigger->exec,
                                     data->prefix->nick,
                                                  "foo",
                                                  "foo",
                                      data->c_params[0],
      &data->c_params_str[strlen(data->c_params[0])+1]);
 
    printf("Evaluating (%s)\n",cmd);
    Tcl_Eval(tcl_interpreter,cmd);
    printf("Matched trigger for %s\n",data->rest[0]);
      
  }
*/
  return 0;
}


int tcl_shutdown(void)
{
  Tcl_DeleteInterp(tcl_interpreter);
}

#endif /* HAVE_TCL */
