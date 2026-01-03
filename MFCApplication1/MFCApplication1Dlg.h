
// MFCApplication1Dlg.h : header file
//

#pragma once

class CDataCallback;

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

// Implementation
protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	DECLARE_MESSAGE_MAP()

private:
	CListCtrl m_listOPCServers;
	CTreeCtrl m_treeOPCServerBrowse;
	IOPCServer *m_pOPCServer;
	CListCtrl m_valueView;
	DWORD m_hGroup;
	IConnectionPoint* m_pDataCallback; // Точка подключения к серверу
	DWORD m_dwCookie;                  // идентификатор подписки на события сервера
	CDataCallback* m_pSink;            // Интерфейс, через который будут проходить обратные вызовы

	int ShowRegisteredServers();
	void DisplayChildren(HTREEITEM hParent, IOPCBrowseServerAddressSpace* pParent);
	void OnServerChange();
	int ConnectAndBrowseServer(const GUID* pGuid);
	void FormatValueView();
	LPWSTR GetCurrentItemID();
};
