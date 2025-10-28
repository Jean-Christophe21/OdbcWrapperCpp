#pragma once

#include "OdbcEnvironment.h"

class OdbcConnection
{
public:
    // fonction de conversion UTF-8 -> UTF-16 (std::string -> std::wstring)
    friend std::wstring to_wstring_utf8(const std::string& s);
    friend std::string to_utf8_from_w(const SQLWCHAR* wstr, int len);
    friend void logDetailedDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle);

    // constructeur utilisant un dsn, username ou uid et un password
    OdbcConnection(OdbcEnvironment& env,
        const std::string& dsn,
        const std::string& user,
        const std::string& password);

    // constructeur utilisant une chaine de connexion
    OdbcConnection(OdbcEnvironment& env,
        const std::string connetionString,
        SQLUSMALLINT driverCompletion = SQL_DRIVER_COMPLETE
    );

    //destructeur
    ~OdbcConnection();

    // Gestion des transactions begin commit et rollback
    void beginTransaction();

    void commit();

    void rollback();

    SQLHDBC& handle(void);
    
    private:
        SQLHDBC dbc_ = nullptr;
        bool inTransaction = false;

};
