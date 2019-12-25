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
  "Error: cannot connect remote (username/address)\n"
};

/** Error enums */
enum ErrorCode {
  SSH_CREATE_ERROR = 0,
  SSH_CONNECT_ERROR = 1
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
