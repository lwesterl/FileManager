/**
  *   @file str_messages.h
  *   @author Lauri Westerholm
  *   @brief Contains string messages
  */

#ifndef STR_MESSAGES_HEADER
#define STR_MESSAGES_HEADER

/** Error messages */
static const char* const error_strings[] =
{
  "Error: cannot create new ssh session\n",
  "Error: cannot connect remote (username/address)\n",
  "Error: cannot get server public key\n",
  "Error: public key hashing failed\n",
  "Error: server public key changed, disconnected by security reasons\n",
  "Error: cannot find apropriate public key for server\n",
  "Info: cannot find ~/.ssh/known_hosts (created if the server key is accepted)\n",
  "Info: server is unknown\n",
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

#endif // end STR_MESSAGES_HEADER
