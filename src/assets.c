/**
  *   @file assets.c
  *   @author Lauri Westerholm
  *   @brief Manage assets
  */

#include "../include/assets.h"

// Define globals
const GdkPixbuf* iconImages[2];

bool init_assets() {
  GError *error = NULL;
  for (unsigned i = 0; i <= FOLDER_ICON; i++) {
    iconImages[i] = gdk_pixbuf_new_from_file(IconPaths[i], &error);
    if (iconImages[i] == NULL) {
      fprintf(stderr, "Asset loading error: %s\n", error->message);
      return false;
    }
  }
  return true;
}
