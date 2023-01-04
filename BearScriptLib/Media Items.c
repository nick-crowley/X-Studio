//
// Language Files.cpp : Functions regarding language files
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Number of properties in sound effect and video definitions
CONST UINT  iSoundEffectPropertiesCount = 8,
            iVideoPropertiesCount       = 12;

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createMediaItemTreeByPageID
// Description     : Creates an MediaItems tree sorted by ID in ascending order
// 
// Return Value   : New AVLTree, you are responsible for destroying it
//  
AVL_TREE*  createMediaItemTreeByPageID()
{
   return createAVLTree(extractMediaItemTreeNode, deleteMediaItemTreeNode, createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}


/// Function name  : createMediaPageTreeByGroup
// Description     : Creates an MediaPages tree sorted by Type and then PageID (in ascending order)
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
AVL_TREE*  createMediaPageTreeByGroup()
{
   return createAVLTree(extractMediaPageTreeNode, deleteMediaPageTreeNode, createAVLTreeSortKey(AK_GROUP, AO_ASCENDING), createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), NULL);
}


/// Function name  : createMediaPage
// Description     : Create a new MediaPage object
// 
// CONST MEDIA_ITEM_TYPE  eType   : [in] Type of MediaItems contained within the page
// CONST UINT             iPageID : [in] PageID of the new page
// CONST TCHAR*           szTitle : [in] Title for the new page
// 
// Return Value   : New MEDIA_PAGE object, you are responsible for destroying it
// 
MEDIA_PAGE*  createMediaPage(CONST MEDIA_ITEM_TYPE  eType, CONST UINT  iPageID, CONST TCHAR*  szTitle)
{
   MEDIA_PAGE*  pNewPage;     // New MediaPage object

   // Create new object
   pNewPage = utilCreateEmptyObject(MEDIA_PAGE);

   // Set properties
   pNewPage->eType   = eType;
   pNewPage->iPageID = iPageID;
   pNewPage->szTitle = utilDuplicateString(szTitle, lstrlen(szTitle));

   // Return
   return pNewPage;
}


/// Function name  : createSpeechClipItem
// Description     : Create a new MediaItem representing a speech clip
// 
// CONST UINT  iPageID   : [in] ID of the page containing the item
// CONST UINT  iID       : [in] ID of the new item
// CONST UINT  iPosition : [in] Starting position of the speech clip, in milliseconds
// CONST UINT  iLength   : [in] Length of the speech clip, in milliseconds
// 
// Return Value   : New MediaItem object, you are responsible for destroying it
// 
MEDIA_ITEM*  createSpeechClipItem(CONST UINT  iPageID, CONST UINT  iID, CONST UINT  iPosition, CONST UINT  iLength)
{
   MEDIA_ITEM*  pNewItem;

   // Create empty Speech Clip item
   pNewItem = utilCreateEmptyObject(MEDIA_ITEM);
   pNewItem->eType = MIT_SPEECH_CLIP;

   // Set properties
   pNewItem->iID       = iID;
   pNewItem->iPageID   = iPageID;
   pNewItem->iPosition = iPosition;
   pNewItem->iLength   = iLength;

   // Return new item
   return pNewItem;
}

/// Function name  : createSoundEffectItem
// Description     : Create a new 'sound effect' media item
// 
// CONST UINT    iPageID       : [in] ID of the page containing the MediaItem
// CONST UINT    iID           : [in] ID of the new MediaItem
// CONST TCHAR*  szDescription : [in] Description of the sound effect
// 
// Return Value   : New MediaItem object, you are responsible for destroying it
// 
MEDIA_ITEM*  createSoundEffectItem(CONST UINT  iPageID, CONST UINT  iID, CONST TCHAR*  szDescription)
{
   MEDIA_ITEM*  pNewItem;

   // Create empty SoundEffect item
   pNewItem = utilCreateEmptyObject(MEDIA_ITEM);
   pNewItem->eType = MIT_SOUND_EFFECT;

   // Set properties
   pNewItem->iID           = iID;
   pNewItem->iPageID       = iPageID;
   pNewItem->szDescription = utilDuplicateString(szDescription, lstrlen(szDescription));

   // Return new item
   return pNewItem;
}


/// Function name  : createVideoClipItem
// Description     : Create a new video clip 'media item'
// 
// CONST UINT             iPageID        : [in] ID of the page containing the MediaItem
// CONST UINT             iID            : [in] ID for the new MediaItem
// CONST TCHAR*           szDescription  : [in] Description of the video clip
// CONST MEDIA_POSITION*  pStart         : [in] Time-index of the first frame of the video clip
// CONST MEDIA_POSITION*  pFinish        : [in] Time-index of the last frame of the video clip
// 
// Return Value   : New MediaItem object, you are responsible for destroying it
// 
MEDIA_ITEM*  createVideoClipItem(CONST UINT  iPageID, CONST UINT  iID, CONST TCHAR*  szDescription, CONST MEDIA_POSITION*  pStart, CONST MEDIA_POSITION*  pFinish)
{
   MEDIA_ITEM*  pNewItem = utilCreateEmptyObject(MEDIA_ITEM);

   // Set properties
   pNewItem->eType         = MIT_VIDEO_CLIP;
   pNewItem->iID           = iID;
   pNewItem->iPageID       = iPageID;
   pNewItem->oStart        = (*pStart);
   pNewItem->oFinish       = (*pFinish);
   pNewItem->szDescription = utilDuplicateSimpleString(szDescription);

   // Return new item
   return pNewItem;
}


