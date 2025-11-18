#include <iostream>
#include <libpq-fe.h>
#include "env_utils.h"
#include <unistd.h>
#include <limits.h>
#include <string>

std::string getExecutableDir() {
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    if (len != -1) {
        buffer[len] = '\0';
        std::string fullPath(buffer);
        return fullPath.substr(0, fullPath.find_last_of("/"));
    }
    return "";
}

int main() {
    // Legge la connection string dal .env
    const std::string envPath = getExecutableDir() + "/.env";
    std::string host = getEnvVar("DB_HOST", envPath);
    std::string name = getEnvVar("DB_NAME", envPath);
    std::string user = getEnvVar("DB_USER", envPath);
    std::string pass = getEnvVar("DB_PASSWORD", envPath);

    std::string conninfo =
        "host=" + host +
        " dbname=" + name +
        " user=" + user +
        " password=" + pass;

    if (conninfo.empty()) {
        std::cerr << "DB_CONNECTION non trovata nel file .env\n";
        return 1;
    }

    PGconn *conn = PQconnectdb(conninfo.c_str());

    if (PQstatus(conn) == CONNECTION_BAD) {
        std::cerr << "Errore di connessione: " << PQerrorMessage(conn);
        PQfinish(conn);
        return 1;
    }

    const char* query = "DELETE FROM log WHERE 1=1;";

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Errore esecuzione query: " << PQerrorMessage(conn);
        PQclear(res);
        PQfinish(conn);
        return 1;
    }

    PQclear(res);
    PQfinish(conn);

    std::cout << "Tabella pulita con successo.\n";
    return 0;
}

