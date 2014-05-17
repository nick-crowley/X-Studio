// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER NTDDI_WINXP              // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT                    // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT NTDDI_WINXP        // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS                  // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS NTDDI_WINXP      // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0501                // Change this to the appropriate value to target other versions of IE.
#endif


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlwapi.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>

#pragma warning(disable : 4995)

// C++ STL
#include <exception>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

#include "Lib/BaseWindow.h"
#include "Lib/MessagePump.h"
#include "Lib/ListView.h"


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

/// Macro: STRING
// Description: Generate a wstring from an ANSI literal
#define      STRING(abc)              wstring(TEXT(abc))
