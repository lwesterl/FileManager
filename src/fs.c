/**
  *   @file fs.c
  *   @author Lauri Westerholm
  *   @brief Local filesystem management source
  */

#include "../include/fs.h"

void iterate_FileList(GSList *files, void f (File_t *, void *), void *ptr) {
  GSList *nxt = files;
  do {
    if (nxt) {
      f((struct File *) nxt->data, ptr);
    }
  } while ((nxt = nxt->next) != NULL);
}

void iterate_FileCopyList( GSList *fileCopyList,
                           void f (const FileCopy_t *, const void *),
                           const void *ptr) {
  GSList *nxt = fileCopyList;
  do {
    if (nxt) {
      f((FileCopy_t *) nxt->data, ptr);
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

int fs_rmdir(const char *dir_name, bool recursive) {
  if (recursive) {
      DIR *dir = NULL;
      struct dirent *dt = NULL;

      dir = opendir(dir_name);
      if (!dir) {
        return -1;
      }
      int ret;
      while ((dt = readdir(dir)) != NULL) {
        if ((strcmp(dt->d_name, ".") != 0) && (strcmp(dt->d_name, "..") != 0)) {

          char *filepath = construct_filepath(dir_name, dt->d_name);
          if (!filepath) {
            closedir(dir);
            return -1;
          }

          if (is_folder(dt->d_type)) {
            ret = fs_rmdir(filepath, true);
          } else {
            // Remove single file
            ret = unlink(filepath);
          }
          free(filepath);
          if (ret < 0) {
            closedir(dir);
            return ret; // Some error, return immediately
          }
        }
      }
      closedir(dir);
  }
  // Remove the current dir
  return rmdir(dir_name);
}

GSList* ls_dir(GSList *files, const char *dir_name) {
  DIR *dir = NULL;
  struct stat st = {0};
  struct dirent *dt = NULL;
  clear_Filelist(files);
  files = NULL;

  dir = opendir(dir_name);
  if (!dir) {
    return NULL;
  }
  while ((dt = readdir(dir)) != NULL) {
    struct File *file = malloc(sizeof(struct File));
    file->name = malloc(strlen(dt->d_name) + 1);
    strcpy(file->name, dt->d_name);
    file->type = dt->d_type;

    char *filepath = construct_filepath(dir_name, file->name);
    if (!filepath) {
      free(file);
      clear_Filelist(files);
      return NULL;
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
  return files;
}

char *get_home_dir() {
  char *home = NULL;
  struct passwd *pw = getpwuid(getuid());
  if (pw) {
    home = malloc(strlen(pw->pw_dir) + 1);
    if (home) strcpy(home, pw->pw_dir);
  }
  return home;
}

enum FileStatus fs_rename(const char *old_name, const char *new_name) {
  if (file_exists(new_name)) return FILE_ALREADY_EXISTS;
  if (rename(old_name, new_name) == 0) return FILE_WRITTEN_SUCCESSFULLY;
  return FILE_WRITE_FAILED;
}

int remove_completely(const char *filepath) {
  struct stat st;
  if (stat(filepath, &st) == 0) {
    if (st.st_mode & S_IFDIR) {
      return fs_rmdir(filepath, true);
    } else {
      return unlink(filepath);
    }
  }
  return -1; // stat error
}
