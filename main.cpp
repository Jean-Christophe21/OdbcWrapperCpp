#include <iostream>
#include <cassert>
#include "necessary_function.h"
#include "OdbcConnection.h"
#include "OdbcEnvironment.h"
#include "OdbcError.h"
#include "OdbcStatement.h"


int main()
{
    try
    {
        std::cout << "création d'un environnement..\n";
        OdbcEnvironment env;
        assert(env.handle() != SQL_NULL_HENV);
        std::cout << "> création d'un environnement avec succes\n\n";

        std::string connectionString = "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\mylocaldb;Database=test3;Trusted_Connection=Yes;";
        std::cout << "Test du second constructeur création d'une connection..\n";
        OdbcConnection conn(env, connectionString, SQL_DRIVER_COMPLETE);   // test du second constructeur de la classe
        assert(conn.handle() != SQL_NULL_HDBC);
        std::cout << "> création d'une connexion avec succes\n\n";

        std::cout << "> création d'un handle de statement..\n\n";
        OdbcStatement stmt(conn.handle());
        assert(stmt.handle() != SQL_NULL_HSTMT);

        std::cout << "Handle de statement créé avec succes..\n\n";

        // test de directExecute
        std::cout << "Execution de requetes simples...\n";

        // préparation de la requete 
        std::string requete = "SELECT Id,Date FROM Test_Date;";
        // Insertion de données
        std::cout << "  > get de donnees...\n";
        stmt.Prepare(requete);
        stmt.execute();
        while (stmt.fetch())
        {
            int id = stmt.getInt(1);
            DATE_STRUCT date = stmt.getDate(2);

            // affichage des résulats
            std::cout << "id : " << id << "  Date" << date.day << "/" << date.month << "/" << date.year << "\n";
        }

        stmt.closeCursor();

        std::cout << "  > Donnees inserees avec succes.\n";

        std::cout << "\n> Toutes les requetes directes ont ete executees avec succes.\n";
    }
    catch (std::exception& e) {
        std::cerr << "> Erreur lors de l'exécution de testOdbcStatement...\n\n";
        std::cerr << "Erreur :" << e.what();
        assert(false);
    }


    return 0;
}






//// test de bind date et nullable date
//#include <iostream>
//#include <cassert>
//#include "necessary_function.h"
//#include "OdbcConnection.h"
//#include "OdbcEnvironment.h"
//#include "OdbcError.h"
//#include "OdbcStatement.h"
//
//
//
//
//int main()
//{
//    try 
//    {
//        std::cout << "création d'un environnement..\n";
//        OdbcEnvironment env;
//        assert(env.handle() != SQL_NULL_HENV);
//        std::cout << "> création d'un environnement avec succes\n\n";
//        
//        std::string connectionString = "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\mylocaldb;Database=test3;Trusted_Connection=Yes;";
//        std::cout << "Test du second constructeur création d'une connection..\n";
//        OdbcConnection conn(env, connectionString, SQL_DRIVER_COMPLETE);   // test du second constructeur de la classe
//        assert(conn.handle() != SQL_NULL_HDBC);
//        std::cout << "> création d'une connexion avec succes\n\n";
//        
//        std::cout << "> création d'un handle de statement..\n\n";
//        OdbcStatement stmt(conn.handle());
//        assert(stmt.handle() != SQL_NULL_HSTMT);
//        
//        std::cout << "Handle de statement créé avec succes..\n\n";
//        
//        // test de directExecute
//        std::cout << "Execution de requetes simples...\n";
//        
//        // préparation de la requete 
//
//        // Insertion de données
//        std::cout << "  > get de donnees...\n";
//        stmt.Prepare("FROM Test_date SELECT* (Date) VALUES (?)");
//        DATE_STRUCT date_sql_type(2038, 1, 18);
//        stmt.bindNullableDate(1,nullptr /*&date_sql_type*/);        
//        stmt.execute();
//
//        std::cout << "  > Donnees inserees avec succes.\n";
//
//        std::cout << "\n> Toutes les requetes directes ont ete executees avec succes.\n";
//    }
//    catch (std::exception& e) {
//        std::cerr << "> Erreur lors de l'exécution de testOdbcStatement...\n\n";
//        std::cerr << "Erreur :" << e.what();
//        assert(false);
//    }
//
//    
//    return 0;
//}


