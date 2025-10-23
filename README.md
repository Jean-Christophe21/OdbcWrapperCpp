Ce Projet implémente l'API ODBC et permet de se connecter à une base de donnée.
Le projet a été testé avec succes en utilisant la version localdb de Microsoft SQL Server.

* Note pour tester ce projet avec Localdb veuillez :
  - créer une source de connexion via l'application Source données ODBC en renseignant correctement les informations relatifs au serveur localdb et la base de donnée.
  - passer une chaine de connexion valide à l'instance de OdbcConnection comme par exemple : "Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\instance_name;Database=test_Database;Trusted_Connection=Yes;" ou utiliser le second constructeur de OdbcConnection qui prend un dsn comme par exemple : "Test_ODBC", Username et un password.

Toutefois il faut noter que le projet est en cours de développement veuillez l'utiliser avec précaution.
