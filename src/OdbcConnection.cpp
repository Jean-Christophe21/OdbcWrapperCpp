#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <sqlext.h>
#include "OdbcConnection.h"



// constructeur prenant un dsn, un username ou uid et un password
OdbcConnection::OdbcConnection(OdbcEnvironment& env,
    const std::string& dsn,
    const std::string& user,
    const std::string& password)
{
    // tentative d'allocation d'un handle de connection HDBC
    if (SQLAllocHandle(SQL_HANDLE_DBC, env.handle(), &dbc_) != SQL_SUCCESS)
    {
        throw OdbcError("SQLAllocHandle failled to alloc HDBC handle.");
    }

    // Conversion des paramètres de utf-8 vers utf-16 avec la fonction to_wstring_utf8 dans necessary_function.h
    auto wDsn = to_wstring_utf8(dsn);
    auto wUser = to_wstring_utf8(user);
    auto wPassword = to_wstring_utf8(password);


    // Tentative de connexion à la base de donnée
    SQLRETURN ret = SQLConnectW(
        dbc_,
        const_cast<SQLWCHAR*>(reinterpret_cast<const SQLWCHAR*>(wDsn.c_str())), SQL_NTS,
        const_cast<SQLWCHAR*>(reinterpret_cast<const SQLWCHAR*>(wUser.c_str())), SQL_NTS,
        const_cast<SQLWCHAR*>(reinterpret_cast<const SQLWCHAR*>(wPassword.c_str())), SQL_NTS
    );

    if (ret == SQL_SUCCESS or ret == SQL_SUCCESS_WITH_INFO)               // on considère le cas de succes avec retour d'info comme un succes. 
                                                                          //Dans les versions à venir prévoir la gestion séparée de ce cas
    {
        std::cout << "Connexion avec succes..\n";
    }
    else if (ret == SQL_NEED_DATA) {                                                                     // la fonction SQLDriverConnectW requiert plus d'infos pour pouvoir se connecter
        std::cout << "Échec de connexion ODBC : SQL_NEED_DATA. Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);                                                            // la fonction SQLDriverConnectW requiert plus d'infos pour pouvoir se connecter
        throw OdbcError("Échec de connexion ODBC: SQL_NEED_DATA (voir logs)");
    }
    else if (ret == SQL_ERROR) {                                                                        // la fonction SQLDriverConnectW a renvoyé une erreur
        std::cout << "Erreur ODBC : SQL_ERROR. Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        throw OdbcError("Échec de connexion ODBC: SQL_ERROR (voir logs)");
    }
    else if (ret == SQL_INVALID_HANDLE) {                                                               // handle HDBC invalide 
        std::cout << "Erreur ODBC : SQL_INVALID_HANDLE. Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        throw OdbcError("Échec de connexion ODBC: SQL_INVALID_HANDLE (voir logs)");
    }
    else {
        //on récupère les diagnostics détaillés et on échoue le programme en fournissant le code renvoyé.
        std::cout << "Échec de connexion ODBC : code retour = " << ret << ". Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        throw OdbcError("Échec de connexion ODBC: code retour=" + std::to_string(static_cast<int>(ret)));
    }
}


// constructeur prenant une chaine de connexion 
OdbcConnection::OdbcConnection(OdbcEnvironment& env,
    const std::string connetionString,
    SQLUSMALLINT driverCompletion
)
{
    // tentative d'allocation d'un handle de connection HDBC
    if (SQLAllocHandle(SQL_HANDLE_DBC, env.handle(), &dbc_) != SQL_SUCCESS)
    {
        throw OdbcError("SQLAllocHandle failled to alloc DBC handle.");
    }

    auto wConnStr = to_wstring_utf8(connetionString);   // chaine de connexion au format utf-16
    const SQLWCHAR* inConn = reinterpret_cast<const SQLWCHAR*>(wConnStr.c_str());

    SQLWCHAR outConn[1024] = {};
    SQLSMALLINT outlen = 0;

    // Tentative de connexion à la base de donnée
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

    if (ret == SQL_SUCCESS or ret == SQL_SUCCESS_WITH_INFO)   // on considère le cas de succes avec retour d'info comme un succes. 
                                                              //Dans les versions à venir prévoir la gestion séparée de ce cas
    {
        std::cout << "Connexion avec succes..\n";
    }
    else if (ret == SQL_NEED_DATA) {                         // la fonction SQLDriverConnectW requiert plus d'infos pour pouvoir se connecter
        std::cout << "Échec de connexion ODBC : SQL_NEED_DATA. Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        throw OdbcError("Échec de connexion ODBC: SQL_NEED_DATA (voir logs)");
    }
    else if (ret == SQL_ERROR) {                            // la fonction SQLDriverConnectW a renvoyé une erreur
        std::cout << "Erreur ODBC : SQL_ERROR. Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        throw OdbcError("Échec de connexion ODBC: SQL_ERROR (voir logs)");
    }
    else if (ret == SQL_INVALID_HANDLE) {                   // handle HDBC invalide 
        std::cout << "Erreur ODBC : SQL_INVALID_HANDLE. Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        throw OdbcError("Échec de connexion ODBC: SQL_INVALID_HANDLE (voir logs)");
    }
    else {
        //on récupère les diagnostics détaillés et on échoue le programme en fournissant le code renvoyé.
        std::cout << "Échec de connexion ODBC : code retour = " << ret << ". Diagnostics détaillés :" << std::endl;
        logDetailedDiagnostics(SQL_HANDLE_DBC, dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
        throw OdbcError("Échec de connexion ODBC: code retour=" + std::to_string(static_cast<int>(ret)));
    }
}

//destructeur
OdbcConnection::~OdbcConnection() {
    if (dbc_)
    {
        SQLDisconnect(dbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, dbc_);
    }
}

// Gestion transaction (false est la valeur par défaut d' autocommit)
void OdbcConnection::beginTransaction() {
    if (!inTransaction) {
        SQLSetConnectAttr(dbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0);
        inTransaction = true;
    }
}

// cette fonction permet de commiter les changements sur la base de donnée
void OdbcConnection::commit() {  
    if (inTransaction) {
        SQLEndTran(SQL_HANDLE_DBC, dbc_, SQL_COMMIT);
        SQLSetConnectAttr(dbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
        inTransaction = false;
    }
}

// cette fonction permet de rollback ou annuler les changements sur la base de donnée
void OdbcConnection::rollback() {
    if (inTransaction) {
        SQLEndTran(SQL_HANDLE_DBC, dbc_, SQL_ROLLBACK);
        SQLSetConnectAttr(dbc_, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);
        inTransaction = false;
    }
}

// retourne le handle de connection
SQLHDBC& OdbcConnection::handle(void) { return this->dbc_; }
