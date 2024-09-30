#include <iostream>
#include <string>
#include <unordered_map>

typedef struct {
    int ImageSize;
    std::string ProcessName; 
} ProcessData;

class ProcessDatabase
{
private:
    std::unordered_map<size_t, ProcessData> ProcessHashmap;
public:
    std::string TestString = "test string";
    
    size_t HashProcess(ProcessData &proc) {
        std::string base = ""; 
        base += std::to_string(proc.ImageSize);
        base += proc.ProcessName;
        size_t hash = std::hash<std::string>{}(base);
        return hash;
    }
    
    void InsertNewProcess(size_t hash, ProcessData proc) {
        ProcessHashmap.insert({hash, proc});
    }
    
    bool IsKnownProcess(size_t hash) {
        try { ProcessHashmap.at(hash); } 
        catch(const std::out_of_range &e) {
            return false;
        }
        return true;
    }
    
    void PrintProcessData(size_t hash) {
        ProcessData *proc = &ProcessHashmap.at(hash);
        printf("HASH: %20zu SIZE: %5d NAME: %s\n", hash, proc->ImageSize, proc->ProcessName.c_str());
    }
    
    void PrintTable() {
        for (auto& i: ProcessHashmap)
        {
            printf("HASH: %20zu SIZE: %5d NAME: %s\n", (size_t)i.first, i.second.ImageSize, i.second.ProcessName.c_str());
        }
    }
    void DeleteProcess(size_t hash) {
        ProcessHashmap.erase(hash);
    }
    
	//TODO: 
    void SaveDatabaseToFile();
	void LoadDatabaseFromFile();
};

void test_database()
{
    ProcessDatabase *database = new ProcessDatabase;
    ProcessData p1 = {50001, "MyProcess"};
    size_t h1 = database->HashProcess(p1);
    std::cout << "50001 MyProcess " << std::endl;
    std::cout << "Process to hash: " << h1<< std::endl;
    
    std::cout << "Inserting new process to table: " << std::endl;
    database->InsertNewProcess(h1, p1);
    
    std::cout << "Checking if real process is known in table: " << database->IsKnownProcess(h1) << std::endl;
    std::cout << "Checking if fake process is known in table: " << database->IsKnownProcess(3154543) << std::endl;
    
    
    std::cout << "Print process data: " << std::endl;
    database->PrintProcessData(h1);
    
    std::cout << "Print table: " << std::endl;
    database->PrintTable();
    
    std::cout << std::endl<< std::endl<< std::endl << "adding more processes" << std::endl; 
    ProcessData p2 = {800, "CoolWindowsProcess"};
    database->InsertNewProcess(database->HashProcess(p2), p2);
    ProcessData p3 = {801, "FunnyProcess"};
    database->InsertNewProcess(database->HashProcess(p3), p3);
    ProcessData p4 = {802, "MoreProcess"};
    database->InsertNewProcess(database->HashProcess(p4), p4);
    ProcessData p5 = {803, "ComputerVirus"};
    database->InsertNewProcess(database->HashProcess(p5), p5);
    ProcessData p6 = {804, "Calculator.dll"};
    database->InsertNewProcess(database->HashProcess(p6), p6);
    database->PrintTable();
    
    std::cout << "Erasing..." << std::endl;
    database->DeleteProcess(database->HashProcess(p3));
    database->PrintTable();

}

