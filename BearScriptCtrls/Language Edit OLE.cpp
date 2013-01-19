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

/// Function name  : CLanguageButtonObject
/// Class          : CLanguageButtonObject
// Description     : Create a RichEdit language message button object
// 
CLanguageButtonObject::CLanguageButtonObject()
{
   m_bDataOwner = FALSE;
   m_iReferenceCount = 0;
   m_pEvents = NULL;
	
   utilZeroObject(&m_oData, STGMEDIUM);
   utilZeroObject(&m_oDataFormat, FORMATETC);
}

/// Function name  : ~CLanguageButtonObject
/// Class          : CLanguageButtonObject
// Description     : Deletes a RichEdit language message button object
// 
CLanguageButtonObject::~CLanguageButtonObject()
{
   // [CHECK] Destroy data if we're the owner
	if (m_bDataOwner)
		::ReleaseStgMedium(&m_oData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : AddRef
/// Class          : CLanguageButtonObject
// Description     : Increment the reference count of a RichEdit language message button object
// 
// Return Value   : New reference count
// 
OLE_Method 
ULONG   CLanguageButtonObject::AddRef(VOID)
{
	return ++m_iReferenceCount;
}

/// Function name  : AddRef
/// Class          : CLanguageButtonObjectCallback
// Description     : Increment the reference count of a RichEdit language message button callback object
// 
// Return Value   : New reference count
// 
OLE_Method 
ULONG   CLanguageButtonObjectCallback::AddRef(VOID)
{
   return ++m_iReferenceCount;
}

/// Function name  : Release
/// Class          : CLanguageButtonObject
// Description     : Release a RichEdit language message button object
// 
OLE_Method 
ULONG   CLanguageButtonObject::Release(VOID)
{
	if (--m_iReferenceCount == 0)
		delete this;
	
	return m_iReferenceCount;
}

/// Function name  : Release
/// Class          : CLanguageButtonObjectCallback
// Description     : Release a RichEdit language message button callback object
// 
OLE_Method 
ULONG   CLanguageButtonObjectCallback::Release(VOID)
{
   if (--m_iReferenceCount == 0)
      delete this;

   return m_iReferenceCount;
}


/// Function name  : QueryInterface
/// Class          : CLanguageButtonObject
// Description     : Get a pointer to a language message button object
// 
// REFIID       iID        : [in]  ID of the desired interface
// IInterface*  pInterface : [out] Language message button object interface
// 
// Return Value   : S_OK if succesful, E_NOINTERFACE otherwise
// 
OLE_Method 
HRESULT   CLanguageButtonObject::QueryInterface(REFIID  iID, IInterface*  pInterface)
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


/// Function name  : QueryInterface
/// Class          : CLanguageButtonObjectCallback
// Description     : Get a pointer to a language message button callback object
// 
// REFIID       iid         : [in]  ID of the desired interface
// IInterface*  pInterface  : [out] Language message button object callback interface
// 
// Return Value   : S_OK if succesful, E_NOINTERFACE otherwise
// 
OLE_Method 
HRESULT   CLanguageButtonObjectCallback::QueryInterface(REFIID  iid, IInterface*  pInterface)
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
/// Class          : CLanguageButtonObjectCallback
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
HRESULT    CLanguageButtonObjectCallback::GetContextMenu(WORD  iSelectionType, IOleObject*  piObject, CHARRANGE*  pSelection, HMENU*  pOutput)
{
   HMENU  hParentMenu;

   BUG("Richedit provided context menu can't be custom drawn");

   // Retrieve appropriate Popup
   hParentMenu = LoadMenu(getResourceInstance(), TEXT("LANGUAGE_MENU"));
   *pOutput = GetSubMenu(hParentMenu, IDM_RICHEDIT_POPUP);

   // Return without destroying menu.
   return S_OK;
}


/// Function name  : DeleteObject
/// Class          : CLanguageButtonObjectCallback
// Description     : 
// 
// IOleObject* pOleObject   : [in] 
// 
// Return Value   : 
// 
OLE_Method
HRESULT    CLanguageButtonObjectCallback::DeleteObject(IOleObject*  pOleObject)
{
   // Return OK
   return S_OK;
}


/// Function name  : GetData
/// Class          : CLanguageButtonObject
// Description     : Retrieve a copy of the data in the language message button object
// 
// FORMATETC*  pFormat   : [in]  Requested format for the data
// STGMEDIUM*  pOutput   : [out] Data in the requested format
// 
// Return Value   : S_OK if succesful or E_HANDLE otherwise
// 
OLE_Method 
HRESULT  CLanguageButtonObject::GetData(FORMATETC*  pFormat, STGMEDIUM*  pOutput)
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

/// Function name  : SetData
/// Class          : CLanguageButtonObject
// Description     : Passes new data to the language button object
// 
// FORMATETC*  pFormat    : [in] Format of the new data
// STGMEDIUM*  pData      : [in] New data
// BOOL        bDataOwner : [in] Whether the object owns the new data and is responsible for destroying it
// 
// Return Value   : S_OK
// 
OLE_Method 
HRESULT   CLanguageButtonObject::SetData(FORMATETC*  pFormat, STGMEDIUM*  pData, BOOL  bDataOwner)
{
   // [CHECK] Destroy data if we're the owner
   if (m_bDataOwner AND m_oData.hBitmap)
		::ReleaseStgMedium(&m_oData);

   // Save input data
	m_oDataFormat = *pFormat;
	m_oData       = *pData;
   m_bDataOwner  = bDataOwner;

	return S_OK;
}

/// Function name  : DAdvise
/// Class          : CLanguageButtonObject
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
HRESULT  CLanguageButtonObject::DAdvise(FORMATETC*  pFormat, DWORD  dwFlags, IAdviseSink*  pOutput, DWORD*  pdwConnectionID)
{
   HRESULT  hResult;    // Operation result

   // Create events object if necessary
   if (m_pEvents == NULL)
   {
      hResult = CreateDataAdviseHolder(&m_pEvents);
      if (FAILED(hResult))
         return hResult;
   }

   // Register connect using events object
   return m_pEvents->Advise(this, pFormat, dwFlags, pOutput, pdwConnectionID);
}

/// Function name  : DAdvise
/// Class          : CLanguageButtonObject
// Description     : Un-register a previously registered 'data changed' notification
// 
// DWORD   dwConnectionID  : [in] ID of the connection to un-register
// 
// Return Value   : S_OK if succesful
// 
OLE_Method
HRESULT  CLanguageButtonObject::DUnadvise(DWORD dwConnectionID)
{
   // [CHECK] Events object exists
   if (m_pEvents == NULL)
      return E_UNEXPECTED;

   // Un-register event using events object
   return m_pEvents->Unadvise(dwConnectionID);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL         CLanguageButtonObject::checkValid()
{
   return m_pEvents->SendOnDataChange(this, NULL, NULL);
}

/// Function name  : GetOleObject
/// Class          : CLanguageButtonObject
// Description     : Creates a static representation of our data.  I'm not sure what that means yet.
// 
// IOleClientSite*  pOleClientSite  : [in] 
// IStorage*        pStorage        : [in] 
// 
// Return Value   : 
// 
IOleObject*  CLanguageButtonObject::GetOleObject(IOleClientSite*  pOleClientSite, IStorage*  pStorage)
{
   IOleObject* pOleObject;
   //IUnknown*   pUnknown;
   HRESULT     hResult;

	ASSERT(m_oData.hBitmap);

   /*oProperties.cbSizeofstruct = sizeof(PICTDESC);
   oProperties.bmp.hbitmap = m_oData.hBitmap;
   oProperties.picType = PICTYPE_BITMAP;*/
	
	hResult = ::OleCreateStaticFromData(this, IID_IOleObject, OLERENDER_FORMAT, &m_oDataFormat, pOleClientSite, pStorage, (IInterface*)&pOleObject);

   //hResult = ::OleCreate(this, IID_IOleObject, OLERENDER_FORMAT, &m_oDataFormat, pOleClientSite, pStorage, (IInterface*)&pOleObject);
   //hResult = ::OleCreatePictureIndirect(&oProperties, IID_IPicture, TRUE, (IInterface*)&pOleObject);

   //hResult = ::OleCreateFromData(this, IID_IOleObject, OLERENDER_FORMAT, &m_oDataFormat, pOleClientSite, pStorage, (IInterface*)&pOleObject);
   //hResult = ::OleCreateFromData(this, IID_IUnknown, OLERENDER_DRAW, &m_oDataFormat, pOleClientSite, pStorage, (IInterface*)&pOleObject);

	if (hResult != S_OK)
   {
      ERROR_CHECK("creating OLE picture", hResult);
		return NULL; //AfxThrowOleException(sc);
   }

   /*pUnknown = pOleObject;
   pUnknown->QueryInterface(IID_IOleObject, (IInterface*)&pOleObject);
   pUnknown->Release();*/
	return pOleObject;
}


/// Function name  : InsertBitmap
/// Class          : CLanguageButtonObject
// Description     : Create a new OLE button object in a language message RichEdit control from a bitmap handle.
// 
// IRichEditOle*           pRichEditOLE      : [in] RichEdit OLE interface
// HBITMAP                 hBitmap           : [in] Bitmap handle
// CONST LANGUAGE_BUTTON*  pButtonProperties : [in] Text and ID of the button
//
VOID   CLanguageButtonObject::InsertBitmap(IRichEditOle*  pRichEditOLE, HBITMAP  hBitmap, CONST LANGUAGE_BUTTON*  pButtonProperties)
{
   CLanguageButtonObject*  pOLEButtonObject = NULL;      // Resultant button object
   IOleClientSite*         pOLEClientSite = NULL;
   IOleObject*             pOLEObject = NULL; 
   IDataObject*            pDataObject = NULL;
   ILockBytes*             pLockBytes = NULL;
   IStorage*               pStorage = NULL;
   REOBJECT                oRichEditObject;
   CLSID                   iCLSID;
	SCODE                   iResult;

   // Prepare
   ZeroMemory(&oRichEditObject, sizeof(REOBJECT));
   oRichEditObject.cbStruct = sizeof(REOBJECT);

	// Create the button OLE object, duplicate and cleanup the bitmap
	pOLEButtonObject = new CLanguageButtonObject();
	pOLEButtonObject->QueryInterface(IID_IDataObject, (IInterface*)&pDataObject);
	pOLEButtonObject->SetBitmapHandle(hBitmap);

	// Get the RichEdit container site
	pRichEditOLE->GetClientSite(&pOLEClientSite);

	// Initialize a Storage Object
	iResult = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
	if (iResult == S_OK AND pLockBytes)
   {
	   iResult = ::StgCreateDocfileOnILockBytes(pLockBytes, STGM_SHARE_EXCLUSIVE WITH STGM_CREATE WITH STGM_READWRITE, 0, &pStorage);
	   if (iResult == S_OK AND pStorage)
	   {
	      // Create an OLE Object in the GDI format specified by the button object
	      if (pOLEObject = pOLEButtonObject->GetOleObject(pOLEClientSite, pStorage))
         {
	         // Notify object it's embedded
	         OleSetContainedObject(pOLEObject, TRUE);

	         // Now Add the object to the RichEdit 
	         iResult = pOLEObject->GetUserClassID(&iCLSID);
	         if (iResult == S_OK)
            {
               // Define object
	            oRichEditObject.clsid    = iCLSID;
	            oRichEditObject.cp       = REO_CP_SELECTION;
	            oRichEditObject.dvaspect = DVASPECT_CONTENT;
	            oRichEditObject.poleobj  = pOLEObject;
	            oRichEditObject.polesite = pOLEClientSite;
	            oRichEditObject.pstg     = pStorage;
               // Associate button data
               oRichEditObject.dwUser   = (DWORD)pButtonProperties;
	            // Insert the object at the current location in the richedit control
	            pRichEditOLE->InsertObject(&oRichEditObject);
            }
         }
      }
      // Cleanup
      utilReleaseInterface(pLockBytes);
   }

	// Cleanup
	utilReleaseInterface(pOLEObject);
	utilReleaseInterface(pOLEClientSite);
	utilReleaseInterface(pDataObject);
   utilReleaseInterface(pStorage);
}

/// Function name  : SetBitmapHandle
/// Class          : CLanguageButtonObject
// Description     : Conveience function for wrapping a bitmap handle in the appropriate OLE structures
//                    and saving in the language message button object
// 
// HBITMAP hBitmap   : [in] Bitmap handle
// 
VOID    CLanguageButtonObject::SetBitmapHandle(HBITMAP  hBitmap)
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

