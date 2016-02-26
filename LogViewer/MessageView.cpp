//
// MessageView.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "LogViewer.h"


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

#define  IDC_THREAD_LIST      100
#define  IDC_MESSAGE_LIST     200



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTRUCTION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

MessageView::MessageView()
{
   // Use CustomDraw 
   //SetCustomDraw(new MessageViewDrawImpl);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       METHODS
/// /////////////////////////////////////////////////////////////////////////////////////////


long  MessageViewDrawImpl::onDrawCycle(const UINT  iControlID, LVCustomDraw  oDrawData)
{ 
   return CustomDraw::DoDefault;  
};

long  MessageViewDrawImpl::onDrawItem(const UINT  iControlID, LVCustomDraw  oDrawData)     
{ 
   if (oDrawData.Param)
      oDrawData.TextColour = RGB(255,0,0);

   return CustomDraw::DoDefault | CustomDraw::FontChanged;
};





   