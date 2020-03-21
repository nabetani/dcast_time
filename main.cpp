#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <type_traits>
using namespace std;
using namespace std::chrono;

// define class
#define DERIVE(base, derived, num)                                             \
  class derived : public base {                                                \
    int derived_##member;                                                      \
                                                                               \
  public:                                                                      \
    static inline constexpr int tid = num;                                     \
    derived() : derived_##member(0) {}                                         \
    int m() const { return derived_##member; }                                 \
    int derived##_proc() const { return num; };                                \
    bool is_a(int t) const override { return t == tid || base::is_a(t); }      \
  };

class root {
public:
  virtual ~root() {}
  virtual bool is_a(int t) const { return false; }
};

DERIVE(root, vertebrate, __LINE__)
DERIVE(vertebrate, fish, __LINE__)
DERIVE(fish, amphibian, __LINE__)
DERIVE(fish, shark, __LINE__)
DERIVE(fish, tuna, __LINE__)
DERIVE(amphibian, reptile, __LINE__)
DERIVE(amphibian, newt, __LINE__)
DERIVE(amphibian, frog, __LINE__)
DERIVE(reptile, mammal, __LINE__)
DERIVE(reptile, crocodile, __LINE__)
DERIVE(reptile, snake, __LINE__)
DERIVE(mammal, monkey, __LINE__)
DERIVE(monkey, aye_aye, __LINE__)
DERIVE(monkey, tamarin, __LINE__)
DERIVE(monkey, hominidae, __LINE__)
DERIVE(hominidae, gorilla, __LINE__)
DERIVE(hominidae, human, __LINE__)
DERIVE(mammal, whale, __LINE__)
DERIVE(whale, blue_whale, __LINE__)
DERIVE(whale, narwhal, __LINE__)
DERIVE(whale, sperm_whale, __LINE__)
DERIVE(reptile, bird, __LINE__)
DERIVE(bird, penguin, __LINE__)
DERIVE(penguin, king_penguin, __LINE__)
DERIVE(penguin, magellanic_penguin, __LINE__)
DERIVE(penguin, galapagos_penguin, __LINE__)
DERIVE(bird, sparrow, __LINE__)

constexpr size_t COUNT = 1'000'000;

std::function<root *()> newAnimals[] = {
    []() -> root * { return new shark(); },
    []() -> root * { return new tuna(); },
    []() -> root * { return new newt(); },
    []() -> root * { return new frog(); },
    []() -> root * { return new crocodile(); },
    []() -> root * { return new snake(); },
    []() -> root * { return new aye_aye(); },
    []() -> root * { return new tamarin(); },
    []() -> root * { return new gorilla(); },
    []() -> root * { return new human(); },
    []() -> root * { return new blue_whale(); },
    []() -> root * { return new narwhal(); },
    []() -> root * { return new sperm_whale(); },
    []() -> root * { return new king_penguin(); },
    []() -> root * { return new magellanic_penguin(); },
    []() -> root * { return new galapagos_penguin(); },
    []() -> root * { return new sparrow(); },
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
  if (p->is_a(std::remove_pointer<cast>::type::tid)) {
    return static_cast<cast>(p);
  }
  return nullptr;
}

template <typename cast> //
inline cast animal_cast(root const *p) {
  if (p->is_a(std::remove_pointer<cast>::type::tid)) {
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
    run("animal_cast", static_run, m, 2 <= i);
  }
}

int main(int argc, char const *argv[]) {
  test(argc < 2 ? 0 : std::atoi(argv[1]));
}
