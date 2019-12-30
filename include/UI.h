/**
  *   @file UI.h
  *   @author Lauri Westerholm
  *   @brief UI created using GTK+, header
  */


#ifndef UI_HEADER
#define UI_HEADER

#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "ssh.h"

#define LAYOUT_PATH "../layout/FileManagerUI.glade" /** Path to the glade file */

// UI top-level windows

/**
  *   @struct MainWindow
  *   @brief Contains TopWindow and its child UI elements
  */
typedef struct {
  GtkWidget *TopWindow; /**< Top-level element, @see FileManagerUI.glade TopWindow */
    GtkWidget *TopGrid; /**< @see FileManagerUI.glade TopGrid */
      // left-hand side
      GtkWidget *LeftTopFrame;  /**< @see FileManagerUI.glade LeftTopFrame */
        GtkWidget *LeftTopFrameLabel; /**< @see FileManagerUI.glade LeftTopFrameLabel */
        GtkWidget *LeftTopFrameAlignment; /**< @see FileManagerUI.glade LeftTopFrameAlignment */
          GtkWidget *LeftInnerFrame;  /**< @see FileManagerUI.glade LeftInnerFrame */
            GtkWidget *LeftInnerFrameLabel; /**< @see FileManagerUI.glade LeftInnerFrameLabel */
            GtkWidget *LeftInnerFrameAlignment; /**< @see FileManagerUI.glade LeftInnerFrameAlignment */
              GtkWidget *LeftFileSelectFrame; /**< @see FileManagerUI.glade LeftFileSelectFrame */
                GtkWidget *LeftFileSelectFrameAlignment;  /**< @see FileManagerUI.glade LeftFileSelectFrameAlignment */
                  GtkWidget *LeftFileScrollWindow;  /**< @see FileManagerUI.glade LeftFileScrollWindow */
                    GtkWidget *LeftFileScrollWindowViewPort;  /**< @see FileManagerUI.glade LeftFileScrollWindowViewPort */
                      GtkWidget *LeftFileView;  /**< @see FileManagerUI.glade LeftFileView */
                GtkWidget *LeftFileSelectGrid;  /**< @see FileManagerUI.glade LeftFileSelectGrid */
                  GtkWidget *LeftFileHomeButton;  /**< @see FileManagerUI.glade LeftFileHomeButton */
                  GtkWidget *LeftFileBackButton;  /**< @see FileManagerUI.glade LeftFileBackButton */
                  GtkWidget *LeftNewFolderButton; /**< @see FileManagerUI.glade LeftNewFolderButton */
      // right-hand side
      GtkWidget *RightTopFrame; /**< @see FileManagerUI.glade RightTopFrame */
        GtkWidget *RightTopFrameLabel;  /**< @see FileManagerUI.glade RightTopFrameLabel */
        GtkWidget *RightTopFrameAlignment;  /**< @see FileManagerUI.glade RightTopFrameAlignment */
          GtkWidget *RightInnerFrame; /**< @see FileManagerUI.glade RightInnerFrame */
            GtkWidget *RightInnerFrameLabel;  /**< @see FileManagerUI.glade RightInnerFrameLabel */
            GtkWidget *RightInnerFrameAlignment;  /**< @see FileManagerUI.glade RightInnerFrameAlignment */
              GtkWidget *RightFileSelectFrame;  /**< @see FileManagerUI.glade RightFileSelectFrame */
                GtkWidget *RightFileSelectFrameAlignment; /**< @see FileManagerUI.glade RightFileSelectFrameAlignment */
                  GtkWidget *RightFileScrollWindow; /**< @see FileManagerUI.glade RightFileScrollWindow */
                    GtkWidget *RightScrollWindowViewPort; /**< @see FileManagerUI.glade RightScrollWindowViewPort */
                      GtkWidget *RightFileView; /**< @see FileManagerUI.glade RightFileView */
                GtkWidget *RightFileSelectGrid; /**< @see FileManagerUI.glade RightFileSelectGrid */
                  GtkWidget *RightFileHomeButton; /**< @see FileManagerUI.glade RightFileHomeButton */
                  GtkWidget *RightFileBackButton; /**< @see FileManagerUI.glade RightFileBackButton */
                  GtkWidget *RightNewFolderButton;  /**< @see FileManagerUI.glade RightNewFolderButton */
} MainWindow;

/**
  *   @struct ConnectWindow
  *   @brief Contains ConnectDialog and its child UI elements
  */
