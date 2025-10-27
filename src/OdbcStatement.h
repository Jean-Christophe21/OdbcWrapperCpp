#pragma once

class OdbcStatement {

public:
    OdbcStatement(SQLHDBC dbc);

    ~OdbcStatement();

    SQLHSTMT handle(void) const;

    void directExecute(const std::string& requete);


    void Prepare(const std::string requete);   // ne pas �viter d'execute() sinon la commande ne sera tout jouste pas ex�cut�e

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

    //ajout de la m�thode get Date
    DATE_STRUCT getDate(int col) const;

    void closeCursor() const;

private:
    SQLHSTMT stmt_ = nullptr;
    std::vector<std::string> ownedStrings_; // Stocke les cha�nes pour garantir leur dur�e de vie pendant l'ex�cution de la requete
    std::vector<int> ownedInts_;            // Stocke les entiers pour garantir leur dur�e de vie
    std::vector<SQLLEN> indicators_;        // Pour valeurs NULL �ventuelles ou longueurs 
    std::vector<DATE_STRUCT> ownedDates_;
};
