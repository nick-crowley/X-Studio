//
// Debugging.cpp : Debugging helpers
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "Dbghelp.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

typedef  SYMBOL_INFO_PACKAGEW  SymbolInfo;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

BearScriptAPI
VOID  debugAccessViolation()
{
   INT*  pNull = NULL;
   *pNull = 42;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

BearScriptAPI
VOID  debugArgument(const ARGUMENT*  pArgument)
{
   CONSOLE("Argument '%s' : iID=%d  szDesc='%s'  eType='%s'", pArgument->szName, pArgument->iID, pArgument->szDescription, identifyParameterSyntaxString(pArgument->eType));
}


BearScriptAPI
VOID  debugCommand(const COMMAND*  pCommand)
{
   if (!pCommand) 
      CONSOLE("Command is null");
   else
   {
      CONSOLE("Command '%s' iID=%d  iFlags=%d  eConditional=%d  pErrorQueue=%d", pCommand->szBuffer, pCommand->iID, pCommand->iFlags, pCommand->eConditional, pCommand->pErrorQueue);

      // ErrorQueue
      debugCommandSyntax(pCommand->pSyntax);
      debugErrorQueue(pCommand->pErrorQueue);
   }
}


BearScriptAPI
VOID  debugCommandSyntax(const COMMAND_SYNTAX*  pSyntax)
{
   if (!pSyntax)
      CONSOLE("Syntax is null");
   else
      CONSOLE("Syntax '%s' iID=%d", pSyntax->szSyntax, pSyntax->iID);
}


BearScriptAPI
VOID  debugList(const LIST*  pList)
{
   if (!pList)
      CONSOLE("List is null");
   else
      CONSOLE("List: iCount=%d", pList->iCount);
}

BearScriptAPI
VOID  debugErrorQueue(const ERROR_QUEUE*  pQueue)
{
   if (!pQueue)
      CONSOLE("ErrorQueue is null");
   else
      CONSOLE("ErrorQueue eType='%s': iCount=%d", identifyErrorTypeString(identifyErrorQueueType(pQueue)), pQueue->iCount);
}


BearScriptAPI
VOID  debugGameFile(const GAME_FILE*  pGameFile)
{
   if (!pGameFile)
      CONSOLE("GameFile is null");
   else
      CONSOLE("GameFile '%s' bResourceBased=%d  iInputSize=%d  iOutputSize=%d", pGameFile->szFullPath, pGameFile->bResourceBased, pGameFile->iInputSize, pGameFile->iOutputSize);
}

BearScriptAPI
VOID  debugGameString(const GAME_STRING*  pGameString)
{
   if (!pGameString)
      CONSOLE("GameString is null");
   else
      CONSOLE("GameString PageID=%d  ID=%d  eType=%d  eVersion=%d  iCount=%d: '%s'", pGameString->iPageID, pGameString->iID, pGameString->eType, pGameString->eVersion, pGameString->iCount, pGameString->szText);
}


BearScriptAPI
VOID  debugJumpStack(const STACK*  pStack)
{
   JUMP_STACK_ITEM*  pItem = NULL;

   if (!pStack)
      CONSOLE("JumpStack is null");
   else
   {
      CONSOLE("Printing %d JumpStack items:", getListItemCount(pStack));
      for (UINT  i = 0; findListObjectByIndex(pStack, 0, (LPARAM&)pItem); i++)
         debugJumpStackItem(pItem);
   }
}

BearScriptAPI
VOID  debugJumpStackItem(const JUMP_STACK_ITEM*  pItem)
{
   if (!pItem)
      CONSOLE("JumpStackItem is null");
   else
   {
      CONSOLE("JumpStackItem eConditional=%d", pItem->eConditional);
      debugCommand(pItem->pCommand);
      debugCommand(pItem->pJumpCommand);
   }
}

BearScriptAPI
VOID  debugOperationData(const OPERATION_DATA*  pOperation)
{
   if (!pOperation)
      CONSOLE("Operation is null");
   else
   {
      const TCHAR*  szStatus = (pOperation->pProgress AND !pOperation->pProgress->bComplete ? TEXT("Running") : identifyOperationResultString(pOperation->eResult));
      CONSOLE("Operation '%s' eStatus=%s  hThread=0x%04x", identifyOperationTypeString(pOperation->eType), szStatus, pOperation->dwThreadID);
   }
}

BearScriptAPI
VOID  debugObjectName(const OBJECT_NAME*  pObjectName)
{
   if (!pObjectName)
      CONSOLE("ObjectName is null");
   else
      CONSOLE("ObjectName eType=%d  eGroup=%d  iID=%d  szText='%s'  szObjectID='%s'", pObjectName->eType, pObjectName->eGroup, pObjectName->iID, pObjectName->szText, pObjectName->szObjectID);
}

BearScriptAPI
VOID  debugProjectFile(const PROJECT_FILE*  pProjectFile)
{
   PROJECT_VARIABLE*  pVariable;
   STORED_DOCUMENT*   pDocument;

   if (!pProjectFile)
      CONSOLE("ProjectFile is null");
   else
      CONSOLE("ProjectFile '%s' : iVarCount=%d  iScriptCount=%d  iLanguageCount=%d", pProjectFile->szFullPath, 
               getTreeNodeCount(pProjectFile->pVariablesTree), getListItemCount(pProjectFile->pFileList[PF_SCRIPT]), getListItemCount(pProjectFile->pFileList[PF_LANGUAGE]));

   // Documents
   for (PROJECT_FOLDER  eFolder = PF_SCRIPT; eFolder <= PF_MISSION; eFolder = (PROJECT_FOLDER)(eFolder + 1))
      for (UINT  iIndex = 0; findDocumentInProjectFileByIndex(pProjectFile, eFolder, iIndex, pDocument); iIndex++)
         debugStoredDocument(pDocument);

   // Variables
   for (UINT  iIndex = 0; findVariableInProjectFileByIndex(pProjectFile, iIndex, pVariable); iIndex++)
      debugProjectVariable(pVariable);
}


BearScriptAPI
VOID  debugProjectVariable(const PROJECT_VARIABLE*  pVariable)
{
   if (!pVariable)
      CONSOLE("ProjectVariable is null");
   else
      CONSOLE("ProjectVariable '%s': iValue=%d", pVariable->szName, pVariable->iValue);
}


BearScriptAPI
VOID  debugQueue(const QUEUE*  pQueue)
{
   if (!pQueue)
      CONSOLE("Queue is null");
   else
      CONSOLE("Queue: iCount=%d", pQueue->iCount);
}


BearScriptAPI
VOID  debugScriptDependency(const SCRIPT_DEPENDENCY*  pDependency)
{
   if (!pDependency)
      CONSOLE("Dependency is null");
   else
      CONSOLE("Dependency '%s': iCalls=%d  iLineNumber=%d", pDependency->szScriptName, pDependency->iCalls, pDependency->iLine);
}



BearScriptAPI
VOID  debugScriptDependencyTree(const AVL_TREE*  pTree)
{
   SCRIPT_DEPENDENCY*  pDependency;

   if (!pTree)
      CONSOLE("DependencyTree is null");
   else
   {
      CONSOLE("DependencyTree contains %d items:", getTreeNodeCount(pTree));

      for (UINT  iIndex = 0; findObjectInAVLTreeByIndex(pTree, iIndex, (LPARAM&)pDependency); iIndex++)
         debugScriptDependency(pDependency);
   }
}


BearScriptAPI
VOID  debugScriptFile(const SCRIPT_FILE*  pScriptFile)
{
   ARGUMENT*  pArgument;

   if (!pScriptFile)
      CONSOLE("ScriptFile is null");
   else
      CONSOLE("ScriptFile '%s' : Description='%s' Command='%s' Version=%d  GameVersion=%d  Sig=%d  LiveData=%d  ModifiedLoad=%d", pScriptFile->szScriptname,
            pScriptFile->szDescription, pScriptFile->szCommandID, pScriptFile->iVersion, pScriptFile->eGameVersion, pScriptFile->bSignature, pScriptFile->bLiveData, pScriptFile->bModifiedOnLoad);

   // Arguments
   if (pScriptFile->pArgumentTreeByID)
      for (UINT  iIndex = 0; findArgumentInScriptFileByIndex(pScriptFile, iIndex, pArgument); iIndex++)
         debugArgument(pArgument);
}


BearScriptAPI
VOID  debugStack(const STACK*  pStack)
{
   if (!pStack)
      CONSOLE("Stack is null");
   else
      CONSOLE("Stack: iCount=%d", pStack->iCount);
}


BearScriptAPI
VOID  debugStoredDocument(const STORED_DOCUMENT*  pDocument)
{
   if (!pDocument)
      CONSOLE("StoredDocument is null");
   else
      CONSOLE("StoredDocument '%s': iFlags=%d", pDocument->szFullPath, pDocument->eType);
}

BearScriptAPI
VOID  debugTree(const AVL_TREE*  pTree)
{
   if (!pTree)
      CONSOLE("Tree is null");
   else
      CONSOLE("Tree: iCount=%d", pTree->iCount);
}


BearScriptAPI
VOID  debugVariableName(const VARIABLE_NAME*  pName)
{
   if (!pName)
      CONSOLE("VariableName is null");
   else
      CONSOLE("VariableName '%s': iID=%d  iCount=%d eAssigned=%d", pName->szName, pName->iID, pName->iCount, pName->eAssigned);
}

BearScriptAPI
VOID  debugXMLToken(const XML_TOKENISER*  pToken)
{
   XML_PROPERTY*  pProperty;
   const TCHAR*   szFormat;
   TCHAR          szDebug[256];

   if (!pToken)
      CONSOLE("Token is null");
   else 
   {
      switch (pToken->eType)
      {
      case XTT_TEXT:              szFormat = TEXT("Token: '%s'");                break;
      case XTT_OPENING_TAG:       szFormat = TEXT("Token: '<%s>'");              break;
      case XTT_CLOSING_TAG:       szFormat = TEXT("Token: '</%s>'");             break;
      case XTT_SELF_CLOSING_TAG:  szFormat = TEXT("Token: '<%s />'");            break;
      case XTT_COMMENT_TAG:       szFormat = TEXT("Token: '<!--%s-->'");         break;
      case XTT_CDATA_TAG:         szFormat = TEXT("Token: '<![CDATA[%s]]>'");    break;
      case XTT_SCHEMA_TAG:        szFormat = TEXT("Token: '<?%s>'");             break;
      default:        
         CONSOLE("Token: <whitespace>");  
         return;
      }
      // Generate name
      StringCchPrintf(szDebug, 256, szFormat, pToken->szText);

      // Append properties
      for (UINT  iIndex = 0; pToken->pNode AND findXMLPropertyByIndex(pToken->pNode, iIndex, pProperty); iIndex++)
         utilStringCchCatf(szDebug, 256, TEXT(" (%s:%s)"), pProperty->szName, pProperty->szValue);

      // Output
      consolePrint(szDebug);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

#ifdef PRINT_WINDOW_PROPERTIES
BearScriptAPI
VOID   debugQueryWindow(HWND  hWnd, CONST TCHAR*  szName, CONST BOOL  bIncludeParent)
{
   TCHAR  *szParent,
          *szTitle;
   RECT    rcWindow;

   // [CHECK] Does window exist?
   if (IsWindow(hWnd))
   {
      // Prepare
      GetWindowRect(hWnd, &rcWindow);

      // Output window properties
      CONSOLE("DEBUG: The window '%s' is %s/%s and has title '%s'. It is located at (%d,%d), (%d,%d)", szName, IsWindowVisible(hWnd) ? TEXT("Visible") : TEXT("Hidden"), IsWindowEnabled(hWnd) ? TEXT("Enabled") : TEXT("Disabled"), 
                                                                                                               szTitle = utilGetWindowText(hWnd), rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom);
      // [CHECK] Are we displaying the window parent?
      if (GetParent(hWnd) AND bIncludeParent)
      {
         debugQueryWindow(GetParent(hWnd), szParent = utilCreateStringf(64, TEXT("Parent of %s"), szName), FALSE);
         utilDeleteString(szParent);
      }

      // Cleanup
      utilDeleteString(szTitle);
   }
   else
      CONSOLE("DEBUG: The window '%s' does not exist", szName);
}
#endif


/// Function name  : getFrameStack
// Description     : Generates frame stack
// 
// CONTEXT     oContext  : [in]     CPU context
// DWORD64*    pStack    : [in/out] Stack address
// const UINT  iFrameMax : [in]     Size of array
// 
// Return Value   : Number of frames returned
// 
UINT  getFrameStack(CONTEXT  oContext, DWORD64*  pStack, const UINT  iFrameMax)
{
   DWORD        eMachine;  
   STACKFRAME64 oFrame;  
   BOOL         bResult;
   UINT         iCount;
   
   // Prepare
   ZeroMemory(&oFrame, sizeof(STACKFRAME64));  

   // Setup context
   oFrame.AddrPC.Mode = AddrModeFlat;  
   oFrame.AddrFrame.Mode = AddrModeFlat;  
   oFrame.AddrStack.Mode = AddrModeFlat;  

   #ifdef _M_X64  // 64Bit
      oFrame.AddrPC.Offset = oContext.Rip;  
      oFrame.AddrFrame.Offset = oContext.Rbp;  
      oFrame.AddrStack.Offset = oContext.Rsp;  
      eMachine = IMAGE_FILE_MACHINE_AMD64;  
   #else   // 32bit
      oFrame.AddrPC.Offset = oContext.Eip;  
      oFrame.AddrPC.Offset = oContext.Ebp;  
      oFrame.AddrPC.Offset = oContext.Esp;  
      eMachine = IMAGE_FILE_MACHINE_I386;  
   #endif  

   // Build stack Trace
   for (iCount = 0, bResult = TRUE; bResult AND iCount < iFrameMax; iCount++)  
   {  
      if (bResult = StackWalk64(eMachine, GetCurrentProcess(), GetCurrentThread(), &oFrame, &oContext, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))  
         pStack[iCount] = oFrame.AddrPC.Offset;
   }  

   // Return stack size
   return iCount;
}

 
/// Function name  : generateStackTrace
// Description     : Generates a stack trace
// 
// CONTEXT  oContext   : [in] Current context
// 
// Return Value   : List of FunctionCalls representing the stack trace
// 
BearScriptAPI
LIST*  generateStackTrace(CONTEXT  oContext)
{
   IMAGEHLP_LINEW64  oLine;
   SymbolInfo        oSymbol;
   HANDLE            hProcess = GetCurrentProcess();
   LIST*             pOutput;
   DWORD64           pFrames[63];
   int               iFrameCount;

   // Prepare
   iFrameCount = getFrameStack(oContext, pFrames, 63);
   pOutput     = createList(destructorFunctionCall);

   // Extract symbol names
   for(int i = 0; i < iFrameCount; i++ )
   {
      DWORD   dwDummy = NULL;

      // Prepare
      utilZeroObject(&oSymbol, SymbolInfo);
      utilZeroObject(&oLine, IMAGEHLP_LINEW64);
      oSymbol.si.MaxNameLen   = MAX_SYM_NAME;
      oSymbol.si.SizeOfStruct = sizeof(SYMBOL_INFOW);
      oLine.SizeOfStruct      = sizeof(IMAGEHLP_LINEW64);

      // Resolve symbol, Filename, LineNumber
      SymFromAddrW(hProcess, pFrames[i], 0, &oSymbol.si);
      SymGetLineFromAddrW64(hProcess, oSymbol.si.Address, &dwDummy, &oLine);

      /*if (utilCompareString(oSymbol.si.Name, "generateStackTrace"))
         continue;*/

      // Append to trace  [Ignore system calls]
      if (oLine.FileName AND utilFindCharacter(oLine.FileName, '\\'))
         appendObjectToList(pOutput, (LPARAM)createFunctionCall2(utilFindCharacterReverse(oLine.FileName, '\\') + 1, oSymbol.si.Name, oLine.LineNumber));

      // [MAIN ADDED] Abort
      if (utilCompareString(oSymbol.si.Name, "wWinMain"))
         break;
   }

   // Return trace
   return pOutput;
}




/// Function name  : identifyMessage
// Description     : Resolves a windows message to a string
// 
// UINT  iMessage   : [in] 
// 
// Return Value   : Message name
// 
BearScriptAPI
const TCHAR*  identifyMessage(UINT  iMessage)
{
   const TCHAR*  szOutput;

   // Examine message
   switch (iMessage)
   {
   default:                                szOutput = TEXT("Unrecognised");                    break;
   case WM_NULL:                           szOutput = TEXT("WM_NULL");                         break;
   case WM_CREATE:                         szOutput = TEXT("WM_CREATE");                       break;
   case WM_DESTROY:                        szOutput = TEXT("WM_DESTROY");                      break;
   case WM_MOVE:                           szOutput = TEXT("WM_MOVE");                         break;
   case WM_SIZE:                           szOutput = TEXT("WM_SIZE");                         break;
   case WM_ACTIVATE:                       szOutput = TEXT("WM_ACTIVATE");                     break;
   case WM_SETFOCUS:                       szOutput = TEXT("WM_SETFOCUS");                     break;
   case WM_KILLFOCUS:                      szOutput = TEXT("WM_KILLFOCUS");                    break;
   case WM_ENABLE:                         szOutput = TEXT("WM_ENABLE");                       break;
   case WM_SETREDRAW:                      szOutput = TEXT("WM_SETREDRAW");                    break;
   case WM_SETTEXT:                        szOutput = TEXT("WM_SETTEXT");                      break;
   case WM_GETTEXT:                        szOutput = TEXT("WM_GETTEXT");                      break;
   case WM_GETTEXTLENGTH:                  szOutput = TEXT("WM_GETTEXTLENGTH");                break;
   case WM_PAINT:                          szOutput = TEXT("WM_PAINT");                        break;
   case WM_CLOSE:                          szOutput = TEXT("WM_CLOSE");                        break;
   case WM_QUERYENDSESSION:                szOutput = TEXT("WM_QUERYENDSESSION");              break;
   case WM_QUERYOPEN:                      szOutput = TEXT("WM_QUERYOPEN");                    break;
   case WM_ENDSESSION:                     szOutput = TEXT("WM_ENDSESSION");                   break;
   case WM_QUIT:                           szOutput = TEXT("WM_QUIT");                         break;
   case WM_ERASEBKGND:                     szOutput = TEXT("WM_ERASEBKGND");                   break;
   case WM_SYSCOLORCHANGE:                 szOutput = TEXT("WM_SYSCOLORCHANGE");               break;
   case WM_SHOWWINDOW:                     szOutput = TEXT("WM_SHOWWINDOW");                   break;
   case WM_SETTINGCHANGE:                  szOutput = TEXT("WM_WININICHANGE/WM_SETTINGCHANGE");break;
   case WM_DEVMODECHANGE:                  szOutput = TEXT("WM_DEVMODECHANGE");                break;
   case WM_ACTIVATEAPP:                    szOutput = TEXT("WM_ACTIVATEAPP");                  break;
   case WM_FONTCHANGE:                     szOutput = TEXT("WM_FONTCHANGE");                   break;
   case WM_TIMECHANGE:                     szOutput = TEXT("WM_TIMECHANGE");                   break;
   case WM_CANCELMODE:                     szOutput = TEXT("WM_CANCELMODE");                   break;
   case WM_SETCURSOR:                      szOutput = TEXT("WM_SETCURSOR");                    break;
   case WM_MOUSEACTIVATE:                  szOutput = TEXT("WM_MOUSEACTIVATE");                break;
   case WM_CHILDACTIVATE:                  szOutput = TEXT("WM_CHILDACTIVATE");                break;
   case WM_QUEUESYNC:                      szOutput = TEXT("WM_QUEUESYNC");                    break;
   case WM_GETMINMAXINFO:                  szOutput = TEXT("WM_GETMINMAXINFO");                break;
   case WM_PAINTICON:                      szOutput = TEXT("WM_PAINTICON");                    break;
   case WM_ICONERASEBKGND:                 szOutput = TEXT("WM_ICONERASEBKGND");               break;
   case WM_NEXTDLGCTL:                     szOutput = TEXT("WM_NEXTDLGCTL");                   break;
   case WM_SPOOLERSTATUS:                  szOutput = TEXT("WM_SPOOLERSTATUS");                break;
   case WM_DRAWITEM:                       szOutput = TEXT("WM_DRAWITEM");                     break;
   case WM_MEASUREITEM:                    szOutput = TEXT("WM_MEASUREITEM");                  break;
   case WM_DELETEITEM:                     szOutput = TEXT("WM_DELETEITEM");                   break;
   case WM_VKEYTOITEM:                     szOutput = TEXT("WM_VKEYTOITEM");                   break;
   case WM_CHARTOITEM:                     szOutput = TEXT("WM_CHARTOITEM");                   break;
   case WM_SETFONT:                        szOutput = TEXT("WM_SETFONT");                      break;
   case WM_GETFONT:                        szOutput = TEXT("WM_GETFONT");                      break;
   case WM_SETHOTKEY:                      szOutput = TEXT("WM_SETHOTKEY");                    break;
   case WM_GETHOTKEY:                      szOutput = TEXT("WM_GETHOTKEY");                    break;
   case WM_QUERYDRAGICON:                  szOutput = TEXT("WM_QUERYDRAGICON");                break;
   case WM_COMPAREITEM:                    szOutput = TEXT("WM_COMPAREITEM");                  break;
   case WM_GETOBJECT:                      szOutput = TEXT("WM_GETOBJECT");                    break;
   case WM_COMPACTING:                     szOutput = TEXT("WM_COMPACTING");                   break;
   case WM_COMMNOTIFY:                     szOutput = TEXT("WM_COMMNOTIFY");                   break;
   case WM_WINDOWPOSCHANGING:              szOutput = TEXT("WM_WINDOWPOSCHANGING");            break;
   case WM_WINDOWPOSCHANGED:               szOutput = TEXT("WM_WINDOWPOSCHANGED");             break;
   case WM_POWER:                          szOutput = TEXT("WM_POWER");                        break;
   case WM_COPYDATA:                       szOutput = TEXT("WM_COPYDATA");                     break;
   case WM_CANCELJOURNAL:                  szOutput = TEXT("WM_CANCELJOURNAL");                break;
   case WM_NOTIFY:                         szOutput = TEXT("WM_NOTIFY");                       break;
   case WM_INPUTLANGCHANGEREQUEST:         szOutput = TEXT("WM_INPUTLANGCHANGEREQUEST");       break;
   case WM_INPUTLANGCHANGE:                szOutput = TEXT("WM_INPUTLANGCHANGE");              break;
   case WM_TCARD:                          szOutput = TEXT("WM_TCARD");                        break;
   case WM_HELP:                           szOutput = TEXT("WM_HELP");                         break;
   case WM_USERCHANGED:                    szOutput = TEXT("WM_USERCHANGED");                  break;
   case WM_NOTIFYFORMAT:                   szOutput = TEXT("WM_NOTIFYFORMAT");                 break;
   case WM_CONTEXTMENU:                    szOutput = TEXT("WM_CONTEXTMENU");                  break;
   case WM_STYLECHANGING:                  szOutput = TEXT("WM_STYLECHANGING");                break;
   case WM_STYLECHANGED:                   szOutput = TEXT("WM_STYLECHANGED");                 break;
   case WM_DISPLAYCHANGE:                  szOutput = TEXT("WM_DISPLAYCHANGE");                break;
   case WM_GETICON:                        szOutput = TEXT("WM_GETICON");                      break;
   case WM_SETICON:                        szOutput = TEXT("WM_SETICON");                      break;
   case WM_NCCREATE:                       szOutput = TEXT("WM_NCCREATE");                     break;
   case WM_NCDESTROY:                      szOutput = TEXT("WM_NCDESTROY");                    break;
   case WM_NCCALCSIZE:                     szOutput = TEXT("WM_NCCALCSIZE");                   break;
   case WM_NCHITTEST:                      szOutput = TEXT("WM_NCHITTEST");                    break;
   case WM_NCPAINT:                        szOutput = TEXT("WM_NCPAINT");                      break;
   case WM_NCACTIVATE:                     szOutput = TEXT("WM_NCACTIVATE");                   break;
   case WM_GETDLGCODE:                     szOutput = TEXT("WM_GETDLGCODE");                   break;
   case WM_SYNCPAINT:                      szOutput = TEXT("WM_SYNCPAINT");                    break;
   case WM_NCMOUSEMOVE:                    szOutput = TEXT("WM_NCMOUSEMOVE");                  break;
   case WM_NCLBUTTONDOWN:                  szOutput = TEXT("WM_NCLBUTTONDOWN");                break;
   case WM_NCLBUTTONUP:                    szOutput = TEXT("WM_NCLBUTTONUP");                  break;
   case WM_NCLBUTTONDBLCLK:                szOutput = TEXT("WM_NCLBUTTONDBLCLK");              break;
   case WM_NCRBUTTONDOWN:                  szOutput = TEXT("WM_NCRBUTTONDOWN");                break;
   case WM_NCRBUTTONUP:                    szOutput = TEXT("WM_NCRBUTTONUP");                  break;
   case WM_NCRBUTTONDBLCLK:                szOutput = TEXT("WM_NCRBUTTONDBLCLK");              break;
   case WM_NCMBUTTONDOWN:                  szOutput = TEXT("WM_NCMBUTTONDOWN");                break;
   case WM_NCMBUTTONUP:                    szOutput = TEXT("WM_NCMBUTTONUP");                  break;
   case WM_NCMBUTTONDBLCLK:                szOutput = TEXT("WM_NCMBUTTONDBLCLK");              break;
   case WM_NCXBUTTONDOWN:                  szOutput = TEXT("WM_NCXBUTTONDOWN");                break;
   case WM_NCXBUTTONUP:                    szOutput = TEXT("WM_NCXBUTTONUP");                  break;
   case WM_NCXBUTTONDBLCLK:                szOutput = TEXT("WM_NCXBUTTONDBLCLK");              break;
   case WM_INPUT_DEVICE_CHANGE:            szOutput = TEXT("WM_INPUT_DEVICE_CHANGE");          break;
   case WM_INPUT:                          szOutput = TEXT("WM_INPUT");                        break;
   case WM_KEYDOWN:                        szOutput = TEXT("WM_KEYFIRST/WM_KEYDOWN");          break;
   case WM_KEYUP:                          szOutput = TEXT("WM_KEYUP");                        break;
   case WM_CHAR:                           szOutput = TEXT("WM_CHAR");                         break;
   case WM_DEADCHAR:                       szOutput = TEXT("WM_DEADCHAR");                     break;
   case WM_SYSKEYDOWN:                     szOutput = TEXT("WM_SYSKEYDOWN");                   break;
   case WM_SYSKEYUP:                       szOutput = TEXT("WM_SYSKEYUP");                     break;
   case WM_SYSCHAR:                        szOutput = TEXT("WM_SYSCHAR");                      break;
   case WM_SYSDEADCHAR:                    szOutput = TEXT("WM_SYSDEADCHAR");                  break;
   case WM_UNICHAR:                        szOutput = TEXT("WM_UNICHAR");                      break;
   case WM_IME_STARTCOMPOSITION:           szOutput = TEXT("WM_IME_STARTCOMPOSITION");         break;
   case WM_IME_ENDCOMPOSITION:             szOutput = TEXT("WM_IME_ENDCOMPOSITION");           break;
   case WM_IME_COMPOSITION:                szOutput = TEXT("WM_IME_COMPOSITION");              break;
   case WM_INITDIALOG:                     szOutput = TEXT("WM_INITDIALOG");                   break;
   case WM_COMMAND:                        szOutput = TEXT("WM_COMMAND");                      break;
   case WM_SYSCOMMAND:                     szOutput = TEXT("WM_SYSCOMMAND");                   break;
   case WM_TIMER:                          szOutput = TEXT("WM_TIMER");                        break;
   case WM_HSCROLL:                        szOutput = TEXT("WM_HSCROLL");                      break;
   case WM_VSCROLL:                        szOutput = TEXT("WM_VSCROLL");                      break;
   case WM_INITMENU:                       szOutput = TEXT("WM_INITMENU");                     break;
   case WM_INITMENUPOPUP:                  szOutput = TEXT("WM_INITMENUPOPUP");                break;
#if(WINVER >= 0x0601)
   case WM_GESTURE:                        szOutput = TEXT("WM_GESTURE");                      break;
   case WM_GESTURENOTIFY:                  szOutput = TEXT("WM_GESTURENOTIFY");                break;
#endif
   case WM_MENUSELECT:                     szOutput = TEXT("WM_MENUSELECT");                   break;
   case WM_MENUCHAR:                       szOutput = TEXT("WM_MENUCHAR");                     break;
   case WM_ENTERIDLE:                      szOutput = TEXT("WM_ENTERIDLE");                    break;
   case WM_MENURBUTTONUP:                  szOutput = TEXT("WM_MENURBUTTONUP");                break;
   case WM_MENUDRAG:                       szOutput = TEXT("WM_MENUDRAG");                     break;
   case WM_MENUGETOBJECT:                  szOutput = TEXT("WM_MENUGETOBJECT");                break;
   case WM_UNINITMENUPOPUP:                szOutput = TEXT("WM_UNINITMENUPOPUP");              break;
   case WM_MENUCOMMAND:                    szOutput = TEXT("WM_MENUCOMMAND");                  break;
   case WM_CHANGEUISTATE:                  szOutput = TEXT("WM_CHANGEUISTATE");                break;
   case WM_UPDATEUISTATE:                  szOutput = TEXT("WM_UPDATEUISTATE");                break;
   case WM_QUERYUISTATE:                   szOutput = TEXT("WM_QUERYUISTATE");                 break;
   case WM_CTLCOLORMSGBOX:                 szOutput = TEXT("WM_CTLCOLORMSGBOX");               break;
   case WM_CTLCOLOREDIT:                   szOutput = TEXT("WM_CTLCOLOREDIT");                 break;
   case WM_CTLCOLORLISTBOX:                szOutput = TEXT("WM_CTLCOLORLISTBOX");              break;
   case WM_CTLCOLORBTN:                    szOutput = TEXT("WM_CTLCOLORBTN");                  break;
   case WM_CTLCOLORDLG:                    szOutput = TEXT("WM_CTLCOLORDLG");                  break;
   case WM_CTLCOLORSCROLLBAR:              szOutput = TEXT("WM_CTLCOLORSCROLLBAR");            break;
   case WM_CTLCOLORSTATIC:                 szOutput = TEXT("WM_CTLCOLORSTATIC");               break;
   case WM_MOUSEMOVE:                      szOutput = TEXT("WM_MOUSEMOVE");                    break;
   case WM_LBUTTONDOWN:                    szOutput = TEXT("WM_LBUTTONDOWN");                  break;
   case WM_LBUTTONUP:                      szOutput = TEXT("WM_LBUTTONUP");                    break;
   case WM_LBUTTONDBLCLK:                  szOutput = TEXT("WM_LBUTTONDBLCLK");                break;
   case WM_RBUTTONDOWN:                    szOutput = TEXT("WM_RBUTTONDOWN");                  break;
   case WM_RBUTTONUP:                      szOutput = TEXT("WM_RBUTTONUP");                    break;
   case WM_RBUTTONDBLCLK:                  szOutput = TEXT("WM_RBUTTONDBLCLK");                break;
   case WM_MBUTTONDOWN:                    szOutput = TEXT("WM_MBUTTONDOWN");                  break;
   case WM_MBUTTONUP:                      szOutput = TEXT("WM_MBUTTONUP");                    break;
   case WM_MBUTTONDBLCLK:                  szOutput = TEXT("WM_MBUTTONDBLCLK");                break;
   case WM_MOUSEWHEEL:                     szOutput = TEXT("WM_MOUSEWHEEL");                   break;
   case WM_XBUTTONDOWN:                    szOutput = TEXT("WM_XBUTTONDOWN");                  break;
   case WM_XBUTTONUP:                      szOutput = TEXT("WM_XBUTTONUP");                    break;
   case WM_XBUTTONDBLCLK:                  szOutput = TEXT("WM_XBUTTONDBLCLK");                break;
   case WM_MOUSEHWHEEL:                    szOutput = TEXT("WM_MOUSEHWHEEL");                  break;
   case WM_PARENTNOTIFY:                   szOutput = TEXT("WM_PARENTNOTIFY");                 break;
   case WM_ENTERMENULOOP:                  szOutput = TEXT("WM_ENTERMENULOOP");                break;
   case WM_EXITMENULOOP:                   szOutput = TEXT("WM_EXITMENULOOP");                 break;
   case WM_NEXTMENU:                       szOutput = TEXT("WM_NEXTMENU");                     break;
   case WM_SIZING:                         szOutput = TEXT("WM_SIZING");                       break;
   case WM_CAPTURECHANGED:                 szOutput = TEXT("WM_CAPTURECHANGED");               break;
   case WM_MOVING:                         szOutput = TEXT("WM_MOVING");                       break;
   case WM_POWERBROADCAST:                 szOutput = TEXT("WM_POWERBROADCAST");               break;
   case WM_DEVICECHANGE:                   szOutput = TEXT("WM_DEVICECHANGE");                 break;
   case WM_MDICREATE:                      szOutput = TEXT("WM_MDICREATE");                    break;
   case WM_MDIDESTROY:                     szOutput = TEXT("WM_MDIDESTROY");                   break;
   case WM_MDIACTIVATE:                    szOutput = TEXT("WM_MDIACTIVATE");                  break;
   case WM_MDIRESTORE:                     szOutput = TEXT("WM_MDIRESTORE");                   break;
   case WM_MDINEXT:                        szOutput = TEXT("WM_MDINEXT");                      break;
   case WM_MDIMAXIMIZE:                    szOutput = TEXT("WM_MDIMAXIMIZE");                  break;
   case WM_MDITILE:                        szOutput = TEXT("WM_MDITILE");                      break;
   case WM_MDICASCADE:                     szOutput = TEXT("WM_MDICASCADE");                   break;
   case WM_MDIICONARRANGE:                 szOutput = TEXT("WM_MDIICONARRANGE");               break;
   case WM_MDIGETACTIVE:                   szOutput = TEXT("WM_MDIGETACTIVE");                 break;
   case WM_MDISETMENU:                     szOutput = TEXT("WM_MDISETMENU");                   break;
   case WM_ENTERSIZEMOVE:                  szOutput = TEXT("WM_ENTERSIZEMOVE");                break;
   case WM_EXITSIZEMOVE:                   szOutput = TEXT("WM_EXITSIZEMOVE");                 break;
   case WM_DROPFILES:                      szOutput = TEXT("WM_DROPFILES");                    break;
   case WM_MDIREFRESHMENU:                 szOutput = TEXT("WM_MDIREFRESHMENU");               break;
#if(WINVER >= 0x0601)
   case WM_TOUCH:                          szOutput = TEXT("WM_TOUCH");                        break;
#endif
   case WM_IME_SETCONTEXT:                 szOutput = TEXT("WM_IME_SETCONTEXT");               break;
   case WM_IME_NOTIFY:                     szOutput = TEXT("WM_IME_NOTIFY");                   break;
   case WM_IME_CONTROL:                    szOutput = TEXT("WM_IME_CONTROL");                  break;
   case WM_IME_COMPOSITIONFULL:            szOutput = TEXT("WM_IME_COMPOSITIONFULL");          break;
   case WM_IME_SELECT:                     szOutput = TEXT("WM_IME_SELECT");                   break;
   case WM_IME_CHAR:                       szOutput = TEXT("WM_IME_CHAR");                     break;
   case WM_IME_REQUEST:                    szOutput = TEXT("WM_IME_REQUEST");                  break;
   case WM_IME_KEYDOWN:                    szOutput = TEXT("WM_IME_KEYDOWN");                  break;
   case WM_IME_KEYUP:                      szOutput = TEXT("WM_IME_KEYUP");                    break;
   case WM_MOUSEHOVER:                     szOutput = TEXT("WM_MOUSEHOVER");                   break;
   case WM_MOUSELEAVE:                     szOutput = TEXT("WM_MOUSELEAVE");                   break;
   case WM_NCMOUSEHOVER:                   szOutput = TEXT("WM_NCMOUSEHOVER");                 break;
   case WM_NCMOUSELEAVE:                   szOutput = TEXT("WM_NCMOUSELEAVE");                 break;
   case WM_WTSSESSION_CHANGE:              szOutput = TEXT("WM_WTSSESSION_CHANGE");            break;
   case WM_TABLET_FIRST:                   szOutput = TEXT("WM_TABLET_FIRST");                 break;
   case WM_TABLET_LAST:                    szOutput = TEXT("WM_TABLET_LAST");                  break;
   case WM_CUT:                            szOutput = TEXT("WM_CUT");                          break;
   case WM_COPY:                           szOutput = TEXT("WM_COPY");                         break;
   case WM_PASTE:                          szOutput = TEXT("WM_PASTE");                        break;
   case WM_CLEAR:                          szOutput = TEXT("WM_CLEAR");                        break;
   case WM_UNDO:                           szOutput = TEXT("WM_UNDO");                         break;
   case WM_RENDERFORMAT:                   szOutput = TEXT("WM_RENDERFORMAT");                 break;
   case WM_RENDERALLFORMATS:               szOutput = TEXT("WM_RENDERALLFORMATS");             break;
   case WM_DESTROYCLIPBOARD:               szOutput = TEXT("WM_DESTROYCLIPBOARD");             break;
   case WM_DRAWCLIPBOARD:                  szOutput = TEXT("WM_DRAWCLIPBOARD");                break;
   case WM_PAINTCLIPBOARD:                 szOutput = TEXT("WM_PAINTCLIPBOARD");               break;
   case WM_VSCROLLCLIPBOARD:               szOutput = TEXT("WM_VSCROLLCLIPBOARD");             break;
   case WM_SIZECLIPBOARD:                  szOutput = TEXT("WM_SIZECLIPBOARD");                break;
   case WM_ASKCBFORMATNAME:                szOutput = TEXT("WM_ASKCBFORMATNAME");              break;
   case WM_CHANGECBCHAIN:                  szOutput = TEXT("WM_CHANGECBCHAIN");                break;
   case WM_HSCROLLCLIPBOARD:               szOutput = TEXT("WM_HSCROLLCLIPBOARD");             break;
   case WM_QUERYNEWPALETTE:                szOutput = TEXT("WM_QUERYNEWPALETTE");              break;
   case WM_PALETTEISCHANGING:              szOutput = TEXT("WM_PALETTEISCHANGING");            break;
   case WM_PALETTECHANGED:                 szOutput = TEXT("WM_PALETTECHANGED");               break;
   case WM_HOTKEY:                         szOutput = TEXT("WM_HOTKEY");                       break;
   case WM_PRINT:                          szOutput = TEXT("WM_PRINT");                        break;
   case WM_PRINTCLIENT:                    szOutput = TEXT("WM_PRINTCLIENT");                  break;
   case WM_APPCOMMAND:                     szOutput = TEXT("WM_APPCOMMAND");                   break;
   case WM_THEMECHANGED:                   szOutput = TEXT("WM_THEMECHANGED");                 break;
   case WM_CLIPBOARDUPDATE:                szOutput = TEXT("WM_CLIPBOARDUPDATE");              break;
   case WM_DWMCOMPOSITIONCHANGED:          szOutput = TEXT("WM_DWMCOMPOSITIONCHANGED");        break;
   case WM_DWMNCRENDERINGCHANGED:          szOutput = TEXT("WM_DWMNCRENDERINGCHANGED");        break;
   case WM_DWMCOLORIZATIONCOLORCHANGED:    szOutput = TEXT("WM_DWMCOLORIZATIONCOLORCHANGED");  break;
   case WM_DWMWINDOWMAXIMIZEDCHANGE:       szOutput = TEXT("WM_DWMWINDOWMAXIMIZEDCHANGE");     break;
#if(WINVER >= 0x0601)
   case WM_DWMSENDICONICTHUMBNAIL:         szOutput = TEXT("WM_DWMSENDICONICTHUMBNAIL");           break;
   case WM_DWMSENDICONICLIVEPREVIEWBITMAP: szOutput = TEXT("WM_DWMSENDICONICLIVEPREVIEWBITMAP");   break;
#endif
   case WM_GETTITLEBARINFOEX:              szOutput = TEXT("WM_GETTITLEBARINFOEX");            break;
   case WM_HANDHELDFIRST:                  szOutput = TEXT("WM_HANDHELDFIRST");                break;
   case WM_HANDHELDLAST:                   szOutput = TEXT("WM_HANDHELDLAST");                 break;
   case WM_AFXFIRST:                       szOutput = TEXT("WM_AFXFIRST");                     break;
   case WM_AFXLAST:                        szOutput = TEXT("WM_AFXLAST");                      break;
   case WM_PENWINFIRST:                    szOutput = TEXT("WM_PENWINFIRST");                  break;
   case WM_PENWINLAST:                     szOutput = TEXT("WM_PENWINLAST");                   break;
   case WM_APP:                            szOutput = TEXT("WM_APP");                          break;
   case WM_USER:                           szOutput = TEXT("WM_USER");                         break;
   }

   return szOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


