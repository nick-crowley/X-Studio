//
// XML Parser.cpp : Powerful XML parser
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// OnException: Print to console
#define    ON_EXCEPTION()    printException(pException);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createXMLProperty
// Description     : Creates a new XML Tag Property from the property name
// 
// CONST TCHAR*  szName : [in] Name of the property
// 
// Return Value   : New XML Tag Property, you are responsible for destroying it
// 
XML_PROPERTY*  createXMLProperty(CONST TCHAR*  szName)
{
   XML_PROPERTY*  pNewProperty = utilCreateEmptyObject(XML_PROPERTY);  // New property

   // Set name property
   StringCchCopy(pNewProperty->szName, 32, szName);

   // Return new object
   return pNewProperty;
}



/// Function name  : createXMLTokeniser
// Description     : Create an XML tokeniser
// 
// CONST TCHAR*  szInput : [in] Input XML to be tokenised
// 
// Return Value   : New XML tokeniser, you are responsible for destroying it
// 
XML_TOKENISER*  createXMLTokeniser(const TCHAR*  szFileName, const TCHAR*  szInput)
{
   XML_TOKENISER*  pTokeniser = utilCreateEmptyObject(XML_TOKENISER);

   // Set properties
   pTokeniser->szSource    = szInput;
   pTokeniser->szNextToken = szInput;
   pTokeniser->szFileName  = szFileName;
   pTokeniser->iLineNumber = 1;

   // Return new tokeniser
   return pTokeniser;
}


/// Function name  : createXMLTree
// Description     : Creates a tree structure for holding the contents of an XML hierarchy
// 
// Return Value   : New XML tree, you are responsible for destroying it
// 
BearScriptAPI
XML_TREE*  createXMLTree()
{
   XML_TREE*  pNewTree = utilCreateEmptyObject(XML_TREE);

   // Create new root
   pNewTree->pRoot  = createXMLTreeRoot();
   pNewTree->iCount = 0;

   // Return tree
   return pNewTree;
}


/// Function name  : createXMLTreeNode
// Description     : Creates an XML tree node from the current token of an XML Tokeniser
// 
// XML_TREE_NODE*        pParent : [in] Parent node for the new node
// CONST XML_TOKENISER*  pToken  : [in] Current token (containing the XML tag and properties)
// 
// Return Value   : New XML Tree Node, you are responsible for destroying it
// 
XML_TREE_NODE*   createXMLTreeNode(const XML_TOKENISER*  pToken)
{
   XML_TREE_NODE*   pNewNode = utilCreateEmptyObject(XML_TREE_NODE);  // New tree node

   // [CHECK] Nodes can only be created from certain tags
   ASSERT(pToken->eType == XTT_OPENING_TAG OR pToken->eType == XTT_SELF_CLOSING_TAG);

   // Set properties
   pNewNode->iLineNumber = pToken->iLineNumber;
   pNewNode->iIndex      = 0;

   // Create lists
   pNewNode->oChildren.pfnDeleteItem   = destructorXMLTreeNode;      //pNewNode->pChildren   = createList(destructorXMLTreeNode);
   pNewNode->oProperties.pfnDeleteItem = destructorXMLProperty;      //pNewNode->pProperties = createList(destructorXMLProperty);

   // Parse tag name properties into node
   translateXMLTagIntoNode(pToken->szText, pToken->iCount, pNewNode);

   // Return new node
   return pNewNode;
}


/// Function name  : createXMLTreeRoot
// Description     : Creates an empty XML Tree Node for use as the XML Tree root
// 
// Return Value   : New XML Tree Node, you are responsible for destroying it
// 
XML_TREE_NODE*   createXMLTreeRoot()
{
   XML_TREE_NODE*  pNewRoot = utilCreateEmptyObject(XML_TREE_NODE);  // New root node

   // Set properties
   StringCchCopy(pNewRoot->szName, 24, TEXT("root"));

   // Create lists
   pNewRoot->oChildren.pfnDeleteItem   = destructorXMLTreeNode;      //pNewNode->pChildren   = createList(destructorXMLTreeNode);
   pNewRoot->oProperties.pfnDeleteItem = destructorXMLProperty;      //pNewNode->pProperties = createList(destructorXMLProperty);

   // Return new root
   return pNewRoot;
}


/// Function name  : deleteXMLProperty
// Description     : Deletes an XMLProperty object
// 
// XML_PROPERTY*  &pProperty   : [in] XML Property object to delete
// 

VOID  deleteXMLProperty(XML_PROPERTY*  &pProperty)
{   
   // Delete value
   utilSafeDeleteString(pProperty->szValue);

   // Delete calling object
   utilDeleteObject(pProperty);
}


/// Function name  : deleteXMLTokeniser
// Description     : Destroy an XML tokenising object
// 
// XML_TOKENISER*  &pTokeniser   : [in] XML Tokeniser to destroy
// 
VOID  deleteXMLTokeniser(XML_TOKENISER*  &pTokeniser)
{
   // Delete current token (if any)
   //utilSafeDeleteString(pTokeniser->szText);

   // Delete calling object
   utilDeleteObject(pTokeniser);
}


/// Function name  : deleteXMLTree
// Description     : Delete an XML Tree and any items it contains
// 
// XML_TREE*  &pTree : [in] XML Tree to destroy
// 
BearScriptAPI
VOID  deleteXMLTree(XML_TREE*  &pTree)
{
   TRY
   {
      // [CHECK] Tree must exist
      if (pTree != NULL)
      {
         // Delete nodes
         deleteXMLTreeNode(pTree->pRoot);

         // Delete calling object
         utilDeleteObject(pTree);
      }
   }
   CATCH0("Unable to delete XML tree")
}


