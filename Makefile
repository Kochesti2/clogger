# Compilatore
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -I/usr/include/postgresql

# Librerie da linkare (per PostgreSQL)
LIBS = -lpq

# Cartelle
SRC_DIR = src
INC_DIR = include

# Oggetti comuni (moduli condivisi)
OBJS_COMMON = $(SRC_DIR)/env_utils.o

# Programmi finali
TARGET_MAIN = clogger
TARGET_CLEAN = cleanup_table

# Tutti i target
all: $(TARGET_MAIN) $(TARGET_CLEAN)

# ====================
# Compilazione CLOGGER
# ====================
$(TARGET_MAIN): $(SRC_DIR)/clogger.o $(OBJS_COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# ==========================
# Compilazione CLEANUP_TABLE
# ==========================
$(TARGET_CLEAN): $(SRC_DIR)/cleanup_table.o $(OBJS_COMMON)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# ====================
# Regola generica .cpp
# ====================
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Questa regola serve per file senza header corrispondenti (es. clogger.cpp)
$(SRC_DIR)/clogger.o: $(SRC_DIR)/clogger.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRC_DIR)/cleanup_table.o: $(SRC_DIR)/cleanup_table.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ==========
# Pulizia
# ==========
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET_MAIN) $(TARGET_CLEAN)

