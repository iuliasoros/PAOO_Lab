# Tema 3 – Coș de cumpărături (PAOO)

Aplicație C++ (proof of life, temă individuală) care ilustrează:

| # | Cerință | Unde în cod | Punctaj |
|---|---------|-------------|---------|
| 1 | Build cu CMake (structura din laborator) | `CMakeLists.txt`, `inc/`, `src/` | 3p |
| 2 | Încapsulare | `Cos` – membri `private`, metode care validează | 1p |
| 3 | Inițializarea membrilor din constructor | lista de inițializare din `Cos`, `demoListaInitializare()` | 1p |
| 4 | Eliberarea heap-ului în destructor | `Cos::~Cos()` vs `CosFaraDestructor` | 1p |
| 5 | Copy constructor | `Cos(const Cos&)` vs `CosFaraCopyCtor` | 2p |
| 6 | Move constructor | `Cos(Cos&&) noexcept` | 2p |

Fiecare constructor și destructor afișează un mesaj și adresa memoriei de pe heap, ca să se vadă clar ce se alocă, ce se copiază și ce se eliberează.

---

## Structura proiectului

```
LAB3/
├── CMakeLists.txt              # proiectul: biblioteca statica + executabilul
├── inc/tema/                   # headere (incluse cu prefix: #include <tema/cos.hpp>)
│   ├── cos.hpp                 # clasa CORECTA
│   ├── exemple_gresite.hpp     # clase GRESITE intentionat (contra-exemple)
│   ├── contor_heap.hpp         # contor de blocuri new[] neeliberate
│   └── demo.hpp                # cate o functie demonstrativa pe cerinta
├── src/
│   ├── cos.cpp
│   ├── exemple_gresite.cpp
│   ├── contor_heap.cpp
│   ├── demo.cpp
│   └── main.cpp
└── build/                      # generat de CMake (nu se preda)
```

## Compilare și rulare

```bash
cd LAB3
mkdir build && cd build
cmake ..
make
./tema3_cos          # toate demonstratiile corecte
./tema3_cos crash    # ce se intampla FARA copy constructor (programul se opreste)
```

---

## Pas 1 – CMake (3p)

1. **`cmake_minimum_required` + `project`**. Versiunea 3.10 în loc de 3.0, pentru că CMake 3.29 dă avertisment pentru versiuni < 3.5, iar CMake 4.x dă eroare.
2. **Standardul C++14**: `CMAKE_CXX_STANDARD 14`, `..._REQUIRED ON`, `..._EXTENSIONS OFF`. Move constructor-ul există doar de la C++11.
3. **Separare `inc/` și `src/`**. Headerele stau în `inc/tema/` și se includ cu prefix: `#include <tema/cos.hpp>`.
4. **Bibliotecă statică**: `add_library(tema_lib STATIC ${lib_files})` conține toate clasele.
5. **`target_include_directories(tema_lib PUBLIC $<BUILD_INTERFACE:.../inc> $<INSTALL_INTERFACE:inc>)`**. `PUBLIC` înseamnă că folderul `inc/` e folosit atât la compilarea bibliotecii, cât și de oricine se leagă de ea. Din această cauză `main.cpp` nu are nevoie de `#include "../inc/..."`.
6. **Executabil**: `add_executable(tema3_cos src/main.cpp)` + `target_link_libraries(tema3_cos PRIVATE tema_lib)`.
7. **Build out-of-source**: totul se generează în `build/`, iar sursele rămân curate.

## Pas 2 – Încapsulare (1p)

Datele `Cos`-ului (`preturi_`, `numarProduse_`, `capacitate_`) sunt `private`. Se pot modifica numai prin `adaugaPret()` și `modificaPret()`, care verifică:
- prețul să fie pozitiv (și cel mult 100000);
- să mai fie loc în tablou, altfel s-ar scrie în afara memoriei alocate (buffer overflow);
- indexul prețului modificat să existe.

```
>> cos.adaugaPret(-10);   ->  RESPINS - pretul trebuie sa fie pozitiv
>> cos.adaugaPret(7);     ->  RESPINS - cosul e plin
```

Accesul direct nu compilează: `cos.preturi_[7] = 0.01;` → `error: 'preturi_' is a private member of 'tema::Cos'`.

**De ce e nevoie:** dacă `preturi_` ar fi public, oricine ar putea scrie `preturi_[100] = 5` și ar corupe memoria. Încapsularea garantează că obiectul e mereu într-o stare validă.

## Pas 3 – Inițializarea membrilor în constructor (1p)

```cpp
Cos::Cos(int id, const std::string& client, std::size_t capacitate)
    : id_(id)                        // lista de initializare
    , client_(client)
    , capacitate_(capacitate)
    , numarProduse_(0)
    , preturi_(new double[capacitate])
{ ... }
```

**De ce e nevoie:** membrii unui obiect sunt construiți **înainte** de a se intra în corpul `{ }` al constructorului.

- **Uneori lista e obligatorie.** `id_` e `const int`, deci nu i se poate atribui o valoare în corp:
  `error: cannot assign to non-static data member 'id_' with const-qualified type 'const int'`.
  Același lucru e valabil pentru referințe și pentru membri care nu au constructor implicit.
