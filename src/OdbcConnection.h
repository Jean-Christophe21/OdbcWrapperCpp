#pragma once
#include <iostream>
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <windows.h>
#include "OdbcError.h"
#include "OdbcEnvironment.h"



class OdbcConnection
{
public:
    // utilitaire de conversion UTF-8 -> UTF-16 (std::string -> std::wstring)
    // d�finition dans OdbcError.h 
    friend std::wstring to_wstring_utf8(const std::string& s);
    friend std::string to_utf8_from_w(const SQLWCHAR* wstr, int len);
    friend void logDetailedDiagnostics(SQLSMALLINT handleType, SQLHANDLE handle);

    // constructeur utilisant un dsn, username ou uid et un password
    OdbcConnection(OdbcEnvironment& env,
        const std::string& dsn,
        const std::string& user,
        const std::string& password)
    {
        // tentative d'allocation d'un handle de connection HDBC
        if (SQLAllocHandle(SQL_HANDLE_DBC, env.handle(), &dbc_) != SQL_SUCCESS)
        {
            throw OdbcError("SQLAllocHandle failled to alloc DBC handle.");
        }
        // (Optionnel) Timeout de connexion
        SQLSetConnectAttr(dbc_, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER>(static_cast<uintptr_t>(5)),0);

        // Conversion des param�tres de utf-8 vers utf-16
        auto wDsn = to_wstring_utf8(dsn);
        auto wUser = to_wstring_utf8(user);
        auto wPassword = to_wstring_utf8(password);


        // Tentative de connexion � la base de donn�e
        SQLRETURN ret = SQLConnectW(
            dbc_,
            const_cast<SQLWCHAR*>(reinterpret_cast<const SQLWCHAR*>(wDsn.c_str())), SQL_NTS,
            const_cast<SQLWCHAR*>(reinterpret_cast<const SQLWCHAR*>(wUser.c_str())), SQL_NTS,
            const_cast<SQLWCHAR*>(reinterpret_cast<const SQLWCHAR*>(wPassword.c_str())), SQL_NTS
        );

        if (ret == SQL_SUCCESS or ret == SQL_SUCCESS_WITH_INFO)
        {
            std::cout << "Connexion avec succes..\n";
        }
        else if (ret == SQL_NEED_DATA) {
            std::cout << "�chec de connexion ODBC : SQL_NEED_DATA. Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: SQL_NEED_DATA (voir logs)");
        }
        else if (ret == SQL_ERROR) {
            std::cout << "Erreur ODBC : SQL_ERROR. Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: SQL_ERROR (voir logs)");
        }
        else if (ret == SQL_INVALID_HANDLE) {
            std::cout << "Erreur ODBC : SQL_INVALID_HANDLE. Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: SQL_INVALID_HANDLE (voir logs)");
        }
        else {
            // Cas g�n�rique : on r�cup�re les diagnostics d�taill�s et on �choue en fournissant le code renvoy�.
            std::cout << "�chec de connexion ODBC : code retour = " << ret << ". Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: code retour=" + std::to_string(static_cast<int>(ret)));
        }
    }
    // constructeur utilisant une chaine de connexion
    OdbcConnection(OdbcEnvironment& env,
        const std::string connetionString,
        SQLUSMALLINT driverCompletion = SQL_DRIVER_COMPLETE
    )
    {
        // tentative d'allocation d'un handle de connection HDBC
        if (SQLAllocHandle(SQL_HANDLE_DBC, env.handle(), &dbc_) != SQL_SUCCESS)
        {

            throw OdbcError("SQLAllocHandle failled to alloc DBC handle.");
        }

        // (Optionnel) Timeout de connexion
        SQLSetConnectAttr(dbc_, SQL_LOGIN_TIMEOUT, reinterpret_cast<SQLPOINTER>(static_cast<uintptr_t>(5)), 0);

        auto wConnStr = to_wstring_utf8(connetionString);
        const SQLWCHAR* inConn = reinterpret_cast<const SQLWCHAR*>(wConnStr.c_str());

        SQLWCHAR outConn[1024] = {};
        SQLSMALLINT outlen = 0;

        // Tentative de connexion � la base de donn�e
        SQLRETURN ret = SQLDriverConnectW(
            dbc_,
            nullptr,
            const_cast<SQLWCHAR*>(inConn),
            SQL_NTS,
            outConn,
            static_cast<SQLSMALLINT>(std::size(outConn)),
            &outlen,
            driverCompletion
            );

        if (ret == SQL_SUCCESS or ret == SQL_SUCCESS_WITH_INFO)
        {
            std::cout << "Connexion avec succes..\n";
        }
        else if (ret == SQL_NEED_DATA) {
            std::cout << "�chec de connexion ODBC : SQL_NEED_DATA. Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: SQL_NEED_DATA (voir logs)");
        }
        else if (ret == SQL_ERROR) {
            std::cout << "Erreur ODBC : SQL_ERROR. Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: SQL_ERROR (voir logs)");
        }
        else if (ret == SQL_INVALID_HANDLE) {
            std::cout << "Erreur ODBC : SQL_INVALID_HANDLE. Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: SQL_INVALID_HANDLE (voir logs)");
        }
        else {
            // Cas g�n�rique : on r�cup�re les diagnostics d�taill�s et on �choue en fournissant le code renvoy�.
            std::cout << "�chec de connexion ODBC : code retour = " << ret << ". Diagnostics d�taill�s :" << std::endl;
            logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
            throw OdbcError("�chec de connexion ODBC: code retour=" + std::to_string(static_cast<int>(ret)));
        }
    }

    //destructeur
    ~OdbcConnection() {
        if (dbc_)
        {
            SQLDisconnect(dbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        }
    }

    // Gestion transaction simple (off par d�faut en autocommit implicite)
    void beginTransaction() {
        if (!inTransaction) {
            SQLSetConnectAttr(dbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
            inTransaction = true;
        }
    }

    void commit() {
        if (inTransaction) {
            SQLEndTran(SQL_HANDLE_DBC, dbc_, SQL_COMMIT);
            SQLSetConnectAttr(dbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
            inTransaction = false;
        }
    }

    void rollback() {
        if (inTransaction) {
            SQLEndTran(SQL_HANDLE_DBC, dbc_, SQL_ROLLBACK);
            SQLSetConnectAttr(dbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
            inTransaction = false;
        }
    }

    SQLHDBC& handle(void) { return this->dbc_; }
    
    private:
        SQLHDBC dbc_ = nullptr;
        bool inTransaction = false;

};
