#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include "communication.h"

#define TRUSTED_COUNT 5

typedef struct {
    int ImageSize;
    std::string ProcessName; 
} ProcessData;

typedef struct {
    ProcessData proc;
    int Count;
} ProcessDataCounted;

class ProcessDatabase
{
private:
    std::unordered_map<size_t, ProcessDataCounted> ProcessHashmap;
public:
    std::string TestString = "test string";
    
    size_t HashProcess(ProcessData &proc) {
        std::string base = ""; 
        base += std::to_string(proc.ImageSize);
        base += proc.ProcessName;
        size_t hash = std::hash<std::string>{}(base);
        return hash;
    }
    
    void InsertProcess(ProcessData proc) {
        size_t hash = HashProcess(proc);
        auto p = ProcessHashmap.find(hash);
        if (p == ProcessHashmap.end()) {
            ProcessHashmap.insert({ hash, {proc, 1} });
        }
        else {
            p->second.Count++;
        }
    }

    void InsertProcessCounted(ProcessDataCounted proc) {
        size_t hash = HashProcess(proc.proc);
        ProcessHashmap.insert({ hash, proc });
    }

    bool IsProcessTrusted(size_t hash) {
        auto p = ProcessHashmap.find(hash);
        if (p == ProcessHashmap.end()) {
            return false;
        }
        else if (p->second.Count < TRUSTED_COUNT) {
            return false;
        }
        else {
            return true;
        }
    }
    
    bool ProcessExists(size_t hash) {
        if (ProcessHashmap.find(hash) == ProcessHashmap.end()) {
            return false;
        }
        return true;
    }

    void AddProcBufferToDatabase(PVOID Buffer, ULONG ReturnLength)
    {
        SIZE_T i = 0;
        while (i < ReturnLength) {
            SIZE_T h_image_size = ((PPROCESS_HEADER)((char*)Buffer + i))->image_size;
            SIZE_T h_string_length = ((PPROCESS_HEADER)((char*)Buffer + i))->string_length;

            i += sizeof(PROCESS_HEADER);
            
            PWCH string_buffer = ((PWCH)((char*)Buffer + i));
            std::string proc_name;
            proc_name.resize(h_string_length);
            sprintf_s(&proc_name[0], h_string_length, "%S", string_buffer);
            
            i += h_string_length * sizeof(WCHAR);

            ProcessData p = {h_image_size, proc_name};
            InsertProcess(p);
        }
    }
    
    void PrintProcessData(size_t hash) {
        ProcessData *proc = &ProcessHashmap.at(hash).proc;
        printf("HASH: %20zu SIZE: %5d NAME: %s\n", hash, proc->ImageSize, proc->ProcessName.c_str());
    }
    
    void PrintTable() {
        printf("TABLE SIZE: %zu\n", ProcessHashmap.size());
        for (auto& i: ProcessHashmap)
        {
            printf("HASH: %20zu SIZE: %5d NAME: %s\n", (size_t)i.first, i.second.proc.ImageSize, i.second.proc.ProcessName.c_str());
        }
    }

    SIZE_T GetTableSize() {
        return ProcessHashmap.size();
    }

    void DeleteProcess(size_t hash) {
        ProcessHashmap.erase(hash);
    }
    
    void SaveDatabaseToFile() {
        std::ofstream savefile("database_save.bin", std::ios::binary);

        int item_count = ProcessHashmap.size();
        savefile.write((char*)&item_count, sizeof(item_count));

        for (auto& i : ProcessHashmap) {
            savefile.write((char*)&i.second.proc.ImageSize, sizeof(i.second.proc.ImageSize));

            size_t size = i.second.proc.ProcessName.size();
            savefile.write((char*)&size, sizeof(size));
            savefile.write(&i.second.proc.ProcessName[0], size);

            savefile.write((char*)&i.second.Count, sizeof(i.second.Count));
        }
    }
	void LoadDatabaseFromFile() {
        std::ifstream savefile("database_save.bin", std::ios::binary);
        if (savefile.fail()) {
            std::cout << "Save file database_save.bin does not exist or failed" << std::endl;
            return;
        }
        int item_count;
        savefile.read((char*)&item_count, sizeof(item_count));  // get item count

        for (int i = 0; i < item_count; i++) {
            int x;
            int c;
            savefile.read((char*)&x, sizeof(x));                // get ImageSize
            
            std::string str;
            size_t size; 
            savefile.read((char*)&size, sizeof(size));                //get ProcessName
            str.resize(size);
            savefile.read(&str[0], size);

            savefile.read((char*)&c, sizeof(c));

            ProcessDataCounted p = {{x, str}, c};               // get process count
            InsertProcessCounted(p);
        }
    }
};

void test_database()
{
    ProcessDatabase *database = new ProcessDatabase;
    ProcessData p1 = {50001, "MyProcess"};
    size_t h1 = database->HashProcess(p1);
    std::cout << "50001 MyProcess " << std::endl;
    std::cout << "Process to hash: " << h1<< std::endl;
    
    std::cout << "Inserting new process to table: " << std::endl;
    database->InsertProcess(p1);
    
    std::cout << "Checking if real process is known in table: " << database->ProcessExists(h1) << std::endl;
    std::cout << "Checking if fake process is known in table: " << database->ProcessExists(3154543) << std::endl;
    
    
    std::cout << "Print process data: " << std::endl;
    database->PrintProcessData(h1);
    
    std::cout << "Print table: " << std::endl;
    database->PrintTable();
    
    std::cout << std::endl<< std::endl<< std::endl << "adding more processes" << std::endl; 
    ProcessData p2 = {800, "CoolWindowsProcess"};
    database->InsertProcess(p2);
    ProcessData p3 = {801, "FunnyProcess"};
    database->InsertProcess(p3);
    ProcessData p4 = {802, "MoreProcess"};
    database->InsertProcess(p4);
    ProcessData p5 = {803, "ComputerVirus"};
    database->InsertProcess(p5);
    ProcessData p6 = {804, "Calculator.dll"};
    database->InsertProcess(p6);
    database->PrintTable();
    
    std::cout << "Erasing..." << std::endl;
    database->DeleteProcess(database->HashProcess(p3));
    database->PrintTable();

    std::cout << "Saving and loading table..." << std::endl;
    database->SaveDatabaseToFile();
    database->LoadDatabaseFromFile();
    database->PrintTable();
}

