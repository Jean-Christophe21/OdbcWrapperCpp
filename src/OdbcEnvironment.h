#pragma once
#include <sql.h>
#include <sqlext.h>
#include <Windows.h>
#include "OdbcError.h"

class OdbcEnvironment
{
public:
	OdbcEnvironment()
	{
		if (SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE ,&env_) != SQL_SUCCESS)
		{
			throw OdbcError("SQLAllocEnv failled. Impossible d'allouer un Handle d'environnement");
		}

		if (!SQL_SUCCEEDED(SQLSetEnvAttr(env_,
			SQL_ATTR_ODBC_VERSION,
			(SQLPOINTER)SQL_OV_ODBC3,
			0))) {
			std::string diag = CollectDiagnostics(SQL_HANDLE_ENV, env_);
			SQLFreeHandle(SQL_HANDLE_ENV, env_);
			throw OdbcError("Failed to set ODBC v3 attribute:\n" + diag);
		}
	}

	~OdbcEnvironment(void)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, env_);
	}

	SQLHENV handle(void) const
	{
		return env_;			// déterminer plus tard si on va faire un return de référence ou d'utiliser un return d'une copie de SQLHENV comme fait
	}

	//OdbcEnvironment(const OdbcEnvironment& env) = delete;
	//OdbcEnvironment& operator=(const OdbcEnvironment& env) = delete;

private:
	SQLHENV env_ = nullptr;
};
