#include "re/match.hpp"
#include "re/nfa.hpp"

#include <algorithm>
#include <utility>
#include <cstddef>

namespace re {

Matcher::Matcher(const NFA& nfa) : nfa_(nfa) {
  std::size_t size = nfa_.states.size();
  already_on_.resize(size, false);
  current_states_.reserve(size);
  next_states_.reserve(size);
}


/**
 * @brief Compute the epsilon closure and directly add the results to next_states_.
 *
 * @param id State IDs pending computation.
 */
void Matcher::add_state(StateID id) {
  if (id == kNullState) return;
  if (already_on_[id]) return;

  already_on_[id] = true;

  const State& state = nfa_.states[id];
  if (state.type == StateType::Epsilon) {
    add_state(state.out1);
    if (state.out2 != kNullState) add_state(state.out2);
  } else {
    next_states_.push_back(id);
  }
}

/**
 * @brief Swap next_states_ with current_states_ and clear the markers.
 */
void Matcher::step() {
  std::swap(current_states_, next_states_);
  next_states_.clear();
  std::fill(already_on_.begin(), already_on_.end(), false);
}

bool Matcher::is_match(std::string_view text) {
  // Ensure the env is clean
  step();
  // Add the start node to the epsilon closure
  add_state(nfa_.start);
  // Set the closure set as current_states_.
  step();

  for (char c : text) {
    for (StateID id : current_states_) {
      const State& state = nfa_.states[id];
      if (state.type == StateType::Consume && state.c == c) {
        add_state(state.out1);
      } else if (state.type == StateType::Dot) {
        add_state(state.out1);
      }
    }

    // Set the closure set as current_states_.
    step();

    if (current_states_.empty()) return false;
  }

  for (StateID id : current_states_) {
    State state = nfa_.states[id];
    if (state.type == StateType::Match) return true;
  }

  return false;
}

bool Matcher::search(std::string_view text) {
  // Ensure the env is clean
  step();

  for (char c : text) {
    // Core:
    // For each input character, 
    // while advancing the previous set of matchedion states, 
    // also start a new match stream from the current position.
    add_state(nfa_.start);
    step();

    // Core:
    // Since we are searching for a substring, 
    // the match is considered successful as long as 
    // any state flow successfully reaches the Match state.
    for (StateID id : current_states_) {
      if (nfa_.states[id].type == StateType::Match) {
        return true; 
      }
    }

    for (StateID id : current_states_) {
      const State& state = nfa_.states[id];
      if (state.type == StateType::Consume && state.c == c) {
        add_state(state.out1);
      } else if (state.type == StateType::Dot) {
        add_state(state.out1);
      }
    }
  }

  // Handle the case of an empty string (empty strings do not enter the for loop)
  add_state(nfa_.start);
  step();
  for (StateID id : current_states_) {
    State state = nfa_.states[id];
    if (state.type == StateType::Match) return true;
  }

  return false;
}

bool is_match(const NFA& nfa, std::string_view text) {
  Matcher matcher(nfa);
  return matcher.is_match(text);
}

bool search(const NFA& nfa, std::string_view text) {
  Matcher matcher(nfa);
  return matcher.search(text);
}

} // namespace re