// test de fromDATE_STRUCTtoYear_month_day et de fromYear_month_dayToDATE_STRUCT
//#include <iostream>
//#include "necessary_function.h"
//
//
//int main()
//{
//    // test de fromDATE_STRUCTtoYear_month_day
//    std::chrono::year_month_day today_date = {};
//    DATE_STRUCT date;
//    date.day = 22;
//    date.month = 10;
//    date.year = 2025;
//    today_date = fromDATE_STRUCTtoYear_month_day(date);
//    std::cout << "test de la fonction fromDATE_STRUCTtoYear_month_day : \n";
//
//    std::cout << today_date << "\n";
//    std::cout << today_date.day() << "/" << today_date.month() << "/" << today_date.year() << "\n" << std::endl;
//
//
//    // test de fromYear_month_dayToDATE_STRUCT
//    std::chrono::day day(22);
//    std::chrono::month month(10);
//    std::chrono::year year(2025);
//    std::chrono::year_month_day date_a_convertir(year, month, day);
//
//    auto date1 = fromYear_month_dayToDATE_STRUCT(date_a_convertir);
//    std::cout << "\ntest de la fonction fromYear_month_dayToDATE_STRUCT : \n";
//    std::cout << date1.day << "/" << date1.month << "/" << date1.year;
//
//    
//    return 0;
//}
//




//#include <iostream>
//#include "necessary_function.h"
//
//
//int main()
//{
//    std::cout << "Bienvenue dans ce projet qui implémente l'API ODBC en C++.\n" <<
//        "Ce programme fournit des méthodes et fonctions pour accéder à manipuler des données d'une base de donnée.\n" <<
//        "Vous trouverez si dessous en commentaire quelques test éffectué sur les méthodes et fonctions afin de s'assurer de leur bon fonctionnement.\n";
//    return 0;
//}
//






// test des fonctions membres de OdbcError.h
//#include <cassert>
//#include <iostream>
//#include <windows.h>
//#include <vector>
//#include <string>
//#include <string.h>
//#include <random>
//#include "OdbcEnvironment.h"
//#include "OdbcConnection.h"
//#include "OdbcStatement.h"
//#include "OdbcError.h"
//
//
//void testOdbcErrorFunctions()
//{
//    std::cout << "\n--- Lancement des tests pour OdbcError.h ---\n";
//
//    // --- Test 1: Fonctions de conversion de chaines ---
//    std::cout << "  > Test des conversions UTF-8 <-> UTF-16...\n";
//    try {
//        // Cas standard
//        std::string original_ascii = "Test standard avec ASCII";
//        std::wstring wide_ascii = to_wstring_utf8(original_ascii);
//        std::string result_ascii = to_utf8_from_w(reinterpret_cast<const SQLWCHAR*>(wide_ascii.c_str()));
//        assert(original_ascii == result_ascii);
//        std::cout << "original_ascii == result_ascii\n";
//
//        // Cas avec caractères accentués
//        std::string original_utf8 = "Test avec caractères spéciaux : é à ç û";
//        std::wstring wide_utf8 = to_wstring_utf8(original_utf8);
//        std::string result_utf8 = to_utf8_from_w(reinterpret_cast<const SQLWCHAR*>(wide_utf8.c_str()));
//        assert(original_utf8 == result_utf8);
//        std::cout << "original_utf8 == result_utf8\n";
//
//        // Cas d'une chaîne vide
//        std::string original_empty = "";
//        std::wstring wide_empty = to_wstring_utf8(original_empty);
//        std::string result_empty = to_utf8_from_w(reinterpret_cast<const SQLWCHAR*>(wide_empty.c_str()));
//        assert(original_empty == result_empty);
//        std::cout << "(original_empty == result_empty\n";
//
//        // Cas d'un pointeur nul pour to_utf8_from_w
//        std::string result_null = to_utf8_from_w(nullptr);
//        assert(result_null.empty());
//
//        std::cout << "  > SUCCES : Les fonctions de conversion fonctionnent comme attendu.\n";
//    }
//    catch (const std::exception& e) {
//        std::cerr << "  > ECHEC : Exception inattendue lors du test de conversion : " << e.what() << std::endl;
//        assert(false);
//    }
//
//    // --- Test 2: logDetailedDiagnostics ---
//    std::cout << "\n  > Test de logDetailedDiagnostics en provoquant une erreur...\n";
//    try {
//        OdbcEnvironment env;
//        std::string connectionString = "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\mylocaldb;Database=test3;Trusted_Connection=Yes;";
//        OdbcConnection conn(env, connectionString, SQL_DRIVER_COMPLETE);
//        OdbcStatement stmt(conn.handle());
//
//        // On exécute une requête volontairement incorrecte pour générer un diagnostic
//        std::cout << "  > Execution d'une requete SQL incorrecte (attendu).\n";
//        SQLRETURN ret = SQLExecDirectW(stmt.handle(), (SQLWCHAR*)L"SELECT * FROM TableQuiNexistePas", SQL_NTS);
//
//        if (!SQL_SUCCEEDED(ret)) {
//            std::cout << "  > L'erreur a ete generee comme prevu. Affichage des diagnostics :\n";
//            // Appel de la fonction à tester
//            logDetailedDiagnostics(SQL_HANDLE_STMT, stmt.handle());
//            std::cout << "  > SUCCES : logDetailedDiagnostics a ete appele. Verifiez la sortie console.\n";
//        }
//        else {
//            std::cerr << "  > ECHEC : La requete incorrecte n'a pas genere d'erreur.\n";
//            assert(false);
//        }
//    }
//    catch (const std::exception& e) {
//        // On s'attend à une exception de nos wrappers, mais on continue pour montrer le test
//        std::cerr << "  > NOTE : Une exception a ete interceptee (comportement normal du wrapper) : " << e.what() << std::endl;
//    }
//
//    std::cout << "\n--- Fin des tests pour OdbcError.h ---\n";
//}
//
//
//int main()
//{
//    // testOdbcStatement();
//    testOdbcErrorFunctions();
//
//    return 0;
//}
//
//
//




