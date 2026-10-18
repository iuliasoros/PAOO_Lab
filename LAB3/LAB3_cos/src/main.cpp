#include <tema/demo.hpp>
#include <cstring>

int main(int argc, char* argv[])
{
    // ./tema3_cos crash  -> arata ce se intampla fara copy constructor
    if (argc > 1 && std::strcmp(argv[1], "crash") == 0)
    {
        tema::demoFaraCopyConstructor();
        return 0;
    }

    tema::demoIncapsulare();
    tema::demoListaInitializare();
    tema::demoDestructor();
    tema::demoCopyConstructor();
    tema::demoMoveConstructor();
    tema::raportHeap();

    return 0;
}
