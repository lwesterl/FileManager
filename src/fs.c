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

int iterate_FileCopyList(   GSList *fileCopyList,
                            int f (const FileCopy_t *, const void *, const bool, const bool),
                            const void *ptr,
                            const bool overwrite,
                            const bool target_remote) {
  GSList *nxt = fileCopyList;
  do {
    if (nxt) {
      int ret = f((FileCopy_t *) nxt->data, ptr, overwrite, target_remote);
      if (ret < 0) return ret;
    }
  } while ((nxt = nxt->next) != NULL);
  return 0;
}

GSList *copy_FileCopyList(GSList *srcList) {
  GSList *ptr = srcList;
  GSList *new_list = NULL;
  GSList *first = NULL;
  FileCopy_t *data;
  if (ptr) {
    do {
      FileCopy_t *new = malloc(sizeof(FileCopy_t));
      if (!new && new_list) {
        clear_FileCopyList(new_list);
        return NULL;
      }
      data = (FileCopy_t *) ptr->data;
      new->remote = data->remote;
      new->filename = malloc(strlen(data->filename) + 1);
      new->filepath = malloc(strlen(data->filepath) + 1);
      if (!new->filename || !new->filepath) {
        if (new->filename) free(new->filename);
        if (new->filepath) free(new->filepath);
        free(new);
        clear_FileCopyList(new_list);
        return NULL;
      }
      strcpy(new->filename, data->filename);
      strcpy(new->filepath, data->filepath);
      new_list = append_FileCopyList(new_list, new);
      if (!first) first = new_list;
    } while ((ptr = ptr->next) != NULL);
  }
  return first;
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
  } else return DIR_ALREADY_EXISTS;
  return FILE_WRITTEN_SUCCESSFULLY;
}

enum FileStatus fs_rmdir(const char *dir_name, const bool recursive) {
  if (recursive) {
      DIR *dir = NULL;
      struct dirent *dt = NULL;

      dir = opendir(dir_name);
      if (!dir) {
        return FILE_REMOVE_FAILED;
      }
      int ret;
      while ((dt = readdir(dir)) != NULL) {
        if ((strcmp(dt->d_name, ".") != 0) && (strcmp(dt->d_name, "..") != 0)) {
          if (stop) {
            closedir(dir);
            return STOP_FILE_OPERATIONS;
          }
          char *filepath = construct_filepath(dir_name, dt->d_name);
          if (!filepath) {
            closedir(dir);
            return FILE_REMOVE_FAILED;
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
            return FILE_REMOVE_FAILED; // Some error, return immediately
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

enum FileStatus remove_completely(const char *filepath) {
  struct stat st;
  if (stat(filepath, &st) == 0) {
    if (st.st_mode & S_IFDIR) {
      return fs_rmdir(filepath, true);
    } else {
      if (unlink(filepath) == 0) return FILE_WRITTEN_SUCCESSFULLY;
    }
  }
  return FILE_REMOVE_FAILED; // stat error
}

enum FileStatus fs_copy_file( const char *src,
                              const char *filename,
                              const char *dst,
                              const bool overwrite) {
  int src_fd, dst_fd;
  struct stat st = {0};
  mode_t mode;
  char *new_filepath = NULL;
  off_t copied = 0;
  int src_flags = O_RDONLY;
  int dst_flags = overwrite ? O_CREAT | O_WRONLY | O_TRUNC : O_CREAT | O_WRONLY | O_EXCL;
  if (stat(src, &st) == 0) {
    mode = st.st_mode;
    new_filepath = construct_filepath(dst, filename);
    if (!new_filepath) return FILE_COPY_FAILED;
    if ((src_fd = open(src, src_flags)) != -1) {
      if ((dst_fd = open(new_filepath, dst_flags, mode)) != -1) {
        int ret = sendfile(dst_fd, src_fd, &copied, st.st_size);
        close(src_fd);
        close(dst_fd);
        free(new_filepath);
        if (ret != -1) return FILE_WRITTEN_SUCCESSFULLY;
        return FILE_COPY_FAILED;
      } else {
        close(src_fd);
        free(new_filepath);
        if (errno == EEXIST) return FILE_ALREADY_EXISTS;
        return FILE_COPY_FAILED;
      }
    } else free(new_filepath);
  }
  return FILE_COPY_FAILED;
}

enum FileStatus fs_copy_dir(  const char *src,
                              const char *dirname,
                              const char *dst,
                              const bool recursive,
                              const bool overwrite)
{
  DIR *dir = NULL;
  struct dirent *dt = NULL;
  int ret;
  struct stat st = {0};
  char *new_dir = construct_filepath(dst, dirname);
  if (!new_dir) return FILE_COPY_FAILED;
  bool exists = stat(new_dir, &st) == 0 && S_ISDIR(st.st_mode);
  if (exists && !overwrite) {
    free(new_dir);
    return DIR_ALREADY_EXISTS;
  }
  if (!exists) {
    ret = fs_mkdir(new_dir);
    if (ret == MKDIR_FAILED) return FILE_COPY_FAILED;
  }
  if (recursive) {
    dir = opendir(src);
    if (!dir) {
      free(new_dir);
      return FILE_COPY_FAILED;
    }
    while ((dt = readdir(dir)) != NULL) {
      if (stop) {
        free(new_dir);
        closedir(dir);
        return STOP_FILE_OPERATIONS;
      }
      if ((strcmp(dt->d_name, ".") != 0) && (strcmp(dt->d_name, "..") != 0)) {
        char *src_path = construct_filepath(src, dt->d_name);
        if (!src_path) {
          free(new_dir);
          closedir(dir);
          return FILE_COPY_FAILED;
        }
        if (is_folder(dt->d_type)) {
          // Copy a sub-directory
          ret = fs_copy_dir(src_path, dt->d_name, new_dir, recursive, overwrite);
        } else {
          // Copy a file
          ret = fs_copy_file(src_path, dt->d_name, new_dir, overwrite);
        }
        free(src_path);
        if (ret < 0) {
          free(new_dir);
          closedir(dir);
          return ret; // Some error, return immediately
        }
      }
    }
    closedir(dir);
  }
  free(new_dir);
  return FILE_WRITTEN_SUCCESSFULLY;
}

enum FileStatus fs_copy_files(  const char *src,
                                const char *filename,
                                const char *dst,
                                const bool recursive,
                                const bool overwrite)
{
  struct stat st;
  if (stat(src, &st) == 0) {
    if (st.st_mode & S_IFDIR) {
      // Copy directory
      return fs_copy_dir(src, filename, dst, recursive, overwrite);
    } else {
      // Copy file
      return fs_copy_file(src, filename, dst, overwrite);
    }
  }
  return FILE_WRITE_FAILED;
}

struct FileContent *fs_read_file(const char *filepath) {
  struct FileContent *content = NULL;
  struct stat st = {0};
  int fd = open(filepath, O_RDONLY);
  if ((fd > 0) && (stat(filepath, &st) == 0)) {
    content = malloc(sizeof(struct FileContent));
    if (content) {
      content->buff = malloc(st.st_size);
      if (content->buff) {
        content->len = read(fd, content->buff, st.st_size);
        if (content->len != st.st_size) {
          // Some error
          free(content->buff);
          free(content);
          content = NULL;
        }
      } else {
        free(content);
        content = NULL;
      }
    }
    close(fd);
  }
  return content;
}
