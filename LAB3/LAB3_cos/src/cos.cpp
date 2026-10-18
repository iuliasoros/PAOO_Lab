#include <tema/cos.hpp>
#include <tema/contor_heap.hpp>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>

namespace tema {

// Constructor Cos
Cos::Cos(int id, const std::string& client, std::size_t capacitate)
    : id_(id)
    , client_(client)
    , capacitate_(capacitate)
    , numarProduse_(0)
    , preturi_(new double[capacitate])
{
    ++ContorHeap::blocuriActive;
    std::cout << "    [constructor]      " << client_ << ": new double[" << capacitate_ << "] @" << preturi_ << "\n";
}

// Copy constructor Cos
Cos::Cos(const Cos& other)
    : id_(other.id_)
    , client_(other.client_)
    , capacitate_(other.capacitate_)
    , numarProduse_(other.numarProduse_)
    , preturi_(new double[other.capacitate_])   // memorie NOUA, proprie copiei
{
    // copiem CONTINUTUL tabloului, nu adresa lui (deep copy)
    std::copy(other.preturi_, other.preturi_ + other.numarProduse_, preturi_);
    ++ContorHeap::blocuriActive;
    std::cout << "    [copy constructor] " << client_ << ": new double[" << capacitate_ << "] @" << preturi_
              << " (preturi copiate din @" << other.preturi_ << ")\n";
}

// Move constructor Cos
Cos::Cos(Cos&& other) noexcept
    : id_(other.id_)
    , client_(std::move(other.client_))
    , capacitate_(other.capacitate_)
    , numarProduse_(other.numarProduse_)
    , preturi_(other.preturi_)                  // preluam pointerul, fara new[] si fara copiere
{
    // Sursa nu mai detine memoria. Fara liniile de mai jos, destructorul sursei
    // ar elibera tabloul pe care tocmai l-am preluat (dangling pointer + double free).
    other.preturi_ = nullptr;
    other.capacitate_ = 0;
    other.numarProduse_ = 0;
    std::cout << "    [move constructor] " << client_ << ": preia @" << preturi_ << " (fara alocare)\n";
}

// Destructor Cos
Cos::~Cos()
{
    if (preturi_ != nullptr)
    {
        std::cout << "    [destructor]       " << client_ << ": delete[] @" << preturi_ << "\n";
        delete[] preturi_;
        --ContorHeap::blocuriActive;
    }
    else
    {
        std::cout << "    [destructor]       (obiect mutat, nu mai detine memorie)\n";
    }
}

// Aici sunt metodele pentru clasa Cos
bool Cos::adaugaPret(double pret)
{
    if (!pretValid(pret) || numarProduse_ == capacitate_)
    {
        return false;
    }
    preturi_[numarProduse_++] = pret;
    return true;
}

bool Cos::modificaPret(std::size_t index, double pretNou)
{
    if (index >= numarProduse_ || !pretValid(pretNou))
    {
        return false;
    }
    preturi_[index] = pretNou;
    return true;
}

int Cos::id() const
{
    return id_;
}

const std::string& Cos::client() const
{
    return client_;
}

std::size_t Cos::numarProduse() const
{
    return numarProduse_;
}

double Cos::total() const
{
    double suma = 0.0;
    for (std::size_t i = 0; i < numarProduse_; ++i)
    {
        suma += preturi_[i];
    }
    return suma;
}

void Cos::afiseaza() const
{
    std::cout << "    #" << id_ << " " << (client_.empty() ? "(fara nume)" : client_) << " | preturi: [";
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < numarProduse_; ++i)
    {
        std::cout << (i ? " " : "") << preturi_[i];
    }
    std::cout << "] @" << preturi_ << " | total " << total() << " lei\n";
}

bool Cos::pretValid(double pret)
{
    return pret > 0.0 && pret <= 100000.0;
}

} // namespace tema
