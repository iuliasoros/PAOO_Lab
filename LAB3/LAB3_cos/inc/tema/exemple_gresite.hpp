#ifndef TEMA_EXEMPLE_GRESITE_HPP
#define TEMA_EXEMPLE_GRESITE_HPP

#include <cstddef>
#include <string>

namespace tema {

// GRESIT INTENTIONAT: aloca memorie in constructor, dar NU are destructor.
// Destructorul generat de compilator distruge doar pointerul (8 octeti),
// NU si memoria spre care arata -> memory leak la fiecare obiect.
class CosFaraDestructor
{
public:
    CosFaraDestructor(const std::string& client, std::size_t capacitate);

    // lipseste: ~CosFaraDestructor() { delete[] preturi_; }

private:
    std::string client_;
    double*     preturi_;
};

// GRESIT INTENTIONAT: are destructor, dar NU are copy constructor.
// Copy constructor-ul generat de compilator copiaza membru cu membru, deci copiaza
// ADRESA din preturi_, nu preturile (shallow copy). Originalul si copia ajung sa imparta
// acelasi tablou, iar la final ambii destructori fac delete[] pe aceeasi adresa.
class CosFaraCopyCtor
{
public:
    CosFaraCopyCtor(const std::string& client, std::size_t capacitate);

    ~CosFaraCopyCtor();

    // lipseste: CosFaraCopyCtor(const CosFaraCopyCtor& other);

public:
    void adaugaPret(double pret);
    void modificaPret(std::size_t index, double pretNou);
    void afiseaza(const char* eticheta) const;

private:
    std::string client_;
    std::size_t numarProduse_;
    double*     preturi_;
};

} // namespace tema

#endif // TEMA_EXEMPLE_GRESITE_HPP
