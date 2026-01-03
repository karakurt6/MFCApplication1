
// MFCApplication1Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "ToStringConverter.h"
#include "DataCallback.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCApplication1Dlg dialog



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwCookie = 0;
	m_pSink = NULL;
	m_hGroup = 0;
	m_pOPCServer = NULL;
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listOPCServers);
	DDX_Control(pDX, IDC_TREE1, m_treeOPCServerBrowse);
	DDX_Control(pDX, IDC_LIST2, m_valueView);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
ON_WM_DESTROY()
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CMFCApplication1Dlg::OnLvnItemchangedList1)
ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMFCApplication1Dlg message handlers

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	HRESULT hr = CoInitialize(NULL);
	if (0 == ShowRegisteredServers())
	{
		MessageBox(L"Нет установленных серверов", L"Список серверов", MB_OK);
		return FALSE;
	}
	FormatValueView();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CMFCApplication1Dlg::ShowRegisteredServers()
{
	CLSID clsid; // индентификатор ОРСEnum
	CLSID clsidcat; //идентифификатор категории ОРС DA серверов
	HRESULT hRes;
	// Идентификатор категории ОРС DA 2.0
	hRes = CLSIDFromString(L"{63D5F432-CFE4-11D1-B2C8-0060083BA1FB}",&clsidcat);
	// Идентификатор компонента просмотра списка серверов
	hRes = CLSIDFromProgID(L"OPC.ServerList", &clsid);
	//Идентификатор интерфейса IOPCServerList
	IID IID_IOPCServerList = __uuidof(IOPCServerList);
	IOPCServerList* pServerList;
	// запрос интерфейса у компонента должен вернуть S_OK
	hRes = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER,
		IID_IOPCServerList, (void**)&pServerList);
	//перечислитель, в котором будут храниться GUID серверов
	IOPCEnumGUID* pIOPCEnumGuid;
	//запрос серверов спецификации OPC DA 2.0
	pServerList->EnumClassesOfCategories(1, &clsidcat, 0, NULL, (IEnumGUID**) & pIOPCEnumGuid);
	OLECHAR* pszProgID; // буфер для записи ProgID серверов
	OLECHAR* pszUserType; // буфер для записи описания серверов
	LVITEM lvItem; // подготовка элемента списка для вставки

	ZeroMemory(&lvItem, sizeof(lvItem));
	lvItem.cchTextMax = 100;
	lvItem.mask = LVIF_TEXT;

	GUID guid; // Сюда будет записывать идентификатор текущего сервера
	int nServerCnt = 0; // общее количество доступных серверов
	unsigned long iRetSvr; // количество серверов, предоставленных запросом
	// получение первого доступного идентификатора сервера
	pIOPCEnumGuid->Next(1, &guid, &iRetSvr);
	while (iRetSvr != 0)
	{
		nServerCnt++;
		pServerList->GetClassDetails(guid, &pszProgID, &pszUserType);
		lvItem.pszText = pszProgID;
		int iItem = m_listOPCServers.InsertItem(&lvItem);
		GUID* pGuid = new GUID;
		//создаем область памяти, чтобы хранить идентификатор в привязке к строке списка
		memcpy(pGuid, &guid, sizeof(guid));

		TCHAR buf[64];
		_stprintf_s(buf, 64, _T("GUID allocated address is %p\n"), pGuid);
		OutputDebugString(buf);

		//связываем элемент списка и указатель на идентификатор
		m_listOPCServers.SetItemData(iItem, (DWORD_PTR)pGuid);
		pIOPCEnumGuid->Next(1, &guid, &iRetSvr); // получаем следующий сервер
	}
	return nServerCnt;
}

void CMFCApplication1Dlg::OnDestroy()
{
	for (int i = 0; i < m_listOPCServers.GetItemCount(); ++i)
	{
		GUID* pGuid = (GUID*)m_listOPCServers.GetItemData(i);
	
		TCHAR buf[64];
		_stprintf_s(buf, 64, _T("GUID destroyed address is %p\n"), pGuid);
		OutputDebugString(buf);
	
		delete pGuid;
		m_listOPCServers.SetItemData(i, 0);
	}

	CDialogEx::OnDestroy();

	OnServerChange();
	m_pOPCServer->Release();
	delete m_pSink;
}


