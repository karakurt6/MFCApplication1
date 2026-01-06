using OpcEnumLib;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using opcproxy;
using System.Xml.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        const uint LOCALE_SYSTEM_DEFAULT = 0x0800;
        const uint SENTINEL_VALUE = 0xFFFF;

        public Form1()
        {
            InitializeComponent();
            m_hGroup = SENTINEL_VALUE;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();

        }

        private int ShowRegisteredServers()
        {
            //Создаем объект списка OPC серверов
            OpcServerList pServerList = new OpcServerList();
            // Идентификатор категории ОРС DA 2.0
            Guid clsidcat = new Guid("{63D5F432-CFE4-11D1-B2C8-0060083BA1FB}");
            //перечислитель, в котором будут храниться GUID серверов
            IOPCEnumGUID pIOPCEnumGuid;
            //запрос по группе серверов спецификации OPC DA 2.0
            pServerList.EnumClassesOfCategories(1, ref clsidcat, 0, ref clsidcat, out pIOPCEnumGuid);

            string pszProgID; // буфер для записи ProgID серверов
            string pszUserType; // буфер для записи описания серверов
            string pszVerIndProgID;
            Guid guid = new Guid();
            int nServerCnt = 0;
            uint iRetSvr; // количество серверов, предоставленных запросом
                          // получение идентификаторов серверов
            pIOPCEnumGuid.Next(1, out guid, out iRetSvr);
            while (iRetSvr != 0)
            {
                nServerCnt++;
                pServerList.GetClassDetails(ref guid, out pszProgID, out pszUserType, out pszVerIndProgID);
                ListViewItem lvItem = new ListViewItem();
                lvItem.Tag = (object) guid;
                lvItem.Text = pszProgID;
                m_listOPCServers.Items.Add(lvItem);
                pIOPCEnumGuid.Next(1, out guid, out iRetSvr);
            }
            return nServerCnt;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (0 == ShowRegisteredServers())
            {
                MessageBox.Show("Нет установленных серверов");
            }
        }

        private void DisplayChildren(TreeNode ParentNode, IOPCBrowseServerAddressSpace pParent)
        {
            uint cnt;
            string strName;
            string szItemID;
            opcproxy.IEnumString pEnum;
            tagOPCNAMESPACETYPE ns;
            pParent.QueryOrganization(out ns);
            if (ns == tagOPCNAMESPACETYPE.OPC_NS_FLAT)
            {
                // Вначале выводим все листья на данном уровне
                pParent.BrowseOPCItemIDs(tagOPCBROWSETYPE.OPC_FLAT, "", 0, 0, out pEnum);
                pEnum.RemoteNext(1, out strName, out cnt);
                while (cnt != 0)
                {
                    //TreeNode tvNode = new TreeNode(strName);
                    //ParentNode.Nodes.Add(tvNode);
                    TreeNode tvNode = m_treeOPCServerBrowse.Nodes.Add(strName);
                    //получает полный идентификатор тега
                    pParent.GetItemID(strName, out szItemID);
                    tvNode.Tag = (object)szItemID;
                    pEnum.RemoteNext(1, out strName, out cnt);
                }
            }
            else
            {
                // Вначале выводим все листья на данном уровне
                pParent.BrowseOPCItemIDs(tagOPCBROWSETYPE.OPC_LEAF, "", 1, 0, out pEnum);
                pEnum.RemoteNext(1, out strName, out cnt);
                while (cnt != 0)
                {
                    TreeNode tvNode = ParentNode.Nodes.Add(strName);
                    //получает полный идентификатор тега
                    pParent.GetItemID(strName, out szItemID);
                    tvNode.Tag = (object)szItemID;
                    pEnum.RemoteNext(1, out strName, out cnt);
                }
                // Получаем ветви на данном уровне
                pParent.BrowseOPCItemIDs(tagOPCBROWSETYPE.OPC_BRANCH, "", 1, 0, out pEnum);
                pEnum.RemoteNext(1, out strName, out cnt);
                while (cnt != 0)
                {
                    TreeNode parentNode = ParentNode.Nodes.Add(strName);
                    pParent.ChangeBrowsePosition(tagOPCBROWSEDIRECTION.OPC_BROWSE_DOWN, strName);
                    DisplayChildren(parentNode, pParent); //Рекурсивно вызываем метод
                    //После того как все дочерние узлы показаны постепенно поднимаемся вверх
                    pParent.ChangeBrowsePosition(tagOPCBROWSEDIRECTION.OPC_BROWSE_UP, strName);
                    pEnum.RemoteNext(1, out strName, out cnt);
                }
                
            }
        }

        private object m_pIfaceObj;

        private void OnServerChange()
        {
            if (m_dwCookie != 0)
            {
                m_pDataCallback.Unadvise(m_dwCookie);
                m_dwCookie = 0;
            }
            if (m_hGroup != SENTINEL_VALUE)
            {
                IOPCServer pServer = (IOPCServer)m_pIfaceObj;
                pServer.RemoveGroup(m_hGroup, 1);
                m_hGroup = SENTINEL_VALUE;
            }
        }

        private void AddChildren()
        {
            TreeNode one = m_treeOPCServerBrowse.Nodes.Add("one");
            TreeNode two = new TreeNode("two");
            m_treeOPCServerBrowse.Nodes.Add(two);

        }

        private void ConnectAndBrowseServer(Guid guid)
        {
            try
            {
                Type typeOfServer = Type.GetTypeFromCLSID(guid);
                m_pIfaceObj = Activator.CreateInstance(typeOfServer);
                if (m_pIfaceObj is IOPCBrowseServerAddressSpace)
                {
                    IOPCBrowseServerAddressSpace pBrowse = (IOPCBrowseServerAddressSpace)m_pIfaceObj;
                    DisplayChildren(null, pBrowse);
                    // AddChildren();
                }
            }
            catch (ApplicationException ex)
            {
                string msg;
                //Получаем HRESULT, соответствующий сгенерированному исключению
                int hRes = Marshal.GetHRForException(ex);
                //Запрашиваем у сервера текст ошибки
                IOPCServer pServer = m_pIfaceObj as IOPCServer;
                if (pServer != null) 
                { 
                    pServer.GetErrorString(hRes, LOCALE_SYSTEM_DEFAULT, out msg);
                }
                else
                {
                    msg = "Cannot get reference on IOPCServer";
                }
                //Показываем сообщение ошибки
                MessageBox.Show(msg, "Ошибка");
            }
        }

        private void m_listOPCServers_Click(object sender, EventArgs e)
        {
            ListView.SelectedIndexCollection selCol = m_listOPCServers.SelectedIndices;
            if (selCol.Count == 0) return;
            OnServerChange();
            Guid guid = (Guid)m_listOPCServers.SelectedItems[0].Tag;
            m_treeOPCServerBrowse.Nodes.Clear();
            ConnectAndBrowseServer(guid);
        }

        #region syncroneousreading
        private uint m_hGroup;

        private void button3_Click(object sender, EventArgs e)
        {
            //Получаем текущий выбранный элемент данных
            if (null == m_treeOPCServerBrowse.SelectedNode) return;
            string itemID = (string)m_treeOPCServerBrowse.SelectedNode.Tag;

            // Запрашиваем интерфейс IOPCServer
            IOPCServer pServer = (IOPCServer)m_pIfaceObj;

            uint updateRate = 1000; // время опроса создаваемой группы
            int bActive = 1; // активность группы - активна
            uint hClientGroup = 1; // клиентский описатель группы

            object iFaceObj; // сюда вернем интерфейс к группе
            Guid riid = typeof(IOPCItemMgt).GUID;

            int TimeBias = 0; // не используем смещения по времени
            float DeadBand = 0; // не используем зону нечувствительности

            try
            {
                /*
               Этот участок на данном этапе должен быть закомментирован. Его
               необходимо будет использовать тогда, когда добавим асинхронную
               операцию чтения по подписке
               */
               if (m_dwCookie != 0)
                {
                    m_pDataCallback.Unadvise(m_dwCookie);
                    m_dwCookie = 0;
                }
                if (m_hGroup != SENTINEL_VALUE) // Если группа была создана
                {
                    pServer.RemoveGroup(m_hGroup, 1); // удалим ее
                    m_hGroup = SENTINEL_VALUE;
                }
                //Создаем группу и получаем интерфейс IOPCItemMgt для управления ее состоянием
                pServer.AddGroup("MyGroup", bActive, updateRate, hClientGroup, ref
                    TimeBias, ref DeadBand, 2, out m_hGroup, out updateRate, ref
                    riid, out iFaceObj);
                IOPCItemMgt pItemMgt = (IOPCItemMgt)iFaceObj;

                //Создаем список элементов для добавления в группу, размером 1 элемент
                uint dwCount = 1;

                //Создаем описатель добавляемого элемента
                tagOPCITEMDEF pItems = new tagOPCITEMDEF();
                pItems.szItemID = itemID;
                pItems.szAccessPath = null;
                pItems.bActive = 1;
                pItems.hClient = 1;
                pItems.vtRequestedDataType = (ushort)VarEnum.VT_EMPTY;
                pItems.dwBlobSize = 0;
                pItems.pBlob = IntPtr.Zero;

                // В эти две переменные будут записаны массивы ошибок и результатов выполнения
                IntPtr iptrErrors = IntPtr.Zero;
                IntPtr iptrResults = IntPtr.Zero;

                // Добавляем элемент данных в группу
                pItemMgt.AddItems(dwCount, pItems, out iptrResults, out iptrErrors);

                // Переносим результаты и ошибки из неуправляемой памяти в управляемую
                tagOPCITEMRESULT pResults = (tagOPCITEMRESULT)Marshal.PtrToStructure(iptrResults, typeof(tagOPCITEMRESULT));
                int[] hRes = new int[1];
                Marshal.Copy(iptrErrors, hRes, 0, 1);

                //Генерируем исключение в случае ошибки в HRESULT
                Marshal.ThrowExceptionForHR(hRes[0]);

                // Получаем интерфейс IOPCSyncIO для операций синхронного чтения
                IOPCSyncIO pSyncIO = (IOPCSyncIO)iFaceObj;

                // В эту переменную будут записаны результаты чтения
                IntPtr iptrItemState = IntPtr.Zero;

                iptrErrors = IntPtr.Zero;

                //Читаем данные из сервера
                pSyncIO.Read(tagOPCDATASOURCE.OPC_DS_DEVICE, 1, ref pResults.hServer, out iptrItemState, out iptrErrors);

                // Переносим результаты и ошибки из неуправляемой памяти в управляемую
                tagOPCITEMSTATE pItemState = (tagOPCITEMSTATE)Marshal.PtrToStructure(iptrItemState, typeof(tagOPCITEMSTATE));
                Marshal.Copy(iptrErrors, hRes, 0, 1);

                //Генерируем исключение в случае ошибки в HRESULT
                Marshal.ThrowExceptionForHR(hRes[0]);

                //Выводим полученные данные
                ListViewItem lvItem = new ListViewItem();
                ListViewItem.ListViewSubItem[] lvSubItem = new
                ListViewItem.ListViewSubItem[3];
                lvItem.Text = itemID;
                lvItem.SubItems.Add(ToStringConverter.GetVTString(pResults.vtCanonicalDataType));
                lvItem.SubItems.Add(ToStringConverter.GetValueString(pItemState.vDataValue));
                lvItem.SubItems.Add(ToStringConverter.GetFTSting(pItemState.ftTimeStamp));
                lvItem.SubItems.Add(ToStringConverter.GetQualityString(pItemState.wQuality));
                m_valueView.Items.Clear();
                m_valueView.Items.Add(lvItem);
            }
            catch (System.Exception ex)
            {
                string msg;
                //Получаем HRESULT соответствующий сгененрированному исключению
                int hRes = Marshal.GetHRForException(ex);
                //Запрашиваем у сервера текст ошибки, соответствующий текущему HRESULT
                pServer.GetErrorString(hRes, LOCALE_SYSTEM_DEFAULT, out msg);
                //Показываем сообщение ошибки
                MessageBox.Show(msg, "Ошибка");
            }
        }
        #endregion

        #region asyncroneousreading
        public DataCallback m_pSink; //Объект, который будет принимать вызовы сервера
        public IConnectionPoint m_pDataCallback; // Точка подключения к событиям сервера
        int m_dwCookie; // Описатель подписки к событиям сервера

        private void button4_Click(object sender, EventArgs e)
        {
            // Процесс создания группы и добавления элементов данных аналогичен синхронному чтению
            if (null == m_treeOPCServerBrowse.SelectedNode) return;
            string szItemID = (string)m_treeOPCServerBrowse.SelectedNode.Tag;

            IOPCServer pServer = (IOPCServer)m_pIfaceObj;
            uint updateRate = 1000; //частота опроса группы
            object iFaceObj = null;
            Guid riid = typeof(IOPCItemMgt).GUID;
            int TimeBias = 0;
            float DeadBand = 0;
            int bActive = 1;
            uint hClientGroup = 1;
            try
            {
                //Если ранее была активирована подписка, то отменить ее
                if (m_dwCookie != 0)
                    m_pDataCallback.Unadvise(m_dwCookie);
                if (m_hGroup != SENTINEL_VALUE)
                {
                    pServer.RemoveGroup(m_hGroup, 1);
                    m_hGroup = SENTINEL_VALUE;
                }

                pServer.AddGroup("MyGroup", bActive, updateRate, hClientGroup, ref TimeBias, ref DeadBand,
                2, out m_hGroup, out updateRate, ref riid, out iFaceObj);
                IOPCItemMgt pItemMgt = (IOPCItemMgt)iFaceObj;

                uint dwCount = 1;
                IConnectionPointContainer pCPC;
                pCPC = (IConnectionPointContainer)iFaceObj;

                riid = typeof(IOPCDataCallback).GUID;
                pCPC.FindConnectionPoint(ref riid, out m_pDataCallback);

                tagOPCITEMDEF pItems = new tagOPCITEMDEF();

                //Создаем объект обработчика событий сервера или если он уже был создан изменяем идентификатор элемента данных на текущий
                if (null == m_pSink)
                    m_pSink = new DataCallback(this, szItemID);
                else
                    m_pSink.SetItemID(szItemID);

                //Подписываемся на события сервера
                m_pDataCallback.Advise(m_pSink, out m_dwCookie);

                pItems.szItemID = szItemID;
                pItems.szAccessPath = null;
                pItems.bActive = 1;
                pItems.hClient = 1;
                pItems.vtRequestedDataType = (ushort)VarEnum.VT_EMPTY;
                pItems.dwBlobSize = 0;
                pItems.pBlob = IntPtr.Zero;

                IntPtr iptrErrors = IntPtr.Zero;
                IntPtr iptrResults = IntPtr.Zero;

                pItemMgt.AddItems(dwCount, pItems, out iptrResults, out iptrErrors);
                int[] hRes = new int[1];
                Marshal.Copy(iptrErrors, hRes, 0, 1);
                Marshal.ThrowExceptionForHR(hRes[0]);
                tagOPCITEMRESULT pResults = (tagOPCITEMRESULT)Marshal.PtrToStructure(iptrResults, typeof(tagOPCITEMRESULT));
                Marshal.FreeCoTaskMem(iptrResults);
                Marshal.FreeCoTaskMem(iptrErrors);
            }
            catch (ApplicationException ex)
            {
                string msg;
                int hRes = Marshal.GetHRForException(ex);
                pServer.GetErrorString(hRes, LOCALE_SYSTEM_DEFAULT, out msg);
                MessageBox.Show(msg, "Ошибка");
            }
        }


        delegate void SetItemCallback(ListViewItem lvItem);
        public void SetItemValue(ListViewItem lvItem)
        {
            if (m_valueView.InvokeRequired)
            {
                SetItemCallback d = new SetItemCallback(SetItemValue);
                Invoke(d, new object[] { lvItem });
            }
            else
            {
                m_valueView.Items.Clear();
                m_valueView.Items.Add(lvItem);
            }
        }

        #endregion

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            IOPCServer pServer = (IOPCServer)m_pIfaceObj;
            try
            {
                if (m_dwCookie != 0)
                {
                    m_pDataCallback.Unadvise(m_dwCookie);
                }
                if (m_hGroup != 0)
                {
                    pServer.RemoveGroup(m_hGroup, 1);
                }
            }
            catch (ApplicationException ex)
            {
                int hRes = Marshal.GetHRForException(ex);
                string msg;
                pServer.GetErrorString(hRes, LOCALE_SYSTEM_DEFAULT, out msg);
                MessageBox.Show(msg, "Ошибка");
            }
        }
    }
}
