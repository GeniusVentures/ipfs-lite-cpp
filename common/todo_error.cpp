#include "common/todo_error.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(fc, TodoError, e) {
  using E = sgns::TodoError;
  switch (e) {
    case E::ERROR:
      return "Error";
  }
}
