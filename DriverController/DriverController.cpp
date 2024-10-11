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
        std::cout << "Read buffer with size: " << Driver.ReturnLength << std::endl;

        database->AddProcBufferToDatabase(Driver.Buffer, Driver.ReturnLength);
        std::cout << "Table size: " << database->GetTableSize() << std::endl;

        Driver.DeleteBufferContents();
        Sleep(3000);
    }

    database->SaveDatabaseToFile();

    /*std::cout << "Printing hashmap contents..." << std::endl;
    database->AddProcBufferToDatabase(Driver.Buffer, Driver.ReturnLength);
    database->PrintTable();*/



    // On enter:
    //  Load table
    // Main loop:
    //  Add current buffer to table
    //  Wait 5 seconds
    // On exit:
    //  Save table

}
