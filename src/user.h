#ifndef __USER_H__
#define __USER_H__

enum user_flags
{
  USER_UNKNOWN =  0,
  USER_KNOWN   =  1,
  USER_AUTH    =  2,
  USER_OWNER   =  4
};
  
struct user
{
  char *nick;
  char *passhash;

  char *ident;
  char *realname;
  char *host;

  unsigned int flags;
};

#endif __CHANNEL_H__

