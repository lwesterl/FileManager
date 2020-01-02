/**
  *   @file test_fs.c
  *   @author Lauri Westerholm
  *   @brief Test file for fs.c
  */

#include <assert.h>

#include "../include/fs.h"

void print_File(File_t *file) {
  printf("Filename: %s \t", file->name);
  printf("owner: %s, group: %s\t", file->owner, file->group);
  printf("type: %d, size: %ld\n", file->type, file->size);
}


int main() {
  const char *dir_name = "testDIR";
  assert(fs_mkdir(dir_name) == FILE_WRITTEN_SUCCESSFULLY);
  assert(file_exists(dir_name));
  assert(!file_exists("some_random_file_name"));

  GSList *files = NULL;
  assert((files = ls_dir(files, ".")));
  printf("files length: %d\n", g_slist_length(files));
  iterate_FileList(files, print_File);

  assert(fs_rmdir(dir_name) == 0);

  printf("test_fs.c successfully finished\n");
  return EXIT_SUCCESS;
}
