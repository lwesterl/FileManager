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
#include <string.h>
#include <errno.h>

#include "str_messages.h"

/**
  *   @struct Session
  *   @brief Store ssh session
  */
typedef struct {
  ssh_session session; /** libssh session */
  int sessionID; /** libssh session rc */
  char *message; /** Used to hold info about authetication etc. */
  unsigned char *hash; /** Remote server public key hash */
  size_t hash_len;
} Session;

/**
  *   @brief Set message for the Sesssion
  *   @param session Session struct
  *   @param message New message, possible old message erased
  */
void Session_message(Session *session, const char *message);


/**
  *   @enum AuthenticationAction
  *   @brief Describes authentication action needed
  */
enum AuthenticationAction {
  AUTHENTICATION_OK, /** Authentication completed */
  AUTHENTICATION_CANCEL, /** User cancelled authentication */
  AUTHENTICATION_ERROR, /** Error in authentication */
  AUTHENTICATION_ASK, /** Ask user whether to trust the remote */
  AUTHENTICATION_ACCEPT, /** User accepts remote public key */
  AUTHENTICATION_DECLINE /** User declines remote public key */
};


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

/**
  *   @brief Start authetication using knownhosts
  *   @param session Session struct, created session
  *   @return AuthenticationAction needed: AUTHENTICATION_ASK -> ask user authentication
  *   and call authenticate_key
  */
enum AuthenticationAction authenticate_init(Session *session);

/**
  *   @brief Complete key-based authentication if not finished in authenticate_init
  *   @param session Created session
  *   @param action User promted authetication action
  *   @return AUTHENTICATION_OK, AUTHENTICATION_ERROR or AUTHENTICATION_CANCEL based on
  *   the user action and authentication procedure
  */
enum AuthenticationAction authenticate_key(Session *session, const enum AuthenticationAction action);

/**
  *   @brief Authenticate using password
  *   @param session Created Session
  *   @param password Matching the username
  */
enum AuthenticationAction authenticate_password(Session *session, const char *password);

#endif // end SSH_HEADER
