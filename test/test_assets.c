/**
  *   @file test_assets.c
  *   @author Lauri Westerholm
  *   @brief Test main for assets
  */

#include <assert.h>
#include <stdio.h>

#include "../include/assets.h"


int main() {

  const char *path = "/home/test/";
  const char *path2 = "/home/test";

  init_assets();
  local_pwd = change_pwd(local_pwd, path);
  assert(strcmp(path, local_pwd) == 0);
  local_pwd = change_pwd(local_pwd, path2);
  assert(strcmp(path2, local_pwd) == 0);


  remote_pwd = change_pwd(remote_pwd, local_pwd);
  local_pwd = cd_back_pwd(local_pwd);
  assert(strcmp(local_pwd, "/home/") == 0);
  assert(strcmp(remote_pwd, path2) == 0);
  remote_pwd = cd_back_pwd(remote_pwd);
  assert(strcmp(remote_pwd, "/home/") == 0);
  local_pwd = cd_back_pwd(local_pwd);
  assert(strcmp(local_pwd, "/") == 0);
  local_pwd = cd_back_pwd(local_pwd);
  assert(strcmp(local_pwd, "/") == 0);
  remote_pwd = change_pwd(remote_pwd, "");
  remote_pwd = cd_back_pwd(remote_pwd);
  assert(strcmp(remote_pwd, "") == 0);
  remote_pwd = change_pwd(remote_pwd, "re");
  remote_pwd = cd_back_pwd(remote_pwd);
  assert(strcmp(remote_pwd, "re") == 0);

  local_pwd = change_pwd(local_pwd, path);
  assert(strcmp(local_pwd, path) == 0);
  local_pwd = cd_enter_pwd(local_pwd, "test");
  assert(strcmp(local_pwd, "/home/test/test") == 0);
  local_pwd = change_pwd(local_pwd, path2);
  assert(strcmp(local_pwd, path2) == 0);
  local_pwd = cd_enter_pwd(local_pwd, "test");
  assert(strcmp(local_pwd, "/home/test/test") == 0);


  clear_assets();

  printf("test_assets.c successfully finished\n");
  return EXIT_SUCCESS;
}
