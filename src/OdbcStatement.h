#pragma once

#include <sql.h>
#include <sqlext.h>
#include <Windows.h>
#include <vector>
#include <string.h>
#include <string>
#include "OdbcError.h"


class OdbcStatement {

public :
    OdbcStatement(SQLHDBC dbc)
    {
        if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt_)))
        {
            throw OdbcError("SQLAllocHandle failled to alloc Handle");
        }
    }

    ~OdbcStatement()
    {
        if (stmt_)
        {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
        }
    }

    SQLHSTMT handle(void)
    {
        return stmt_;
    }

    void directExecute(const std::string& requete)
    {
        std::wstring wrequete = to_wstring_utf8(requete);
        SQLRETURN ret = SQLExecDirectW(stmt_, reinterpret_cast<SQLWCHAR*>(const_cast<wchar_t*>(wrequete.c_str())), SQL_NTS); 
        // SQLExecDirectW échouait car on passait en deuxieme paramètre  reinterpret_cast<SQLWCHAR*>(const_cast<char*>(requete.c_str())).
        // remplacer par  reinterpret_cast<SQLWCHAR*>(const_cast<wchar_t*>(wrequete.c_str())) sachant que std::wstring wrequete = to_wstring_utf8(requete); pour éviter l'erreur

        if (!SQL_SUCCEEDED(ret))
        {
            throw OdbcError("SQLExecDirectW failled to directely execute query.\n" + CollectDiagnostics(SQL_HANDLE_STMT,stmt_) );
        }
    } 
    

    void Prepare(const std::string requete)   // ne pas éviter d'execute() sinon la commande ne sera tout jouste pas exécutée
    {
        std::wstring wrequete = to_wstring_utf8(requete);

        auto ret = SQLPrepareW(stmt_, reinterpret_cast<SQLWCHAR*>(const_cast<wchar_t*>(wrequete.c_str())), SQL_NTS);

        if (!SUCCEEDED(ret))
        {
            throw OdbcError("SQLAllocHandle failled to alloc Handle");
        }
        

        // permet d'éviter de garder des valeurs stockés au cas où le même objet est utilisé
        ownedStrings_.clear();
        ownedInts_.clear();
        indicators_.clear();

        size_t param_count = std::count(std::begin(requete), std::end(requete),  '?');

        // reservation des espaces nécéssaires pour stocker les variables à bind dans notre requête
        ownedStrings_.reserve(param_count);
        ownedInts_.reserve(param_count);
        indicators_.reserve(param_count);
    }

    void bindString(int pos, std::string value)
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
        if (!SQL_SUCCEEDED(ret))  // dans les prochaines versions veillez à mettre en place un cas pour SQL_SUCCESS_WITH_INFO 
        {
            throw OdbcError("Echec BindParameter (collectdiagnostic) = " + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
    }

    void bindNullableString(int pos, const std::string* valuePtr)
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


    void bindInt(int pos, const int value)
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

    void bindNullableInt(int pos, int value)
    {
        int* valueptr = &value;
        if (valueptr)
        {
            bindInt(pos, *valueptr);
        }
        else
        {
            indicators_.push_back(SQL_NO_DATA);
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

    void execute() const
    {
        auto ret = SQLExecute(stmt_);
        if (!SUCCEEDED(ret))
        {
            throw OdbcError("Echec BindParameter failled to bind NUllable INt.\n(collectdiagnostic message) = "
            + CollectDiagnostics(SQL_HANDLE_STMT, stmt_));
        }
    }

    bool fetch() const {
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


    std::string getString(int col) const
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


    int getInt(int col) const
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

    void closeCursor() const {
        SQLCloseCursor(stmt_);
    }

private:
    SQLHSTMT stmt_ = nullptr;
    std::vector<std::string> ownedStrings_; // Stocke les chaînes pour garantir leur durée de vie pendant l'exécution de la requete
    std::vector<int> ownedInts_;            // Stocke les entiers copiés pour garantir durée de vie
    std::vector<SQLLEN> indicators_;        // Pour valeurs NULL éventuelles ou longueurs
};
