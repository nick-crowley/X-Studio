#pragma once

/// Macro: READONLY_PROPERTY
// Description: Creates a C++ property declaration and associated 'get' function
//
#define READONLY_PROPERTY(xType,szName,szData)  xType  get##szName() const   { return szData;  };  \
                                                 __declspec(property(get=get##szName)) xType  szName;

/// Macro: READWRITE_PROPERTY
// Description: Creates a C++ property declaration with associated 'get' and 'set' functions
//
#define READWRITE_PROPERTY(xType,szName,szData)  xType  get##szName() const       { return szData;  };  \
                                                  void   set##szName(xType  xVal)  { szData = xVal;  };  \
                                                  __declspec(property(get=get##szName, put=set##szName)) xType  szName;

/// Macro: PROTECTED_READWRITE_PROPERTY
// Description: Declares a property that can be changed internally but not externally
//
#define PROTECTED_READWRITE_PROPERTY(xType,szName,szData)  public:    xType  get##szName() const       { return szData;  };  \
                                                            protected: void   set##szName(xType  xVal)  { szData = xVal;  };  \
                                                            __declspec(property(get=get##szName, put=set##szName)) xType  szName;

