/**
  *   @file ssh.c
  *   @author Lauri Westerholm
  *   @brief Interface libssh API
  */

#include "../include/ssh.h"
#include <libssh/libssh.h>

// Session struct related
void Session_message(Session *session, const char *message) {
  if (session) {
    if (session->message) {
      free(session->message);
    }
    int len = strlen(message) + 1;
    session->message = malloc(len);
    strncpy(session->message, message, len);
  }
}

// SSH session handling
Session *create_session(const char *username, const char *remote) {
  if (remote) {
    Session *session = malloc(sizeof(Session));
    session->message = NULL;
    session->hash = NULL;
    session->sftp = NULL;
    session->home_dir = NULL;
    session->session = ssh_new();
    if (!session->session) {
      perror(get_error(SSH_CREATE_ERROR));
      free(session);
      return NULL;
    }
    ssh_options_set(session->session, SSH_OPTIONS_USER, username);
    ssh_options_set(session->session, SSH_OPTIONS_HOST, remote);

    session->sessionID = ssh_connect(session->session);
    if (session->sessionID == SSH_OK) return session;
    perror(get_error(SSH_CONNECT_ERROR));
    perror(ssh_get_error(session->session));
    ssh_free(session->session);
    free(session);
  }
  return NULL;
}

int end_session(Session *session) {
  if (session) {
    if (session->sftp) {
      sftp_free(session->sftp);
    }
    ssh_disconnect(session->session);
    ssh_free(session->session);
    if (session->message) {
      free(session->message);
    }
    if (session->home_dir) {
      free(session->home_dir);
    }
    free(session);
    return 0;
  }
  return -1;
}

#if LIBSSH_VERSION_MINOR >= 8 || LIBSSH_VERSION_MAJOR > 0
  // For newer sshlib versions
  enum AuthenticationAction authenticate_init(Session *session) {
    ssh_key pubkey;
    enum ssh_known_hosts_e state;

    if (ssh_get_server_publickey(session->session, &pubkey) < 0) {
      Session_message(session, get_error(ERROR_PUB_KEY_NOT_FOUND));
      return AUTHENTICATION_ERROR;
    }
    int ret = ssh_get_publickey_hash(pubkey,
                               SSH_PUBLICKEY_HASH_SHA1,
                               &(session->hash),
                               &(session->hash_len));
    ssh_key_free(pubkey);
    if (ret < 0) {
      Session_message(session, get_error(ERROR_PUB_KEY_HASH_FAILED));
      return AUTHENTICATION_ERROR;
    }
    state = ssh_session_is_known_server(session->session);
    switch(state) {
      case SSH_KNOWN_HOSTS_OK:
        ssh_clean_pubkey_hash(&(session->hash));
        if (ssh_userauth_publickey_auto(session->session, NULL, NULL) != SSH_AUTH_SUCCESS) {
          return AUTHENTICATION_PASSWORD_NEEDED;
        }
        return AUTHENTICATION_OK;
      case SSH_KNOWN_HOSTS_CHANGED:
        Session_message(session, get_error(ERROR_PUB_KEY_CHANGED));
        ssh_clean_pubkey_hash(&(session->hash));
        return AUTHENTICATION_ERROR;
      case SSH_KNOWN_HOSTS_OTHER:
        Session_message(session, get_error(ERROR_PUB_KEY_NOT_APROPRIATE));
        ssh_clean_pubkey_hash(&(session->hash));
        return AUTHENTICATION_ERROR;
      case SSH_KNOWN_HOSTS_NOT_FOUND:
        Session_message(session, get_detailed_error(KNOWN_HOSTS_NOT_FOUND, session->hash, session->hash_len));
        return AUTHENTICATION_ASK;
      case SSH_KNOWN_HOSTS_UNKNOWN:
        Session_message(session, get_detailed_error(SERVER_UNKNOWN, session->hash, session->hash_len));
        return AUTHENTICATION_ASK;
      case SSH_KNOWN_HOSTS_ERROR:
        Session_message(session, ssh_get_error(session->session));
        return AUTHENTICATION_ERROR;
    }
    return AUTHENTICATION_ERROR;
  }

  enum AuthenticationAction authenticate_key(Session *session, const enum AuthenticationAction action) {
    // clear public key hash
    ssh_clean_pubkey_hash(&(session->hash));

    if (action == AUTHENTICATION_ACCEPT) {
      if (ssh_session_update_known_hosts(session->session) < 0) {
        Session_message(session, strerror(errno));
        return AUTHENTICATION_ERROR;
      }
      if (ssh_userauth_publickey_auto(session->session, NULL, NULL) != SSH_AUTH_SUCCESS) {
        return AUTHENTICATION_PASSWORD_NEEDED;
      }
      return AUTHENTICATION_OK;
    }
    // user has decided to decline the server public key
    return AUTHENTICATION_CANCEL;
  }