/// Function name  : deleteXMLTreeNode
// Description     : delete an XMLTreeNode and any children and properties it contains
// 
// XML_TREE_NODE*  &pNode : [in] XMLTreeNode to destroy
// 
VOID  deleteXMLTreeNode(XML_TREE_NODE*  &pNode)
{
   // Delete node text
   utilSafeDeleteString(pNode->szText);

   // Delete lists
   deleteListContents(&pNode->oChildren);    // deleteList(pNode->pChildren);
   deleteListContents(&pNode->oProperties);  // deleteList(pNode->pProperties);

   // Delete calling object
   utilDeleteObject(pNode);
}


/// Function name  : destructorXMLProperty
// Description     : Callback function for a 'deleteListItem' so it can delete item data for XMLProperty lists
// 
// LPARAM  pListItem : [in] ListItem data that is actually an XMLProperty object
// 
VOID  destructorXMLProperty(LPARAM  pListItem)
{
   deleteXMLProperty((XML_PROPERTY*&)pListItem);
}


/// Function name  : destructorXMLTreeNode
// Description     : Callback function for 'deleteListItem' so it can delete XMLTreeNode list items
// 
// LPARAM  pItemData : [in] ListItem data that is actually an XMLTreeNode object
// 
VOID  destructorXMLTreeNode(LPARAM  pItemData)
{
   deleteXMLTreeNode((XML_TREE_NODE*&)pItemData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getXMLNodeCount
// Description     : Determine the number of child nodes an XMLTreeNode possesses
// 
// CONST XML_TREE_NODE*  pNode : [in] Node to examine
// 
// Return Value   : Number of children
// 
BearScriptAPI 
UINT  getXMLNodeCount(CONST XML_TREE_NODE*  pNode)
{
   return pNode->oChildren.iCount;
}


/// Function name  : getXMLTreeItemCount
// Description     : Return the number of items in an XMLTree
// 
// CONST AVL_TREE*  pTree : [in] Tree to query
// 
// Return Value   : Number of items within the tree
// 
UINT  getXMLTreeItemCount(CONST XML_TREE*  pTree)
{
   return pTree->iCount;
}


/// Function name  : isStringNumeric
// Description     : Determines whether a string is entirely numbers or not
// 
// CONST TCHAR*  szString : [in] String to test
// 
// Return Value   : TRUE if string contains only numbers, FALSE if string is empty or contains non-number characters
// 
BearScriptAPI
BOOL  isStringNumeric(CONST TCHAR*  szString)
{
   return lstrlen(szString) AND (utilFindCharacterNotInSet(szString, "0123456789") == NULL);
}


/// Function name  : isStringWhitespace
// Description     : Determine whether a string is entire whitespace or not
// 
// CONST TCHAR*  szString : [in] String to test
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isStringWhitespace(CONST TCHAR*  szString)
{
   return (utilFindCharacterNotInSet(szString, "\r\n\t ") == NULL);
}


/// Function name  : pushXMLTagStack
// Description     : Add a new tag to an XML parse stack
// 
// XML_TAG_STACK*  pStack     : [in/out] XML parse stack to push tag onto
// CONST TCHAR*    szTag      : [in]     Tag to push onto the stack
// CONST UINT      iTagLength : [in]     Length of tag, in characters
// 
VOID  pushXMLTagStack(XML_TAG_STACK*  pStack, const XML_TOKENISER*  pTag)
{
   TCHAR  *szTagCopy,      // Copy of the input tag
          *szNameEnd;      // Marker to the end of the tag name (and start of the properties)

   // Duplicate tag
   szTagCopy = utilDuplicateString(pTag->szText, pTag->iCount);

   // Truncate tag to exclude any properties
   if (szNameEnd = utilFindCharacter(szTagCopy, ' '))
      szNameEnd[0] = NULL;

   // Add to stack
   pushObjectOntoStack(pStack, (LPARAM)szTagCopy);
   
}

/// Function name  : popXMLTagStack
// Description     : Remove and delete an item from an XML parse stack
// 
// XML_TAG_STACK*  pStack   : [in] XML parse stack to pop
// 
VOID  popXMLTagStack(XML_TAG_STACK*  pStack)
{
   TCHAR*  szTopItem;   // Current top item

   /// Pop stack
   if (szTopItem = (TCHAR*)popObjectFromStack(pStack))
      // [SUCCESS] Delete item
      utilDeleteString(szTopItem);
}

/// Function name  : topXMLTagStack
// Description     : Retrieve but not remove the top item from an XML parse stack
// 
// CONST XML_TAG_STACK*  pStack : [in] XML parse stack to top
// 
// Return Value   : Top entry in the stack or NULL if stack is empty
// 
CONST TCHAR*  topXMLTagStack(CONST XML_TAG_STACK*  pStack)
{
   return (TCHAR*)topStackObject(pStack);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendChildToXMLTreeNode
// Description     : Append an XML Tree node to a specified node's list of children
// 
// XML_TREE*       pTree   : [in] XML Tree containing the nodes
// XML_TREE_NODE*  pParent : [in] XML Tree Node to be appended
// XML_TREE_NODE*  pChild  : [in] XML Tree Node to append
// 
VOID  appendChildToXMLTreeNode(XML_TREE*  pTree, XML_TREE_NODE*  pParent, XML_TREE_NODE*  pChild)
{
   // Set ListItem + Index
   pChild->pParent = pParent;
   pChild->pItem   = createListItem((LPARAM)pChild);
   pChild->iIndex  = getXMLNodeCount(pParent);  

   // Append
   appendItemToList(&pParent->oChildren, pChild->pItem);

   // Increment tree size
   pTree->iCount++;
}

/// Function name  : appendPropertyToXMLTreeNode
// Description     : Append a property to an XMLTreeNode's list of properties
// 
// XML_TREE_NODE*  pNode     : [in/out] Node to add property to
// XML_PROPERTY*   pProperty : [in]     Property to add
// 
VOID  appendPropertyToXMLTreeNode(XML_TREE_NODE*  pNode, XML_PROPERTY*  pProperty)
{
   // Append to properties list
   appendObjectToList(&pNode->oProperties, (LPARAM)pProperty);
}

/// Function name  : calculateCharacterCountInString
// Description     : Count the number of occurrences of a specified character within a string
// 
// CONST TCHAR*  szString    : [in] String to search
// CONST TCHAR   chCharacter : [in] Character to search for
// 
// Return Value   : Number of occurrences
// 
UINT  calculateCharacterCountInString(CONST TCHAR*  szString, CONST TCHAR  chCharacter)
{
   TCHAR*  szIterator;  // String iterator
   UINT    iResult;     // Running total of character occurrences

   // Prepare
   iResult = 0;

   // Iterate through all the occurrences of the specified character
   if (szString)
      for (szIterator = utilFindCharacter(szString, chCharacter); szIterator; szIterator = utilFindCharacter(&szIterator[1], chCharacter))
         iResult++;

   // Return occurrence count
   return iResult;
}

/// Function name  : findNextXMLToken
// Description     : Retrieve the next XML token from an XML string using an XML tokeniser
// 
// XML_TOKENISER*  pOutput : [in/out] XML Tokeniser containing the input string. On output this will contain the next token.
// ERROR_STACK*    &pError : [out]    New Error message, if any
// 
// Return Value   : FALSE if there are no more tokens. 
///           NOTE: This function does not return FALSE if there is an error, you must check 'pError' for that.
// 
BOOL  findNextXMLToken(XML_TOKENISER*  pOutput, ERROR_STACK*  &pError)
{
   CONST TCHAR  *szTokenEnd,     // Marks the end of the token
                *szTokenStart;   // Marks the beginning of the token

   TRY
   {
      // Prepare
      szTokenStart = pOutput->szNextToken;
      pError       = NULL;

      /// [NO MORE TOKENS] Return FALSE
      if (!szTokenStart OR !szTokenStart[0])
      {
         pOutput->szText[0] = NULL;
         pOutput->iCount = NULL;
         return FALSE;
      }

      // Determine whether token is a tag or not
      pOutput->eType = (szTokenStart[0] == '<' ? XTT_OPENING_TAG : XTT_TEXT);

      // [CHECK] Is this a comment tag?
      if (utilCompareStringN(szTokenStart, "<!--", 4))
      {
         /// [COMMENT] Search for the closing comment brace
         if (szTokenEnd = utilFindSubString(&szTokenStart[3], "-->"))
         {
            pOutput->eType = XTT_COMMENT_TAG;
            szTokenEnd += 2;  // Position 'TokenEnd' at the '>' character
         }
         else
            // [ERROR] "An unclosed <%s> tag was detected in the attached XML snippet from line %d of '%s'"
            pError = generateDualError(HERE(IDS_XML_UNCLOSED_OPENING_TAG), TEXT("comment"), pOutput->iLineNumber, pOutput->szFileName);
      }
      // [CHECK] Is this a character-data block?
      else if (utilCompareStringN(szTokenStart, "<![CDATA[", 9))
      {
         /// [CHARACTER DATA] Search for the closing character data brace
         if (szTokenEnd = utilFindSubString(&szTokenStart[3], "]]>"))
         {
            pOutput->eType = XTT_COMMENT_TAG;
            szTokenEnd += 2;  // Position 'TokenEnd' at the '>' character
         }
         else
            // [ERROR] "An unclosed <%s> tag was detected in the attached XML snippet from line %d of '%s'"
            pError = generateDualError(HERE(IDS_XML_UNCLOSED_OPENING_TAG), TEXT("!CDATA"), pOutput->iLineNumber, pOutput->szFileName);
      }
      else
         /// [NON-COMMENT] Search for closing tag (or an illegal opening tag)
         szTokenEnd = utilFindCharacterInSet(&szTokenStart[1], "<>");
      
      // [NON-FINAL TOKEN]
      if (!pError AND szTokenEnd)
      {
         // Examine token type
         switch (pOutput->eType)
         {
         /// [TAG] Determine tag type and extract without the angle brackets
         case XTT_OPENING_TAG:
            // [CHECK] Ensure tags finish with '>'
            if (szTokenEnd[0] == '<')
            {  
               // [ERROR] "An unexpected opening tag '<' character was detected in the attached XML snippet from line %d of '%s'"
               pError = generateDualError(HERE(IDS_XML_UNEXPECTED_OPENING_BRACKET), pOutput->iLineNumber, pOutput->szFileName);
               break;
            }
            
            /// Identify tag type
            // [CLOSING] Indicated by the tag starting with '</'
            if (szTokenStart[1] == '/')
               pOutput->eType = XTT_CLOSING_TAG;
            // [SELF CLOSING] Indicated by the tag ending with '/>'
            else if (szTokenEnd[-1] == '/')
               pOutput->eType = XTT_SELF_CLOSING_TAG;
            // [COMMENT] Indicated by the tag starting with '<!'
            else if (szTokenStart[1] == '!')
               pOutput->eType = XTT_COMMENT_TAG;
            // [SCHEMA] Indicated by the tag starting with '<?'
            else if (szTokenStart[1] == '?')
               pOutput->eType = XTT_SCHEMA_TAG;

            // Fall through...

         /// [COMMENT TAG] Already identified, just copy the text
         case XTT_COMMENT_TAG:
            /// Copy and measure tag (without the special characters)
            switch (pOutput->eType)
            {
            /// [OPENING TAG] Copy without < and >
            case XTT_OPENING_TAG:
               pOutput->iCount = max(0, szTokenEnd - szTokenStart - 1);
               StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[1], pOutput->iCount);
               break;
            /// [CLOSING TAG] Copy without </ and >
            case XTT_CLOSING_TAG:
               pOutput->iCount = max(0, szTokenEnd - szTokenStart - 2);
               StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[2], pOutput->iCount);
               break;
            /// [SELF CLOSING TAG] Copy with < and />
            case XTT_SELF_CLOSING_TAG:
               pOutput->iCount = max(0, szTokenEnd - szTokenStart - 2);
               StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[1], pOutput->iCount);
               break;
            /// [COMMENT TAG] Copy without <!-- and -->
            case XTT_COMMENT_TAG:
               pOutput->iCount = max(0, szTokenEnd - szTokenStart - 6);
               StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[4], pOutput->iCount);
               break;
            /// [SCHEMA TAG] Copy without <? and ?>
            case XTT_SCHEMA_TAG:
               pOutput->iCount = max(0, szTokenEnd - szTokenStart - 3);
               StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[2], pOutput->iCount);
               break;
            }

            // Set 'next token' as the character beyond the end of the tag
            pOutput->szNextToken = &szTokenEnd[1];
            break;

         /// [TEXT] Extract tag and identify whitespace 
         case XTT_TEXT:
   #ifdef BUG_FIX
            // [CHECK] Ensure text finishes with '<'
            if (szTokenEnd[0] == '>')
            {  
               // [ERROR] "An unexpected closing tag '>' character was detected in the attached XML snippet from line %d of '%s'"
               pError = generateDualError(HERE(IDS_XML_UNEXPECTED_CLOSING_BRACKET), pOutput->iLineNumber, pOutput->szFileName);
               attachXMLToError(pError, szTokenStart);
               break;
            }
   #endif
            // [CHECK] Ensure text finishes with '<'
            if (szTokenEnd[0] == '>')                    /// [FIX] BUG:002 'You receieve multiple 'an unexpected close tag was detected' errors when loading scripts created by eXscriptor for the first time'
            {  
               //// [ERROR] "An unexpected closing tag '>' character was detected in the attached XML snippet from line %d of '%s'"
               //pError = generateDualWarning(HERE(IDS_XML_UNEXPECTED_CLOSING_BRACKET), pOutput->iLineNumber, pOutput->szFileName);

               // Search for next opening character
               szTokenEnd = utilFindCharacter(&szTokenEnd[1], '<');
               ASSERT(szTokenEnd);            
            }

            // Measure text
            pOutput->iCount = (szTokenEnd - szTokenStart);

            /// [CHECK] Attempt to copy string
            if (pOutput->iCount >= MAX_STRING OR StringCchCopyN(pOutput->szText, MAX_STRING, szTokenStart, pOutput->iCount) != S_OK)
               // [ERROR] "A string exceeding 32,768 characters was detected on line %d of '%s', this is not supported by X-Studio"
               pError = generateDualError(HERE(IDS_XML_TEXT_EXCEEDS_BUFFER), pOutput->iLineNumber, pOutput->szFileName);

            // [CHECK] Check for character entities
            else if (utilFindSubString(pOutput->szText, "&#"))
               // [FOUND] Replace entities
               pOutput->iCount = performXMLCharacterEntityReplacement(pOutput->szText, pOutput->iCount);
            
            // Set 'next token' as the start of the next tag
            pOutput->szNextToken = szTokenEnd;
            break;
         }
      }
      // [FINAL TOKEN] Set the next token to NULL
      else
      {
         // Set next token to NULL
         pOutput->szNextToken = NULL;

         /// Extract final token text
         StringCchCopy(pOutput->szText, MAX_STRING, szTokenStart);
         pOutput->iCount = lstrlen(pOutput->szText);
      }

      /// [WHITESPACE] Determine whether text is whitespace
      if (pOutput->eType == XTT_TEXT AND isStringWhitespace(pOutput->szText))
         pOutput->eType = XTT_WHITESPACE;
      
      /// [LINE NUMBER] Calculate the new line number
      pOutput->iLineNumber += calculateCharacterCountInString(pOutput->szText, '\n');     // [FIX] BUG:1018 'Errors produced by findNextXMLToken() don't have a Line number in any of the messages'

      /// [NEW TAG] Create node from token
      if (!pOutput->bBlocking AND (pOutput->eType == XTT_OPENING_TAG OR pOutput->eType == XTT_SELF_CLOSING_TAG))
         pOutput->pNode = createXMLTreeNode(pOutput);

      // [ERROR] Append XML snippets to all errors
      if (pError)
      {
         // Append XML snippet
         generateOutputTextFromError(pError);
         attachXMLToError(pError, szTokenStart, (szTokenStart - pOutput->szSource));

         // Skip beyond token
         pOutput->szNextToken = &szTokenEnd[1];
         // Zero any existing token
         pOutput->szText[0] = NULL;
         pOutput->iCount    = 0;
      }

      // Return TRUE to indicate there are more tokens, even if an error occurred
      return TRUE;
   }
   CATCH0("Unable to retrieve next token");
   return FALSE;
}


