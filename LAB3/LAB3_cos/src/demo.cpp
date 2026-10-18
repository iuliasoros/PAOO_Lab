#include <tema/demo.hpp>
#include <tema/contor_heap.hpp>
#include <tema/cos.hpp>
#include <tema/exemple_gresite.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace tema {

namespace {

// Functii ajutatoare pentru afisare
void titlu(const char* text)
{
    std::cout << "\n=== " << text << " ===\n";
}

void pas(const char* cod)
{
    std::cout << "  >> " << cod << "\n";
}

void explica(const char* text)
{
    std::cout << "  => " << text << "\n";
}

const char* rezultat(bool acceptat)
{
    return acceptat ? "acceptat" : "RESPINS";
}

// Clase ajutatoare pentru lista de initializare
// Cupon este un membru al claselor ComandaAtribuireInCorp si ComandaListaInitializare
class Cupon
{
public:
    // Constructor fara parametri
    Cupon()
        : cod_("(gol)")
    {
        std::cout << "    Cupon()                 -> construit cu valoarea implicita \"" << cod_ << "\"\n";
    }
    // Constructor cu parametru
    explicit Cupon(const std::string& cod)
        : cod_(cod)
    {
        std::cout << "    Cupon(\"" << cod_ << "\") -> construit direct cu valoarea finala\n";
    }
    // Operator de atribuire
    Cupon& operator=(const std::string& cod)
    {
        std::cout << "    Cupon::operator=        -> suprascrie \"" << cod_ << "\" cu \"" << cod << "\"\n";
        cod_ = cod;
        return *this;
    }

private:
    std::string cod_;
};

// Atribuire in corp: cupon_ este construit implicit, apoi suprascris
class ComandaAtribuireInCorp
{
public:
    explicit ComandaAtribuireInCorp(const std::string& cod)
    {                       // aici cupon_ a fost DEJA construit cu Cupon()
        cupon_ = cod;       // a doua operatie: atribuire
    }

private:
    Cupon cupon_;
};

// Lista de initializare: cupon_ este construit direct cu valoarea corecta
class ComandaListaInitializare
{
public:
    explicit ComandaListaInitializare(const std::string& cod)
        : cupon_(cod)       // o singura operatie: construire directa
    {
    }

private:
    Cupon cupon_;
};

// Functie care primeste un Cos prin valoare (se apeleaza copy constructor-ul)
void raportPrinValoare(Cos c)
{
    std::cout << "    (in functie) ";
    c.afiseaza();
}

} // namespace

// Demo pentru incapsulare
void demoIncapsulare()
{
    titlu("2. INCAPSULARE");
    explica("datele (preturi_, numarProduse_) sunt private; se modifica doar prin metode care le valideaza");
    pas("Cos cos(1001, \"Ana Popescu\", 3);   // cos cu loc pentru 3 produse");

    Cos cos(1001, "Ana Popescu", 3);

    pas("cos.adaugaPret(49.99);");
    std::cout << "    " << rezultat(cos.adaugaPret(49.99)) << "\n";

    pas("cos.adaugaPret(-10);");
    std::cout << "    " << rezultat(cos.adaugaPret(-10)) << " - pretul trebuie sa fie pozitiv\n";

    pas("cos.adaugaPret(120.50); cos.adaugaPret(15);");
    std::cout << "    " << rezultat(cos.adaugaPret(120.50)) << ", " << rezultat(cos.adaugaPret(15)) << "\n";

    pas("cos.adaugaPret(7);");
    std::cout << "    " << rezultat(cos.adaugaPret(7))
              << " - cosul e plin; fara verificare s-ar scrie in afara tabloului (buffer overflow)\n";

    pas("cos.modificaPret(5, 10);");
    std::cout << "    " << rezultat(cos.modificaPret(5, 10)) << " - nu exista produsul cu indexul 5\n";

    pas("cos.afiseaza();");
    cos.afiseaza();

    explica("cos.preturi_[7] = 0.01;  NU compileaza: 'preturi_' is a private member of 'tema::Cos'");
    explica("starea obiectului nu poate fi stricata din exterior");
}

