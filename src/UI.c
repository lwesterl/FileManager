/**
  *   @file UI.c
  *   @author Lauri Westerholm
  *   @brief UI created using GTK+, source
  */

#include "../include/UI.h"

/* UI initializations */
void initUI(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  session = NULL;
  remoteFileStore = NULL;
  localFileStore = NULL;

  builder = gtk_builder_new_from_file(LAYOUT_PATH);

  init_MainWindow();
  init_ConnectWindow();
  init_MessageWindow();
  gtk_builder_connect_signals(builder, NULL);
  gtk_widget_show_all(connectWindow->ConnectDialog);

  // Start main event loop
  gtk_main();
}

void quitUI() {
    // Quit gtk event loop
  gtk_main_quit();
  if (session) {
    end_session(session);
  }

  g_object_unref(builder);
  // Free allocated memory
  clear_FileStore(remoteFileStore);
  clear_FileStore(localFileStore);
  free(mainWindow);
  free(connectWindow);
  free(messageWindow);
}

void init_MainWindow() {
  mainWindow = malloc(sizeof(MainWindow));
  mainWindow->TopWindow = GTK_WIDGET(gtk_builder_get_object(builder, "TopWindow"));
  mainWindow->TopGrid = GTK_WIDGET(gtk_builder_get_object(builder, "TopGrid"));
    // left-hand side
  mainWindow->LeftTopFrame = GTK_WIDGET(gtk_builder_get_object(builder, "LeftTopFrame"));
  mainWindow->LeftTopFrameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "LeftTopFrameLabel"));
  mainWindow->LeftTopFrameAlignment = GTK_WIDGET(gtk_builder_get_object(builder, "LeftTopFrameAlignment"));
  mainWindow->LeftInnerFrame = GTK_WIDGET(gtk_builder_get_object(builder, "LeftInnerFrame"));
  mainWindow->LeftInnerFrameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "LeftInnerFrameLabel"));
  mainWindow->LeftInnerFrameAlignment = GTK_WIDGET(gtk_builder_get_object(builder, "LeftInnerFrameAlignment"));
  mainWindow->LeftFileSelectFrame = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileSelectFrame"));
  mainWindow->LeftFileSelectFrameAlignment = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileSelectFrameAlignment"));
  mainWindow->LeftFileScrollWindow = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileScrollWindow"));
  mainWindow->LeftScrollWindowViewPort = GTK_WIDGET(gtk_builder_get_object(builder, "LeftScrollWindowViewPort"));
  mainWindow->LeftFileView = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileView"));
  mainWindow->LeftFileSelectGrid = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileSelectGrid"));
  mainWindow->LeftFileHomeButton = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileHomeButton"));
  mainWindow->LeftFileBackButton = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileBackButton"));
  mainWindow->LeftNewFolderButton = GTK_WIDGET(gtk_builder_get_object(builder, "LeftNewFolderButton"));
  // right-hand side
  mainWindow->RightTopFrame = GTK_WIDGET(gtk_builder_get_object(builder, "RightTopFrame"));
  mainWindow->RightTopFrameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "RightTopFrameLabel"));
  mainWindow->RightTopFrameAlignment = GTK_WIDGET(gtk_builder_get_object(builder, "RightTopFrameAlignment"));
  mainWindow->RightInnerFrame = GTK_WIDGET(gtk_builder_get_object(builder, "RightInnerFrame"));
  mainWindow->RightInnerFrameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "RightInnerFrameLabel"));
  mainWindow->RightInnerFrameAlignment = GTK_WIDGET(gtk_builder_get_object(builder, "RightInnerFrameAlignment"));
  mainWindow->RightFileSelectFrame = GTK_WIDGET(gtk_builder_get_object(builder, "RightFileSelectFrame"));
  mainWindow->RightFileSelectFrameAlignment = GTK_WIDGET(gtk_builder_get_object(builder, "RightFileSelectFrameAlignment"));
  mainWindow->RightFileScrollWindow = GTK_WIDGET(gtk_builder_get_object(builder, "RightFileScrollWindow"));
  mainWindow->RightScrollWindowViewPort = GTK_WIDGET(gtk_builder_get_object(builder, "RightScrollWindowViewPort"));
  mainWindow->RightFileView = GTK_WIDGET(gtk_builder_get_object(builder, "RightFileView"));
  mainWindow->RightFileSelectGrid = GTK_WIDGET(gtk_builder_get_object(builder, "RightFileSelectGrid"));
  mainWindow->RightFileHomeButton = GTK_WIDGET(gtk_builder_get_object(builder, "RightFileHomeButton"));
  mainWindow->RightFileBackButton = GTK_WIDGET(gtk_builder_get_object(builder, "RightFileBackButton"));
  mainWindow->RightNewFolderButton = GTK_WIDGET(gtk_builder_get_object(builder, "RightNewFolderButton"));

  g_signal_connect(mainWindow->TopWindow, "destroy", G_CALLBACK(quitUI), NULL);
}