#else
  // For old sshlib versions
  enum AuthenticationAction authenticate_init(Session *session) {
    ssh_key pubkey;
    enum ssh_server_known_e state;

    if (ssh_get_publickey(session->session, &pubkey) < 0) {
      Session_message(session, get_error(ERROR_PUB_KEY_NOT_FOUND));
      return AUTHENTICATION_ERROR;
    }
    int ret = ssh_get_publickey_hash(pubkey,
                               SSH_PUBLICKEY_HASH_SHA1,
                               &(session->hash),
                               &(session->hash_len));
    ssh_key_free(pubkey);
    if (ret < 0) {
      Session_message(session, get_error(ERROR_PUB_KEY_HASH_FAILED));
      return AUTHENTICATION_ERROR;
    }
    state = ssh_is_server_known(session->session);
    switch(state) {
      case SSH_SERVER_KNOWN_OK:
        ssh_clean_pubkey_hash(&(session->hash));
        if (ssh_userauth_publickey_auto(session->session, NULL, NULL) != SSH_AUTH_SUCCESS) {
          return AUTHENTICATION_PASSWORD_NEEDED;
        }
        return AUTHENTICATION_OK;
      case SSH_SERVER_KNOWN_CHANGED:
        Session_message(session, get_error(ERROR_PUB_KEY_CHANGED));
        ssh_clean_pubkey_hash(&(session->hash));
        return AUTHENTICATION_ERROR;
      case SSH_SERVER_FOUND_OTHER:
        Session_message(session, get_error(ERROR_PUB_KEY_NOT_APROPRIATE));
        ssh_clean_pubkey_hash(&(session->hash));
        return AUTHENTICATION_ERROR;
      case SSH_SERVER_FILE_NOT_FOUND:
        Session_message(session,  get_detailed_error(KNOWN_HOSTS_NOT_FOUND, session->hash, session->hash_len));
        return AUTHENTICATION_ASK;
      case SSH_SERVER_NOT_KNOWN:
        Session_message(session, get_detailed_error(SERVER_UNKNOWN, session->hash, session->hash_len));
        return AUTHENTICATION_ASK;
      case SSH_SERVER_ERROR:
        Session_message(session, ssh_get_error(session->session));
        return AUTHENTICATION_ERROR;
    }
    return AUTHENTICATION_ERROR;
  }

  enum AuthenticationAction authenticate_key(Session *session, const enum AuthenticationAction action) {
    // clear public key hash
    ssh_clean_pubkey_hash(&(session->hash));

    if (action == AUTHENTICATION_ACCEPT) {
      if (ssh_write_knownhost(session->session) < 0) {
        Session_message(session, strerror(errno));
        return AUTHENTICATION_ERROR;
      }
      if (ssh_userauth_publickey_auto(session->session, NULL, NULL) != SSH_AUTH_SUCCESS) {
        return AUTHENTICATION_PASSWORD_NEEDED;
      }
      return AUTHENTICATION_OK;
    }
    // user has decided to decline the server public key
    return AUTHENTICATION_CANCEL;
  }
#endif // authenticate_init and authenticate_key implementations

enum AuthenticationAction authenticate_password(Session *session, const char *password) {
  if (ssh_userauth_password(session->session, NULL, password) != SSH_AUTH_SUCCESS) {
    Session_message(session, get_error(ERROR_PASSWORD_AUTHENTICATION));
    return AUTHENTICATION_ERROR;
  }
  return AUTHENTICATION_OK;
}


// Executing remote commands

