//
// BearScript Util.h : Declarations for the utility functions
//

#pragma once

// Includes
#include "Types.h"
#include "Windows API.h"
#include "../Build Rules.h"

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          EXPORT
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: _UtilityDLL
// Description: Witholds or exposes parts of the utility library
//
#ifdef _UtilityDLL
   #define UtilityAPI __declspec(dllexport)
#else
   #define UtilityAPI __declspec(dllimport)
#endif

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: MEGABYTE
//  Description: Convenience description of a megabyte
//
#define      MEGABYTE                 (1024*1024)

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: WIDEN(x)  
// Description: Creates a wide char equivilent of an input string (from MSDN)
// 
#define      WIDEN2(x)                L ## x
#define      WIDEN(x)                 WIDEN2(x)

/// Macro: __WFILE__
// Description: Wide character equivilent of __FILE__
//
#define      __WFILE__                WIDEN(__FILE__)

/// Macro: __WFUNCTION__
// Description: Wide character equivilent of __FUNCTION__
//
#define      __WFUNCTION__            WIDEN(__FUNCTION__)

/// Macro: __SWFILE__
// Description: Extracts only the filename from the '__FILE__' macro.         NOTE: /FC switch should achieve this, but doesn't.
//
#define      __SWFILE__               (_tcsrchr(__WFILE__, '\\') + 1)

/// Macro: ASSERT
// Description: Causes a breakpoint if the expression evaluates to FALSE
//
// ... xExpression : Any expression
//
#ifdef _DEBUG
#define      ASSERT(xExpression)      if ((xExpression) == FALSE)  DebugBreak();
#else
#define      ASSERT(xExpression)      if ((xExpression) == FALSE)  RaiseException(EXCEPTION_BREAKPOINT, EXCEPTION_NONCONTINUABLE, (lstrlenA(#xExpression) + 1), (ULONG_PTR*)utilCreateExceptionString(_T(#xExpression), __WFUNCTION__, __LINE__, __WFILE__));
#endif

/// Macro: TODO
// Description: Causes a breakpoint at the specified line and indicates why
//
// CONST CHAR*  szTask : ANSI string containing the required task
//
#define      TODO(szTask)             ASSERT(szTask == FALSE)

/// Macro: OR
// Description: Convenience description for logical OR
#define      OR                       ||

/// Macro: AND
// Description: Convenience description for logical AND
#define      AND                      &&

/// Macro: INCLUDES
// Description: Convenience description for bitwise AND
#define      INCLUDES                 &

/// Macro: WITH
// Description: Convenience description for bitwise OR
#define      WITH                     |

/// Name: MAKE_LONG
//  Description: Replacement macro for MAKELONG indicating which parameter is which
//
//  UINT  iLowWord  : [in] WORD to place in the LO word
//  UINT  iHighWord : [in] WORD to place in the HO word
//
#define      MAKE_LONG(iLowWord,iHighWord)           MAKELONG(iLowWord,iHighWord)

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            COM
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Convenience macros
#define             OLE_Method                     __declspec(nothrow)
#define             OLE_Declaration(xReturnType)   virtual __declspec(nothrow) xReturnType __stdcall

// Convenience descriptions
typedef              VOID*                          IInterface;

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          DRAWING
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Functions
UtilityAPI DC_STATE*   utilCreateDeviceContextState(HDC  hDC);
UtilityAPI HIMAGELIST  utilCreateImageList(HINSTANCE  hInstance, CONST UINT  iIconSize, CONST UINT  iIconCount, ...);
UtilityAPI VOID        utilDeleteDeviceContextState(DC_STATE*  &pPreviousState);
UtilityAPI VOID        utilRestoreDeviceContext(DC_STATE*  pPrevState);
UtilityAPI VOID        utilSetDeviceContextBackgroundColour(DC_STATE*  pPrevState, CONST COLORREF  clBackground);
UtilityAPI VOID        utilSetDeviceContextBackgroundMode(DC_STATE*  pPrevState, CONST UINT  iMode);
UtilityAPI VOID        utilSetDeviceContextBitmap(DC_STATE*  pPrevState, HBITMAP  hBitmap);
UtilityAPI VOID        utilSetDeviceContextBrush(DC_STATE*  pPrevState, HBRUSH  hBrush);
UtilityAPI VOID        utilSetDeviceContextFont(DC_STATE*  pPrevState, HFONT  hFont, COLORREF  clColour);
UtilityAPI VOID        utilSetDeviceContextPen(DC_STATE*  pPrevState, HPEN  hPen);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       LISTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
UtilityAPI LIST_ITEM*  createListItem(LPARAM  oItemData);

// (Exported) Creation / Destruction
UtilityAPI LIST*       createList(DESTRUCTOR  pfnDeleteItem);
UtilityAPI VOID        deleteList(LIST*  &pList);
UtilityAPI VOID        deleteListContents(LIST*  pList);
UtilityAPI VOID        deleteListItem(CONST LIST*  pList, LIST_ITEM*  &pItem, CONST BOOL  bContents);
UtilityAPI VOID        destructorSimpleObject(LPARAM  pObject);

// (Exported) Helpers
UtilityAPI UINT        getListItemCount(CONST LIST*  pList);
UtilityAPI LIST_ITEM*  getListHead(CONST LIST*  pList);
UtilityAPI LIST_ITEM*  getListTail(CONST LIST*  pList);

// (Exported) Functions
UtilityAPI VOID   appendItemToList(LIST*  pList, LIST_ITEM*  pNewItem);
UtilityAPI VOID   appendObjectToList(LIST*  pList, LPARAM  pObject);
UtilityAPI BOOL   destroyListItemByIndex(LIST*  pList, CONST UINT  iIndex);
UtilityAPI UINT   findListIndexByValue(CONST LIST*  pList, LPARAM  xValue);
UtilityAPI BOOL   findListItemByIndex(CONST LIST*  pList, UINT  iIndex, LIST_ITEM*  &pOutput);
UtilityAPI BOOL   findListItemByValue(CONST LIST*  pList, LPARAM  xValue, LIST_ITEM*  &pOutput);
UtilityAPI BOOL   findListObjectByIndex(CONST LIST*  pList, UINT  iIndex, LPARAM  &pOutput);
UtilityAPI BOOL   insertObjectIntoListByIndex(LIST*  pList, CONST UINT  iIndex, LPARAM  pData);
UtilityAPI BOOL   insertListIntoListByIndex(LIST*  pTargetList, CONST UINT  iTargetIndex, LIST*  pSourceList, CONST UINT  iSourceIndex);
UtilityAPI BOOL   isValueInList(CONST LIST*  pList, LPARAM  xValue);
UtilityAPI BOOL   removeItemFromList(LIST*  pList, LIST_ITEM*  pItem);
UtilityAPI BOOL   removeItemFromListByIndex(LIST*  pList, CONST UINT  iIndex, LIST_ITEM*  &pOutput);
UtilityAPI BOOL   removeObjectFromListByValue(LIST*  pList, LPARAM  xValue);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       LIST ITERATOR
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
UtilityAPI LIST_ITERATOR*   createListIterator(CONST LIST*  pList);
UtilityAPI VOID             deleteListIterator(LIST_ITERATOR*  &pListIterator);

