#include <cstdio>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include <chrono> // Para medir tiempo de busqueda 

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
        // Obtener string de ID
        getline(ss, idString, ',');
        // Transformarlo a double ya que soporta notacion cientifica
        sscanf(idString.c_str(), "%lf", &decimal);
        // Castearlo a int por comodidad
        userID = (unsigned long long int) decimal;
        getline(ss, userName, ',');
        ss >> numTweets;
        ss.ignore(); 
        ss >> friendsCount; 
        ss.ignore(); 
        ss >> followersCount;
        ss.ignore(); 
        getline(ss, createdAt, ',');

        // Eliminar espacios en blanco al inicio y al final de los datos
        university.erase(0, university.find_first_not_of(" \t\r\n"));
        university.erase(university.find_last_not_of(" \t\r\n") + 1);
        userName.erase(0, userName.find_first_not_of(" \t\r\n"));
        userName.erase(userName.find_last_not_of(" \t\r\n") + 1);
        createdAt.erase(0, createdAt.find_first_not_of(" \t\r\n"));
        createdAt.erase(createdAt.find_last_not_of(" \t\r\n") + 1);

        // Crear un objeto UserData con los datos
        UserData userData;
        userData.university = university;
        userData.userID = userID;
        userData.userName = userName;
        userData.numTweets = numTweets;
        userData.friendsCount = friendsCount;
        userData.followersCount = followersCount;
        userData.createdAt = createdAt;

        // Insertar en la tabla hash abierta
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

    // Realizar búsquedas y medir tiempos
    std::cout << "|---------Tabla Abierta----------|" << std::endl;
    unsigned long long int searchID = 414942137;
    auto start_time1 = std::chrono::steady_clock::now();
    auto search_result1 = hashTableID.get(searchID);
    auto end_time1 = std::chrono::steady_clock::now();
    auto elapsed_time1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time1 - start_time1).count();
    if (search_result1.has_value()) {
        UserData found_user1 = search_result1.value();
        std::cout << "Encontrado por ID: " << found_user1.userName + " (" << found_user1.userID << ")" << std::endl;
        std::cout << "-Tiempo de busqueda por ID: " << elapsed_time1 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por ID: " << searchID << std::endl;
    }

    // Buscar un nombre de usuario que debería estar presente
    std::string searchName = "SantillanaLAB"; // Ejemplo de nombre que debería estar en el CSV
    // Medir el tiempo antes de realizar la búsqueda
    auto start_time2 = std::chrono::steady_clock::now();
    auto search_result2 = hashTableName.get(searchName);
    // Medir el tiempo después de completar la búsqueda
    auto end_time2 = std::chrono::steady_clock::now();
    // Calcular la diferencia de tiempo para obtener el tiempo transcurrido en milisegundos
    auto elapsed_time2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time2 - start_time2).count();
    UserData found_user2;
    if (search_result2.has_value()) {
        found_user2 = search_result2.value();
        std::cout << "Encontrado por Nombre: " << found_user2.userName + " (" << found_user2.userID << ")" << std::endl; // Cambiado a userID
        // Imprimir el tiempo transcurrido
        std::cout << "-Tiempo de busqueda por Nombre: " << elapsed_time2 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por Nombre: " << searchName << std::endl;
    }

    std::cout << "|---------Tabla Lineal---------|" << std::endl;
    auto start_time3 = std::chrono::steady_clock::now();
    auto search_result3 = linearHashID.get(searchID);
    auto end_time3 = std::chrono::steady_clock::now();
    auto elapsed_time3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time3 - start_time3).count();
    if (search_result3.has_value()) {
        UserData found_user3 = search_result3.value();
        std::cout << "Encontrado por ID: " << found_user3.userName + " (" << found_user3.userID << ")" << std::endl; 
        std::cout << "-Tiempo de busqueda por ID: " << elapsed_time3 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por ID: " << searchID << std::endl;
    }

    // Buscar un nombre de usuario que debería estar presente
    std::string searchName_v2 = "ComercDinamarca"; // Ejemplo de nombre que debería estar en el CSV
    auto start_time4 = std::chrono::steady_clock::now();
    auto search_result4 = hashTableName.get(searchName_v2);
    // Medir el tiempo después de completar la búsqueda
    auto end_time4 = std::chrono::steady_clock::now();
    // Calcular la diferencia de tiempo para obtener el tiempo transcurrido en milisegundos
    auto elapsed_time4 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time4 - start_time4).count();
    UserData found_user4;
    if (search_result4.has_value()) {
        found_user4 = search_result4.value();
        std::cout << "Encontrado por Nombre: " << found_user4.userName + " (" << found_user4.userID << ")" << std::endl; // Cambiado a userID
        // Imprimir el tiempo transcurrido
        std::cout << "-Tiempo de busqueda por Nombre: " << elapsed_time4 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por Nombre: " << searchName_v2 << std::endl;
    }
    
    std::cout << "|---------Tabla Cuadratica---------|" << std::endl;
    auto start_time5 = std::chrono::steady_clock::now();
    auto search_result5 = quadHashID.get(searchID);
    auto end_time5 = std::chrono::steady_clock::now();
    auto elapsed_time5 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time5 - start_time5).count();
    if (search_result5.has_value()) {
        UserData found_user5 = search_result5.value();
        std::cout << "Encontrado por ID: " << found_user5.userName + " (" << found_user5.userID << ")" << std::endl; 
        std::cout << "-Tiempo de busqueda por ID: " << elapsed_time5 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por ID: " << searchID << std::endl;
    }

    // Buscar un nombre de usuario que debería estar presente
    std::string searchName_v3 = "ComercDinamarca"; // Ejemplo de nombre que debería estar en el CSV
    auto start_time6 = std::chrono::steady_clock::now();
    auto search_result6 = quadHashName.get(searchName_v3);
    // Medir el tiempo después de completar la búsqueda
    auto end_time6 = std::chrono::steady_clock::now();
    // Calcular la diferencia de tiempo para obtener el tiempo transcurrido en milisegundos
    auto elapsed_time6 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time6 - start_time6).count();
    UserData found_user6;
    if (search_result6.has_value()) {
        found_user6 = search_result6.value();
        std::cout << "Encontrado por Nombre: " << found_user6.userName + " (" << found_user6.userID << ")" << std::endl; // Cambiado a userID
        // Imprimir el tiempo transcurrido
        std::cout << "-Tiempo de busqueda por Nombre: " << elapsed_time6 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por Nombre: " << searchName_v3 << std::endl;
    }

    std::cout << "|---------Tabla Doble---------|" << std::endl;
    auto start_time7 = std::chrono::steady_clock::now();
    auto search_result7 = doubleClosedHashTableId.get(searchID);
    auto end_time7 = std::chrono::steady_clock::now();
    auto elapsed_time7 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time7 - start_time7).count();
    if (search_result7.has_value()) {
        UserData found_user7 = search_result7.value();
        std::cout << "Encontrado por ID: " << found_user7.userName + " (" << found_user7.userID << ")" << std::endl; 
        std::cout << "-Tiempo de busqueda por ID: " << elapsed_time7 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por ID: " << searchID << std::endl;
    }

    // Buscar un nombre de usuario que debería estar presente
    auto start_time8 = std::chrono::steady_clock::now();
    auto search_result8 = quadHashName.get(searchName_v3);
    // Medir el tiempo después de completar la búsqueda
    auto end_time8 = std::chrono::steady_clock::now();
    // Calcular la diferencia de tiempo para obtener el tiempo transcurrido en milisegundos
    auto elapsed_time8 = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time8 - start_time8).count();
    UserData found_user8;
    if (search_result8.has_value()) {
        found_user8 = search_result8.value();
        std::cout << "Encontrado por Nombre: " << found_user8.userName + " (" << found_user8.userID << ")" << std::endl; // Cambiado a userID
        // Imprimir el tiempo transcurrido
        std::cout << "-Tiempo de busqueda por Nombre: " << elapsed_time8 << " nanosegundos" << std::endl;
    } else {
        std::cout << "No encontrado por Nombre: " << searchName_v3 << std::endl;
    }

    
    return 0;
}

