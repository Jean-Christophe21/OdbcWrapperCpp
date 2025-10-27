#pragma once

#include <Windows.h>
#include <stdexcept>
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include "necessary_function.h"



class OdbcError : public std::runtime_error
{
	public :
		OdbcError(const std::string& message);
		
        friend std::string to_utf8_from_w(const SQLWCHAR* wstr, int len);

};