// Demo pentru lista de initializare
void demoListaInitializare()
{
    titlu("3. INITIALIZAREA MEMBRILOR IN CONSTRUCTOR");
    explica("un membru este construit INAINTE sa se intre in corpul constructorului");

    pas("ComandaAtribuireInCorp c1(\"REDUCERE10\");    // cupon_ = cod; in corp");
    ComandaAtribuireInCorp c1("REDUCERE10");
    explica("2 operatii: construire implicita + atribuire (munca facuta de doua ori)");

    pas("ComandaListaInitializare c2(\"REDUCERE10\");  // : cupon_(cod)");
    ComandaListaInitializare c2("REDUCERE10");
    explica("1 operatie: membrul este construit direct cu valoarea corecta");

    std::cout << "\n";
    explica("pentru unii membri lista de initializare este OBLIGATORIE:");
    std::cout << "     - membri const (Cos::id_), referinte, membri fara constructor implicit\n";
    std::cout << "     - id_ = id; in corp -> eroare: cannot assign to non-static data member\n";
    std::cout << "       'id_' with const-qualified type 'const int'\n";

    pas("Cos cos(1002, \"Ion Ionescu\", 2);  // id_(id), preturi_(new double[...]) in lista");
    Cos cos(1002, "Ion Ionescu", 2);
    std::cout << "    id = " << cos.id() << " (fixat la creare, nu se mai poate schimba)\n";
}

// Demo pentru destructor
void demoDestructor()
{
    titlu("4. ELIBERAREA HEAP-ULUI IN DESTRUCTOR");

    std::cout << "  a) Cos - ARE destructor cu delete[]\n";
    int inainte = ContorHeap::blocuriActive;
    pas("for (i = 1..3) { Cos c(...); }   // c este distrus la fiecare '}'");
    for (int i = 1; i <= 3; ++i)
    {
        Cos c(2000 + i, "Client " + std::to_string(i), 4);
    }
    std::cout << "    blocuri heap pierdute: " << ContorHeap::blocuriActive - inainte << "\n";

    std::cout << "\n  b) CosFaraDestructor - NU are destructor\n";
    inainte = ContorHeap::blocuriActive;
    pas("for (i = 1..3) { CosFaraDestructor c(...); }");
    for (int i = 1; i <= 3; ++i)
    {
        CosFaraDestructor c("Fantoma " + std::to_string(i), 4);
    }
    std::cout << "    blocuri heap pierdute: " << ContorHeap::blocuriActive - inainte << "\n";
    explica("obiectele au disparut, dar memoria lor a ramas ocupata si nimeni nu mai are adresa ei");
    explica("(memory leak) - intr-un program care ruleaza mult, memoria se consuma treptat");
}

// Demo pentru copy constructor
void demoCopyConstructor()
{
    titlu("5. COPY CONSTRUCTOR (copie in adancime)");

    pas("Cos original(3001, \"Maria Ionescu\", 5); + preturi 80 si 20.5");
    Cos original(3001, "Maria Ionescu", 5);
    original.adaugaPret(80.0);
    original.adaugaPret(20.5);

    pas("Cos copie = original;");
    Cos copie = original;

    pas("copie.modificaPret(0, 10.0);   // modificam DOAR copia");
    copie.modificaPret(0, 10.0);

    std::cout << "    original:"; original.afiseaza();
    std::cout << "    copie:   "; copie.afiseaza();
    explica("adrese diferite -> fiecare obiect are propriul tablou; originalul a ramas cu pretul 80");

    std::cout << "\n";
    explica("copy constructor-ul se apeleaza si implicit, la transmiterea prin valoare:");
    pas("raportPrinValoare(original);");
    raportPrinValoare(original);

    std::cout << "\n";
    explica("cum arata FARA copy constructor propriu: ruleaza  ./build/tema3_cos crash");
}

