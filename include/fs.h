/**
  *   @file fs.h
  *   @author Lauri Westerholm
  *   @brief Local file system management
  */

#ifndef FS_HEADER
#define FS_HEADER

#include <gmodule.h> // Linked list implementation, GSList

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/sendfile.h>

#include "assets.h"


/**
  *   @enum FileStatus
  *   @brief Contains values for different file related errors/events
  */
enum FileStatus {
  FILE_WRITTEN_SUCCESSFULLY = 0, /**< Successfully completed writing to a file */
  FILE_ALREADY_EXISTS = -1, /**< File already existed */
  FILE_WRITE_FAILED = -2, /**< File write failed */
  FILE_READ_FAILED = -3, /**< File read failed */
  MKDIR_FAILED = -4, /**< mkdir operation failed */
  DIR_ALREADY_EXISTS = -5, /**< A directory already exists */
  FILE_COPY_FAILED = -6, /**< A file copy operation failed */
  STOP_FILE_OPERATIONS = -7, /**< stop == 1, stop ongoing file operations */
  FILE_REMOVE_FAILED = -8 /**< A file removal failed */
};

/**
  *   @struct File
  *   @brief Contains necessary information about one file (or directory)
  *   @remark These are stored to linked lists
  */
struct File {
  char *name; /**< Filename*/
  uint8_t type; /**< Which type of a file */
  uint64_t size; /**< File size */
  uint32_t uid; /**< File uid, user id */
  uint32_t gid; /**< File gid, group id */
  char *owner;  /**< Owner of the file */
  char *group; /**< Group the file belongs to */
  uint32_t permissions; /**< File permissions */
  uint64_t mtime;  /**< Time when the file was modified */
};
typedef struct File File_t; /**< Needed for FileList iteration */

/**
  *   @struct FileCopy
  *   @brief Contains information about a file to be copied
  *   @remark These are stored in linked lists
  */
struct FileCopy {
  char *filename; /**< Name of the file */
  char *filepath; /**< Path to the file */
  bool remote; /**< Whether the file is on a remote filesystem or not */
};
typedef struct FileCopy FileCopy_t; /**< Type for ease of use */

/**
  *   @struct FileContent
  *   @brief Stores contents of a file
  */
struct FileContent {
  char *buff; /**< Buffer for file content */
  int len; /**< Buffer length */
};

/**
  *   @brief Free FileContent struct instance
  *   @param content Pointer to a FileContent struct
  */
inline static void free_FileContent(struct FileContent *content) {
  if (content) {
    if (content->buff) free(content->buff);
    free(content);
  }
}


/* Linked list management */

/**
  *   @brief Free File struct
  *   @param pointer Pointer to a File struct, passed as void *
  */
inline static void free_File(void *pointer) {
  struct File *file = (struct File *) pointer;
  if (file) {
    if (file->name) free(file->name);
    if (file->owner) free(file->owner);
    if (file->group) free(file->group);
    free(file);
  }
}

/**
  *   @brief Clear linked list containing File items
  *   @param files Pointer to a GSList
  *   @remark All Files must be allocated dynamically. This is basically
  *   a wrapper for g_list_free_full()
  */
inline static void clear_Filelist(GSList *files) {
  g_slist_free_full(files, free_File);
}

/**
  *   @brief Append one element to a list containing File elements
  *   @param files Pointer to a GSList which contains directory files
  *   @param file File to be added (this must be allocated dynamically)
  *   @return Updated pointer to the linked list
  *   @remark This is basically a wrapper for g_slist_append
  */
inline static GSList *append_FileList(GSList *files, struct File *file) {
  return g_slist_append(files, file);
}

/**
  *   @brief Iterate over all File structs in the FileList
  *   @param files Pointer to a linked list containing File structs, additionally void * can be passed
  *   @param f Pointer to a function which is executed for each entry in the list
  *   @param ptr Additional pointer which is passed to the function
  */
void iterate_FileList(GSList *files, void f (File_t*, void *), void *ptr);

/**
  *   @brief Free FileCopy struct
  *   @param pointer Pointer to a FileCopy struct, passed as void *
  */
inline static void free_FileCopy(void *pointer) {
  struct FileCopy *fileCopy = (FileCopy_t *) pointer;
  if (fileCopy) {
    if (fileCopy->filename) free(fileCopy->filename);
    if (fileCopy->filepath) free(fileCopy->filepath);
    free(fileCopy);
  }
}

/**
  *   @brief Clear linked list containing FileCopy structs
  *   @param fileCopyList A linked list containing FileCopy structs
  */
inline static void clear_FileCopyList(GSList *fileCopyList) {
  g_slist_free_full(fileCopyList, free_FileCopy);
}

/**
  *   @brief Append one element to a list containing FileCopy structs
  *   @param fileCopyList Pointer to a GSList containing FileCopy structs
  *   @param fileCopy A FileCopy struct to be appended
  *   @return Updated pointer to the linked list
  *   @remark This is only a wrapper for g_slist_append
  */
inline static GSList *append_FileCopyList(GSList *fileCopyList, FileCopy_t *fileCopy) {
  return g_slist_append(fileCopyList, fileCopy);
}

/**
  *   @brief Iterate over all FileCopy structs in the fileCopies
  *   @param fileCopyList A GSList which contains FileCopy structs
  *   @param f Pointer to a function which is executed for each entry in the fileCopies
  *   @param ptr Additional pointer which is passed to the function
  *   @param overwrite Whether to overwrite existing files
  *   @param target_remote Whether the target filepath is on remote
  *   @return 0 on success, < 0 on error (this requires f to return values in this fashion)
  */
