//
// stdafx.h  --  Controls library pre-compiled header
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 
#define      _CRT_SECURE_NO_WARNINGS   

#pragma once
#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4996 )

/// Specify a Windows XP compatible build environment
#ifndef WINVER 
#define WINVER         NTDDI_WINXP
#endif

#ifndef _WIN32_WINNT               
#define _WIN32_WINNT   NTDDI_WINXP 
#endif

#ifndef _WIN32_WINDOWS             
#define _WIN32_WINDOWS NTDDI_WINXP 
#endif

#ifndef _WIN32_IE                  
#define _WIN32_IE      NTDDI_WINXP
#endif

// Exclude rarely-used stuff from Windows headers
#define      WIN32_LEAN_AND_MEAN             

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <Vssym32.h>
#include <richedit.h>
#include <Richole.h>
#include <Wininet.h>

// Memory Leak Detection
#define _CRTDBG_MAP_ALLOC

// C RunTime Header Files
#include <stdlib.h>
#include <crtdbg.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <tchar.h>

// Windows Secure String Library (Special):
#include <strsafe.h>

// BearScript library
#include "../BearScriptLib/BearScript Lib.h"

// Application header
#include "BearScript Controls.h"

// Visual Leak Detector Library
#include "vld.h"



