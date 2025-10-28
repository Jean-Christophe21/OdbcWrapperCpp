#pragma once

#include <chrono>
#include "OdbcError.h"



std::string to_utf8_from_w(const SQLWCHAR* wstr, int len = -1);

// focntion de conversion UTF-8 -> UTF-16 (std::string -> std::wstring)
std::wstring to_wstring_utf8(const std::string& s);

std::string CollectDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle);

void logDetailedDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle);


// fonctions de conversion de std::chrono::system_clock vers DATE_STRUCT
DATE_STRUCT fromYear_month_dayToDATE_STRUCT(std::chrono::year_month_day date);

std::chrono::year_month_day fromDATE_STRUCTtoYear_month_day(DATE_STRUCT date);