#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>

using namespace std;
using namespace std::chrono;

// define abstract class
#define DERIVE_A(base, derived)                                                \
  class derived : public base {                                                \
    int derived_##member;                                                      \
                                                                               \
  public:                                                                      \
    derived() : derived_##member(0) {}                                         \
    int m() const { return derived_##member; }                                 \
  };

// define concrete class
#define DERIVE_C(base, derived, num)                                           \
  class derived : public base {                                                \
    int derived_##member;                                                      \
                                                                               \
  public:                                                                      \
    static inline constexpr int type = num;                                    \
    derived() : derived_##member(0) {}                                         \
    int m() const { return derived_##member; }                                 \
    int get_type() const override { return num; };                             \
    int get_##derived() const { return num; };                                 \
  };

class root {
public:
  virtual ~root() {}
  virtual int get_type() const = 0;
};

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
DERIVE_C(mammal, monkey, __LINE__)
DERIVE_C(mammal, whale, __LINE__)
DERIVE_A(reptile, bird)
DERIVE_C(bird, penguin, __LINE__)
DERIVE_C(bird, sparrow, __LINE__)

constexpr size_t COUNT = 1'000'000;

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

int dynamic_run(std::array<root *, COUNT> const &m) {
  int sum = 0;
  for (auto const &e : m) {
    if (auto p = dynamic_cast<monkey const *>(e)) {
      sum += p->get_monkey();
    }
    if (auto p = dynamic_cast<penguin const *>(e)) {
      sum += p->get_penguin();
    }
  }
  return sum;
}

int static_run(std::array<root *, COUNT> const &m) {
  int sum = 0;
  for (auto const &e : m) {
    if (monkey::type == e->get_type()) {
      sum += static_cast<monkey const *>(e)->get_monkey();
    }
    if (penguin::type == e->get_type()) {
      sum += static_cast<penguin const *>(e)->get_penguin();
    }
  }
  return sum;
}

void run(char const *title, decltype(dynamic_run) runner,
         std::array<root *, COUNT> const &m, bool production) {
  auto start = high_resolution_clock::now();
  auto r = runner(m);
  auto end = high_resolution_clock::now();
  if (production) {
    std::cout << title
              << "\n"
                 "  res="
              << r
              << "\n"
                 "  "
              << duration_cast<microseconds>(end - start).count() * 1e-3
              << "ms\n";
  }
}

void test(int num) {
  std::array<root *, COUNT> m = {0};
  std::mt19937_64 rng(num);
  std::uniform_int_distribution<size_t> dist(0, newAnimalsCount - 1);
  for (auto &e : m) {
    e = newAnimals[dist(rng)]();
  }
  for (int i = 0; i < 3; ++i) {
    run("dynamic_cast", dynamic_run, m, 2 <= i);
    run("static_cast", static_run, m, 2 <= i);
  }
}

int main(int argc, char const *argv[]) {
  test(argc < 2 ? 0 : std::atoi(argv[1]));
}
