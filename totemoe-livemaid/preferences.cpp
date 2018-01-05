
#include "stdafx.h"
#include "controller.hpp"
#include "resource-i18n.hpp"
#include "Windowsx.h"
#include <iomanip>
#include "bili-https/bili-https.hpp"

INT_PTR CALLBACK I18N_PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // Configure display language.
        HWND hLanguage = GetDlgItem(hDlg, IDC_COMBO_I18N_LANGUAGE);
        std::vector<std::wstring> names;
        {
            ResourceString system(I18N::GetHandle(), IDS_PROPPAGE_I18N_LANGUAGE_SYSTEM);
            ComboBox_AddString(hLanguage, (LPCWSTR)system);
        }
        for (auto &language : names)
        {
            ComboBox_AddString(hLanguage, language.c_str());
        }
        SendMessage(hLanguage, CB_SETCURSEL, (WPARAM)(names.size()), NULL);

        // Configure Time Zone.
        std::wstring timeZoneSetting = Bili::Settings::Get(L"General", L"timeZone");
        HWND hTimeZone = GetDlgItem(hDlg, IDC_COMBO_I18N_TIMEZONE);
        {
            ResourceString system(I18N::GetHandle(), IDS_PROPPAGE_I18N_TIMEZONE_SYSTEM);
            ComboBox_AddString(hTimeZone, (LPCWSTR)system);
            if (timeZoneSetting == L"default")
            {
                ComboBox_SetCurSel(hTimeZone, 0);
            }
        }
        HKEY key;
        std::wstring subKey(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones");
        RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey.c_str(), NULL, KEY_READ, &key);
        DWORD dwIndex = 0;
        WCHAR szContent[MAX_LOADSTRING];
        LONG retCode = ERROR_MORE_DATA;
        std::vector<std::wstring> timeZones;
        while (retCode != ERROR_NO_MORE_ITEMS)
        {
            DWORD dwSize = MAX_LOADSTRING;
            retCode = RegEnumKeyEx(key, dwIndex, szContent, &dwSize,
                NULL, NULL, NULL, NULL);
            DWORD dwType;
            WCHAR szDisplay[MAX_LOADSTRING];
            DWORD cbData = MAX_LOADSTRING;
            if (retCode == ERROR_SUCCESS)
            {
                retCode = RegGetValue(key, szContent, L"Display",
                    RRF_RT_ANY, &dwType, szDisplay, &cbData);
                if (retCode == ERROR_SUCCESS)
                {
                    timeZones.push_back(szDisplay);
                }
            }
            dwIndex++;
        }
        for (auto &timeZone : timeZones)
        {
            ComboBox_AddString(hTimeZone, timeZone.c_str());
        }
        int iTZ;
        if (timeZoneSetting == L"system")
        {
            ResourceString system(I18N::GetHandle(), IDS_PROPPAGE_I18N_TIMEZONE_SYSTEM);
            iTZ = ComboBox_FindString(hTimeZone, -1, (LPCWSTR)system);
        }
        else
        {
            iTZ = ComboBox_FindString(hTimeZone, -1, timeZoneSetting.c_str());
        }
        ComboBox_SetCurSel(hTimeZone, iTZ);

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
            WCHAR szContent[MAX_LOADSTRING];

            // Set time zone settings.
            ::GetWindowText(GetDlgItem(hDlg, IDC_COMBO_I18N_TIMEZONE),
                szContent, MAX_LOADSTRING);
            std::wstring timeZoneSetting = szContent;
            ResourceString sysTimeZone(I18N::GetHandle(), IDS_PROPPAGE_I18N_TIMEZONE_SYSTEM);
            if (timeZoneSetting == (LPCWSTR)sysTimeZone)
            {
                Bili::Settings::Set(L"General", L"timeZone", L"system");
            }
            else
            {
                Bili::Settings::Set(L"General", L"timeZone", szContent);
            }

            ::SetWindowLong(hDlg, DWL_MSGRESULT, PSNRET_NOERROR);
            return TRUE;
        }
        }
        return FALSE;
    }
    }
    return DefWindowProc(hDlg, message, wParam, lParam);
}

