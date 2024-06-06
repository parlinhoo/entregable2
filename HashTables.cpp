#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <chrono> 
#include <iostream> 
#include <stdexcept> // Para std::length_error

#include "clases/DoubleClosedHashTable.h"
#include "clases/HashTableOpen.h"
#include "clases/UserData.h"
#include "clases/LinearHashTable.h"
#include "clases/QuadraticHashTable.h"

// Función para cargar el archivo CSV y almacenar los datos en la tabla hash
std::optional<std::vector<UserData>*> loadCSV(const std::string& filecsv) {
    std::ifstream file(filecsv);
    if (!file.is_open()) {
        std::cout << "Error al abrir el archivo CSV" << std::endl;
        return std::nullopt;
    }

    std::vector<UserData>* data_vector_ptr = new std::vector<UserData>();
    std::string line;

    getline(file, line); // Ignoramos la primera línea (cabecera)

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string university, userName, createdAt, idString;
        unsigned long long int userID;
        double decimal;
        long long int numTweets, friendsCount, followersCount;

        getline(ss, university, ','); 
        getline(ss, idString, ',');
        sscanf(idString.c_str(), "%lf", &decimal);
        userID = (unsigned long long int) decimal;
        getline(ss, userName, ',');
        ss >> numTweets;
        ss.ignore(); 
        ss >> friendsCount; 
        ss.ignore(); 
        ss >> followersCount;
        ss.ignore(); 
        getline(ss, createdAt, ',');

        university.erase(0, university.find_first_not_of(" \t\r\n"));
        university.erase(university.find_last_not_of(" \t\r\n") + 1);
        userName.erase(0, userName.find_first_not_of(" \t\r\n"));
        userName.erase(userName.find_last_not_of(" \t\r\n") + 1);
        createdAt.erase(0, createdAt.find_first_not_of(" \t\r\n"));
        createdAt.erase(createdAt.find_last_not_of(" \t\r\n") + 1);

        UserData userData;
        userData.university = university;
        userData.userID = userID;
        userData.userName = userName;
        userData.numTweets = numTweets;
        userData.friendsCount = friendsCount;
        userData.followersCount = followersCount;
        userData.createdAt = createdAt;

        data_vector_ptr->push_back(userData);
    }
    return data_vector_ptr;
}

class OpenHashTableByID : public HashTableOpen<unsigned long long int, UserData> {
private:
    size_t hash(unsigned long long int userId) {
        return userId % this->vector_size;
    }
public:
    OpenHashTableByID(size_t table_size) : HashTableOpen<unsigned long long int, UserData>(table_size) {}
};

class OpenHashTableByName : public HashTableOpen<std::string, UserData> {
private:
    size_t hash(std::string username) {
        int num = 0;
        int a = 127;
        for (const char character : username) {
            num = (a*num + character) % this->vector_size;
        }
        return num;
    }   
public:
    OpenHashTableByName(size_t table_size) : HashTableOpen<std::string, UserData>(table_size) {}
};

class DoubleClosedHashTableId : public DoubleClosedHashTable<unsigned long long int, UserData> {
private:
    size_t first_hash(unsigned long long key) override {
        return key % this->vector_size;     
    }

    size_t second_hash(unsigned long long key) override {
        return 9967 - (key % 9967); 
    }
public:
    DoubleClosedHashTableId(size_t initial_size) : DoubleClosedHashTable<unsigned long long, UserData>(initial_size) {}
};

class DoubleClosedHashTableName : public DoubleClosedHashTable<std::string, UserData> {
private:
    size_t first_hash(std::string key) override {
        int num = 0;
        int a = 127;
        for (const char character : key) {
            num = (a*num + character) % this->vector_size;
        }
        return num;
    }

    size_t second_hash(std::string key) override {
        int sum = 0;
        for (const char character : key) {
            sum += (int) character; 
        }
        return sum % this->vector_size;
    }
public:
    DoubleClosedHashTableName(size_t initial_size) : DoubleClosedHashTable<std::string, UserData>(initial_size) {}
};

class LinearHashID : public LinearHashTable<unsigned long long int, UserData> {
private:
    size_t hash(unsigned long long key, int tries) override {
        return (key + tries) % this->vector_size;
    }
public:
    LinearHashID(size_t initial_size) : LinearHashTable<unsigned long long int, UserData>(initial_size) {}
};

class LinearHashName : public LinearHashTable<std::string, UserData> {
private:
    size_t hash(std::string key, int tries) override {
        int num = 0;
        int a = 127;
        for (const char character : key) {
            num = (a*num + character) % this->vector_size;
        }
        return (num + tries) % this->vector_size;
    }
public:
    LinearHashName(size_t initial_size) : LinearHashTable<std::string, UserData>(initial_size) {}
};

class QuadraticHashID : public QuadraticHashTable<unsigned long long int, UserData> {
private:
    size_t hash(unsigned long long key, int tries) override {
        return (key + tries * tries) % this->vector_size;
    }
public:
    QuadraticHashID(size_t initial_size) : QuadraticHashTable<unsigned long long int, UserData>(initial_size) {}
};

