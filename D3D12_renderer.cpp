
#include "D3D12_renderer.hpp"

#include <iostream>
#include <iomanip>

bool D3D12_renderer::initialize(IDXGIAdapter1* adapter)
{
    this->adapter = adapter;

    if (!create_device())
        return false;

    if (!create_queue())
        return false;

    if (!create_allocator())
        return false;

    if (!create_command_list())
        return false;

    return true;
}

bool D3D12_renderer::create_device()
{
    HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    if (FAILED(hr))
    {
        std::wcerr << L"D3D12CreateDevice failed. HRESULT = 0x" << std::hex << hr << std::endl;
        return false;
    }

    return true;
}

bool D3D12_renderer::create_queue()
{
    D3D12_COMMAND_QUEUE_DESC queue_desc = {};
    queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    HRESULT hr = device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&queue));

    if (FAILED(hr))
    {
        std::wcerr << L"CreateCommandQueue failed. HRESULT = 0x" << std::hex << hr << std::endl;
        return false;
    }

    return true;
}

bool D3D12_renderer::create_allocator()
{
    HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));

    if (FAILED(hr))
    {
        std::wcerr << L"CreateCommandAllocator failed. HRESULT = 0x" << std::hex << hr << std::endl;
        return false;
    }

    return true;
}

bool D3D12_renderer::create_command_list()
{
    HRESULT hr = device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        allocator.Get(),
        nullptr,
        IID_PPV_ARGS(&command_list)
    );

    if (FAILED(hr))
    {
        std::wcerr << L"CreateCommandList failed. HRESULT = 0x" << std::hex << hr << std::endl;
        return false;
    }

    return true;
}
