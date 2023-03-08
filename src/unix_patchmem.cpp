#include <hooklib/hooklib.h>

#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

bool hooklib::PatchMemData(void* pAddr, std::size_t buf_len, void* pNewData, std::size_t data_len) {
	if (!buf_len || !data_len || !pNewData || !pAddr || buf_len < data_len) {
		return false;
	}

	std::size_t pageSize = sysconf(_SC_PAGESIZE);
	std::uintptr_t start = reinterpret_cast<std::uintptr_t>(pAddr);
	std::uintptr_t end = start + buf_len;
	std::uintptr_t pageStart = start & -pageSize;

	mprotect(reinterpret_cast<void*>(pageStart), end - pageStart, PROT_READ | PROT_WRITE | PROT_EXEC);

	memcpy(pAddr, pNewData, data_len);

	mprotect(reinterpret_cast<void*>(pageStart), end - pageStart, PROT_READ | PROT_EXEC);

	return true;
}