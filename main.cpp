#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <type_traits>
using namespace std;
using namespace std::chrono;

// define abstract class
#define DERIVE_A(base, derived, num)                                           \
  class derived : public base {                                                \
    int derived_##member;                                                      \
                                                                               \
  public:                                                                      \
    static inline constexpr int type = num;                                    \
    derived() : derived_##member(0) {}                                         \
    int m() const { return derived_##member; }                                 \
    int derived##_proc() const { return num; };                                \
    bool is_a(int t) const override { return t == type || base::is_a(t); }     \
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
    int derived##_proc() const { return num; };                                \
    bool is_a(int t) const override { return t == type || base::is_a(t); }     \
  };

class root {
public:
  virtual ~root() {}
  virtual bool is_a(int t) const { return false; }
};

DERIVE_A(root, vertebrate, __LINE__)
DERIVE_A(vertebrate, fish, __LINE__)
DERIVE_A(fish, amphibian, __LINE__)
DERIVE_C(fish, shark, __LINE__)
DERIVE_C(fish, tuna, __LINE__)
DERIVE_A(amphibian, reptile, __LINE__)
DERIVE_C(amphibian, newt, __LINE__)
DERIVE_C(amphibian, frog, __LINE__)
DERIVE_A(reptile, mammal, __LINE__)
DERIVE_C(reptile, crocodile, __LINE__)
DERIVE_C(reptile, snake, __LINE__)
DERIVE_A(mammal, monkey, __LINE__)
DERIVE_C(monkey, aye_aye, __LINE__)
DERIVE_C(monkey, tamarin, __LINE__)
DERIVE_C(monkey, hominidae, __LINE__)
DERIVE_C(hominidae, gorilla, __LINE__)
DERIVE_C(hominidae, human, __LINE__)
DERIVE_A(mammal, whale, __LINE__)
DERIVE_C(whale, blue_whale, __LINE__)
DERIVE_C(whale, narwhal, __LINE__)
DERIVE_C(whale, sperm_whale, __LINE__)
DERIVE_A(reptile, bird, __LINE__)
DERIVE_A(bird, penguin, __LINE__)
DERIVE_C(penguin, king_penguin, __LINE__)
DERIVE_C(penguin, magellanic_penguin, __LINE__)
DERIVE_C(penguin, galapagos_penguin, __LINE__)
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
    if (auto p = dynamic_cast<mammal const *>(e)) {
      sum += p->mammal_proc();
    }
    if (auto p = dynamic_cast<bird const *>(e)) {
      sum += p->bird_proc();
    }
  }
  return sum;
}

template <typename cast> //
inline cast animal_cast(root *p) {
  if (p->is_a(std::remove_pointer<cast>::type::type)) {
    return static_cast<cast>(p);
  }
  return nullptr;
}

template <typename cast> //
inline cast animal_cast(root const *p) {
  if (p->is_a(std::remove_pointer<cast>::type::type)) {
    return static_cast<cast>(p);
  }
  return nullptr;
}

int static_run(std::array<root *, COUNT> const &m) {
  int sum = 0;
  for (auto const &e : m) {
    if (auto p = animal_cast<mammal const *>(e)) {
      sum += p->mammal_proc();
    }
    if (auto p = animal_cast<bird const *>(e)) {
      sum += p->bird_proc();
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
