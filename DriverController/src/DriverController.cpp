#include <iostream>
#include "communication.h"
#include "KernelInterface.h"
#include "ProcessDatabase.h"

int main()
{
    ProcessDatabase* database = new ProcessDatabase;
    while (TRUE) {
        database->HashesToServer_Test();
        std::cout << "--------------------" << std::endl;
        Sleep(5000);
    }
    /*
    KernelInterface Driver = KernelInterface("\\\\.\\xdr");
    ProcessDatabase* database = new ProcessDatabase;

    database->LoadDatabaseFromFile();

    while (TRUE) {
        Driver.GetProcBuffer();

        if (Driver.ReturnLength > 0) {
            std::cout << "Read buffer with size: " << Driver.ReturnLength << std::endl;

            database->AddProcBufferToDatabase(Driver.Buffer, Driver.ReturnLength);

            std::cout << "Table size: " << database->GetTableSize() << std::endl;

            database->SaveDatabaseToFile();
        }
        else {
            std::cout << "." << std::endl;
        }

        Driver.DeleteBufferContents();

        Sleep(3000);
    }

    database->SaveDatabaseToFile();
    */

}
