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
   XML_PROPERTY*  pNewProperty;  // New property

   // Create new object
   pNewProperty = utilCreateEmptyObject(XML_PROPERTY);

   // Set name property
   StringCchCopy(pNewProperty->szName, 24, szName);

   // Return new object
   return pNewProperty;
}


/// Function name  : createXMLTagStack
// Description     : Creates an XML parsing stack
// 
// Return Value   : New XML parsing stack, you are responsible for destroying it
// 
XML_TAG_STACK*  createXMLTagStack()
{
   XML_TAG_STACK*  pStack;  // New stack

   // Create new stack
   pStack = utilCreateEmptyObject(XML_TAG_STACK);

   // Return new stack
   return pStack;
}


/// Function name  : createXMLTokeniser
// Description     : Create an XML tokeniser
// 
// CONST TCHAR*  szInput : [in] Input XML to be tokenised
// 
// Return Value   : New XML tokeniser, you are responsible for destroying it
// 
XML_TOKENISER*  createXMLTokeniser(CONST TCHAR*  szInput)
{
   XML_TOKENISER*  pTokeniser;

   // Create new tokeniser
   pTokeniser = utilCreateEmptyObject(XML_TOKENISER);

   // Set properties
   pTokeniser->szSource    = szInput;
   pTokeniser->szNextToken = szInput;
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
   XML_TREE*  pNewTree;

   // Create new tree
   pNewTree = utilCreateEmptyObject(XML_TREE);

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
XML_TREE_NODE*   createXMLTreeNode(XML_TREE_NODE*  pParent, CONST XML_TOKENISER*  pToken)
{
   XML_TREE_NODE*   pNewNode;  // New tree node

   // [CHECK] Nodes can only be created from certain tags
   ASSERT(pToken->eType == XTT_OPENING_TAG OR pToken->eType == XTT_SELF_CLOSING_TAG);

   // Create new node
   pNewNode = utilCreateEmptyObject(XML_TREE_NODE);

   // Set properties
   pNewNode->pParent     = pParent;
   pNewNode->pChildren   = createList(destructorXMLTreeNode);
   pNewNode->pProperties = createList(destructorXMLProperty);
   pNewNode->iLineNumber = pToken->iLineNumber;
   pNewNode->iIndex      = 0;

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
   XML_TREE_NODE*  pNewRoot;  // New root node

   // Create new root
   pNewRoot = utilCreateEmptyObject(XML_TREE_NODE);

   // Set properties
   StringCchCopy(pNewRoot->szName, 24, TEXT("root"));
   pNewRoot->pChildren   = createList(destructorXMLTreeNode);
   pNewRoot->pProperties = createList(destructorXMLProperty);

   // Return new root
   return pNewRoot;
}


/// Function name  : deleteXMLProperty
// Description     : Deletes an XMLProperty object
// 
// XML_PROPERTY*  &pProperty   : [in] XML Property object to delete
// 
__forceinline 
VOID  deleteXMLProperty(XML_PROPERTY*  &pProperty)
{   
   // Delete value
   utilSafeDeleteString(pProperty->szValue);

   // Delete calling object
   utilDeleteObject(pProperty);
}

/// Function name  : destructorXMLProperty
// Description     : Callback function for a 'deleteListItem' so it can delete item data for XMLProperty lists
// 
// LPARAM  pListItem : [in] ListItem data that is actually an XMLProperty object
// 
__forceinline 
VOID  destructorXMLProperty(LPARAM  pListItem)
{
   deleteXMLProperty((XML_PROPERTY*&)pListItem);
}

/// Function name  : deleteXMLTagStack
// Description     : Delete an XMLTagStack and any items it contains
// 
// XML_TAG_STACK*  &pStack   : [in] XMLTagStack to delete
// 
VOID  deleteXMLTagStack(XML_TAG_STACK*  &pStack)
{
   // Delete any remaining stack items
   while (hasItems(pStack))
      popXMLTagStack(pStack);
   
   // Delete calling object
   utilDeleteObject(pStack);
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
   // [CHECK] Tree must exist
   ASSERT(pTree);

   // Delete nodes
   deleteXMLTreeNode(pTree->pRoot);

   // Delete calling object
   utilDeleteObject(pTree);
}


/// Function name  : deleteXMLTreeNode
// Description     : delete an XMLTreeNode and any children and properties it contains
// 
// XML_TREE_NODE*  &pNode : [in] XMLTreeNode to destroy
// 
__forceinline 
VOID  deleteXMLTreeNode(XML_TREE_NODE*  &pNode)
{
   // Delete node text
   utilSafeDeleteString(pNode->szText);

   // Delete lists
   deleteList(pNode->pChildren);
   deleteList(pNode->pProperties);

   // Delete calling object
   utilDeleteObject(pNode);
}


/// Function name  : destructorXMLTreeNode
// Description     : Callback function for 'deleteListItem' so it can delete XMLTreeNode list items
// 
// LPARAM  pItemData : [in] ListItem data that is actually an XMLTreeNode object
// 
__forceinline 
VOID  destructorXMLTreeNode(LPARAM  pItemData)
{
   deleteXMLTreeNode((XML_TREE_NODE*&)pItemData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : duplicateNodeText
// Description     : Duplicates the node's text
// 
// CONST XML_TREE_NODE*  pNode   : [in] 
// 
// Return Value   : 
// 
TCHAR*  duplicateNodeText(CONST XML_TREE_NODE*  pNode)
{
   return utilDuplicateSimpleString(pNode->szText);
}


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
   return pNode->pChildren->iCount;
}


/// Function name  : getXMLPropertyFromItem
// Description     : Extract the XMLProperty from a ListItem
// 
// CONST LIST_ITEM*  pListItem   : [in] ListItem containing an XMLProperty
// 
// Return Value   : XML Property
// 
XML_PROPERTY*  getXMLPropertyFromItem(CONST LIST_ITEM*  pListItem)
{
   return extractListItemPointer(pListItem, XML_PROPERTY);
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


/// Function name  : getXMLTreeNodeFromItem
// Description     : Extract the XMLTree Node from a ListItem
// 
// CONST LIST_ITEM*  pListItem   : [in] ListItem containing an XML Tree Node
// 
// Return Value   : XML Tree node
// 
XML_TREE_NODE*  getXMLTreeNodeFromItem(CONST LIST_ITEM*  pListItem)
{
   return extractListItemPointer(pListItem, XML_TREE_NODE);
}


/// Function name  : getFirstXMLPropertyListItem
// Description     : Get the first ListItem from an XMLTreeNode's list of properties
// 
// XML_TREE_NODE*  pNode : [in] XML Tree node containing a (possibly empty) list of properties
// 
// Return Value   : First item in the list, or NULL if list is empty
// 
LIST_ITEM*  getFirstXMLPropertyListItem(CONST XML_TREE_NODE*  pNode)
{
   return getListHead(pNode->pProperties);
}


/// Function name  : getFirstXMLTreeNodeListItem
// Description     : Get the first ListItem from an XML Tree Node's list of children
// 
// XML_TREE_NODE*  pNode : [in] XML Tree node containing a (possibly empty) list of children
// 
// Return Value   : First child in the list, or NULL if list is empty
// 
LIST_ITEM*  getFirstXMLTreeNodeListItem(CONST XML_TREE_NODE*  pNode)
{
   return getListHead(pNode->pChildren);
}


/// Function name  : hasChildren
// Description     : Determine whether an XML Tree node has any children or not
// 
// CONST XML_TREE_NODE*  pNode : [in] XML Tree node to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL  hasChildren(CONST XML_TREE_NODE*  pNode)
{
   return (getListItemCount(pNode->pChildren) > 0);
}

/// Function name  : hasItems
// Description     : Determine whether an XML parse stack has any items or not
// 
// CONST XML_TAG_STACK*  pStack   : [in] XML parse stack to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL  hasItems(CONST XML_TAG_STACK*  pStack)
{
   return (pStack->iCount > 0);
}


/// Function name  : hasProperties
// Description     : Determine whether an XML Tree node has any properties or not
// 
// CONST XML_TREE_NODE*  pNode : [in] XML Tree node to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL  hasProperties(CONST XML_TREE_NODE*  pNode)
{
   return (getListItemCount(pNode->pProperties) > 0);
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
VOID  pushXMLTagStack(XML_TAG_STACK*  pStack, CONST TCHAR*  szTag, CONST UINT  iTagLength)
{
   TCHAR  *szTagCopy,      // Copy of the input tag
          *szNameEnd;      // Marker to the end of the tag name (and start of the properties)

   if (pStack->iCount < 32)
   {
      // Duplicate tag
      szTagCopy = utilDuplicateString(szTag, iTagLength);

      // Truncate tag to exclude any properties
      if (szNameEnd = utilFindCharacter(szTagCopy, ' '))
         szNameEnd[0] = NULL;

      // Add to stack
      pStack->pItems[pStack->iCount++] = szTagCopy;
   }
}

/// Function name  : popXMLTagStack
// Description     : Remove and delete an item from an XML parse stack
// 
// XML_TAG_STACK*  pStack   : [in] XML parse stack to pop
// 
VOID  popXMLTagStack(XML_TAG_STACK*  pStack)
{
   TCHAR*  szTopItem;   // Current top item

   // [CHECK] Ensure stack isn't empty
   if (hasItems(pStack))
   {
      // Delete top item
      szTopItem = pStack->pItems[pStack->iCount-1];
      utilDeleteString(szTopItem);
      // Remove from stack
      pStack->pItems[--pStack->iCount] = NULL;
   }
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
   return hasItems(pStack) ? pStack->pItems[pStack->iCount - 1] : NULL;
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
   LIST_ITEM*  pListItem;  // New list item for the node

   // Create new list item from child
   pListItem     = createListItem((LPARAM)pChild);
   pChild->pItem = pListItem;

   /// Add new item as a child of the parent
   pChild->iIndex = getXMLNodeCount(pParent);  // Set appropriate index
   appendItemToList(pParent->pChildren, pListItem);

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
   LIST_ITEM*  pListItem;

   // Create new list item from property
   pListItem = createListItem((LPARAM)pProperty);

   // Add new item to properties list
   appendItemToList(pNode->pProperties, pListItem);
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
// XML_TOKENISER*  &pOutput : [in/out] XML Tokeniser containing the input string. On output this will contain the next token.
// ERROR_STACK*    &pError  : [out]    New Error message, if any
// 
// Return Value   : FALSE if there are no more tokens. 
///           NOTE: This function does not return FALSE if there is an error, you must check 'pError' for that.
// 
BOOL  findNextXMLToken(XML_TOKENISER*  &pOutput, ERROR_STACK*  &pError)
{
   CONST TCHAR  *szTokenEnd,     // Marks the end of the token
                *szTokenStart;   // Marks the beginning of the token

   // [TRACK]
   TRACK_FUNCTION();

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
         // [ERROR] "An unclosed <%s> tag was detected in the attached XML snippet from line %u"
         pError = generateDualError(HERE(IDS_XML_UNCLOSED_OPENING_TAG), TEXT("comment"), pOutput->iLineNumber);
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
         // [ERROR] "An unclosed <%s> tag was detected in the attached XML snippet from line %u"
         pError = generateDualError(HERE(IDS_XML_UNCLOSED_OPENING_TAG), TEXT("!CDATA"), pOutput->iLineNumber);
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
            // [ERROR] "An unexpected opening tag '<' character was detected in the attached XML snippet from line %u"
            pError = generateDualError(HERE(IDS_XML_UNEXPECTED_OPENING_BRACKET), pOutput->iLineNumber);
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
            /*pOutput->szText = &szTokenStart[1];
            szTokenEnd[0]   = NULL;*/
            break;
         /// [CLOSING TAG] Copy without </ and >
         case XTT_CLOSING_TAG:
            pOutput->iCount = max(0, szTokenEnd - szTokenStart - 2);
            StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[2], pOutput->iCount);
            /*pOutput->szText = &szTokenStart[2];
            szTokenEnd[0]   = NULL;*/
            break;
         /// [SELF CLOSING TAG] Copy with < and />
         case XTT_SELF_CLOSING_TAG:
            pOutput->iCount = max(0, szTokenEnd - szTokenStart - 2);
            StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[1], pOutput->iCount);
            /*pOutput->szText = &szTokenStart[1];
            szTokenEnd[-1]  = NULL;*/
            break;
         /// [COMMENT TAG] Copy without <!-- and -->
         case XTT_COMMENT_TAG:
            pOutput->iCount = max(0, szTokenEnd - szTokenStart - 6);
            StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[4], pOutput->iCount);
            /*pOutput->szText = &szTokenStart[4];
            szTokenEnd[-1]  = NULL;*/
            break;
         /// [SCHEMA TAG] Copy without <? and ?>
         case XTT_SCHEMA_TAG:
            pOutput->iCount = max(0, szTokenEnd - szTokenStart - 3);
            StringCchCopyN(pOutput->szText, MAX_STRING, &szTokenStart[2], pOutput->iCount);
            /*pOutput->szText = &szTokenStart[2];
            szTokenEnd[-1]  = NULL;*/
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
            // [ERROR] "An unexpected closing tag '>' character was detected in the attached XML snippet from line %u"
            pError = generateDualError(HERE(IDS_XML_UNEXPECTED_CLOSING_BRACKET), pOutput->iLineNumber);
            attachXMLToError(pError, szTokenStart);
            break;
         }
#endif
         // [CHECK] Ensure text finishes with '<'
         if (szTokenEnd[0] == '>')                    /// [FIX] BUG:002 'You receieve multiple 'an unexpected close tag was detected' errors when loading scripts created by eXscriptor for the first time'
         {  
            //// [ERROR] "An unexpected closing tag '>' character was detected in the attached XML snippet from line %u"
            //pError = generateDualWarning(HERE(IDS_XML_UNEXPECTED_CLOSING_BRACKET), pOutput->iLineNumber);

            // Search for next opening character
            szTokenEnd = utilFindCharacter(&szTokenEnd[1], '<');
            ASSERT(szTokenEnd);            
         }

         /// Measure and copy text
         pOutput->iCount = min(MAX_STRING, szTokenEnd - szTokenStart);
         StringCchCopyN(pOutput->szText, MAX_STRING, szTokenStart, pOutput->iCount);
         /*pOutput->szText = szTokenStart;
         szTokenEnd[0]   = NULL;*/

         // [CHECK] Check for character entities
         if (utilFindSubString(pOutput->szText, "&#"))
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

   // [CHECK] Append XML snippets to all errors
   if (pError)
   {
      // Append XML snippet
      generateOutputTextFromError(pError);
      attachXMLToError(pError, szTokenStart, (szTokenStart - pOutput->szSource));

      /// [ERROR] Skip beyond the end of the current token
      if (isError(pError))
      {
         // Skip beyond token
         pOutput->szNextToken = &szTokenEnd[1];
         // Zero any existing token
         pOutput->szText[0] = NULL;
         pOutput->iCount    = 0;
      }
   }

   // Return TRUE to indicate there are more tokens, even if an error occurred
   END_TRACKING();
   return TRUE;
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
         pOutput = getXMLTreeNodeFromItem(pItem);
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
         pItem = getFirstXMLTreeNodeListItem(pNode);
         pOutput = getXMLTreeNodeFromItem(pItem);
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
         pOutput = getXMLTreeNodeFromItem(pItem);
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
   LIST_ITEM*  pIterator;        // Child nodes list iterator
   UINT        iCurrentIndex;    // Child nodes list iterator index

   // Prepare
   iCurrentIndex = 0;
   pOutput       = NULL;
   
   /// Iterate through child node list
   for (pIterator = getFirstXMLTreeNodeListItem(pParentNode); pOutput == NULL AND iCurrentIndex < getListItemCount(pParentNode->pChildren); pIterator = pIterator->pNext)
      // Check whether current item is the desired item
      if (iIndex == iCurrentIndex++)
         /// [FOUND] Set result and abort search
         pOutput = getXMLTreeNodeFromItem(pIterator);
   
   // Return TRUE if node was found
   return (pOutput != NULL);
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
   for (LIST_ITEM*  pIterator = getFirstXMLTreeNodeListItem(pStartNode); pIterator; pIterator = pIterator->pNext)
   {
      // Extract node from item
      pCurrentNode = getXMLTreeNodeFromItem(pIterator);

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
// CONST TCHAR*         szXML       : [in]     Input string containing the XML
// CONST UINT           iInputLength : [in]    Length of input string, in characters
// CONST TCHAR*         szFileName  : [in]     Filename   [Only used for error reporting]
// HWND                 hParentWnd  : [in]     Parent window to display an error messages
// XML_TREE*           &pOutput     : [out]    New XML Tree if successful, otherwise NULL.  You are responsible for destroying it
// OPERATION_PROGRESS*  pProgress   : [in/out][optional] Operation progress object, if feedback is required
// ERROR_QUEUE*         pErrorQueue : [in/out] Error message queue
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
   XML_TREE_NODE     *pCurrentNode,    // Parent for the node currently being processed
                     *pNewNode;        // New node currently being created 
   ERROR_STACK*       pError;          // Current error, if any
   UINT               iNodeTextLength; // Length of the text in the current node, in characters.  Used only for extending inline text when another node is nested within it.

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Generating XML-Tree from file '%s'", (szFileName ? szFileName : TEXT("(Resource Based)")));

   // Prepare
   eResult  = OR_SUCCESS;
   pError   = NULL;

   /// [GUARD BLOCK]
   __try
   {
      // Prepare
      pOutput      = createXMLTree();
      pCurrentNode = pOutput->pRoot;
      
      // Create parsing objects
      pToken = createXMLTokeniser(szXML);
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

         /// [PARSE ERROR] Determine whether to abort
         if (pError)
         {
            // [ERROR] "A minor error has occurred while parsing the XML file '%s'"
            enhanceError(pError, ERROR_ID(IDS_XML_MINOR_PARSING_ERROR), szFileName);
            attachXMLToError(pError, pToken->szNextToken, pToken->szNextToken - pToken->szSource);
            pushErrorQueue(pErrorQueue, pError);
            //generateOutputTextFromError(pError);

            // [QUESTION] "A minor error has occurred while parsing the XML file '%s', would you like to continue processing?"
            if (displayOperationError(hParentWnd, pError, ERROR_ID(IDS_XML_MINOR_PARSING_ERROR_QUESTION), szFileName) == EH_ABORT)
               eResult = OR_ABORTED;
         }
         else switch (pToken->eType)
         {
         /// [COMMENT, SCHEMA, WHITESPACE] Ignore...
         case XTT_COMMENT_TAG:
         case XTT_SCHEMA_TAG:
         case XTT_WHITESPACE:
            continue;

         default:
            /// [OPENING TAG] Add tag to the stack. update the 'current node'
            if (pToken->eType == XTT_OPENING_TAG)
            {
               // Add to the stack
               pushXMLTagStack(pStack, pToken->szText, pToken->iCount);

               // Create new node and add as a child of the 'current node'
               pNewNode = createXMLTreeNode(pCurrentNode, pToken);
               appendChildToXMLTreeNode(pOutput, pCurrentNode, pNewNode);

               // Set as the 'current node'
               pCurrentNode = pNewNode;
            }
            /// [SELF CLOSING TAG] Add as child of the current node
            else if (pToken->eType == XTT_SELF_CLOSING_TAG)
            {
               // Create new node and add as a child of the 'current node'
               pNewNode = createXMLTreeNode(pCurrentNode, pToken);
               appendChildToXMLTreeNode(pOutput, pCurrentNode, pNewNode);
            }
            // [NON-EMPTY STACK] Check for matching closing tags and store any inline text
            else if (hasItems(pStack))
            {
               switch (pToken->eType)
               {
               /// [CLOSING TAG] Ensure tag matches the currently open tag
               case XTT_CLOSING_TAG:
                  if (utilCompareStringVariables(pToken->szText, topXMLTagStack(pStack)))
                  {
                     // Remove currently open tag. Set it's parent node as active.
                     popXMLTagStack(pStack);
                     pCurrentNode = pCurrentNode->pParent;
                  }
                  else
                     // [ERROR] "The closing tag </%s> on line %u does not match the currently open <%s> tag"
                     pError = generateDualError(HERE(IDS_XML_MISMATCHED_CLOSING_TAG), pToken->szText, pToken->iLineNumber, topXMLTagStack(pStack));
                  break;

               /// [TEXT] Save text in the current node
               case XTT_TEXT:
                  // [NOTE] If you have a node nested within inline text, this node will already contain text.
                  //             eg.  <text>the quick brown <comment>fox</comment> jumped over the</text>
                  if (pCurrentNode->szText == NULL)
                     pCurrentNode->szText = utilDuplicateString(pToken->szText, pToken->iCount);
                  else
                  {
                     // Measure existing length and extend by enough to append token
                     iNodeTextLength = lstrlen(pCurrentNode->szText);
                     pCurrentNode->szText = utilExtendString(pCurrentNode->szText, iNodeTextLength, iNodeTextLength + pToken->iCount);

                     // Calculate new length and append token
                     iNodeTextLength = iNodeTextLength + pToken->iCount;
                     StringCchCat(pCurrentNode->szText, iNodeTextLength + 1, pToken->szText);
                  }               
                  break;
               }
            }
            // [EMPTY STACK] Ensure only opening tags are allowed
            else switch (pToken->eType)
            {
            /// [CLOSING TAG] Unexpected closing tag
            case XTT_CLOSING_TAG:
               // [HACK] Is this a child of a <text> node?
               /*if (topXMLTagStack(pStack)*/
               // [ERROR] "An unexpected closing tag </%s> was detected on line %u"
               pError = generateDualError(HERE(IDS_XML_UNEXPECTED_CLOSING_TAG), pToken->szText, pToken->iLineNumber);
               break;

            /// [TEXT] Unexpected text
            case XTT_TEXT:
               // [ERROR] "Unexpected text '%s' was detected on line %u"
               pError = generateDualError(HERE(IDS_XML_UNEXPECTED_TEXT), pToken->szText, pToken->iLineNumber);
               break;
            }

            /// [SYNTAX ERROR] Determine whether to abort
            if (pError)
            {
               // Add output error and XML attachment
               generateOutputTextFromError(pError);
               attachXMLToError(pError, pToken->szNextToken, pToken->szNextToken - pToken->szSource);

               // [OUTPUT-ERROR] "A minor error has been detected in the syntax of the XML file '%s'"
               enhanceError(pError, ERROR_ID(IDS_XML_MINOR_SYNTAX_ERROR), szFileName);
               pushErrorQueue(pErrorQueue, pError);
               
               // [QUESTION] "A minor error has been detected in the syntax of the XML file '%s', would you like to continue processing?"
               if (displayOperationError(hParentWnd, pError, ERROR_ID(IDS_XML_MINOR_SYNTAX_ERROR_QUESTION), szFileName) == EH_ABORT)
                  eResult = OR_ABORTED;
            }
         } // END: if (token isn't schema, comment or whitespace)

         // [CHECK] Stop processing if user has aborted
         if (eResult != OR_SUCCESS)
            break;

      } // END: for (each XML token)

      /// [CHECK] Detect and open tags remaining
      while (eResult == OR_SUCCESS AND hasItems(pStack))
      {
         // [WARNING] "An unclosed <%s> tag was detected on an unknown line"
         pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_XML_UNCLOSED_OPENING_TAG), topXMLTagStack(pStack)));
         // Remove item
         popXMLTagStack(pStack);
      }

      // [CHECK] Destroy output tree if user aborted
      if (eResult != OR_SUCCESS)
         deleteXMLTree(pOutput);
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pErrorQueue))
   {
      // [FAILURE] "A critical error occurred while parsing the XML file '%s', at token '%s' on line %u."
      enhanceLastError(pErrorQueue, ERROR_ID(IDS_EXCEPTION_GENERATE_XML_TREE), szFileName, TEXT("TEST"), pToken->iLineNumber);
      attachTextToLastError(pErrorQueue, pToken->szText);
      eResult = OR_FAILURE;
   }
   
   // Cleanup and return result
   deleteXMLTagStack(pStack);
   deleteXMLTokeniser(pToken);
   END_TRACKING();
   return eResult;
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

   /// Iterate through node properties
   for (LIST_ITEM*  pIterator = getListHead(pNode->pProperties); pIterator; pIterator = pIterator->pNext)
   {
      // Extract property from item
      pProperty = extractListItemPointer(pIterator, XML_PROPERTY);

      /// Determine whether property name matches (and value exists)
      if (utilCompareStringVariables(pProperty->szName, szProperty) AND pProperty->szValue != NULL)
      {
         // [FOUND] Convert value to an integer and return TRUE
         iOutput = utilConvertStringToInteger(pProperty->szValue);
         return TRUE;
      }
   }

   // [NOT FOUND] Return FALSE
   iOutput = NULL;
   return FALSE;
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

   /// Iterate through node properties
   for (LIST_ITEM*  pIterator = getListHead(pNode->pProperties); pIterator; pIterator = pIterator->pNext)
   {
      // Extract property from item
      pProperty = extractListItemPointer(pIterator, XML_PROPERTY);

      /// Determine whether property name matches (and value exists)
      if (utilCompareStringVariables(pProperty->szName, szProperty) AND pProperty->szValue != NULL)
      {
         // [FOUND] Set result and return TRUE
         szOutput = pProperty->szValue;
         return TRUE;
      }
   }

   // [NOT FOUND] Return FALSE
   szOutput = NULL;
   return FALSE;
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
            pProperty->szValue = utilDuplicateString(szToken, lstrlen(szToken));

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
   for (LIST_ITEM*  pIterator = getFirstXMLTreeNodeListItem(pStartNode); pIterator; pIterator = pIterator->pNext)
   {
      // Extract node from item
      pCurrentNode = getXMLTreeNodeFromItem(pIterator);

      // Print tag name
      CONSOLE("Node: '%s' (%u properties)", pCurrentNode->szName, pCurrentNode->pProperties->iCount);

      // Recurse into children
      traverseXMLTree(pCurrentNode);
   }
}
