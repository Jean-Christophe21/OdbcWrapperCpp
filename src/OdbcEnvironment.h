#pragma once
#include <sql.h>
#include <sqlext.h>
#include <Windows.h>
#include "OdbcError.h"

class OdbcEnvironment
{
public:
	OdbcEnvironment();

	~OdbcEnvironment(void);

	SQLHENV handle(void) const;

	//OdbcEnvironment(const OdbcEnvironment& env) = delete;
	//OdbcEnvironment& operator=(const OdbcEnvironment& env) = delete;

private:
	SQLHENV env_ = nullptr;
};
