#include <hooklib/hooklib.h>

#include <Windows.h>

bool hooklib::PatchMemData(void* pAddr, std::size_t buf_len, void* pNewData, std::size_t data_len) {
	if (!buf_len || !data_len || !pNewData || !pAddr || buf_len < data_len) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	DWORD dwLastProtection;
	if (!VirtualProtect(pAddr, data_len, PAGE_EXECUTE_READWRITE, &dwLastProtection))
		return false;

	memcpy_s(pAddr, buf_len, pNewData, data_len);

	return VirtualProtect(pAddr, data_len, dwLastProtection, &dwLastProtection);
}
