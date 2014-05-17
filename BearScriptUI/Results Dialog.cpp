//
// Results Dialog.cpp : Container dialog for a searchable virtual grouped ListView
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

// ListView column IDs
#define    COMMANDS_COLUMN_NAME               0
#define    OBJECT_COLUMN_NAME                 0
#define    OBJECT_COLUMN_TYPE                 1

#define    RESULTS_DELAY_TICKER               42

// OnException: Display in output
#define  ON_EXCEPTION()    displayException(pException);

// Debug
static const TCHAR*  szPageNames[4] = { TEXT("Commands"), TEXT("Game Objects"), TEXT("Script Objects"), TEXT("None") };

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createResultsDialog
// Description     : Creates one of the three search results child dialogs
// 
// HWND               hParentWnd : [in] Parent window
// CONST RESULT_TYPE  eType      : [in] Type of results to display
// 
// Return type : Window handle of the new dialog if successful, otherwise NULL
// 
HWND  createResultsDialog(HWND  hParentWnd, CONST RESULT_TYPE  eType)
{
   RESULTS_DIALOG_DATA*  pDialogData = createResultsDialogData(eType);
   HWND                  hDialog;
   
   TRY
   {
      /// Create ResultsDialog
      hDialog = loadDialog(TEXT("RESULTS_DIALOG"), hParentWnd, dlgprocResultsDialog, (LPARAM)pDialogData);
      ERROR_CHECK("creating search results dialog", hDialog);
      //DEBUG_WINDOW("Results Dialog", hDialog);

      // [CHECK] Cleanup if dialog failed to create
      if (!hDialog)
         deleteResultsDialogData(pDialogData);

      // Return dialog handle
      return hDialog;
   }
   CATCH1("Unable to create ResultsDialog for '%s'", szPageNames[eType]);
   return NULL;
}



/// Function name  : createResultsDialogData
// Description     : Creates dialog data for the SearchResults dialog
// 
// CONST RESULT_TYPE  eType : [in] Type of results to display
// 
// Return Value   : New dialog data, you are responsible for destroying it
// 
RESULTS_DIALOG_DATA*  createResultsDialogData(CONST RESULT_TYPE  eType)
{
   RESULTS_DIALOG_DATA*  pDialogData = utilCreateEmptyObject(RESULTS_DIALOG_DATA);
   
   // Set properties
   pDialogData->eType = eType;

   // Return new object
   return pDialogData;
}


/// Function name  : createResultsDialogTree
// Description     : Creates the appropriate tree for the dialog type
// 
// const RESULT_TYPE  eType   : [in] Dialog tree
// 
// Return Value   : Empty tree
// 
AVL_TREE*  createResultsDialogTree(const RESULT_TYPE  eType)
{
   AVL_TREE*  pTree = NULL;

   /// Create appropriate tree type and perform operation on appropriate GameData
   switch (eType)
   {
   // [COMMANDS] Run on CommandSyntax by ID
   case RT_COMMANDS:       pTree = createAVLTree(extractCommandSyntaxTreeNode, NULL, createAVLTreeSortKey(AK_GROUP, AO_ASCENDING), createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL);  break;
   // [GAME OBJECTS] Run on GameObjects by Group/MainType
   case RT_GAME_OBJECTS:   pTree = createAVLTree(extractGameObjectTreeNode, NULL, createAVLTreeSortKey(AK_GROUP, AO_ASCENDING), createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL);     break;
   // [SCRIPT OBJECTS] Run on ScriptObjects by Group
   case RT_SCRIPT_OBJECTS: pTree = createAVLTree(extractScriptObjectTreeNode, NULL, createAVLTreeSortKey(AK_GROUP, AO_ASCENDING), createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL);   break;
   }

   return pTree;
}

