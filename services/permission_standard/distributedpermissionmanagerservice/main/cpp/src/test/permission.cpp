#include <iostream>
#include <atomic>

int main()
{
    std::cout << ">>>>>> test begin" << std::endl;
    std::atomic<bool> isRegistered(false);
    bool init = isRegistered.load();
    isRegistered.compare_exchange_strong(init, true);
    int i = 1 != 0;
    std::cout << ">>>>>>> test end" << std::endl << std::endl;
    return 0;
}