int get_remote_home_dir(Session *session) {
  char buffer[250] = {0};
  int nread, tot = 0;
  ssh_channel channel;
  channel = ssh_channel_new(session->session);
  if (!channel) {
    Session_message(session, get_error(SSH_CHANNEL_ERROR));
    return -1;
  }
  if (ssh_channel_open_session(channel) != SSH_OK) {
    Session_message(session, get_error(SSH_CHANNEL_ERROR));
    ssh_channel_free(channel);
    return -1;
  }
  if (ssh_channel_request_exec(channel, "echo $HOME") != SSH_OK) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    Session_message(session, get_error(SSH_REMOTE_COMMAND_ERROR));
    return -1;
  }
  do {
    nread = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    tot += nread;
  } while (nread > 0);
  if (nread < 0) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    Session_message(session, get_error(SSH_REMOTE_COMMAND_ERROR));
    return -1;
  }
  buffer[tot - 1] = '\0'; // The last char is EOF/EOT, overwrite it with a null byte
  if (session->home_dir) free(session->home_dir);
  session->home_dir = malloc(strlen(buffer) + 1);
  strcpy(session->home_dir, buffer);
  return 0;
}


// SFTP related

int init_sftp_session(Session *session) {
  session->sftp = sftp_new(session->session);
  if (!session->sftp) {
    Session_message(session, get_error(ERROR_SFTP_ALLOCATION));
    return -1;
  }
  if (sftp_init(session->sftp) != SSH_OK) {
    Session_message(session, get_error(ERROR_SFTP_INITIALIZATION));
    sftp_free(session->sftp);
    return -1;
  }
  return 0;
}

enum FileStatus sftp_session_mkdir(Session *session, const char *dir_name) {
  int permissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH;
  if(sftp_mkdir(session->sftp, dir_name, permissions) != SSH_OK) {
    if (sftp_get_error(session->sftp) != SSH_FX_FILE_ALREADY_EXISTS) {
      Session_message(session, ssh_get_error(session->session));
      return FILE_WRITTEN_SUCCESSFULLY;
    }
    return MKDIR_FAILED;
  }
  return FILE_WRITTEN_SUCCESSFULLY;
}

GSList *sftp_session_ls_dir(Session *session, GSList *files, const char *dir_name) {
  sftp_dir dir;
  sftp_attributes attr;

  // Clear old content
  clear_Filelist(files);
  files = NULL;

  dir = sftp_opendir(session->sftp, dir_name);
  if (!dir) {
    Session_message(session, get_error(ERROR_OPENING_DIRECTORY));
    return NULL;
  }
  while ((attr = sftp_readdir(session->sftp, dir)) != NULL) {
    // malloc should not fail
    struct File *file = malloc(sizeof(struct File));
    file->name = malloc(strlen(attr->name) + 1);
    strcpy(file->name, attr->name);
    file->type = attr->type;
    file->size = attr->size;
    file->uid = attr->uid;
    file->gid = attr->gid;
    file->owner = malloc(strlen(attr->owner) + 1);
    strcpy(file->owner, attr->owner);
    file->group = malloc(strlen(attr->group) + 1);
    strcpy(file->group, attr->group);
    file->permissions = attr->permissions;
    file->mtime = attr->mtime;

    files = append_FileList(files, file);
    sftp_attributes_free(attr);

  }

  if (!sftp_dir_eof(dir)) {
    Session_message(session, get_error(ERROR_LISTING_DIRECTORY));
    clear_Filelist(files);
    return NULL;
  }
  sftp_closedir(dir); // No error checking because if this fails there is very little that can be done
  return files;
}

enum FileStatus sftp_session_write_file(Session *session,
                            const char *filename,
                            const void *buff,
                            const size_t len,
                            const bool overwrite)
{
  int write_flags = overwrite ? O_WRONLY | O_CREAT | O_TRUNC : O_WRONLY | O_CREAT;
  int permissions = S_IRWXU;

  sftp_file file = sftp_open(session->sftp, filename, write_flags, permissions);
  if (!file) {
    sftp_file open_test = sftp_open(session->sftp, filename, O_RDONLY, 0);
    if (open_test){
      // File already exists and it is tried to be written without being truncated
      sftp_close(open_test);
      Session_message(session, get_error(ERROR_FILE_ALREADY_EXISTS));
      return FILE_ALREADY_EXISTS;
    }
    Session_message(session, get_error(ERROR_WRITING_TO_FILE));
    return FILE_WRITE_FAILED;
  }
  size_t count = sftp_write(file, buff, len);
  if (count != len) {
    Session_message(session, get_error(ERROR_OPENING_FILE));
    return FILE_WRITE_FAILED;
  }
  sftp_close(file); // No error checking because if this fails there is very little that can be done
  return FILE_WRITTEN_SUCCESSFULLY;
}

