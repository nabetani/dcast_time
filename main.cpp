#include <array>
#include <chrono>
#include <functional>
#include <iostream>

using namespace std;
using namespace std::chrono;

#define DERIVE_A(base, derived)                                                \
  class derived : public base {                                                \
    int derived_##member;                                                      \
                                                                               \
  public:                                                                      \
    derived() : derived_##member(0) {}                                             \
  };

#define DERIVE_C(base, derived, num)                                           \
  class derived : public base {                                                \
    int derived_##member;                                                      \
                                                                               \
  public:                                                                      \
    derived() : derived_##member(0) {}                                             \
    int hoge() const override { return num; };                                 \
    int get_##derived() const { return num; };                                 \
  };

class root {
public:
  virtual ~root() {}
  virtual int hoge() const = 0;
};

constexpr int MONKEY_VAL = 256;

DERIVE_A(root, vertebrate)
DERIVE_A(vertebrate, fish)
DERIVE_A(fish, amphibian)
DERIVE_C(fish, shark, __LINE__)
DERIVE_C(fish, tuna, __LINE__)
DERIVE_A(amphibian, reptile)
DERIVE_C(amphibian, newt, __LINE__)
DERIVE_C(amphibian, frog, __LINE__)
DERIVE_A(reptile, mammal)
DERIVE_C(reptile, crocodile, __LINE__)
DERIVE_C(reptile, snake, __LINE__)
DERIVE_C(mammal, monkey, MONKEY_VAL)
DERIVE_C(mammal, whale, __LINE__)
DERIVE_A(reptile, bird)
DERIVE_C(bird, penguin, __LINE__)
DERIVE_C(bird, sparrow, __LINE__)

constexpr size_t COUNT = 10000;

std::function<root *()> newAnimals[] = {
    []() -> root * { return new shark; },
    []() -> root * { return new tuna; },
    []() -> root * { return new newt; },
    []() -> root * { return new frog; },
    []() -> root * { return new crocodile; },
    []() -> root * { return new snake; },
    []() -> root * { return new monkey; },
    []() -> root * { return new whale; },
    []() -> root * { return new penguin; },
    []() -> root * { return new sparrow; },
};

constexpr size_t newAnimalsCount = sizeof(newAnimals) / sizeof(*newAnimals);

int dynamic_bench(std::array<root *, COUNT> const &m) {
  int sum = 0;
  for (auto const &e : m) {
    if (auto p = dynamic_cast<monkey const *>(e)) {
      sum +=p->get_monkey();
    }
  }
  return sum;
}

int static_bench(std::array<root *, COUNT> const &m) {
  int sum = 0;
  for (auto const &e : m) {
    if ( MONKEY_VAL == e->hoge() ){
      sum += static_cast<monkey const *>(e)->get_monkey();
    }
  }
  return sum;
}

void test(int num) {
  std::array<root *, COUNT> m = {0};
  for (auto &e : m) {
    e = newAnimals[++num % newAnimalsCount]();
  }
  for( int i=0 ; i<3 ; ++i ){
    {
      auto start = high_resolution_clock::now();
      auto r = dynamic_bench(m);
      auto end = high_resolution_clock::now();
      std::cout << "res=" << r << "\n";
      std::cout << duration_cast<microseconds>(end - start).count() * 1e-3
                << "ms\n";
    }
    {
      auto start = high_resolution_clock::now();
      auto r = static_bench(m);
      auto end = high_resolution_clock::now();
      std::cout << "res=" << r << "\n";
      std::cout << duration_cast<microseconds>(end - start).count() * 1e-3
                << "ms\n";
    }
  }
}

int main(int argc, char const *argv[]) {
  test(argc < 2 ? 100 : std::atoi(argv[1]));
}
