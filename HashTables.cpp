#include <array>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
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
template<class key, class value>
double realizarBusquedaSTL(std::unordered_map<key, value>& table, const std::vector<key>& searchKeys, int numSearches) {
    std::chrono::nanoseconds total_duration(0);
    for (int i = 0; i < numSearches; ++i) {
        if (i >= searchKeys.size()) {
            std::cerr << "Error: no hay suficientes claves de busqueda." << std::endl;
            return -1.0;
        }
        auto start_time = std::chrono::steady_clock::now();
        auto search_result = table.find(searchKeys[i]);
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        total_duration += duration; 
    }
    return static_cast<double>(total_duration.count()); 
}
template <class hashmap>
std::vector<double>* time_inserts_id(hashmap& map, std::vector<UserData>& vector) {
    std::chrono::nanoseconds total_duration(0);
    std::vector<double>* time_vector = new std::vector<double>();
    for (int i = 0; i < vector.size(); i++) {
        auto start = std::chrono::steady_clock::now();
        map.put(vector[i].userID, vector[i]);
        auto end = std::chrono::steady_clock::now();
        total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        if ((i+1) == 1000 || (i+1) % 5000 == 0) {
            time_vector->push_back((double) total_duration.count());
        }
    }
    return time_vector;
}

template <class hashmap>
std::vector<double>* time_inserts_name(hashmap& map, std::vector<UserData>& vector) {
    std::chrono::nanoseconds total_duration(0);
    std::vector<double>* time_vector = new std::vector<double>();
    for (int i = 0; i < vector.size(); i++) {
        auto start = std::chrono::steady_clock::now();
        map.put(vector[i].userName, vector[i]);
        auto end = std::chrono::steady_clock::now();
        total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        if ((i+1) == 1000 || (i+1) % 5000 == 0) {
            time_vector->push_back((double) total_duration.count());
        }
    }
    return time_vector;
}

template <class key>
std::vector<double>* time_stl_inserts_id(std::unordered_map<key, UserData>& map, std::vector<UserData>& vector) {
    std::chrono::nanoseconds total_duration(0);
    std::vector<double>* time_vector = new std::vector<double>();
    for (int i = 0; i < vector.size(); i++) {
        auto start = std::chrono::steady_clock::now();
        map.insert({vector[i].userID, vector[i]});
        auto end = std::chrono::steady_clock::now();
        total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        if ((i+1) == 1000 || (i+1) % 5000 == 0) {
            time_vector->push_back((double) total_duration.count());
        }
    }
    return time_vector;
}

