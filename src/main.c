/**
  *   @file main.c
  *   @author Lauri Westerholm
  *   @brief FileManager main
  */

#include "../include/UI.h"
#include "../include/assets.h"

int main(int argc, char *argv[]) {
  
  if(!init_assets()) {
    perror(get_error(ERROR_LOADING_ASSETS));
    return EXIT_FAILURE;
  }
  initUI(argc, argv);
  return EXIT_SUCCESS;
}
