#include <windows.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#include "D3D12_renderer.hpp"

#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

static const wchar_t* vendor_name(UINT vendor_id)
{
    switch (vendor_id)
    {
    case 0x10DE: return L"NVIDIA";
    case 0x1002: return L"AMD";
    case 0x1022: return L"AMD";
    case 0x8086: return L"Intel";
    case 0x1414: return L"Microsoft (software / WARP)";
    case 0x5143: return L"Qualcomm";
    default:     return L"Unknown";
    }
}

static std::wstring format_bytes(SIZE_T bytes)
{
    constexpr double mb = 1024.0 * 1024.0;
    constexpr double gb = mb * 1024.0;

    std::wstringstream stream;
    stream << std::fixed << std::setprecision(2);

    if (bytes >= static_cast<SIZE_T>(gb))
        stream << (bytes / gb) << L" GB";
    else
        stream << (bytes / mb) << L" MB";

    return stream.str();
}

int wmain()
{
    std::wcout.imbue(std::locale(""));

    ComPtr<IDXGIFactory1> factory;

    HRESULT hr = CreateDXGIFactory1(
        IID_PPV_ARGS(&factory)
    );

    if (FAILED(hr))
    {
        std::wcerr << L"CreateDXGIFactory1 failed. HRESULT = 0x"
                   << std::hex << hr << std::endl;
        return 1;
    }

    std::wcout << L"--- DXGI GPU Adapter Enumeration ---\n\n";

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIAdapter1> chosen_adapter;

    UINT adapter_index = 0;

    while (factory->EnumAdapters1(adapter_index, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 adapter_desc = {};

        if (SUCCEEDED(adapter->GetDesc1(&adapter_desc)))
        {
            bool is_software = (adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0;

            std::wcout << L"Adapter #" << adapter_index << L"\n";
            std::wcout << L"  Model (Description) : " << adapter_desc.Description << L"\n";
            std::wcout << L"  Vendor              : " << vendor_name(adapter_desc.VendorId) 
                       << L"  (ID: 0x" << std::hex << std::uppercase 
                       << adapter_desc.VendorId << std::dec << L")\n";
            std::wcout << L"  Device ID           : 0x" << std::hex << std::uppercase << adapter_desc.DeviceId << std::dec << L"\n";
            std::wcout << L"  SubSys ID           : 0x" << std::hex << std::uppercase << adapter_desc.SubSysId << std::dec << L"\n";
            std::wcout << L"  Revision            : " << adapter_desc.Revision << L"\n";
            std::wcout << L"  Dedicated Video Mem : " << format_bytes(adapter_desc.DedicatedVideoMemory) << L"\n";
            std::wcout << L"  Dedicated System Mem: " << format_bytes(adapter_desc.DedicatedSystemMemory) << L"\n";
            std::wcout << L"  Shared System Mem   : " << format_bytes(adapter_desc.SharedSystemMemory) << L"\n";
            std::wcout << L"  LUID                : "
                       << adapter_desc.AdapterLuid.HighPart << L":" << adapter_desc.AdapterLuid.LowPart << L"\n";

            std::wcout << L"  Type                : "
                       << ((adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                               ? L"Software (e.g. Microsoft Basic Render Driver)"
                               : L"Hardware")
                       << L"\n";

            if (!is_software && chosen_adapter == nullptr)
                chosen_adapter = adapter;
        }

        adapter.Reset();
        ++adapter_index;
    }

    if (adapter_index == 0)
    {
        std::wcout << L"No adapters found.\n";
        return 1;
    }

    if (chosen_adapter == nullptr)
    {
        std::wcerr << L"No hardware adapter found.\n";
        return 1;
    }

    D3D12_renderer context;

    if (!context.initialize(chosen_adapter.Get()))
        return 1;

    std::wcout << L"Direct3D 12 device, queue, allocator and command list created.\n";

    return 0;
}
