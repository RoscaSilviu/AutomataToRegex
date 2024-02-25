#include "Automata.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

void Automata::ReadFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    // Citirea stărilor
    std::getline(file, line);
    std::istringstream statesStream(line);
    std::string state;
    while (statesStream >> state) {
        m_states.insert(state);
    }

    // Citirea stărilor de start
    std::getline(file, line);
    m_initialState = line;

    // Citirea stărilor finale
    std::getline(file, line);
    std::istringstream finalStatesStream(line);
    while (finalStatesStream >> state) {
        m_finalStates.insert(state);
    }

    // Citirea alfabetului
    std::getline(file, line);
    std::istringstream alphabetStream(line);
    char symbol;
    while (alphabetStream >> symbol) {
        m_alphabet.insert(symbol);
    }

    // Citirea tranzitiilor
    while (std::getline(file, line)) {
        std::istringstream transitionStream(line);
        std::string fromState, toState;
        std::string inputSymbol;

        transitionStream >> fromState >> inputSymbol >> toState;
        AddTransition(fromState, inputSymbol, toState);
    }

    file.close();
}
void Automata::AddTransition(std::string fromState, std::string inputSymbol, std::string toState)
{
    m_transitions.push_back(std::make_tuple(fromState,inputSymbol, toState));
}

void Automata::RemoveState(const std::string& state) {
    std::vector<std::pair<std::string, std::string>> firstStates;
    std::vector<std::pair<std::string, std::string>> secondStates;
    bool hasRecursive = false;
    std::string recursiveSymbol;
    for (const auto& transition : m_transitions) {
        if (std::get<0>(transition) == state && std::get<2>(transition) == state) {
            hasRecursive = true;
            recursiveSymbol = std::get<1>(transition) + "*";
        }
    }
    for (const auto& transition : m_transitions) {
        if (std::get<0>(transition) == state) {
            secondStates.push_back(std::make_pair(std::get<2>(transition), std::get<1>(transition)));
        }
        if (std::get<2>(transition) == state) {
            firstStates.push_back(std::make_pair(std::get<0>(transition), std::get<1>(transition)));
        }
    }
    

    // Adăugăm tranzițiile rezultate din combinarea stărilor inițiale și finale
    for (const auto& state1 : firstStates) {
        for (const auto& state2 : secondStates) {
            if (hasRecursive)
            AddTransition(state1.first, state1.second + recursiveSymbol + state2.second, state2.first);
            else
			AddTransition(state1.first, state1.second + state2.second, state2.first);
        }
    }

    // Eliminăm toate tranzițiile care implică starea specificată
    m_transitions.erase(std::remove_if(m_transitions.begin(), m_transitions.end(),
        [state](const auto& transition) {
            return std::get<0>(transition) == state || std::get<2>(transition) == state;
        }), m_transitions.end());
}


void Automata::CombineTransitions() {
    // Folosim un iterator pentru a parcurge și actualiza vectorul de tranziții
    for (auto it = m_transitions.begin(); it != m_transitions.end(); ++it) {
        const auto& transition = *it;

        // Căutăm alte tranziții cu aceeași stare de plecare ,simbol diferit si aceeasi stare de final
        auto matchingTransition = std::find_if(m_transitions.begin(), m_transitions.end(),
            [&transition](const auto& otherTransition) {
                return std::get<0>(transition) == std::get<0>(otherTransition) && std::get<2>(transition) == std::get<2>(otherTransition) && std::get<1>(transition) != std::get<1>(otherTransition);
            });

        // Dacă am găsit o tranziție similară
        if (matchingTransition != m_transitions.end()) {
            // Adăugăm o nouă tranziție combinată
            AddTransition(std::get<0>(transition), "("+std::get<1>(*matchingTransition) + "+" + std::get<1>(transition)+")", std::get<2>(transition));

            // Ștergem tranzițiile vechi
            m_transitions.erase(matchingTransition);
            m_transitions.erase(it);
            // Reinițializăm iteratorul pentru a începe din nou parcurgerea
            it = m_transitions.begin();
        }
    }
}


void Automata::ApplyRule1() {
    // Verificăm dacă există tranziții de intrare către starea inițială
    bool hasIncomingTransitions = std::any_of(
        m_transitions.begin(),
        m_transitions.end(),
        [this](const auto& transition) {
            return std::get<2>(transition) == m_initialState;
        }
    );

    // Dacă există tranziții de intrare către starea inițială, aplicăm regula
    if (hasIncomingTransitions) {
        // Creăm o nouă stare care devine noul start state
        std::string newStartState = "qstart";
        m_states.insert(newStartState);

        // Adăugăm o tranziție de la noua stare către vechea stare inițială cu Ɛ-transition
        AddTransition(newStartState, "ε", m_initialState);

        m_initialState = newStartState; // Actualizăm starea inițială a automatului
    }
}

void Automata::ApplyRule2() {
    std::string newFinalState = "qfinal";
    m_states.insert(newFinalState);

    // Conectăm toate stările finale la noua stare finală cu Ɛ-transition
    for (const auto& finalState : m_finalStates) {
        AddTransition(finalState, "ε", newFinalState);
    }

    // Actualizăm lista stărilor finale ale automatului
    m_finalStates.clear();
    m_finalStates.insert(newFinalState);
}

void Automata::AutomatatoRegex() {
    ApplyRule1();
    ApplyRule2();
    CombineTransitions();
    while (m_states.size() > 2) {
        for (const auto& state : m_states) {
            if (m_finalStates.find(state) == m_finalStates.end() && state != m_initialState) {
                RemoveState(state);
                CombineTransitions();
            }
        }
    }

}



//Getteri
const std::unordered_set<std::string>& Automata::GetStates() const {
    return m_states;
}

const std::unordered_set<char>& Automata::GetAlphabet() const {
    return m_alphabet;
}

const std::vector<std::tuple<std::string, std::string, std::string>>& Automata::GetTransitions() const {
    return m_transitions;
}

const std::string& Automata::GetInitialState() const {
    return m_initialState;
}

const std::unordered_set<std::string>& Automata::GetFinalStates() const {
    return m_finalStates;
}

// Setteri
void Automata::SetStates(const std::unordered_set<std::string>& states) {
    m_states = states;
}

void Automata::SetAlphabet(const std::unordered_set<char>& alphabet) {
    m_alphabet = alphabet;
}

void Automata::SetTransitions(const std::vector<std::tuple<std::string, std::string, std::string>>& transitions) {
    m_transitions = transitions;
}

void Automata::SetInitialState(const std::string& initialState) {
    m_initialState = initialState;
}

void Automata::SetFinalStates(const std::unordered_set<std::string>& finalStates) {
    m_finalStates = finalStates;
}