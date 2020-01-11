/**
  *   @file test_fs.c
  *   @author Lauri Westerholm
  *   @brief Test file for fs.c
  */

#include <assert.h>

#include "../include/fs.h"

void print_File(File_t *file, __attribute__((unused)) void *ptr) {
  printf("Filename: %s \t", file->name);
  printf("owner: %s, group: %s\t", file->owner, file->group);
  printf("type: %d, size: %ld\n", file->type, file->size);
}


int main() {
  const char *dir_name = "testDIR";
  const char *dir_name2 = "testDIR2";
  assert(fs_mkdir(dir_name) == FILE_WRITTEN_SUCCESSFULLY);
  assert(file_exists(dir_name));
  assert(!file_exists("some_random_file_name"));

  GSList *files = NULL;
  assert((files = ls_dir(files, ".")));
  printf("files length: %d\n", g_slist_length(files));
  iterate_FileList(files, print_File, NULL);
  clear_Filelist(files);

  const char *file = "testDIR/test_file.txt";
  const char *file2 = "testDIR/test_file_updated.txt";
  int fd = open(file, O_CREAT | O_WRONLY);
  if (fd) {
    const char *buff = "Hello world\n";
    write(fd, buff, strlen(buff));
    close(fd);
    assert(fs_rename(file, file2) == FILE_WRITTEN_SUCCESSFULLY);
  }

  assert(fs_rename(dir_name, dir_name2) == FILE_WRITTEN_SUCCESSFULLY);
  assert(fs_rmdir(dir_name2, true) == 0);

  printf("test_fs.c successfully finished\n");
  return EXIT_SUCCESS;
}
