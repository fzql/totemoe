// totemoe-chat.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "controller.hpp"
#include "livemaid.h"
#include "resource-i18n.hpp"
#include "win-main.hpp"

// Current instance.
HINSTANCE hInst;

// Controller
Controller *pCtrl;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // Suppress warnings due to unreferenced parameters.
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    try
    {
        // Load Initial Settings.
        Bili::Settings::File::Load();
        // Load Resource String.
        I18N::Initialize();

        // Register window class.
        WinMainClass mainWndClass(WndProc, IDC_TOTEMOELIVEMAID, hInstance);
        HWND hWndOther = mainWndClass.GetRunningWindow();
        // Is there a running instance of this program?
        // if (hWndOther != nullptr)
        // {
        //     ::SetForegroundWindow(hWndOther);
        //     if (::IsIconic(hWndOther))
        //     {
        //         ::ShowWindow(hWndOther, SW_RESTORE);
        //     }
        //     return 0;
        // }
        mainWndClass.Register();

        // Initialize and show window.
        WinMainMaker mainWindow(mainWndClass, IDS_APP_TITLE);
        mainWindow.Create();
        mainWindow.Show(nCmdShow);

        // Load acceleration table.
        HACCEL hAccelTable =
            LoadAccelerators(I18N::GetHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR));

        /*
        WIN32_FIND_DATA ffd;
        DWORD dwError;
        HANDLE hFind = FindFirstFile(
            TOTEMOE_PLUGIN_CONTRIVED_FOLDER_PATH L"*.dll", &ffd);
        if (INVALID_HANDLE_VALUE == hFind)
        {
            throw WinException(L"FindFirstFile");
        }
        // List all the files in the directory with some info about them.
        do
        {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                ::MessageBox((HWND)mainWindow, ffd.cFileName, L"A A", MB_OK);
            }
            else
            {
                LARGE_INTEGER filesize;
                filesize.LowPart = ffd.nFileSizeLow;
                filesize.HighPart = ffd.nFileSizeHigh;
                ::MessageBox((HWND)mainWindow, ffd.cFileName, L"A B", MB_OK);
            }
        } while (FindNextFile(hFind, &ffd) != 0);
        dwError = GetLastError();
        if (dwError != ERROR_NO_MORE_FILES)
        {
            throw WinException(L"FindFirstFile");
        }
        FindClose(hFind);
        */

        MSG msg;
        BOOL status;

        // Main message loop:
        while (status = ::GetMessage(&msg, nullptr, 0, 0))
        {
            if (status == -1)
            {
                return -1;
            }
            if (!(pCtrl && PropSheet_IsDialogMessage(pCtrl->getPropertySheet(), &msg)))
            {
                if (!TranslateAccelerator((HWND)mainWindow, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else if (pCtrl && PropSheet_GetCurrentPageHwnd(pCtrl->getPropertySheet()) == NULL)
            {
                ::DestroyWindow(pCtrl->getPropertySheet());
                Bili::Settings::File::Save();
            }
        }
        return (int)msg.wParam;
    }
    catch (WinException e)
    {
        WCHAR buf[MAX_LOADSTRING];
        wsprintf(buf, L"%s, Error %d", e.GetMessage(), e.GetError());
        ::MessageBox(0, buf, L"Exception", MB_ICONEXCLAMATION | MB_OK);
    }
    catch (...)
    {
        ::MessageBox(0, L"Unknown", L"Exception",
            MB_ICONEXCLAMATION | MB_OK);
    }

    I18N::Free();
    return 0;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    pCtrl = WinGetLong<Controller *>(hWnd);

    /*
    if (pCtrl != nullptr && PropSheet_IsDialogMessage(pCtrl->getPropSheetHandle(), )
    {

    }
    */

    switch (message)
    {
    case WM_CREATE:
        try
        {
            pCtrl = new Controller(hWnd, reinterpret_cast<CREATESTRUCT *>(lParam));
            WinSetLong<Controller *>(hWnd, pCtrl);
        }
        catch (WinException &e)
        {
            ::MessageBox(hWnd, e.GetMessage(), L"Initialization",
                MB_ICONEXCLAMATION | MB_OK);
            return -1;
        }
        catch (...)
        {
            ::MessageBox(hWnd, L"Unknown Error", L"Initialization",
                MB_ICONEXCLAMATION | MB_OK);
            return -1;
        }
        return 0;
    case WM_NOTIFY:
        pCtrl->notify((LPNMHDR)lParam);
        break;
    case WM_SIZE:
        pCtrl->size(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_PAINT:
        pCtrl->paint();
        break;
    case WM_COMMAND:
        pCtrl->command(LOWORD(wParam));
        break;
    case WM_SET_ROOM:
        pCtrl->setRoom((ROOM)lParam);
        break;
    case WM_CONNECT:
        pCtrl->connect();
        break;
    case WM_DISCONNECT:
        pCtrl->disconnect();
        break;
    case WM_COMMANDLINE_ENTER:
        pCtrl->submit();
        break;
    case WM_COMMANDTYPE_CYCLE:
        pCtrl->cycleMode(lParam);
        break;
    case WM_GET_SESSIONCOLUMN:
        pCtrl->getActiveSessionColumn((int)wParam, (int)lParam);
        break;
    case WM_INITMENUPOPUP:
        pCtrl->initMenu((HMENU)wParam);
        break;
    case WM_DESTROY:
        WinSetLong<Controller *>(hWnd, nullptr);
        delete pCtrl;
        return 0;
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
