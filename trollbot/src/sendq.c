/******************************
 * Trollbot                   *
 ******************************
 * Written by poutine DALnet  *
 ******************************
 * This software is public    *
 * domain. Free for any use   *
 * whatsoever.                *
 ******************************/

#include <unistd.h>
#include <signal.h>

/* The send queue will be only for output aimed at the IRCD. It will utilize 
 * a signal based timer which reads/modifies a global variable which 
 * represents total "messages" sent in timer frame (decided by user conf ratio
 * for instance 5:10 would be 5 messages in 10 seconds)
 */