/// Function name  : deleteResultsDialogData
// Description     : Destroys ResultsDialog window data
// 
// RESULTS_DIALOG_DATA*  &pDialogData   : [in] 
// 
VOID  deleteResultsDialogData(RESULTS_DIALOG_DATA*  &pDialogData)
{
   TRY
   {
      // Destroy search results tree
      if (pDialogData->pResultsTree)
         deleteAVLTree(pDialogData->pResultsTree);

      // Delete calling object
      utilDeleteObject(pDialogData);
   }
   CATCH0("Unable to destroy dialog data");
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getResultsDialogData
// Description     : Retrieve window data
// 
// HWND  hDialog   : [in] Results dialog
// 
// Return Value   : ResultsDialog data
// 
RESULTS_DIALOG_DATA*  getResultsDialogData(HWND  hDialog)
{
   return (RESULTS_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
}


/// Function name  : identifyResultsDialogFilter
// Description     : Retrieves the currently selected filter
// 
// HWND  hDialog   : [in] Results dialog
// 
// Return Value   : Filter index
// 
UINT  identifyResultsDialogFilter(HWND  hDialog)
{
   return ComboBox_GetCurSel(GetControl(hDialog, IDC_RESULTS_FILTER));
}


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findResultsDialogItemByIndex
// Description     : Lookup item data for a result 
// 
// RESULTS_DIALOG_DATA*  pDialogData   : [in]     Dialog data
// CONST INT             iLogicalIndex : [in]     Logical item index
// SUGGESTION_RESULT&    xOutput       : [in/out] Item data
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL   findResultsDialogItemByIndex(RESULTS_DIALOG_DATA*  pDialogData, CONST INT  iLogicalIndex, SUGGESTION_RESULT&  xOutput)
{
   // Prepare
   xOutput.asPointer = NULL;

   // Lookup item data
   if (iLogicalIndex != -1)
      findObjectInAVLTreeByIndex(pDialogData->pResultsTree, iLogicalIndex, xOutput.asPointer);
   
   // Return TRUE if found
   return (xOutput.asPointer != NULL);
}


/// Function name  : initResultsDialog
// Description     : Initialises the search results dialog appropriately, depending on its type
//
// RESULTS_DIALOG_DATA*  pDialogData : [in] Dialog data
// HWND                  hDialog     : [in] Results dialog
// 
VOID   initResultsDialog(RESULTS_DIALOG_DATA*  pDialogData, HWND  hDialog)
{
   TRY
   {
      /// Store dialog data
      pDialogData->hDialog = hDialog;
      SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);

      // Define Groups to display
      switch (pDialogData->eType)
      {
      // [COMMAND LIST] Use all of the CommandSyntax groups except the last : 'Hidden Commands'
      case RT_COMMANDS:
         pDialogData->iFirstGroupResourceID = FIRST_COMMAND_GROUP_RESOURCE;
         pDialogData->iFirstGroupID         = FIRST_COMMAND_GROUP;
         pDialogData->iLastGroupID          = LAST_COMMAND_GROUP;
         break;

      // [GAME OBJECTS] Use all of the GameObjects groups
      case RT_GAME_OBJECTS:
         pDialogData->iFirstGroupResourceID = FIRST_GAME_OBJECT_GROUP_NAME;
         pDialogData->iFirstGroupID         = FIRST_GAME_OBJECT_GROUP;
         pDialogData->iLastGroupID          = LAST_GAME_OBJECT_GROUP;
         break;

      // [SCRIPT OBJECTS] Use all of the ScriptObject groups except the last : 'Operators'
      case RT_SCRIPT_OBJECTS:
         pDialogData->iFirstGroupResourceID = FIRST_SCRIPT_OBJECT_GROUP_NAME;
         pDialogData->iFirstGroupID         = FIRST_SCRIPT_OBJECT_GROUP;
         pDialogData->iLastGroupID          = LAST_SCRIPT_OBJECT_GROUP;
         break;
      }

      /// Initialise controls
      initResultsDialogControls(pDialogData);

      /// Create tooltips
      pDialogData->hTooltip = createTooltipWindow(hDialog);
      addTooltipTextToControl(pDialogData->hTooltip, hDialog, IDC_RESULTS_SEARCH);
      addTooltipTextToControl(pDialogData->hTooltip, hDialog, IDC_RESULTS_FILTER);
   }
   CATCH1("Unable to initialise '%s' dialog", szPageNames[pDialogData->eType]);
}


/// Function name  : initResultsDialogControls
// Description     : Initialises the GroupedListView and FilterCombo with the details of the chosen groups
// 
// RESULTS_DIALOG_DATA*  pDialogData   : [in] Dialog data
// 
VOID  initResultsDialogControls(RESULTS_DIALOG_DATA*  pDialogData)
{
   CONST TCHAR*      szTypeIcons[3]        = { TEXT("COMMAND_LIST_ICON"), TEXT("GAME_OBJECT_ICON"), TEXT("SCRIPT_OBJECT_ICON") };    // Icon used in the Filter ComboBox
   LISTVIEW_COLUMNS  oCommandListView      = { 1, IDS_COMMAND_LIST_COLUMN_NAME, 400, NULL, NULL, NULL, NULL };       // CommandList column data
   LISTVIEW_COLUMNS  oObjectNameListView   = { 2, IDS_AUTOCOMPLETE_COLUMN_NAME, 200, 100,  NULL, NULL, NULL };       // GameObject/ScriptObject column data
   LISTVIEW_GROUP*   pGroupData;           // GroupedListView group
   HWND              hListViewTooltip;     // ListView tooltip

   TRY
   {
      // Prepare
      pDialogData->hFilterCombo = GetControl(pDialogData->hDialog, IDC_RESULTS_FILTER);
      pDialogData->hListView    = GetControl(pDialogData->hDialog, IDC_RESULTS_LIST);
      hListViewTooltip          = ListView_GetToolTips(pDialogData->hListView);

      /// [GROUP-LIST] Add ListView columns and ImageLists
      initReportModeListView(pDialogData->hListView, (pDialogData->eType == RT_COMMANDS ? &oCommandListView : &oObjectNameListView), FALSE);

      // [LIST] Set tooltip display times
      Tooltip_SetDisplayTime(hListViewTooltip, TTDT_INITIAL, getAppPreferences()->iSearchResultTooltipDelay * 1000);   // Wait x seconds before display
      Tooltip_SetDisplayTime(hListViewTooltip, TTDT_RESHOW,  2 * 1000);   // Wait 2 seconds between displays
      Tooltip_SetDisplayTime(hListViewTooltip, TTDT_AUTOPOP, 30 * 1000);  // Display for 30 seconds
      // Enable/Disable tooltips
      Tooltip_Activate(hListViewTooltip, getAppPreferences()->bSearchResultTooltips);
      
      /// [DEBUG]
      /*Tooltip_SetDisplayTime(ListView_GetToolTips(hListView), TTDT_INITIAL, 300); 
      Tooltip_SetDisplayTime(ListView_GetToolTips(hListView), TTDT_RESHOW,  300); */
      
      // Request ListView Tooltips
      ListView_SetExtendedListViewStyleEx(pDialogData->hListView, LVS_EX_INFOTIP, LVS_EX_INFOTIP);
      
      // Add 'Show All' to filter
      appendCustomComboBoxItem(pDialogData->hFilterCombo, TEXT("(Show All)"));

      /// [LIST/COMBO] Define GroupedListView groups and insert Filter group names
      for (UINT iGroupID = pDialogData->iFirstGroupID; iGroupID <= pDialogData->iLastGroupID; iGroupID++)
      {
         // Create group data and text
         pGroupData = createGroupedListViewGroup(iGroupID, pDialogData->iFirstGroupResourceID + (iGroupID - pDialogData->iFirstGroupID));

         // Insert group into the ComboFilter and the ListView
         appendCustomComboBoxItemEx(pDialogData->hFilterCombo, pGroupData->szName, NULL, szTypeIcons[pDialogData->eType], NULL);
         GroupedListView_AddGroup(pDialogData->hListView, pGroupData);
      }

      /// [COMBO] Setup drop-down size, and display previous filter
      ComboBox_SetMinVisible(pDialogData->hFilterCombo, 15);
      ComboBox_SetCurSel(pDialogData->hFilterCombo, getAppPreferences()->eSearchDialogFilters[pDialogData->eType]);

      /// [COMPATIBLE] Check+Enable for Commands and ScriptObjects
      utilShowDlgItem(pDialogData->hDialog, IDC_RESULTS_COMPATIBLE, pDialogData->eType != RT_GAME_OBJECTS);
      CheckDlgButton(pDialogData->hDialog, IDC_RESULTS_COMPATIBLE, pDialogData->eType != RT_GAME_OBJECTS);

      // [THEME]
      ERROR_CHECK("Enabling theme dialog texture", EnableThemeDialogTexture(pDialogData->hDialog, ETDT_ENABLETAB) == S_OK);
   }
   CATCH1("Unable to initialise controls for page '%s'", szPageNames[pDialogData->eType]);
}


/// Function name  : performResultsDialogQuery
// Description     : Builds the search results tree from items that match the specified parameters
// 
// RESULTS_DIALOG_DATA*  pDialogData  : [in]            Dialog data
// CONST TCHAR*          szSearchTerm : [in] [optional] Required text each result must contain 
///                                                         NULL indicates no search term
// CONST UINT            iGroupFilter : [in] [optional] Require group to which each result must belong
///                                                         -1 indicates no group filter
// 
VOID   performResultsDialogQuery(RESULTS_DIALOG_DATA*  pDialogData, CONST TCHAR*  szSearchTerm, CONST UINT  iGroupFilter, CONST BOOL  bCompatibleOnly)
{
   AVL_TREE_OPERATION*      pQueryOperation;     // Operation data for the query operation

   TRY
   {
      CONSOLE("Performing results dialog query on '%s': szSearchTerm='%s'. iGroupFilter=%d  bCompatible=%d", szPageNames[pDialogData->eType], szSearchTerm, iGroupFilter, bCompatibleOnly);

      // Prepare
      pQueryOperation = createAVLTreeOperation(treeprocSearchGameData, ATT_INORDER);

      /// Delete previous results
      if (pDialogData->pResultsTree)
         deleteAVLTree(pDialogData->pResultsTree);

      // Setup operation
      pQueryOperation->xFirstInput  = (LPARAM)szSearchTerm;
      pQueryOperation->xSecondInput = (LPARAM)iGroupFilter;
      pQueryOperation->xThirdInput  = (LPARAM)pDialogData;
      pQueryOperation->xFourthInput = (LPARAM)bCompatibleOnly;

      /// Create appropriate tree type and perform operation on appropriate GameData
      pQueryOperation->pOutputTree = createResultsDialogTree(pDialogData->eType);
      switch (pDialogData->eType)
      {
      // [COMMANDS] Run on CommandSyntax by ID
      case RT_COMMANDS:       performOperationOnAVLTree(getGameData()->pCommandTreeByID, pQueryOperation);        break;
      // [GAME OBJECTS] Run on GameObjects by Group/MainType
      case RT_GAME_OBJECTS:   performOperationOnAVLTree(getGameData()->pGameObjectsByMainType, pQueryOperation);  break;
      // [SCRIPT OBJECTS] Run on ScriptObjects by Group
      case RT_SCRIPT_OBJECTS: performOperationOnAVLTree(getGameData()->pScriptObjectsByGroup, pQueryOperation);   break;
      }

      /// Index new tree
      performAVLTreeIndexing(pQueryOperation->pOutputTree);
      pDialogData->pResultsTree = pQueryOperation->pOutputTree;

      // Cleanup
      deleteAVLTreeOperation(pQueryOperation);
      return;
   }
   CATCH4("Unable to perform query on page '%s'. szSearchTerm='%s'. iGroupFilter=%d  bCompatible=%d", szPageNames[pDialogData->eType], szSearchTerm, iGroupFilter, bCompatibleOnly);

   // [ERROR] Create empty tree
   pDialogData->pResultsTree = createResultsDialogTree(pDialogData->eType);
   performAVLTreeIndexing(pQueryOperation->pOutputTree);
}


/// Function name  : updateResultsDialog
// Description     : Rebuilds the ResultsTree and refreshes the ListView
// 
// HWND    hDialog : [in] Results dialog
// 
VOID   updateResultsDialog(HWND  hDialog)
{
   AVL_TREE_GROUP_COUNTER*  pGroupCounter;    // Tree GroupCounter
   RESULTS_DIALOG_DATA*     pDialogData = 0;  // Search dialog Dialog data
   TCHAR*                   szSearchTerm;     // Search term provided by the user, if any
   INT                      iGroupFilter;     // Current group filter, if any
   BOOL                     bEnabled;

   // Prepare
   TRY
   {
      pDialogData = getResultsDialogData(hDialog);
      bEnabled    = (getActiveLanguageDocument() == NULL);

      // Enable/Disable controls
      utilEnableDlgItem(hDialog, IDC_RESULTS_SEARCH, bEnabled);
      utilEnableDlgItem(hDialog, IDC_RESULTS_FILTER, bEnabled);
      utilEnableDlgItem(hDialog, IDC_RESULTS_LIST, bEnabled);
      utilEnableDlgItem(hDialog, IDC_RESULTS_COMPATIBLE, bEnabled);
      
      // [CHECK] Ensure GameData is present
      if (getAppState() != AS_GAME_DATA_LOADED)
         // [FAILURE] Empty ListView
         GroupedListView_EmptyGroups(pDialogData->hListView);

      else
      {
         // Prepare
         pGroupCounter = createAVLTreeGroupCounter(COMMAND_GROUP_COUNT);

         // [SEARCH TERM] Get current search term, and use NULL if there is none
         if (!lstrlen(szSearchTerm = utilGetDlgItemText(pDialogData->hDialog, IDC_RESULTS_SEARCH)))
            utilDeleteString(szSearchTerm);

         // [FILTER] Calculate zero-based group filter, with -1 representing 'Show All'
         iGroupFilter = ComboBox_GetCurSel(GetControl(pDialogData->hDialog, IDC_RESULTS_FILTER)) - 1;

         // [GAME VERSION] Use GameVersion of the current script if possible, otherwise the GameData
         pDialogData->eGameVersion = (getActiveScriptDocument() ? getActiveScriptDocument()->pScriptFile->eGameVersion : getAppPreferences()->eGameVersion);

         /// Build results tree and calculate group sizes
         performResultsDialogQuery(pDialogData, szSearchTerm, iGroupFilter, IsDlgButtonChecked(hDialog, IDC_RESULTS_COMPATIBLE));   
         performAVLTreeGroupCount(pDialogData->pResultsTree, pGroupCounter, AK_GROUP);

         // Iterate through all groups
         for (UINT iGroupID = pDialogData->iFirstGroupID; iGroupID <= pDialogData->iLastGroupID; iGroupID++)
            /// [GROUPS] Set new group sizes
            GroupedListView_SetGroupCount(pDialogData->hListView, iGroupID, getAVLTreeGroupCount(pGroupCounter, iGroupID));

         // Cleanup
         utilSafeDeleteString(szSearchTerm);
         deleteAVLTreeGroupCounter(pGroupCounter);
      }

      // Return
      return;
   }
   // [ERROR] Empty ListView
   CATCH1("Unable to set Grouped ListView group counts in page '%s'", szPageNames[pDialogData->eType]);
   GroupedListView_EmptyGroups(pDialogData ? pDialogData->hListView : NULL);
}


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onResultsDialog_Command
// Description     : Handles a change in search term or filter
//
// RESULTS_DIALOG_DATA*  pDialogData   : [in] Dialog data
// CONST UINT            iControlID    : [in] ID of the control
// CONST UINT            iNotification : [in] Notification message
// HWND                  hCtrl         : [in] Control sending the message
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL   onResultsDialog_Command(RESULTS_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   SUGGESTION_RESULT  xSelectedItem;
   BOOL               bResult = FALSE;

   TRY
   {
      // Examine control
      switch (iControlID)
      {
      /// [EDIT CHANGED] Initialise 500ms timer to refresh current results
      case IDC_RESULTS_SEARCH:
         if (bResult = (iNotification == EN_UPDATE))
         {
            //CONSOLE_UI(IDC_RESULTS_SEARCH, EN_UPDATE);
            KillTimer(pDialogData->hDialog, RESULTS_DELAY_TICKER);
            SetTimer(pDialogData->hDialog, RESULTS_DELAY_TICKER, 500, NULL);
         }
         // [FOCUSED]
         else if (bResult = (iNotification == EN_SETFOCUS))
            CONSOLE("ResultsDialog SearchTerm Edit receiving input focus");
         break;

      /// [FILTER CHANGED] Refresh results
      case IDC_RESULTS_FILTER:
         if (bResult = (iNotification == CBN_SELCHANGE))
         {
            CONSOLE_UI(IDC_RESULTS_FILTER, CBN_SELCHANGE);
            updateResultsDialog(pDialogData->hDialog);
         }
         // [FOCUSED]
         else if (bResult = (iNotification == CBN_SETFOCUS))
            CONSOLE("ResultsDialog Filter ComboBox receiving input focus");
         break;

      /// [COMPATIBLE CHECK CHANGED] Refresh results
      case IDC_RESULTS_COMPATIBLE:
         CONSOLE_UI(IDC_RESULTS_COMPATIBLE, BN_CLICK);

         updateResultsDialog(pDialogData->hDialog);
         bResult = TRUE;
         break;

      /// [INSERT SUGGESTION]
      case IDM_RESULTS_INSERT_RESULT:
         CONSOLE_MENU(IDM_RESULTS_INSERT_RESULT);

         // Lookup selected item
         if (bResult = findResultsDialogItemByIndex(pDialogData, ListView_GetSelected(pDialogData->hListView), xSelectedItem))
            // [FOUND] Insert result
            onResultsDialog_InsertResult(pDialogData, xSelectedItem);
         break;

      /// [MSCI LOOKUP]
      case IDM_RESULTS_MSCI_LOOKUP:       onResultsDialog_LookupCommand(pDialogData);     break;
      /// [SEARCH CONTENT]
      case IDM_RESULTS_SEARCH_CONTENT:    onResultsDialog_SearchContent(pDialogData);     break;
      /// [SUBMIT CORRECTION]
      case IDM_RESULTS_SUBMIT_CORRECTION: onResultsDialog_SubmitCorrection(pDialogData);  break;
      }
      
      // Return result
      return bResult;
   }
   CATCH3("Unable to process command (%d, %d) on page '%s'", iControlID, iNotification, szPageNames[pDialogData->eType]);
   return FALSE;
}


/// Function name  : onResultsDialog_ContextMenu
// Description     : Display context menu
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] Window data
// HWND                 hCtrl       : [in] Window handle of the control that was right-clicked
// POINT*               ptCursor    : [in] Cursor position in screen co-ordinates
// 
VOID  onResultsDialog_ContextMenu(RESULTS_DIALOG_DATA*  pDialogData, HWND  hCtrl, POINT*  ptCursor)
{
   SUGGESTION_RESULT  xSelectedItem;  // Selected item
   CUSTOM_MENU_ITEM*  pItemData;      // Item data for the default item
   CUSTOM_MENU*       pCustomMenu;    // Custom Popup menu
   RICH_TEXT*         pDescription;

   TRY
   {
      // [CHECK] Ensure ListView item is selected
      if (GetDlgCtrlID(hCtrl) == IDC_RESULTS_LIST AND findResultsDialogItemByIndex(pDialogData, ListView_GetSelected(pDialogData->hListView), xSelectedItem))
      {
         CONSOLE_COMMAND();
         CONSOLE("Generating menu items for page %s", szPageNames[pDialogData->eType]);

         // Create CustomMenu
         pCustomMenu = createCustomMenu(TEXT("DIALOG_MENU"), TRUE, IDM_RESULTS_POPUP);

         // Lookup CustomMenu data for item 'Insert Result'
         pItemData = getCustomMenuItemData(pCustomMenu->hPopup, IDM_RESULTS_INSERT_RESULT, FALSE);

         /// [INSERT RESULT] Set as default, enable for ScriptDocuments
         EnableMenuItem(pCustomMenu->hPopup, IDM_RESULTS_INSERT_RESULT, (getActiveScriptDocument() ? MF_ENABLED : MF_DISABLED));
         pItemData->bDefault = TRUE;

         // Examine dialog type
         switch (pDialogData->eType)
         {
         /// [COMMANDS] Set default item text, enable MSCI Lookup if relevant
         case RT_COMMANDS: 
            // Set item text + Find description
            utilReplaceString(pItemData->szText, TEXT("Insert Command"));   
            pDescription = xSelectedItem.asCommandSyntax->pTooltipDescription;

            // [MSCI LOOKUP] Enable for commands with a lookup
            EnableMenuItem(pCustomMenu->hPopup, IDM_RESULTS_MSCI_LOOKUP, (isCommandReferenceURLPresent(xSelectedItem.asCommandSyntax) ? MF_ENABLED : MF_DISABLED));
            break;

         /// [GAME/SCRIPT OBJECTS] Set default item text. Remove MSCI Lookup
         case RT_GAME_OBJECTS:
         case RT_SCRIPT_OBJECTS:  
            // Set item text + Find description
            utilReplaceString(pItemData->szText, (pDialogData->eType == RT_GAME_OBJECTS ? TEXT("Insert Game Object") : TEXT("Insert Script Object")));
            pDescription = xSelectedItem.asObjectName->pDescription;

            // [MSCI-LOOKUP] Remove item
            removeCustomMenuItem(pCustomMenu->hPopup, IDM_RESULTS_MSCI_LOOKUP, FALSE); 

            // [SEARCH CONTENT] Remove from GameObjects
            /*if (pDialogData->eType == RT_GAME_OBJECTS)
               removeCustomMenuItem(pCustomMenu->hPopup, IDM_RESULTS_SEARCH_CONTENT, FALSE); */
            break;
         }

         // [SUBMIT/SEARCH] Require certain panels
         EnableMenuItem(pCustomMenu->hPopup, IDM_RESULTS_SUBMIT_CORRECTION, pDialogData->eType != RT_GAME_OBJECTS ? MF_ENABLED : MF_DISABLED);
         //EnableMenuItem(pCustomMenu->hPopup, IDM_RESULTS_SEARCH_CONTENT,    pDialogData->eType == RT_COMMANDS     ? MF_ENABLED : MF_DISABLED);

         /// [SUBMIT CORRECTION] Enable + Set appropriate text         
         if (pDialogData->eType != RT_GAME_OBJECTS)
            setCustomMenuItemText(pCustomMenu->hPopup, IDM_RESULTS_SUBMIT_CORRECTION, FALSE, pDescription ? TEXT("Submit Correction") : TEXT("Submit Description"));
         
         // Display pop-up menu
         TrackPopupMenu(pCustomMenu->hPopup, TPM_LEFTALIGN WITH TPM_TOPALIGN WITH TPM_LEFTBUTTON, ptCursor->x, ptCursor->y, NULL, pDialogData->hDialog, NULL);
         deleteCustomMenu(pCustomMenu);
      }
   }
   CATCH0("");
}