// test correct 
//#include <cassert>
//#include <iostream>
//#include <windows.h>
//#include <vector>
//#include <string>
//#include <string.h>
//#include <random>
//#include "OdbcEnvironment.h"
//#include "OdbcConnection.h"
//#include "OdbcStatement.h"
////#include "BookRepository.h"
//#include "OdbcError.h"
//
//
//void testOdbcStatement(void)
//{
//    try {
//        std::cout << "création d'un environnement..\n";
//        OdbcEnvironment env;
//        assert(env.handle() != SQL_NULL_HENV);
//        std::cout << "> création d'un environnement avec succes\n\n";
//
//        std::string connectionString = "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\mylocaldb;Database=test3;Trusted_Connection=Yes;";
//        std::cout << "Test du second constructeur création d'une connection..\n";
//        OdbcConnection conn(env, connectionString, SQL_DRIVER_COMPLETE);   // test du second constructeur de la classe
//        assert(conn.handle() != SQL_NULL_HDBC);
//        std::cout << "> création d'une connexion avec succes\n\n";
//
//        std::cout << "> création d'un handle de statement..\n\n";
//        OdbcStatement stmt(conn.handle());
//        assert(stmt.handle() != SQL_NULL_HSTMT);
//
//        std::cout << "Handle de statement créé avec succes..\n\n";
//
//        // --- Début du test pour fetch, getInt, getString ---
//
//
//
//        // 2. Exécution de la requête SELECT
//        std::cout << "\n  > Execution de la requete SELECT...\n";
//        stmt.directExecute("SELECT ID, Name FROM FetchTest ORDER BY ID");
//
//        // 3. Parcours des résultats avec fetch() et récupération des données
//        std::cout << "  > Parcours des resultats :\n";
//        int rowCount = 0;
//        while (stmt.fetch()) {
//            rowCount++;
//            int id = stmt.getInt(1); // Récupère la 1ère colonne (ID)
//            std::string name = stmt.getString(2); // Récupère la 2ème colonne (Name)
//
//            std::cout << "    Ligne " << rowCount << ": ID=" << id << ", Name='" << name << "'\n";
//
//            // 4. Vérification des données
//            if (rowCount == 1) {
//                assert(id == 10);
//                //assert(name == "Premiere Ligne");
//            }
//            else if (rowCount == 2) {
//                assert(id == 20);
//                //assert(name == "Seconde Ligne");
//            }
//        }
//        assert(rowCount == 2); // S'assurer qu'on a bien lu 2 lignes
//        std::cout << "  > Verification des donnees reussie.\n";
//
//        // 5. Test de closeCursor()
//        std::cout << "\n  > Fermeture du curseur...\n";
//        stmt.closeCursor();
//        std::cout << "  > Curseur ferme.\n";
//
//        std::cout << "\n> Test de fetch/get/close reussi.\n";
//    }
//    catch (std::exception& e) {
//        std::cerr << "> Erreur lors de l'exécution de testOdbcStatement...\n\n";
//        std::cerr << "Erreur :" << e.what();
//        assert(false);
//    }
//}
//
//int main()
//{
//    testOdbcStatement();
//
//    return 0;
//}









