// env_utils.h
#pragma once

#include <string>

// Legge il valore associato a "key" dal file .env.
// envPath di default è ".env", ma puoi passare un percorso diverso.
std::string getEnvVar(const std::string& key, const std::string& envPath = ".env");