class QuadraticHashName : public QuadraticHashTable<std::string, UserData> {
private:
    size_t hash(std::string key, int tries) override {
        size_t hash_val = 0;
        for (char c : key) {
            hash_val = (hash_val * 127 + c) % this->vector_size;
        }
        return (hash_val + tries * tries) % this->vector_size;
    }
public:
    QuadraticHashName(size_t initial_size) : QuadraticHashTable<std::string, UserData>(initial_size) {}
};

std::vector<int>* get_random_array(int vector_size, int min, int max) {
    srand(100); // numero magico para el rand 
    std::vector<int>* vec = new std::vector<int>();
    for (int i = 0; i < vector_size; i++) {
        vec->push_back(rand() % (max - min + 1) + min);
    }
    return vec;
}

template<typename HashTable, typename Key>
double realizarBusquedas(HashTable& table, const std::vector<Key>& searchKeys, int numSearches) {
    std::chrono::nanoseconds total_duration(0);
    for (int i = 0; i < numSearches; ++i) {
        if (i >= searchKeys.size()) {
            std::cerr << "Error: no hay suficientes claves de busqueda." << std::endl;
            return -1.0;
        }
        auto start_time = std::chrono::steady_clock::now();
        auto search_result = table.get(searchKeys[i]);
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        total_duration += duration; 
    }
    return static_cast<double>(total_duration.count()); 
}

int main() {
    // Crear tablas hash
    OpenHashTableByID hashTableID(24989);
    OpenHashTableByName hashTableName(24989);
    LinearHashID linearHashID(42139);
    LinearHashName linearHashName(42139);
    QuadraticHashID quadHashID(42139); 
    QuadraticHashName quadHashName(42139);
    DoubleClosedHashTableId doubleClosedHashTableId(42139); 
    DoubleClosedHashTableName doubleClosedHashTableName(42139);

    // Cargar el CSV
    std::string filecsv = "universities_followers.csv";
    auto load_result = loadCSV(filecsv);
    if (!load_result.has_value()) return 1;
    std::vector<UserData> data_vector = *load_result.value();

    for (const UserData& user : data_vector) {
        hashTableID.put(user.userID, user);
        hashTableName.put(user.userName, user);
        linearHashID.put(user.userID, user);
        linearHashName.put(user.userName, user);
        quadHashID.put(user.userID, user);
        quadHashName.put(user.userName, user);
        doubleClosedHashTableId.put(user.userID, user);
        doubleClosedHashTableName.put(user.userName, user);
    }

    // Definir claves para busqueda

    // Se definen arreglos de 
    const int numSearches = 2000; 
    const int n_experimentos = 20;

    auto num_vector = *get_random_array(numSearches, 0, 21069);

    std::vector<unsigned long long int> searchIDs;
    std::vector<std::string> searchNames;

    for (int i = 0; i < numSearches; i++) {
        UserData data = data_vector[num_vector[i]];
        searchIDs.push_back(data.userID);
        searchNames.push_back(data.userName);
    }

    std::ofstream outputfile;
    outputfile.open("resultados.csv");

    const std::string title = "estructura;dataset;n_experimento;n_consultas;tiempo_ejecucion\n";
    const std::string format = "%s;%s;%i;%i;%.4lf\n";

    outputfile << title;

    char buffer[100];

    for (int i = 1; i <= n_experimentos; i++) {
        auto promIDOpen = realizarBusquedas(hashTableID, searchIDs, numSearches);
        auto promNameOpen = realizarBusquedas(hashTableName, searchNames, numSearches);
        sprintf(buffer, format.c_str(), "open", "id", i, numSearches, promIDOpen);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "open", "name", i, numSearches, promNameOpen);
        outputfile << buffer;
    }
    for (int i = 1; i <= n_experimentos; i++) {
        auto promIDLinear = realizarBusquedas(linearHashID, searchIDs, numSearches);
        auto promNameLinear = realizarBusquedas(linearHashName, searchNames, numSearches);
        sprintf(buffer, format.c_str(), "closed_linear", "id", i, numSearches, promIDLinear);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "closed_linear", "name", i, numSearches, promNameLinear);
        outputfile << buffer;
    }
    for (int i = 1; i <= n_experimentos; i++) {
        auto promIDQuadratic = realizarBusquedas(quadHashID, searchIDs, numSearches);
        auto promNameQuadratic = realizarBusquedas(quadHashName, searchNames, numSearches);
        sprintf(buffer, format.c_str(), "closed_quadratic", "id", i, numSearches, promIDQuadratic);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "closed_quadratic", "name", i, numSearches, promNameQuadratic);
        outputfile << buffer;
    }
    for (int i = 1; i <= n_experimentos; i++) {
        auto promIDDouble = realizarBusquedas(doubleClosedHashTableId, searchIDs, numSearches);
        auto promNameDouble = realizarBusquedas(doubleClosedHashTableName, searchNames, numSearches);
        sprintf(buffer, format.c_str(), "closed_double", "id", i, numSearches, promIDDouble);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "closed_double", "name", i, numSearches, promNameDouble);
        outputfile << buffer;
    }

    // Liberar memoria del vector
    delete load_result.value();

    return 0;
}
