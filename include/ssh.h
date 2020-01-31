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
#include "assets.h"

#define MAX_BUF_SIZE 16384 /**< Used for sftp_session_read_file */
#define WRITE_CHUNK_SIZE 50000 /**< Used for sftp_session_write_file */

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
  *   @brief Execute remote command
  *   @param session Session struct which contains already established ssh session
  *   @param cmd Command to be executed
  *   @param res Buffer where result of the comand is updated (this needs to be a valid pointer)
  *   @param res_len Length of the res buffer, the memory need to be pre-allocated
  */
int execute_remote_command( Session *session,
                            const char *cmd,
                            char **res,
                            const unsigned res_len);

/**
  *   @brief Get remote home directory
  *   @param session Already established ssh session
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
  *   @brief Check whether a filename leads to a folder
  *   @param session Session struct with already established sftp session
  *   @param filename Name of the file in the pwd
  *   @param pwd Present working directory
  *   @return true if filename is a folder, otherwise false
  */
bool sftp_session_is_filename_folder(Session *session, const char *filename, const char *pwd);

/**
  *   @brief Create new directory using sftp
  *   @param session Session which contains already established sftp connection
  *   @param dir_name Directory name
  *   @return 0 on success, on error either DIR_ALREADY_EXISTS or MKDIR_FAILED
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
                                          const char *buff,
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

/**
  *   @brief Rename file on remote using sftp
  *   @param session Session struct which contains already established sftp session
  *   @param path Current file path on the remote
  *   @param new_path New path for the file
  *   @return FILE_WRITTEN_SUCCESSFULLY or FILE_WRITE_FAILED
  *   @remark Only a wrapper for sftp_rename
  */
enum FileStatus sftp_session_rename_file(Session *session, const char *path, const char *new_path);

/**
  *   @brief Remove directory (and its content)
  *   @param session Session struct which contains already established sftp session
  *   @param dir_name Name of the directory to be removed
  *   @param recursive Whether to remove an empty directory or all the contents
  *   @return 0 on success, < 0 on error
  */
enum FileStatus sftp_session_rmdir(Session *session, const char *dir_name, bool recursive);

/**
  *   @brief Remove filepath completely (works for both files and directories)
  *   @details For directories this will call sftp_session_rmdir to recursively
  *   erase the whole directory. For files this will call sftp_unlink
  *   @param session Session struct which contains already established sftp session
  *   @param filepath To be removed
  *   @return 0 on success, < 0 on error (sets corresponding error message, @see Session_message)
  */
enum FileStatus sftp_session_remove_completely_file(Session *session, const char *filepath);

/**
  *   @brief Paste file from local filesystem to remote
  *   @param session Session struct which contains already established sftp session
  *   @param local_filepath Path to the file to be copied on the local filesystem
  *   @param remote_dir Target directory in the remote filesystem (parent directory)
  *   @param filename File or directory which is copied
  *   @param overwrite Whether to overwrite possible already existing remote files
  *   @remark This is a recursive function. This should be run in another thread.
  *   This will gracefully stop when global stop == 0, returns with STOP_FILE_OPERATIONS
  *   (stop is defined in @see assets.h)
  *   @return 0 on success, otherwise return < 0 and matching FileStatus
  */
enum FileStatus sftp_session_copy_to_remote(  Session *session,
                                              const char *local_filepath,
                                              const char *remote_dir,
                                              const char *filename,
                                              const bool overwrite);

/**
  *   @brief Copy file from remote to local filesystem
  *   @param session Session struct which contains already established sftp session
  *   @param local_dir Target directory (parent directory) on local filesystem
  *   @param remote_filepath Filepath on the remote filesystem (the actual file, not parent dir)
  *   @param filename File or directory which is copied
  *   @param overwrite Whether to overwrite possible already existing local files
  *   @remark This is a recursive function. This will return with STOP_FILE_OPERATIONS
  *   when global stop == 1 (stop is defined in @see assets.h)
  *   @return 0 on success, otherwise return < 0 and matching FileStatus
  */
enum FileStatus sftp_session_copy_from_remote(  Session *session,
                                                const char *local_dir,
                                                const char *remote_filepath,
                                                const char *filename,
                                                const bool overwrite);

/**
  *   @brief Copy files on remote filesystem
  *   @param session Session struct which contains already established sftp session
  *   @param src_filepath Path from which the files are copied
  *   @param dst_dir Target directory for the copy operation (parent directory)
  *   @param filename Name of file or directory to be copied
  *   @param overwrite Whether to overwrite possible already existing remote files
  *   @return 0 on success, otherwise return < 0 and matching FileStatus
  */
enum FileStatus sftp_session_copy_on_remote(    Session *session,
                                                const char *src_filepath,
                                                const char *dst_dir,
                                                const char *filename,
                                                const bool overwrite);
#endif // end SSH_HEADER