void CMFCApplication1Dlg::DisplayChildren(HTREEITEM hParent, IOPCBrowseServerAddressSpace* pParent)
{
	IEnumString* pEnum;
	HTREEITEM hItem;
	wchar_t* strName;
	unsigned long cnt;
	TVINSERTSTRUCT tvInsert;
	ZeroMemory(&tvInsert, sizeof(tvInsert));
	OPCNAMESPACETYPE ns;
	pParent->QueryOrganization(&ns);
	if (ns == OPC_NS_FLAT)
	{
		AfxOutputDebugString(L"Underlying system is inherently flat\n");
		pParent->BrowseOPCItemIDs(OPC_FLAT, L"", VT_EMPTY, 0, &pEnum);
		tvInsert.item.cchTextMax = 100;
		tvInsert.item.mask = TVIF_TEXT;
		tvInsert.hParent = hParent;
		pEnum->Next(1, &strName, &cnt);
		LPWSTR lpItemID;
		while (cnt != 0)
		{
			tvInsert.item.pszText = strName;
			hItem = m_treeOPCServerBrowse.InsertItem(&tvInsert);
			pParent->GetItemID(strName, &lpItemID);//получает полный идентификатор тега
			m_treeOPCServerBrowse.SetItemData(hItem, (DWORD_PTR)lpItemID);
			pEnum->Next(1, &strName, &cnt);
		}
	}
	else
	{
		AfxOutputDebugString(L"Underlying system is inherently hierarchical\n");
		pParent->BrowseOPCItemIDs(OPC_LEAF, L"", VT_EMPTY, 0, &pEnum);
		tvInsert.item.cchTextMax = 100;
		tvInsert.item.mask = TVIF_TEXT;
		tvInsert.hParent = hParent;
		pEnum->Next(1, &strName, &cnt);
		LPWSTR lpItemID;
		while (cnt != 0)
		{
			tvInsert.item.pszText = strName;
			hItem = m_treeOPCServerBrowse.InsertItem(&tvInsert);
			pParent->GetItemID(strName, &lpItemID);//получает полный идентификатор тега
			m_treeOPCServerBrowse.SetItemData(hItem, (DWORD_PTR)lpItemID);
			pEnum->Next(1, &strName, &cnt);
		}
		pParent->BrowseOPCItemIDs(OPC_BRANCH, L"", VT_EMPTY, 0, &pEnum);
		tvInsert.hParent = hParent;
		pEnum->Next(1, &strName, &cnt);
		HRESULT hRes;
		tvInsert.item.iImage = 0;
		tvInsert.item.iSelectedImage = 0;
		while (cnt != 0)
		{
			tvInsert.item.pszText = strName;
			hItem = this->m_treeOPCServerBrowse.InsertItem(&tvInsert);
			hRes = pParent->ChangeBrowsePosition(OPC_BROWSE_DOWN, strName);
			if (S_OK == hRes)
				DisplayChildren(hItem, pParent);
			pParent->ChangeBrowsePosition(OPC_BROWSE_UP, strName);
			pEnum->Next(1, &strName, &cnt);

		}
	}
}


void CMFCApplication1Dlg::OnServerChange()
{
	if (m_dwCookie != 0)
	{
		m_pDataCallback->Unadvise(m_dwCookie);
		m_dwCookie = 0;
	}
	if (m_hGroup != 0)
	{
		m_pOPCServer->RemoveGroup(m_hGroup, 1);
		m_hGroup = 0;
	}
}

