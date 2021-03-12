#include <Windows.h>

#include <iostream>
#include <string>

namespace {

std::wstring UTF8ToUTF16(const std::string &str) {
    if (str.empty()) {
        return L"";
    }

    const char *ptr = str.c_str();
    const auto size = static_cast<int>(str.size());
    const int bytes = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, ptr, size, nullptr, 0);
    if (bytes == 0) {
        return L"";
    }

    std::wstring tmp(bytes, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, ptr, size, &tmp[0], bytes);
    return tmp;
}

} // namespace

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: get_service_displayname service_name" << std::endl;
        return 1;
    }

    const std::wstring service_name = UTF8ToUTF16(argv[1]);

    SC_HANDLE service_manager_handle = ::OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (service_manager_handle == nullptr) {
        std::cerr << "Failed to execute OpenSCManagerW: LastError=" << ::GetLastError() << std::endl;
        return 1;
    }

    wchar_t display_name[1024];
    DWORD display_size = 1024;
    const BOOL ret = GetServiceDisplayNameW(service_manager_handle, service_name.c_str(), display_name, &display_size);
    ::CloseServiceHandle(service_manager_handle);
    if (!ret) {
        std::cerr << "Failed to GetServiceDisplayNameW: LastError=" << ::GetLastError() << std::endl;
        return 1;
    }

    std::wcout << L"ServiceName: " << service_name << " DisplayName: " << display_name << std::endl;
    return 0;
}