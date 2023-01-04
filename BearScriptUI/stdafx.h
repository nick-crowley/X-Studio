//
// Pre-Compiled Header
//
#define      _CRT_SECURE_NO_WARNINGS   

#pragma once
#pragma warning ( disable : 4995 )
#pragma warning ( disable : 4996 )

/// Target Windows XP
#include "../TargetVer.h"

// Exclude rarely-used stuff from Windows headers
#define      WIN32_LEAN_AND_MEAN             

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <richedit.h>
#include <shlwapi.h>
#include <Commctrl.h>
#include <Commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Richole.h>
#include <uxtheme.h>
#include <Vssym32.h>
#include <vsstyle.h>
#include <htmlhelp.h>
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
// DirectShow
#include <Dshow.h>

// BearScript main library
#include "../BearScriptLib/BearScript Lib.h"

// BearScript control library
#include "../BearScriptCtrls/BearScript Controls.h"

// Application headers
#include "Resource.h"
#include "Types.h"
#include "BearScript UI.h"

// Visual Leak Detector Library
//#include "vld.h"