/// Function name  : findLastXMLTreeNode
// Description     : Retrieve the previous sibling of a specified node
// 
// XML_TREE_NODE*               pNode         : [in]  The input node, this will not be altered 
// CONST XML_NODE_RELATIONSHIP  eRelationship : [in]  Whether to find the previous sibling or the parent
// XML_TREE_NODE*              &pOutput       : [out] Desired node if successful, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findLastXMLTreeNode(CONST XML_TREE_NODE*  pNode, CONST XML_NODE_RELATIONSHIP  eRelationship, XML_TREE_NODE*  &pOutput)
{
   LIST_ITEM*  pItem;

   switch (eRelationship)
   {
   /// [CHILD] Cannot return previous child
   case XNR_CHILD:
      ASSERT(FALSE);
      pOutput = NULL;
      break;

   /// [SIBLING] Return the next item within the list containing the input node
   case XNR_SIBLING:
      // [CHECK] Cannot perform on the root
      if (pNode->pItem == NULL)
         pOutput = NULL;

      // Go back to the node's previous item
      else if (pNode->pItem->pPrev)
      {
         pItem   = pNode->pItem->pPrev;
         pOutput = extractListItemPointer(pItem, XML_TREE_NODE);
      }
      // [NOT FOUND] Set result to NULL
      else
         pOutput = NULL;
      break;

   /// [PARENT] Return the input node's parent node (if any)
   case XNR_PARENT: 
      pOutput = (pNode->pParent ? pNode->pParent : NULL);
      break;
   }

   // Return TRUE if found, FALSE otherwise
   return (pOutput != NULL);
}