template <class key>
std::vector<double>* time_stl_inserts_name(std::unordered_map<key, UserData>& map, std::vector<UserData>& vector) {
    std::chrono::nanoseconds total_duration(0);
    std::vector<double>* time_vector = new std::vector<double>();
    for (int i = 0; i < vector.size(); i++) {
        auto start = std::chrono::steady_clock::now();
        map.insert({vector[i].userName, vector[i]});
        auto end = std::chrono::steady_clock::now();
        total_duration += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        if ((i+1) == 1000 || (i+1) % 5000 == 0) {
            time_vector->push_back((double) total_duration.count());
        }
    }
    return time_vector;
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
    std::unordered_map<unsigned long long int, UserData> stlId;
    std::unordered_map<std::string, UserData> stlName;

    // Cargar el CSV
    std::string filecsv = "universities_followers.csv";
    auto load_result = loadCSV(filecsv);
    if (!load_result.has_value()) return 1;
    std::vector<UserData> data_vector = *load_result.value();
    
    std::ofstream outputfileinserts;
    outputfileinserts.open("inserts.csv");

    const std::string title_inserts = "estructura;dataset;tiempo1000;tiempo5000;tiempo10000;tiempo15000;tiempo20000\n";
    const std::string format_inserts = "%s;%s;%.4lf;%.4lf;%.4lf;%.4lf;%.4lf\n";

    outputfileinserts << title_inserts;

    std::vector<double>* times_buffer;
    char buffer[100];
 
    times_buffer = time_inserts_id<OpenHashTableByID>(hashTableID, data_vector);
    sprintf(buffer, format_inserts.c_str(), "open", "id", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_inserts_name<OpenHashTableByName>(hashTableName, data_vector);
    sprintf(buffer, format_inserts.c_str(), "open", "name", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_inserts_id<LinearHashID>(linearHashID, data_vector);
    sprintf(buffer, format_inserts.c_str(), "closed_linear", "id", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_inserts_name<LinearHashName>(linearHashName, data_vector);
    sprintf(buffer, format_inserts.c_str(), "closed_linear", "name", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_inserts_id<QuadraticHashID>(quadHashID, data_vector);
    sprintf(buffer, format_inserts.c_str(), "closed_quadratic", "id", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_inserts_name<QuadraticHashName>(quadHashName, data_vector);
    sprintf(buffer, format_inserts.c_str(), "closed_quadratic", "name", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_inserts_id<DoubleClosedHashTableId>(doubleClosedHashTableId, data_vector);
    sprintf(buffer, format_inserts.c_str(), "closed_double", "id", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_inserts_name<DoubleClosedHashTableName>(doubleClosedHashTableName, data_vector);
    sprintf(buffer, format_inserts.c_str(), "closed_double", "name", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_stl_inserts_id(stlId, data_vector);
    sprintf(buffer, format_inserts.c_str(), "stl_unordered_map", "id", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;

    times_buffer = time_stl_inserts_name(stlName, data_vector);
    sprintf(buffer, format_inserts.c_str(), "stl_unordered_map", "name", (*times_buffer)[0], (*times_buffer)[1], (*times_buffer)[2], (*times_buffer)[3], (*times_buffer)[4]);
    outputfileinserts << buffer;
    delete times_buffer;


    // Se definen arreglos de 
    const int numSearches = 2000; 
    const int n_experimentos = 20;
    
    auto num_vector = *get_random_array(numSearches*n_experimentos, 0, 21069);

    std::vector<std::vector<unsigned long long int>> searchIDs;
    std::vector<std::vector<std::string>> searchNames;

    std::ifstream random_str;
    random_str.open("randomstrings.txt");
    
    std::vector<std::vector<std::string>> random_strings;
    std::vector<std::vector<unsigned long long int>> random_ids;

    searchIDs.resize(n_experimentos);
    searchNames.resize(n_experimentos);
    random_strings.resize(n_experimentos);
    random_ids.resize(n_experimentos);

    for (int i = 0; i < n_experimentos; i++) {
        for (int j = 0; j < numSearches; j++) {
            UserData data = data_vector[num_vector[i*numSearches + j]];
            searchIDs[i].push_back(data.userID);
            searchNames[i].push_back(data.userName);
            random_str.getline(buffer, 100);
            random_strings[i].push_back(std::string(buffer));
            random_ids[i].push_back(rand() + 1);
        }
    }
    
    std::ofstream outputfile;
    outputfile.open("resultados.csv");

    std::ofstream inexistent;
    inexistent.open("resultados_inexistentes.csv");

    const std::string title = "estructura;dataset;n_experimento;n_consultas;tiempo_ejecucion\n";
    const std::string format = "%s;%s;%i;%i;%.4lf\n";

    outputfile << title;

    for (int i = 0; i < n_experimentos; i++) {
        auto promIDOpen = realizarBusquedas(hashTableID, searchIDs[i], numSearches);
        auto promNameOpen = realizarBusquedas(hashTableName, searchNames[i], numSearches);
        sprintf(buffer, format.c_str(), "open", "id", i+1, numSearches, promIDOpen);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "open", "name", i+1, numSearches, promNameOpen);
        outputfile << buffer;

        auto totalInexIDOpen = realizarBusquedas(hashTableID, random_ids[i], numSearches);
        auto totalInexNameOpen = realizarBusquedas(hashTableName, random_strings[i], numSearches);
        sprintf(buffer, format.c_str(), "open", "id", i+1, numSearches, totalInexIDOpen);
        inexistent << buffer;
        sprintf(buffer, format.c_str(), "open", "name", i+1, numSearches, totalInexNameOpen);
        inexistent << buffer;
    }
    for (int i = 0; i < n_experimentos; i++) {
        auto promIDLinear = realizarBusquedas(linearHashID, searchIDs[i], numSearches);
        auto promNameLinear = realizarBusquedas(linearHashName, searchNames[i], numSearches);
        sprintf(buffer, format.c_str(), "closed_linear", "id", i+1, numSearches, promIDLinear);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "closed_linear", "name", i+1, numSearches, promNameLinear);
        outputfile << buffer;

        auto totalInexIDLinear = realizarBusquedas(linearHashID, random_ids[i], numSearches);
        auto totalInexNameLinear = realizarBusquedas(linearHashName, random_strings[i], numSearches);
        sprintf(buffer, format.c_str(), "closed_linear", "id", i+1, numSearches, totalInexIDLinear);
        inexistent << buffer;
        sprintf(buffer, format.c_str(), "closed_linear", "name", i+1, numSearches, totalInexNameLinear);
        inexistent << buffer;
    }
    for (int i = 0; i < n_experimentos; i++) {
        auto promIDQuadratic = realizarBusquedas(quadHashID, searchIDs[i], numSearches);
        auto promNameQuadratic = realizarBusquedas(quadHashName, searchNames[i], numSearches);
        sprintf(buffer, format.c_str(), "closed_quadratic", "id", i+1, numSearches, promIDQuadratic);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "closed_quadratic", "name", i+1, numSearches, promNameQuadratic);
        outputfile << buffer;

        auto totalInexIDQuad = realizarBusquedas(quadHashID, random_ids[i], numSearches);
        auto totalInexNameQuad = realizarBusquedas(quadHashName, random_strings[i], numSearches);
        sprintf(buffer, format.c_str(), "closed_quadratic", "id", i+1, numSearches, totalInexIDQuad);
        inexistent << buffer;
        sprintf(buffer, format.c_str(), "closed_quadratic", "name", i+1, numSearches, totalInexNameQuad);
        inexistent << buffer;
    }
    for (int i = 0; i < n_experimentos; i++) {
        auto promIDDouble = realizarBusquedas(doubleClosedHashTableId, searchIDs[i], numSearches);
        auto promNameDouble = realizarBusquedas(doubleClosedHashTableName, searchNames[i], numSearches);
        sprintf(buffer, format.c_str(), "closed_double", "id", i+1, numSearches, promIDDouble);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "closed_double", "name", i+1, numSearches, promNameDouble);
        outputfile << buffer;

        auto totalInexIDDouble = realizarBusquedas(doubleClosedHashTableId, random_ids[i], numSearches);
        auto totalInexNameDouble = realizarBusquedas(doubleClosedHashTableName, random_strings[i], numSearches);
        sprintf(buffer, format.c_str(), "closed_double", "id", i+1, numSearches, totalInexIDDouble);
        inexistent << buffer;
        sprintf(buffer, format.c_str(), "closed_double", "name", i+1, numSearches, totalInexNameDouble);
        inexistent << buffer;
    }
    for (int i = 0; i < n_experimentos; i++) {
        auto promIDSTL = realizarBusquedaSTL<unsigned long long int, UserData>(stlId, searchIDs[i], numSearches);
        auto promNameSTL = realizarBusquedaSTL<std::string, UserData>(stlName, searchNames[i], numSearches);
        sprintf(buffer, format.c_str(), "stl_unordered_map", "id", i+1, numSearches, promIDSTL);
        outputfile << buffer;
        sprintf(buffer, format.c_str(), "stl_unordered_map", "name", i+1, numSearches, promNameSTL);
        outputfile << buffer;

        auto totalInexIDSTL = realizarBusquedaSTL<unsigned long long int, UserData>(stlId, random_ids[i], numSearches);
        auto totalInexNameSTL = realizarBusquedaSTL<std::string, UserData>(stlName, random_strings[i], numSearches);
        sprintf(buffer, format.c_str(), "stl_unordered_map", "id", i+1, numSearches, totalInexIDSTL);
        inexistent << buffer;
        sprintf(buffer, format.c_str(), "stl_unordered_map", "name", i+1, numSearches, totalInexNameSTL);
        inexistent << buffer;
    }



    // Liberar memoria del vector
    delete load_result.value();

    return 0;
}
