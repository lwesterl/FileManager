/**
  *   @file str_messages.h
  *   @author Lauri Westerholm
  *   @brief Contains string messages
  */

#ifndef STR_MESSAGES_HEADER
#define STR_MESSAGES_HEADER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MESSAGE_LEN 200

/** Error messages */
static const char* const error_strings[] =
{
  "Error: cannot create new ssh session\n",
  "Error: cannot connect remote (username/address)\n",
  "Error: cannot get server public key\n",
  "Error: public key hashing failed\n",
  "Error: server public key changed, disconnected by security reasons\n",
  "Error: cannot find apropriate public key for server\n",
  "Info: cannot find ~/.ssh/known_hosts\n(created if the server key is accepted).\nThe server provided the following SHA1 public key:\n",
  "Info: server is unknown.\nDo you trust the server?\nThe server provided the following SHA1 public key:\n",
  "Error: password authentication\n",
  "Error: SFTP session allocation failed\n",
  "Error: SFTP session initialization failed\n",
  "Error: file already exists. Do you want to overwrite it?\n",
  "Error: cannot open the file\n",
  "Error: cannot write to the file\n",
  "Error: cannot open the directory\n",
  "Error: cannot list the directory\n",
  "Error: cannot read the file\n",
  "Error: cannot display local files\n",
  "Error: cannot load assets\nExit\n",
  "Error: cannot display remote files\n",
  "Error: cannot establish channelled connection to remote\n",
  "Error: executing command on remote failed\n",
  "Error: cannot rename the file\n",
  "Error: cannot create directory\n",
  "Error: cannot remove file\n"
};

/** Error enums */
enum ErrorCode {
  SSH_CREATE_ERROR,
  SSH_CONNECT_ERROR,
  ERROR_PUB_KEY_NOT_FOUND,
  ERROR_PUB_KEY_HASH_FAILED,
  ERROR_PUB_KEY_CHANGED,
  ERROR_PUB_KEY_NOT_APROPRIATE,
  KNOWN_HOSTS_NOT_FOUND,
  SERVER_UNKNOWN,
  ERROR_PASSWORD_AUTHENTICATION,
  ERROR_SFTP_ALLOCATION,
  ERROR_SFTP_INITIALIZATION,
  ERROR_FILE_ALREADY_EXISTS,
  ERROR_OPENING_FILE,
  ERROR_WRITING_TO_FILE,
  ERROR_OPENING_DIRECTORY,
  ERROR_LISTING_DIRECTORY,
  ERROR_READING_FILE,
  ERROR_DISPLAYING_LOCAL_FILES,
  ERROR_LOADING_ASSETS,
  ERROR_DISPLAYING_REMOTE_FILES,
  SSH_CHANNEL_ERROR,
  SSH_REMOTE_COMMAND_ERROR,
  ERROR_RENAMING_FILE,
  ERROR_MK_DIR,
  ERROR_DELETE_FILE
};

/**
  *   @brief Mapping between the enums and messages
  *   @param code enum ErrorCode
  *   @return Matching error string
  */
static inline const char *get_error(enum ErrorCode code) {
  return error_strings[(int) code];
}

extern char DetailedMessage[]; /**< Used to store a detailed error message */

/**
  *   @brief Set detailed error message
  *   @param code ErrorCode corresponding to the error
  *   @param message Descriptive error message (hash code)
  *   @param len Length of the message
  *   @return Pointer to the static detailed message
  *   @remark Notify MESSAGE_LEN
  */
const char *get_detailed_error(const enum ErrorCode code, const unsigned char *message, size_t len);

/**
  *   @brief Convert binary byte to char
  *   @param byte To be converted
  *   @remark Handle lower and upper bits separately
  */
static inline char binary_to_hex(const unsigned char byte) {
  return byte + (byte < 10 ? '0' : 'a' - 10);
}

#endif // end STR_MESSAGES_HEADER
