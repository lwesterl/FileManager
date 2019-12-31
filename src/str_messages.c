/**
  *   @file str_messages.c
  *   @author Lauri Westerholm
  *   @brief Source for str_messages
  */

#include "../include/str_messages.h"

char DetailedMessage[MESSAGE_LEN] = {0};

const char *get_detailed_error(const enum ErrorCode code, const unsigned char *message, size_t len) {
  const char *error_msg = get_error(code);
  char *tmp = malloc(2 * len + 1);
  for (size_t i = 0; i < len; i++) {
    tmp[2 * i] = binary_to_hex(message[i] >> 4);
    tmp[2 * i + 1] = binary_to_hex(message[i] & 15);
  }
  tmp[2 * len] = '\0';
  strcpy(DetailedMessage, error_msg);
  if (strlen(tmp) + strlen(error_msg) < MESSAGE_LEN) {
    strcat(DetailedMessage, tmp);
  }
  free(tmp);
  return DetailedMessage;
}
