//
// Language Edit OLE.cpp : OLE interface implementations for the button 'control' objects inside the RichEdit
//
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "olectl.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

RichEditImage::RichEditImage() 
{
   utilZeroObject(&m_oData, STGMEDIUM);
   utilZeroObject(&m_oFormat, FORMATETC);

   m_bDataOwner = TRUE;
   m_iRefCount  = 0;
   m_pEvents    = NULL;
}

RichEditImage::~RichEditImage()
{
   // [CHECK] Destroy data if we're the owner
	if (m_bDataOwner)
		::ReleaseStgMedium(&m_oData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : AddRef
// Description     : Increment the reference count of a RichEdit language message button object
// 
// Return Value   : New reference count
// 
OLE_Method 
ULONG   RichEditImage::AddRef(VOID)
{
	return ++m_iRefCount;
}

/// Function name  : Release
// Description     : Release a RichEdit language message button object
// 
OLE_Method 
ULONG   RichEditImage::Release(VOID)
{
	if (--m_iRefCount == 0)
		delete this;
	
	return m_iRefCount;
}


/// Function name  : QueryInterface
// Description     : Get a pointer to a language message button object
// 
// REFIID       iID        : [in]  ID of the desired interface
// IInterface*  pInterface : [out] Language message button object interface
// 
// Return Value   : S_OK if succesful, E_NOINTERFACE otherwise
// 
OLE_Method 
HRESULT   RichEditImage::QueryInterface(REFIID  iID, IInterface*  pInterface)
{
	if (iID == IID_IUnknown OR iID == IID_IDataObject)
	{
		*pInterface = this;
		AddRef();
		return S_OK;
	}
	else
		return E_NOINTERFACE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         INTERFACE METHODS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : GetData
// Description     : Retrieve a copy of the data in the language message button object
// 
// FORMATETC*  pFormat   : [in]  Requested format for the data
// STGMEDIUM*  pOutput   : [out] Data in the requested format
// 
// Return Value   : S_OK if succesful or E_HANDLE otherwise
// 
OLE_Method 
HRESULT  RichEditImage::GetData(FORMATETC*  pFormat, STGMEDIUM*  pOutput)
{
	HBITMAP  hBitmapCopy;

   // Duplicate bitmap 
	hBitmapCopy = (HBITMAP)::OleDuplicateData(m_oData.hBitmap, CF_BITMAP, NULL);
   // [CHECK] Bitmap handle was valid
	if (hBitmapCopy == NULL)
		return E_HANDLE;
	
   // Set result 
	pOutput->tymed   = TYMED_GDI;
	pOutput->hBitmap = hBitmapCopy;
	pOutput->pUnkForRelease = NULL;

	return S_OK;
}


/// Function name  : SetBitmap
// Description     : Convenience wrapper for storing a bitmap handle 
// 
// HBITMAP hBitmap   : [in] Bitmap handle
// 
VOID    RichEditImage::SetBitmap(HBITMAP  hBitmap)
{
   STGMEDIUM  oData;
   FORMATETC  oFormat;

	ASSERT(hBitmap);

   /// Data
	oData.tymed   = TYMED_GDI;	
	oData.hBitmap = hBitmap;
	oData.pUnkForRelease = NULL;

	/// Data format
	oFormat.cfFormat = CF_BITMAP;				// Clipboard format = CF_BITMAP
	oFormat.ptd      = NULL;					// Target Device = Screen
	oFormat.dwAspect = DVASPECT_CONTENT;	// Level of detail = Full content
	oFormat.lindex   = -1;						// Index = Not applicaple
	oFormat.tymed    = TYMED_GDI;				// Storage medium = HBITMAP handle

   // Save data
	SetData(&oFormat, &oData, TRUE);		
}


/// Function name  : SetData
// Description     : Saves image data
// 
// FORMATETC*  pFormat    : [in] Format of the new data
// STGMEDIUM*  pData      : [in] New data
// BOOL        bDataOwner : [in] Whether the object owns the new data and is responsible for destroying it
// 
// Return Value   : S_OK
// 
OLE_Method 
HRESULT   RichEditImage::SetData(FORMATETC*  pFormat, STGMEDIUM*  pData, BOOL  bDataOwner)
{
   // [CHECK] Destroy existing data, if any
   if (m_bDataOwner AND m_oData.hBitmap)
		::ReleaseStgMedium(&m_oData);

   // Save new data
	m_oFormat    = *pFormat;
	m_oData      = *pData;
   m_bDataOwner = bDataOwner;

	return S_OK;
}

/// Function name  : DAdvise
// Description     : Register for a 'data changed' notification
// 
// FORMATETC*    pFormat          : [in]  The data format to monitor for changes
// DWORD         dwFlags          : [in]  flags
// IAdviseSink*  pOutput          : [out] The OLE object to call when changes occurr
// DWORD*        pdwConnectionID  : [out] Unique ID to identify the event
// 
// Return Value   : S_OK if succesful
// 
OLE_Method
HRESULT  RichEditImage::DAdvise(FORMATETC*  pFormat, DWORD  dwFlags, IAdviseSink*  pOutput, DWORD*  pdwConnectionID)
{
   HRESULT  hResult;    // Operation result

   // Create events object if necessary
   if (!m_pEvents AND FAILED(hResult = CreateDataAdviseHolder(&m_pEvents)))
      return hResult;

   // Register connect using events object
   return m_pEvents->Advise(this, pFormat, dwFlags, pOutput, pdwConnectionID);
}

/// Function name  : DAdvise
// Description     : Un-register a previously registered 'data changed' notification
// 
// DWORD   dwConnectionID  : [in] ID of the connection to un-register
// 
// Return Value   : S_OK if succesful
// 
OLE_Method
HRESULT  RichEditImage::DUnadvise(DWORD dwConnectionID)
{
   // [CHECK] Events object exists
   if (!m_pEvents)
      return E_UNEXPECTED;

   // Un-register event using events object
   return m_pEvents->Unadvise(dwConnectionID);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : Create
// Description     : Creates a simple OLE picture 
// 
// IOleClientSite*  pOleClientSite  : [in] 
// IStorage*        pStorage        : [in] 
// 
// Return Value   : New OLE object
// 
IOleObject*  RichEditImage::Create(IOleClientSite*  pOleClientSite, IStorage*  pStorage)
{
   IOleObject* pOleObject = NULL;
   HRESULT     hResult;

   // [CHECK] Ensure we have a bitmap
	ASSERT(m_oData.hBitmap);

   // Create OleObject
	hResult = ::OleCreateStaticFromData(this, IID_IOleObject, OLERENDER_FORMAT, &m_oFormat, pOleClientSite, pStorage, (IInterface*)&pOleObject);
	ASSERT(hResult == S_OK);

	return pOleObject;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         CALLBACK INTERFACE
// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : AddRef
// Description     : Increment the reference count of a RichEdit language message button callback object
// 
// Return Value   : New reference count
// 
OLE_Method 
ULONG   RichEditCallback::AddRef(VOID)
{
   return ++m_iRefCount;
}


/// Function name  : Release
// Description     : Release a RichEdit language message button callback object
// 
OLE_Method 
ULONG   RichEditCallback::Release(VOID)
{
   if (--m_iRefCount == 0)
      delete this;

   return m_iRefCount;
}

/// Function name  : QueryInterface
// Description     : Get a pointer to a language message button callback object
// 
// REFIID       iid         : [in]  ID of the desired interface
// IInterface*  pInterface  : [out] Language message button object callback interface
// 
// Return Value   : S_OK if succesful, E_NOINTERFACE otherwise
// 
OLE_Method 
HRESULT   RichEditCallback::QueryInterface(REFIID  iid, IInterface*  pInterface)
{
   if (iid == IID_IUnknown OR iid == IID_IRichEditOleCallback)
	{
		*pInterface = this;
		AddRef();
		return S_OK;
	}
	else
		return E_NOINTERFACE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         INTERFACE METHODS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : GetContextMenu
// Description     : Retrieve the context menu for an Language entry's RichEdit button object
// 
// WORD         iSelectionType  : [in]  Whether text and/or objects are selected
// IOleObject*  piObject        : [in]  OLE interface of the current object selection
// CHARRANGE*   pSelection      : [in]  Character indicies of the current text selection
// HMENU*       pOutput         : [out] Handle to a popup menu
// 
// Return Value   : S_OK
// 
OLE_Method 
HRESULT    RichEditCallback::GetContextMenu(WORD  iSelectionType, IOleObject*  piObject, CHARRANGE*  pSelection, HMENU*  pOutput)
{
   HMENU  hParentMenu;

   // BUG
   VERBOSE(BUG "Richedit provided context menu can't be custom drawn");

   // Retrieve appropriate Popup
   hParentMenu = LoadMenu(getResourceInstance(), TEXT("LANGUAGE_MENU"));
   *pOutput = GetSubMenu(hParentMenu, IDM_RICHEDIT_POPUP);

   // Return without destroying menu.
   return S_OK;
}


/// Function name  : DeleteObject
// Description     : 
// 
OLE_Method
HRESULT    RichEditCallback::DeleteObject(IOleObject*  pOleObject)
{
   /// Notify parent
   SendMessage(m_hParent, UN_OBJECT_DESTROYED, NULL, (LPARAM)pOleObject);

   // Return OK
   return S_OK;
}


/// Function name  : DeleteObject
// Description     : 
// 
OLE_Method
HRESULT   RichEditCallback::QueryInsertObject(CLSID* pCLSID, IStorage* pStorage, LONG cp)
{ 
   return S_OK; 
}