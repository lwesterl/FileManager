/**
  *   @file main.c
  *   @author Lauri Westerholm
  *   @brief FileManager main
  */

#include "../include/UI.h"

int main(int argc, char *argv[]) {

  initUI(argc, argv);

  /*const char *username = "test";
  const char *address = "localhost";

  Session *session = create_session(username, address);

  if (session) {
    printf("SessionID: %d\n", session->sessionID);

    enum AuthenticationAction result = authenticate_init(session);
    printf("Authentication result: %d\n", result);

    printf("Removing session\n");
    end_session(session);
  }*/
  return EXIT_SUCCESS;
}
