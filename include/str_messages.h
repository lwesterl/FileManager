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
  "Error: password authentication\n"
};

/** Error enums */
enum ErrorCode {
  SSH_CREATE_ERROR = 0,
  SSH_CONNECT_ERROR = 1,
  ERROR_PUB_KEY_NOT_FOUND = 2,
  ERROR_PUB_KEY_HASH_FAILED = 3,
  ERROR_PUB_KEY_CHANGED = 4,
  ERROR_PUB_KEY_NOT_APROPRIATE = 5,
  KNOWN_HOSTS_NOT_FOUND = 6,
  SERVER_UNKNOWN = 7,
  ERROR_PASSWORD_AUTHENTICATION = 8
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
