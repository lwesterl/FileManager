/**
  *   @file UI.h
  *   @author Lauri Westerholm
  *   @brief UI created using GTK+, header
  */


#ifndef UI_HEADER
#define UI_HEADER

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gmodule.h> // Linked list implementation, GSList

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "ssh.h"
#include "fs.h"
#include "assets.h"

#define LAYOUT_PATH "../layout/FileManagerUI.glade" /** Path to the glade file */

// UI top-level windows

/**
  *   @struct ContextMenu
  *   @brief Contains context menu related elements
  */
typedef struct {
  GtkMenu *Menu; /**< Context menu opened by right click, @see ContextMenuActions */
  GdkRectangle *ContextMenuRect; /**< GdkRectangle which specifies location of the ContextMenu */
  GtkWidget *ContextMenuEmitter; /**< Store the widget that emitted ContextMenu popup, LeftFileView or RightFileView */

  GtkMenuItem *copy; /**< GtkMenuItem triggers file copying operation */
  GtkMenuItem *paste; /**< GtkMenuItem triggers file pasting operation */
  GtkMenuItem *rename; /**< GtkMenuItem triggers file rename operation */
  GtkMenuItem *create_folder; /**< GtkMenuItem triggers create folder operation */
} ContextMenu;

/**
  *   @enum ContextMenuActions
  *   @brief Available actions in context menus
  */
enum ContextMenuActions {
  COPY, /**< Copy selected files */
  PASTE, /**< Paste selected files */
  RENAME, /**< Rename selected files */
  CREATE_FOLDER /**< Create new folder */
};

/**< String names for ContextMenuActions */
static const char* const ContextMenuAction_names[] =
{
  "Copy",
  "Paste",
  "Rename",
  "Create new folder"
};

/**
  *   @brief Get matching string for ContextMenuActions
  *   @param action ContextMenuAction
  *   @return Correct entry from ContextMenuAction_names
  */
static inline const char* get_ContextMenuAction_name(const enum ContextMenuActions action) {
  return ContextMenuAction_names[action];
}

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
                    GtkWidget *LeftScrollWindowViewPort;  /**< @see FileManagerUI.glade LeftFileScrollWindowViewPort */
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

      ContextMenu *contextMenu;
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

/**
  *   @struct FileStore
  *   @brief Used to store displayed files
  */
typedef struct {
  GtkListStore *listStore; /**< Store displayed files */
  GtkTreeIter it;  /**< Iterator to the GtkListStore */
  GSList *files; /**< Linked list storing matching file details, @see File */
} FileStore;

enum {
  STRING_COLUMN, /**< Used for GtkListStore, the first column: 0 */
  PIXBUF_COLUMN, /**< Used for GtkListStore, the second column: 1 */
  UINT_COLUMN, /**< Used for GtkListStore, the third column: 2 */
  N_COLUMNS /**< Used for GtkListStore, amount of columns: 3 */
};


// Global variables
GtkBuilder *builder; /** GtkBuilder used to create all the windows */
MainWindow *mainWindow; /** Pointer to the main window instance */
MessageWindow *messageWindow; /** Pointer to a message window */
ConnectWindow *connectWindow; /** Pointer to the connect window */
Session *session; /**< SSH Session pointer */
FileStore *remoteFileStore; /**< Used to store displayed elements which correspond to RemoteFiles */
FileStore *localFileStore; /**< Uses to store displayed elements which correspond to LocalFiles */

/* UI initialization */
/**
  *   @brief Initialize UI from the glade file
  *   @param argc Number of command line arguments
  *   @param argv Command line arguments
  *   @remark Updates global structs
  */
void initUI(int argc, char *argv[]);

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
  *   @brief Init ContextMenu
  *   @remark Intended to be called only once from init_MainWindow
  */
void init_ContextMenu();

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
  *   @brief Close messageWindow
  */
void close_MessageWindow();

/**
  *   @brief Clear ContextMenu
  *   @remark Intended to be called from quitUI
  */
void clear_ContextMenu();


/* Window transitions */

/**
  *   @brief Transition to messageWindow and show correct message and MessageType
  *   @param messageType Apropriate MessageType: CancelButton only shown when
  *   MessageType.ASK_SSH
  *   @param message To be displayed
  *   @remark Call this always when messageWindow needs to be shown
  */
void transition_MessageWindow(const enum MessageType messageType, const char *message);

/**
  *   @brief Transition from ConnectWindow to MainWindow
  */
void transition_MainWindow();

/**
  *   @brief Transition/show ContextMenu
  *   @remark On the first transition the context menu is also initialized
  *   @param widget LeftFileView or RightFileView from which the event was emitted
  *   @param event Event which triggered the transition, callback event
  */