void init_ConnectWindow() {
  connectWindow = malloc(sizeof(ConnectWindow));
  connectWindow->ConnectDialog = GTK_WIDGET(gtk_builder_get_object(builder, "ConnectDialog"));
  connectWindow->ConnectDialogBox = GTK_WIDGET(gtk_builder_get_object(builder, "ConnectDialogBox"));
  connectWindow->ConnectWindowLabel = GTK_WIDGET(gtk_builder_get_object(builder, "ConnectWindowLabel"));
  connectWindow->SetIPLabel = GTK_WIDGET(gtk_builder_get_object(builder, "SetIPLabel"));
  connectWindow->SetIPEntry = GTK_WIDGET(gtk_builder_get_object(builder, "SetIPEntry"));
  connectWindow->SetUsernameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "SetUsernameLabel"));
  connectWindow->SetUsernameEntry = GTK_WIDGET(gtk_builder_get_object(builder, "SetUsernameEntry"));
  connectWindow->SetPasswordLabel = GTK_WIDGET(gtk_builder_get_object(builder, "SetPasswordLabel"));
  connectWindow->SetPasswordEntry = GTK_WIDGET(gtk_builder_get_object(builder, "SetPasswordEntry"));
  connectWindow->ConnectDialogButtonBox = GTK_WIDGET(gtk_builder_get_object(builder, "ConnectDialogButtonBox"));
  connectWindow->QuitButton = GTK_WIDGET(gtk_builder_get_object(builder, "QuitButton"));
  connectWindow->ConnectButton = GTK_WIDGET(gtk_builder_get_object(builder, "ConnectButton"));

  g_signal_connect(connectWindow->ConnectDialog, "destroy", G_CALLBACK(quitUI), NULL);
}

void init_MessageWindow() {
  messageWindow = malloc(sizeof(MessageWindow));
  messageWindow->MessageDialog = GTK_WIDGET(gtk_builder_get_object(builder, "MessageDialog"));
  messageWindow->MessageDialogBox = GTK_WIDGET(gtk_builder_get_object(builder, "MessageDialogBox"));
  messageWindow->MessageLabel = GTK_WIDGET(gtk_builder_get_object(builder, "MessageLabel"));
  messageWindow->MessageButtonBox = GTK_WIDGET(gtk_builder_get_object(builder, "MessageButtonBox"));
  messageWindow->CancelButton = GTK_WIDGET(gtk_builder_get_object(builder, "CancelButton"));
  messageWindow->OkButton = GTK_WIDGET(gtk_builder_get_object(builder, "OkButton"));

  g_signal_connect(messageWindow->MessageDialog, "destroy", G_CALLBACK(close_MessageWindow), NULL);
}

void close_MessageWindow() {
  gtk_widget_hide(messageWindow->MessageDialog);
  gtk_label_set_text((GtkLabel*) messageWindow->MessageLabel, "");
}

/* Window transitions */
void transition_MessageWindow(const enum MessageType messageType, const char *message) {
  messageWindow->messageType = messageType;
  gtk_label_set_text((GtkLabel*) messageWindow->MessageLabel, message);
  gtk_widget_show_all(messageWindow->MessageDialog);
  if (messageType == INFO_ERROR) {
    // CancelButton not available
    gtk_widget_hide(messageWindow->CancelButton);
  }
}

void transition_MainWindow() {
  // Establish SFTP session
  if ((get_remote_home_dir(session) == 0) && (init_sftp_session(session) == 0)) {
    close_MessageWindow();
    gtk_widget_hide(connectWindow->ConnectDialog);
    char *home = get_home_dir();
    if (home) {
      local_pwd = change_pwd(local_pwd, home); // This hard-copies the content
      free(home);
    } else {
      local_pwd = change_pwd(local_pwd, "/");
    }
    remote_pwd = session->home_dir ? change_pwd(remote_pwd, session->home_dir) : change_pwd(remote_pwd, "/");
    if (show_FileStore(local_pwd, false) != 0) return;
    gtk_label_set_text((GtkLabel *) mainWindow->LeftInnerFrameLabel, local_pwd);
    if (show_FileStore(remote_pwd, true) != 0) return;
    gtk_label_set_text((GtkLabel *) mainWindow->RightInnerFrameLabel, remote_pwd);
  } else {
    // Some error happened
    transition_MessageWindow(INFO_ERROR, session->message);
  }
}


