#include <Windows.h>
#include <d3d11.h>
#include <iostream>
#include "gui.h"
#include "../cheat/aimbot.h"
#include "../renderer/imgui.h"
#include "../renderer/imgui_impl_dx9.h"
#include "../renderer/imgui_impl_win32.h"
#include <ctime>
#include <cstdio> 

#include <d3d9.h>
#include <d3dx9.h>
#include "../renderer/font.h"
#include <ctime>
#include <fstream>
#include <string>
#include <iostream>
#include <windows.h>
#include <shlobj.h> 
#include <string>
#include <fstream>
#include <iostream>
#include <thread>
#include "../renderer/iconNew.h"
#include "../renderer/imgui_internal.h"
#include <io.h>
#include <iostream>
#include <string>
#include <fstream>
#include <Windows.h>
#include <direct.h>
#include <filesystem>
// #include "../trigerbot.h"
#include <d3dx9.h>


LPDIRECT3DTEXTURE9 aimIconTexture = nullptr;
LPDIRECT3DTEXTURE9 miscIconTexture = nullptr;

LPDIRECT3DTEXTURE9 AimTexture = nullptr;
LPDIRECT3DTEXTURE9 MiscTexture = nullptr;
LPDIRECT3DTEXTURE9 ConfigTexture = nullptr;




bool LoadTextureFromMemory(LPDIRECT3DTEXTURE9& texture, unsigned char* data, int size) {
    HRESULT result = D3DXCreateTextureFromFileInMemory(gui::device, data, size, &texture);
    return result == D3D_OK;
}


void gui::LoadTextures() {

  

    D3DXCreateTextureFromFileInMemory(device, AimbotIcoNew, sizeof(AimbotIcoNew), &AimTexture);
    D3DXCreateTextureFromFileInMemory(device, MiscIcoNew, sizeof(MiscIcoNew), &MiscTexture);
    D3DXCreateTextureFromFileInMemory(device, ConfigIcoNew, sizeof(ConfigIcoNew), &ConfigTexture);



}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    switch (message)
    {
    case WM_SIZE: {
        if (gui::device && wideParameter != SIZE_MINIMIZED)
        {
            gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
            gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
            gui::ResetDevice();
        }
        return 0;
    }

    case WM_SYSCOMMAND: {
        if ((wideParameter & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        gui::position = MAKEPOINTS(longParameter);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (wideParameter == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(longParameter);
            auto rect = RECT{};

            GetWindowRect(gui::window, &rect);

            rect.left += points.x - gui::position.x;
            rect.top += points.y - gui::position.y;

            if (gui::position.x >= 0 &&
                gui::position.x <= gui::WIDTH &&
                gui::position.y >= 0 && gui::position.y <= 19)
                SetWindowPos(
                    gui::window,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }
        return 0;
    }
    }

    return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept {


    WNDCLASSEXA windowClass = { sizeof(WNDCLASSEXA), CS_CLASSDC, WindowProcess, 0L, 0L,
                                GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
                                "class001", nullptr };
    RegisterClassExA(&windowClass);

    window = CreateWindowExA(
        WS_EX_APPWINDOW,
        "class001",
        windowName,
        WS_POPUP,
        100,
        100,
        WIDTH,
        HEIGHT,
        0,
        0,
        windowClass.hInstance,
        0
    );

    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}





void gui::DestroyHWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClassA("class001", GetModuleHandle(nullptr));
}

bool gui::CreateDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParameters,
        &device) < 0)
        return false;

    return true;


}


void gui::ResetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}


void gui::DestroyDevice() noexcept
{
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (d3d)
    {
        d3d->Release();
        d3d = nullptr;
    }
}

