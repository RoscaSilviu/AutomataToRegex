#include "Automata.h"
void main() {
	Automata automata;
	automata.ReadFromFile("automata.txt");
	automata.AutomatatoRegex();
}