- **Altfel se face muncă dublă.** Demonstrația folosește clasa `Cupon`, care afișează fiecare operație:
  ```
  ComandaAtribuireInCorp c1("REDUCERE10");   ComandaListaInitializare c2("REDUCERE10");
    Cupon()   implicit                         Cupon("REDUCERE10")  direct
    Cupon::operator=  suprascrie
  ```
- **Ordinea contează.** Membrii se inițializează în ordinea în care sunt **declarați** în clasă, nu în ordinea din listă. De aceea `preturi_` (care folosește `capacitate_`) e declarat după `capacitate_`.

## Pas 4 – Eliberarea heap-ului în destructor (1p)

```cpp
Cos::~Cos()
{
    if (preturi_ != nullptr) { delete[] preturi_; ... }
}
```

**De ce e nevoie:** destructorul generat automat distruge doar **pointerul** (8 octeți), nu și memoria spre care arată. Comparația din program:

```
a) Cos (CU destructor):              blocuri heap pierdute: 0
b) CosFaraDestructor (FARA):         blocuri heap pierdute: 3
```

Se poate confirma și cu `valgrind --leak-check=full ./build/tema3_cos` pe Linux (sau `leaks --atExit -- ./build/tema3_cos` pe macOS): 3 leaks, exact cele 3 obiecte `CosFaraDestructor`.

## Pas 5 – Copy constructor (2p)

```cpp
Cos::Cos(const Cos& other)
    : ..., preturi_(new double[other.capacitate_])               // memorie NOUA
{
    std::copy(other.preturi_, other.preturi_ + other.numarProduse_, preturi_);   // copiem CONTINUTUL
}
```

**De ce e nevoie:** copy constructor-ul generat de compilator copiază membru cu membru. Pentru `double* preturi_` se copiază doar **adresa** (shallow copy). Rulează `./tema3_cos crash` ca să vezi efectul:

```
>> CosFaraCopyCtor copie = original;
>> copie.modificaPret(0, 10.0);           // modificam DOAR copia
   original: preturi: [10.00 20.50] @0x...42c0      <- s-a stricat si originalul!
   copie:    preturi: [10.00 20.50] @0x...42c0      <- ACEEASI adresa
   [destructor] delete[] @0x...42c0
   [destructor] delete[] @0x...42c0                 <- double free -> abort (exit code 134)
```

Cu copy constructor-ul propriu (`./tema3_cos`), cele două obiecte au adrese diferite și sunt independente:

```
original: preturi: [80.00 20.50] @0x...5370
copie:    preturi: [10.00 20.50] @0x...53a0
```

Demonstrația arată și că copy constructor-ul se apelează **implicit** când un obiect e transmis prin valoare (`raportPrinValoare(original)`).

## Pas 6 – Move constructor (2p)

```cpp
Cos::Cos(Cos&& other) noexcept
    : ..., preturi_(other.preturi_)       // preluam pointerul - fara new[], fara copiere
{
    other.preturi_ = nullptr;             // sursa nu mai detine memoria
}
```

**De ce e nevoie:** când sursa este un obiect temporar sau unul marcat cu `std::move`, e inutil să alocăm memorie nouă și să copiem tot. Putem pur și simplu „fura” tabloul sursei.

```
Cos copie = sursa;              -> alocari noi: 1  (new double[1000] + copiere)
Cos mutat = std::move(sursa);   -> alocari noi: 0  (doar se preia pointerul)
   sursa: preturi: [] @0        <- sursa a ramas goala
   mutat: preturi: [10.00 9.50] @<aceeasi adresa pe care o avea sursa>
```

În `std::vector`:
- la `push_back(Cos(...))`, obiectul temporar e **mutat** în vector, nu copiat;
- când vectorul se mărește, elementele vechi sunt **mutate** în noua zonă de memorie.

Detalii importante:
- **`other.preturi_ = nullptr`**: fără această linie, destructorul sursei ar elibera tabloul preluat, iar destinația ar rămâne cu un pointer invalid (și ar urma un double free).
- **`noexcept`**: `std::vector` folosește move constructor-ul la redimensionare doar dacă e marcat `noexcept`. Altfel, din motive de siguranță la excepții, copiază.
- Dacă o clasă are copy constructor sau destructor declarat de noi, compilatorul **nu** mai generează automat un move constructor. Atunci `std::move` ar apela tot copy constructor-ul, adică o copie completă.

---

## Întrebări posibile la prezentare

- **Regula celor 3/5.** Dacă o clasă are nevoie de destructor, are nevoie și de copy constructor și de `operator=`. De la C++11 se adaugă move constructor și move assignment. Atribuirile nu erau cerute în temă, așa că le-am **interzis explicit** (`= delete`). Altfel compilatorul ar fi generat un `operator=` care copiază doar pointerul. În plus, `id_` e `const`, deci atribuirea nu are sens.
- **De ce `if (preturi_ != nullptr)` în destructor?** `delete[] nullptr` e sigur, dar verificarea separă cazul „obiect mutat”: nu mai are ce elibera și nu trebuie scăzut din contor.
- **De ce `Cos c = creeaza();` nu apelează move constructor-ul?** Din cauza *copy elision* / RVO: compilatorul construiește obiectul direct la destinație. De aceea demonstrația folosește `std::move` explicit și `std::vector`.
