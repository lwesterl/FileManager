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
  FILE_READ_FAILED = -3 /**< File read failed */
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
  uint32_t uid; /**< File uid */
  uint32_t gid; /**< File gid */
  char *owner;  /**< Owner of the file */
  char *group; /**< Group the file belongs to */
  uint32_t permissions; /**< File permissions */
  uint64_t createtime;  /**< Time when the file was created */
};

/* Linked list management */

/**
  *   @brief Clear linked list containing File items
  *   @param files Pointer to a GSList
  *   @remark All Files must be allocated dynamically. This is basically
  *   a wrapper for g_list_free_full()
  */
inline static void clear_Filelist(GSList *files) {
  g_slist_free_full(files, free);
}

/**
  *   @brief append one element to list containing File elements
  *   @param files Pointer to a GSList which contains directory files
  *   @param file File to be added (this must be allocated dynamically)
  *   @remark This is basically a wrapper for
  */
inline static GSList *append_FileList(GSList *files, struct File *file) {
  return g_slist_append(files, file);
}


/* Local filesystem management */
/**
  *   @brief Check whether a file exists
  *   @param filename To be checked (path)
  *   @return true if file exists, otherwise false
  */
bool file_exists(const char *filename);


#endif // end FS_HEADER
