#include <iostream>
#include "kernelinterface.h"

int main()
{
    KernelInterface Driver = KernelInterface("\\\\.\\xdr");

    ULONG number = Driver.GetExampleNumber();

    std::cout << "Number: " << number << std::endl;

    std::cout << "Hello World!\n";
}
