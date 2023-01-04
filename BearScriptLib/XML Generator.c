//
// XML Generator.cpp : Helpers for generating XML 
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      GLOBALS / CONSTANTS
/// /////////////////////////////////////////////////////////////////////////////////////////

CONST TCHAR*  szSourceValueTypes[4] = { TEXT("unknown"), TEXT("array"), TEXT("string"), TEXT("int") };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : createSourceValueNodeFromString
// Description     : Create an XMLTreeNode representing an <sval> tag
// 
// XML_TREE_NODE*  pParent : [in] Parent Node
// CONST TCHAR*    szValue : [in] Tag value
// 
// Return Value   : New XMLTreeNode, you are responsible for destroying it
// 
XML_TREE_NODE*  createSourceValueNodeFromString(XML_TREE_NODE*  pParent, CONST TCHAR*  szValue)
{
   XML_TREE_NODE*  pNewNode;     // Node being created

   /// Create new <sval> node
   pNewNode = createXMLTreeNodeFromString(pParent, TEXT("sval"), NULL);

   // Add properties
   appendPropertyToXMLTreeNode(pNewNode, createXMLPropertyFromString(TEXT("type"), szSourceValueTypes[SVT_STRING]));
   appendPropertyToXMLTreeNode(pNewNode, createXMLPropertyFromString(TEXT("val"), szValue));

   // Return new object
   return pNewNode;
}


/// Function name  : createSourceValueNodeFromInteger
// Description     : Creates an XMLTreeNode to represent an <sval> tag containing an integer
// 
// XML_TREE_NODE*           pParent  : [in] Parent node
// CONST SOURCE_VALUE_TYPE  eType    : [in] Indicates whether tag contains an integer or an array
// CONST INT                iValue   : [in] Integer value or Array size
// 
// Return Value   : New XMLTreeNode, you are responsible for destroying it
// 
XML_TREE_NODE*  createSourceValueNodeFromInteger(XML_TREE_NODE*  pParent, CONST SOURCE_VALUE_TYPE  eType, CONST INT  iValue)
{
   XML_TREE_NODE*  pNewNode;     // Node being created

   // [CHECK] Ensure type is either ARRAY or INTEGER
   ASSERT(eType == SVT_ARRAY OR eType == SVT_INTEGER);

   /// Create new <sval> node
   pNewNode = createXMLTreeNodeFromString(pParent, TEXT("sval"), NULL);

   // Add properties
   appendPropertyToXMLTreeNode(pNewNode, createXMLPropertyFromString(TEXT("type"), szSourceValueTypes[eType]));
   appendPropertyToXMLTreeNode(pNewNode, createXMLPropertyFromInteger(eType == SVT_ARRAY ? TEXT("size") : TEXT("val"), iValue));
   
   // Return new object
   return pNewNode;
}


/// Function name  : createXMLTreeNodeFromString
// Description     : Creates an empty, name XML tree node with optional inline text
// 
// XML_TREE_NODE*   pParent  : [in]           Parent node for the new node
// CONST TCHAR*     szName   : [in]           Name of the node
// CONST TCHAR*     szText   : [in][optional] Inline text for the node
// 
// Return Value   : New XML Tree Node, you are responsible for destroying it
// 
XML_TREE_NODE*   createXMLTreeNodeFromString(XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST TCHAR*  szText)
{
   XML_TREE_NODE*   pNewNode = utilCreateEmptyObject(XML_TREE_NODE);  // New tree node

   // Set properties
   StringCchCopy(pNewNode->szName, 24, szName);
   pNewNode->szText = utilSafeDuplicateSimpleString(szText);
   
   // Create children and properties lists
   pNewNode->pParent                   = pParent;
   pNewNode->oChildren.pfnDeleteItem   = destructorXMLTreeNode;      //pNewNode->pChildren   = createList(destructorXMLTreeNode);
   pNewNode->oProperties.pfnDeleteItem = destructorXMLProperty;      //pNewNode->pProperties = createList(destructorXMLProperty);

   // Return new node
   return pNewNode;
}