/// Function name  : onResultsDialog_Destroy
// Description     : Cleanup Search results dialog
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Search results dialog window data
// 
VOID    onResultsDialog_Destroy(RESULTS_DIALOG_DATA*  pDialogData)
{
   TRY
   {
      /// [OPTIONAL] Destroy Tooltip
      if (pDialogData->hTooltip)
         utilDeleteWindow(pDialogData->hTooltip);

      /// [OPTIONAL] Destroy RichTooltip
      if (pDialogData->hRichTooltip)
         utilDeleteWindow(pDialogData->hRichTooltip);

      // Sever and delete dialog data
      SetWindowLong(pDialogData->hDialog, DWL_USER, NULL);
      deleteResultsDialogData(pDialogData);
   }
   CATCH0("Unable to destroy tooltips");
}


/// Function name  :  onResultsDialog_GetMenuItemState
// Description     : Determines whether a toolbar/menu command relating to the document should be enabled or disabled
// 
// RESULTS_DIALOG_DATA* pDialogData : [in] 
// CONST UINT           iCommandID  : [in]     Menu/toolbar Command
// UINT*                piState     : [in/out] Combination of MF_ENABLED, MF_DISABLED, MF_CHECKED, MF_UNCHECKED
// 
// Return Value   : TRUE
// 
BOOL   onResultsDialog_GetMenuItemState(RESULTS_DIALOG_DATA*  pDialogData, const UINT  iCommandID, UINT*  piState)
{
   if (!pDialogData)
      return FALSE;

   TRY
   {
      switch (iCommandID)
      {
      /// [EDIT] Require focus to be on the search term
      case IDM_EDIT_REDO:  
      case IDM_EDIT_UNDO:  
      case IDM_EDIT_CUT:
      case IDM_EDIT_COPY:
      case IDM_EDIT_DELETE:
      case IDM_EDIT_PASTE:
      case IDM_EDIT_SELECT_ALL:
         *piState = (GetWindowID(GetFocus()) == IDC_RESULTS_SEARCH ? MF_ENABLED : MF_DISABLED);
         break;

      // [COMMENT] Unsupported
      default:
         *piState = MF_DISABLED;
         break;
      }

      // Return TRUE
      SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, TRUE);
      return TRUE;
   }
   CATCH1("Unable to retrieve state of command %d", iCommandID);
   return FALSE;
}