// Helpers
UtilityAPI BOOL   hasCurrentItem(CONST LIST_ITERATOR*  pListIterator);
UtilityAPI BOOL   hasNextItem(CONST LIST_ITERATOR*  pListIterator);
UtilityAPI BOOL   hasPrevItem(CONST LIST_ITERATOR*  pListIterator);

// Functions
UtilityAPI BOOL  getCurrentItem(CONST LIST_ITERATOR*  pListIterator, LPARAM&  pOutput);
UtilityAPI BOOL  moveNextItem(LIST_ITERATOR*  pListIterator);
UtilityAPI BOOL  movePrevItem(LIST_ITERATOR*  pListIterator);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MATHS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Functions
//UtilityAPI UINT      utilCalculateMagnitude(CONST INT  iNumber);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MEMORY
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
UtilityAPI VOID*     utilAllocateMemory(CONST UINT  iBytes);
UtilityAPI VOID*     utilAllocateEmptyMemory(CONST UINT  iBytes);
UtilityAPI VOID      utilDeleteWindow(HWND&  hWnd);
UtilityAPI VOID      utilFreeMemory(VOID*  &pObject);

// Functions
UtilityAPI VOID      utilCopyMemory(VOID*  pDestination, VOID*  pSource, CONST UINT  iBytes);
UtilityAPI BOOL      utilValidateMemory();
UtilityAPI VOID      utilZeroMemory(VOID*  pMemory, CONST UINT  iBytes);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          PATHS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Functions
UtilityAPI TCHAR*    utilDuplicatePath(CONST TCHAR*  szSource);
UtilityAPI TCHAR*    utilDuplicateFolderPath(CONST TCHAR*  szFullPath);
UtilityAPI TCHAR*    utilGenerateAppFilePath(CONST TCHAR*  szFileName);
UtilityAPI TCHAR*    utilRenameFilePath(CONST TCHAR*  szFullPath, CONST TCHAR*  szNewFileName);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       QUEUES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
#define             createQueue                createList
#define             createQueueItem            createListItem
#define             deleteQueue                deleteList
#define             deleteQueueItem            deleteListItem

// Helpers
#define             getQueueItemCount          getListItemCount

// Functions
UtilityAPI VOID      pushLastQueueObject(QUEUE*  pQueue, LPARAM  pObject);
UtilityAPI LPARAM    popLastQueueObject(QUEUE*  pQueue);
UtilityAPI LPARAM    popFirstQueueObject(QUEUE*  pQueue);
UtilityAPI LPARAM    firstObjectFromQueue(CONST QUEUE*  pQueue);
UtilityAPI LPARAM    lastObjectFromQueue(CONST QUEUE*  pQueue);

/// Name: extractQueueItemPointer
// Description: Convenience macro for extracting an object pointer from a QueueItem
//
#define             extractQueueItemPointer     extractListItemPointer

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          REGISTRY
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Functions
UtilityAPI BOOL      utilRegistryCheckAppKeyExists(CONST TCHAR*  szAppName);
UtilityAPI VOID      utilRegistryCloseKey(HKEY&  hKey);
UtilityAPI HKEY      utilRegistryCreateAppKey(CONST TCHAR*  szAppName);
UtilityAPI HKEY      utilRegistryCreateAppSubKey(CONST TCHAR*  szAppName, CONST TCHAR*  szSubKey);
UtilityAPI BOOL      utilRegistryDeleteAppKey(CONST TCHAR*  szAppName);
UtilityAPI BOOL      utilRegistryDeleteAppSubKey(CONST TCHAR*  szAppName, CONST TCHAR*  szSubKey);
UtilityAPI HKEY      utilRegistryOpenAppKey(CONST TCHAR*  szAppName);
UtilityAPI HKEY      utilRegistryOpenAppSubKey(CONST TCHAR*  szAppName, CONST TCHAR*  szSubKey);
UtilityAPI BOOL      utilRegistryReadBinary(HKEY  hKey, CONST TCHAR*  szName, BYTE*  pBuffer, DWORD  iBufferSize);
UtilityAPI BOOL      utilRegistryReadInteger(HKEY  hKey, CONST TCHAR*  szName, UINT*  piOutput);
UtilityAPI BOOL      utilRegistryReadString(HKEY  hKey, CONST TCHAR*  szName, TCHAR*  szBuffer, UINT  iBufferLength);
UtilityAPI BOOL      utilRegistryQueryValueType(HKEY  hKey, CONST TCHAR*  szValue, UINT*  piType);
UtilityAPI BOOL      utilRegistryWriteBinary(HKEY  hKey, CONST TCHAR*  szName, CONST BYTE*  pBuffer, CONST DWORD  iBufferSize);
UtilityAPI BOOL      utilRegistryWriteInteger(HKEY  hKey, CONST TCHAR*  szName, CONST UINT  iValue);
UtilityAPI BOOL      utilRegistryWriteString(HKEY  hKey, CONST TCHAR*  szName, CONST TCHAR*  szString);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       STACKS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
#define             createStack                createList
#define             createStackItem            createListItem
#define             deleteStack                deleteList
#define             deleteStackItem            deleteListItem

// Helpers
#define             getStackItemCount          getListItemCount

// Functions
UtilityAPI BOOL      destroyTopStackObject(STACK*  pStack);
UtilityAPI VOID      pushObjectOntoStack(STACK*  pStack, LPARAM  pObject);
UtilityAPI LPARAM    popObjectFromStack(STACK*  pStack);
UtilityAPI LPARAM    popObjectFromStackBottom(STACK*  pStack);
UtilityAPI LPARAM    topStackObject(CONST STACK*  pStack);

/// Name: extractStackItemPointer
// Description: Convenience macro for extracting an object pointer from a StackItem
//
#define             extractStackItemPointer     extractListItemPointer

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          STRINGS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation
UtilityAPI TCHAR*    utilCreateEmptyString(CONST UINT  iBufferSizeInChars);
UtilityAPI CHAR*     utilCreateEmptyStringA(CONST UINT  iBufferSizeInChars);
UtilityAPI TCHAR*    utilCreateString(CONST UINT  iBufferSizeInChars);
UtilityAPI TCHAR*    utilCreateStringf(CONST UINT  iLength, CONST TCHAR*  szFormat, ...);
UtilityAPI TCHAR*    utilCreateStringFromInteger(CONST INT  iValue);
UtilityAPI TCHAR*    utilCreateStringVf(CONST UINT  iLength, CONST TCHAR*  szFormat, va_list  pArgList);
UtilityAPI TCHAR*    utilDuplicateString(CONST TCHAR*  szSource, CONST UINT  iSourceLength);
UtilityAPI TCHAR*    utilDuplicateSubString(CONST TCHAR*  szSource, CONST UINT  iSourceLength, CONST UINT  iOffset, CONST UINT  iSubStringLength);
UtilityAPI TCHAR*    utilLoadString(HINSTANCE  hInstance, CONST UINT  iResourceID, UINT  iBufferLength = NULL);