int CMFCApplication1Dlg::ConnectAndBrowseServer(const GUID* pGuid)
{
	//Если уже был подключен к серверу
	if (m_pOPCServer != NULL)
	{
		OnServerChange();
		m_pOPCServer->Release();
	}
	IID IID_IOPCSERVER = __uuidof(IOPCServer);
	//пробуем подключиться как к локальному dll серверу
	HRESULT hRes = CoCreateInstance(*pGuid, NULL, CLSCTX_INPROC_SERVER, IID_IOPCSERVER, (void**)&m_pOPCServer);
	if (hRes != S_OK)
	{
		//пробуем подключиться как к локальному exe серверу
		hRes = CoCreateInstance(*pGuid, NULL, CLSCTX_LOCAL_SERVER, IID_IOPCSERVER, (void**)&m_pOPCServer);
		if (hRes != S_OK)
		{
			MessageBox(L"Не удалось подключиться к серверу", L"Ошибка подключения",MB_OK);
			return -1;
		}
	}
	//Подключение установлено
	IID IID_IOPCBrowseServerAddressSpace = __uuidof(IOPCBrowseServerAddressSpace);
	IOPCBrowseServerAddressSpace* pBrowse;
	hRes = m_pOPCServer->QueryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&pBrowse);
	if (hRes != S_OK)
	{
		MessageBox(L"Не удалось получить IID_IOPCBrowseServerAddressSpace", L"Ошибка просмотра", MB_OK);
		return -1;
	}
	// отображаем содержимое сервера, начиная с корневого узла
	DisplayChildren(TVI_ROOT, pBrowse);
	pBrowse->Release();
	return 0;
}

void CMFCApplication1Dlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	BOOL bSelectedNow = (pNMLV->uNewState & LVIS_SELECTED);
	BOOL bSelectedBefore = (pNMLV->uOldState & LVIS_SELECTED);
	if (bSelectedNow && !bSelectedBefore)
	{
		int nItem = pNMLV->iItem;
		m_treeOPCServerBrowse.DeleteAllItems();
		GUID* guid = (GUID*)m_listOPCServers.GetItemData(nItem);
		ConnectAndBrowseServer(guid);
	}
	*pResult = 0;
}


void CMFCApplication1Dlg::FormatValueView()
{
	m_valueView.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	LVCOLUMNW lvColumn;
	lvColumn.cchTextMax = 25;
	lvColumn.cx = 150;
	lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	lvColumn.pszText = L"Элемент";
	m_valueView.InsertColumn(0, &lvColumn);
	lvColumn.cx = 100;
	lvColumn.pszText = L"Тип";
	m_valueView.InsertColumn(1, &lvColumn);
	lvColumn.cx = 100;
	lvColumn.pszText = L"Значение";
	m_valueView.InsertColumn(2, &lvColumn);
	lvColumn.cx = 150;
	lvColumn.pszText = L"Метка времени";
	m_valueView.InsertColumn(3, &lvColumn);
	lvColumn.cx = 100;
	lvColumn.pszText = L"Качество";
	m_valueView.InsertColumn(4, &lvColumn);
}


LPWSTR CMFCApplication1Dlg::GetCurrentItemID()
{
	HTREEITEM hItem = m_treeOPCServerBrowse.GetSelectedItem();
	if (NULL == hItem)
		return NULL;
	LPWSTR szItemID = (LPWSTR) m_treeOPCServerBrowse.GetItemData(hItem);
	return szItemID;
}


