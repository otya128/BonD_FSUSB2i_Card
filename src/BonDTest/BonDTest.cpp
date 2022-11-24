#include <Windows.h>
#include "../inc/IBonDriver.h"

int main()
{
    auto bond = CreateBonDriver();
    bond->OpenTuner();
    bond->SetChannel(14);
    return 0;
}