// Destruction
UtilityAPI VOID      utilDeleteString(TCHAR*  &szStringBuffer);
UtilityAPI VOID      utilDeleteStringA(CHAR*  &szStringBuffer);
UtilityAPI VOID      utilDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString);
UtilityAPI VOID      utilDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString, TCHAR*  &szThirdString);
UtilityAPI VOID      utilSafeDeleteString(TCHAR*  &szStringBuffer);
UtilityAPI VOID      utilSafeDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString);
UtilityAPI VOID      utilSafeDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString, TCHAR*  &szThirdString);

// Functions
UtilityAPI TCHAR*    utilExtendString(TCHAR*  szSource, CONST UINT  iSourceLength, CONST UINT  iNewLength);
UtilityAPI TCHAR*    utilReAllocString(TCHAR*  szSource);
UtilityAPI TCHAR*    utilReplaceString(TCHAR*  &szExistingString, CONST TCHAR*  szNewString);
UtilityAPI TCHAR*    utilReplaceStringf(TCHAR*  &szExistingString, CONST UINT  iNewLength, CONST TCHAR*  szFormat, ...);
UtilityAPI HRESULT   utilStringCchCatf(TCHAR*  szDestination, CONST UINT  iLength, CONST TCHAR*  szFormat, ...);
UtilityAPI TCHAR*    utilTranslateStringToUNICODE(CONST CHAR*  szText, CONST UINT  iTextLength);
UtilityAPI TCHAR*    utilTranslateStringToUNICODEEx(CONST CHAR*  szText, UINT&  iTextLength);
UtilityAPI CHAR*     utilTranslateStringToANSI(CONST TCHAR*  szText, UINT&  iTextLength, CONST UINT  iBufferExtension = 115);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          SYSTEM
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
UtilityAPI EXCEPTION_STRING*  utilCreateExceptionString(CONST TCHAR*  szAssertion, CONST TCHAR*  szFunction, CONST UINT  iLine, CONST TCHAR*  szFileName);
UtilityAPI VOID               utilDeleteExceptionString(EXCEPTION_STRING*  &pExceptionString);

// Functions
UtilityAPI VOID               utilCloseHandle(HANDLE&  pHandle);
UtilityAPI BOOL               utilInitProcessImageList();
UtilityAPI BOOL               utilGetCachedIconIndex(CONST TCHAR*  szPath, INT  iIconIndex, UINT  iFlags);
UtilityAPI WINDOWS_VERSION    utilGetWindowsVersion();
UtilityAPI TCHAR*             utilGetWindowsVersionString();
UtilityAPI UINT               utilLoadResourceFile(HINSTANCE  hInstance, CONST TCHAR*  szResourceName, TCHAR*  &szOutput);
UtilityAPI UINT               utilLoadResourceFileA(HINSTANCE  hInstance, CONST TCHAR*  szResourceName, CONST BYTE*  &szOutputA);
UtilityAPI VOID               utilSetProcessWorkingFolder();
UtilityAPI VOID               utilSetThreadName(CHAR*  szThreadNameA);

/// Macros
#ifdef _DEBUG
#define  SET_THREAD_NAME(szNameA)   utilSetThreadName(szNameA)
#else
#define  SET_THREAD_NAME(szNameA)   
#endif

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        USER INTERFACE
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
UtilityAPI HFONT     utilCreateFont(HDC  hDC, CONST TCHAR*  szName, CONST UINT  iPointSize, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline);
UtilityAPI HFONT     utilCreateFontEx(HWND  hWnd, CONST TCHAR*  szName, CONST UINT  iPointSize, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline);
UtilityAPI HFONT     utilDuplicateFont(HDC  hDC, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline);
UtilityAPI HFONT     utilDuplicateFontEx(HDC  hDC, CONST TCHAR*  szFontName, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline);
UtilityAPI VOID      utilDeleteGDIObject(HGDIOBJ&  hObject);

