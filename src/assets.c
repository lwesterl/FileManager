/**
  *   @file assets.c
  *   @author Lauri Westerholm
  *   @brief Manage assets
  */

#include "../include/assets.h"

// Define globals
const GdkPixbuf* iconImages[2];
char *local_pwd = NULL;
char *remote_pwd = NULL;

bool init_assets() {
  GError *error = NULL;
  for (unsigned i = 0; i <= FOLDER_ICON; i++) {
    iconImages[i] = gdk_pixbuf_new_from_file(IconPaths[i], &error);
    if (iconImages[i] == NULL) {
      fprintf(stderr, "Asset loading error: %s\n", error->message);
      return false;
    }
  }
  return true;
}

void clear_assets() {
  if (local_pwd) free(local_pwd);
  if (remote_pwd) free(remote_pwd);
}

char *change_pwd(char *pwd, const char *new_pwd) {
  if (pwd) {
    free(pwd);
    pwd = NULL;
  }
  if (new_pwd) {
    pwd = malloc(strlen(new_pwd + 1));
    strcpy(pwd, new_pwd);
  }
  return pwd;
}
