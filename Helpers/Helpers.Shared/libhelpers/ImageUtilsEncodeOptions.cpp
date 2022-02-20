#include "ImageUtilsEncodeOptions.h"

#include <string>

ImageUtilsEncodeNoOptions::ImageUtilsEncodeNoOptions() {
}

HRESULT ImageUtilsEncodeNoOptions::SaveImpl(IPropertyBag2 *props) const {
	return S_OK;
}

ImageUtilsEncodeJpegOptions::ImageUtilsEncodeJpegOptions()
	: Quality(1.0f) {
}

HRESULT ImageUtilsEncodeJpegOptions::SaveImpl(IPropertyBag2 *props) const {
	std::wstring tmpStr = L"ImageQuality";
	PROPBAG2 option = { 0 };
	option.pstrName = tmpStr.data();
	VARIANT varValue;
	varValue.vt = VT_R4;
	varValue.fltVal = this->Quality;

	return props->Write(1, &option, &varValue);
}