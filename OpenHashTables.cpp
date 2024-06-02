#include <cstdio>
#include <fstream>
#include <optional>
#include <sstream>
#include <vector>
#include <chrono> // Para medir tiempo de busqueda 
#include <thread> // Agregar esta línea

#include "clases/HashTableOpen.h"
#include "clases/UserData.h"

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
        UserData userData{university, userID, userName, numTweets, friendsCount, followersCount, createdAt};

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

int main() {
    // Crear una tabla hash abierta con tamaño inicial 4096
    OpenHashTableByID hashTableID(4096);
    OpenHashTableByName hashTableName(4096);

    // Cargamos el CSV 
    std::string filecsv = "universities_followers.csv";
    auto load_result = loadCSV(filecsv); //  auto : Permite que el compilador deduzca automáticamente el tipo de una variable en función del valor que se le asigna. 
    if (!load_result.has_value()) return 1;  // has_value : devuelve true si el std::optional contiene un valor y false si está vacío.
    std::vector<UserData> data_vector = *load_result.value();

    for (const UserData user : data_vector) {
        hashTableID.put(user.userID, user);
        hashTableName.put(user.userName, user);
    }

    // Buscar un ID de usuario que debería estar presente
    unsigned long long int searchID = 414942137; // Ejemplo de ID que debería estar en el CSV
    // Medir el tiempo antes de realizar la búsqueda
    auto start_time = std::chrono::steady_clock::now();
    auto search_result = hashTableID.get(searchID);
    // Agregar una pequeña pausa antes de finalizar la medicion del tiempo para simular una búsqueda más prolongada para asegurarnos de que el tiempo medido sea significativo.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // Medir el tiempo después de completar la búsqueda
    auto end_time = std::chrono::steady_clock::now();
    // Calcular la diferencia de tiempo para obtener el tiempo transcurrido en milisegundos
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    if (search_result.has_value()) {
        UserData found_user = search_result.value();
        std::cout << "Encontrado por ID: " << found_user.userName + " (" << found_user.userID << ")" << std::endl; // Cambiado a userName
        // Imprimir el tiempo transcurrido
        std::cout << "Tiempo de busqueda por ID: " << elapsed_time << " milisegundos" << std::endl;
    } else {
        std::cout << "No encontrado por ID: " << searchID << std::endl;
    }

    // Buscar un nombre de usuario que debería estar presente
    std::string searchName = "SantillanaLAB"; // Ejemplo de nombre que debería estar en el CSV
    // Medir el tiempo antes de realizar la búsqueda
    auto start_time2 = std::chrono::steady_clock::now();
    auto search_result2 = hashTableName.get(searchName);
    // Agregar una pequeña pausa antes de finalizar la medicion del tiempo para simular una búsqueda más prolongada para asegurarnos de que el tiempo medido sea significativo.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // Medir el tiempo después de completar la búsqueda
    auto end_time2 = std::chrono::steady_clock::now();
    // Calcular la diferencia de tiempo para obtener el tiempo transcurrido en milisegundos
    auto elapsed_time2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_time2 - start_time).count();
    UserData found_user2;
    if (search_result2.has_value()) {
        found_user2 = search_result2.value();
        std::cout << "Encontrado por Nombre: " << found_user2.userName + " (" << found_user2.userID << ")" << std::endl; // Cambiado a userID
        // Imprimir el tiempo transcurrido
        std::cout << "Tiempo de busqueda por Nombre: " << elapsed_time2 << " milisegundos" << std::endl;
    } else {
        std::cout << "No encontrado por Nombre: " << searchName << std::endl;
    }
    
    return 0;
}
