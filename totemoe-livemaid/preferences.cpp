
#include "stdafx.h"
#include "controller.hpp"
#include "bili-https/bili-https.hpp"

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
        std::wstring sessionChksm = Bili::Settings::File::GetW("Session", "DedeUserID__ckMd5");
        std::wstring sessionData = Bili::Settings::File::GetW("Session", "SESSDATA");
        ::SetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID, sessionUID.c_str());
        ::SetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID__CKMD5, sessionChksm.c_str());
        ::SetDlgItemText(hDlg, IDC_EDIT_SESSDATA, sessionData.c_str());
        if (sessionUID.empty() || sessionChksm.empty() || sessionData.empty())
        {
            EnableWindow(GetDlgItem(hDlg, IDC_SESSION_VERIFY), FALSE);
        }
        else
        {
            EnableWindow(GetDlgItem(hDlg, IDC_SESSION_VERIFY), TRUE);
        }
        return TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_EDIT_DEDEUSERID ||
            LOWORD(wParam) == IDC_EDIT_DEDEUSERID__CKMD5 ||
            LOWORD(wParam) == IDC_EDIT_SESSDATA)
        {
            if (HIWORD(wParam) == EN_UPDATE)
            {
                bool allNonEmpty = true;
                allNonEmpty &= ::GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_DEDEUSERID)) > 0;
                allNonEmpty &= ::GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_DEDEUSERID__CKMD5)) > 0;
                allNonEmpty &= ::GetWindowTextLength(GetDlgItem(hDlg, IDC_EDIT_SESSDATA)) > 0;
                if (allNonEmpty)
                {
                    EnableWindow(GetDlgItem(hDlg, IDC_SESSION_VERIFY), TRUE);
                }
                else
                {
                    EnableWindow(GetDlgItem(hDlg, IDC_SESSION_VERIFY), FALSE);
                }
                return TRUE;
            }
        }
        else if (LOWORD(wParam) == IDC_SESSION_VERIFY)
        {
            if (HIWORD(wParam) == BN_CLICKED)
            {
                Bili::User::Credentials cred;
                WCHAR szContent[MAX_LOADSTRING];
                std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

                ::GetWindowText(GetDlgItem(hDlg, IDC_EDIT_DEDEUSERID),
                    szContent, MAX_LOADSTRING);
                cred.DedeUserID = converter.to_bytes(szContent);

                ::GetWindowText(GetDlgItem(hDlg, IDC_EDIT_DEDEUSERID__CKMD5),
                    szContent, MAX_LOADSTRING);
                cred.DedeUserID__ckMd5 = converter.to_bytes(szContent);

                ::GetWindowText(GetDlgItem(hDlg, IDC_EDIT_SESSDATA),
                    szContent, MAX_LOADSTRING);
                cred.SESSDATA = converter.to_bytes(szContent);

                auto response = Bili::User::GetSignInInfo(cred);
                auto wResponse = converter.from_bytes(response.dump());

                ResourceString title(
                    I18N::GetHandle(), IDS_PROPPAGE_SESSION_RESULT);
                if (response.is_object() && 
                    response.find("error") == response.end())
                {
                    ResourceString success(
                        I18N::GetHandle(), IDS_PROPPAGE_SESSION_SUCCESS);
                    ::MessageBox(hDlg, (LPCWSTR)success, (LPCWSTR)title, MB_OK);
                }
                else
                {
                    ResourceString failure(
                        I18N::GetHandle(), IDS_PROPPAGE_SESSION_FAIL);
                    std::wstringstream wss;
                    wss << (LPCWSTR)failure << wResponse;
                    std::wstring result = wss.str();
                    ::MessageBox(hDlg, result.c_str(), (LPCWSTR)title, MB_OK);
                }
            }
        }
        break;

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
    }
    return DefWindowProc(hDlg, message, wParam, lParam);
}