void gui::CreateImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();


    io.Fonts->AddFontFromMemoryTTF(font_binary, sizeof(font_binary), 16.0f);

    io.IniFilename = NULL;
    gui::LoadTextures();

    ImGui::StyleColorsDark();

    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_WindowBg] = ImColor(241, 241, 241);
    style->Colors[ImGuiCol_Button] = ImColor(255, 0, 0);
    style->Colors[ImGuiCol_ButtonHovered] = ImColor(255, 0, 0);
    style->Colors[ImGuiCol_ButtonActive] = ImColor(255, 0, 0);
    style->Colors[ImGuiCol_CheckMark] = ImColor(255, 0, 0);
    style->Colors[ImGuiCol_FrameBg] = ImColor(30, 30, 30);
    style->Colors[ImGuiCol_FrameBgHovered] = ImColor(50, 50, 50);
    style->Colors[ImGuiCol_FrameBgActive] = ImColor(70, 70, 70);
    style->Colors[ImGuiCol_ChildBg] = ImColor(255, 255, 255);

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}



void gui::DestroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            isRunning = false;
            return;
        }
    }


    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0, 0, 0, 0);


    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}


int sliderValue = 50;
bool triggerbotEnabled = false;
bool headshotOnly = false;
bool gunEnabled = false;
float posX = 0.0f;
float posY = 0.0f;
float posZ = 0.0f;
bool visualsEnabled = false;

bool isRunning = true;


void ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 2.0f;
    float radius = height * 0.40f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;


    float t = *v ? 1.0f : 0.0f;
    ImGuiContext& g = *GImGui;
    float ANIM_SPEED = 0.12f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))
    {
        float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }


    ImU32 col_bg = IM_COL32(200, 200, 200, 255);


    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);


    float circleX = p.x + radius + t * (width - radius * 2.0f);
    float circleY = p.y + radius + 1.9f;


    ImU32 circleColor = *v ? IM_COL32(255, 0, 0, 255) : IM_COL32(155, 155, 155, 255);
    draw_list->AddCircleFilled(ImVec2(circleX, circleY), radius - 1.5f, circleColor);
}




namespace fs = std::filesystem;

struct Config {
    bool aimbotEnabled;
    int selectedHotkey;
    int sliderValue;
    bool safeConfigEnabled;
    bool visibilityCheckEnabled;
    int fovValue;


    void save(const std::string& filePath) {
        std::ofstream file(filePath);
        if (file.is_open()) {
            file << aimbotEnabled << '\n';
            file << selectedHotkey << '\n';
            file << sliderValue << '\n';
            file << safeConfigEnabled << '\n';

            file << fovValue << '\n';
            file.close();
        }
    }

    void load(const std::string& filePath) {
        std::ifstream file(filePath);
        if (file.is_open()) {
            file >> aimbotEnabled;
            file >> selectedHotkey;
            file >> sliderValue;
            file >> safeConfigEnabled;

            file >> fovValue;
            file.close();
        }
    }
};


#include <cstdlib>
#include <filesystem>
#include <mutex>

std::string getConfigFilePath() {
    char* buffer = nullptr;
    size_t size = 0;

    if (_dupenv_s(&buffer, &size, "USERPROFILE") == 0 && buffer != nullptr) {
        std::filesystem::path downloadsPath = std::filesystem::path(buffer) / "Downloads";
        free(buffer);
        return (downloadsPath / "config.txt").string();
    }

    if (buffer) {
        free(buffer);
    }

    return "";
}

bool CustomSliderFloat(const char* label, float* value, float min_value, float max_value) {
    ImGui::Text(label);


    float width = 150.0f;
    float height = 10.0f;

    ImVec2 pos = ImGui::GetCursorScreenPos();


    ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), ImGui::GetColorU32(ImVec4(0.843, 0.843, 0.843, 1.0)));


    float fill_ratio = (*value - min_value) / (max_value - min_value);


    ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + fill_ratio * width, pos.y + height), IM_COL32(0xFF, 0x00, 0x00, 0xFF));


    float grab_width = 5.0f;
    ImVec2 grab_pos(pos.x + fill_ratio * width - grab_width / 2, pos.y);
    ImGui::GetWindowDrawList()->AddRectFilled(grab_pos, ImVec2(grab_pos.x + grab_width, grab_pos.y + height), IM_COL32(0xFF, 0x00, 0x00, 0xFF));


    bool changed = ImGui::InvisibleButton(label, ImVec2(width, height));
    if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0]) {
        float mouse_pos = ImGui::GetIO().MousePos.x - pos.x;
        float new_value = min_value + (mouse_pos / width) * (max_value - min_value);
        *value = ImClamp(new_value, min_value, max_value);
        changed = true;
    }



    ImGui::SameLine();
    ImGui::Text("%.0f", *value);

    return changed;
}