/// Function name  : onResultsDialog_InsertResult
// Description     : Passes a formatted suggestion to the ActiveDocument
// 
// RESULTS_DIALOG_DATA*  pDialogData   : [in] results dialog data
// CONST INT             iLogicalIndex : [in] Logical item index
// 
VOID    onResultsDialog_InsertResult(RESULTS_DIALOG_DATA*  pDialogData, CONST SUGGESTION_RESULT  xResult)
{
   TRY
   {
      TCHAR*   szOutput = utilCreateEmptyString(256);          // Output text to be sent to the target document

      /// Format search result text
      switch (pDialogData->eType)
      {
      // [SYNTAX] Output: <text>
      case RT_COMMANDS:        StringCchCopy(szOutput, 256, xResult.asCommandSyntax->szSuggestion);        break;
      // [GAME OBJECT] Output: { <text> }
      case RT_GAME_OBJECTS:    StringCchPrintf(szOutput, 256, TEXT("{%s}"), xResult.asObjectName->szText);     break;
      // [SCRIPT OBJECT] Output: [ <text> ]
      case RT_SCRIPT_OBJECTS:  StringCchPrintf(szOutput, 256, TEXT("[%s]"), xResult.asObjectName->szText);     break;
      }

      CONSOLE("Inserting results '%s'", szOutput);

      /// [EVENT] Fire UM_INSERT_TEXT_AT_CARET at the active document
      sendAppMessage(AW_DOCUMENT, UM_INSERT_TEXT_AT_CARET, NULL, (LPARAM)szOutput);
      utilDeleteString(szOutput);
   }
   CATCH2("Unable to insert %s result '%s' into active document", szPageNames[pDialogData->eType], identifySuggestionResult(pDialogData->eType, xResult));
}


