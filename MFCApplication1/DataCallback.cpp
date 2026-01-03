#include "pch.h"
#include "DataCallback.h"
#include "ToStringConverter.h"

STDMETHODIMP CDataCallback::QueryInterface(REFIID iid, LPVOID* ppInterface)
{
	if (NULL == ppInterface)
	{
		return E_INVALIDARG;
	}
	if (IID_IUnknown == iid)
	{
		*ppInterface = dynamic_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}
	if (__uuidof(IOPCDataCallback) == iid)
	{
		*ppInterface = dynamic_cast<IOPCDataCallback*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

//--------------------
STDMETHODIMP_(ULONG) CDataCallback::AddRef()
{
	return InterlockedIncrement((LONG*)&m_ulRefs);
}

//------------
STDMETHODIMP_(ULONG) CDataCallback::Release()
{
	ULONG ulRefs = InterlockedDecrement((LONG*)&m_ulRefs);
	if (ulRefs == 0)
	{
		delete this;
		return 0;
	}
	return ulRefs;
}

STDMETHODIMP CDataCallback::OnDataChange(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup,
	/*[in]*/ HRESULT hrMasterquality,
	/*[in]*/ HRESULT hrMastererror,
	/*[in]*/ unsigned long dwCount,
	/*[in]*/ unsigned long* phClientItems,
	/*[in]*/ VARIANT* pvValues,
	/*[in]*/ unsigned short* pwQualities,
	/*[in]*/ struct _FILETIME* pftTimeStamps,
	/*[in]*/ HRESULT* pErrors)
{
	LVITEM lvItem;
	ZeroMemory(&lvItem, sizeof(lvItem));
	lvItem.cchTextMax = 255;
	lvItem.pszText = m_szItemID.GetBuffer();
	lvItem.mask = LVIF_TEXT;
	m_pValueView->DeleteAllItems();
	int i = m_pValueView->InsertItem(&lvItem);
	m_pValueView->SetItem(0, 1, LVIF_TEXT, CToStringConverter::GetVTypeString(pvValues[0].vt), 0, 0, 0, 0, 0);
	m_pValueView->SetItem(0, 3, LVIF_TEXT, CToStringConverter::GetFTString(pftTimeStamps[0]), 0, 0, 0, 0, 0);
	m_pValueView->SetItem(0, 2, LVIF_TEXT, CToStringConverter::GetValueString(pvValues[0]), 0, 0, 0, 0, 0);
	m_pValueView->SetItem(0, 4, LVIF_TEXT, CToStringConverter::GetQualityString(pwQualities[0]), 0, 0, 0, 0, 0);
	return S_OK;
}

STDMETHODIMP CDataCallback::OnReadComplete(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup,
	/*[in]*/ HRESULT hrMasterquality,
	/*[in]*/ HRESULT hrMastererror,
	/*[in]*/ unsigned long dwCount,
	/*[in]*/ unsigned long* phClientItems,
	/*[in]*/ VARIANT* pvValues,
	/*[in]*/ unsigned short* pwQualities,
	/*[in]*/ struct _FILETIME* pftTimeStamps,
	/*[in]*/ HRESULT* pErrors) {
	return S_OK;
}
STDMETHODIMP CDataCallback::OnWriteComplete(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup,
	/*[in]*/ HRESULT hrMastererr,
	/*[in]*/ unsigned long dwCount,
	/*[in]*/ unsigned long* pClienthandles,
	/*[in]*/ HRESULT* pErrors) {
	return S_OK;
}
STDMETHODIMP CDataCallback::OnCancelComplete(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup) {
	return S_OK;
}