using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using opcproxy;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices.ComTypes;

namespace WindowsFormsApp1
{
    public class DataCallback: IOPCDataCallback
    {
        //конструктор
        public DataCallback(Form1 dlg, string szItemID)
        {
            m_Dlg = dlg; // Передаем ссылку на нашу форму в качестве параметра
            SetItemID(szItemID); // и идентификатор текущего элемента данных
        }
        public void SetItemID(string szItemID)
        {
            m_szItemID = szItemID;
        }

        //========Перегрузка методов интерфейса IOPCDataCallback ============
        public void OnCancelComplete(uint dwTransid, uint hGroup) { }
        public void OnDataChange(uint dwTransid, uint hGroup, int hrMasterquality, int hrMastererror,
        uint dwCount, ref uint phClientItems, ref object pvValues, ref ushort pwQualities, ref
        _FILETIME pftTimeStamps, ref int pErrors)
        {
            //Получаем тип элемента данных
            IntPtr iptrValues = Marshal.AllocCoTaskMem(2);
            Marshal.GetNativeVariantForObject(pvValues, iptrValues);
            byte[] vt = new byte[2];
            Marshal.Copy(iptrValues, vt, 0, 2);
            Marshal.FreeCoTaskMem(iptrValues);
            ushort usVt = (ushort)(vt[0] + vt[1] * 255);
            ListViewItem lvItem = new ListViewItem();
            ListViewItem.ListViewSubItem[] lvSubItem = new ListViewItem.ListViewSubItem[3];
            lvItem.Text = m_szItemID;
            lvItem.SubItems.Add(ToStringConverter.GetVTString(usVt));
            lvItem.SubItems.Add(ToStringConverter.GetValueString(pvValues));
            lvItem.SubItems.Add(ToStringConverter.GetFTSting(pftTimeStamps));
            lvItem.SubItems.Add(ToStringConverter.GetQualityString(pwQualities));
            m_Dlg.SetItemValue(lvItem); //делегируем вызов форме
        }
        public void OnReadComplete(uint dwTransid, uint hGroup, int hrMasterquality, int
        hrMastererror, uint dwCount, ref uint phClientItems, ref object pvValues, ref ushort pwQualities,
        ref _FILETIME pftTimeStamps, ref int pErrors)
        {
        }
        public void OnWriteComplete(uint dwTransid, uint hGroup, int hrMastererr, uint dwCount, ref
        uint pClienthandles, ref int pErrors)
        { 
        }
        
        //================Переменные=========================
        private Form1 m_Dlg;
        private string m_szItemID;
    }
}