/// Function name  : onResultsDialog_LookupCommand
// Description     : Launches the MSCI reference URL (if any) of the currently selected command (if any)
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Results dialog
// 
VOID  onResultsDialog_LookupCommand(RESULTS_DIALOG_DATA*  pDialogData)
{
   SUGGESTION_RESULT  xSelectedItem;  // Selected item

   TRY
   {
      CONSOLE_COMMAND();

      // Lookup selected item
      if (findResultsDialogItemByIndex(pDialogData, ListView_GetSelected(pDialogData->hListView), xSelectedItem))
      {
         // [CHECK] Is this a command with an MSCI URL?
         if (pDialogData->eType == RT_COMMANDS AND lstrlen(xSelectedItem.asCommandSyntax->szReferenceURL))
         {
            /// [FOUND] Launch MSCI reference URL
            CONSOLE("Launching MSCI URL '%s' for command '%s'", xSelectedItem.asCommandSyntax->szReferenceURL, xSelectedItem.asCommandSyntax->szContent);
            utilLaunchURL(getAppWindow(), xSelectedItem.asCommandSyntax->szReferenceURL, SW_SHOWMAXIMIZED);
         }
      }
   }
   CATCH2("iSelected=%d  xResult=%s", ListView_GetSelected(pDialogData->hListView), identifySuggestionResult(pDialogData->eType, xSelectedItem));
}


/// Function name  : onResultsDialog_MeasureItem
// Description     : Performs OwnerDraw item measurements for the ComboBox and ContextMenu
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Result dialog
// MEASUREITEMSTRUCT*    pMessage    : [in] MeasureItem header
// 
// Return Value   : TRUE
// 
BOOL   onResultsDialog_MeasureItem(RESULTS_DIALOG_DATA*  pDialogData, MEASUREITEMSTRUCT*  pMessage)
{
   TRY
   {
      // Examine sender
      switch (pMessage->CtlType)
      {
      case ODT_MENU:      calculateCustomMenuItemSize(pDialogData->hDialog, pMessage);  break;
      case ODT_COMBOBOX:  onWindow_MeasureComboBox(pMessage, ITS_SMALL, ITS_SMALL);     break;
      }
                     
      // Return TRUE
      return TRUE;
   }
   CATCH0("");
   return FALSE;
}


/// Function name  : onResultsDialog_Notify
// Description     : Notification processing
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Search results dialog window data
// HWND                  hDialog     : [in] Search results dialog window handle
// NMHDR*                pMessage    : [in] WM_NOTIFY message header
//
// Return Type : TRUE if processed
// 
BOOL  onResultsDialog_Notify(RESULTS_DIALOG_DATA*  pDialogData, NMHDR*  pMessage)
{
   SUGGESTION_RESULT  xSelectedItem;      // Selected item
   NMITEMACTIVATE*    pListViewItem;      // ListView click data
   BOOL               bResult = FALSE;
   
   TRY
   {
      /// [GROUPED LISTVIEW]
      if (pMessage->idFrom == IDC_RESULTS_LIST)
      {
         // Examine notification
         switch (pMessage->code)
         {
         /// [CUSTOM DRAW]
         case NM_CUSTOMDRAW:
            bResult = onCustomDraw_GroupedListView(pDialogData->hDialog, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage);
            SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, bResult);
            break;

         /// [RICH TOOLTIP]
         case LVN_GETINFOTIP:
            onResultsDialog_RequestTooltipData(pDialogData, pMessage->hwndFrom, (NMLVGETINFOTIP*)pMessage);
            bResult = TRUE;
            break;

         /// [ITEM DATA]
         case LVN_GETDISPINFO:
            onResultsDialog_RequestData(pDialogData, (NMLVDISPINFO*)pMessage);
            bResult = TRUE;
            break;

         /// [DOUBLE CLICK]
         case NM_DBLCLK:
            // Prepare
            pListViewItem = (NMITEMACTIVATE*)pMessage;

            // Lookup selected item
            if (bResult = findResultsDialogItemByIndex(pDialogData, GroupedListView_PhysicalToLogical(pDialogData->hListView, pListViewItem->iItem), xSelectedItem))
               // [FOUND] Insert item
               onResultsDialog_InsertResult(pDialogData, xSelectedItem);
            break;

         // [FOCUSED]
         case NM_SETFOCUS:
            CONSOLE("ResultsDialog ListView receiving input focus");
            break;
         }
      }
      /// [DIALOG TOOLTIP]
      else if (bResult = (pMessage->code == TTN_GETDISPINFO))
         onTooltipRequestText((NMTTDISPINFO*)pMessage);
      
      // Return result
      return bResult;
   }
   CATCH2("idFrom=%d  code=%d", pMessage->idFrom, pMessage->code);
   return FALSE;
}


/// Function name  : onResultsDialog_PreferencesChanged
// Description     : Updates the tooltip display values
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Dialog data
// 
VOID  onResultsDialog_PreferencesChanged(RESULTS_DIALOG_DATA*  pDialogData)
{
   TRY
   {
      // Prepare
      HWND  hListViewTooltip = ListView_GetToolTips(pDialogData->hListView);

      // Update tooltip state/delay
      Tooltip_Activate(hListViewTooltip, getAppPreferences()->bSearchResultTooltips);
      Tooltip_SetDisplayTime(hListViewTooltip, TTDT_INITIAL, getAppPreferences()->iSearchResultTooltipDelay * 1000);   // Wait x seconds before display
      Tooltip_SetDisplayTime(hListViewTooltip, TTDT_RESHOW,  2 * 1000);   // Wait 2 seconds between displays
      Tooltip_SetDisplayTime(hListViewTooltip, TTDT_AUTOPOP, 30 * 1000);  // Display for 30 seconds
   }
   CATCH0("");
}