/// Function name  : findNextXMLTreeNode
// Description     : Retrieve the next child or sibling of a specified node
// 
// XML_TREE_NODE*               pNode         : [in]  The input node, this will not be altered 
// CONST XML_NODE_RELATIONSHIP  eRelationship : [in]  Whether to find the first child or first sibling node
// XML_TREE_NODE*              &pOutput       : [out] Desired node if successful, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findNextXMLTreeNode(CONST XML_TREE_NODE*  pNode, CONST XML_NODE_RELATIONSHIP  eRelationship, XML_TREE_NODE*  &pOutput)
{
   LIST_ITEM*  pItem;

   // Prepare
   /*if (!pNode)
      return FALSE;*/

   // Examine relationship
   switch (eRelationship)
   {
   /// [CHILD] Return first child
   case XNR_CHILD:
      // Advance to the first child of the current node (if any)
      if (getXMLNodeCount(pNode))
      {
         pItem = getListHead(&pNode->oChildren);
         pOutput = extractListItemPointer(pItem, XML_TREE_NODE);
      }
      // [NOT FOUND] Set result to NULL
      else
         pOutput = NULL;
      break;

   /// [SIBLING] Return the next item within the list containing the input node
   case XNR_SIBLING:
      // [CHECK] Cannot perform on the root
      if (pNode->pItem == NULL)
         pOutput = NULL;

      // Advance to the node's next item
      else if (pNode->pItem->pNext)
      {
         pItem = pNode->pItem->pNext;
         pOutput = extractListItemPointer(pItem, XML_TREE_NODE);
      }
      // [NOT FOUND] Set result to NULL
      else
         pOutput = NULL;
      break;

   /// [PARENT] Return the input node's parent node (if any)
   case XNR_PARENT: 
      pOutput = (pNode->pParent ? pNode->pParent : NULL);
      break;
   }

   // Return TRUE if found, FALSE otherwise
   return (pOutput != NULL);
}



