#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include "OdbcError.h"
#include "OdbcEnvironment.h"

OdbcEnvironment::OdbcEnvironment()
{
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env_) != SQL_SUCCESS)
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

OdbcEnvironment::~OdbcEnvironment(void)
{
	SQLFreeHandle(SQL_HANDLE_ENV, env_);
}

SQLHENV OdbcEnvironment::handle(void) const
{
	return env_;			// déterminer plus tard si on va faire un return de référence ou d'utiliser un return d'une copie de SQLHENV comme fait
}