/// Function name  : onResultsDialog_RequestData
// Description     : Provides ListView data
// 
// RESULTS_DIALOG_DATA*  pDialogData   : [in] 
// NMLVDISPINFO*         pHeader       : [in/out] 
// 
VOID  onResultsDialog_RequestData(RESULTS_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pHeader)
{
   SUGGESTION_RESULT  xResult;         // Search result wrapper
   LVITEM            *pItem;           // Convenience pointer
   CONST TCHAR       *szIconID,        // Icon resource ID
                     *szOverlayIcon;   // Overlay icon resource ID
   TRY
   {
      // Prepare
      pItem    = &pHeader->item;
      szIconID = szOverlayIcon = NULL;

      /// Lookup item data
      if (!findResultsDialogItemByIndex(pDialogData, pItem->iItem, xResult))
         setMissingListViewItem(pItem, getTreeNodeCount(pDialogData->pResultsTree));
      else
      {
         // [SUCCESS] Examine object type
         switch (pDialogData->eType)
         {
         /// [COMMAND LIST]
         case RT_COMMANDS:
            // [TEXT] Provide RichText
            pItem->lParam = (LPARAM)xResult.asCommandSyntax->pDisplaySyntax;
            pItem->mask  |= LVIF_RICHTEXT;

            // [ICON] Use the GameVersion icon
            szIconID = identifyGameVersionIconID(xResult.asCommandSyntax->eGameVersion);

            // [OVERLAY] Add overlay if command MSCI reference URL
            szOverlayIcon = (isCommandReferenceURLPresent(xResult.asCommandSyntax) OR xResult.asCommandSyntax->pTooltipDescription ? TEXT("DESCRIPTION_OVERLAY") : NULL);

            /// [DEBUG] Add overlay if syntax is missing
            //szOverlayIcon = (!xResult.asCommandSyntax->pTooltipDescription ? TEXT("INVALID_ICON") : NULL);
            break;

         /// [GAME OBJECTS / SCRIPT OBJECTS] 
         case RT_GAME_OBJECTS:
         case RT_SCRIPT_OBJECTS:
            // Examine column
            switch (pItem->iSubItem)
            {
            case OBJECT_COLUMN_NAME:
               // [TEXT] Supply object name
               if (pItem->mask INCLUDES LVIF_TEXT)
                  StringCchCopy(pItem->pszText, pItem->cchTextMax, xResult.asObjectName->szText);

               // [ICON/OVERLAY]
               if (pDialogData->eType == RT_GAME_OBJECTS)
               {
                  // [GAME-OBJECT] Use a fixed icon, and check for a description GameString
                  szIconID      = TEXT("GAME_OBJECT_ICON");
                  szOverlayIcon = TEXT("DESCRIPTION_OVERLAY");
               }
               else 
               {
                  // [SCRIPT-OBJECT] Use a fixed or GameVersion icon. Check for RichText description
                  szIconID      = (!xResult.asObjectName->bModContent ? identifyGameVersionIconID(xResult.asObjectName->eVersion) : TEXT("SCRIPT_OBJECT_ICON"));
                  szOverlayIcon = (xResult.asObjectName->pDescription ? TEXT("DESCRIPTION_OVERLAY") : NULL);   
               }
               break;

            case OBJECT_COLUMN_TYPE:
               // [TEXT] Lookup group name
               if (pItem->mask INCLUDES LVIF_TEXT)
                  loadString(pDialogData->iFirstGroupResourceID + (xResult.asObjectName->eGroup - pDialogData->iFirstGroupID), pItem->pszText, pItem->cchTextMax);
               break;
            }
            break;
         }

         // [ICON] Encode icon index and overlay index (if present) into result
         if (szIconID)
            pItem->iImage = MAKE_LONG(getAppImageTreeIconIndex(ITS_SMALL, szIconID), (szOverlayIcon ? getAppImageTreeIconIndex(ITS_SMALL, szOverlayIcon) : NULL));
      }
   }
   CATCH2("Unable to retrieve data for item %d. eDialogType=%d", pHeader->item, pDialogData->eType);
}



/// Function name  : onResultsDialog_RequestTooltipData
// Description     : Provides CustomDraw tooltip data 
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Dialog data
// HWND                  hListView   : [in] ListView sending the message
// NMLVGETINFOTIP*       pHeader     : [in] Tooltip request header
// 
// Return Value   : TRUE if handled, otherwise FALSE
// 
BOOL  onResultsDialog_RequestTooltipData(RESULTS_DIALOG_DATA*  pDialogData, HWND  hListView, NMLVGETINFOTIP*  pHeader)
{
   SUGGESTION_RESULT   xResult;

   TRY
   {
      // Resolve item  (Logical when sent by GroupedListView handler, physical when sent by ListView base)
      INT  iItem = (pHeader->dwFlags == LVGIT_CUSTOM ? pHeader->iItem : GroupedListView_PhysicalToLogical(hListView, pHeader->iItem));

      // [CHECK] Ensure user is hovering over an item
      if (findResultsDialogItemByIndex(pDialogData, iItem, xResult))
      {
         /// [CUSTOM DRAWN]
         if (pHeader->dwFlags == LVGIT_CUSTOM)
         {
            // [TRACK]
            CONSOLE_COMMAND();
            CONSOLE("Returning tooltip for %s '%s'", szPageNames[pDialogData->eType], identifySuggestionResult(pDialogData->eType, xResult));
            
            // Set custom Data
            pHeader->lParam = (LPARAM)createCustomTooltipData(pDialogData->eType, xResult);
         }

         // Insert placeholder - Tooltip will not display if buffer is empty
         StringCchCopy(pHeader->pszText, pHeader->cchTextMax, TEXT("."));

         // Store Tooltip data in header
         SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, TRUE);
      }
      
      // [SUCCESS] Return TRUE
      return TRUE;
   }
   // [FAILED] Return FALSE
   CATCH0("");
   return FALSE;
}


/// Function name  : onResultsDialog_Resize
// Description     : Resize controls to cover the dialog
//
// RESULTS_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST SIZE*           pDialogSize : [in] New dialog size
// 
VOID   onResultsDialog_Resize(RESULTS_DIALOG_DATA*  pDialogData, CONST SIZE*  pDialogSize)
{
   SIZE    siColumn[2];        // ListView column sizes
   RECT    rcControl;          // Client rectangle of the control being resized
   HWND    hListView;          // Convenience handle
   HDWP    hControlPositions;  // Multiple window positions

   TRY
   {
      // Prepare
      hListView = GetControl(pDialogData->hDialog, IDC_RESULTS_LIST);

      // Setup positioning
      hControlPositions = BeginDeferWindowPos(4);

      /// Extend ListView to the right and bottom edge of the dialog
      utilGetDlgItemRect(pDialogData->hDialog, IDC_RESULTS_LIST, &rcControl);
      rcControl.right  = pDialogSize->cx;
      rcControl.bottom = pDialogSize->cy;
      utilDeferDlgItemRect(hControlPositions, pDialogData->hDialog, IDC_RESULTS_LIST, &rcControl, FALSE, TRUE);

      /// Align CheckBox with edge of the dialog
      utilGetDlgItemRect(pDialogData->hDialog, IDC_RESULTS_COMPATIBLE, &rcControl);
      rcControl.left  = pDialogSize->cx - (rcControl.right - rcControl.left);
      rcControl.right = pDialogSize->cx;
      utilDeferDlgItemRect(hControlPositions, pDialogData->hDialog, IDC_RESULTS_COMPATIBLE, &rcControl, TRUE, FALSE);

      /// Extend Search Edit to the edge of the dialog
      utilGetDlgItemRect(pDialogData->hDialog, IDC_RESULTS_SEARCH, &rcControl);
      rcControl.right = pDialogSize->cx;
      utilDeferDlgItemRect(hControlPositions, pDialogData->hDialog, IDC_RESULTS_SEARCH, &rcControl, FALSE, TRUE);

      /// Extend Search Filter to the edge of the dialog
      utilGetDlgItemRect(pDialogData->hDialog, IDC_RESULTS_FILTER, &rcControl);
      rcControl.right = pDialogSize->cx;
      utilDeferDlgItemRect(hControlPositions, pDialogData->hDialog, IDC_RESULTS_FILTER, &rcControl, FALSE, TRUE);

      // Reposition windows
      EndDeferWindowPos(hControlPositions);

      /// [GAME/SCRIPT OBJECTS] Resize both ListView columns
      if (pDialogData->eType != RT_COMMANDS)
      {
         // Load the longest second column entry
         const TCHAR*  szColumnText = loadTempString(pDialogData->eType == RT_SCRIPT_OBJECTS ? IDS_SCRIPT_OBJECT_GROUP_OBJECT_COMMAND : IDS_GAME_OBJECT_GROUP_FACTORY);

         // Determine minimum 2nd column width
         siColumn[1].cx = ListView_GetStringWidth(hListView, szColumnText) + 4 * GetSystemMetrics(SM_CXFIXEDFRAME);  // Account for 2xFixedFrame on either side of sub-item text

         // Use the rest for the 1st column.
         siColumn[0].cx = max(100, pDialogSize->cx - siColumn[1].cx - GetSystemMetrics(SM_CXVSCROLL) - 2 * GetSystemMetrics(SM_CXFIXEDFRAME));  // Account for 2xFixedFrame on right side of text, plus scroll bar

         // Adjust columns
         ListView_SetColumnWidth(hListView, 0, siColumn[0].cx);
         ListView_SetColumnWidth(hListView, 1, siColumn[1].cx);
      }
      /// [COMMAND LIST] Resize single ListView column
      else
         //ListView_SetColumnWidth(hListView, 0, pDialogSize->cx - (2 * GetSystemMetrics(SM_CXFIXEDFRAME)) - GetSystemMetrics(SM_CXVSCROLL));  // Account for 2xFixedFrame on right side of text, plus scroll bar
         ListView_SetColumnWidth(hListView, 0, LVSCW_AUTOSIZE_USEHEADER);
   }
   CATCH2("Unable to resize dialog to (%d, %d)", pDialogSize->cx, pDialogSize->cy);
}


