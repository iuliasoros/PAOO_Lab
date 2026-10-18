#include <tema/exemple_gresite.hpp>
#include <tema/contor_heap.hpp>
#include <iomanip>
#include <iostream>

namespace tema {

// Clasa pentru CosFaraDestructor
CosFaraDestructor::CosFaraDestructor(const std::string& client, std::size_t capacitate)
    : client_(client)
    , preturi_(new double[capacitate])
{
    ++ContorHeap::blocuriActive;
    std::cout << "    [constructor]      " << client_ << ": new double[" << capacitate << "] @" << preturi_
              << "  (nu va fi eliberat niciodata)\n";
}

// Clasa pentru CosFaraCopyCtor
CosFaraCopyCtor::CosFaraCopyCtor(const std::string& client, std::size_t capacitate)
    : client_(client)
    , numarProduse_(0)
    , preturi_(new double[capacitate])
{
    ++ContorHeap::blocuriActive;
    std::cout << "    [constructor]      " << client_ << ": new double[" << capacitate << "] @" << preturi_ << "\n";
}

CosFaraCopyCtor::~CosFaraCopyCtor()
{
    std::cout << "    [destructor]       " << client_ << ": delete[] @" << preturi_ << std::endl;
    delete[] preturi_;
    --ContorHeap::blocuriActive;
}

void CosFaraCopyCtor::adaugaPret(double pret)
{
    preturi_[numarProduse_++] = pret;
}

void CosFaraCopyCtor::modificaPret(std::size_t index, double pretNou)
{
    preturi_[index] = pretNou;
}

void CosFaraCopyCtor::afiseaza(const char* eticheta) const
{
    std::cout << "    " << eticheta << client_ << " | preturi: [" << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < numarProduse_; ++i)
    {
        std::cout << (i ? " " : "") << preturi_[i];
    }
    std::cout << "] @" << preturi_ << "\n";
}

} // namespace tema
