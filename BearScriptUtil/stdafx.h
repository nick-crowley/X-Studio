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

/// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shlobj.h>

// Memory Leak Detection
#define _CRTDBG_MAP_ALLOC

// C RunTime Header Files
#include <stdlib.h>
#include <crtdbg.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <math.h>
#include <tchar.h>

// Windows Secure String Library (Special):
#include <strsafe.h>

// Application header
#include "BearScript Util.h"

// Visual Leak Detector Library
//#include "vld.h"


