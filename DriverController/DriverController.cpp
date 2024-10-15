#include <iostream>
#include "communication.h"
#include "KernelInterface.h"
#include "ProcessDatabase.h"

int main()
{
    KernelInterface Driver = KernelInterface("\\\\.\\xdr");
    ProcessDatabase* database = new ProcessDatabase;

    database->LoadDatabaseFromFile();

    while (TRUE) {
        Driver.GetProcBuffer();

        if (Driver.ReturnLength > 0) {
            std::cout << "Read buffer with size: " << Driver.ReturnLength << std::endl;
            database->AddProcBufferToDatabase(Driver.Buffer, Driver.ReturnLength);
            std::cout << "Table size: " << database->GetTableSize() << std::endl;
        }
        else {
            std::cout << "." << std::endl;
        }

        Driver.DeleteBufferContents();
        Sleep(3000);
    }

    database->SaveDatabaseToFile();
}
