
// MFCApplication1Dlg.h : header file
//

#pragma once


// CMFCApplication1Dlg dialog
class CMFCApplication1Dlg : public CDialogEx
{
// Construction
public:
	CMFCApplication1Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listOPCServers;
	CTreeCtrl m_treeOPCServerBrowse;
private:
	int ShowRegisteredServers();
public:
	afx_msg void OnDestroy();
private:
	void DisplayChildren(HTREEITEM hParent, IOPCBrowseServerAddressSpace* pParent);
	IOPCServer *m_pOPCServer;
	void OnServerChange();
	int ConnectAndBrowseServer(const GUID* pGuid);
public:
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl m_valueView;
private:
	void FormatValueView();
	DWORD m_hGroup;
	LPWSTR GetCurrentItemID();
public:
	afx_msg void OnBnClickedButton1();
};
