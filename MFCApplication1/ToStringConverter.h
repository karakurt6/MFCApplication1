#pragma once
class CToStringConverter
{
public:
	static CString GetVTypeString(unsigned short usType);
	static CString GetQualityString(unsigned short usQuality);
	static CString GetFTString(_FILETIME ft);
	static CString GetValueString(VARIANT var);
};

