/**
  *   @file main.c
  *   @author Lauri Westerholm
  *   @brief FileManager main
  */

#include "../include/UI.h"
#include "../include/assets.h"

int main(int argc, char *argv[]) {
  initUI(argc, argv);
  clear_assets();
  return EXIT_SUCCESS;
}
