#ifndef EXAMPLE_H
#define EXAMPLE_H
#include <string>

/**
 * This is a dummy class to demonstrate features of the boiler plate.
 */
class Dummy {
public:

  /**
   * Default constructur for Dummy (does nothing).
   */
  Dummy();
  /**
   * Returns a bool.
   * @return Always True.
   */
  bool doSomething();

  std::string say_hello();
};

#endif
