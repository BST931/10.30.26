#include <iostream>
#include <vector>
#include <memory>

class DataBlock {
private:
    int data[1024]; 
    int id;

public:
    DataBlock() : id(0) {
        std::cout << "[+] Конструктор: Выделена виртуальная страница для Блока #" << id << "\n";
    }

    DataBlock(int blockId) : id(blockId) {
        std::cout << "[+] Конструктор: Выделена виртуальная страница для Блока #" << id << "\n";
    }

    ~DataBlock() {
        std::cout << "[-] Деструктор: ОС освободила страницу Блока #" << id << "\n";
    }

    void setId(int blockId) {
        id = blockId;
    }
};

void runScenario1() {
    std::cout << "\n=== ЗАПУСК СЦЕНАРИЯ 1 (Утечка памяти) ===\n";

    
    DataBlock* array = new DataBlock;
    
    for (int i = 0; i < 3; ++i) {
        array[i].setId(i + 1);
    }

    std::cout << "...Имитируем ошибку: удаляем массив через одиночный деструктор...\n";
    
    array[0].~DataBlock(); 

    operator delete[](array); 

    std::cout << "=== КОНЕЦ СЦЕНАРИЯ 1 ===\n";
}

void runScenario2() {
    std::cout << "\n=== ЗАПУСК СЦЕНАРИЯ 2 (Умные указатели) ===\n";

    auto pointer1 = std::make_shared<std::vector<DataBlock>>();
    
    pointer1->push_back(DataBlock(1));
    pointer1->push_back(DataBlock(2));
    pointer1->push_back(DataBlock(3));

    std::cout << "\n[Умный указатель] Создаем pointer2 и копируем в него pointer1...\n";
    std::shared_ptr<std::vector<DataBlock>> pointer2 = pointer1;
    std::cout << "Количество владельцев ресурса: " << pointer1.use_count() << "\n\n";

    std::cout << "[Умный указатель] Сбрасываем первый указатель (pointer1.reset())...\n";
    pointer1.reset();
    std::cout << "Количество владельцев ресурса теперь: " << pointer2.use_count() << "\n";
    std::cout << "(Страницы все еще в памяти, так как pointer2 удерживает их)\n\n";

    std::cout << "[Умный указатель] Выходим из функции runScenario2 (разрушение pointer2)...\n";
}

int main() {
    runScenario1();

    runScenario2();

    std::cout << "\n=== ПРОГРАММА ЗАВЕРШЕНА ===\n";
    return 0;
}