/// Function name  : findXMLPropertyByIndex
// Description     : Finds the specified property of an XML node by index
// 
// XML_TREE_NODE*  pNode   : [in]  Node containing the property
// CONST UINT      iIndex  : [in]  Zero-based index of the property to retrieve
// XML_PROPERTY*  &pOutput : [out] Desired property if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findXMLPropertyByIndex(CONST XML_TREE_NODE*  pNode, CONST UINT  iIndex, XML_PROPERTY*  &pOutput)
{
   // Search properties
   return findListObjectByIndex(&pNode->oProperties, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findXMLPropertyByName
// Description     : Search for a property within a node by name
// 
// CONST XML_TREE_NODE*  pNode      : [in] Node
// CONST TCHAR*          szProperty : [in] Property name
// XML_PROPERTY*        &pOutput    : [out] Property
// 
// Return Value   : TRUE/FALSE
// 
BOOL   findXMLPropertyByName(CONST XML_TREE_NODE*  pNode, CONST TCHAR*  szProperty, XML_PROPERTY*  &pOutput)
{
   XML_PROPERTY*  pProperty;

   // Prepare
   pOutput = NULL;

   /// Iterate through node properties
   for (LIST_ITEM*  pIterator = getListHead(&pNode->oProperties); !pOutput AND (pProperty = extractListItemPointer(pIterator, XML_PROPERTY)); pIterator = pIterator->pNext)
   {
      /// Determine whether property name matches
      if (utilCompareStringVariables(pProperty->szName, szProperty))
         // [FOUND] Set result and abort
         pOutput = pProperty;
   }

   // Return TRUE if found
   return pOutput != NULL;
}


/// Function name  : findXMLTreeNodeByIndex
// Description     : Finds the specified child of a node by index
// 
// XML_TREE_NODE*   pParentNode : [in]  Parent node of the child to retrieve
// CONST UINT       iIndex      : [in]  Zero-based index of the child to retrieve
// XML_TREE_NODE*  &pOutput     : [out] Desired node if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findXMLTreeNodeByIndex(CONST XML_TREE_NODE*  pParentNode, CONST UINT  iIndex, XML_TREE_NODE*  &pOutput)
{
   return findListObjectByIndex(&pParentNode->oChildren, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findXMLTreeNodeByName
// Description     : Finds a node within an XML tree by name.
// 
// XML_TREE_NODE*   pStartNode       : [in]  Node to start searching from
// CONST TCHAR*     szName           : [in]  Name of node to search for
// CONST BOOL       bRecurseChildren : [in]  Whether to search child nodes recursively or just search the immediate child nodes
// XML_TREE_NODE*  &pOutput          : [out] Desired node if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findXMLTreeNodeByName(CONST XML_TREE_NODE*  pStartNode, CONST TCHAR*  szName, CONST BOOL  bRecurseChildren, XML_TREE_NODE*  &pOutput)
{
   XML_TREE_NODE*  pCurrentNode; // Node within the list item currently being processed

   // [CHECK] Ensure start node exists
   ASSERT(pStartNode != NULL);

   // Prepare
   pOutput = NULL;

   /// Iterate through child node list
   for (LIST_ITEM*  pIterator = getListHead(&pStartNode->oChildren); pCurrentNode = extractListItemPointer(pIterator, XML_TREE_NODE); pIterator = pIterator->pNext)
   {
      // Check node name
      if (utilCompareStringVariables(pCurrentNode->szName, szName))
      {
         /// [FOUND-DIRECT] Set result and abort
         pOutput = pCurrentNode;
         break;
      }
      /// Recurse into children
      else if (bRecurseChildren AND findXMLTreeNodeByName(pCurrentNode, szName, bRecurseChildren, pOutput))
         /// [FOUND-INDIRECT] Abort...
         break;
   }

   /// Return TRUE if found, FALSE otherwise
   return (pOutput != NULL);
}


/// Function name  : generateXMLTree
// Description     : Generate an XML Tree from an input string of XML
// 
// CONST TCHAR*         szXML        : [in]     Input string containing the XML
// CONST UINT           iInputLength : [in]     Length of input string, in characters
// CONST TCHAR*         szFileName   : [in]     Filename   [Only used for error reporting]
// HWND                 hParentWnd   : [in]     Ignored
// XML_TREE*           &pOutput      : [out]    New XMLTree, You are responsible for destroying it
// OPERATION_PROGRESS*  pProgress    : [in/out][optional] Operation progress object, if feedback is required
// ERROR_QUEUE*         pErrorQueue  : [in/out] Error message queue
// 
// Return Value   : OR_SUCCESS - File was parsed successfully, any errors were ignored by the user
//                  OR_ABORTED - File was NOT parsed successfully due to the user aborting after minor errors
// 
BearScriptAPI
OPERATION_RESULT  generateXMLTree(CONST TCHAR*  szXML, CONST UINT  iInputLength, CONST TCHAR*  szFileName, HWND  hParentWnd, XML_TREE*  &pOutput, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT   eResult;         // Operation result
   XML_TOKENISER*     pToken;          // XML token currently being processed
   XML_TAG_STACK*     pStack;          // XML parse stack
   XML_TREE_NODE*     pCurrentNode;    // Parent for the node currently being processed
   ERROR_STACK*       pError;          // Current error, if any

   // Prepare
   eResult  = OR_SUCCESS;
   pError   = NULL;

   __try
   {
      // Prepare
      pOutput      = createXMLTree();
      pCurrentNode = pOutput->pRoot;
      
      // Create parsing objects
      pToken = createXMLTokeniser(szFileName, szXML);
      pStack = createXMLTagStack();

      // [OPTIONAL] Define progress based on the number of pages processed
      if (pProgress)
         updateOperationProgressMaximum(pProgress, iInputLength);

      // Iterate through tokens
      while (findNextXMLToken(pToken, pError))
      {
         // Update progress object, if present
         if (pProgress)
            updateOperationProgressValue(pProgress, pToken->szNextToken - pToken->szSource);

         // [CHECK] Ensure token was parsed succesfully
         if (!pError)
         {
            switch (pToken->eType)
            {
            /// [COMMENT, SCHEMA, WHITESPACE] Ignore...
            case XTT_COMMENT_TAG:
            case XTT_CDATA_TAG:
            case XTT_SCHEMA_TAG:
            case XTT_WHITESPACE:
               continue;

            /// [TAG/TEXT] Process if unblocked, skip otherwise
            case XTT_OPENING_TAG:
            case XTT_SELF_CLOSING_TAG:
            case XTT_CLOSING_TAG:
            case XTT_TEXT:
               // [BLOCKING] Skip all tags until </sourcetext> is found
               if (pToken->bBlocking)
               {
                  if (pToken->eType != XTT_CLOSING_TAG OR !utilCompareString(pToken->szText, "sourcetext"))
                     continue;
                  
                  pToken->bBlocking = FALSE;
               }

               // Examine tag
               switch (pToken->eType)
               {
               /// [OPENING TAG] Add tag to stack + Add node to tree + Set as 'current node'
               case XTT_OPENING_TAG:
                  // Push tag onto stack
                  pushXMLTagStack(pStack, pToken);

                  // Append new node to 'CurrentNode' + Update 'CurrentNode'
                  appendChildToXMLTreeNode(pOutput, pCurrentNode, pToken->pNode);      //pNewNode = createXMLTreeNode(pCurrentNode, pToken);
                  pCurrentNode = pToken->pNode;

                  // [BLOCK] Skip all tags within <sourcetext>
                  if (utilCompareString(pToken->szText, "sourcetext"))
                     pToken->bBlocking = TRUE;
                  break;
                  
               /// [SELF CLOSING TAG] Add node to tree
               case XTT_SELF_CLOSING_TAG:
                  // Append new node to 'CurrentNode'
                  appendChildToXMLTreeNode(pOutput, pCurrentNode, pToken->pNode);      // pNewNode = createXMLTreeNode(pCurrentNode, pToken);
                  break;
                  
               /// [CLOSING TAG] Ensure tag matches the currently open tag
               case XTT_CLOSING_TAG:
                  // [CHECK] Ensure stack isn't empty
                  if (!getStackItemCount(pStack))
                     // [ERROR] "An unexpected closing tag </%s> was detected on line %d of '%s'"
                     pError = generateDualError(HERE(IDS_XML_UNEXPECTED_CLOSING_TAG), pToken->szText, pToken->iLineNumber, pToken->szFileName);

                  // [CHECK] Ensure tags match
                  else if (utilCompareStringVariables(pToken->szText, topXMLTagStack(pStack)))
                  {
                     // [SUCCESS] Remove matching tag. Set parent as 'Current'
                     popXMLTagStack(pStack);
                     pCurrentNode = pCurrentNode->pParent;
                  }
                  else  // [ERROR] "The closing tag </%s> on line %d does not match the currently open <%s> tag in '%s'"
                     pError = generateDualError(HERE(IDS_XML_MISMATCHED_CLOSING_TAG), pToken->szText, pToken->iLineNumber, topXMLTagStack(pStack), pToken->szFileName);
                  break;

               /// [TEXT] Save text in the current node
               case XTT_TEXT:
                  // [CHECK] Ensure stack isn't empty / Node doesn't already have text
                  if (getStackItemCount(pStack) AND !pCurrentNode->szText)
                     // [SUCCESS] Save node text
                     pCurrentNode->szText = utilDuplicateString(pToken->szText, pToken->iCount);
                  else
                     // [ERROR] "Unexpected text '%s' was detected on line %d of '%s'"
                     pError = generateDualError(HERE(IDS_XML_UNEXPECTED_TEXT), pToken->szText, pToken->iLineNumber, pToken->szFileName);
                  break;
               } // END: switch (pToken->eType)
            }

            // [DEBUG]
            //debugXMLToken(pToken);

            /// [SYNTAX ERROR] "A minor error has been detected in the syntax of the XML file '%s'"
            if (pError)
            {
               generateOutputTextFromError(pError);
               //enhanceError(pError, ERROR_ID(IDS_XML_MINOR_SYNTAX_ERROR), szFileName);
               attachXMLToError(pError, pToken->szNextToken, pToken->szNextToken - pToken->szSource);
            }

         } // END: if (!pError)
         /*else
            /// [PARSE ERROR] "A minor error has occurred while parsing the XML file '%s'"
            enhanceError(pError, ERROR_ID(IDS_XML_MINOR_PARSING_ERROR), szFileName);*/

         // [ERROR] Abort
         if (pError)
         {
            pushErrorQueue(pErrorQueue, pError);
            eResult = OR_FAILURE;
            break;
         }

      } // END: while (findNextXMLToken(pToken, pError))

      /// [CHECK] Detect any open tags remaining
      while (eResult == OR_SUCCESS AND getStackItemCount(pStack))
      {
         // [WARNING] "An unclosed <%s> tag was detected on an unknown line of '%s'"
         pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_XML_UNCLOSED_TAG_REMAINING), topXMLTagStack(pStack), pToken->szFileName));
         generateOutputTextFromLastError(pErrorQueue);
         popXMLTagStack(pStack);
      }

      // Cleanup and return result
      deleteXMLTagStack(pStack);
      deleteXMLTokeniser(pToken);
      return eResult;

      //// [ERROR] Destroy output tree
      //if (eResult != OR_SUCCESS)
      //   deleteXMLTree(pOutput);
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION3("Unable to parse XML file '%s' at token '%s' on line %u", szFileName, pToken->szText, pToken->iLineNumber);
      return OR_FAILURE;
   }
}


/// Function name  : getXMLPropertyInteger
// Description     : Retrieve the integer version of a specified property
// 
// CONST XML_TREE_NODE*  pNode      : [in]  XMLTreeNode to check
// CONST TCHAR*          szProperty : [in]  Name of the property to retrieve
// INT&                  iOutput    : [out] Integer value of property if successful, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE if property has no associated value or property could not found
// 
BOOL   getXMLPropertyInteger(CONST XML_TREE_NODE*  pNode, CONST TCHAR*  szProperty, INT&  iOutput)
{
   XML_PROPERTY*  pProperty;

   // Prepare
   iOutput = NULL;

   /// Lookup property
   if (!findXMLPropertyByName(pNode, szProperty, pProperty) OR !pProperty->szValue)
      return FALSE;
   
   // [FOUND] Convert value to an integer and return TRUE
   iOutput = utilConvertStringToInteger(pProperty->szValue);
   return TRUE;
}

 

/// Function name  : getXMLPropertyString
// Description     : Retrieve the string version of a specified property
// 
// CONST XML_TREE_NODE*  pNode      : [in]  XMLTreeNode to check
// CONST TCHAR*          szProperty : [in]  Name of the property to retrieve
// CONST TCHAR*         &szOutput   : [out] Read-Only string property if successful, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE if property has no associated value or property could not found
// 
BearScriptAPI 
BOOL   getXMLPropertyString(CONST XML_TREE_NODE*  pNode, CONST TCHAR*  szProperty, CONST TCHAR*  &szOutput)
{
   XML_PROPERTY*  pProperty;

   // Prepare
   szOutput = NULL;

   /// Lookup property
   if (!findXMLPropertyByName(pNode, szProperty, pProperty) OR !pProperty->szValue)
      return FALSE;
   
   // [FOUND] Return TRUE
   szOutput = pProperty->szValue;
   return TRUE;
}


/// Function name  :  performXMLCharacterEntityReplacement
// Description     : 
// 
// TCHAR*      szInput      : [in] 
// CONST UINT  iInputLength : [in] 
// 
// Return Value   : New string length
// 
UINT   performXMLCharacterEntityReplacement(TCHAR*  szInput, CONST UINT  iInputLength)
{
   CONST TCHAR  *szPosition,        // Current position
                *szEntity,          // Entity being parsed
                *szSemiColon;       // Semicolon marking the end of the entity
   TCHAR        *szBuffer,          // Output Assembly buffer
                 szCharacter[2];    // Converted entity character as a string
   
   // Prepare
   szCharacter[1] = NULL;
   szPosition     = szInput;

   // Create working buffer
   szBuffer = utilCreateEmptyString(iInputLength);

   /// [CHECK] Find next entity
   while (szPosition AND (szEntity = utilFindSubString(szPosition, "&#")))
   {
      // Copy preceeding characters to output
      if (szEntity - szPosition)
         StringCchCatN(szBuffer, iInputLength, szPosition, szEntity - szPosition);

      /// [CHECK] Find end marker
      if (szSemiColon = utilFindCharacter(szEntity, ';'))
      {
         // [CHECK] Examine encoding type
         if (szEntity[2] == 'x')
            // [HEX]  &#xhhhh
            utilScanf(&szEntity[3], TEXT("%04x"), &szCharacter[0]);
         else
            // [DECIMAL]  &#nnnn
            szCharacter[0] = utilConvertStringToInteger(&szEntity[2]);

         /// [FOUND] Append entity and update position
         StringCchCat(szBuffer, iInputLength, szCharacter);
         szPosition = &szSemiColon[1];
      }
      else
      {
         /// [NOT FOUND] Copy marker and remainder of string to the output buffer
         StringCchCat(szBuffer, iInputLength, szEntity);
         szPosition = NULL;
      }
   }

   /// [COMPLETE] Overwrite input buffer
   StringCchCopy(szInput, iInputLength, szBuffer);

   /// Append remainder of string
   if (szPosition)
      StringCchCat(szInput, iInputLength, szPosition);
   
   // Cleanup and return length
   utilDeleteString(szBuffer);
   return lstrlen(szInput);
}


/// Function name  : translateXMLTagIntoNode
// Description     : Extract the name from an XML tag into an XMLTreeNode.  Also extracts any property name/value pairs
//                      and adds them to the XMLTreeNode's property list
// 
// CONST TCHAR*    szTag   : [in]     XML tag, optionally containing properties as awell as a name
// CONST UINT      iLength : [in]     Length of XML tag, in characters
// XML_TREE_NODE*  pOutput : [in/out] Node to parse the properties into
// 
VOID  translateXMLTagIntoNode(CONST TCHAR*  szTag, CONST UINT  iLength, XML_TREE_NODE*  pOutput)
{
   XML_TAG_PARSE_STATE  eState;        // Current property parsing state
   XML_PROPERTY        *pProperty;     // Current property being parsed
   TCHAR               *szTagCopy,     // Copy of the input tag, for tokenising
                       *szTokenEnd,    // Pointer to the end of the current token
                       *szToken;       // Current token
   // Prepare
   eState = XTPS_PROPERTY_NAME;

   /// [JUST NAME] If tag contains no spaces then there is only the tag name
   if (utilFindCharacterInSet(szTag, " \r\n\t") == NULL)
      StringCchCopy(pOutput->szName, 24, szTag);

   /// [NAME and PROPERTIES] Extract the name and then parse the properties
   else
   {
      // Copy the input string (for tokenising)
      szTagCopy = utilDuplicateString(szTag, iLength);

      // Sever the tag name
      szTokenEnd = utilFindCharacter(szTagCopy, ' ');
      szTokenEnd[0] = NULL;

      /// [NAME] Store in the output node
      StringCchCopy(pOutput->szName, 24, szTagCopy);
      szToken = &szTokenEnd[1];

      /// [PROPERTIES] Tokenise each property name and value manually (to avoid the "" (empty property) problem)
      //   Reference: <sval type="array" size="18">, <sval type="int" val="703" />
      for (szTokenEnd = utilFindCharacter(szToken, '"'); szTokenEnd; szTokenEnd = utilFindCharacter(szToken, '"'))
      {
         // Null terminate token
         szTokenEnd[0] = NULL;

         switch (eState)
         {
         /// [PROPERTY-NAME] Create a new property from the name
         case XTPS_PROPERTY_NAME:
            StrTrim(szToken, TEXT("= \t\r\n"));
            // Append new property to output node
            pProperty = createXMLProperty(szToken);
            appendPropertyToXMLTreeNode(pOutput, pProperty);
            // Switch to property values
            eState = XTPS_PROPERTY_VALUE;
            break;

         /// [VALUE] Store value in the current property
         case XTPS_PROPERTY_VALUE:
            pProperty->szValue = utilDuplicateSimpleString(szToken);

            // [CHECK] Replace any character entities
            if (utilFindSubString(pProperty->szValue, "&#"))
               performXMLCharacterEntityReplacement(pProperty->szValue, lstrlen(pProperty->szValue));

            // Switch to property name
            eState = XTPS_PROPERTY_NAME;
            break;
         }

         // Move to next token
         szToken = &szTokenEnd[1];
      }

      // Cleanup
      utilDeleteString(szTagCopy);
   }
}


/// Function name  : traverseXMLTree
// Description     : Traverse through an XML Tree, printing each node to the console
// 
// XML_TREE_NODE*  pStartNode : [in] Node to start traversal at
// 
BearScriptAPI
VOID  traverseXMLTree(XML_TREE_NODE*  pStartNode)
{
   XML_TREE_NODE*  pCurrentNode;

   // [CHECK] Ensure start node exists
   ASSERT(pStartNode != NULL);

   // Iterate through child node list
   for (LIST_ITEM*  pIterator = getListHead(&pStartNode->oChildren); pCurrentNode = extractListItemPointer(pIterator, XML_TREE_NODE); pIterator = pIterator->pNext)
   {
      // Print tag name
      CONSOLE("Node: '%s' (%u properties)", pCurrentNode->szName, pCurrentNode->oProperties.iCount);

      // Recurse into children
      traverseXMLTree(pCurrentNode);
   }
}