// Functions
UtilityAPI LONG      utilAddWindowStyle(HWND  hWnd, DWORD  dwStyle);
UtilityAPI LONG      utilAddWindowStyleEx(HWND  hWnd, DWORD  dwStyleEx);
UtilityAPI HFONT     utilAdjustDlgItemFont(HWND  hDialog, CONST UINT  iControlID, CONST TCHAR*  szFontName, CONST UINT  iPointSize, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline);
UtilityAPI INT       utilCalculateFontHeight(HDC  hDC, CONST UINT  iPointSize);
UtilityAPI VOID      utilCentreRectangle(CONST RECT*  pParentRect, RECT*  pChildRect, CONST UINT  iFlags);
UtilityAPI VOID      utilCentreWindow(HWND  hParentWnd, HWND  hChildWnd);
UtilityAPI VOID      utilClientToScreenRect(HWND  hWnd, RECT*  pClientRect);
UtilityAPI VOID      utilConvertDialogUnitsToPixels(HWND  hDialog, CONST POINT*  ptDialogUnits, POINT*  ptPixels);
UtilityAPI VOID      utilDeferClientRect(HDWP  hWindowPos, HWND  hWnd, CONST RECT*  pRect, CONST BOOL  bMove, CONST BOOL  bResize);
UtilityAPI VOID      utilDeferDlgItemRect(HDWP  hWindowPos, HWND  hDialog, CONST UINT  iControlID, CONST RECT*  pRect, CONST BOOL  bMove, CONST BOOL  bResize);
UtilityAPI BOOL      utilEnableDragNDrop(HWND  hWnd);
UtilityAPI BOOL      utilEnableDlgItem(HWND  hDialog, CONST UINT  iControlID, CONST BOOL  bEnable);
UtilityAPI VOID      utilEnableDlgItems(HWND  hDialog, const BOOL  bEnable, const UINT  iCount, ...);
UtilityAPI TCHAR*    utilGetDlgItemPath(HWND  hDialog, CONST UINT  iControlID);
UtilityAPI VOID      utilGetDlgItemRect(HWND  hDialog, CONST UINT  iControlID, RECT*  pClientRect);
UtilityAPI TCHAR*    utilGetDlgItemText(HWND  hDialog, CONST UINT  iControlID);
UtilityAPI UINT      utilGetDlgItemTextLength(HWND  hDialog, CONST UINT  iControlID);
UtilityAPI HWND      utilGetTopWindow(HWND  hWnd);
UtilityAPI BOOL      utilGetWindowCursorPos(HWND  hWnd, POINT*  ptCursor);
UtilityAPI UINT      utilGetWindowHeight(HWND  hWnd);
UtilityAPI UINT      utilGetWindowInt(HWND  hWnd);
UtilityAPI VOID      utilGetWindowSize(HWND  hWnd, SIZE*  pSize);
UtilityAPI TCHAR*    utilGetWindowText(HWND  hWnd);
UtilityAPI UINT      utilGetWindowWidth(HWND  hWnd);
UtilityAPI BOOL      utilLaunchURL(HWND  hParentWnd, CONST TCHAR*  szURL, CONST INT  iShowCmd);
UtilityAPI BOOL      utilReflectMessage(HWND  hWnd, CONST UINT  iMessage, CONST WPARAM  wParam, CONST LPARAM  lParam);
UtilityAPI LONG      utilRemoveWindowStyle(HWND  hWnd, DWORD  dwStyle);
UtilityAPI LONG      utilRemoveWindowStyleEx(HWND  hWnd, DWORD  dwStyleEx);
UtilityAPI TCHAR*    utilReplaceWindowTextString(TCHAR*  &szString, HWND  hWnd);
UtilityAPI VOID      utilSafeDeleteWindow(HWND  &hWnd);
UtilityAPI VOID      utilScreenToClientRect(HWND  hWnd, RECT*  pWindowRect);
UtilityAPI VOID      utilSetDlgItemFocus(HWND  hDialog, CONST UINT  iControlID);
UtilityAPI VOID      utilSetDlgItemID(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNewID);
UtilityAPI VOID      utilSetDlgItemRect(HWND  hDialog, CONST UINT  iControlID, CONST RECT*  pRect, CONST BOOL  bRepaint);
//UtilityAPI VOID      utilSetDlgItemText(HWND  hDialog, CONST UINT  iControlID, HINSTANCE  hInstance, CONST UINT  iStringID);
UtilityAPI VOID      utilSetWindowInt(HWND  hWnd, CONST INT  iInteger);
UtilityAPI BOOL      utilSetWindowProgressState(HWND  hWnd, CONST UINT  eState);
UtilityAPI BOOL      utilSetWindowProgressValue(HWND  hWnd, CONST INT  iProgress, CONST INT  iMaximum);
UtilityAPI VOID      utilSetWindowRect(HWND  hWnd, CONST RECT*  pWindowRect, CONST BOOL  bRepaint);
UtilityAPI VOID      utilSetWindowTextf(HWND  hWnd, CONST TCHAR*  szFormat, ...);
UtilityAPI LRESULT   utilSendNotifyMessage(HWND  hWnd, CONST UINT  iMessage, HWND  hCtrl, CONST UINT  iControlID);
UtilityAPI VOID      utilShowDlgItem(HWND  hDialog, CONST UINT  iControlID, CONST BOOL  bShow);
UtilityAPI BOOL      utilTrackMouseEvent(HWND  hCtrl, DWORD  dwFlags, DWORD  dwTimeout);


// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          BUFFER MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: utilCopyBuffer
//  Description: Convenience macro for copying the contents from one buffer to another
///                     WARNING: You must ensure destination buffer is large enough
//
//  TCHAR*        szDestination  : [in] Destination buffer
//  CONST TCHAR*  szSource       : [in] Source buffer
//  CONST UINT    iSourceLength  : [in] Object's type
//
#define      utilCopyBuffer(szDestination,szSource,iSourceLength)             utilCopyMemory((VOID*)(szDestination), (VOID*)(szSource), iSourceLength)

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          OBJECT MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: utilCreateObject
//  Description: Convenience macro for allocating space for a new object
//
//  N/A   xType      : [in] Object's type
//
#define      utilCreateObject(xType)                                          (xType*)utilAllocateMemory(sizeof(xType))

/// Name: utilCreateObjectArray
//  Description: Convenience macro for allocating space for an array of new objects
//
//  N/A   xType      : [in] Objects' type
//  UINT  nElements  : [in] Number of objects in the array
//
#define      utilCreateObjectArray(xType,nElements)                           (xType*)utilAllocateMemory(sizeof(xType) * (nElements))


/// Name: utilCreateEmptyObject
//  Description: Convenience macro for allocating space for a new object and zero'ing it
//
//  N/A   xType      : [in] Object's type
//
#define      utilCreateEmptyObject(xType)                                     (xType*)utilAllocateEmptyMemory(sizeof(xType))


/// Name: utilCreateEmptyObjectArray
//  Description: Convenience macro for allocating space for an array of new objects and zero'ing them
//
//  N/A   xType      : [in] Objects' type
//  UINT  nElements  : [in] Number of objects in the array
//
#define      utilCreateEmptyObjectArray(xType,nElements)                      (xType*)utilAllocateEmptyMemory(sizeof(xType) * (nElements))


/// Name: utilDeleteObject
//  Description: Convenience macro for deleting an object without having to cast it to VOID*
//
//  <Various>  pObject : [in] Object to destroy
//
#define      utilDeleteObject(pObject)                                        utilFreeMemory((VOID*&)pObject)


/// Name: utilCopyObject
//  Description: Convenience macro for copying an object from one buffer to another
//
//  TCHAR*  pszDestination  : [in] Destination buffer
//  TCHAR*  pszSource       : [in] Source buffer
//  N/A    xType            : [in] Object's type
//
#define      utilCopyObject(pDestination,pSource,xType)                       utilCopyMemory((VOID*)(pDestination), (VOID*)(pSource), sizeof(xType))

/// Name: utilCopyObjects
//  Description: Convenience macro for copying an array of elements of any type
//
//  TCHAR*  pszDestination  : [in] Destination buffer
//  TCHAR*  pszSource       : [in] Source buffer
//  N/A    xType            : [in] Objects' type
//  UINT    nElements       : [in] Number of objects to copy
//
#define      utilCopyObjects(pDestination,pSource,xType,nElements)            utilCopyMemory((VOID*)(pDestination), (VOID*)(pSource), sizeof(xType) * (nElements))


/// Name: utilZeroObject
//  Description: Convenience macro for zero'ing an object
//
//  VOID*  pObject : [in] Object to zero
//  N/A    xType   : [in] Object's type
//
#define      utilZeroObject(pObject,xType)                                    utilZeroMemory((pObject), sizeof(xType))


