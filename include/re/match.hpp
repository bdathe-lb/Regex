#ifndef RE_MATCH_HPP
#define RE_MATCH_HPP

#include "re/nfa.hpp"

#include <vector>
#include <string_view>

namespace re {

class Matcher {
private:
  const NFA& nfa_;
  std::vector<StateID> current_states_;
  std::vector<StateID> next_states_;
  std::vector<bool> already_on_;

public:
  explicit Matcher(const NFA& nfa);
  // Exact match.
  bool is_match(std::string_view text);
  // Partial match.
  bool search(std::string_view text);

private:
  // Compute the epsilon closure and directly add the results to next_states_.
  void add_state(StateID id);
  // Swap next_states_ with current_states_ and clear the markers.
  void step();
};

// Exact match.
bool is_match(const NFA& nfa, std::string_view text);
// Partial match.
bool search(const NFA& nfa, std::string_view text);

} // namespace re

#endif // !RE_MATCH_HPP