void CMFCApplication1Dlg::OnBnClickedButton1()
{

	HRESULT hRes;
	
	//Этот участок на этапе синхронного чтения должен быть закомментирован. Его
	//необходимо будет использовать тогда, когда добавим асинхронную
	//операцию чтения по подписке
	if (m_dwCookie!=0)
	{
		hRes = ((IConnectionPoint*)m_pDataCallback) ->Unadvise(m_dwCookie);
		m_pDataCallback->Release();
		m_dwCookie = 0;
	}
	

	m_valueView.DeleteAllItems();
	LPWSTR szItemID = GetCurrentItemID();
	if (NULL == szItemID) return;

	IOPCItemIO* pOPCItemIO;
	HRESULT hr = m_pOPCServer->QueryInterface(IID_IOPCItemIO, reinterpret_cast<void**>(&pOPCItemIO));
	if (SUCCEEDED(hr))
	{
		AfxOutputDebugString(L"QueryInterface(pOPCItemIO) S_OK");
	}
	else
	{
		DWORD updateRate = 1000;
		IOPCItemMgt* pItemMgt = NULL;
		//hr = m_pOPCServer->QueryInterface(IID_IOPCItemMgt, reinterpret_cast<void**>(&pItemMgt));
		//if (SUCCEEDED(hr))
		//{
		//	AfxOutputDebugString(L"QueryInterface(pItemMgt) S_OK");
		//	pItemMgt->Release();
		//}
		long bActive = 1;
		// if (m_hGroup != 0)
		{
			hRes = m_pOPCServer->RemoveGroup(m_hGroup, 1);
			m_hGroup = 0;
		}
		OPCHANDLE hServerGroup = NULL;
		hRes = m_pOPCServer->AddGroup(OLESTR("MyGroup"), bActive,
			updateRate,
			0, NULL, NULL, 0,
			&hServerGroup, &updateRate, IID_IOPCItemMgt,
			(IUnknown**)&pItemMgt);

		if (FAILED(hRes))
		{
			LPWSTR lpError;
			m_pOPCServer->GetErrorString(hRes, LOCALE_SYSTEM_DEFAULT, &lpError);
			MessageBox(lpError, L"Ошибка", MB_OK);
			return;
		}
		else
		{
			IOPCGroupStateMgt* pOPCGroupStateMgt = NULL;
			hr = pItemMgt->QueryInterface(IID_IOPCGroupStateMgt, reinterpret_cast<void**>(&pOPCGroupStateMgt));
			if (SUCCEEDED(hr))
			{
				AfxOutputDebugString(L"QueryInterface(pGroupStateMgt) S_OK");
				DWORD dwUpdateRate;
				BOOL bActive;
				LPWSTR pName;
				LONG lTimeBias;
				FLOAT fPercentDeadband;
				DWORD localeID;
				OPCHANDLE hClientGroup_State;
				OPCHANDLE hServerGroup_State;
				hr = pOPCGroupStateMgt->GetState(&dwUpdateRate, &bActive, &pName, &lTimeBias, &fPercentDeadband, &localeID, &hClientGroup_State, &hServerGroup_State);
				if (SUCCEEDED(hr))
				{
					hServerGroup = hServerGroup_State;
				}
			}
			m_hGroup = hServerGroup;
		}

		//Добавляем элементы в группу
		DWORD dwCount = 1;
		tagOPCITEMDEF* pItems = (tagOPCITEMDEF*)CoTaskMemAlloc(dwCount * sizeof(tagOPCITEMDEF));
		tagOPCITEMRESULT* pResults = NULL;
		HRESULT* pErrors = NULL;

		pItems[0].szItemID = szItemID;
		pItems[0].szAccessPath = NULL;
		pItems[0].bActive = TRUE;
		pItems[0].hClient = 0;
		pItems[0].vtRequestedDataType = VT_EMPTY;
		pItems[0].dwBlobSize = 0;
		pItems[0].pBlob = NULL;

		hRes = pItemMgt->AddItems(1, pItems, &pResults, &pErrors);
		if (FAILED(hRes))
		{
			LPWSTR lpMsg = NULL;
			m_pOPCServer->GetErrorString(hRes, 2, &lpMsg);
			MessageBox(lpMsg, L"Ошибка", MB_OK);
		}
		else
		{
			IOPCSyncIO* pSyncIO = NULL;
			IID IID_IOPCSYNCIO = __uuidof(IOPCSyncIO);
			hRes = pItemMgt->QueryInterface(IID_IOPCSYNCIO, (void**)&pSyncIO);
			tagOPCITEMSTATE* pItemValue = NULL;

			//Считываем элементы
			hRes = pSyncIO->Read(OPC_DS_CACHE, 1, &pResults->hServer, &pItemValue, &pErrors);

			//Отображаем данные
			m_valueView.DeleteAllItems();
			LVITEM lvItem;
			ZeroMemory(&lvItem, sizeof(lvItem));
			lvItem.cchTextMax = 25;
			lvItem.mask = LVIF_TEXT;
			lvItem.pszText = szItemID;
			m_valueView.InsertItem(&lvItem);
			m_valueView.SetItem(0, 1, LVIF_TEXT, CToStringConverter::GetVTypeString(pResults -> vtCanonicalDataType), 0, 0, 0, 0, 0);
			m_valueView.SetItem(0, 3, LVIF_TEXT, CToStringConverter::GetFTString(pItemValue -> ftTimeStamp), 0, 0, 0, 0, 0);
			m_valueView.SetItem(0, 2, LVIF_TEXT, CToStringConverter::GetValueString(pItemValue -> vDataValue), 0, 0, 0, 0, 0);
			m_valueView.SetItem(0, 4, LVIF_TEXT, CToStringConverter::GetQualityString(pItemValue->wQuality), 0, 0, 0, 0, 0);
			pSyncIO->Release();
			CoTaskMemFree(pItemValue);
		}
		pItemMgt->Release();
		CoTaskMemFree(pErrors);
		CoTaskMemFree(pResults);
		CoTaskMemFree(pItems);
	}

}


