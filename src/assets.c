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
volatile sig_atomic_t stop = 0;

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

char *seconds_to_time(uint64_t s) {
  const size_t len = 80;
  char *time = malloc(len);
  struct tm lt;
  time_t t = (time_t) s;
  localtime_r(&t, &lt);
  strftime(time, len, "%c", &lt);
  return time;
}

char *get_size_str(uint64_t bytes) {
  const int len = 30; //  This should be enough
  char *size = malloc(len);
  if (size) {
    snprintf(size, len, "%ld bytes", bytes);
  }
  return size;
}

char *get_file_permissions_str(uint32_t permissions) {
  char *str = malloc(10);
  if (str) {
    str[0] = permissions & S_IRUSR ? 'r' : '-';
    str[1] = permissions & S_IWUSR ? 'w' : '-';
    str[2] = permissions & S_IXUSR ? 'x' : '-';
    str[3] = permissions & S_IRGRP ? 'r' : '-';
    str[4] = permissions & S_IWGRP ? 'w' : '-';
    str[5] = permissions & S_IXGRP ? 'x' : '-';
    str[6] = permissions & S_IROTH ? 'r' : '-';
    str[7] = permissions & S_IWOTH ? 'w' : '-';
    str[8] = permissions & S_IXOTH ? 'x' : '-';
    str[9] = '\0';
  }
  return str;
}

const char *get_permission_description(const char *perm, enum PermissionType type) {
  if (perm) {
    if (perm[type] == 'r') {
      if (perm[type + 1] == 'w') {
        if (perm[type + 2] == 'x') return get_FilePermissionStr(READ_WRITE_EXECUTE);
        return get_FilePermissionStr(READ_WRITE);
      } else {
        if (perm[type + 2] == 'x') return get_FilePermissionStr(READ_EXECUTE);
        return get_FilePermissionStr(READ);
      }
    }
    if (perm[type + 1] == 'w') {
      if (perm[type + 2] == 'x') return get_FilePermissionStr(WRITE_EXECUTE);
      return get_FilePermissionStr(WRITE);
    }
    if (perm[type + 2] == 'x') return get_FilePermissionStr(EXECUTE);
  }
  return get_FilePermissionStr(NONE);
}