/// Function name  : createXMLTreeNodeFromInteger
// Description     : Creates an empty, named XML tree node with inline text generated from an integer
// 
// XML_TREE_NODE*   pParent  : [in] Parent node for the new node
// CONST TCHAR*     szName   : [in] Name of the node
// CONST INT        iValue   : [in] Integer to be converted to inline text for the node
// 
// Return Value   : New XML Tree Node, you are responsible for destroying it
// 
XML_TREE_NODE*   createXMLTreeNodeFromInteger(XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST INT  iValue)
{
   XML_TREE_NODE*   pNewNode = utilCreateEmptyObject(XML_TREE_NODE);;  // New tree node

   // Set properties
   StringCchCopy(pNewNode->szName, 24, szName);
   pNewNode->szText = utilCreateStringf(32, TEXT("%d"), iValue);

   // Create children and properties lists
   pNewNode->pParent                   = pParent;
   pNewNode->oChildren.pfnDeleteItem   = destructorXMLTreeNode;      //pNewNode->pChildren   = createList(destructorXMLTreeNode);
   pNewNode->oProperties.pfnDeleteItem = destructorXMLProperty;      //pNewNode->pProperties = createList(destructorXMLProperty);

   // Return new node
   return pNewNode;
}


/// Function name  : createXMLPropertyFromString
// Description     : Creates a new XML Tag Property with a name and string value
// 
// CONST TCHAR*  szName  : [in] Name of the property
// CONST TCHAR*  szValue : [in] Property value
// 
// Return Value   : New XML Tag Property, you are responsible for destroying it
// 
XML_PROPERTY*  createXMLPropertyFromString(CONST TCHAR*  szName, CONST TCHAR*  szValue)
{
   XML_PROPERTY*  pNewProperty = utilCreateEmptyObject(XML_PROPERTY);  // New property

   // Set properties
   StringCchCopy(pNewProperty->szName, 32, szName);
   pNewProperty->szValue = utilDuplicateSimpleString(szValue);

   // Return new object
   return pNewProperty;
}

/// Function name  : createXMLPropertyFromInteger
// Description     : Creates a new XML Tag Property with a name and numeric value
// 
// CONST TCHAR*  szName  : [in] Name of the property
// CONST INT     iValue  : [in] Property value
// 
// Return Value   : New XML Tag Property, you are responsible for destroying it
// 
XML_PROPERTY*  createXMLPropertyFromInteger(CONST TCHAR*  szName, CONST INT  iValue)
{
   XML_PROPERTY*  pNewProperty = utilCreateEmptyObject(XML_PROPERTY);  // New property

   // Set properties
   StringCchCopy(pNewProperty->szName, 32, szName);
   pNewProperty->szValue = utilCreateStringf(24, TEXT("%d"), iValue);

   // Return new object
   return pNewProperty;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendStringNodeToXMLTree
// Description     : Create and append a node with a custom name and inline text
// 
// XML_TREE*       pTree    : [in/out] Tree containing the node
// XML_TREE_NODE*  pParent  : [in/out] Parent node for the new node
// CONST TCHAR*    szName   : [in]     Name of the new node
// CONST TCHAR*    szValue  : [in]     Inline text for the new node
// 
// Return Value   : The new node, which has been appended to the tree. You are not responsibe for destroying it, the tree is
// 
XML_TREE_NODE*  appendStringNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST TCHAR*  szValue)
{
   XML_TREE_NODE*  pNode;     // Node being created

   // Create node from string
   pNode = createXMLTreeNodeFromString(pParent, szName, szValue);

   // Attach node to tree and return it
   appendChildToXMLTreeNode(pTree, pParent, pNode);
   return pNode;
}


/// Function name  : appendIntegerNodeToXMLTree
// Description     : Create and append a node with a custom name and inline number-text
// 
// XML_TREE*       pTree   : [in/out] Tree containing the node
// XML_TREE_NODE*  pParent : [in/out] Parent node for the new node
// CONST TCHAR*    szName  : [in]     Name of the new node
// CONST INT       iValue  : [in]     Integer value to create inline text for the new node from
// 
// Return Value   : The new node, which has been appended to the tree. You are not responsibe for destroying it, the tree is
// 
XML_TREE_NODE*  appendIntegerNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST INT  iValue)
{
   XML_TREE_NODE*  pNode;     // Node being created

   // Create node from string
   pNode = createXMLTreeNodeFromInteger(pParent, szName, iValue);

   // Attach node to tree and return it
   appendChildToXMLTreeNode(pTree, pParent, pNode);
   return pNode;
}


