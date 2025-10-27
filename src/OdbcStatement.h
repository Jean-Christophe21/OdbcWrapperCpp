#pragma once

class OdbcStatement {

public:
    OdbcStatement(SQLHDBC dbc);

    ~OdbcStatement();

    SQLHSTMT handle(void) const;

    void directExecute(const std::string& requete);


    void Prepare(const std::string requete);   // ne pas éviter d'execute() sinon la commande ne sera tout jouste pas exécutée

    void bindString(int pos, std::string value);

    void bindNullableString(int pos, const std::string* valuePtr);

    void bindInt(int pos, const int value);

    void bindNullableInt(int pos, int value);

    // ajout de bind date
    void bindDate(int pos, DATE_STRUCT date);

    void bindNullableDate(int pos, DATE_STRUCT* date);

    void execute() const;

    bool fetch() const;

    std::string getString(int col) const;

    int getInt(int col) const;

    //ajout de la méthode get Date
    DATE_STRUCT getDate(int col) const;

    void closeCursor() const;

private:
    SQLHSTMT stmt_ = nullptr;
    std::vector<std::string> ownedStrings_; // Stocke les chaînes pour garantir leur durée de vie pendant l'exécution de la requete
    std::vector<int> ownedInts_;            // Stocke les entiers pour garantir leur durée de vie
    std::vector<SQLLEN> indicators_;        // Pour valeurs NULL éventuelles ou longueurs 
    std::vector<DATE_STRUCT> ownedDates_;
};
