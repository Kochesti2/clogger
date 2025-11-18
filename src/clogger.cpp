#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cctype>
#include <libpq-fe.h>
#include "env_utils.h"

bool is_valid_ean13(const std::string &code) {
    if (code.size() != 13) return false;
    for (char c : code)
        if (!std::isdigit((unsigned char)c)) return false;

    int sum_odd = 0, sum_even = 0;
    for (int i = 0; i < 12; ++i) {
        int d = code[i] - '0';
        if ((i % 2) == 0) sum_odd += d;  // pos 1,3,5...
        else              sum_even += d; // pos 2,4,6...
    }
    int check = (10 - ((sum_odd + 3 * sum_even) % 10)) % 10;
    return check == (code[12] - '0');
}

int main() {
    // Legge la connection string dal .env
    const std::string envPath = ".env";
    std::string host = getEnvVar("DB_HOST", envPath);
    std::cout << host << std::endl;
    std::string name = getEnvVar("DB_NAME", envPath);
    std::string user = getEnvVar("DB_USER", envPath);
    std::string pass = getEnvVar("DB_PASSWORD", envPath);

    std::string conninfo =
        "host=" + host +
        " dbname=" + name +
        " user=" + user +
        " password=" + pass;

    PGconn *conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connessione fallita: " << PQerrorMessage(conn);
        PQfinish(conn);
        return 1;
    }

    std::cout << "Pronto a leggere codici EAN-13...\n";

    while (true) {
        std::string barcode;
        if (!std::getline(std::cin, barcode)) break;  // EOF => esce

        // togli eventuale \r finale (da alcuni scanner)
        if (!barcode.empty() && barcode.back() == '\r')
            barcode.pop_back();

        if (barcode.empty()) continue;

        if (!is_valid_ean13(barcode)) {
            std::cout << "EAN-13 non valido\n";
            continue;
        }

        // 1) verifica esistenza in users
        const char *paramsUser[1] = { barcode.c_str() };
        PGresult *res = PQexecParams(
            conn,
            "SELECT count(*) FROM users WHERE barcode = $1",
            1, nullptr, paramsUser, nullptr, nullptr, 0
        );

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            std::cerr << "Errore SELECT users: " << PQerrorMessage(conn);
            PQclear(res);
            continue;
        }

        int count = std::stoi(PQgetvalue(res, 0, 0));
        PQclear(res);

        if (count == 0) {
            std::cout << "Errore codice inserito non esiste\n";
            continue; // torna in attesa del prossimo codice
        }

        // 2) leggi ultima direction per quel barcode
        res = PQexecParams(
            conn,
            "SELECT direction FROM log WHERE barcode = $1 "
            "ORDER BY event_time DESC LIMIT 1",
            1, nullptr, paramsUser, nullptr, nullptr, 0
        );

        std::string nextDir = "CHECKIN";
        if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) == 1) {
            std::string lastDir = PQgetvalue(res, 0, 0);
            if (lastDir == "CHECKIN")
                nextDir = "CHECKOUT";
            else
                nextDir = "CHECKIN";
        }
        PQclear(res);

        // 3) inserisci nella tabella log
        const char *paramsLog[2] = { barcode.c_str(), nextDir.c_str() };
        res = PQexecParams(
            conn,
            "INSERT INTO log(barcode, direction) VALUES($1, $2::access_direction)",
            2, nullptr, paramsLog, nullptr, nullptr, 0
        );

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Errore INSERT: " << PQerrorMessage(conn);
        } else {
            std::cout << "Inserito " << barcode << " -> " << nextDir << "\n";
        }

        PQclear(res);
    }

    PQfinish(conn);
    return 0;
}