// test de rollback()
//#include <cassert>
//#include <iostream>
//#include <windows.h>
//#include <vector>
//#include <string>
//#include <string.h>
//#include <random>
//#include "OdbcEnvironment.h"
//#include "OdbcConnection.h"
//#include "OdbcStatement.h"
////#include "BookRepository.h"
//#include "OdbcError.h"
//
//
//void testOdbcStatement(void)
//{
//    try {
//        std::cout << "création d'un environnement..\n";
//        OdbcEnvironment env;
//        assert(env.handle() != SQL_NULL_HENV);
//        std::cout << "> création d'un environnement avec succes\n\n";
//
//        std::string connectionString = "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\mylocaldb;Database=test3;Trusted_Connection=Yes;";
//        std::cout << "Test du second constructeur création d'une connection..\n";
//        OdbcConnection conn(env, connectionString, SQL_DRIVER_COMPLETE);   // test du second constructeur de la classe
//        assert(conn.handle() != SQL_NULL_HDBC);
//        std::cout << "> création d'une connexion avec succes\n\n";
//
//        std::cout << "> création d'un handle de statement..\n\n";
//        OdbcStatement stmt(conn.handle());
//        assert(stmt.handle() != SQL_NULL_HSTMT);
//
//        std::cout << "Handle de statement créé avec succes..\n\n";
//
//
//
//        // Insertion de données
//
//        conn.beginTransaction();
//
//        std::cout << "  > Insertion de donnees...\n";
//        stmt.directExecute("INSERT INTO TransTest (ID, Name) VALUES (18, 'valeur de ID=18')");
//        std::cout << "  > Donnees inserees avec succes.\n";
//
//        conn.rollback();
//
//        std::cout << "\n> Toutes les requetes directes ont ete executees avec succes.\n";
//    }
//    catch (std::exception& e) {
//        std::cerr << "> Erreur lors de l'exécution de testOdbcStatement...\n\n";
//        std::cerr << "Erreur :" << e.what();
//        assert(false);
//    }
//}
//
//int main()
//{
//    testOdbcStatement();
//
//    return 0;
//}