INT_PTR CALLBACK Danmaku_PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // Get handles to protocol filtering sliders.
        HWND hFilterDanmaku = ::GetDlgItem(hDlg, IDC_SLIDER_DANMAKU);
        HWND hFilterGifting = ::GetDlgItem(hDlg, IDC_SLIDER_GIFTING);
        HWND hFilterAnnouncement = ::GetDlgItem(hDlg, IDC_SLIDER_ANNOUNCEMENT);
        HWND hFilterUnknown = ::GetDlgItem(hDlg, IDC_SLIDER_UNKNOWN);

        // Obtain handles to the rest of the checkboxes.
        HWND hFilterRegex = ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_REGEX);
        HWND hFilterSmallTV = ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_SMALLTV);
        HWND hExport = ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_EXPORT);
        HWND hExportCSV = ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_EXPORT_CSV);

        int mint = 0;
        int maxt = 3;
        // Initiate sliders to having four ticks only.
        ::SendMessage(hFilterDanmaku, TBM_SETRANGE, TRUE, MAKELPARAM(mint, maxt));
        ::SendMessage(hFilterGifting, TBM_SETRANGE, TRUE, MAKELPARAM(mint, maxt));
        ::SendMessage(hFilterAnnouncement, TBM_SETRANGE, TRUE, MAKELPARAM(mint, maxt));
        ::SendMessage(hFilterUnknown, TBM_SETRANGE, TRUE, MAKELPARAM(mint, maxt));

        int position;
        // Set the sliders.
        try
        {
            auto p = Bili::Settings::Get(L"Danmaku", L"filterDanmaku");
            position = std::stoi(p);
        }
        catch (...)
        {
            auto p = Bili::Settings::GetDefault(L"Danmaku", L"filterDanmaku");
            position = std::stoi(p);
        }
        ::SendMessage(hFilterDanmaku, TBM_SETPOS, TRUE, (LPARAM)position);

        try
        {
            auto p = Bili::Settings::Get(L"Danmaku", L"filterGifting");
            position = std::stoi(p);
        }
        catch (...)
        {
            auto p = Bili::Settings::GetDefault(L"Danmaku", L"filterGifting");
            position = std::stoi(p);
        }
        ::SendMessage(hFilterGifting, TBM_SETPOS, TRUE, (LPARAM)position);

        try
        {
            auto p = Bili::Settings::Get(L"Danmaku", L"filterAnnouncement");
            position = std::stoi(p);
        }
        catch (...)
        {
            auto p = Bili::Settings::GetDefault(L"Danmaku", L"filterAnnouncement");
            position = std::stoi(p);
        }
        ::SendMessage(hFilterAnnouncement, TBM_SETPOS, TRUE, (LPARAM)position);

        try
        {
            auto p = Bili::Settings::Get(L"Danmaku", L"filterUnknown");
            position = std::stoi(p);
        }
        catch (...)
        {
            auto p = Bili::Settings::GetDefault(L"Danmaku", L"filterUnknown");
            position = std::stoi(p);
        }
        ::SendMessage(hFilterUnknown, TBM_SETPOS, TRUE, (LPARAM)position);


        std::wstring filterRegex = Bili::Settings::Get(L"Danmaku", L"filterRegex");
        if (filterRegex == L"on")
        {
            Button_SetCheck(hFilterRegex, TRUE);
        }
        std::wstring filterSmallTV = Bili::Settings::Get(L"Danmaku", L"filterSmallTV");
        if (filterSmallTV == L"on")
        {
            Button_SetCheck(hFilterSmallTV, TRUE);
        }
        std::wstring autoExport = Bili::Settings::Get(L"Danmaku", L"autoExport");
        if (autoExport == L"on" || autoExport == L"csv")
        {
            Button_SetCheck(hExport, TRUE);
            if (autoExport == L"csv")
            {
                Button_SetCheck(hExportCSV, TRUE);
            }
        }
        else
        {
            Button_Enable(hExportCSV, FALSE);
        }

        return TRUE;
    }

    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            if (LOWORD(wParam) == IDC_CHECK_DANMAKU_EXPORT)
            {
                HWND hExportCSV = GetDlgItem(hDlg, IDC_CHECK_DANMAKU_EXPORT_CSV);
                if (Button_GetCheck(GetDlgItem(hDlg, IDC_CHECK_DANMAKU_EXPORT)))
                {
                    Button_Enable(hExportCSV, TRUE);
                }
                else
                {
                    Button_Enable(hExportCSV, FALSE);
                    Button_SetCheck(hExportCSV, FALSE);
                }
            }
        }
        break;
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

            ::SetWindowLong(hDlg, DWL_MSGRESULT, FALSE);
            return TRUE;
        }

        case PSN_APPLY:
        {
            // Get handles to protocol filtering slider controls.
            HWND hFilterDanmaku =
                ::GetDlgItem(hDlg, IDC_SLIDER_DANMAKU);
            HWND hFilterGifting =
                ::GetDlgItem(hDlg, IDC_SLIDER_GIFTING);
            HWND hFilterAnnouncement =
                ::GetDlgItem(hDlg, IDC_SLIDER_ANNOUNCEMENT);
            HWND hFilterUnknown =
                ::GetDlgItem(hDlg, IDC_SLIDER_UNKNOWN);

            // Get handles to other checkboxes.
            HWND hFilterRegex =
                ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_REGEX);
            HWND hFilterSmallTV =
                ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_SMALLTV);
            HWND hExport =
                ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_EXPORT);
            HWND hExportCSV =
                ::GetDlgItem(hDlg, IDC_CHECK_DANMAKU_EXPORT_CSV);

            std::wstring filterLevel;
            // Record protocol filtering levels.
            filterLevel = std::to_wstring(
                ::SendMessage(hFilterDanmaku, TBM_GETPOS, NULL, NULL));
            Bili::Settings::Set(
                L"Danmaku", L"filterDanmaku", filterLevel.c_str());

            filterLevel = std::to_wstring(
                ::SendMessage(hFilterGifting, TBM_GETPOS, NULL, NULL));
            Bili::Settings::Set(
                L"Danmaku", L"filterGifting", filterLevel.c_str());

            filterLevel = std::to_wstring(
                ::SendMessage(hFilterAnnouncement, TBM_GETPOS, NULL, NULL));
            Bili::Settings::Set(
                L"Danmaku", L"filterAnnouncement", filterLevel.c_str());

            filterLevel = std::to_wstring(
                ::SendMessage(hFilterUnknown, TBM_GETPOS, NULL, NULL));
            Bili::Settings::Set(
                L"Danmaku", L"filterUnknown", filterLevel.c_str());

            if (::SendMessage(hFilterRegex, BM_GETCHECK, NULL, NULL))
            {
                Bili::Settings::Set(L"Danmaku", L"filterRegex", L"on");
            }
            else
            {
                Bili::Settings::Set(L"Danmaku", L"filterRegex", L"off");
            }

            if (::SendMessage(hFilterSmallTV, BM_GETCHECK, NULL, NULL))
            {
                Bili::Settings::Set(L"Danmaku", L"filterSmallTV", L"on");
            }
            else
            {
                Bili::Settings::Set(L"Danmaku", L"filterSmallTV", L"off");
            }

            if (::SendMessage(hExport, BM_GETCHECK, NULL, NULL))
            {
                if (::SendMessage(hExportCSV, BM_GETCHECK, NULL, NULL))
                {
                    Bili::Settings::Set(L"Danmaku", L"autoExport", L"csv");
                }
                else
                {
                    Bili::Settings::Set(L"Danmaku", L"autoExport", L"on");
                }
            }
            else
            {
                Bili::Settings::Set(L"Danmaku", L"autoExport", L"off");
            }

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
        std::wstring sessionUID = Bili::Settings::Get(L"Session", L"DedeUserID");
        std::wstring sessionChksm = Bili::Settings::Get(L"Session", L"DedeUserID__ckMd5");
        std::wstring sessionData = Bili::Settings::Get(L"Session", L"SESSDATA");
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
            Bili::Settings::Set(L"Session", L"DedeUserID", szText);
            ::GetDlgItemText(hDlg, IDC_EDIT_DEDEUSERID__CKMD5, szText, MAX_LOADSTRING);
            Bili::Settings::Set(L"Session", L"DedeUserID__ckMd5", szText);
            ::GetDlgItemText(hDlg, IDC_EDIT_SESSDATA, szText, MAX_LOADSTRING);
            Bili::Settings::Set(L"Session", L"SESSDATA", szText);
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