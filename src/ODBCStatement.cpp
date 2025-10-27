#pragma once

#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include <string.h>
#include <chrono>
#include "OdbcError.h"
#include "necessary_function.h"
#include "OdbcStatement.h"


OdbcStatement::OdbcStatement(SQLHDBC dbc)
    {
        if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt_)))
        {
            throw OdbcError("SQLAllocHandle failled to alloc Handle");
        }
    }

OdbcStatement::~OdbcStatement()
    {
        if (stmt_)
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
        }
    }

    SQLHSTMT OdbcStatement::handle(void) const
    {
        return stmt_;
    }

    void OdbcStatement::directExecute(const std::string& requete)
    {
        std::wstring wrequete = to_wstring_utf8(requete);
        SQLRETURN ret = SQLExecDirectW(stmt_, reinterpret_cast<SQLWCHAR*>(const_cast<wchar_t*>(wrequete.c_str())), SQL_NTS); 
        // SQLExecDirectW �chouait car on passait en deuxieme param�tre  reinterpret_cast<SQLWCHAR*>(const_cast<char*>(requete.c_str())).
        // remplacer par  reinterpret_cast<SQLWCHAR*>(const_cast<wchar_t*>(wrequete.c_str())) sachant que std::wstring wrequete = to_wstring_utf8(requete); pour �viter l'erreur

        if (!SQL_SUCCEEDED(ret))
        {
            throw OdbcError("SQLExecDirectW failled to directely execute query.\n" + CollectDiagnostics(SQL_HANDLE_STMT,stmt_) );
        }
    } 
    

    void OdbcStatement::Prepare(const std::string requete)   // ne pas �viter d'execute() sinon la commande ne sera tout jouste pas ex�cut�e
    {
        std::wstring wrequete = to_wstring_utf8(requete);

        auto ret = SQLPrepareW(stmt_, reinterpret_cast<SQLWCHAR*>(const_cast<wchar_t*>(wrequete.c_str())), SQL_NTS);

        if (!SUCCEEDED(ret))
        {
            throw OdbcError("SQLAllocHandle failled to alloc Handle");
        }
        

        // permet d'�viter de garder des valeurs stock�s au cas o� le m�me objet est utilis�
        ownedStrings_.clear();
        ownedInts_.clear();
        indicators_.clear();

        size_t param_count = std::count(std::begin(requete), std::end(requete),  '?');

        // reservation des espaces n�c�ssaires pour stocker les variables � bind dans notre requ�te
        ownedStrings_.reserve(param_count);
        ownedInts_.reserve(param_count);
        indicators_.reserve(param_count);
    }

    void OdbcStatement::bindString(int pos, std::string value)
    {
        ownedStrings_.push_back(value);
        auto& ref = ownedStrings_.back();
        indicators_.push_back(static_cast<SQLLEN>(ref.size()));
        SQLLEN* ind = &indicators_.back();

        SQLULEN valueSize = static_cast<SQLULEN>(ref.size());
        SQLULEN bufLen = static_cast<SQLULEN>(ref.size() + 1);

        auto ret = SQLBindParameter(
            stmt_,
            static_cast<SQLUSMALLINT>(pos),
            SQL_PARAM_INPUT,
            SQL_C_CHAR,
            SQL_VARCHAR,
            valueSize,
            0,
            (SQLPOINTER)ref.c_str(),
            bufLen,
            ind);                   // lors du bind d'un string jusqu'ici tout fonctionne bien
        if (!SQL_SUCCEEDED(ret))  // dans les prochaines versions veillez � mettre en place un cas pour SQL_SUCCESS_WITH_INFO 
        {
            throw OdbcError("Echec BindParameter (collectdiagnostic) = " + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
    }

    void OdbcStatement::bindNullableString(int pos, const std::string* valuePtr)
    {
        if (valuePtr)
        {
            bindString(pos, std::string(*valuePtr));

       }
        else
        {
            indicators_.push_back(SQL_NULL_DATA);
            SQLLEN* ind = &indicators_.back();

            auto ret = SQLBindParameter(
                stmt_,
                static_cast<SQLUSMALLINT>(pos),
                SQL_PARAM_INPUT,
                SQL_C_CHAR,
                SQL_VARCHAR,
                0,
                0,
                nullptr,
                0,
                ind);
            if (!SUCCEEDED(ret))
            {
                throw OdbcError("Echec BindParameter failled to bind Nullable string.\n(collectdiagnostic) = " + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
            }
        }
    }


    void OdbcStatement::bindInt(int pos, const int value)
    {
        ownedInts_.push_back(value);
        auto& ref = ownedInts_.back();

        indicators_.push_back(static_cast<SQLLEN>(sizeof(SQLINTEGER)));
        SQLLEN& ind = indicators_.back();

        auto ret = SQLBindParameter(
            stmt_,
            static_cast<SQLSMALLINT>(pos),
            SQL_PARAM_INPUT,
            SQL_C_LONG,
            SQL_INTEGER,
            0,
            0,
            (SQLPOINTER)&ref,
            static_cast<SQLLEN>(sizeof(SQLINTEGER)),
            &ind);
            
        if (!SUCCEEDED(ret))
        {
            throw OdbcError("Echec BindParameter failled to bind INt.\n(collectdiagnostic) = " + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
    }

    void OdbcStatement::bindNullableInt(int pos, int value)
    {
        int* valueptr = &value;
        if (valueptr)
        {
            bindInt(pos, *valueptr);
        }
        else
        {
            indicators_.push_back(SQL_NULL_DATA);
            SQLLEN* ind = &indicators_.back();

            auto ret = SQLBindParameter(
                stmt_,
                static_cast<SQLUSMALLINT>(pos),
                SQL_PARAM_INPUT,
                SQL_C_LONG,
                SQL_INTEGER,
                0,
                0,
                nullptr,
                0,
                ind );

            if (!SUCCEEDED(ret))
            {
                throw OdbcError("Echec BindParameter failled to bind NUllable INt.\n(collectdiagnostic) = " 
                    + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
            }
        }
    }

    // ajout de bind date
    void OdbcStatement::bindDate(int pos, DATE_STRUCT date)
    {
        ownedDates_.push_back(date);
        auto& ref = ownedDates_.back();

        indicators_.push_back(static_cast<SQLLEN>(sizeof(date)));
        SQLLEN ind = indicators_.back();

        SQLRETURN ret = SQLBindParameter(
            stmt_,
            static_cast<SQLLEN>(pos),
            SQL_PARAM_INPUT,
            SQL_C_DATE,
            SQL_DATE,
            0,
            0,
            (SQLPOINTER)&ref,
            static_cast<SQLLEN>(SQL_DATE),
            &ind
        );

        if (!SQL_SUCCEEDED(ret))
        {
            throw OdbcError("Error BindDate failled to bind date in the statement.\n(collectdiagnostic) = " + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
    }

    void OdbcStatement::bindNullableDate(int pos, DATE_STRUCT* date)
    {
        if (date)
            bindDate(pos, *date);
        else {
            indicators_.push_back(SQL_NULL_DATA);
            SQLLEN* ind = &indicators_.back();

            auto ret = SQLBindParameter(
                stmt_,
                static_cast<SQLUSMALLINT>(pos),
                SQL_PARAM_INPUT,
                SQL_C_DATE,
                SQL_DATE,
                0,
                0,
                nullptr,
                0,
                ind);

            if (!SQL_SUCCEEDED(ret))
            {
                throw OdbcError("Error BindDate failled to bind date in the statement.\n(collectdiagnostic) = " + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
            }
        }
    }


    void OdbcStatement::execute() const
    {
        auto ret = SQLExecute(stmt_);
        if (!SUCCEEDED(ret))
        {
            throw OdbcError("Echec BindParameter failled to bind NUllable INt.\n(collectdiagnostic message) = "
            + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
    }

    bool OdbcStatement::fetch() const {
        auto ret = SQLFetch(stmt_);
        if (ret == SQL_NO_DATA)
            return false;

        if (!SUCCEEDED(ret))
        {
            auto message = CollectDiagnostics(SQL_HANDLE_STMT, stmt_);
            throw OdbcError("SQLFetch failled.\n(collectdiagnostic message) = "
                + message);
        }
        return true;
    }


    std::string OdbcStatement::getString(int col) const
    {
        SQLLEN ind = 0;
        char buf[1024] = {};
        SQLLEN taillebuf = static_cast<SQLLEN>(sizeof(buf));

        SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(col), SQL_C_CHAR, buf, static_cast<SQLLEN>(sizeof(buf)), &ind);

        if (!SQL_SUCCEEDED(ret) && ret != SQL_SUCCESS_WITH_INFO)
        {
            throw OdbcError("SQLGetData failled..\nErreur :" + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
        
        if (ret == SQL_NULL_DATA) {
            return {};
        }
        if (ret > 0 && ind <= taillebuf)
        {
            return std::string(buf, static_cast<size_t>(sizeof(buf)));
        }
        return std::string(buf, static_cast<size_t>(sizeof(buf)));
    }


    int OdbcStatement::getInt(int col) const
    {
        SQLLEN ind = 0;
        SQLINTEGER buf = {};
        SQLLEN sizebuf = static_cast<SQLLEN>(sizeof(buf));

        SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLUSMALLINT>(col), SQL_C_SLONG, &buf, static_cast<SQLLEN>(sizeof(buf)), &ind);

        if (!SQL_SUCCEEDED(ret) && ret != SQL_SUCCESS_WITH_INFO)
        {
            throw OdbcError("SQLGetData failled..\nErreur :" + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }

        if (ret == SQL_NULL_DATA) {
            return {};
        }
        if (ret > 0)
        {
            return static_cast<int>(buf);
        }
        return static_cast<int>(buf);
    }

    //ajout de la m�thode get Date
    DATE_STRUCT OdbcStatement::getDate(int col) const
    {
        SQLLEN ind = 0;
        DATE_STRUCT buf = {};
        SQLLEN sizebuf = static_cast<SQLLEN>(sizeof(buf));

        SQLRETURN ret = SQLGetData(stmt_, static_cast<SQLLEN>(col), SQL_DATE, (SQLPOINTER)&buf, sizebuf, &ind);

        if (!SQL_SUCCEEDED(ret))
        {
            throw OdbcError("SQLGetData failled..\nErreur :" + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
        if (ret == SQL_NULL_DATA)
            return {};  // on retourne rien
        else
        {
            return DATE_STRUCT(buf);
        }
        return DATE_STRUCT(buf);
    }


    void OdbcStatement::closeCursor() const {
        SQLCloseCursor(stmt_);
    }