/// Name: utilZeroObjectArray
//  Description: Convenience macro for zero'ing an array of objects
//
//  VOID*  pObject   : [in] Object to zero
//  N/A    xType     : [in] Object's type
//  UINT   nElements : [in] Number of objects to zero
//
#define      utilZeroObjectArray(pObject,xType,nElements)                     utilZeroMemory((pObject), sizeof(xType) * (nElements))

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          MEMORY MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: utilCompareMemory
//  Description: Compare two blocks of memory
//
//  VOID*       pFirst  : [in] First block of memory
//  VOID*       pSecond : [in] Second block of memory
//  CONST UINT  iBytes  : [in] Number of bytes to compare
//
#define      utilCompareMemory(pFirst, pSecond, iBytes)                       (memcmp((VOID*)(pFirst), (VOID*)(pSecond), (iBytes)) == 0)

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          PATH MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: utilCreateEmptyPath
//  Description: Create an empty string of MAX_PATH characters
//
// Return Value: New string of MAX_PATH characters, you are responsible for destroying it
//
#define      utilCreateEmptyPath()                                            utilCreateEmptyString(MAX_PATH)

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          STRING MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: utilAppendString
//  Description: Copy a string to the end of another, directly, without the overhead of the concatenation functions
//
//  TCHAR*      szDestination      : [in] Existing string to be appended
//  CONST UINT  nDestinationUsed   : [in] Length of existing string above, in characters
//  CONST UINT  nDestinationLength : [in] Length of the string buffer for string above, in characters
//  TCHAR*      szNew              : [in] New string to append to existing string
//
#define      utilAppendString(szDestination,nDestinationUsed,nDestinationLength,szNew)         StringCchCopy(&(szDestination)[nDestinationUsed], nDestinationLength, szNew)

/// Name: utilCopyString
//  Description: Convenience macro for memcpy'ing a TCHAR string
//
//  TCHAR*  szDestination   : [in] Destination string
//  TCHAR*  szSource        : [in] Source string   
//  UINT    nChars          : [in] Number of characters to copy
//
#define      utilCopyString(szDestination,szSource,nChars)                    utilCopyMemory((VOID*)(szDestination), (VOID*)(szSource), sizeof(TCHAR) * (nChars))

/// Name: utilZeroString
//  Description: Convenience macro for zero'ing a TCHAR string
//
//  TCHAR*  szString : [in] String to zero
//  UINT    nChars   : [in] Length of string, in characters
//
#define      utilZeroString(szString,nChars)                                  utilZeroMemory((VOID*)(szString), sizeof(TCHAR) * (nChars))

/// Macro: utilCompareString
//  Description: Convenience macro for comparing strings
//
//  TCHAR*       szVariable : [in] String variable to compare
//  CONST CHAR*  szString   : [in] ANSI string literal to compare against
//
#define      utilCompareString(szVariable,szString)                           (StrCmpI(szVariable, TEXT(szString)) == 0)

/// Macro: utilCompareCharacter
//  Description: Convenience macro for case insensitive character comparison
//
//  TCHAR        chCharacter1 : [in] Character to compare
//  TCHAR        chCharacter2 : [in] Character to compare against
//
#define      utilCompareCharacter(chCharacter1,chCharacter2)                  (ChrCmpI(chCharacter1, chCharacter2) == 0)

/// Macro: utilCompareStringN
//  Description: Convenience macro for comparing 'n' characters of a string
//
//  TCHAR*       szVariable : [in] String variable to compare
//  CONST CHAR*  szString   : [in] ANSI string literal to compare against
//  UINT         nChars     : [in] Number of characters to compare
//
#define      utilCompareStringN(szVariable,szString,nChars)                   (StrCmpNI(szVariable, TEXT(szString), nChars) == 0)

/// Macro: utilCompareStringVariables
//  Description: Convenience macro for comparing string variables
//
//  CONST TCHAR* szFirstVariable  : [in] String variable to compare
//  CONST TCHAR* szSecondVariable : [in] String variable to compare against
//
#define      utilCompareStringVariables(szFirstVariable,szSecondVariable)     (StrCmpI(szFirstVariable, szSecondVariable) == 0)

/// Macro: utilCompareStringVariablesA
//  Description: Convenience macro for comparing ANSI string variables
//
//  CONST CHAR* szFirstVariableA  : [in] ANSI String variable to compare
//  CONST CHAR* szSecondVariableA : [in] ANSI String variable to compare against
//
#define      utilCompareStringVariablesA(szFirstVariableA,szSecondVariableA)   (StrCmpIA(szFirstVariableA, szSecondVariableA) == 0)

/// Macro: utilCompareStringVariablesN
//  Description: Convenience macro for comparing 'n' characters in two string variables
//
//  CONST TCHAR* szFirstVariable  : [in] String variable to compare
//  CONST TCHAR* szSecondVariable : [in] String variable to compare against
//  CONST UINT   nChars           : [in] Number of characters to compare
//
#define      utilCompareStringVariablesN(szFirstVariable,szSecondVariable, nChars)    (StrCmpNI(szFirstVariable, szSecondVariable, nChars) == 0)

/// Macro: utilCompareStringVariablesN
//  Description: Convenience macro for comparing 'n' characters in two string variables in a case sensitive manner
//
//  CONST TCHAR* szFirstVariable  : [in] String variable to compare
//  CONST TCHAR* szSecondVariable : [in] String variable to compare against
//  CONST UINT   nChars           : [in] Number of characters to compare
//
#define      utilCompareStringVariablesNS(szFirstVariable,szSecondVariable, nChars)    (StrCmpN(szFirstVariable, szSecondVariable, nChars) == 0)

/// Macro: utilCompareIntegers
//  Description: Convenience macro for comparing integers and returning the same -1,0,+1 result as StrCmp
//
//  CONST INT    iFirstInteger  : [in] Integer to compare
//  CONST INT    iSecondInteger : [in] Integer to compare against
//
#define      utilCompareIntegers(iFirstInteger,iSecondInteger)                (iFirstInteger < iSecondInteger ? -1 : iFirstInteger > iSecondInteger)

/// Macro: utilConvertIntegerToString
//  Description: Convenience macro for converting an integer to a string
//
//  CONST TCHAR* szString : [in] String buffer
//  CONST INT    iInteger : [in] Integer to convert
//
#define      utilConvertIntegerToString(szString,iInteger)                    _itot(iInteger, szString, 10)

/// Macro: utilConvertIntegerToStringA
//  Description: Convenience macro for converting an integer to an ANSI string
//
//  CONST CHAR*  szStringA : [in] ANSI String buffer
//  CONST INT    iInteger  : [in] Integer to convert
//
#define      utilConvertIntegerToStringA(szStringA,iInteger)                  itoa(iInteger, szStringA, 10)

/// Macro: utilConvertIntegerToString
//  Description: Convenience macro for converting a string to an integer
//
//  CONST TCHAR* szString : [in] String to convert
//
#define      utilConvertStringToInteger(szString)                             _ttoi(szString)

/// Macro: utilSafeConvertStringToInteger
//  Description: Convenience macro for converting a string to an integer
//
//  CONST TCHAR* szString : [in] String to convert
//
#define      utilSafeConvertStringToInteger(szString)                         (szString ? _ttoi(szString) : NULL)

