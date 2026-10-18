#ifndef TEMA_DEMO_HPP
#define TEMA_DEMO_HPP

namespace tema {

// Cate o demonstratie pentru fiecare cerinta a temei
void demoIncapsulare();

void demoListaInitializare();

void demoDestructor();

void demoCopyConstructor();

void demoMoveConstructor();

// Ce se intampla FARA copy constructor propriu. Se termina cu crash (double free).
void demoFaraCopyConstructor();

// Afiseaza cate blocuri new[] au ramas neeliberate
void raportHeap();

} // namespace tema

#endif // TEMA_DEMO_HPP