/// Function name  : deleteMediaItemTreeNode
// Description     : Deletes a MediaItem within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a MediaItem pointer
// 
VOID   deleteMediaItemTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteMediaItem((MEDIA_ITEM*&)pData);
}


/// Function name  : deleteMediaItem
// Description     : Delete a MediaItem object
// 
// MEDIA_ITEM*  &pMediaItem   : [in] MediaItem object to delete
// 
VOID  deleteMediaItem(MEDIA_ITEM*  &pMediaItem)
{
   // Delete item's description, if it has one
   utilSafeDeleteString(pMediaItem->szDescription);
   // Delete calling object
   utilDeleteObject(pMediaItem);
}


/// Function name  : deleteMediaPage
// Description     : Delete a MediaPage object
// 
// MEDIA_PAGE*  &pMediaPage   : [in] MediaPage object to delete
// 
VOID  deleteMediaPage(MEDIA_PAGE*  &pMediaPage)
{
   // Delete page title
   utilDeleteString(pMediaPage->szTitle);
   // Delete calling object
   utilDeleteObject(pMediaPage);
}


/// Function name  : deleteMediaPageTreeNode
// Description     : Deletes a MediaPage within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a MediaPage pointer
// 
VOID   deleteMediaPageTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteMediaPage((MEDIA_PAGE*&)pData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractMediaItemTreeNode
// Description     : Extract the desired property from a given TreeNode containing a MediaItem
// 
// LPARAM                   pNode      : [in] MEDIA_ITEM* : Node data containing a MediaItem
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractMediaItemTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   MEDIA_ITEM*  pMediaItem;    // Convenience pointer
   LPARAM       xOutput;        // Property value output

   // Prepare
   pMediaItem = (MEDIA_ITEM*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_ID:       xOutput = (LPARAM)pMediaItem->iID;          break;
   case AK_PAGE_ID:  xOutput = (LPARAM)pMediaItem->iPageID;      break;
   default:          xOutput = NULL;                              break;
   }

   // Return property value
   return xOutput;
}



