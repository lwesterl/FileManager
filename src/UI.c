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
  mainWindow->LeftFileScrollWindowViewPort = GTK_WIDGET(gtk_builder_get_object(builder, "LeftFileScrollWindowViewPort"));
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
  if (init_sftp_session(session) == 0) {
    close_MessageWindow();
    gtk_widget_hide(connectWindow->ConnectDialog);
    gtk_widget_show_all(mainWindow->TopWindow);
  } else {
    // Some error happened
    transition_MessageWindow(INFO_ERROR, session->message);
  }
}


/* Button actions */
void LeftFileHomeButton_action(__attribute__((unused)) GtkButton *LeftFileHomeButton) {}
void LeftFileBackButton_action(__attribute__((unused)) GtkButton *LeftFileBackButton) {}
void LeftNewFolderButton_action(__attribute__((unused)) GtkButton *LeftNewFolderButton) {}
void RightFileHomeButton_action(__attribute__((unused)) GtkButton *RightFileHomeButton) {}
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
