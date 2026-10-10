#include <iostream>
#include <vector>
#include <memory>

// Класс DataBlock с доработками для Сценария 2:
// - Публичный деструктор (нужен для std::vector)
// - Удаленное копирование и перемещающий конструктор (для эффективности и безопасности в векторе)
class DataBlock {
private:
    int* data; // Имитация "виртуальной страницы"
    int id;

public:
    DataBlock(int block_id) : id(block_id) {
        data = new int[1024]; 
        std::cout << "[+] Конструктор: выделена виртуальная страница для блока #" << id << std::endl;
    }

    // Деструктор должен быть публичным, чтобы std::vector мог удалять объекты
    ~DataBlock() {
        delete[] data;
        std::cout << "[-] Деструктор: ОС освободила страницу у блока #" << id << std::endl;
    }

    // Запрещаем копирование (оно может привести к двойному удалению data)
    DataBlock(const DataBlock&) = delete;
    DataBlock& operator=(const DataBlock&) = delete;

    // Разрешаем перемещение (нужно для эффективной работы std::vector при реаллокации)
    DataBlock(DataBlock&& other) noexcept : data(other.data), id(other.id) {
        other.data = nullptr;
        other.id = 0;
    }
    DataBlock& operator=(DataBlock&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            id = other.id;
            other.data = nullptr;
            other.id = 0;
        }
        return *this;
    }
};

// Функция для изоляции Сценария 1
void run_scenario_1() {
    std::cout << "===========================================" << std::endl;
    std::cout << "СЦЕНАРИЙ 1: Утечка памяти (new[] + delete)" << std::endl;
    std::cout << "===========================================" << std::endl;

    // Выделяем массив из 3 объектов в куче
    DataBlock* blocks = new DataBlock[3] {DataBlock(1), DataBlock(2), DataBlock(3)};

    std::cout << "\n--- Имитируем ошибку: применяем скалярный delete к массиву ---" << std::endl;
    
    // ОШИБКА: к массиву применен скалярный delete
    // 1. Вызовется деструктор ТОЛЬКО блока #1.
    // 2. Произойдет падение программы (Segmentation Fault) при попытке освободить память.
    delete blocks; 

    // Этот код никогда не выполнится из-за падения выше
    std::cout << "Эта строка не будет достигнута." << std::endl;
}

// Функция для изоляции Сценария 2
void run_scenario_2() {
    std::cout << "\n\n===========================================" << std::endl;
    std::cout << "СЦЕНАРИЙ 2: Исправление (std::shared_ptr + std::vector)" << std::endl;
    std::cout << "===========================================" << std::endl;

    // Создаем shared_ptr, владеющий вектором
    std::shared_ptr<std::vector<DataBlock>> pointer1 = std::make_shared<std::vector<DataBlock>>();
    
    // Эмулируем выделение 3 блоков
    pointer1->emplace_back(1);
    pointer1->emplace_back(2);
    pointer1->emplace_back(3);

    std::cout << "\n[LOG] Счетчик ссылок после создания pointer1: " << pointer1.use_count() << std::endl;

    // Копируем указатель: теперь владельцев двое, счетчик ссылок = 2
    std::shared_ptr<std::vector<DataBlock>> pointer2 = pointer1;
    std::cout << "[LOG] Счетчик ссылок после копирования в pointer2: " << pointer1.use_count() << std::endl;

    // Сбрасываем первый указатель. Память НЕ освобождается, так как pointer2 жив.
    std::cout << "\n[ACTION] Вызываем pointer1.reset()" << std::endl;
    pointer1.reset(); 
    std::cout << "[LOG] Счетчик ссылок у pointer2 после сброса pointer1: " << pointer2.use_count() << std::endl;

    // Проверяем, что данные все еще доступны через второй указатель
    std::cout << "[LOG] Доступ к данным через pointer2: вектор содержит " << pointer2->size() << " блоков." << std::endl;

    // Сбрасываем последний указатель. Счетчик = 0. 
    // Только сейчас вызываются деструкторы DataBlock #3, #2, #1.
    std::cout << "\n[ACTION] Вызываем pointer2.reset()" << std::endl;
    pointer2.reset();
}

int main() {
    // Запускаем первый сценарий. 
    // Программа упадет здесь. Если закомментировать run_scenario_1(), 
    // программа выполнится до конца и покажет второй сценарий.
    run_scenario_1(); 

    // Запускаем второй сценарий (безопасный)
    run_scenario_2();

    std::cout << "\n\n--- Программа успешно завершила выполнение безопасного сценария ---" << std::endl;
    return 0;
}