/// Macro: utilConvertIntegerToStringA
//  Description: Convenience macro for converting an ANSI string to an integer
//
//  CONST CHAR* szStringA : [in] ANSI String to convert
//
#define      utilConvertStringToIntegerA(szStringA)                           atoi(szStringA)

/// Macro: utilDuplicateSimpleString
//  Description: Convenience macro for duplicating a string
//
//  CONST TCHAR* szString : [in] String to duplicate
//
#define      utilDuplicateSimpleString(szString)                              utilDuplicateString(szString, lstrlen(szString))

/// Macro: utilSafeDuplicateSimpleString
//  Description: Convenience macro for duplicating a string
//
//  CONST TCHAR* szString : [in] String to duplicate, if any
//
//  Return Value : Duplicate of input string, if any, otherwise NULL
//
#define      utilSafeDuplicateSimpleString(szString)                          (szString ? utilDuplicateString(szString, lstrlen(szString)) : NULL)

/// Macro: utilFindCharacter
//  Description: Convenience macro for finding characters
//
//  TCHAR*  szString    : [in] String variable to search
//  TCHAR   chCharacter : [in] Character to search for
//
#define      utilFindCharacter(szString,chCharacter)                          StrChr(szString, chCharacter)

/// Macro: utilFindCharacterA
//  Description: Convenience macro for finding characters in ANSI strings
//
//  CHAR*   szStringA    : [in] String variable to search
//  CHAR    chCharacterA : [in] Character to search for
//
#define      utilFindCharacterA(szStringA,chCharacterA)                       StrChrA(szStringA, chCharacterA)

/// Macro: utilFindCharacterInSet
//  Description: Convenience macro for finding a sub-set of characters
//
//  TCHAR*  szString     : [in] String variable to search
//  TCHAR*  szCharacters : [in] Characters to search for
//
#define      utilFindCharacterInSet(szString,szCharacters)                    StrPBrk(szString, TEXT(szCharacters))

/// Macro: utilFindCharacterNotInSet
//  Description: Convenience macro for finding a character not in a sub-set of characters
//
//  TCHAR*  szString     : [in] String variable to search
//  TCHAR*  szCharacters : [in] Characters to search for
//
#define      utilFindCharacterNotInSet(szString,szCharacters)                 _tcsspnp(szString, TEXT(szCharacters))

/// Macro: utilFindCharacterReverse
//  Description: Convenience macro for finding characters from the end of a string
//
//  TCHAR*  szString    : [in] String variable to search
//  TCHAR   chCharacter : [in] Character to search for
//
#define      utilFindCharacterReverse(szString,chCharacter)                   StrRChr(szString, NULL, chCharacter)

/// Macro: utilFindCharacterReverseA
//  Description: Convenience macro for finding characters from the end of an ANSI string
//
//  CHAR*   szStringA    : [in] String variable to search
//  CHAR    chCharacterA : [in] Character to search for
//
#define      utilFindCharacterReverseA(szStringA,chCharacterA)                StrRChrA(szStringA, NULL, chCharacterA)

/// Macro: utilFindSubString
//  Description: Convenience macro for finding substrings
//
//  TCHAR*       szStringW    : [in] String variable to search
//  CONST CHAR*  szSubStringA : [in] ANSI string literal to search for
//
#define      utilFindSubString(szStringW,szSubStringA)                        StrStr(szStringW, TEXT(szSubStringA))

/// Macro: utilFindSubStringI
//  Description: Convenience macro for finding substrings case insensitively
//
//  TCHAR*       szStringW    : [in] String variable to search
//  CONST CHAR*  szSubStringA : [in] ANSI string literal to search for
//
#define      utilFindSubStringI(szStringW,szSubStringA)                       StrStrI(szStringW, TEXT(szSubStringA))

/// Macro: utilFindSubStringVariable
//  Description: Convenience macro for finding substrings within variables
//
//  TCHAR*       szStringW    : [in] String variable to search
//  CONST CHAR*  szSubString  : [in] Search term
//
#define      utilFindSubStringVariable(szString,szSubString)                  StrStr(szString, szSubString)

/// Macro: utilFindSubStringVariableI
//  Description: Convenience macro for finding substrings  within variables case insensitively
//
//  TCHAR*       szStringW    : [in] String variable to search
//  CONST CHAR*  szSubString  : [in] Search term
//
#define      utilFindSubStringVariableI(szString,szSubString)                 StrStrI(szString, szSubString)

/// Macro: utilFindStringRightN
//  Description: Retrieves a pointer to the sub-string starting 'n' characters from the end
//                -> If 'n' exceeds the length of the string, the beginning of the string is returned
//
//  CONST TCHAR* szString : [in] String literal or variable to search
//  CONST INT    nChars   : [in] Number of characters from the end
//
#define      utilFindStringRightN(szString, nChars)                           (&szString[(nChars > lstrlen(szString) ? 0 : lstrlen(szString) - nChars)])

/// Macro: utilTokeniseString
//  Description: Convenience macro for tokenising a UNICODE string buffer
//
//  CONST TCHAR*   szString      : [in] String buffer to tokenise
//  CONST CHAR*    szDelimiters  : [in] ANSI string specifying delimiters
//  CONST TCHAR**  pIterator     : [in] Tokeniser data
//
#define      utilTokeniseString(szString,szDelimiters,pIterator)              _tcstok_s(szString,TEXT(szDelimiters),pIterator)

/// Macro: utilGetNextToken
//  Description: Convenience macro for retrieving the next token while tokenising a UNICODE string buffer
//
//  CONST CHAR*    szDelimiters  : [in] ANSI string specifying delimiters
//  CONST TCHAR**  pIterator     : [in] Tokeniser data
//
#define      utilGetNextToken(szDelimiters,pIterator)                         utilTokeniseString(NULL, szDelimiters, pIterator)

/// Macro: utilTokeniseStringA
//  Description: Convenience macro for tokenising an ANSI string buffer
//
//  CONST CHAR*    szString      : [in] ANSI String buffer to tokenise
//  CONST CHAR*    szDelimiters  : [in] ANSI string specifying delimiters
//  CONST TCHAR**  pIterator     : [in] Tokeniser data
//
#define      utilTokeniseStringA(szString,szDelimiters,pIterator)             strtok_s((CHAR*)szString, szDelimiters, (CHAR**)pIterator)

/// Macro: utilGetNextTokenA
//  Description: Convenience macro for retrieving the next token while tokenising an ANSI string buffer
//
//  CONST CHAR*    szDelimiters  : [in] ANSI string specifying delimiters
//  CONST TCHAR**  pIterator     : [in] Tokeniser data
//
#define      utilGetNextTokenA(szDelimiters,pIterator)                        utilTokeniseStringA(NULL, szDelimiters, pIterator)

