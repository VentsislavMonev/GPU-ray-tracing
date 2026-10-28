
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

    
    if (!create_render_target_texture())
        return false;

    if (!create_descriptor_heap())
        return false;

    if (!create_render_target_view())
        return false;

    if (!record_commands())
        return false;

    return true;
}

bool D3D12_renderer::create_device()
{
    HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

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

// the memory the GPU will draw into
bool D3D12_renderer::create_render_target_texture()
{
    D3D12_HEAP_PROPERTIES heap_properties = {};
    heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;
 
    resource_desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resource_desc.Alignment          = 0;
    resource_desc.Width              = texture_width;
    resource_desc.Height             = texture_height;
    resource_desc.DepthOrArraySize   = 1;
    resource_desc.MipLevels          = 1;
    resource_desc.Format             = texture_format;
    resource_desc.SampleDesc.Count   = 1;
    resource_desc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
 
    D3D12_CLEAR_VALUE clear_value = {};
    clear_value.Format = texture_format;
    clear_value.Color[0] = clear_color[0];
    clear_value.Color[1] = clear_color[1];
    clear_value.Color[2] = clear_color[2];
    clear_value.Color[3] = clear_color[3];
 
    HRESULT hr = device->CreateCommittedResource(
        &heap_properties,
        D3D12_HEAP_FLAG_NONE,
        &resource_desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clear_value,
        IID_PPV_ARGS(&render_target));
 
    return SUCCEEDED(hr);
}

// stores the descriptor
bool D3D12_renderer::create_descriptor_heap()
{
    // descriptor heap
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heap_desc.NumDescriptors = 1;
    heap_desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
 
    // creates descriptor
    HRESULT hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&rtv_heap));
    if (FAILED(hr))
        return false;
 
    // show us where that heap starts
    rtv_handle = rtv_heap->GetCPUDescriptorHandleForHeapStart();
    return true;
}
 
// it writes a descriptor int the slot at rtv_handle and the descriptor points at render_target
bool D3D12_renderer::create_render_target_view()
{
    device->CreateRenderTargetView(render_target.Get(), nullptr, rtv_handle);
    return true;
}
 
bool D3D12_renderer::record_commands()
{
    // “draw into this render target from now on.”
    command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);
 
    // Fill it with a color
    command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
 
    // Finish recording
    return SUCCEEDED(command_list->Close());
}
 
/*
Call order in initialize(), after your existing steps:
 
    create_device();
    create_queue();
    create_allocator();
    create_command_list();
    create_render_target_texture();   // needs device
    create_descriptor_heap();         // needs device
    create_render_target_view();      // needs resource + descriptor handle
    record_commands();                // needs command list + handle
 
Note: if your create_command_list() already calls Close() on the list, either remove that
call or do allocator->Reset() and command_list->Reset(allocator.Get(), nullptr) before
record_commands(), since a closed list can't record.
*/