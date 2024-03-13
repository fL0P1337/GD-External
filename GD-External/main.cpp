#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#define DIRECTINPUT_VERSION 0x0800
#include "main.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
HWND GD_window;
DWORD procID;   
HANDLE hProcess = "GeometryDash.exe";
uintptr_t GetModuleBaseAddress(const char* modName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!strcmp(modEntry.szModule, modName)) {
                    CloseHandle(hSnap);
                    return (uintptr_t)modEntry.modBaseAddr;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
}

int main(int, char**)
{
    bool noclip = false;
    bool EAL = false;
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, AppClass, NULL };
    RegisterClassEx(&wc);
    hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, AppClass, AppName, WS_POPUP, (desktop.right / 2) - (WindowWidth / 2), (desktop.bottom / 2) - (WindowHeight / 2), WindowWidth, WindowHeight, 0, 0, wc.hInstance, 0);

    //SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);

    if (CreateDeviceD3D(hwnd) < 0)
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    DefaultFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    static bool open = true;
    DWORD dwFlag = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
    char somelogin[25] = "";
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        if (!open) ExitProcess(EXIT_SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        if (noclip) {
            GetWindowThreadProcessId(GD_window, &procID);
            uintptr_t ModuleBase = GetModuleBaseAddress("GeometryDash.exe");
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
            uintptr_t nc_address = ModuleBase + 0x20A23C;
            std::vector<uint8_t> bytes = { 0xE9, 0x79, 0x06, 0x00, 0x00 };
            WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(nc_address), bytes.data(), bytes.size(), nullptr);
        }
        else {
            procID = GetProcessId(hProcess);
            uintptr_t ModuleBase = GetModuleBaseAddress("GeometryDash.exe");
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
            uintptr_t nc_address = ModuleBase + 0x20A23C;
            std::vector<uint8_t> bytes = { 0x6A, 0x14, 0x8B, 0xCB, 0xFF };
            WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(nc_address), bytes.data(), bytes.size(), nullptr);
        }
        if (EAL) {
            GetWindowThreadProcessId(GD_window, &procID);
            uintptr_t ModuleBase = GetModuleBaseAddress("GeometryDash.exe");
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
            uintptr_t EAL_adr = ModuleBase + 0x1E4A32;
            std::vector<uint8_t> bytes = { 0x90, 0x90 };
            WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(EAL_adr), bytes.data(), bytes.size(), nullptr);
        }
        else {
            GetWindowThreadProcessId(GD_window, &procID);
            uintptr_t ModuleBase = GetModuleBaseAddress("GeometryDash.exe");
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
            uintptr_t EAL_adr = ModuleBase + 0x1E4A32;
            std::vector<uint8_t> bytes = { 0x75, 0x6C };
            WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(EAL_adr), bytes.data(), bytes.size(), nullptr);
        }
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        {
            ImGui::Begin(AppName, &open, dwFlag);
            static int state;
            ImGui::AlignTextToFramePadding();
            if (ImGui::Button("Inject")) {
                GD_window = FindWindow(0, "Geometry Dash");
                if (GD_window == NULL) {
                    state = 0;
                }
                else {
                    state = 1;
                }
            }
            ImGui::Text("isOpened: %d", state);
            ImGui::Text("procID: %d", GetWindowThreadProcessId(GD_window, &procID));
            ImGui::Checkbox("Noclip", &noclip);
            ImGui::Checkbox("Edit Any Level", &EAL);


        }
        ImGui::End();

        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) ResetDevice();
        {
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}
