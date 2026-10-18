#ifndef TEMA_COS_HPP
#define TEMA_COS_HPP

#include <cstddef>
#include <string>

namespace tema {

class Cos
{
public:
    Cos(int id, const std::string& client, std::size_t capacitate);
    Cos(const Cos& other);
    Cos(Cos&& other) noexcept;
    ~Cos();

    // Atribuirile nu fac parte din tema. Le interzicem explicit: altfel compilatorul ar genera
    // un operator= care copiaza doar pointerul (aceeasi problema ca la lipsa copy constructor-ului).
    // Oricum, membrul const id_ face ca atribuirea sa nu aiba sens.
    Cos& operator=(const Cos&) = delete;
    Cos& operator=(Cos&&) = delete;

public:
    bool adaugaPret(double pret);
    bool modificaPret(std::size_t index, double pretNou);
    int id() const;
    const std::string& client() const;
    std::size_t numarProduse() const;
    double total() const;
    void afiseaza() const;

private:
    static bool pretValid(double pret);

private:
    // Membrii se initializeaza in ORDINEA DECLARARII (nu in ordinea din lista de initializare),
    // de aceea preturi_ (care foloseste capacitate_) este declarat dupa capacitate_.
    const int   id_;            // const -> poate primi valoare DOAR in lista de initializare
    std::string client_;
    std::size_t capacitate_;
    std::size_t numarProduse_;
    double*     preturi_;       // resursa de pe heap detinuta de obiect
};

} // namespace tema

#endif // TEMA_COS_HPP
