/**
  *   @file ssh.c
  *   @author Lauri Westerholm
  *   @brief Interface libssh API
  */

#include "../include/ssh.h"

Session *create_session(const char *username, const char *remote) {
  if (remote) {
    Session *session = malloc(sizeof(Session));
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
    ssh_disconnect(session->session);
    ssh_free(session->session);
    free(session);
    return 0;
  }
  return -1;
}