/// Function name  : onResultsDialog_Show
// Description     : Refreshes results when the dialog is displayed
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Dialog data
// 
VOID  onResultsDialog_Show(RESULTS_DIALOG_DATA*  pDialogData)
{
   TRY
   {
      /// Update results
      updateResultsDialog(pDialogData->hDialog);
      SetFocus(GetDlgItem(pDialogData->hDialog, IDC_RESULTS_SEARCH));
   }
   CATCH0("");
}


/// Function name  : onResultsDialog_SearchContent
// Description     : Searches scripts for the selected Command
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Results dialog
// 
VOID  onResultsDialog_SearchContent(RESULTS_DIALOG_DATA*  pDialogData)
{
   SUGGESTION_RESULT  xSelectedItem;         // Search result wrapper
   SCRIPT_CONTENT     xContent;
   TCHAR*             szFolder;
   
   TRY
   {
      // [DEBUG]
      CONSOLE_ACTION_BOLD();

      // [CHECK] Lookup selected item
      if (!findResultsDialogItemByIndex(pDialogData, ListView_GetSelected(pDialogData->hListView), xSelectedItem))
         return;

      // Get 'Scripts' Folder
      szFolder = generateGameDataFilePath(GFI_SCRIPT_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);
      
      // [TRACK]
      CONSOLE("Searching for scripts in '%s' that use %s '%s'", szFolder, szPageNames[pDialogData->eType], identifySuggestionResult(pDialogData->eType, xSelectedItem));

      // [QUESTION] "Would you like to search '%s' for all external scripts that use the selected command?"
      if (displayMessageDialogf(NULL, IDS_GENERAL_CONFIRM_CONTENT_SEARCH, MDF_YESNO WITH MDF_QUESTION, szFolder) == IDYES)
      {
         /// Launch search thread
         xContent.asPointer = xSelectedItem.asPointer;
         launchOperation(getMainWindowData(), createScriptOperationData(pDialogData->eType == RT_COMMANDS ? CT_SYNTAX : CT_OBJECT, szFolder, xContent));
      }

      // Cleanup
      utilDeleteString(szFolder);
   }
   CATCH2("Unable to display correction dialog for %s item %s", szPageNames[pDialogData->eType], identifySuggestionResult(pDialogData->eType, xSelectedItem));
}


/// Function name  : onResultsDialog_SubmitCorrection
// Description     : Displays the correction dialog for the currently selected item (if any)
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Results dialog
// 
VOID  onResultsDialog_SubmitCorrection(RESULTS_DIALOG_DATA*  pDialogData)
{
   SUGGESTION_RESULT  xSelectedItem;         // Search result wrapper
   
   TRY
   {
      CONSOLE_ACTION_BOLD();

      // Lookup selected item
      if (findResultsDialogItemByIndex(pDialogData, ListView_GetSelected(pDialogData->hListView), xSelectedItem))
      {
         CONSOLE("Submitting correction for %s '%s'", szPageNames[pDialogData->eType], identifySuggestionResult(pDialogData->eType, xSelectedItem));

         /// [FOUND] Pass item data to Correction Dialog
         displayCorrectionDialog(getAppWindow(), pDialogData->eType, xSelectedItem);
      }
   }
   CATCH2("Unable to display correction dialog for %s item %s", szPageNames[pDialogData->eType], identifySuggestionResult(pDialogData->eType, xSelectedItem));
}


