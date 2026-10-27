
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

    Microsoft::WRL::ComPtr<IDXGIAdapter1>               adapter;
    Microsoft::WRL::ComPtr<ID3D12Device>                device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue>          queue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator>      allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>   command_list;

private:
    bool create_device();
    bool create_queue();
    bool create_allocator();
    bool create_command_list();
};