/// Macro: utilReverseString
//  Description: Convenience macro for reversing a UNICODE string buffer
//
//  CONST TCHAR*   szString      : [in] String buffer to tokenise
//
#define      utilReverseString(szString)                                      _tcsrev(szString)

/// Macro: utilScanf
//  Description: Convenience macro for reading formatted data from a string
//
//  CONST TCHAR*   szString : [in]     String to parse
//  CONST TCHAR*   szFormat : [in]     Formatting string
//  ...            ...      : [in/out] Parameters to receive data
//
#define      utilScanf                                                        _stscanf_s

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          OLE MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: utilReleaseInterface
// Description: Convenience macro for releasing OLE interfaces
//
// IInterface*  pInterface : [in] COM Interface to release
//
#define      utilReleaseInterface(pInterface)                                 if (pInterface) { pInterface->Release();  pInterface = NULL; }

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        DRAWING MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: utilFillStockRect
//  Description: Convenience macro for filling a rectangle using a stock object
//
// HDC         hDC     : [in] Device context
// RECT*       pRect   : [in] Drawing rectangle
// CONST UINT  iObject : [in] Stock object ID
//
#define  utilFillStockRect(hDC, pRect, iObject)                               FillRect(hDC, pRect, (HBRUSH)GetStockObject(iObject))


/// Name: utilFillSysColourRect
//  Description: Convenience macro for filling a rectangle using a system colour
//
// HDC         hDC     : [in] Device context
// RECT*       pRect   : [in] Drawing rectangle
// CONST UINT  iColour : [in] System colour ID
//
#define  utilFillSysColourRect(hDC, pRect, iColour)                           FillRect(hDC, pRect, (HBRUSH)GetSysColorBrush(iColour))

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                        GENERAL MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: utilEither
// Description: Return first pointer if non-zero, otherwise the second
//
// VOID*  pFirst  : [in] First pointer
// VOID*  pSecond : [in] Second pointer
//
// Return Value : First pointer if any, otherwise second
//
#define      utilEither(pFirst,pSecond)                                       ((pFirst) ? (pFirst) : (pSecond))

/// Macro: utilEnforceRange
// Description: Ensures a value is between a specified range
//
// INT    iValue : [in] Value to check
// INT    iMin   : [in] Minimum value
// INT    iMax   : [in] Maximum value
//
// Return Value : iMin <= Return Value <= iMax
//
#define      utilEnforceRange(iValue, iMin, iMax)                             ((INT)(iValue) < (INT)(iMin) ? (iMin) : ((INT)(iValue) > (INT)(iMax) ? (iMax) : (iValue)));

/// Macro: utilExcludes
// Description: Determines whether a value excludes the specified flags
//
// UINT   iValue : [in] Value to test
// UINT   iFlags : [in] Flags to test the presence of
//
// Return Value : TRUE if flags are not present, FALSE otherwise
//
#define      utilExcludes(iValue, iFlags)                                     (((iValue) INCLUDES (iFlags)) == FALSE)

/// Macro: utilIncludes
// Description: Determines whether a value includes the specified flags
//
// UINT   iValue : [in] Value to test
// UINT   iFlags : [in] Flags to test the presence of
//
// Return Value : TRUE if flags are not present, FALSE otherwise
//
#define      utilIncludes(iValue, iFlags)                                     (((iValue) INCLUDES (iFlags)))

/// Macro: utilLarger
// Description: Return first if larger, otherwise second
//
// ...    xFirst  : [in] First
// ...    xSecond : [in] Second 
//
// Return Value : First if smaller, otherwise second
//
#define      utilLarger(xFirst,xSecond)                                       ((xFirst) > (xSecond) ? (xFirst) : (xSecond))

/// Macro: utilLargerOrEqual
// Description: Return first if larger or equal, otherwise second
//
// ...    xFirst  : [in] First
// ...    xSecond : [in] Second 
//
// Return Value : First if smaller or equal, otherwise second
//
#define      utilLargerOrEqual(xFirst,xSecond)                                ((xFirst) >= (xSecond) ? (xFirst) : (xSecond))

/// Macro: utilSmaller
// Description: Return first if smaller, otherwise second
//
// ...    xFirst  : [in] First
// ...    xSecond : [in] Second 
//
// Return Value : First if smaller, otherwise second
//
#define      utilSmaller(xFirst,xSecond)                                      ((xFirst) < (xSecond) ? (xFirst) : (xSecond))

/// Macro: utilSmallerOrEqual
// Description: Return first if smaller or equal, otherwise second
//
// ...    xFirst  : [in] First
// ...    xSecond : [in] Second 
//
// Return Value : First if smaller or equal, otherwise second
//
#define      utilSmallerOrEqual(xFirst,xSecond)                               ((xFirst) <= (xSecond) ? (xFirst) : (xSecond))

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          MISC MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Name: utilCalculateMagnitude
//  Description: Convenience macro for calculating the magnitude of an integer
//
// UINT   nNumber     : [in] Value to calculate the magnitude of
//
#define      utilCalculateMagnitude(nNumber)                                  (((INT)nNumber) < 0 ? -((INT)nNumber) : ((INT)nNumber))

/// Name: utilCalculatePercentage
//  Description: Convenience macro for calculating the percentage of a number
//
// UINT   nNumber     : [in] Value to calculate a percentage of
// UINT   nPercentage : [in] Desired percentage
//
#define      utilCalculatePercentage(nNumber,nPercentage)                     ((nNumber) * (nPercentage) / 100)

/// Name: utilConvertCharToInteger
//  Description: Convenience macro for converting a string digit into an integer
//
// CONST TCHAR chChar : [in] Character to convert
//
#define      utilConvertCharToInteger(chChar)                                 (UINT)(chChar - 48)

/// Macro: utilConvertRectangleToSize
// Description: Calculate the size of a RECT structure and output result into a SIZE structure
//
// CONST RECT*  pRect : [in] Source rectangle
// SIZE*        pSize : [out] Destination size structure
//
#define      utilConvertRectangleToSize(pRect,pSize)                          { (pSize)->cx = (pRect)->right - (pRect)->left;  (pSize)->cy = (pRect)->bottom - (pRect)->top; }

/// Macro: utilConvertRectangleToPoint
// Description: Extract the top left corner of a RECT structure as a POINT
//
// CONST RECT*  pRect  : [in] Source rectangle
// POINT*       pPoint : [out] Destination point structure
//
#define      utilConvertRectangleToPoint(pRect,pPoint)                        { (pPoint)->x = (pRect)->left;  (pPoint)->y = (pRect)->top; }

/// Name: utilGetFirstVariableArgument
//  Description: Retrieve a pointer to the first variable argument in a function. Convenience replacement for va_start.
//
//  ...   pLastArgument : [in] Pointer to the last argument in the argument list
//
#define      utilGetFirstVariableArgument(pLastArgument)                      ((va_list)(pLastArgument) + sizeof(pLastArgument))

