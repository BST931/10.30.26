#include <iostream>
#include <vector>
#include <memory>
#include <string>

class DataBlock {
private:
  
    int data[1024];
    int id;

public:
    explicit DataBlock(int blockId) : id(blockId) {
        
        for (int i = 0; i < 1024; ++i) data[i] = i;
        std::cout << "[+] Конструктор: Выделена виртуальная страница для Блока #"
                  << id << std::endl;
    }

    ~DataBlock() {
        std::cout << "[-] Деструктор: ОС освободила страницу для Блока #"
                  << id << std::endl;
    }

    int getId() const { return id; }
};

void scenario1_leak() {
    std::cout << "\n========== Сценарий 1: Утечка страниц ==========\n";

    DataBlock* blocks = new DataBlock[3]{ DataBlock(100), DataBlock(101), DataBlock(102) };

    delete blocks;

    std::cout << "[!] Программа продолжает работу, но 2 страницы "
                 "(и часть первой) утекли безвозвратно.\n";
    std::cout << "    Деструкторы для блоков #101 и #102 не были вызваны!\n";
}


void scenario2_smart_pointers() {
    std::cout << "\n========== Сценарий 2: Умные указатели ==========\n";


    auto ptr1 = std::make_shared<std::vector<DataBlock>>();
    ptr1->emplace_back(200);
    ptr1->emplace_back(201);
    ptr1->emplace_back(202);

    std::cout << "    use_count ptr1 = " << ptr1.use_count() << std::endl;

    
    std::shared_ptr<std::vector<DataBlock>> ptr2 = ptr1;
    std::cout << "    После копирования в ptr2: use_count = "
              << ptr1.use_count() << std::endl;

   
    ptr1.reset();
    std::cout << "    После ptr1.reset(): ptr1 = "
              << (ptr1 ? "не пуст" : "пуст")
              << ", use_count ptr2 = " << ptr2.use_count() << std::endl;
    std::cout << "    >>> Страницы ещё живы, потому что ptr2 владеет ими <<<\n";

    std::cout << "    Сбрасываем ptr2...\n";
    ptr2.reset();

    std::cout << "    >>> Все страницы освобождены ОС <<<\n";
}

int main() {
    scenario1_leak();
    scenario2_smart_pointers();
    return 0;
}