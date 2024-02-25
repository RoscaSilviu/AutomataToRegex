#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>

class Automata {

public:
	Automata() = default;
    ~Automata()=default;

public:
    void ReadFromFile(const std::string& filename);
    void AutomatatoRegex();
    
private:
    void AddTransition(std::string fromState, std::string inputSymbol, std::string toState);
    void RemoveState(const std::string& state);
    void ApplyRule1();
    void ApplyRule2();
    void CombineTransitions();

    // Getteri
    const std::unordered_set<std::string>& GetStates() const;
    const std::unordered_set<char>& GetAlphabet() const;
    const std::vector<std::tuple<std::string, std::string, std::string>>& GetTransitions() const;
    const std::string& GetInitialState() const;
    const std::unordered_set<std::string>& GetFinalStates() const;

    // Setteri
    void SetStates(const std::unordered_set<std::string>& states);
    void SetAlphabet(const std::unordered_set<char>& alphabet);
    void SetTransitions(const std::vector<std::tuple<std::string, std::string, std::string>>& transitions);
    void SetInitialState(const std::string& initialState);
    void SetFinalStates(const std::unordered_set<std::string>& finalStates);

private:
    std::unordered_set<std::string> m_states;
    std::unordered_set<char> m_alphabet;
    std::vector<std::tuple<std::string, std::string,std::string>> m_transitions;
    std::string m_initialState;
    std::unordered_set<std::string> m_finalStates;
};
