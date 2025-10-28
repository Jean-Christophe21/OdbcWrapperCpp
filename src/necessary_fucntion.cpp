#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include "necessary_function.h"


// fonction pour convertir une chaine utf16 en utf16  (std::wstring -> std::string)
std::string to_utf8_from_w(const SQLWCHAR* wstr, int len) {
    if (!wstr) return {};
    int needed = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<const wchar_t*>(wstr), len, nullptr, 0, nullptr, nullptr);   // retourne le nombre d'octet écris dans la mémoire tampon qu'on utilisera plus tard pour créer un string sur mésure
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
std::wstring to_wstring_utf8(const std::string& s) {
    if (s.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    if (len <= 0) return {};
    std::wstring w(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], len);
    if (!w.empty() && w.back() == L'\0') w.pop_back();
    return w;
}


// fonction pour collecter les dignostics sur un type de Sur un type de handle donné Elle retourne un string contenant des erreurs  
std::string CollectDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle)
{
    std::string out;
    SQLSMALLINT recNumber = 1;

    while (true) {
        // ces variables ont besoin d'être redéclarer à la fin d'une itération ou soit on les sorte et on les réinitialise à la fin
        SQLWCHAR sqlState[6] = {};
        SQLINTEGER nativeError = 0;
        SQLWCHAR message[1024] = {};
        SQLSMALLINT textLength = 0;

        SQLRETURN diagRet = SQLGetDiagRecW(  // éviter d'utiliser SQLGetDiagRec car on travaille avec des caractères unicode
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

        if (diagRet == SQL_SUCCESS || diagRet == SQL_SUCCESS_WITH_INFO) {  // cas où la fonction réussi ou renvoie en plus un message
            out += "[";
            out.append(to_utf8_from_w(sqlState, -1));
            out.append("] ");
            out.append(to_utf8_from_w(message, textLength));
            out.append(" (native=" + std::to_string(nativeError) + ")\n");
        }
        else {
            out += "Erreur récupération diagnostics, code=" + std::to_string(static_cast<int>(diagRet)) + "\n";
            break;
        }

        ++recNumber;
    }

    return out;
}

// fonction pour collecter les dignostics sur un type de Sur un type de handle donné Elle affiche les erreurs dans le flux de sortie par défaut 
void logDetailedDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle) {
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
            std::cout << "ODBC Diagnostic: échec récupération diag recNumber=" << recNumber
                << " code=" << static_cast<int>(diagRet) << std::endl;
            break;
        }
        ++recNumber;
    }
}


// ajout des fonctions de conversion de std::chrono::system_clock vers DATE_STRUCT et vice versa
DATE_STRUCT fromYear_month_dayToDATE_STRUCT(std::chrono::year_month_day date)
{
    DATE_STRUCT date_in_sql = {};
    date_in_sql.day = unsigned(date.day());
    date_in_sql.month = unsigned(date.month());
    date_in_sql.year = int(date.year());

    return date_in_sql;
}

std::chrono::year_month_day fromDATE_STRUCTtoYear_month_day(DATE_STRUCT date)
{
    std::chrono::day day = static_cast<std::chrono::day>(date.day);
    std::chrono::month month = static_cast<std::chrono::month>(date.month);
    std::chrono::year year = static_cast<std::chrono::year>(date.year);

    std::chrono::year_month_day normal_date(year, month, day);
    return normal_date;
}