/// Name: utilGetNextVariableArgument
//  Description: Retrieve the next variable argument in a function. Convenience replacement for va_arg.
//
//  ...   pIterator : [in] The current variable argument
//  ...   xType     : [in] Data type of the current variable argument
//
#define      utilGetNextVariableArgument(pIterator, xType)                    ((va_list)(pIterator) + sizeof(xType))

/// Name: utilGetCurrentVariableArgument
//  Description: Retrieve the current variable argument value. Convenience replacement for va_arg.
//
//  ...   pIterator : [in] The current variable argument
//  ...   xType     : [in] Data type of the current variable argument
//
#define      utilGetCurrentVariableArgument(pIterator, xType)                 (*(xType*)(pIterator))

/// Name: utilLaunchThread
//  Description: Launches a new worker thread manually
//
//  LPTHREAD_START_ROUTINE  pfnThreadFunction : [in]     Thread function
//  VOID*                   pThreadData       : [in]     Thread data
//  DWORD*                  pThreadID         : [in/out] Thread ID, optional
//
#define      utilLaunchThread(pfnThreadFunction, pThreadData, bSuspended, pThreadID)   CreateThread(NULL, NULL, pfnThreadFunction, (VOID*)pThreadData, bSuspended ? CREATE_SUSPENDED : NULL, pThreadID)

/// Name: utilLaunchThreadpoolThread
//  Description: Launches a new worker thread using a threadpool
//
//  LPTHREAD_START_ROUTINE  pfnThreadFunction : [in] Thread function
//  VOID*                   pThreadData       : [in] Thread data
//
#define      utilLaunchThreadpoolThread(pfnThreadFunction, pThreadData)       QueueUserWorkItem(pfnThreadFunction, (VOID*)pThreadData, MAKE_LONG(WT_EXECUTEDEFAULT WITH WT_TRANSFER_IMPERSONATION, 8))

/// Macro: utilRedrawWindow
// Description: Invalidates and redraws a window
//
// HWND  hWnd : [in] Window handle
//
#define      utilRedrawWindow(hWnd)                                           RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE WITH RDW_UPDATENOW);

/// Macro: utilSetRectangle
// Description: Fill a RECT structure from a co-ordinate plus a width and height
//
// CONST RECT*  pRect   : [out] Destination rectangle
// CONST INT    iLeft   : [in]  Left co-ordinate
// CONST INT    iTop    : [in]  Top co-ordinate
// CONST INT    iWidth  : [in]  Width
// CONST INT    iHeight : [in]  Height
//
#define      utilSetRectangle(pRect,iLeft,iTop,iWidth,iHeight)                SetRect(pRect, iLeft, iTop, (iLeft) + (iWidth), (iTop) + (iHeight));

/// Macro: utilSetSize
// Description: Fill a SIZE structure
//
// CONST SIZE*  pSize   : [out] Destination size
// CONST INT    iWidth  : [in]  Width
// CONST INT    iHeight : [in]  Height
//
#define      utilSetSize(pSize,iWidth,iHeight)                                { (pSize)->cx = iWidth;  (pSize)->cy = iHeight; }

/// Macro: utilSetPoint
// Description: Fill a POINT structure
//
// CONST POINT*  pPoint : [out] Destination point
// CONST INT     iX     : [in]  X co-ordinate
// CONST INT     iY     : [in]  Y co-ordinate
//
#define      utilSetPoint(pPoint,iX,iY)                                       { (pPoint)->x = iX;  (pPoint)->y = iY; }

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                          REGISTRY MACROS
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: utilRegistryReadObject
// Description: Loads an object from the registry
//
// HKEY          hKey    : [in] Open Registry key
// CONST TCHAR*  szValue : [in] Name of the value
// VOID*         pObject : [in] Object to store data in
//  ...          xType   : [in] Type of object
//
// Return value : TRUE if successful, otherwise FALSE
//
#define  utilRegistryReadObject(hKey, szValue, pObject, xType)                utilRegistryReadBinary(hKey, szValue, (BYTE*)(pObject), sizeof(xType))

/// Macro: utilRegistryReadObjectArray
// Description: Loads an array of objects from the registry
//
// HKEY          hKey    : [in] Open Registry key
// CONST TCHAR*  szValue : [in] Name of the value
// CONST VOID*   pObject : [in] Object to load
//  ...          xType   : [in] Type of object
// CONST UINT    iCount  : [in] Number of objects
//
// Return value : TRUE if successful, otherwise FALSE
//
#define  utilRegistryReadObjectArray(hKey, szValue, pObject, xType, iCount)   utilRegistryReadBinary(hKey, szValue, (BYTE*)(pObject), (iCount) * sizeof(xType))

/// Macro: utilRegistryReadNumber
// Description: Loads an integer from the registry
//
// HKEY          hKey    : [in] Open Registry key
// CONST TCHAR*  szValue : [in] Name of the value
// UINT*         pNumber : [in] Number to store value in
//
// Return value : TRUE if successful, otherwise FALSE
//
#define  utilRegistryReadNumber(hKey, szValue, pNumber)                       utilRegistryReadInteger(hKey, szValue, (UINT*)(pNumber))

/// Macro: utilRegistryWriteObject
// Description: Saves an object to the registry
//
// HKEY          hKey    : [in] Open Registry key
// CONST TCHAR*  szValue : [in] Name of the value
// CONST VOID*   pObject : [in] Object to save
//  ...          xType   : [in] Type of object
//
// Return value : TRUE if successful, otherwise FALSE
//
#define  utilRegistryWriteObject(hKey, szValue, pObject, xType)               utilRegistryWriteBinary(hKey, szValue, (BYTE*)(pObject), sizeof(xType))

/// Macro: utilRegistryWriteObjectArray
// Description: Saves an array of objects to the registry
//
// HKEY          hKey    : [in] Open Registry key
// CONST TCHAR*  szValue : [in] Name of the value
// CONST VOID*   pObject : [in] Object to save
//  ...          xType   : [in] Type of object
// CONST UINT    iCount  : [in] Number of objects
//
// Return value : TRUE if successful, otherwise FALSE
//
#define  utilRegistryWriteObjectArray(hKey, szValue, pObject, xType, iCount)  utilRegistryWriteBinary(hKey, szValue, (BYTE*)(pObject), (iCount) * sizeof(xType))

/// Macro: utilRegistryWriteNumber
// Description: Saves an integer to the registry
//
// HKEY          hKey    : [in] Open Registry key
// CONST TCHAR*  szValue : [in] Name of the value
// CONST UINT*   pNumber : [in] Number to store
//
// Return value : TRUE if successful, otherwise FALSE
//
#define  utilRegistryWriteNumber(hKey, szValue, pNumber)                      utilRegistryWriteInteger(hKey, szValue, (UINT)(pNumber))