/* Button actions */

void LeftFileHomeButton_action(__attribute__((unused)) GtkButton *LeftFileHomeButton) {
  char *home = get_home_dir();
  if (home) {
    local_pwd = change_pwd(local_pwd, home);
    free(home);
    if (show_FileStore(local_pwd, false) == 0) {
      gtk_label_set_text((GtkLabel *) mainWindow->LeftInnerFrameLabel, local_pwd);
    } else {
      transition_MessageWindow(INFO_ERROR, session->message);
    }
  }
}

void LeftFileBackButton_action(__attribute__((unused)) GtkButton *LeftFileBackButton) {}
void LeftNewFolderButton_action(__attribute__((unused)) GtkButton *LeftNewFolderButton) {}

void RightFileHomeButton_action(__attribute__((unused)) GtkButton *RightFileHomeButton) {
  if (session->home_dir) {
    remote_pwd = change_pwd(remote_pwd, session->home_dir);
    if (show_FileStore(remote_pwd, true) == 0) {
      gtk_label_set_text((GtkLabel *) mainWindow->RightInnerFrameLabel, remote_pwd);
    } else {
      transition_MessageWindow(INFO_ERROR, session->message);
    }
  }
}

void RightFileBackButton_action(__attribute__((unused)) GtkButton *RightFileBackButton) {}
void RightNewFolderButton_action(__attribute__((unused)) GtkButton *RightNewFolderButton) {}

void QuitButton_action(__attribute__((unused)) GtkButton *QuitButton) {
  quitUI();
}

void ConnectButton_action(__attribute__((unused)) GtkButton *ConnectButton) {
  // get ip, username (and password)
  const char *ip = (char *) gtk_entry_get_text((GtkEntry*) connectWindow->SetIPEntry);
  const char *username = (char *) gtk_entry_get_text((GtkEntry*) connectWindow->SetUsernameEntry);
  const char *password = (char *) gtk_entry_get_text((GtkEntry*) connectWindow->SetPasswordEntry);
  // Try to connect using create_session() and autheticate_init()
  session = create_session(username, ip);
  if (session == NULL) {
    transition_MessageWindow(INFO_ERROR, get_error(SSH_CREATE_ERROR));
    return;
  }
  enum AuthenticationAction result = authenticate_init(session);
  switch (result) {
    case AUTHENTICATION_OK:
      transition_MainWindow();
      break;
    case AUTHENTICATION_PASSWORD_NEEDED:
      if (authenticate_password(session, password) == AUTHENTICATION_OK) {
        transition_MainWindow();
      } else {
        transition_MessageWindow(INFO_ERROR, session->message);
      }
      break;
    case AUTHENTICATION_ASK:
      transition_MessageWindow(ASK_SSH, session->message);
      break;
    case AUTHENTICATION_ERROR:
      transition_MessageWindow(INFO_ERROR, session->message);
    default:
      return;
  }
}

void CancelButton_action(__attribute__((unused)) GtkButton *CancelButton) {
  gtk_widget_hide(messageWindow->MessageDialog);
}

void OkButton_action(__attribute__((unused)) GtkButton *OkButton) {
  if (messageWindow->messageType == ASK_SSH) {
    // User accepted the ssh key
    // Try to connect using authenticate_key(Session *session, const enum AuthenticationAction action)
    const char *password = (char *) gtk_entry_get_text((GtkEntry*) connectWindow->SetPasswordEntry);
    enum AuthenticationAction result = authenticate_key(session, AUTHENTICATION_ACCEPT);
    switch(result) {
      case AUTHENTICATION_OK:
        transition_MainWindow();
        break;
      case AUTHENTICATION_PASSWORD_NEEDED:
        if (authenticate_password(session, password) == AUTHENTICATION_OK) {
          transition_MainWindow();
        } else {
          transition_MessageWindow(INFO_ERROR, session->message);
        }
        break;
      case AUTHENTICATION_ERROR:
        transition_MessageWindow(INFO_ERROR, session->message);
      default:
        return;
    }
  } else {
    close_MessageWindow();
  }
}

