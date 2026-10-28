
#pragma once

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d12.lib")

class D3D12_renderer
{
public:
    bool initialize(IDXGIAdapter1* adapter);

private:
    bool create_device();
    bool create_queue();
    bool create_allocator();
    bool create_command_list();

    // the memory the GPU will draw into
    bool create_render_target_texture();

    // stores the descriptor
    bool create_descriptor_heap();

    // it writes a descriptor int the slot at rtv_handle and the descriptor points at render_target
    bool create_render_target_view();
 
    // record the commands that fill the render target with a color
    bool record_commands();

    Microsoft::WRL::ComPtr<IDXGIAdapter1>               adapter;
    Microsoft::WRL::ComPtr<ID3D12Device>                device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>          queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>   command_list;

    D3D12_RESOURCE_DESC                                 resource_desc {};
    Microsoft::WRL::ComPtr<ID3D12Resource>              render_target;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        rtv_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE                         rtv_handle = {};

    static constexpr UINT        texture_width  = 1280;
    static constexpr UINT        texture_height = 720;
    static constexpr DXGI_FORMAT texture_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    static constexpr float       clear_color[4] = { 0.2f, 0.4f, 0.8f, 1.0f };
};