int iterate_FileCopyList(   GSList *fileCopyList,
                            int f (const FileCopy_t*, const void *, const bool, const bool),
                            const void *ptr,
                            const bool overwrite,
                            const bool target_remote);

/**
  *   @brief Copy GSList containing FileCopy structs
  *   @param srcList To be copied (pointer to the start of the list)
  *   @return Pointer to the start of the copy
  */
GSList *copy_FileCopyList(GSList *srcList);

/* Local filesystem management */

/**
  *   @brief Check whether a file exists
  *   @param filename To be checked (path)
  *   @return true if file exists, otherwise false
  */
bool file_exists(const char *filename);

/**
  *   @brief Create a new directory
  *   @param dir_name Path to the directory to be created
  *   @return FILE_WRITTEN_SUCCESSFULLY on success, MKDIR_FAILED or DIR_ALREADY_EXISTS
  *   on error
  */
enum FileStatus fs_mkdir(const char *dir_name);

/**
  *   @brief Remove a directory
  *   @param dir_name Path to the directory to be removed
  *   @param recursive Whether to remove the dir recursively, or only an empty dir
  *   @return 0 on success, < 0 on error
  *   @remark This will stop and return STOP_FILE_OPERATIONS when global stop == 1
  *   (stop is defined in @see assets.h)
  */
enum FileStatus fs_rmdir(const char *dir_name, const bool recursive);

/**
  *   @brief List directory content to linked list of struct File
  *   @param files Linked list of struct File
  *   @param dir_name Path of the directory to be listed
  *   @return Valid pointer on success, otherwise a NULL pointer
  *   @remark Contents of files are cleared first, make sure files content is
  *   dynamically allocated
  */
GSList *ls_dir(GSList *files, const char *dir_name);

/**
  *   @brief Get home directory for the user
  *   @return Pointer to dynamically allocated memory, this must be freed elsewhere.
  *   In case of an error, returns NULL
  */
char *get_home_dir();

/**
  *   @brief Rename file if the new_name is not already used
  *   @param old_name Filepath to the current file location
  *   @param new_name Filepath to a new file location
  *   @return FILE_WRITTEN_SUCCESSFULLY, FILE_ALREADY_EXISTS or FILE_WRITE_FAILED
  */
enum FileStatus fs_rename(const char *old_name, const char *new_name);

/**
  *   @brief Remove filepath completely (works for files and directories)
  *   @details For directories this will call fs_rmdir to recursively erase the
  *   whole directory. For files this will call unlink
  *   @param filepath To be removed, can be file or directory
  *   @return 0 on success, < 0 on error
  */
enum FileStatus remove_completely(const char *filepath);

/**
  *   @brief Copy a single file from source to destination on local filesystem
  *   @param src Source file path (this needs to be a file not directory)
  *   @param filename Source file name
  *   @param dst Destination path (this is the parent directory)
  *   @param overwrite Whether to overwrite a possibly already existing file
  *   @return FileStatus (FILE_WRITTEN_SUCCESSFULLY = ok, FILE_ALREADY_EXISTS = you may try
  *   again with overwrite set to true, FILE_COPY_FAILED = some severe error)
  *   @remark This allows truncating a file when src and dst point to same path;
  *   check elsewhere that src and dst are not the same or set overwrite to false
  */
enum FileStatus fs_copy_file( const char *src,
                              const char *filename,
                              const char *dst,
                              const bool overwrite);

/**
  *   @brief Copy directory from source to destination on local filesystem
  *   @param src Source directory path (this is path to the dir itself)
  *   @param dirname Source directory name
  *   @param dst Destination directory path (this is path to the parent directory)
  *   @param overwrite Whether to overwrite a possibly already existing directory
  *   @param recursive Whether to copy the directory recursively
  *   @return FileStatus (FILE_WRITTEN_SUCCESSFULLY = ok, FILE_ALREADY_EXISTS = you may try
  *   again with overwrite set to true, DIR_ALREADY_EXISTS = you may try again with overwrite
  *   set to true, FILE_COPY_FAILED = some severe error)
  *   @remark This will gracefully stop and return STOP_FILE_OPERATIONS when global stop == 1
  *   (stop is defined in @see assets.h)
  */
enum FileStatus fs_copy_dir(  const char *src,
                              const char *dirname,
                              const char *dst,
                              const bool recursive,
                              const bool overwrite);

/**
  *   @brief Copy file (or directory content) from source to destination
  *   @param src Source file or directory path (path to the file itself, not parent directory)
  *   @param filename Source file name
  *   @param dst Destination file or directory path (path to the parent directory)
  *   @param recursive Whether to copy directory recursively, this does not
  *   @param overwrite Whether to overwrite possibly already existing destination
  *   file
  *   @remark This is a wrapper for fs_copy_file and fs_copy_dir
  *   @return FileStatus (FILE_WRITTEN_SUCCESSFULLY = ok, FILE_ALREADY_EXISTS = you may try
  *   again with overwrite set to true, DIR_ALREADY_EXISTS = you may try again with overwrite
  *   set to true, FILE_COPY_FAILED = some severe error)
  */
enum FileStatus fs_copy_files(  const char *src,
                                const char *filename,
                                const char *dst,
                                const bool recursive,
                                const bool overwrite);

/**
  *   @brief Read file from local filesystem to a buffer
  *   @param filepath Path to the file to be read
  *   @return Pointer to a FileContent struct or NULL in case of an error
  *   (dynamically allocated, must be freed)
  */
struct FileContent* fs_read_file(const char *filepath);

#endif // end FS_HEADER
