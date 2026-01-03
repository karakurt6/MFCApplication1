#pragma once

class CDataCallback: public IOPCDataCallback
{
public:
	CDataCallback(CListCtrl* pValueView, CString szItemID)
	{
		m_pValueView = pValueView;
		m_szItemID = szItemID;
		m_ulRefs = 1;
	}
	void SetItemID(CString szItemID)
	{
		m_szItemID = szItemID;
	}
	//==========================================================================
	// IUnknown
	// QueryInterface
	virtual STDMETHODIMP QueryInterface(REFIID iid, LPVOID *ppInterface);
	virtual STDMETHODIMP_(ULONG) AddRef();
	// Release
	virtual STDMETHODIMP_(ULONG) Release();
	//==========================================================================
	// IOPCDataCallback
	virtual STDMETHODIMP OnDataChange(
		/*[in]*/ unsigned long dwTransid,
		/*[in]*/ unsigned long hGroup,
		/*[in]*/ HRESULT hrMasterquality,
		/*[in]*/ HRESULT hrMastererror,
		/*[in]*/ unsigned long dwCount,
		/*[in]*/ unsigned long* phClientItems,
		/*[in]*/ VARIANT* pvValues,
		/*[in]*/ unsigned short* pwQualities,
		/*[in]*/ struct _FILETIME* pftTimeStamps,
		/*[in]*/ HRESULT* pErrors);
	virtual HRESULT __stdcall OnReadComplete(
		/*[in]*/ unsigned long dwTransid,
		/*[in]*/ unsigned long hGroup,
		/*[in]*/ HRESULT hrMasterquality,
		/*[in]*/ HRESULT hrMastererror,
		/*[in]*/ unsigned long dwCount,
		/*[in]*/ unsigned long* phClientItems,
		/*[in]*/ VARIANT* pvValues,
		/*[in]*/ unsigned short* pwQualities,
		/*[in]*/ struct _FILETIME* pftTimeStamps,
		/*[in]*/ HRESULT* pErrors);
	virtual STDMETHODIMP OnWriteComplete(
		/*[in]*/ unsigned long dwTransid,
		/*[in]*/ unsigned long hGroup,
		/*[in]*/ HRESULT hrMastererr,
		/*[in]*/ unsigned long dwCount,
		/*[in]*/ unsigned long* pClienthandles,
		/*[in]*/ HRESULT* pErrors);
	virtual STDMETHODIMP OnCancelComplete(
		/*[in]*/ unsigned long dwTransid,
		/*[in]*/ unsigned long hGroup);
private:
	CListCtrl* m_pValueView;
	ULONG m_ulRefs;
	CString m_szItemID;
};

