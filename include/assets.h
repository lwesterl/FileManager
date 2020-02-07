/**
  *   @file assets.h
  *   @author Lauri Westerholm
  *   @brief Manage assets
  */

#ifndef ASSETS_HEADER
#define ASSETS_HEADER

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libssh/sftp.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define CSS_FILE_PATH "../layout/styles.css" /**< Path to the css file */

/**
  *   @enum bool
  *   @brief Booleans: true and false
  */
typedef enum {
  false = 0, /**< false Boolean */
  true = 1 /**< true Boolean */
} bool;

/**
  *   @enum Icon
  *   @brief Icons used to represent different types of files
  */
enum Icon {
  FILE_ICON,
  FOLDER_ICON
};

/**
  *   @enum FilePermissions
  *   @brief Contains different file permission combinations
  */
enum FilePermissions {
  NONE,
  READ,
  WRITE,
  EXECUTE,
  READ_WRITE,
  READ_EXECUTE,
  READ_WRITE_EXECUTE,
  WRITE_EXECUTE
};

/**< Strings matching FilePermissions */
static const char* const FilePermissionStrings[] = {
  "",
  "Read",
  "Write",
  "Execute",
  "Read-Write",
  "Read-Execute",
  "Read-Write-Execute",
  "Write-Execute"
};

/**
  *   @enum PermissionType
  *   @brief Specifies permission type used
  */
enum PermissionType {
  USER_PERMISSIONS = 0,
  GROUP_PERMISSIONS = 3,
  OTHERS_PERMISSIONS = 6
};


extern GdkPixbuf* iconImages[2]; /**< Stores Icon images */
extern char *local_pwd; /**< Present local working directory */
extern char *remote_pwd; /**< Present remote working directory */
extern volatile sig_atomic_t stop; /**< Stopping condition, set by UI */

/**
  *   @brief Get Icon, image
  *   @param icon Matching enum Icon value
  *   @return const pointer to a GdkPixbuf
  */
inline static const GdkPixbuf *get_Icon(const enum Icon icon) {
  return iconImages[icon];
}

/**
  *   @brief Get FilePermissionStrings entry
  *   @param perm FilePermissions enum value
  *   @return FilePermissionStrings entry
  */
inline static const char *get_FilePermissionStr(const enum FilePermissions perm) {
  return FilePermissionStrings[perm];
}

/**
  *   @brief Check whether a file type indicates a folder
  *   @param file_type To be checked
  *   @param remote Remote file (true) or local file (false)
  *   @return true if the file type corresponds to a folder
  */
inline static bool is_folder(const uint8_t file_type, const bool remote) {
  if (remote) return ((file_type == SSH_FILEXFER_TYPE_DIRECTORY) || (file_type == SSH_FILEXFER_TYPE_SYMLINK));
  return ((file_type == DT_DIR) || (file_type == DT_LNK));
}

/**
  *   @brief Get Icon, image based on file type
  *   @param file_type File type spesification
  *   @param remote Remote file (true) or local file (false)
  *   @return const pointer to a GdkPixbuf
  */
inline static const GdkPixbuf *get_Icon_filetype(const uint8_t file_type, const bool remote) {
  return  is_folder(file_type, remote) ? iconImages[FOLDER_ICON] : iconImages[FILE_ICON];
}

/**
  *   @brief Initialize all assets
  *   @remark Call this when the main window is started (from initUI)
  *   @return true if init was succesful, otherwise false (in that case the
  *   program should exit immediately)
  */
bool init_assets();

/**
  *   @brief Clear allocated assets
  *   @remark Call this just before the program exits
  */
void clear_assets();

/**
  *   @brief Load css styles
  *   @remark This must not be run prior the main window is created
  *   (call this from initUI)
  */
void load_css_styles();

/**
  *   @brief Change pwd
  *   @param pwd Current working directory
  *   @param new_pwd New working directory
  *   @return Pointer to the updated pwd
  *   @remark This should be only used to update local_pwd and remote_pwd
  */
char *change_pwd(char *pwd, const char *new_pwd);

/**
  *   @brief Change pwd back to parent directory if possible
  *   @remark Changing pwd back is not possible if the pwd is already root.
  *   This is intended to be called either for local_pwd or remote_pwd
  *   @param pwd Current working directory
  *   @return Pointer to the updated pwd
  */
char *cd_back_pwd(char *pwd);

/**
  *   @brief Change pwd to another directory
  *   @param pwd Current working directory, must be dynamically allocated
  *   @param dir_name Directory where the pwd is changed to
  *   @return Pointer to the updated pwd: dynamically allocated, must be freed
  *   @remark This is intended to be called either for local_pwd or remote_pwd
  */
char *cd_enter_pwd(char *pwd, const char *dir_name);

/**
  *   @brief Construct filepath from pwd and filename
  *   @param pwd Present working directory (either ends to '/' or not)
  *   @param filename Filename in the current working directory
  *   @return Dynamically allocated pointer to filepath, must be freed elsewhere
  */
char *construct_filepath(const char *pwd, const char *filename);

/**
  *   @brief Concat three strings with spaces between the strings
  *   @param str1 The 1st string
  *   @param str2 The 2nd string
  *   @param str3 The 3rd string
  *   @return Pointer to a dynamically alloated string which is concatenation of
  *   the three inputs (must be free elsewhere) or NULL in case of an error
  */
char *concat_three_strings_with_spaces(const char *str1, const char *str2, const char *str3);

/**
  *   @brief Convert seconds from epoch to time
  *   @param s Seconds since epoch
  *   @return Pointer to a dynamically allocated string
  */
char *seconds_to_time(uint64_t s);

/**
  *   @brief Get matching str for size in bytes
  *   @param bytes Size in bytes
  *   @return Dynamically allocated str consisting of size in bytes + " bytes"
  */
char *get_size_str(uint64_t bytes);

/**
  *   @brief Get string representation of file permissions
  *   @param permissions Permissions in stats.st_mode format
  *   @return Dynamically allocated string representing the permissions
  */
char *get_file_permissions_str(uint32_t permissions);

/**
  *   @brief Get description of permissions either for user, group or others
  *   @param perm Permissions in str format generated by get_file_permissions_str
  *   (must be this exact format)
  *   @param type Specifies which permission should be retrieved
  *   @return Constant string from FilePermissionStrings
  */
const char *get_permission_description(const char *perm, enum PermissionType type);

#endif // end ASSETS_HEADER
