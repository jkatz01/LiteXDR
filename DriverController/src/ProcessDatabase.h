#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "communication.h"

#define TRUSTED_COUNT 5

typedef struct {
    size_t ImageSize;
    std::string ProcessName; 
} ProcessData;

typedef struct {
    ProcessData proc;
    size_t Count;
} ProcessDataCounted;

class ProcessDatabase
{
private:
    std::unordered_map<size_t, ProcessDataCounted> LocalProcMap;
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
        auto p = LocalProcMap.find(hash);
        if (p == LocalProcMap.end()) {
            LocalProcMap.insert({ hash, {proc, 1} });
        }
        else {
            p->second.Count++;
        }
    }

    void InsertProcessCounted(ProcessDataCounted proc) {
        size_t hash = HashProcess(proc.proc);
        LocalProcMap.insert({ hash, proc });
    }

    bool IsProcessTrusted(size_t hash) {
        auto p = LocalProcMap.find(hash);
        if (p == LocalProcMap.end()) {
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
        if (LocalProcMap.find(hash) == LocalProcMap.end()) {
            return false;
        }
        return true;
    }

    size_t CountItemsInProcBuffer(PVOID Buffer, ULONG ReturnLength)
    {
        size_t i = 0;
        size_t count = 0;
        while (i < ReturnLength) {
            size_t h_image_size = ((PPROCESS_HEADER)((char*)Buffer + i))->image_size;
            size_t h_string_length = ((PPROCESS_HEADER)((char*)Buffer + i))->string_length;
            i += sizeof(PROCESS_HEADER);
            i += h_string_length * sizeof(WCHAR);
            count++;
        }
        return count;
    }

    void AddProcBufferToDatabase(PVOID Buffer, ULONG ReturnLength)
    {
        size_t i = 0;
        while (i < ReturnLength) {
            size_t h_image_size = ((PPROCESS_HEADER)((char*)Buffer + i))->image_size;
            size_t h_string_length = ((PPROCESS_HEADER)((char*)Buffer + i))->string_length;

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

    void HashProcBuffer(PVOID Buffer, ULONG ReturnLength, std::vector<size_t> *hashes)
    {
        size_t i = 0;
        while (i < ReturnLength) {
            size_t h_image_size = ((PPROCESS_HEADER)((char*)Buffer + i))->image_size;
            size_t h_string_length = ((PPROCESS_HEADER)((char*)Buffer + i))->string_length;

            i += sizeof(PROCESS_HEADER);

            PWCH string_buffer = ((PWCH)((char*)Buffer + i));
            std::string proc_name;
            proc_name.resize(h_string_length);
            sprintf_s(&proc_name[0], h_string_length, "%S", string_buffer);

            i += h_string_length * sizeof(WCHAR);

            ProcessData p = { h_image_size, proc_name };
            hashes->push_back(HashProcess(p));
        }
    }

    void SendHashesToServer(PVOID Buffer, ULONG ReturnLength) {
        std::vector<size_t> hashes;
        HashProcBuffer(Buffer, ReturnLength, &hashes);

        std::stringstream hashes_str(std::ios::binary);
        for (size_t& h : hashes) {
            hashes_str << h;
        }

        // send request
        char url[22] = "http://127.0.0.1:8080";
        try {
            curlpp::Easy request;

            request.setOpt(new curlpp::options::Url(url));
            request.setOpt(new curlpp::options::Verbose(true));

            std::list<std::string> header;
            header.push_back("Content-Type: hash-send");

            request.setOpt(new curlpp::options::HttpHeader(header));

            // Send stringstream created from the vector<size_t> of hashes
            // When reading in server, needs to load the stringstream into size_t
            request.setOpt(new curlpp::options::PostFields(hashes_str.str()));
            request.setOpt(new curlpp::options::PostFieldSize((long)hashes_str.str().size()));

            request.perform();
        }
        catch (curlpp::LogicError& e) {
            std::cout << e.what() << std::endl;
        }
        catch (curlpp::RuntimeError& e) {
            std::cout << e.what() << std::endl;
        }

        // vector should free itself
    }
    
    void PrintProcessData(size_t hash) {
        ProcessData *proc = &LocalProcMap.at(hash).proc;
        printf("HASH: %20zu SIZE: %5zu NAME: %s\n", hash, proc->ImageSize, proc->ProcessName.c_str());
    }
    
    void PrintTable() {
        printf("TABLE SIZE: %zu\n", LocalProcMap.size());
        for (auto& i: LocalProcMap)
        {
            printf("HASH: %20zu SIZE: %5zu NAME: %s\n", (size_t)i.first, i.second.proc.ImageSize, i.second.proc.ProcessName.c_str());
        }
    }

    size_t GetTableSize() {
        return LocalProcMap.size();
    }

    void DeleteProcess(size_t hash) {
        LocalProcMap.erase(hash);
    }
    
    void SaveDatabaseToFile() {
        std::ofstream savefile("database_save.bin", std::ios::binary);

        size_t item_count = LocalProcMap.size();
        savefile.write((char*)&item_count, sizeof(item_count));

        for (auto& i : LocalProcMap) {
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
        size_t item_count;
        savefile.read((char*)&item_count, sizeof(item_count));  // get item count

        for (int i = 0; i < item_count; i++) {
            size_t x;
            size_t c;
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

