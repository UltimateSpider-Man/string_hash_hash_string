#include <Windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>

// Function to load dictionary from file
std::vector<std::wstring> loadDictionary(const std::wstring& filename) {
    std::vector<std::wstring> dictionary;
    std::wifstream file(filename);
    if (file.is_open()) {
        std::wstring line;
        while (std::getline(file, line)) {
            dictionary.push_back(line);
        }
        file.close();
    }
    else {
        MessageBox(NULL, L"Unable to open dictionary file", L"Error", MB_ICONERROR);
    }
    return dictionary;
}

// Function to hash a string
unsigned int hashString(const std::wstring& str) {
    unsigned int hash = 0;
    for (wchar_t c : str) {
        int chLower = std::isalpha(c) ? std::tolower(c) : c;
        hash = chLower + 33 * hash;
    }
    return hash;
}

// Function to reverse hash using dictionary lookup
std::wstring reverseHash(unsigned int hashToFind, const std::vector<std::wstring>& dictionary) {
    for (const auto& str : dictionary) {
        if (hashString(str) == hashToFind) {
            return str;
        }
    }
    return L""; // Not found
}

// Global variables
std::vector<std::wstring> dictionary;
HWND hwndInput, hwndOutput1, hwndOutput2, hwndOutput3, hwndCopyInt, hwndCopyByte, hwndCopyIByte;
HWND hwndInputHash, hwndHashButton, hwndReverseHashButton;
// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        dictionary = loadDictionary(L"dictionary.txt");

        hwndInput = CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            20, 20, 300, 25,
            hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Input String:",
            WS_CHILD | WS_VISIBLE,
            20, 50, 300, 25,
            hwnd, NULL, NULL, NULL);

        hwndOutput1 = CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
            20, 80, 300, 25,
            hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Hash:",
            WS_CHILD | WS_VISIBLE,
            20, 110, 300, 25,
            hwnd, NULL, NULL, NULL);



        hwndInputHash = CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER,
            20, 130, 300, 25,
            hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Input Hash:",
            WS_CHILD | WS_VISIBLE,
            20, 160, 300, 25,
            hwnd, NULL, NULL, NULL);

        hwndOutput3 = CreateWindow(L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY,
            20, 190, 300, 25,
            hwnd, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"String from Hash:",
            WS_CHILD | WS_VISIBLE,
            20, 220, 300, 25,
            hwnd, NULL, NULL, NULL);

        hwndHashButton = CreateWindow(L"BUTTON", L"Hash!",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            330, 20, 75, 25,
            hwnd, (HMENU)4, NULL, NULL);

        hwndReverseHashButton = CreateWindow(L"BUTTON", L"Reverse Hash!",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            330, 130, 100, 25,
            hwnd, (HMENU)5, NULL, NULL);

        hwndCopyInt = CreateWindow(L"BUTTON", L"Copy",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            330, 80, 75, 25,
            hwnd, (HMENU)1, NULL, NULL);

        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == 4) {
            wchar_t inputBuffer[256];
            GetWindowText(hwndInput, inputBuffer, sizeof(inputBuffer) / sizeof(wchar_t));

            if (wcslen(inputBuffer) > 0) {
                unsigned int hashValue = hashString(inputBuffer);
                std::wstring output = std::to_wstring(hashValue);
                SetWindowText(hwndOutput1, output.c_str());

                std::wstring reversed = reverseHash(hashValue, dictionary);
                if (!reversed.empty()) {
                    SetWindowText(hwndOutput2, reversed.c_str());
                }
                else {
                    SetWindowText(hwndOutput2, L"Not found");
                }
            }
            else {
                SetWindowText(hwndOutput1, L"");
                SetWindowText(hwndOutput2, L"");
            }
        }
        else if (LOWORD(wParam) == 5) {
            wchar_t inputBuffer[256];
            GetWindowText(hwndInputHash, inputBuffer, sizeof(inputBuffer) / sizeof(wchar_t));

            if (wcslen(inputBuffer) > 0) {
                unsigned int hashValue = std::stoul(inputBuffer);
                std::wstring reversed = reverseHash(hashValue, dictionary);
                if (!reversed.empty()) {
                    SetWindowText(hwndOutput3, reversed.c_str());
                }
                else {
                    SetWindowText(hwndOutput3, L"Not found");
                }
            }
            else {
                SetWindowText(hwndOutput3, L"");
            }
        }
        else if (LOWORD(wParam) == 1) {
            wchar_t buffer[256];
            GetWindowText(hwndOutput1, buffer, sizeof(buffer) / sizeof(wchar_t));
            OpenClipboard(hwnd);
            EmptyClipboard();
            HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, (wcslen(buffer) + 1) * sizeof(wchar_t));
            wcscpy_s((wchar_t*)hGlob, wcslen(buffer) + 1, buffer);
            SetClipboardData(CF_UNICODETEXT, hGlob);
            CloseClipboard();
        }
        else if (LOWORD(wParam) == 2) {
            wchar_t buffer[256];
            GetWindowText(hwndOutput3, buffer, sizeof(buffer) / sizeof(wchar_t));
            OpenClipboard(hwnd);
            EmptyClipboard();
            HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, (wcslen(buffer) + 1) * sizeof(wchar_t));
            wcscpy_s((wchar_t*)hGlob, wcslen(buffer) + 1, buffer);
            SetClipboardData(CF_UNICODETEXT, hGlob);
            CloseClipboard();
        }
    
        else if (LOWORD(wParam) == 3) {
            // Currently no action assigned
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"HashTool";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Hash Tool",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 450, 300,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

int main() {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    HINSTANCE hInstance = GetModuleHandle(NULL);

    int result = wWinMain(hInstance, NULL, NULL, SW_SHOWDEFAULT);

    LocalFree(argv);
    return result;
}