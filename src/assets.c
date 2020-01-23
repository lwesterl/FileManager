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
    pwd = malloc(strlen(new_pwd) + 1);
    strcpy(pwd, new_pwd);
  }
  return pwd;
}

char *cd_back_pwd(char *pwd) {
  if (pwd) {
    size_t len = strlen(pwd);
    if (len > 1) {
      if (pwd[len -1] == '/') {
        // Path ends with /
        pwd[len -1] = '\0';
      }
      // Path doesn't end with /
      char *front = strrchr(pwd, '/');
      if (front) {
        for (++front; front != &pwd[len]; front++) {
          *front = '\0';
        }
      }
    }
  }
  return pwd;
}

char *cd_enter_pwd(char *pwd, const char *dir_name) {
  char *new_pwd = pwd;
  if (pwd) {
    size_t len = strlen(pwd);
    if (pwd[len -1] != '/') {
      // Need to also allocate space for /
      new_pwd = malloc(len + strlen(dir_name) + 2);
      strcpy(new_pwd, pwd);
      new_pwd[len] = '/';
      new_pwd[len + 1] = '\0';
    } else {
      new_pwd = malloc(len + strlen(dir_name) + 1);
      strcpy(new_pwd, pwd);
    }
    strcat(new_pwd, dir_name);
    free(pwd);
  }
  return new_pwd;
}

char *construct_filepath(const char *pwd, const char *filename) {
  char *filepath = NULL;
  size_t len = strlen(pwd);
  if (pwd[len -1] == '/') {
    filepath = malloc(len + strlen(filename) + 1);
    strcpy(filepath, pwd);
    strcat(filepath, filename);
  } else {
    filepath = malloc(len + strlen(filename) + 2);
    strcpy(filepath, pwd);
    filepath[len] = '/';
    filepath[len + 1] = '\0';
    strcat(filepath, filename);
  }
  return filepath;
}

char *concat_three_strings_with_spaces(const char *str1, const char *str2, const char *str3) {
  char *str = NULL;
  size_t len = strlen(str1) + strlen(str2) + strlen(str3);
  size_t tmp_len;
  str = malloc(len + 3); // 2 spaces and null byte
  if (str) {
    strcpy(str, str1);
    tmp_len = strlen(str);
    str[tmp_len] = ' ';
    str[tmp_len + 1] = '\0';
    strcat(str, str2);
    tmp_len = strlen(str);
    str[tmp_len] = ' ';
    str[tmp_len + 1] = '\0';
    strcat(str, str3);
  }
  return str;
}
