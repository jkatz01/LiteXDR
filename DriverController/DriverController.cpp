#include <iostream>
#include "KernelInterface.h"
#include "ProcessDatabase.h"

int main()
{
    KernelInterface Driver = KernelInterface("\\\\.\\xdr");

    ULONG number = Driver.GetExampleNumber();

    std::cout << "Number: " << number << std::endl;

    Driver.GetProcBuffer();

    std::cout << "ReturnLength = " << Driver.ReturnLength << std::endl;

    Driver.PrintProcBufferKernel();

    ProcessDatabase* database = new ProcessDatabase;

    std::cout << "Printing hashmap contents..." << std::endl;
    database->AddProcBufferToDatabase(Driver.Buffer, Driver.ReturnLength);
    database->PrintTable();

}
