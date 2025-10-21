#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <chrono>
#include "OdbcError.h"



static std::string to_utf8_from_w(const SQLWCHAR* wstr, int len = -1) {
    if (!wstr) return {};
    int needed = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(wstr), len, nullptr, 0, nullptr, nullptr);   // retourne le nombre d'octet �cris dans la m�moire tampon qu'on utilisera plus tard pour cr�er un string sur m�sure
    if (needed <= 0) return {};
    std::string s(needed, '\0');
    int ret = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(wstr), len, &s[0], needed, nullptr, nullptr);
    if (ret == 0)
    {
        throw OdbcError("WideCharToMultiByte failed to convert const SQLWCHAR* to std::string(utf-8)");
    }
    if (!s.empty() && s.back() == '\0') s.pop_back();
    return s;
}

// utilitaire de conversion UTF-8 -> UTF-16 (std::string -> std::wstring)
static std::wstring to_wstring_utf8(const std::string& s) {
    if (s.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    if (len <= 0) return {};
    std::wstring w(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], len);
    if (!w.empty() && w.back() == L'\0') w.pop_back();
    return w;
}

static std::string CollectDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle)
{
    std::string out;
    SQLSMALLINT recNumber = 1;

    while (true) {
        // ces variables ont besoin d'�tre red�clarer � la fin d'une it�ration ou soit on les sorte et on les r�initialise � la fin
        SQLWCHAR sqlState[6] = {};
        SQLINTEGER nativeError = 0;
        SQLWCHAR message[1024] = {};
        SQLSMALLINT textLength = 0;

        SQLRETURN diagRet = SQLGetDiagRecW(  // �viter d'utiliser SQLGetDiagRec car on travaille avec des caract�res unicode
            handleType,
            handle,
            recNumber,
            sqlState,
            &nativeError,
            message,
            static_cast<SQLSMALLINT>(std::size(message)),
            &textLength
        );

        if (diagRet == SQL_NO_DATA) break;

        if (diagRet == SQL_SUCCESS || diagRet == SQL_SUCCESS_WITH_INFO) {  // cas o� la fonction r�ussi ou renvoie en plus un message
            out += "[";
            out.append(to_utf8_from_w(sqlState, -1));
            out.append("] ");
            out.append(to_utf8_from_w(message, textLength));
            out.append(" (native=" + std::to_string(nativeError) + ")\n");
        }
        else {
            out += "Erreur r�cup�ration diagnostics, code=" + std::to_string(static_cast<int>(diagRet)) + "\n";
            break;
        }

        ++recNumber;
    }

    return out;
}


static void logDetailedDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle) {
    SQLSMALLINT recNumber = 1;
    while (true) {
        SQLWCHAR sqlState[6] = {};
        SQLINTEGER nativeError = 0;
        SQLWCHAR message[1024] = {};
        SQLSMALLINT textLength = 0;

        SQLRETURN diagRet = SQLGetDiagRecW(
            handleType,
            handle,
            recNumber,
            sqlState,
            &nativeError,
            message,
            static_cast<SQLSMALLINT>(std::size(message)),
            &textLength
        );

        if (diagRet == SQL_NO_DATA) break;

        if (diagRet == SQL_SUCCESS || diagRet == SQL_SUCCESS_WITH_INFO) {
            std::string state = to_utf8_from_w(sqlState, -1);
            std::string msg = to_utf8_from_w(message, textLength);
            std::cout << "ODBC Diagnostic [" << recNumber << "] SQLSTATE=" << state
                << " NativeError=" << nativeError << " Message=\"" << msg << "\"" << std::endl;
        }
        else {
            std::cout << "ODBC Diagnostic: �chec r�cup�ration diag recNumber=" << recNumber
                << " code=" << static_cast<int>(diagRet) << std::endl;
            break;
        }
        ++recNumber;
    }
}


// ajout des fonctions de conversion de std::chrono::system_clock vers DATE_STRUCT

DATE_STRUCT FromSystemClockToDATE_STRUCT(std::chrono::year_month_day date)
{
    DATE_STRUCT sql_date;
    sql_date.day = unsigned(date.day());

}
