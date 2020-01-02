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

/**
  *   @enum bool
  *   @brief Booleans: true and false
  */
typedef enum {
  false = 0, /**< false Boolean */
  true = 1 /**< true Boolean */
} bool;


/**
  *   @enum FileStatus
  *   @brief Contains values for different file related errors/events
  */
enum FileStatus {
  FILE_WRITTEN_SUCCESSFULLY = 0, /**< Successfully completed writing to a file */
  FILE_ALREADY_EXISTS = -1, /**< File already existed */
  FILE_WRITE_FAILED = -2, /**< File write failed */
  FILE_READ_FAILED = -3, /**< File read failed */
  MKDIR_FAILED = -4 /**< mkdir operation failed */
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
  *   @brief append one element to list containing File elements
  *   @param files Pointer to a GSList which contains directory files
  *   @param file File to be added (this must be allocated dynamically)
  *   @remark This is basically a wrapper for g_slist_append
  */
inline static GSList *append_FileList(GSList *files, struct File *file) {
  return g_slist_append(files, file);
}

/**
  *   @brief Iterate over all File structs in the FileList
  *   @param files Pointer to a linked list containing File structs
  *   @param f pointer to a function which is executed for each entry in the list
  */
void iterate_FileList(GSList *files, void f (File_t*));


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
  *   @return FILE_WRITTEN_SUCCESSFULLY on success, MKDIR_FAILED on error
  */
enum FileStatus fs_mkdir(const char *dir_name);

/**
  *   @brief Remove an empty directory
  *   @param dir_name Path to the directory to be removed
  *   @remark This is just a wrapper for rmdir
  */
int fs_rmdir(const char *dir_name);

/**
  *   @brief List directory content to linked list of struct File
  *   @param files Linked list of struct File
  *   @param dir_name Path of the directory to be listed
  *   @return Valid pointer on success, otherwise a NULL pointer
  *   @remark Contents of files are cleared first, make sure files content is
  *   dynamically allocated
  */
GSList  *ls_dir(GSList *files, const char *dir_name);


#endif // end FS_HEADER
