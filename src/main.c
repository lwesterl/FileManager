/**
  *   @file main.c
  *   @author Lauri Westerholm
  *   @brief FileManager main
  */

#include "../include/ssh.h"

int main() {
  const char *username = "";
  const char *address = "localhost";

  Session *session = create_session(username, address);

  if (session) {
    printf("SessionID: %d\n", session->sessionID);
    printf("Removing session\n");
    end_session(session);
  }
}
