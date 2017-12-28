
#include "stdafx.h"

#include "controller.hpp"

INT_PTR CALLBACK PreferencesDlgProc(HWND hDlg, UINT message, LPARAM lParam)
{
    switch (message)
    {
    case PSCB_INITIALIZED:
        return FALSE;

    case WM_COMMAND:
        break;

    case PSCB_BUTTONPRESSED:
    {
        switch (lParam)
        {
        case PSBTN_OK:

            // Bili::Settings::File::Save();
            ::MessageBox(hDlg, L"LOL", L"FAK", MB_OK);
            break;
        case PSBTN_CANCEL:
            break;
        case PSBTN_APPLYNOW:
            break;
        case PSBTN_FINISH:
            break;
        }
    }
    break;

    case WM_DESTROY:
        return TRUE;
    }
    return DefWindowProc(hDlg, message, NULL, lParam);
}

INT_PTR CALLBACK I18N_PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hLanguage = GetDlgItem(hDlg, IDC_COMBO_I18N_LANGUAGE);
        ResourceString system(I18N::GetHandle(), IDS_PROPPAGE_I18N_LANGUAGE_SYSTEM);

        std::vector<std::wstring> names;

        // EnumUILanguages([](LPTSTR lpUILanguageString, LONG_PTR lParam) -> BOOL {
        //     auto *pNames = (std::vector<std::wstring> *)lParam;
        //     pNames->push_back(lpUILanguageString);
        //     return TRUE;
        // }, MUI_LANGUAGE_NAME, (LONG_PTR)&names);

        for (auto &language : names)
        {
            SendMessage(hLanguage, CB_ADDSTRING, NULL, (LPARAM)language.c_str());
        }

        SendMessage(hLanguage, CB_ADDSTRING, NULL, (LPARAM)((LPCWSTR)system));
        SendMessage(hLanguage, CB_SETCURSEL, (WPARAM)(names.size()), NULL);

        return TRUE;
    }

    case WM_NOTIFY:
    {
        NMHDR *info = (NMHDR *)lParam;
        switch (info->code)
        {
        case PSN_KILLACTIVE:
        {
            ::SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
            return TRUE;
        }

        case PSN_APPLY:
        {
            ::SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
            return TRUE;
        }
        }
        return FALSE;
    }
    }
    return DefWindowProc(hDlg, message, wParam, lParam);
}

INT_PTR CALLBACK Session_PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // Populate session data from settings.
        std::wstring sessionUID = Bili::Settings::File::GetW("Session", "DedeUserID");
        std::wstring sessionChecksum = Bili::Settings::File::GetW("Session", "DedeUserID__ckMd5");
        std::wstring sessionSessData = Bili::Settings::File::GetW("Session", "SESSDATA");
        ::SetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID, sessionUID.c_str());
        ::SetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID__CKMD5, sessionChecksum.c_str());
        ::SetDlgItemText(hDlg, IDC_EDIT_SESSDATA, sessionSessData.c_str());
        return TRUE;
    }

    case WM_NOTIFY:
    {
        NMHDR *info = (NMHDR *)lParam;
        switch (info->code)
        {
        case PSN_KILLACTIVE:
        {
            WCHAR szText[MAX_LOADSTRING];
            ::GetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID, szText, MAX_LOADSTRING);
            std::wstring sessionUID = szText;
            if (sessionUID.empty())
            {
                ::SetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID__CKMD5, L"");
                ::SetDlgItemText(hDlg, IDC_EDIT_SESSDATA, L"");
            }
            ::SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
            return TRUE;
        }

        case PSN_APPLY:
        {
            WCHAR szText[MAX_LOADSTRING];
            ::GetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID, szText, MAX_LOADSTRING);
            Bili::Settings::File::SetW("Session", "DedeUserID", szText);
            ::GetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID__CKMD5, szText, MAX_LOADSTRING);
            Bili::Settings::File::SetW("Session", "DedeUserID__ckMd5", szText);
            ::GetDlgItemText(hDlg, IDC_EDIT_SESSDATA, szText, MAX_LOADSTRING);
            Bili::Settings::File::SetW("Session", "SESSDATA", szText);
            ::SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
            // ::SendMessage(hDlg, PSM_CANCELTOCLOSE, NULL, NULL);
            return TRUE;
        }
        }
        return FALSE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            // EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return DefWindowProc(hDlg, message, wParam, lParam);
}