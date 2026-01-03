#include "pch.h"
#include <atlsafe.h>
#include "ToStringConverter.h"

// Преобразование типа в текст
CString CToStringConverter::GetVTypeString(unsigned short usType)
{
	CString prefix;
	if (usType & VT_ARRAY)
	{
		prefix = L"Array of ";
		usType &= ~VT_ARRAY;
	}
	switch (usType)
	{
		case VARENUM::VT_R4: return prefix+CString("VT_R4");
		case VARENUM::VT_R8: return prefix + CString("VT_R8");
		// case VARENUM::VT_ARRAY: return CString("VT_ARRAY");
		case VARENUM::VT_BOOL: return prefix + CString("VT_BOOL");
		case VARENUM::VT_BSTR: return prefix + CString("VT_BSTR");
		case VARENUM::VT_DECIMAL: return prefix + CString("VT_DECIMAL");
		case VARENUM::VT_I1: return prefix + CString("VT_I1");
		case VARENUM::VT_I2: return prefix + CString("VT_I2");
		case VARENUM::VT_I4: return prefix + CString("VT_I4");
		case VARENUM::VT_I8: return prefix + CString("VT_I8");
		case VARENUM::VT_FILETIME: return prefix + CString("FILETIME");
		default: return CString("Unknown Type");
	}
}
CString CToStringConverter::GetQualityString(unsigned short usQuality)
{
	switch (usQuality)
	{
		case 0x00: return L"Bad";
		case 0x04: return L"Config Error";
		case 0x08: return L"Not Connected";
		case 0x0C: return L"Device Failure";
		case 0x10: return L"Sensor Failure";
		case 0x14: return L"Last Known";
		case 0x18: return L"Comm Failure";
		case 0x1C: return L"Out of Service";
		case 0x20: return L"Initializing";
		case 0x40: return L"Uncertain";
		case 0x44: return L"Last Usable";
		case 0x50: return L"Sensor Calibration";
		case 0x54: return L"EGU Exceeded";
		case 0x58: return L"Sub Normal";
		case 0xC0: return L"Good";
		case 0xD8: return L"Local Override";
		default: return L"Unknown";
	}
}

CString CToStringConverter::GetFTString(_FILETIME ft)
{
	COleDateTime dt = COleDateTime(ft);
	return dt.Format(L"%Y-%m-%d %H:%M:%S");
}

CString CToStringConverter::GetValueString(VARIANT var)
{
	if (var.vt == (VT_ARRAY | VT_R8))
	{
		CComSafeArray<double> sa;
		sa.Attach(var.parray);
		ULONG size = sa.GetCount();
		CString result = L"[";
		for (ULONG i = 0; i < size; ++i)
		{
			if (i > 0)
			{
				result += L", ";
			}
			CString item;
			item.Format(L"%g", sa.GetAt(i));
			result += item;
		}
		result += L"]";
		return result;
	}
	return CString(var);
}