enum FileStatus sftp_session_read_file( Session *session,
                                        const char *remote_filename,
                                        const char *local_filename,
                                        const bool overwrite)
{
  char buffer[MAX_BUF_SIZE];
  sftp_file file;
  int nread, nwritten;
  int fd;
  int write_flags = overwrite ? O_CREAT | O_WRONLY | O_TRUNC : O_CREAT | O_WRONLY;
  int permissions = S_IRWXU;

  file = sftp_open(session->sftp, remote_filename, O_RDONLY, 0);
  if (!file) {
    Session_message(session, get_error(ERROR_OPENING_FILE));
    return FILE_WRITE_FAILED;
  }
  fd = open(local_filename, write_flags, permissions);
  if (fd < 0) {
    if (!overwrite && file_exists(local_filename)) {
      Session_message(session, get_error(ERROR_FILE_ALREADY_EXISTS));
      return FILE_ALREADY_EXISTS;
    }
    Session_message(session, get_error(ERROR_OPENING_FILE));
    return FILE_WRITE_FAILED;
  }
  while(1) {
    nread = sftp_read(file, buffer, sizeof(buffer));
    if (nread == 0) break;
    else if (nread < 0) {
      close(fd);
      sftp_close(file);
      Session_message(session, get_error(ERROR_READING_FILE));
      return FILE_READ_FAILED;
    }
    nwritten = write(fd, buffer, nread);
    if (nwritten != nread) {
      close(fd);
      sftp_close(file);
      Session_message(session, get_error(ERROR_WRITING_TO_FILE));
      return FILE_WRITE_FAILED;
    }
  }
  close(fd); // No error checking because if this fails there is very little that can be done
  sftp_close(file);
  return FILE_WRITTEN_SUCCESSFULLY;
}

enum FileStatus sftp_session_rename_file(Session *session, const char *path, const char *new_path) {
  if (sftp_rename(session->sftp, path, new_path) < 0) return FILE_WRITE_FAILED;
  return FILE_WRITTEN_SUCCESSFULLY;
}

int sftp_session_rmdir(Session *session, const char *dir_name, bool recursive) {
  sftp_dir dir;
  sftp_attributes attr;
  int ret;

  if (recursive) {
    dir = sftp_opendir(session->sftp, dir_name);
    if (!dir) return -1;

    while ((attr = sftp_readdir(session->sftp, dir)) != NULL) {
      if ((strcmp(attr->name, ".") != 0) && (strcmp(attr->name, "..") != 0)) {

        char *filepath = construct_filepath(dir_name, attr->name);
        if (!filepath) {
          sftp_attributes_free(attr);
          sftp_closedir(dir);
          return -1;
        }

        if (is_folder(attr->type)) {
          ret = sftp_session_rmdir(session, filepath, true);
        } else {
          ret = sftp_unlink(session->sftp, filepath);
        }
        if (ret < 0) {
          sftp_attributes_free(attr);
          sftp_closedir(dir);
          return -1;
        }
      }
      sftp_attributes_free(attr);
    }

    if (!sftp_dir_eof(dir)) return -1;
    sftp_closedir(dir);

  }
  return sftp_rmdir(session->sftp, dir_name);
}

int sftp_session_remove_completely_file(Session *session, const char *filepath) {
  sftp_attributes attr;
  attr = sftp_stat(session->sftp, filepath);
  int ret = -1;
  if (attr) {
    if (is_folder(attr->type)) {
      sftp_attributes_free(attr);
      if ((ret = sftp_session_rmdir(session, filepath, true)) != 0) {
        Session_message(session, get_error(ERROR_DELETE_REMOTE_DIR));
      }
    } else {
      sftp_attributes_free(attr);
      if ((ret = sftp_unlink(session->sftp, filepath)) != 0) {
        Session_message(session, get_error(ERROR_DELETE_REMOTE_FILE));
      }
    }
  } else {
    Session_message(session, get_error(ERROR_DISPLAYING_REMOTE_FILES));
  }
  return ret;
}
