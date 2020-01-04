/**
  *   @file assets.h
  *   @author Lauri Westerholm
  *   @brief Manage assets
  */

#ifndef ASSETS_HEADER
#define ASSETS_HEADER

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libssh/sftp.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#include "fs.h" // bool

/**
  *   @enum Icon
  *   @brief Icons used to represent different types of files
  */
enum Icon {
  FILE_ICON,
  FOLDER_ICON
};

/**< Paths mathing to Icons */
static const char* const IconPaths[] = {
  "../layout/icons/Gnome-text-x-generic.svg",
  "../layout/icons/Gnome-folder.svg"
};

extern const GdkPixbuf* iconImages[2]; /**< Stores Icon images */
extern char *local_pwd; /**< Present local working directory */
extern char *remote_pwd; /**< Present remote working directory */

/**
  *   @brief Get Icon, image
  *   @param icon Matching enum Icon value
  *   @return const pointer to a GdkPixbuf
  */
inline static const GdkPixbuf *get_Icon(const enum Icon icon) {
  return iconImages[icon];
}

/**
  *   @brief Get Icon, image based on file type
  *   @param file_type File type spesification
  *   @return const pointer to a GdkPixbuf
  *   TODO: make apropriate
  */
inline static const GdkPixbuf *get_Icon_filetype(const uint8_t file_type) {
  return  file_type == 4 || file_type == SSH_FILEXFER_TYPE_DIRECTORY ?
          iconImages[FOLDER_ICON] : iconImages[FILE_ICON];
}

/**
  *   @brief Initialize all assets
  *   @remark Call this when the program is started
  *   @return true if init was succesful, otherwise false
  */
bool init_assets();

/**
  *   @brief Clear allocated assets
  *   @remark Call this just before the program exits
  */
void clear_assets();

/**
  *   @brief Change pwd
  *   @param pwd Current working directory
  *   @param new_pwd New working directory
  *   @return Pointer to the updated pwd
  *   @remark This should be only used to update local_pwd and remote_pwd
  */
char *change_pwd(char *pwd, const char *new_pwd);

#endif // end ASSETS_HEADER