typedef struct {
  GtkWidget *ConnectDialog; /**< Top-level dialog, @see FileManagerUI.glade ConnectDialog */
    GtkWidget *ConnectDialogBox;  /**< @see FileManagerUI.glade ConnectDialogBox */
      GtkWidget *ConnectWindowLabel;  /**< @see FileManagerUI.glade ConnectWindowLabel */
      GtkWidget *SetIPLabel;  /**< @see FileManagerUI.glade SetIPLabel */
      GtkWidget *SetIPEntry;  /**< @see FileManagerUI.glade SetIPEntry */
      GtkWidget *SetUsernameLabel;  /**< @see FileManagerUI.glade SetUsernameLabel */
      GtkWidget *SetUsernameEntry;  /**< @see FileManagerUI.glade SetUsernameEntry */
      GtkWidget *SetPasswordLabel;  /**< @see FileManagerUI.glade SetPasswordLabel */
      GtkWidget *SetPasswordEntry;  /**< @see FileManagerUI.glade SetPasswordEntry */
      GtkWidget *ConnectDialogButtonBox;  /**< @see FileManagerUI.glade ConnectDialogButtonBox */
        GtkWidget *QuitButton;  /**< @see FileManagerUI.glade QuitButton */
        GtkWidget *ConnectButton; /**< @see FileManagerUI.glade ConnectButton */
} ConnectWindow;

/**
  *   @enum MessageType
  *   @brief Describes which type of a message MessageWindow is showing
  */
enum MessageType {
  ASK_SSH, /**< A message contains a ssh key which needs user interaction */
  INFO_ERROR /**< A message is either an info or an error message */
};

/**
  *   @struct MessageWindow
  *   @brief Contains MessageDialog and its child UI elements
  */
typedef struct {
  enum MessageType messageType; /**< MessageDialog actions differ based on the type */
  GtkWidget *MessageDialog; /**< Top-level dialog, @see FileManagerUI.glade MessageDialog */
    GtkWidget *MessageDialogBox;  /**< @see FileManagerUI.glade MessageDialogBox */
      GtkWidget *MessageLabel;  /**< @see FileManagerUI.glade MessageLabel */
      GtkWidget *MessageButtonBox;  /**< @see FileManagerUI.glade MessageButtonBox */
        GtkWidget *CancelButton;  /**< @see FileManagerUI.glade CancelButton */
        GtkWidget *OkButton;  /**< @see FileManagerUI.glade OkButton */
} MessageWindow;


// Global variables
GtkBuilder *builder; /** GtkBuilder used to create all the windows */
MainWindow *mainWindow; /** Pointer to the main window instance */
MessageWindow *messageWindow; /** Pointer to a message window */
ConnectWindow *connectWindow; /** Pointer to the connect window */
Session *session; /**< SSH Session pointer */

/**
  *   @brief Initialize UI from the glade file
  *   @remark Updates global structs
  */
void initUI();

/**
  *   @brief Quit UI event loop and destroy all UI elements
  */
void quitUI();

/**
  *   @brief Init mainWindow
  *   @remark Inteded to be called only once from initUI
  */
void init_MainWindow();

/**
  *   @brief Init connectWindow
  *   @remark Intended to be called only once from initUI
  */
void init_ConnectWindow();

/**
  *   @brief Init messageWindow
  *   @remark Intended to be called only once from initUI. This does not set
  *   messageWindow.messageType
  */
void init_MessageWindow();

/**
  *   @brief Adjust messageWindow to show correct message and MessageType
  *   @param messageType Apropriate MessageType: CancelButton only shown when
  *   MessageType.ASK_SSH
  *   @param message To be displayed
  *   @remark Call this always when messageWindow needs to be shown
  */
void adjust_MessageWindow(const enum MessageType messageType, const char *message);


/* Button actions */
void LeftFileHomeButton_action();
void LeftFileBackButton_action();
void LeftNewFolderButton_action();
void RightFileHomeButton_action();
void RightFileBackButton_action();
void RightNewFolderButton_action();

/**
  *   @brief Quit from the ConnectWindow
  */
void QuitButton_action();
/**
  *   @brief Try to connect via SSH to the provided remote
  *   @details When connection succeeds mainWindow is shown. If user needs to
  *   confirm the remote puclic key, messageWindow is shown (MessageType.ASK_SSH).
  *   In case of an error, messageWindow is shown (MessageType.INFO_ERROR)
  */
void ConnectButton_action();


/**
  *   @brief Cancel previous action
  *   @remark Used to reject a public ssh key after which the messageWindow
  *   should be closed and connectWindow activated
  */
void CancelButton_action();
/**
  *   @brief Proceed / accept action
  *   @remark Used to accept a puclic ssh key after which the mainWindow is
  *   activated and messageWindow & connectWindow closed. Can be also used to
  *   display info/error messages and to return to connectWindow
  */
void OkButton_action();



#endif // end UI_HEADER