/// Function name  : appendSourceValueStringNodeToXMLTree
// Description     : Create and append a new <sval> node representing a string
// 
// XML_TREE*       pTree   : [in/out] Tree containing the node
// XML_TREE_NODE*  pParent : [in/out] Parent node for the new node
// CONST TCHAR*    szValue : [in]     String value to represent
// 
// Return Value   : The new node, which has been appended to the tree. You are not responsibe for destroying it, the tree is
// 
XML_TREE_NODE*  appendSourceValueStringNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST TCHAR*  szValue)
{
   XML_TREE_NODE*  pSourceValueNode;      // Node currently being created

   // Create new node
   pSourceValueNode = createSourceValueNodeFromString(pParent, szValue);

   // Attach to tree and return new node
   appendChildToXMLTreeNode(pTree, pParent, pSourceValueNode);
   return pSourceValueNode;
}


/// Function name  : appendSourceValueIntegerNodeToXMLTree
// Description     : Create and append a new <sval> node representing an integer
// 
// XML_TREE*       pTree   : [in/out] Tree containing the node
// XML_TREE_NODE*  pParent : [in/out] Parent node for the new node
// CONST INT       iValue  : [in]     Integer value to represent
// 
// Return Value   : The new node, which has been appended to the tree. You are not responsibe for destroying it, the tree is
// 
XML_TREE_NODE*  appendSourceValueIntegerNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST INT  iValue)
{
   XML_TREE_NODE*  pSourceValueNode;      // Node currently being created

   // Create new node
   pSourceValueNode = createSourceValueNodeFromInteger(pParent, SVT_INTEGER, iValue);

   // Apppend to tree and return new node
   appendChildToXMLTreeNode(pTree, pParent, pSourceValueNode);
   return pSourceValueNode;
}


/// Function name  : appendSourceValueArrayNodeToXMLTree
// Description     : Create and append a new <sval> node representing an array
// 
// XML_TREE*       pTree   : [in/out] Tree containing the node
// XML_TREE_NODE*  pParent : [in/out] Parent node for the new node
// CONST INT       iSize   : [in]     Size of the array to represent
// 
// Return Value   : The new node, which has been appended to the tree. You are not responsibe for destroying it, the tree is
// 
XML_TREE_NODE*  appendSourceValueArrayNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST UINT  iSize)
{
   XML_TREE_NODE*  pSourceValueNode;      // Node currently being created

   // Create new node
   pSourceValueNode = createSourceValueNodeFromInteger(pParent, SVT_ARRAY, iSize);

   // Apppend to tree and return new node
   appendChildToXMLTreeNode(pTree, pParent, pSourceValueNode);
   return pSourceValueNode;
}