/// Function name  : onResultsDialog_Timer
// Description     : Refreshes results after the user has changed the search term
// 
// RESULTS_DIALOG_DATA*  pDialogData : [in] Commmand dialog window handle
// CONST UINT            iTimerID    : [in] Timer ID
// 
VOID   onResultsDialog_Timer(RESULTS_DIALOG_DATA*  pDialogData, CONST UINT  iTimerID)
{
   TRY
   {
      ASSERT(iTimerID == RESULTS_DELAY_TICKER);
      CONSOLE_UI(WM_TIMER, RESULTS_DELAY_TICKER);
      
      /// [UPDATE] Refresh current results
      updateResultsDialog(pDialogData->hDialog);

      // Destroy the timer
      KillTimer(pDialogData->hDialog, iTimerID);
   }
   CATCH0("");
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocSearchGameData
// Description     : Populates a ResultsDialog tree by matching Text, Group and GameVersion
// 
// AVL_TREE_NODE*       pNode          : [in/out] Node containing object to compare
// AVL_TREE_OPERATION*  pOperationData : [in]  -> xFirstInput  : [in]  Text to search for (or NULL to perform no check)
//                                             -> xSecondInput : [in]  Group to filter by (or -1 to perform no check)
//                                             -> xThirdInput  : [in]  SearchResults dialog data (Containing GameVersion, Object type and group IDs)
//                                             -> xFourthInput : [in]  Whether to show compatible objects only
//
//                                             -> pOutputTree  : [out] Output tree for successful matches
//
///                                                        NOTE: This operation does not set an operation result
//
VOID    treeprocSearchGameData(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   RESULTS_DIALOG_DATA  *pDialogData;       // Dialog data
   GAME_VERSION          eNodeVersion;      // Node GameVersion, if appliciable
   CONST TCHAR          *szSearchTerm,      // Search term, if any
                        *szNodeText = NULL; // Text of the current node
   UINT                  eGroupFilter,      // Group filter, if any
                         eNodeGroup;        // Group of the current node
   BOOL                  bIncludeResult,    // Comparison result
                         bCompatibleOnly;   // Whether to show compatible results only

   TRY
   {
      // Prepare
      bIncludeResult = TRUE;

      // Extract parameters
      szSearchTerm    = (CONST TCHAR*)pOperationData->xFirstInput;
      eGroupFilter    = (UINT)pOperationData->xSecondInput;
      pDialogData     = (RESULTS_DIALOG_DATA*)pOperationData->xThirdInput;
      bCompatibleOnly = (BOOL)pOperationData->xFourthInput;

      /// Extract relevant node data
      switch (pDialogData->eType)
      {
      // [COMMAND LIST] Check against SYNTAX and COMMAND_GROUP
      case RT_COMMANDS:      
         szNodeText   = extractPointerFromTreeNode(pNode, COMMAND_SYNTAX)->szSyntax;
         eNodeGroup   = extractPointerFromTreeNode(pNode, COMMAND_SYNTAX)->eGroup;
         eNodeVersion = extractPointerFromTreeNode(pNode, COMMAND_SYNTAX)->eGameVersion;
         break;

      // [GAME/SCRIPT OBJECT] Check against TEXT and OBJECT_NAME_GROUP
      case RT_GAME_OBJECTS:
      case RT_SCRIPT_OBJECTS:
         szNodeText   = extractPointerFromTreeNode(pNode, OBJECT_NAME)->szText;
         eNodeGroup   = extractPointerFromTreeNode(pNode, OBJECT_NAME)->eGroup;
         eNodeVersion = extractPointerFromTreeNode(pNode, OBJECT_NAME)->eVersion;
         break;
      }

      /// [HIDDEN GROUP] Ensure this group is not always hidden
      if (eNodeGroup < pDialogData->iFirstGroupID OR eNodeGroup > pDialogData->iLastGroupID) 
         // [FAILED] Group has been excluded by design
         bIncludeResult = FALSE;

      /// [FILTER GROUP] Are we filtering results to a single group?
      else if (eGroupFilter != -1 AND eGroupFilter != (eNodeGroup - pDialogData->iFirstGroupID))
         // [FAILED] Node belongs to a different group
         bIncludeResult = FALSE;

      /// [GAME VERSION] Check GameVersion  (Also exclude internal ParameterTypes manually)
      else switch (pDialogData->eType)
      {
      // [COMMANDS] Check syntax is compatible
      case RT_COMMANDS:   
         bIncludeResult = !bCompatibleOnly OR isCommandSyntaxCompatible(extractPointerFromTreeNode(pNode, COMMAND_SYNTAX), pDialogData->eGameVersion);
         break;

      // [SCRIPT OBJECTS] Check GameVersion
      case RT_SCRIPT_OBJECTS:
         if (eNodeVersion > pDialogData->eGameVersion)
            bIncludeResult = !bCompatibleOnly;

         // [PARAMETER TYPE] Always exclude the internal parameter types
         else if (eNodeGroup == ONG_PARAMETER_TYPE)
         {
            switch (extractPointerFromTreeNode(pNode, OBJECT_NAME)->iID)
            {
            case PS_COMMENT:
            case PS_CONDITION:      
            case PS_EXPRESSION:
            case PS_INTERRUPT_RETURN_OBJECT_IF:
            case PS_LABEL_NAME:
            case PS_LABEL_NUMBER:
            case PS_PARAMETER:
            case PS_REFERENCE_OBJECT:
            case PS_RETURN_OBJECT:
            case PS_RETURN_OBJECT_IF:
            case PS_RETURN_OBJECT_IF_START:
               bIncludeResult = FALSE;
               break;
            }
         }
         break;
      }

      /// [SEARCH TERM] Ensure search term matches
      if (bIncludeResult AND (!szSearchTerm OR utilFindSubStringVariableI(szNodeText, szSearchTerm)))
         // [SUCCESS] Insert object into the results tree
         insertObjectIntoAVLTree(pOperationData->pOutputTree, pNode->pData);
   }
   CATCH3("Unable to compare node '%s'.  eNodeGroup=%d  eNodeVersion=%d", szNodeText, eNodeGroup, eNodeVersion);
}


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  WINDOW PROCEDURE
/// ////////////////////////////////////////////////////////////////////////////////////////
         
/// Function name  : dlgprocResultsDialog
// Description     : Search results dialog window procedure
// 
INT_PTR    dlgprocResultsDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   RESULTS_DIALOG_DATA*  pDialogData = getResultsDialogData(hDialog);
   POINT                 ptCursor;
   BOOL                  bResult = TRUE;
   SIZE                  siDialog;
   
   TRY
   {
      // Examine message
      switch (iMessage)
      {
      /// [CREATION]
      case WM_INITDIALOG:
         initResultsDialog((RESULTS_DIALOG_DATA*)lParam, hDialog);
         break;

      /// [DESTRUCTION]
      case WM_DESTROY:
         onResultsDialog_Destroy(pDialogData);
         break;

      /// [COMMANDS]
      case WM_COMMAND:
         bResult = onResultsDialog_Command(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         bResult = onResultsDialog_Notify(pDialogData, (NMHDR*)lParam);
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         ptCursor.x = LOWORD(lParam);
         ptCursor.y = HIWORD(lParam);
         onResultsDialog_ContextMenu(pDialogData, (HWND)wParam, &ptCursor);
         break;

      /// [MENU ITEM HOVER] Forward messages from CodeEdit up the chain to the Main window
      case WM_MENUSELECT:
         sendAppMessage(AW_MAIN, WM_MENUSELECT, wParam, lParam);
         break;

      /// [MENU ITEM STATE]
      case UM_GET_MENU_ITEM_STATE:
         bResult = onResultsDialog_GetMenuItemState(pDialogData, wParam, (UINT*)lParam);
         break;

      /// [SHOW]
      case WM_SHOWWINDOW:
         if (wParam == TRUE)
            onResultsDialog_Show(pDialogData);
         break;
      
      /// [RESIZING]
      case WM_SIZE:
         siDialog.cx = LOWORD(lParam);
         siDialog.cy = HIWORD(lParam);
         onResultsDialog_Resize(pDialogData, &siDialog);
         break;

      /// [INPUT TIMER]
      case WM_TIMER:
         onResultsDialog_Timer(pDialogData, wParam);
         break;

      /// [HELP]
      case WM_HELP:
         displayHelp(TEXT("Window_Commands"));
         break;

      /// [CUSTOM MENU/CUSTOM COMBO]
      case WM_DRAWITEM:    bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);       break;
      case WM_DELETEITEM:  bResult = onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);   break;
      case WM_MEASUREITEM: bResult = onResultsDialog_MeasureItem(pDialogData, (MEASUREITEMSTRUCT*)lParam);  break;
                           
      /*/// [VISUAL STYLES]
      case WM_CTLCOLORDLG:
      case WM_CTLCOLORSTATIC:
         bResult = (BOOL)onDialog_ControlColour((HDC)wParam, IsThemeActive() ? COLOR_WINDOW : COLOR_BTNFACE);
         break;*/

      /// [STYLE CHANGED]
      case WM_THEMECHANGED:
         // Change ListView colour
         ListView_SetBkColor(pDialogData->hListView, getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
         break;

      /// [ACTIVATE]
      case WM_ACTIVATE:
         setAppActiveDialog(wParam ? hDialog : NULL);
         bResult = FALSE;
         break;

      // [PREFERENCES CHANGED]
      case UN_PREFERENCES_CHANGED:
         onResultsDialog_PreferencesChanged(pDialogData);
         break;

      // [UNHANDLED] 
      default:
         bResult = FALSE;
         break;
      }

      // [FOCUS HANDLER]
      updateMainWindowToolBar(iMessage, wParam, lParam);

      // Return result
      return (INT_PTR)bResult;
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);
   return FALSE;
}

