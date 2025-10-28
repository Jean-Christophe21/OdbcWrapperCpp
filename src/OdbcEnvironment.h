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

	OdbcEnvironment(const OdbcEnvironment& env) = delete;            // il faut �viter l'appel du constructeur de copie et l'utilisation de l'op�rateur = 
	OdbcEnvironment& operator=(const OdbcEnvironment& env) = delete;

private:
	SQLHENV env_ = nullptr;
};