// test de directeExecute correcte 
//#include <cassert>
//#include <iostream>
//#include <windows.h>
//#include <vector>
//#include <string>
//#include <string.h>
//#include <random>
//#include "OdbcEnvironment.h"
//#include "OdbcConnection.h"
//#include "OdbcStatement.h"
////#include "BookRepository.h"
//#include "OdbcError.h"
//
//
//void testOdbcStatement(void)
//{
//    try {
//        std::cout << "création d'un environnement..\n";
//        OdbcEnvironment env;
//        assert(env.handle() != SQL_NULL_HENV);
//        std::cout << "> création d'un environnement avec succes\n\n";
//
//        std::string connectionString = "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\mylocaldb;Database=test3;Trusted_Connection=Yes;";
//        std::cout << "Test du second constructeur création d'une connection..\n";
//        OdbcConnection conn(env, connectionString, SQL_DRIVER_COMPLETE);   // test du second constructeur de la classe
//        assert(conn.handle() != SQL_NULL_HDBC);
//        std::cout << "> création d'une connexion avec succes\n\n";
//
//        std::cout << "> création d'un handle de statement..\n\n";
//        OdbcStatement stmt(conn.handle());
//        assert(stmt.handle() != SQL_NULL_HSTMT);
//
//        std::cout << "Handle de statement créé avec succes..\n\n";
//
//        // test de directExecute
//        std::cout << "Execution de requetes simples...\n";
//
//        // Création d'une table
//        std::cout << "  > Creation de la table 'TransTest'...\n";
//        stmt.directExecute("CREATE TABLE TransTest2 (ID INT, Name VARCHAR(50))");
//        std::cout << "  > Table 'TransTest' creee avec succes.\n";
//
//        // Insertion de données
//        std::cout << "  > Insertion de donnees...\n";
//        stmt.directExecute("INSERT INTO TransTest (ID, Name) VALUES (1, 'Test 1')");
//        std::cout << "  > Donnees inserees avec succes.\n";
//
//        // Insertion de données
//        std::cout << "  > Insertion de donnees...\n";
//        stmt.directExecute("INSERT INTO TransTest2 (ID, Name) VALUES (1, 'Test 1')");
//        std::cout << "  > Donnees inserees avec succes.\n";
//
//        // Mise à jour de données
//        std::cout << "  > Mise a jour des donnees...\n";
//        stmt.directExecute("UPDATE TransTest2 SET Name = 'Test 1 Modifie' WHERE ID = 1");
//        std::cout << "  > Donnees mises a jour avec succes.\n";
//
//        // Suppression de données
//        std::cout << "  > Suppression des donnees...\n";
//        stmt.directExecute("DELETE FROM TransTest WHERE ID = 100");
//        std::cout << "  > Donnees supprimees avec succes.\n";
//
//        //// Suppression de la table
//        //std::cout << "  > Suppression de la table 'TransTest'...\n";
//        //stmt.directExecute("DROP TABLE TransTest2");
//        //std::cout << "  > Table 'TransTest2' supprimee avec succes.\n";
//
//        std::cout << "\n> Toutes les requetes directes ont ete executees avec succes.\n";
//    }
//    catch (std::exception& e) {
//        std::cerr << "> Erreur lors de l'exécution de testOdbcStatement...\n\n";
//        std::cerr << "Erreur :" << e.what();
//        assert(false);
//    }
//}
//
//
//
//int main()
//{
//    testOdbcStatement();
//
//    return 0;
//}
//







// version de test des construction de connexion avec les deux constructeurs et de destruction de connexion

// Implement_ODBC.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
// 
//#include <cassert>
//#include <iostream>
//#include <windows.h>
//#include <vector>
//#include <string>
//#include <string.h>
//#include <random>
//#include "OdbcEnvironment.h"
//#include "OdbcConnection.h"
////#include "OdbcStatement.h"
////#include "BookRepository.h"
//#include "OdbcError.h"
//
//// Test de la classe 
//void testOdbcConnection()
//{
//    try {
//        std::cout << "création d'un environnement..\n";
//        OdbcEnvironment env;
//        assert(env.handle() != SQL_NULL_HENV);
//        std::cout << "> création d'un environnement avec succes\n\n";
//
//        std::string connectionString = "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\mylocaldb;Database=Database_Test_ODBC;Trusted_Connection=Yes;";
//        std::cout << "Test du second constructeur création d'une connection..\n";
//        OdbcConnection conn1(env, connectionString, SQL_DRIVER_COMPLETE);   // test du second constructeur de la classe
//        assert(conn1.handle() != SQL_NULL_HDBC);
//        std::cout << "> création d'une connexion avec succes\n\n";
//
//        std::cout << "Test du premier constructeur création d'une connection..\n";
//        std::string dsn = "Test_ODBC";
//        std::string user = "";
//        std::string password = "";
//        OdbcConnection conn2(env, dsn, user, password);   // test du premier constructeur de la classe
//        assert(conn2.handle() != SQL_NULL_HDBC);
//        std::cout << "> création d'une connexion avec succes\n\n";
//
//        std::cout << "Test de destruction..\n";
//        SQLHDBC conn3;
//
//        {
//            std::cout << "création d'une connexion..\n";
//            std::string connectionString = "DSN=Test_ODBC;";
//            std::string dsn = "Test_ODBC";
//            std::string user = "";
//            std::string password = "";
//            OdbcConnection conn4(env, dsn, user, password);   // test du premier constructeur de la classe
//            std::cout << "> création de la connexion à passer avec succes\n\n";
//            conn3 = conn4.handle();
//            assert(conn4.handle() != SQL_NULL_HANDLE);
//        } // Le destructeur de 'conn4' est appelé ici, libérant son handle.
//
//        SQLINTEGER version;
//        SQLRETURN ret = SQLGetConnectAttr(conn3, SQL_ATTR_ODBC_VERSION, &version, 0, nullptr);
//        assert(ret == SQL_INVALID_HANDLE);
//        std::cout << "  > SUCCES : Le handle est invalide apres destruction, comme attendu." << std::endl;
//    }
//    catch (std::exception& e) {
//        std::cerr << "> Erreur lors de l'exécution de testOdbcConnection...\n\n";
//        std::cerr << "Erreur :" << e.what();
//        assert(false);
//    }
//}
//
//
//int main() {
//
//    testOdbcConnection();
//
//    return 0;
//}
//
//














