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
