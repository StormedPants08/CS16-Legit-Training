#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commdlg.h>
#include <string>
#include <fstream>
#include <sstream>
#include <shellapi.h>

#pragma comment(lib, "Comdlg32.lib")

static HWND hSens, hZoom, hDynamic, hStatus;
static const wchar_t* APP_TITLE = L"CS 1.6 - Legit Training Panel";

std::wstring GetText(HWND h) {
    int n = GetWindowTextLengthW(h);
    std::wstring s(n + 1, L'\0');
    GetWindowTextW(h, &s[0], n + 1);
    s.resize(n);
    return s;
}
void SetStatus(const std::wstring& s) {
    SetWindowTextW(hStatus, s.c_str());
}
void AddLabel(HWND parent, const wchar_t* text, int x, int y, int w=210) {
    CreateWindowW(L"STATIC", text, WS_CHILD|WS_VISIBLE, x,y,w,24,parent,nullptr,nullptr,nullptr);
}
void SaveConfig(HWND hwnd) {
    std::wstring sens = GetText(hSens), zoom = GetText(hZoom);
    if (sens.empty()) sens = L"2.5";
    if (zoom.empty()) zoom = L"1.0";

    // Basic numeric validation to avoid writing arbitrary console commands.
    try {
        double a = std::stod(sens), b = std::stod(zoom);
        if (a < 0.1 || a > 20.0 || b < 0.1 || b > 5.0) throw 1;
    } catch (...) {
        MessageBoxW(hwnd, L"Use sensibilidade entre 0.1 e 20 e zoom entre 0.1 e 5.", APP_TITLE, MB_ICONWARNING);
        return;
    }

    wchar_t path[MAX_PATH] = L"autoexec_training.cfg";
    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Counter-Strike config (*.cfg)\0*.cfg\0Todos os arquivos\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = L"cfg";
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

    if (!GetSaveFileNameW(&ofn)) return;

    std::wofstream f(path);
    if (!f) {
        MessageBoxW(hwnd, L"Nao foi possivel salvar o arquivo.", APP_TITLE, MB_ICONERROR);
        return;
    }
    f << L"// CS 1.6 Legit Training - generated settings\n";
    f << L"sensitivity \"" << sens << L"\"\n";
    f << L"zoom_sensitivity_ratio \"" << zoom << L"\"\n";
    f << L"cl_dynamiccrosshair \"" << (SendMessageW(hDynamic, BM_GETCHECK,0,0)==BST_CHECKED ? 1 : 0) << L"\"\n";
    f << L"echo ^\"Legit Training config loaded^\"\n";
    f.close();
    SetStatus(L"Configuracao salva com sucesso.");
    MessageBoxW(hwnd, L"Arquivo CFG salvo. Coloque-o na pasta cstrike e execute no console: exec autoexec_training.cfg", APP_TITLE, MB_ICONINFORMATION);
}
void OpenFolder(HWND hwnd) {
    wchar_t path[MAX_PATH]{};
    if (GetCurrentDirectoryW(MAX_PATH, path)) {
        ShellExecuteW(hwnd, L"open", path, nullptr, nullptr, SW_SHOWNORMAL);
    }
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch(msg) {
    case WM_CREATE:
        AddLabel(hwnd, L"Sensibilidade (0.1 - 20.0)", 24, 24, 260);
        hSens = CreateWindowW(L"EDIT", L"2.5", WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL, 300,20,120,28,hwnd,(HMENU)101,nullptr,nullptr);
        AddLabel(hwnd, L"Zoom sensitivity (0.1 - 5.0)", 24, 68, 260);
        hZoom = CreateWindowW(L"EDIT", L"1.0", WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL, 300,64,120,28,hwnd,(HMENU)102,nullptr,nullptr);
        hDynamic = CreateWindowW(L"BUTTON", L"Desativar crosshair dinamica", WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, 24,112,300,28,hwnd,(HMENU)103,nullptr,nullptr);
        SendMessageW(hDynamic, BM_SETCHECK, BST_CHECKED, 0);
        CreateWindowW(L"BUTTON", L"Salvar arquivo CFG", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 24,164,190,36,hwnd,(HMENU)201,nullptr,nullptr);
        CreateWindowW(L"BUTTON", L"Abrir pasta atual", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 230,164,190,36,hwnd,(HMENU)202,nullptr,nullptr);
        hStatus = CreateWindowW(L"STATIC", L"Pronto. Ajuste os valores e salve seu CFG.", WS_CHILD|WS_VISIBLE, 24,220,410,28,hwnd,nullptr,nullptr,nullptr);
        break;
    case WM_COMMAND:
        if (LOWORD(wp)==201) SaveConfig(hwnd);
        if (LOWORD(wp)==202) OpenFolder(hwnd);
        break;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
int WINAPI wWinMain(HINSTANCE inst, HINSTANCE, PWSTR, int show) {
    const wchar_t CLASS_NAME[] = L"CS16TrainingPanelWindow";
    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClassW(&wc);
    HWND hwnd = CreateWindowExW(0, CLASS_NAME, APP_TITLE, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        CW_USEDEFAULT,CW_USEDEFAULT,480,310,nullptr,nullptr,inst,nullptr);
    if (!hwnd) return 1;
    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);
    MSG msg{};
    while (GetMessageW(&msg,nullptr,0,0)>0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    return 0;
}
