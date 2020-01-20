/**
  *   @file test_fs.c
  *   @author Lauri Westerholm
  *   @brief Test file for fs.c
  */

#include <assert.h>

#include "../include/fs.h"
#include "../include/assets.h"

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

  const char *src_dir = "TEST_test";
  const char *dst_dir = "TEST_test2";
  const char *filepath = "TEST_test/test.txt";
  const char *filename = "test.txt";
  assert(fs_mkdir(src_dir) == FILE_WRITTEN_SUCCESSFULLY);
  assert(fs_mkdir(dst_dir) == FILE_WRITTEN_SUCCESSFULLY);
  fd = open(filepath, O_CREAT | O_WRONLY, S_IRWXU);
  if (fd) {
    const char *text = "Hello\nHello\nWhat's up?\nNothing special, I'm just testing\n....\n";
    write(fd, text, strlen(text));
    close(fd);
    assert(fs_copy_file(filepath, filename, dst_dir, false) == FILE_WRITTEN_SUCCESSFULLY);
    assert(fs_copy_file(filepath, filename, dst_dir, false) == FILE_ALREADY_EXISTS);
    assert(fs_copy_file(filepath, filename, dst_dir, true) == FILE_WRITTEN_SUCCESSFULLY);
    assert(fs_copy_file(filepath, filename, src_dir, true) == FILE_WRITTEN_SUCCESSFULLY); // truncates the file
    assert(fs_copy_file(filepath, filename, src_dir, false) == FILE_ALREADY_EXISTS);

    assert(fs_copy_dir(src_dir, src_dir, dst_dir, false, false) == FILE_WRITTEN_SUCCESSFULLY);
    assert(fs_copy_dir(src_dir, src_dir, dst_dir, false, false) == DIR_ALREADY_EXISTS);
    assert(fs_copy_dir(src_dir, src_dir, dst_dir, true, false) == DIR_ALREADY_EXISTS);
    assert(fs_copy_dir(src_dir, src_dir, dst_dir, true, true) == FILE_WRITTEN_SUCCESSFULLY);
    assert(fs_copy_files(src_dir, src_dir, dst_dir, true, false) == DIR_ALREADY_EXISTS);
    assert(fs_copy_files(src_dir, src_dir, dst_dir, true, true) == FILE_WRITTEN_SUCCESSFULLY);
  }
  assert(fs_rmdir(src_dir, true) == 0);
  assert(fs_rmdir(dst_dir, true) == 0);
  printf("test_fs.c successfully finished\n");
  return EXIT_SUCCESS;
}