//// version de test pour OdbcEnvironment.h
//// Implement_ODBC.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
////
//
//
//#include <cassert>
//#include <iostream>
//#include <windows.h>
//#include <vector>
//#include <string>
//#include <string.h>
//#include <random>
//#include "OdbcEnvironment.h"
////#include "OdbcConnection.h"
////#include "OdbcStatement.h"
////#include "BookRepository.h"
//#include "OdbcError.h"
//
//// --- Début des tests pour OdbcEnvironment ---
//
//// Test 1 : Vérifie que le constructeur alloue un handle ODBC valide.
//void test_ConstructionReussie() {
//    std::cout << "Execution du test : test_ConstructionReussie" << std::endl;
//    try {
//        OdbcEnvironment env;
//        // Le handle ne doit pas être nul si la construction a réussi.
//        assert(env.handle() != SQL_NULL_HANDLE);
//        std::cout << "  > SUCCES : Le handle a ete alloue." << std::endl;
//    }
//    catch (const OdbcError& e) {
//        std::cerr << "  > ECHEC : Exception inattendue : " << e.what() << std::endl;
//        assert(false); // Force l'arrêt du programme en cas d'échec
//    }
//}
//
//// Test 2 : Vérifie que le destructeur libère le handle (principe RAII).
//void test_DestructionLibereHandle() {
//    std::cout << "Execution du test : test_DestructionLibereHandle..." << std::endl;
//    SQLHENV handle = SQL_NULL_HANDLE;
//
//    {
//        OdbcEnvironment env;
//        handle = env.handle();
//        assert(handle != SQL_NULL_HANDLE);
//    } // Le destructeur de 'env' est appelé ici, libérant le handle.
//
//    // Toute opération sur un handle libéré doit échouer.
//    // On s'attend à recevoir SQL_INVALID_HANDLE.
//    SQLINTEGER version;
//    SQLRETURN ret = SQLGetEnvAttr(handle, SQL_ATTR_ODBC_VERSION, &version, 0, nullptr);
//    assert(ret == SQL_INVALID_HANDLE);
//    std::cout << "  > SUCCES : Le handle est invalide apres destruction, comme attendu." << std::endl;
//}
//
//// Test 3 : Vérifie que la classe n'est pas copiable (vérification à la compilation).
//void test_NonCopie() {
//    std::cout << "Execution du test : test_NonCopie..." << std::endl;
//    // Le code suivant ne compilera pas, ce qui est le comportement attendu.
//    OdbcEnvironment env1;
//    OdbcEnvironment env2 = env1; // Erreur de compilation
//    OdbcEnvironment env3;
//    env3 = env1; // Erreur de compilation
//    std::cout << "  > SUCCES : La non-copiabilite est verifiee a la compilation." << std::endl;
//}
//
//
//int main() {
//    try {
//        std::cout << "--- Lancement des tests pour OdbcEnvironment ---" << std::endl;
//        test_ConstructionReussie();
//        test_DestructionLibereHandle();
//        test_NonCopie();
//        std::cout << "--- Tous les tests pour OdbcEnvironment ont reussi ---" << std::endl;
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Un test a echoue avec une exception : " << e.what() << std::endl;
//        return 1; // Termine avec un code d'erreur
//    }
//
//
//
//    return 0;
//}
//