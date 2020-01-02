/**
  *   @file fs.c
  *   @author Lauri Westerholm
  *   @brief Local filesystem management source
  */

#include "../include/fs.h"

void iterate_FileList(GSList *files, void f (File_t *)) {
  GSList *nxt = files;
  do {
    if (nxt) {
      f((struct File *) nxt->data);
    }
  } while ((nxt = nxt->next) != NULL);
}

/* Local filesystem management */

bool file_exists(const char *filename) {
  struct stat st = {0};
  return (stat (filename, &st) == 0);
}

enum FileStatus fs_mkdir(const char *dir_name) {
  int permissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IXOTH;
  if (!file_exists(dir_name)) {
    if (mkdir(dir_name, permissions) != 0) {
      return MKDIR_FAILED;
    }
  }
  return FILE_WRITTEN_SUCCESSFULLY;
}

int fs_rmdir(const char *dir_name) {
  return rmdir(dir_name);
}

GSList* ls_dir(GSList *files, const char *dir_name) {
  DIR *dir = NULL;
  struct stat st = {0};
  struct dirent *dt = NULL;
  clear_Filelist(files);

  dir = opendir(dir_name);
  if (!dir) {
    return NULL;
  }
  while ((dt = readdir(dir)) != NULL) {
    if ((strcmp(dt->d_name, ".") != 0) && (strcmp(dt->d_name, "..") != 0)) {
      struct File *file = malloc(sizeof(struct File));
      file->name = malloc(strlen(dt->d_name) + 1);
      strcpy(file->name, dt->d_name);
      file->type = dt->d_type;

      char *filepath = NULL;
      if (dir_name[strlen(dir_name) -1] == '/') {
        // No need to set slash to the end of dir_name
        filepath = malloc(strlen(dir_name) + strlen(file->name) + 1);
        strcpy(filepath, dir_name);
        strcat(filepath, file->name);
      } else {
        filepath = malloc(strlen(dir_name) + strlen(file->name) + 2);
        strcpy(filepath, dir_name);
        filepath[strlen(filepath)] = '/';
        strcat(filepath, file->name);
      }

      if (stat(filepath, &st) != 0) {
        free(file);
        free(filepath);
        clear_Filelist(files);
        return NULL;
      }
      free(filepath);
      file->size = st.st_size;
      file->uid = st.st_uid;
      file->gid = st.st_gid;
      file->owner = NULL;
      file->group = NULL;
      file->permissions = st.st_mode;
      file->mtime = st.st_mtim.tv_sec;
      struct passwd *pw = getpwuid(st.st_uid);
      if (pw) {
        file->owner = malloc(strlen(pw->pw_name) + 1);
        strcpy(file->owner, pw->pw_name);
      }
      struct group *gr = getgrgid(st.st_gid);
      if (gr) {
        file->group = malloc(strlen(gr->gr_name) + 1);
        strcpy(file->group, gr->gr_name);
      }
      files = append_FileList(files, file);
    }
  }
  return files;
}