/// Function name  : setXMLPropertyInteger
// Description     : Set the value of an existing integer property within an XMLTreeNode
// 
// XML_TREE_NODE*  pNode      : [in/out]  XMLTreeNode to update
// CONST TCHAR*    szProperty : [in]      Name of the property to update
// CONST INT       iValue     : [in]      New Integer value for the property
// 
// Return Value   : TRUE if found, FALSE if property has no associated value or property could not found
// 
BOOL   setXMLPropertyInteger(XML_TREE_NODE*  pNode, CONST TCHAR*  szProperty, CONST INT  iValue)
{
   XML_PROPERTY*  pProperty;     
   BOOL           bResult;

   /// Find property
   if (bResult = findXMLPropertyByName(pNode, szProperty, pProperty))
      // [FOUND] Set value
      StringCchPrintf(pProperty->szValue, 32, TEXT("%d"), iValue);

   // Return TRUE/FALSE
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateTextStreamFromXMLTree
// Description     : Converts an entire XMLTree into a string, storing that string in a TextStream object
// 
// CONST XML_TREE*  pTree   : [in]     Tree to convert to a string
// TEXT_STREAM*     pOutput : [in/out] TextStream object to hold the string
// const bool       bIndent : [in]     Whether to indent XML  (Incompatible with X3:R)
// 
// Return Value   : TRUE if the tree is not empty, otherwise FALSE
// 
BOOL  generateTextStreamFromXMLTree(CONST XML_TREE*  pTree, TEXT_STREAM*  pOutput, const bool bIndent)
{
   XML_TREE_NODE*  pRootChild;      // First child of the root node

   // [CHECK] Abort if tree is empty
   if (getXMLTreeItemCount(pTree) == 0)
      return FALSE;

   // Add program name
   appendStringToTextStreamf(pOutput, TEXT("<!-- Generated using %s -->\r\n"), getAppName());
   
   // Locate the first child of the root node
   if (findXMLTreeNodeByIndex(pTree->pRoot, 0, pRootChild))
      /// Flatten entire tree into a string
      generateTextStreamFromXMLTreeNode(pRootChild, pOutput, bIndent, 0);

   // Return TRUE
   return TRUE;
}


/// Function name  : generateTextStreamFromXMLTreeNode
// Description     : Converts an XMLTree node and all it's children into a string, and stores that in a TextStream object
// 
// CONST XML_TREE_NODE*  pCurrentNode : [in]     Node to convert to a string
// TEXT_STREAM*          pOutput      : [in/out] TextStream object to hold the string
// const bool            bIndent      : [in]     Whether to indent XML  (Incompatible with X3:R)
// CONST UINT            iIndentation : [in]     Number of tabs to indent the current tag by
// 
VOID  generateTextStreamFromXMLTreeNode(CONST XML_TREE_NODE*  pCurrentNode, TEXT_STREAM*  pOutput, const bool bIndent, CONST UINT  iIndentation)
{
   XML_PROPERTY*   pProperty;    // Input node property iterator
   XML_TREE_NODE*  pChildNode;   // Input node child iterator

   // [CHECK] Ensure tag does not have inline text AND children
   ASSERT((getXMLNodeCount(pCurrentNode) AND pCurrentNode->szText) == FALSE);

   // [INDENTATION] Indent tag appropriately
   if (bIndent)
      for (UINT i = 0; i < iIndentation; i++)
         appendCharToTextStream(pOutput, TEXT('\t'));

   // Open current tag
   appendStringToTextStreamf(pOutput, TEXT("<%s"), pCurrentNode->szName);

   /// [PROPERTIES] Add properties (if any) to the current tag
   for (UINT iIndex = 0; findXMLPropertyByIndex(pCurrentNode, iIndex, pProperty); iIndex++)
      // Append name+value pair to the current tag
      appendStringToTextStreamf(pOutput, TEXT(" %s=\"%s\""), pProperty->szName, pProperty->szValue);
   
   /// [INLINE TEXT] Close tag, output text and create a matching closing tag
   if (!getXMLNodeCount(pCurrentNode) AND pCurrentNode->szText)
      appendStringToTextStreamf(pOutput, TEXT(">%s</%s>\r\n"), pCurrentNode->szText, pCurrentNode->szName);
   
   /// [NO CHILDREN] Convert tag to a 'self-closing' tag
   else if (!getXMLNodeCount(pCurrentNode))
      appendStringToTextStreamf(pOutput, TEXT(" />\r\n"), pCurrentNode->szName);

   /// [CHILDREN] Output each child-tag under the current tag
   else
   {
      // Convert current tag an 'opening' tag
      appendStringToTextStreamf(pOutput, TEXT(">\r\n"));

      // Print each child-tag on a separate line
      for (UINT iIndex = 0; findXMLTreeNodeByIndex(pCurrentNode, iIndex, pChildNode); iIndex++)
         generateTextStreamFromXMLTreeNode(pChildNode, pOutput, bIndent, iIndentation + 1);
      
      // Indent close-tag
      if (bIndent)
         for (UINT i = 0; i < iIndentation; i++)
            appendCharToTextStream(pOutput, TEXT('\t'));

      // Close tag 
      appendStringToTextStreamf(pOutput, TEXT("</%s>\r\n"), pCurrentNode->szName);
   }
}

