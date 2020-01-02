/**
  *   @file fs.c
  *   @author Lauri Westerholm
  *   @brief Local filesystem management source
  */

#include "../include/fs.h"

bool file_exists(const char *filename) {
  struct stat test;
  return (stat (filename, &test) == 0);
}