void CMFCApplication1Dlg::OnBnClickedButton2()
{
	HRESULT hRes;
	LPWSTR szItemID = GetCurrentItemID();
	if (NULL == szItemID) return;

	long bActive = 1;
	DWORD dwUpdateRate = 0;
	IOPCItemMgt* pItemMgt = NULL;
	unsigned hClientGroup = 1;

	if (m_dwCookie != 0)
	{
		hRes = ((IConnectionPoint*)m_pDataCallback)->Unadvise(m_dwCookie);
		m_pDataCallback->Release();
		m_dwCookie = 0;
	}
	// if (m_hGroup != 0)
	{
		hRes = m_pOPCServer->RemoveGroup(m_hGroup, 1);
		m_hGroup = 0;
	}
	// else
	{
		hRes = m_pOPCServer->AddGroup(OLESTR("MyGroup"), bActive, dwUpdateRate, hClientGroup, NULL, NULL, 0,
			&m_hGroup, &dwUpdateRate, IID_IOPCItemMgt, (IUnknown**)&pItemMgt);
		if (FAILED(hRes))
		{
			LPWSTR lpError;
			m_pOPCServer->GetErrorString(hRes, LOCALE_SYSTEM_DEFAULT, &lpError);
			MessageBox(lpError, L"Ошибка", MB_OK);
			return;
		}
	}
	IConnectionPointContainer* pCPC;
	IID IID_CPC = __uuidof(IConnectionPointContainer);
	hRes = pItemMgt->QueryInterface(IID_CPC, (void**)&pCPC);

	hRes = pCPC->FindConnectionPoint(__uuidof(IOPCDataCallback),
		(IConnectionPoint**)&m_pDataCallback);
	pCPC->Release();

	if (NULL == m_pSink)
		m_pSink = new CDataCallback(&m_valueView, szItemID);
	else
		m_pSink->SetItemID(szItemID);

	hRes = ((IConnectionPoint*)m_pDataCallback)->Advise((IUnknown*)m_pSink, &m_dwCookie);

	if (FAILED(hRes))
	{
		LPWSTR lpError;
		m_pOPCServer->GetErrorString(hRes, 2, &lpError);
		MessageBox(lpError, L"Ошибка", MB_OK);
	}

	//Добавляем элементы в группу
	DWORD dwCount = 1;
	tagOPCITEMDEF* pItems = (tagOPCITEMDEF*)CoTaskMemAlloc(dwCount * sizeof(tagOPCITEMDEF));
	tagOPCITEMRESULT* pResults = NULL;
	HRESULT* pErrors = NULL;

	pItems[0].szItemID = szItemID;
	pItems[0].szAccessPath = NULL;
	pItems[0].bActive = TRUE;
	pItems[0].hClient = 0;
	pItems[0].vtRequestedDataType = VT_EMPTY;
	pItems[0].dwBlobSize = 0;
	pItems[0].pBlob = NULL;

	hRes = pItemMgt->AddItems(1, pItems, &pResults, &pErrors);
	if (FAILED(hRes))
	{
		LPWSTR lpMsg = NULL;
	
		m_pOPCServer->GetErrorString(hRes, 2, &lpMsg);
		MessageBox(lpMsg, L"Ошибка", MB_OK);
	}
	else
	{
		CoTaskMemFree(pResults);
		CoTaskMemFree(pErrors);
	}

	// Уменьшает кол-во ссылок на интерфейс
	pItemMgt->Release();
	//Освобождаем выделенную память
	CoTaskMemFree(pItems);
}
