/**
  *   @file ssh.h
  *   @author Lauri Westerholm
  *   @brief Interface to libssh API, header
  */

#ifndef SSH_HEADER
#define SSH_HEADER

#define LIBSSH_STATIC 1
#include <libssh/libssh.h>
#include <libssh/sftp.h>
#include <gmodule.h> // Linked list implementation, GSList

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "str_messages.h"
#include "fs.h"

#define MAX_BUF_SIZE 16384 /**< Used for sftp_session_read_file */

/**
  *   @struct Session
  *   @brief Store ssh session
  */
typedef struct {
  ssh_session session; /**< libssh session */
  sftp_session sftp;  /**< sftp session */
  int sessionID; /**< libssh session rc */
  char *message; /**< Used to hold info about authentication etc. */
  unsigned char *hash; /**< Remote server public key hash */
  size_t hash_len; /**< Length of the hash */
  char *home_dir; /**< Home dir for on the remote server */
} Session;


/* SSH session */

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
  AUTHENTICATION_OK, /**< Authentication completed */
  AUTHENTICATION_CANCEL, /**< User cancelled authentication */
  AUTHENTICATION_ERROR, /**< Error in authentication */
  AUTHENTICATION_ASK, /**< Ask user whether to trust the remote */
  AUTHENTICATION_ACCEPT, /**< User accepts remote public key */
  AUTHENTICATION_DECLINE, /**< User declines remote public key */
  AUTHENTICATION_PASSWORD_NEEDED /**< User needs to authenticate using password */
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
  *   and call authenticate_key. When AUTHENTICATION_PASSWORD_NEEDED is returned,
  *   call authenticate_password
  */
enum AuthenticationAction authenticate_init(Session *session);

/**
  *   @brief Complete key-based authentication if not finished in authenticate_init
  *   @param session Created session
  *   @param action User promted authetication action
  *   @return AUTHENTICATION_OK, AUTHENTICATION_ERROR, AUTHENTICATION_PASSWORD_NEEDED
  *   or AUTHENTICATION_CANCEL based on the user action and authentication procedure.
  *   When AUTHENTICATION_PASSWORD_NEEDED is returned, call authenticate_password
  */
enum AuthenticationAction authenticate_key(Session *session, const enum AuthenticationAction action);

/**
  *   @brief Authenticate using password
  *   @param session Created Session
  *   @param password Matching the username
  *   @return AUTHENTICATION_OK or AUTHENTICATION_ERROR
  */
enum AuthenticationAction authenticate_password(Session *session, const char *password);


/*  Executing remote commands */

/**
  *   @brief Get remote home directory
  *   @param Already established ssh session
  *   @return 0 on success, -1 on error (sets corresponding error message)
  *   @remark This should be called only once per session
  */
int get_remote_home_dir(Session *session);


/* SFTP session */

/**
  *   @brief Init sftp session after ssh session has been created
  *   @param session Already established ssh session
  *   @remark Make sure that session->session is valid prior calling this
  *   @return 0 on success, -1 on error
  */
int init_sftp_session(Session *session);

/**
  *   @brief Create new directory using sftp
  *   @param session Session which contains already established sftp connection
  *   @param dir_name Directory name
  *   @return 0 on success, < 0 on error (sets corresponding error message, @see Session_message)
  *   @remark This is only a wrapper for libssh sftp_mkdir
  */
enum FileStatus sftp_session_mkdir(Session *session, const char *dir_name);

/**
  *   @brief List remote files using sftp
  *   @param session Session which contains already established sftp connection
  *   @param files Linked list where the directory content is appended to,
  *   old content is removed (needs to be dynamically allocated)
  *   @param dir_name Path of the directory to be listed
  *   @return Valid pointer on success, NULL on error (sets corresponding error message, @see Session_message)
  *   @remark This won't list . and .. because those are not subfolders or files
  */
GSList *sftp_session_ls_dir(Session *session, GSList *files, const char *dir_name);

/**
  *   @brief Write to remote file using sftp
  *   @param session Session which contains already established sftp connection
  *   @param filename Target filename on the remote (filepath)
  *   @param buff Buffer which contains the file content
  *   @param len buff length in bytes
  *   @param overwrite Whether to overwrite if the file already exists
  *   @return FileStatus (sets corresponding error message, @see Session_message)
  */
enum FileStatus sftp_session_write_file(  Session *session,
                                          const char *filename,
                                          const void *buff,
                                          const size_t len,
                                          const bool overwrite);

/**
  *   @brief Read (copy) file from remote to the host machine
  *   @param session Session which contains already established sftp connection
  *   @param remote_filename To be read (path in the remote filesystem)
  *   @param local_filename Path where the remote file is written to
  *   @param overwrite Whether to overwrite possibly already existing local file
  *   @return FileStatus (sets corresponding error message, @see Session_message)
  *   @remark This implements blocking read/write, call this from another thread
  *   than the main thread
  */
enum FileStatus sftp_session_read_file( Session *session,
                                        const char *remote_filename,
                                        const char *local_filename,
                                        const bool overwrite);

#endif // end SSH_HEADER