// Demo pentru move constructor
void demoMoveConstructor()
{
    titlu("6. MOVE CONSTRUCTOR (transfer fara copiere)");

    std::cout << "  a) copiere vs mutare\n";
    pas("Cos sursa(4001, \"Andrei Pop\", 1000); + preturi 10 si 9.5");
    Cos sursa(4001, "Andrei Pop", 1000);
    sursa.adaugaPret(10.0);
    sursa.adaugaPret(9.5);

    int inainte = ContorHeap::blocuriActive;
    pas("Cos copie = sursa;                 // copy");
    Cos copie = sursa;
    std::cout << "    alocari noi: " << ContorHeap::blocuriActive - inainte
              << " (new double[1000] + copierea preturilor)\n";

    inainte = ContorHeap::blocuriActive;
    pas("Cos mutat = std::move(sursa);      // move");
    Cos mutat = std::move(sursa);
    std::cout << "    alocari noi: " << ContorHeap::blocuriActive - inainte << " (doar se preia pointerul)\n";

    std::cout << "    sursa: "; sursa.afiseaza();
    std::cout << "    mutat: "; mutat.afiseaza();
    explica("'mutat' are exact adresa pe care o avea 'sursa'; sursa a ramas goala (preturi_ = nullptr)");

    std::cout << "\n  b) obiecte temporare si std::vector\n";
    explica("un temporar oricum dispare -> e mai ieftin sa ii 'furam' memoria decat sa o copiem");
    std::vector<Cos> comenzi;

    pas("comenzi.push_back(Cos(4002, \"Elena\", 5));");
    comenzi.push_back(Cos(4002, "Elena", 5));

    pas("comenzi.push_back(Cos(4003, \"Mihai\", 5));   // vectorul se mareste");
    comenzi.push_back(Cos(4003, "Mihai", 5));

    pas("comenzi.push_back(Cos(4004, \"Ioana\", 5));   // vectorul se mareste");
    comenzi.push_back(Cos(4004, "Ioana", 5));

    explica("la marire, vectorul MUTA elementele vechi (move ctor e noexcept), nu le copiaza");
    explica("fara move constructor, fiecare pas de mai sus ar fi facut new[] + copiere completa");

    std::cout << "\n  -- sfarsitul functiei: se distrug toate obiectele locale --\n";
}

// Demo extra: exemplu fara copy constructor
void demoFaraCopyConstructor()
{
    titlu("FARA COPY CONSTRUCTOR (copie superficiala)");

    pas("CosFaraCopyCtor original(\"Maria Ionescu\", 5); + preturi 80 si 20.5");
    CosFaraCopyCtor original("Maria Ionescu", 5);
    original.adaugaPret(80.0);
    original.adaugaPret(20.5);

    pas("CosFaraCopyCtor copie = original;   // copy ctor generat de compilator");
    CosFaraCopyCtor copie = original;
    explica("niciun mesaj de constructor si nicio alocare: s-a copiat doar valoarea pointerului");

    pas("copie.modificaPret(0, 10.0);   // modificam DOAR copia");
    copie.modificaPret(0, 10.0);

    original.afiseaza("original: ");
    copie.afiseaza("copie:    ");
    explica("ACEEASI adresa -> modificarea copiei a stricat si originalul (80 a devenit 10)");

    std::cout << "\n";
    explica("la iesirea din functie ambii destructori fac delete[] pe ACEEASI adresa");
    explica("-> double free -> programul este oprit de sistem");
    std::cout << std::endl;
}

void raportHeap()
{
    std::cout << "\n=== FINAL ===\n";
    std::cout << "  blocuri heap neeliberate: " << ContorHeap::blocuriActive
              << " (toate provin de la CosFaraDestructor)\n";
}

} // namespace tema