/// Function name  : extractMediaPageTreeNode
// Description     : Extract the desired property from a given TreeNode containing a MediaPage
// 
// LPARAM                   pNode      : [in] MEDIA_PAGE* : Node data containing a MediaPage
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractMediaPageTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   MEDIA_PAGE*  pMediaPage;    // Convenience pointer
   LPARAM       xOutput;        // Property value output

   // Prepare
   pMediaPage = (MEDIA_PAGE*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_GROUP:    xOutput = (LPARAM)pMediaPage->eType;         break;
   case AK_PAGE_ID:  xOutput = (LPARAM)pMediaPage->iPageID;       break;
   default:          xOutput = NULL;                              break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : identifyMediaItemType
// Description     : Returns description string for a MediaItem
// 
// CONST MEDIA_ITEM*  pItem   : [in] Media Item
// 
// Return Value   : description string
// 
CONST TCHAR*  identifyMediaItemType(CONST MEDIA_ITEM*  pItem)
{
   CONST TCHAR*  szOutput;

   // Examine type
   switch (pItem->eType)
   {
   case MIT_SPEECH_CLIP:   szOutput = TEXT("speech clip");    break;
   case MIT_SOUND_EFFECT:  szOutput = TEXT("sound effect");   break;
   case MIT_VIDEO_CLIP:    szOutput = TEXT("video clip");     break;
   }

   // Return description
   return szOutput;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateMediaItemDuration
// Description     : Calculate the duration of a media item in milliseconds
// 
// CONST MEDIA_ITEM*  pItem   : [in] MediaItem of any type
// 
// Return Value   : Duration of MediaItem, in milliseconds
// 
BearScriptAPI
UINT  calculateMediaItemDuration(CONST MEDIA_ITEM*  pItem)
{
   UINT  iResult;

   switch (pItem->eType)
   {
   // [SFX/SPEECH] Return length stored in item
   case MIT_SPEECH_CLIP:
   case MIT_SOUND_EFFECT:
      iResult = pItem->iLength;
      break;

   // [VIDEO] Convert start and end frames into a number of milliseconds
   case MIT_VIDEO_CLIP:
      iResult = (pItem->oFinish.iMilliseconds - pItem->oStart.iMilliseconds);
      iResult += (pItem->oFinish.iSeconds - pItem->oStart.iSeconds) * 1000;
      iResult += (pItem->oFinish.iMinutes - pItem->oStart.iMinutes) * (60 * 1000);
      break;
   }

   return iResult;
}



/// Function name  : findMediaItemByID
// Description     : Find a media item in the game data by ID and by PageID
// 
// CONST UINT   iID      : [in]  Media Item ID
// CONST UINT   iPageID  : [in]  Media item PageID
// MEDIA_ITEM*  &pOutput : [out] Resultant MediaItem if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findMediaItemByID(CONST UINT iID, CONST UINT iPageID, MEDIA_ITEM*  &pOutput)
{
   // Search GameData's 'MediaItems' tree
   return findObjectInAVLTreeByValue(getGameData()->pMediaItemsByID, iPageID, iID, (LPARAM&)pOutput);
}


/// Function name  : findMediaItemByIndex
// Description     : Find a media item in the game data
// 
// CONST UINT    iIndex   : [in]  Zero-based Index of the media item to find
// MEDIA_ITEM*  &pOutput  : [out] Resultant media item if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findMediaItemByIndex(CONST UINT iIndex, MEDIA_ITEM*  &pOutput)
{
   // Search GameData's 'MediaItems' tree
   return findObjectInAVLTreeByIndex(getGameData()->pMediaItemsByID, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findMediaPageByID
// Description     : Find a media page in the game data
// 
// CONST MEDIA_ITEM_TYPE  eType : [in]
// CONST UINT    iPageID  : [in]  Zero-based PageID of the MediaPage to find
// MEDIA_PAGE*  &pOutput  : [out] Resultant MediaPage if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findMediaPageByID(CONST MEDIA_ITEM_TYPE  eType, CONST UINT  iPageID, MEDIA_PAGE*  &pOutput)
{
   // Search GameData's 'MediaItems' tree
   return findObjectInAVLTreeByValue(getGameData()->pMediaPagesByGroup, (LPARAM)eType, iPageID, (LPARAM&)pOutput);
}


/// Function name  : findMediaPageByIndex
// Description     : Find a media page in the game data
// 
// CONST UINT    iIndex   : [in]  Zero-based Index of the MediaPage to find
// MEDIA_PAGE*  &pOutput  : [out] Resultant MediaPage if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findMediaPageByIndex(CONST UINT iIndex, MEDIA_PAGE*  &pOutput)
{
   // Search GameData's 'MediaItems' tree
   return findObjectInAVLTreeByIndex(getGameData()->pMediaPagesByGroup, iIndex, (LPARAM&)pOutput);
}


/// Function name  : formatMediaItemDuration
// Description     : Format a media item duration into a string
// 
// CONST UINT  iDuration     : [in]  Media item duration, in millseconds
// TCHAR*      szOutput      : [out] Output string
// CONST UINT  iOutputLength : [in]  Length of output string, in characters
// 
BearScriptAPI
VOID  formatMediaItemDuration(CONST UINT  iDuration, TCHAR*  szOutput, CONST UINT  iOutputLength)
{
   UINT  iMinutes,      // Number of minutes
         iSeconds;      // Number of seconds

   // Calculate Minutes and Seconds
   iMinutes = iDuration / (60 * 1000);
   iSeconds = (iDuration / 1000) % 60;

   // Format
   if (iMinutes == 0 AND iSeconds == 0)
      StringCchCopy(szOutput, iOutputLength, TEXT("< 1 sec"));
   else if (iMinutes == 0)
      StringCchPrintf(szOutput, iOutputLength, TEXT("%u secs"), iSeconds);
   else 
      StringCchPrintf(szOutput, iOutputLength, TEXT("%u minus %u secs"), iMinutes);
}


/// Function name  : insertMediaPageIntoGameData
// Description     : Attempt to create a new SFX or VIDEO MediaPage and insert it into the GameData
// 
// CONST MEDIA_ITEM_TYPE  eType   : [in]  The type of MediaPage to create
// CONST UINT             iPageID : [in]  Page ID
// CONST TCHAR*           szTitle : [in]  Page title
// ERROR_STACK*          &pError  : [out] Warning caused a conflict with an existing entry, if any
// 
// Return Value   : TRUE if added successfully, otherwise FALSE
// 
BOOL    insertMediaPageIntoGameData(CONST MEDIA_ITEM_TYPE  eType, CONST UINT  iPageID, CONST TCHAR*  szTitle, ERROR_STACK*  &pError)
{
   MEDIA_PAGE    *pNewMediaPage,          // MediaPage being created
                 *pExistingMediaPage;     // Pre-existing MediaPage causing the conflict
   
   // [CHECK] Cannot add speech pages in this way
   ASSERT(eType != MIT_SPEECH_CLIP);

   // Prepare
   pError = NULL;

   /// Create new MediaPage
   pNewMediaPage = createMediaPage(eType, iPageID, szTitle);

   /// Attempt to insert into GameData
   if (!insertObjectIntoAVLTree(getGameData()->pMediaPagesByGroup, (LPARAM)pNewMediaPage))
   {
      /// Lookup conflicting item
      if (findObjectInAVLTreeByValue(getGameData()->pMediaPagesByGroup, pNewMediaPage->eType, pNewMediaPage->iPageID, (LPARAM&)pExistingMediaPage))
      {
         // Generate appropriate error message
         switch (eType)
         {
         /// [SOUND EFFECT] These pages are generated manually by ME and should always be added successfully
         case MIT_SOUND_EFFECT:  
            // [WARNING] "Cannot add a media page for sound effects relating to '%s' (ID:%u) because it conflicts with '%s' (ID:%u)"
            pError = generateDualWarning(HERE(IDS_MEDIA_SFX_DUPLICATE_PAGE), pNewMediaPage->szTitle, pNewMediaPage->iPageID, pExistingMediaPage->szTitle, pExistingMediaPage->iPageID);
            break;

         /// [VIDEO CLIP] These pages are generated manually by ME and should always be added successfully
         case MIT_VIDEO_CLIP:
            // [WARNING] "Cannot add a media page for video clips relating to '%s' (ID:%u) because it conflicts with '%s' (ID:%u)"
            pError = generateDualWarning(HERE(IDS_MEDIA_VIDEO_DUPLICATE_PAGE), pNewMediaPage->szTitle, pNewMediaPage->iPageID, pExistingMediaPage->szTitle, pExistingMediaPage->iPageID);
            break;
         }
      }

      // Cleanup
      deleteMediaPage(pNewMediaPage);
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : insertMediaPageIntoSpeechFile
// Description     : Attempt insert a new page of speech clips into a SpeechFile. If the page already exists the new one is discarded
// 
// SPEECH_FILE*  pSpeechFile : [in/out] SpeechFile to insert page into
// CONST UINT    iPageID     : [in]     ID of the page to create
// 
VOID    insertMediaPageIntoSpeechFile(SPEECH_FILE*  pSpeechFile, CONST UINT  iPageID)
{
   MEDIA_PAGE    *pNewSpeechPage;          // MediaPage being created
   
   /// Create new speech clip page
   pNewSpeechPage = createMediaPage(MIT_SPEECH_CLIP, iPageID, NULL);

   /// Attempt to insert into SpeechFile's page tree
   if (!insertObjectIntoAVLTree(pSpeechFile->pSpeechPagesByGroup, (LPARAM)pNewSpeechPage))
      // [CONFLICT] Discard new page
      deleteMediaPage(pNewSpeechPage);
}


/// Function name  : insertSoundEffectItemIntoGameData
// Description     : Attempts to create an insert a new SoundEffect into the GameData
//  
// CONST UINT     iPageID        : [in]  ID of the page to store SoundEffect under
// CONST UINT     iID            : [in]  SoundEffect ID
// CONST TCHAR*   szDescription  : [in]  SoundEffect description
// ERROR_STACK*  &pError         : [out] Error message describing the conflict, if any, otherwise NULL
// 
// Return Value   : TRUE if added successfully, otherwise FALSE
// 
BOOL  insertSoundEffectItemIntoGameData(CONST UINT  iPageID, CONST UINT  iID, CONST TCHAR*  szDescription, ERROR_STACK*  &pError)
{
   MEDIA_ITEM    *pNewSoundEffect,
                 *pExistingSoundEffect;

   // Prepare
   pError = NULL;

   /// Create new sound effect item
   pNewSoundEffect = createSoundEffectItem(iPageID, iID, szDescription);

   /// Attempt to insert into GameData
   if (!insertObjectIntoAVLTree(getGameData()->pMediaItemsByID, (LPARAM)pNewSoundEffect))
   {
      // [CONFLICT] Lookup conflicting item
      if (findObjectInAVLTreeByValue(getGameData()->pMediaItemsByID, pNewSoundEffect->iPageID, pNewSoundEffect->iID, (LPARAM&)pExistingSoundEffect))
      {
         // [CHECK] Skip generating warnings for old SFXs that had no description
         if (pExistingSoundEffect->szDescription)
            // [WARNING] "The sound effect for '%s' (ID:%u) has overwritten the previous entry for '%s' with the same ID"
            pError = generateDualWarning(HERE(IDS_MEDIA_SFX_DUPLICATE_ITEM), pNewSoundEffect->szDescription, pNewSoundEffect->iID, pExistingSoundEffect->szDescription);

         // Replace conflicting item
         destroyObjectInAVLTreeByValue(getGameData()->pMediaItemsByID, pExistingSoundEffect->iPageID, pExistingSoundEffect->iID);
         insertObjectIntoAVLTree(getGameData()->pMediaItemsByID, (LPARAM)pNewSoundEffect);
      }
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : insertSpeechClipIntoSpeechFile
// Description     : Attempts to insert a new speech clip into the GameData. 
///                                                Existing speech clips are overwritten
// 
// SPEECH_FILE*  pSpeechFile : [in] Speech file to insert clip into
// CONST UINT    iPageID     : [in] ID of the page containing the new MediaItem
// CONST UINT    iItemID     : [in] ID of the new MediaItem
// CONST UINT    iPosition   : [in] Position within the audio file
// CONST UINT    iLength     : [in] Length of the clip in milliseconds
// 
VOID   insertSpeechClipIntoSpeechFile(SPEECH_FILE*  pSpeechFile, CONST UINT  iPageID, CONST UINT  iItemID, CONST UINT  iPosition, CONST UINT  iLength)
{                  
   MEDIA_ITEM   *pNewSpeechClip,        // Item being created and inserted
                *pExistingSpeechClip;   // Pre-existing conflicting item
   
   /// Create new speech clip
   pNewSpeechClip = createSpeechClipItem(iPageID, iItemID, iPosition, iLength);

   /// Attempt to insert into GameData tree
   if (!insertObjectIntoAVLTree(pSpeechFile->pSpeechClipsByID, (LPARAM)pNewSpeechClip))
   {
      // Lookup conflicting item
      findObjectInAVLTreeByValue(pSpeechFile->pSpeechClipsByID, pNewSpeechClip->iPageID, pNewSpeechClip->iID, (LPARAM&)pExistingSpeechClip);
      ASSERT(pExistingSpeechClip);

      /// [CONFLICT] Update existing item with new values
      pExistingSpeechClip->iPosition = iPosition;
      pExistingSpeechClip->iLength   = iLength;
      
      // Cleanup
      deleteMediaItem(pNewSpeechClip);
   }
}


/// Function name  : insertVideoClipItemIntoGameData
// Description     : Attempts to create a new video clip and insert it into the GameData.
// 
// CONST UINT             iPageID       : [in]  Page in which to store the video file
// CONST UINT             iItemID       : [in]  Video clip ID
// CONST TCHAR*           szDescription : [in]  Video clip description
// CONST MEDIA_POSITION*  pStartPos     : [in]  Start position of the video clip (within the main video file)
// CONST MEDIA_POSITION*  pEndPos       : [in]  End position of the video clip (within the main video file)
// ERROR_STACK*          &pError        : [out] Error message describing the conflict, if any, otherwise NULL
// 
// Return Value   : TRUE if inserted successfully, otherwise FALSE
// 
BOOL   insertVideoClipItemIntoGameData(CONST UINT  iPageID, CONST UINT  iVideoID, CONST TCHAR*  szDescription, CONST MEDIA_POSITION*  pStartPos, CONST MEDIA_POSITION*  pEndPos, ERROR_STACK*  &pError)
{                  
   MEDIA_ITEM   *pNewVideoClip,        // Item being created and inserted
                *pConflictItem;   // Pre-existing conflicting video
   // Prepare
   pError = NULL;

   /// Create new MediaItem
   pNewVideoClip = createVideoClipItem(iPageID, iVideoID, szDescription, pStartPos, pEndPos);

   /// Attempt to insert into GameData tree
   if (!insertObjectIntoAVLTree(getGameData()->pMediaItemsByID, (LPARAM)pNewVideoClip))
   {
      // [CONFLICT] Lookup conflicting item
      if (findObjectInAVLTreeByValue(getGameData()->pMediaItemsByID, iPageID, iVideoID, (LPARAM&)pConflictItem))
         // [WARNING] "Cannot add a video clip for '%s' into media page %u because it conflict with the %s with ID %u"
         pError = generateDualWarning(HERE(IDS_MEDIA_VIDEO_DUPLICATE_ITEM), szDescription, iPageID, identifyMediaItemType(pConflictItem), pConflictItem->iID);
      
      // Cleanup
      deleteMediaItem(pNewVideoClip);
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : loadMediaItemTrees
// Description     : Build the MediaItem game trees from the speech, sound effect and video definition files
// 
// CONST FILE_SYSTEM*  pFileSystem : [in]     FileSystem object
// HWND                hParentWnd  : [in]     Parent window for any error dialogs displayed
// OPERATION_PROGRESS* pProgress   : [in/out] Operation progress
// ERROR_QUEUE*        pErrorQueue : [out]    Error message, if any
//
// Return type : OR_SUCCESS - MediaItem trees were created successfully, the user ignored any minor errors
//               OR_FAILURE - MediaItem trees were NOT created due to critical errors (missing files)
//               OR_ABORTED - MediaItem trees were NOT created because the user aborted after minor errors
// 
OPERATION_RESULT  loadMediaItemTrees(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT  eResult;             // Operation result
   SPEECH_FILE*      pSpeechFile;         // Used for parsing the speech definition file. Also conveniently creates the media trees.
   TCHAR*            szSubPath;

   __try
   {
      CONSOLE_STAGE();
      CONSOLE("Generating SpeechClip MediaItems from file '%s'", szSubPath = generateGameLanguageFileSubPath(GFI_SPEECH_CLIPS, getAppPreferences()->eGameVersion, getAppPreferences()->eGameLanguage));

      // [INFO/PROGRESS] "Loading speech clips from '%s'" 
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_SPEECH_CLIPS), szSubPath));
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_SPEECH_CLIPS);

      // [CHECK] MediaItem trees don't exist yet
      ASSERT(getGameData()->pMediaItemsByID == NULL AND getGameData()->pMediaPagesByGroup == NULL);

      /// Create SpeechFile
      pSpeechFile = createLanguageFile(LFT_SPEECH, NULL, FALSE);

      /// Load Speech Clips
      if ((eResult = loadLanguageFile(pFileSystem, pSpeechFile, FALSE, hParentWnd, pProgress, pErrorQueue)) == OR_SUCCESS)
      {
         /// Extract the SpeechFile's trees as the basis for the media game data
         transferAVLTree(pSpeechFile->pSpeechClipsByID, getGameData()->pMediaItemsByID);
         transferAVLTree(pSpeechFile->pSpeechPagesByGroup, getGameData()->pMediaPagesByGroup);

         // [VERBOSE]
         CONSOLE_HEADING("Loaded %d speech clips over %d media pages", getTreeNodeCount(getGameData()->pMediaItemsByID), getTreeNodeCount(getGameData()->pMediaPagesByGroup));

         // [PROGRESS] Move to 'Loading Media' stage. 
         ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_MEDIA_ITEMS);
         updateOperationProgressMaximum(pProgress, 10);    // Define progress as percentage.

         /// Load Sound Effects
         if ((eResult = loadMediaSoundEffectItems(pFileSystem, hParentWnd, pErrorQueue)) == OR_SUCCESS)
         {
            // [PROGRESS] Manually update progress
            updateOperationProgressValue(pProgress, 3);
         
            /// Load Video Clips
            if ((eResult = loadMediaVideoClipItems(pFileSystem, hParentWnd, pErrorQueue)) == OR_SUCCESS)
            {
               // [PROGRESS] Manually update progress
               updateOperationProgressValue(pProgress, 6);

               /// Index media data trees
               performAVLTreeIndexing(getGameData()->pMediaItemsByID);
               updateOperationProgressValue(pProgress, 8);
               
               /// Index groups
               performAVLTreeIndexing(getGameData()->pMediaPagesByGroup);
               updateOperationProgressValue(pProgress, 10);
            }
         }
      }

      /// Print result to console
      CONSOLE_HEADING("Loaded %d media items over %d media pages", getTreeNodeCount(getGameData()->pMediaItemsByID), getTreeNodeCount(getGameData()->pMediaPagesByGroup)); 

      // Cleanup and return result
      utilDeleteString(szSubPath);
      deleteLanguageFile(pSpeechFile);
      return eResult;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION("Unable to load media items");
      return OR_FAILURE;
   }
}



/// Function name  : loadMediaSoundEffectItems
// Description     : Add all the entries in the SFX definition file to the MediaItem tree. Spread them out
//                      over seven MediaPages (for readability) defined by the SFX IDs.
// 
// CONST FILE_SYSTEM*  pFileSystem : [in]     FileSystem object
// HWND                hParentWnd  : [in]     Parent window for any error dialogs that are displayed
// ERROR_QUEUE*        pErrorQueue : [out]    Error messages, if any
// 
// Return Value : OR_SUCCESS - The SFX entries were successfully added to the MediaItems tree.
//                OR_FAILURE - No SFX entries were added to the MediaItem tree because the definition file could not be opened.
//                OR_ABORTED - Some SFX entries were added to the MediaItem tree but the user aborted after minor errors
//
OPERATION_RESULT  loadMediaSoundEffectItems(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR         *szProperties[iSoundEffectPropertiesCount];     // Current line split into individual properties
   OPERATION_RESULT     eResult = OR_SUCCESS; // Operation result
   ERROR_STACK         *pError;               // Operation error
   GAME_FILE           *pSoundEffectFile;     // GameFile containing the sound effects list file
   CONST TCHAR         *szFileSubPath;        // SubPath of file, used for error reporting
   UINT                 iLineNumber,          // Line number of the current line (used for error reporting)
                        iPropertyCount,       // Number of properties found on the current line
                        iEffectID,            // ID of the effect on the current line
                        iPageID,              // PageID current effect will be saved under
                        iEffectsLoaded;       // Number of sound effects loaded
   TCHAR               *szLine,               // Current Line token
                       *szProperty,           // Current Property token
                       *pLineEnd,             // Line tokeniser data
                       *pPropertyEnd,         // Property tokeniser data
                       *szPageTitle;          // Title for the page current effect will be saved under

   __try
   {
      // [INFO] "Loading sound effects from '%s'"
      CONSOLE_STAGE();
      CONSOLE("Generating SoundEffect MediaItems from file '%s'", szFileSubPath = findGameDataFileSubPathByID(GFI_SOUND_EFFECTS, getAppPreferences()->eGameVersion));
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_SOUND_EFFECTS), szFileSubPath));

      // Prepare
      pSoundEffectFile = createGameFileFromSubPath(szFileSubPath);
      szPageTitle      = utilCreateEmptyString(32);

      /// Attempt to load SFX file
      if (!loadGameFileFromFileSystemByPath(pFileSystem, pSoundEffectFile, TEXT(".txt"), pErrorQueue))
      {
         // No enhancement necessary
         generateOutputTextFromLastError(pErrorQueue);         // [ERROR] "The game sound effects definition file '%s' is missing or could not be accessed"
         eResult = OR_FAILURE;                                 // enhanceLastError(pErrorQueue, ERROR_ID(IDS_MEDIA_SFX_FILE_IO_ERROR), szFileSubPath);
      }
      else
      {
         // Prepare
         iPageID = iEffectID = iEffectsLoaded = iLineNumber = 0;
         pError  = NULL;

         /// Tokenise file into lines
         for (szLine = utilTokeniseString(pSoundEffectFile->szInputBuffer, "\r\n", &pLineEnd); szLine; szLine = utilGetNextToken("\r\n", &pLineEnd))
         {
            // Increment the line number used for error reporting
            iLineNumber++;

            // [COMMENTS] Skip commented lines
            if (utilCompareStringN(szLine, "//", 2))
               continue;

            // Prepare properties array
            utilZeroObjectArray(szProperties, TCHAR*, iSoundEffectPropertiesCount);
            iPropertyCount = 0;

            /// Split line into up to 8 properties
            for (szProperty = utilTokeniseString(szLine, ";", &pPropertyEnd); szProperty; szProperty = utilGetNextToken(";", &pPropertyEnd))
            {
               szProperties[iPropertyCount++] = szProperty;
               if (iPropertyCount == iSoundEffectPropertiesCount)
                  break;
            }

            // Identify line from the number of properties it contains
            switch (iPropertyCount)
            {
            /// [FILE VERSION] -- Ignore
            case 1:
               break;

            /// [GROUP DECLARATION] -- Store as a media page
            case 2:  // Group declaration without trailing whitespace
            case 3:  // Group declaration with trailing whitespace
               // Generate group title and reset effect ID
               iEffectID = 0;
               iPageID   = utilConvertStringToInteger(szProperties[0]);
               StringCchPrintf(szPageTitle, 32, TEXT("%u Series"), iPageID);

               // Create media page and attempt to insert into game data
               insertMediaPageIntoGameData(MIT_SOUND_EFFECT, iPageID, szPageTitle, pError);
               break;

            /// [SOUND EFFECT] -- Store as a media item
            case 7:  // Sound effect definition without comment
            case 8:  // Sound effect definition with comment
               // Cleanup description. Replace empty descriptions with 'None'
               StrTrim((TCHAR*)szProperties[SEI_DESCRIPTION], TEXT("/ "));
               if (lstrlen(szProperties[SEI_DESCRIPTION]) == 0)
                  szProperties[SEI_DESCRIPTION] = TEXT("[No Description]");

               // Attempt to insert SoundEffect
               insertSoundEffectItemIntoGameData(iPageID, iEffectID + iPageID, szProperties[SEI_DESCRIPTION], pError);
               iEffectsLoaded++;

               // Increment effect id iterator
               iEffectID++;
               break;
            
            /// [UNKNOWN] -- Malformed entry
            default:
               // [ERROR] "There are an unrecognised number of properties (%u) in the sound effect definition on line %u of '%s'"
               pError = generateDualError(HERE(IDS_MEDIA_SFX_ENTRY_INVALID), iPropertyCount, iLineNumber, szFileSubPath);
               break;

            } // END : switch (number of properties)

            // [ERROR] Store error and determine whether to abort
            if (pError)
            {
               // Store current error and select as the output dialog text
               generateOutputTextFromError(pError);
               pushErrorQueue(pErrorQueue, pError);
               pError = NULL;
            }

         } // END : for (each line in TFile)
      }

      /// Print results
      CONSOLE_HEADING("Loaded %d sound effects", iEffectsLoaded);

      // Cleanup and return result
      utilDeleteString(szPageTitle);
      deleteGameFile(pSoundEffectFile);
      return eResult;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION1("Unable to load sound effects: Error processing '%s'", szLine);
      return OR_FAILURE;
   }
}


/// Function name  : loadMediaVideoClipItems
// Description     : Load the video clips file into the media items tree. 
//                     Manually split them into categories - 'Characters' and 'Wares' using ID < 1000.
// 
// CONST FILE_SYSTEM* pFileSystem : [in]  FileSystem object
// HWND               hParentWnd  : [in]  Parent window for any error dialogs that are displayed
// ERROR_QUEUE*       pErrorQueue : [out] Error messages, if any
// 
// Return Value : OR_SUCCESS - All valid MediaItems were added to the game data, any errors were ignored by the user
//                OR_FAILURE - No MediaItems were added to the game data because of a critical error (missing file)
//                OR_ABORTED - Some MediaItems were added to the game data but the user aborted due to a minor error
//
OPERATION_RESULT  loadMediaVideoClipItems(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR         *szProperties[iVideoPropertiesCount];    // Current line split into individual properties
   OPERATION_RESULT     eResult = OR_SUCCESS;// Operation result
   ERROR_STACK         *pError;              // Operation error
   GAME_FILE           *pVideoClipFile;      // GameFile containing the video clip list
   CONST TCHAR         *szFileSubPath;       // File subpath, used for error reporting
   UINT                 iLineNumber = 0,     // Current line number (for error reporting)
                        iPropertyCount,      // Number of properties found on the current line
                        iVideoCount,         // Number of video clips in the clip list
                        iVideosLoaded = 0,   // Number of video clips successfully loaded
                        iVideoID,            // ID of the current video clip, used for determining media page
                        iPageID;             // Destination page for the current video clip, based on it's video ID above
   MEDIA_POSITION       oVideoStartPos,      // Video Clip start position
                        oVideoFinishPos;     // Video clip end position
   TCHAR               *szLine,              // Current Line token
                       *szProperty,          // Current Property token
                       *pLineEnd,            // Line tokeniser data
                       *pPropertyEnd;        // Property tokeniser data
   
   __try
   {
      // [INFO] "Loading video clips from '%s'"
      CONSOLE_STAGE();
      CONSOLE("Generating VideoClip MediaItems from file '%s'", szFileSubPath = findGameDataFileSubPathByID(GFI_VIDEO_CLIPS, getAppPreferences()->eGameVersion));
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_VIDEO_CLIPS), szFileSubPath));

      /// Attempt to load video listing file
      if (!loadGameFileFromFileSystemByPath(pFileSystem, pVideoClipFile = createGameFileFromSubPath(szFileSubPath), TEXT(".txt"), pErrorQueue))
      {
         // No enhancement necessary
         generateOutputTextFromLastError(pErrorQueue);         // [ERROR] "The game video clips definition file '%s%s' is missing or could not be accessed"
         eResult = OR_FAILURE;                                 // enhanceLastError(pErrorQueue, ERROR_ID(IDS_MEDIA_VIDEO_FILE_IO_ERROR), pFileSystem->szGameFolder, szFileSubPath);
      }
      else
      {
         // Manually add the 'Characters' and 'Wares' pages
         insertMediaPageIntoGameData(MIT_VIDEO_CLIP, MPI_VIDEO_AVATARS, TEXT("Transmission Avatars"), pError);
         insertMediaPageIntoGameData(MIT_VIDEO_CLIP, MPI_VIDEO_WARES, TEXT("Wares and weapons"), pError);
         
         /// Tokenise file into lines
         for (szLine = utilTokeniseString(pVideoClipFile->szInputBuffer, "\r\n", &pLineEnd); szLine; szLine = utilGetNextToken("\r\n", &pLineEnd))
         {
            // Count lines (for error reporting)
            iLineNumber++;

            // [CHECK] Skip commented lines
            if (utilCompareStringN(szLine, "//", 2))
               continue;

            // Prepare properties array
            utilZeroObjectArray(szProperties, TCHAR*, iVideoPropertiesCount);
            iPropertyCount = 0;

            /// Split line into up to 12 properties
            for (szProperty = utilTokeniseString(szLine, ";", &pPropertyEnd); szProperty AND (iPropertyCount < iVideoPropertiesCount); szProperty = utilGetNextToken(";", &pPropertyEnd))
               szProperties[iPropertyCount++] = szProperty;
            
            // Identify line from the number of properties it contains
            switch (iPropertyCount)
            {
            /// [VIDEO CLIP COUNT] -- Store
            case 1:
               iVideoCount = utilConvertStringToInteger(szProperties[0]);
               continue;

            /// [VIDEO CLIP] -- Store as a video media item
            case 12:
               // [CHECK] Ignore VideoClips from 'Stream 2' for the moment - they're the split screen movies. I don't understand their declarations
               if (utilConvertStringToInteger(szProperties[VCI_STREAM_ID]) == 2)
                  continue;

               // Extract start position
               oVideoStartPos.iMinutes       = utilConvertStringToInteger(szProperties[VCI_START_MINUTES]);
               oVideoStartPos.iSeconds       = utilConvertStringToInteger(szProperties[VCI_START_SECONDS]);
               oVideoStartPos.iMilliseconds  = utilConvertStringToInteger(szProperties[VCI_START_MILLISECONDS]);
               // Extract finish position
               oVideoFinishPos.iMinutes      = utilConvertStringToInteger(szProperties[VCI_FINISH_MINUTES]);
               oVideoFinishPos.iSeconds      = utilConvertStringToInteger(szProperties[VCI_FINISH_SECONDS]);
               oVideoFinishPos.iMilliseconds = utilConvertStringToInteger(szProperties[VCI_FINISH_MILLISECONDS]);

               // Determine video and page ID
               iVideoID = utilConvertStringToInteger(szProperties[VCI_ID]);
               iPageID  = (iVideoID < 1000 ? MPI_VIDEO_AVATARS : MPI_VIDEO_WARES);

               // Cleanup description. Replace empty descriptions with 'None'
               StrTrim((TCHAR*)szProperties[VCI_DESCRIPTION], TEXT("/ )>"));
               if (lstrlen(szProperties[VCI_DESCRIPTION]) == 0)
                  szProperties[VCI_DESCRIPTION] = TEXT("[No Description]");

               // Attempt to insert new VideoClip item
               insertVideoClipItemIntoGameData(iPageID, iVideoID, szProperties[VCI_DESCRIPTION], &oVideoStartPos, &oVideoFinishPos, pError);
               iVideosLoaded++;
               break;

            /// [UNKNOWN ENTRY] -- Determine whether to continue or abort
            default:
               // [ERROR] "There are an unrecognised number of properties (%u) in the video clip definition on line %u of '%s%s'"
               pError = generateDualError(HERE(IDS_MEDIA_VIDEO_ENTRY_INVALID), iPropertyCount, iLineNumber, getFileSystem()->szGameFolder, szFileSubPath);
               break;
            } // END: switch (number of properties)

            // [ERROR] Use error message as the output text, then add it to the error queue
            if (pError)
            {
               generateOutputTextFromError(pError);
               pushErrorQueue(pErrorQueue, pError);
               pError = NULL;
            }

         } // END: for (each line)
      } // END: if (opened succesfully)

      /// Print result
      CONSOLE("Loaded %d video clips from stream 1, %d clips from stream 2 were dropped.", iVideosLoaded, iVideosLoaded - iVideoCount);

      // Cleanup and return result
      deleteGameFile(pVideoClipFile);
      return eResult;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION1("Unable to load video clips: Error processing '%s'", szLine);
      return OR_FAILURE;
   }
}