void gui::Render() noexcept {
    static Config config;
    static bool configLoaded = false;


    if (!configLoaded) {
        config.load(getConfigFilePath());
        configLoaded = true;
    }


    ImVec4 backgroundColor = ImVec4(0.945f, 0.945f, 0.945f, 1.0f);
    ImVec4 childWindowColor = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    ImVec4 textColor = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    ImVec4 buttonColor = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    ImVec4 buttonHoverColor = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    ImVec4 neonRed = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);


    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 10.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 10.0f);


    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = backgroundColor;
    ImGui::GetStyle().Colors[ImGuiCol_ChildBg] = childWindowColor;
    ImGui::GetStyle().Colors[ImGuiCol_Text] = textColor;
    ImGui::GetStyle().Colors[ImGuiCol_Button] = buttonColor;
    ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = buttonHoverColor;
    ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = buttonColor;
    ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = buttonHoverColor;
    ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = neonRed;

    ImFontConfig font2_cfg;
    font2_cfg.PixelSnapH = true;

    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::SetNextWindowSize({ WIDTH, HEIGHT });

    ImGui::Begin(
        "EvoElite",
        &isRunning,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar
    );

    ImGui::BeginChild("LeftPanel", ImVec2(130, HEIGHT - 25), true);
    ImGui::Text(" ");
    static int activeTab = 0;


    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));



    ImGui::SetCursorPos(ImVec2(30, 30));
    if (ImGui::ImageButton((void*)AimTexture, ImVec2(75, 55))) {
        activeTab = 0;
    }

    

    ImGui::SetCursorPos(ImVec2(20, 120));
    if (ImGui::ImageButton((void*)ConfigTexture, ImVec2(75, 55))) {
        activeTab = 2;
    }



    ImGui::PopStyleColor(3);

    ImGui::EndChild();



    ImGui::SetCursorPos(ImVec2(WIDTH - 82.0f, 20.0f));
    ImGui::BeginChild("Close Window", ImVec2(40, 40), true, ImGuiWindowFlags_NoScrollbar);

    ImVec2 textSize = ImGui::CalcTextSize("X");
    ImGui::SetCursorPos(ImVec2((40 - textSize.x) * 0.5f, (40 - textSize.y) * 0.5f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
    ImGui::Text("X");
    ImGui::PopStyleColor();
    ImGui::EndChild();




    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        exit(0);
    }

    ImGui::SetCursorPos(ImVec2(130 + 37.8f, 20.0f));
    ImGui::BeginChild("Csgo 2 Name window", ImVec2(120, 40), true, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("CSGO 2");
    ImGui::EndChild();

    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);
    char timeStr[10];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &localTime);

    ImGui::SetCursorPos(ImVec2(270 + 37.8f, 20.0f));
    ImGui::BeginChild("platin , Elipapagei , real time window", ImVec2(210, 40), true);


    ImGui::Text("Platin");


    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
    ImGui::Text(" | EliAim");
    ImGui::PopStyleColor();


    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
    ImGui::Text(" | %s", timeStr);
    ImGui::PopStyleColor();

    ImGui::EndChild();




    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        exit(0);
    }

    if (activeTab == 0) {
        ImGui::SetCursorPos(ImVec2(130 + 37.8f, 76.0f));
        ImGui::BeginChild("Aimbot Settings", ImVec2(425, HEIGHT + 8 - 76.0f - 30.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::Text("                                            AIMBOT");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

       
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::BeginChild("Aimbot", ImVec2(250, 30 + ImGui::GetTextLineHeight() + 10.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);

        ImGui::Text("Enable Aimbot");

       
        ImGui::SameLine(ImGui::GetWindowWidth() - 70);
        ToggleButton("", &config.aimbotEnabled);

      
        if (config.aimbotEnabled) {
            aimbot::enable();
        }
        else {
            aimbot::disable();
        }

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
        ImGui::Text("Enable or disable the aimbot.");
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

      
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::BeginChild("vis check child", ImVec2(250, 30 + ImGui::GetTextLineHeight() + 10.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::Text("Enable Vis Check");

       
        ImGui::SameLine(ImGui::GetWindowWidth() - 70);
        ToggleButton("", &aimbot::visibilityCheck);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
        ImGui::Text("Enable Visibility Check.");
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

      
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));

        ImGui::BeginChild("FOVChild", ImVec2(250, 30 + ImGui::GetTextLineHeight() + 10.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::Text("FOV");

        ImGui::SameLine(0.0f, 10.0f);
        ImGui::PushItemWidth(120);
        CustomSliderFloat("", &aimbot::fovLimit, 1.0f, 180.0f);
        ImGui::PopItemWidth();

        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

       
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));

        ImGui::BeginChild("SpeedChild", ImVec2(250, 30 + ImGui::GetTextLineHeight() + 10.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::Text("Speed");

        ImGui::SameLine(0.0f, 10.0f);
        ImGui::PushItemWidth(120);
        CustomSliderFloat("", &aimbot::speedLimit, 1.0f, 100.0f);
        ImGui::PopItemWidth();

        ImGui::EndChild();

        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        

        ImGui::EndChild();
    



    }
    else if (activeTab == 2) {
        ImGui::SetCursorPos(ImVec2(130 + 37.8f, 76.0f));
        ImGui::BeginChild("cfgg child", ImVec2(425, HEIGHT + 8 - 76.0f - 30.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::Text("                                           Configs");


        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.945f, 0.945f, 0.945f, 1.0f));
        ImGui::BeginChild("cfg child", ImVec2(250, 30 + ImGui::GetTextLineHeight() + 10.0f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


        ImGui::Text("Enable Config.");

        ImGui::SameLine(ImGui::GetWindowWidth() - 70);
        ToggleButton("", &aimbot::visibilityCheck);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
        ImGui::Text("Enable or disable safe Config.");
        ImGui::PopStyleColor();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        if (config.safeConfigEnabled) {

            config.save(getConfigFilePath());
        }

        

const char* hotkeyOptions[] = { "Shift", "Mouse 4", "Mouse 5", "F1", "F2" };
ImGui::Combo("Aim Key", &config.selectedHotkey, hotkeyOptions, IM_ARRAYSIZE(hotkeyOptions));
ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.545f, 0.545f, 0.545f, 1.0f));
ImGui::Text("HotKey for Aimbot.");
ImGui::PopStyleColor();

 

        int hotkey = VK_SHIFT;
        switch (config.selectedHotkey) {
        case 0: hotkey = VK_SHIFT; break;
        case 1: hotkey = VK_XBUTTON1; break;
        case 2: hotkey = VK_XBUTTON2; break;
        case 3: hotkey = VK_F1; break;
        case 4: hotkey = VK_F2; break;
        }


        if (config.aimbotEnabled && GetAsyncKeyState(hotkey)) {
            aimbot::frame();
        }

        ImGui::EndChild();
    }
   

    ImGui::End();
    ImGui::PopStyleVar(6);
}



std::mutex botMutex;


void RunAimbot()
{
    aimbot::procID = memory::GetProcID(L"cs2.exe");
    aimbot::module_base = memory::GetModuleBaseAddress(aimbot::procID, L"client.dll");

    while (true)
    {
        if (GetAsyncKeyState(VK_F8))
        {
            break;
        }

        
        
        Sleep(1); 
    }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    std::thread aimbotThread(RunAimbot);

    gui::CreateHWindow("Nvz");
    if (!gui::CreateDevice()) {
        gui::DestroyHWindow();
        return 0;
    }

    gui::CreateImGui();

    while (gui::isRunning) {
        gui::BeginRender();
        gui::Render();
        gui::EndRender();
    }

    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    if (aimbotThread.joinable()) aimbotThread.join();
  

    return 0;
}