gboolean FileView_OnButtonPress(GtkWidget *widget, GdkEvent *event, __attribute__((unused)) gpointer user_data) {
  if (event->type == GDK_2BUTTON_PRESS) {
    printf("2nd button press\n");
    GdkEventButton *press = (GdkEventButton *) event;
    GtkTreePath *path = gtk_icon_view_get_path_at_pos((GtkIconView *) widget, press->x, press->y);
    gchar *filename;
    unsigned filetype;
    GtkTreeIter it;
        if (widget == mainWindow->LeftFileView) {
      gtk_tree_model_get_iter((GtkTreeModel *) localFileStore->listStore, &it, path);
      gtk_tree_model_get((GtkTreeModel *) localFileStore->listStore, &it,
                                          STRING_COLUMN, &filename,
                                          UINT_COLUMN, &filetype,
                                          -1);
    } else {
      gtk_tree_model_get_iter((GtkTreeModel *) remoteFileStore->listStore, &it, path);
      gtk_tree_model_get((GtkTreeModel *) remoteFileStore->listStore, &it,
                                          STRING_COLUMN, &filename,
                                          UINT_COLUMN, &filetype,
                                          -1);
    }

    printf("filename: %s, filetype: %d\n", (char *) filename, filetype);
    g_free(filename);
    return TRUE;
  }
  return FALSE;
}


/*  File handling */

FileStore *update_FileStore(FileStore *fileStore, const char *dir_name, bool remote) {
  if (!fileStore) {
    // Create new FileStore
    fileStore = malloc(sizeof(FileStore));
    fileStore->listStore = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_UINT);
    fileStore->files = NULL;
  }
  gtk_list_store_clear(fileStore->listStore);
  // List all the files
  fileStore->files = remote ? sftp_session_ls_dir(session, fileStore->files, dir_name)
                            : ls_dir(fileStore->files, dir_name);
  if (!fileStore->files) {
    // Some error happened
    clear_FileStore(fileStore);
    return NULL;
  }
  // Append all entries
  iterate_FileList(fileStore->files, add_FileStore, (void *) fileStore);
  return fileStore;
}

void add_FileStore(struct File *file, void *ptr) {
    FileStore *fileStore = (FileStore *) ptr;
    gtk_list_store_append(fileStore->listStore, &(fileStore->it));
    gtk_list_store_set( fileStore->listStore, &(fileStore->it),
                        STRING_COLUMN, (GValue *) file->name,
                        PIXBUF_COLUMN, (GValue *) get_Icon_filetype(file->type),
                        UINT_COLUMN, file->type,
                        -1);
}

void clear_FileStore(FileStore *fileStore) {
  if (fileStore) {
    gtk_list_store_clear(fileStore->listStore);
    clear_Filelist(fileStore->files);
    free(fileStore);
  }
}

int show_FileStore(const char *pwd, const bool remote) {
  if (remote) {
    if ((remoteFileStore = update_FileStore(remoteFileStore, pwd, true)) != NULL) {
      gtk_icon_view_set_model((GtkIconView *) mainWindow->RightFileView, (GtkTreeModel *) remoteFileStore->listStore);
      gtk_icon_view_set_text_column((GtkIconView *) mainWindow->RightFileView, STRING_COLUMN);
      gtk_icon_view_set_pixbuf_column((GtkIconView *) mainWindow->RightFileView, PIXBUF_COLUMN);
      gtk_widget_show_all(mainWindow->TopWindow);
    } else {
      Session_message(session, get_error(ERROR_DISPLAYING_REMOTE_FILES));
      transition_MessageWindow(INFO_ERROR, session->message);
      return -1;
    }
  } else {
    if ((localFileStore = update_FileStore(localFileStore, pwd, false)) != NULL) {
      gtk_icon_view_set_model((GtkIconView *) mainWindow->LeftFileView, (GtkTreeModel *) localFileStore->listStore);
      gtk_icon_view_set_text_column((GtkIconView *) mainWindow->LeftFileView, STRING_COLUMN);
      gtk_icon_view_set_pixbuf_column((GtkIconView *) mainWindow->LeftFileView, PIXBUF_COLUMN);
      gtk_widget_show_all(mainWindow->TopWindow);
    } else {
      Session_message(session, get_error(ERROR_DISPLAYING_LOCAL_FILES));
      transition_MessageWindow(INFO_ERROR, session->message);
      return -1;
    }
  }
  return 0;
}
