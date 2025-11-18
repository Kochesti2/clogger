#!/bin/bash

# === CONFIGURAZIONE ===
PROJECT_DIR="/home/xxx/Sources/LogMe/clogger" #<--- correggi qui
BIN="$PROJECT_DIR/cleanup_table"
LOG_DIR="$PROJECT_DIR/logs"
LOG_FILE="$LOG_DIR/cleanup.log"

#Alle 23:59 di ogni giorno
CRON_RULE="59 23 * * * $BIN >> $LOG_FILE 2>&1"
#Ogni minuto
#CRON_RULE="* * * * * $BIN >> $LOG_FILE 2>&1"


echo "== Installazione cron job: cleanup_table =="

# 1. Verifica percorso eseguibile
if [ ! -f "$BIN" ]; then
    echo "ERRORE: eseguibile non trovato:"
    echo "       $BIN"
    echo "Compila prima con 'make' oppure controlla il percorso."
    exit 1
fi

# 2. Crea la cartella logs se non esiste
if [ ! -d "$LOG_DIR" ]; then
    echo "Creo cartella logs in $LOG_DIR"
    mkdir -p "$LOG_DIR"
fi

# 3. Controlla se la regola cron esiste già
( crontab -l | grep -F "$BIN" ) >/dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "La regola cron per cleanup_table esiste già, non la reinserisco."
    exit 0
fi

# 4. Installa la nuova entry cron
echo "$CRON_RULE" | crontab -l 2>/dev/null | {
    cat
    # aggiungi la regola
    if ! grep -F "$CRON_RULE" >/dev/null 2>&1; then
        echo "$CRON_RULE"
    fi
} | crontab -

echo "Cron job installato correttamente!"
echo "Regola:"
echo "  $CRON_RULE"
echo "Log:"
echo "  $LOG_FILE"