gboolean transition_ContextMenu(GtkWidget *widget, GdkEvent *event);

/**
  *   @brief Show correct ContextMenu buttons
  *   @param file_selected Boolean whether context menu is show for a selected file
  */
void show_ContextMenu_buttons(bool file_selected);


/* Button actions */

/**
  *   @brief Update localFileStore to show files in local home directory
  *   @param LeftFileHomeButton Not used
  */
void LeftFileHomeButton_action(GtkButton *LeftFileHomeButton);

/**
  *   @brief Update localFileStore to show files in the parent directory
  *   @param LeftFileBackButton Not used
  */
void LeftFileBackButton_action(GtkButton *LeftFileBackButton);
void LeftNewFolderButton_action(GtkButton *LeftNewFolderButton);

/**
  *   @brief Update remoteFileStore to show files in remote home directory
  *   @param RightFileHomeButton Not used
  */
void RightFileHomeButton_action(GtkButton *RightFileHomeButton);

/**
  *   @brief Update remoteFileStore to show files in the parent directory
  *   @param RightFileBackButton Not used
  */
void RightFileBackButton_action(GtkButton *RightFileBackButton);
void RightNewFolderButton_action(GtkButton *RightNewFolderButton);

/**
  *   @brief Quit from the ConnectWindow
  *   @param QuitButton Button clicked, unused
  */
void QuitButton_action(GtkButton *QuitButton);
/**
  *   @brief Try to connect via SSH to the provided remote
  *   @param ConnectButton Button clicked, unused
  *   @details When connection succeeds mainWindow is shown. If user needs to
  *   confirm the remote puclic key, messageWindow is shown (MessageType.ASK_SSH).
  *   In case of an error, messageWindow is shown (MessageType.INFO_ERROR)
  */
void ConnectButton_action(GtkButton *ConnectButton);

/**
  *   @brief Cancel previous action
  *   @param CancelButton Button clicked, unused
  *   @remark Used to reject a public ssh key after which the messageWindow
  *   should be closed and connectWindow activated
  */
void CancelButton_action(GtkButton *CancelButton);
/**
  *   @brief Proceed / accept action
  *   @param OkButton Button clicked, unused
  *   @remark Used to accept a puclic ssh key after which the mainWindow is
  *   activated and messageWindow & connectWindow closed. Can be also used to
  *   display info/error messages and to return to connectWindow
  */
void OkButton_action(GtkButton *OkButton);

/**
  *   @brief Called after a FileView icon is pressed
  *   @param widget Which widget (FileView created the event)
  *   @param event Emitted after icon is pressed
  *   @param user_data Possible addition data, not used
  *   @return TRUE when the event should not be further processed, otherwise FALSE
  */
gboolean FileView_OnButtonPress(GtkWidget *widget, GdkEvent *event, gpointer user_data);

/**
  *   @brief Action which is attached as callback to ContextMenuItems
  *   @param menuItem Item which emitted the action
  *   @param user_data Additional data, not used
  */
void ContextMenuItem_action(GtkMenuItem *menuItem, gpointer user_data);


/*  File handling */

/**
  *   @brief Update fileStore to contains contents of GSList which holds struct File entries
  *   @param fileStore FileStore to be updated
  *   @param dir_name Target directory name (i.e. which files should be displayed)
  *   @param remote Whether this is a remote filesystem (connect using sftp)
  *   @return Valid pointer on success, otherwise NULL
  */
FileStore *update_FileStore(FileStore *fileStore, const char *dir_name, const bool remote);

/**
  *   @brief Add entry to FileStore
  *   @remark This is called from update_FileStore via iterate_FileList. This
  *   does not add "." and ".." filenames to FileStore
  *   @param file struct File instance
  *   @param ptr Pointer to a FileStore passed as void *
  */
void add_FileStore(struct File *file, void *ptr);

/**
  *   @brief Clear fileStore contents and free allocated memory
  *   @param fileStore To be cleared
  */
void clear_FileStore(FileStore *fileStore);

/**
  *   @brief Update FileStore to show correct files
  *   @param pwd Present working directory, @see assets.h
  *   @param remote Whether the corresponding FileStore is remote or local
  *   @return 0 on success, -1 on error (sets error using Session_message and
  *   transitions to MessageWindow)
  */
int show_FileStore(const char *pwd, bool remote);

/**
  *   @brief Update FileViews to show updated FileStores
  *   @remark This is basically only a wrapper for show_FileStore
  *   @param remote Whether to update remote or local FileStore
  */
void update_FileView(bool remote);


#endif // end UI_HEADER
