/**
  *   @file ssh.h
  *   @author Lauri Westerholm
  *   @brief Interface to libssh API, header
  */

#ifndef SSH_HEADER
#define SSH_HEADER

#define LIBSSH_STATIC 1
#include <libssh/libssh.h>

#include <stdlib.h>
#include <stdio.h>

#include "str_messages.h"

/**
  *   @struct Session
  *   @brief Store ssh session
  */
typedef struct {
  ssh_session session;
  int sessionID;
} Session;

/**
  *   @brief Create a new ssh session
  *   @param username Username for authentication
  *   @param remote Address for the remote server
  *   @return The remote session as pointer or NULL on error
  */
Session *create_session(const char *username, const char *remote);

/**
  *   @brief End ssh session
  *   @param session pointer to a Session struct
  *   @return 0 on success, -1 on error
  *   @remark On success the pointer will be deallocated
  */
int end_session(Session *session);



#endif // end SSH_HEADER
