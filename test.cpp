#include <windows.h>        //Has HRESULT UINT SUCCEEDED FAILED and other usefull stuff 
#include <dxgi1_6.h>        // this is the direct x library that has factory and adapter patterns
#include <wrl/client.h>     // Microsoft::WRL::ComPtr smart pointer for the factory and adapter so they automatically release

#include <iomanip>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>

#pragma comment(lib, "dxgi.lib")   // Links dxgi.lib automatically with MSVC

using Microsoft::WRL::ComPtr;

// Map famous vendor ids to the names (AMD has 2 apperantly)
static const wchar_t* VendorName(UINT vendorId)
{
    switch (vendorId)
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

// Convert a byte count to a "x.xx GB" / "x.xx MB" string
static std::wstring FormatBytes(SIZE_T bytes)
{
    const double MB = 1024.0 * 1024.0;
    const double GB = MB * 1024.0;

    std::wstringstream ss;
    ss << std::fixed << std::setprecision(2);
    if (bytes >= static_cast<SIZE_T>(GB))
        ss << (bytes / GB) << L" GB";
    else
        ss << (bytes / MB) << L" MB";
    return ss.str();
}

int wmain()
{
    // Tells wcout what anotations to use for decimal point is it . or , for thousands is it 1 000 1,000 and how to handle charachters" 
    std::wcout.imbue(std::locale(""));

    // Create the DXGI factory and use ComPtr which is basicly smart pointer so we dont have to free memory ourselves
    ComPtr<IDXGIFactory1> factory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr))
    {
        std::wcerr << L"CreateDXGIFactory1 failed. HRESULT = 0x"
                   << std::hex << hr << std::endl;
        return 1;
    }

    // Enumerate all adapters and use ComPtr which is basicly smart pointer so we dont have to free memory ourselves
    std::wcout << L"--- DXGI GPU Adapter Enumeration ---\n\n";

    ComPtr<IDXGIAdapter1> adapter;
    UINT index = 0;
    while (factory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        // Get the description of the adapter.
        DXGI_ADAPTER_DESC1 desc = {};
        if (SUCCEEDED(adapter->GetDesc1(&desc)))
        {
            // Print the information.
            // The L before a string is used to indicate that the string uses wide charachters
            std::wcout << L"Adapter #" << index << L"\n";
            std::wcout << L"  Model (Description) : " << desc.Description << L"\n";
            std::wcout << L"  Vendor              : " << VendorName(desc.VendorId) 
                       << L"  (ID: 0x" << std::hex << std::uppercase 
                       << desc.VendorId << std::dec << L")\n";
            std::wcout << L"  Device ID           : 0x" << std::hex << std::uppercase << desc.DeviceId << std::dec << L"\n";
            std::wcout << L"  SubSys ID           : 0x" << std::hex << std::uppercase << desc.SubSysId << std::dec << L"\n";
            std::wcout << L"  Revision            : " << desc.Revision << L"\n";
            std::wcout << L"  Dedicated Video Mem : " << FormatBytes(desc.DedicatedVideoMemory) << L"\n";
            std::wcout << L"  Dedicated System Mem: " << FormatBytes(desc.DedicatedSystemMemory) << L"\n";
            std::wcout << L"  Shared System Mem   : " << FormatBytes(desc.SharedSystemMemory) << L"\n";
            std::wcout << L"  LUID                : "
                       << desc.AdapterLuid.HighPart << L":" << desc.AdapterLuid.LowPart << L"\n";

            std::wcout << L"  Type                : "
                       << ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                               ? L"Software (e.g. Microsoft Basic Render Driver)"
                               : L"Hardware")
                       << L"\n";
        }

        adapter.Reset();
        ++index;
    }

    if (index == 0)
        std::wcout << L"No adapters found.\n";

    return 0;
}