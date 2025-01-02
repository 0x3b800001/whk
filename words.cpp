#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

int getcol() {
  struct winsize info;
  ioctl(STDIN_FILENO, TIOCGWINSZ, &info);
  return info.ws_col;
}

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

std::vector<std::pair<std::string, std::string>> words;
bool disle1(std::string s1, std::string s2){
  std::vector<int> s(26);
  for (auto i : s1) {
    if ('a' <= i && i <= 'z') {
      ++s[i - 'a'];
    }
  }
  for (auto i : s2) {
    if ('a' <= i && i <= 'z') {
      --s[i - 'a'];
    }
  }
  int c = 0;
  for (int i = 0; i < 26; ++i) {
    if (s[i] > 1 || s[i] < -1) {
      return false;
    }
    c += (s[i] != 0);
  }
  return c <= 2;
}
int main() {
  auto start = std::chrono::system_clock::now();
  std::system("clear");
  std::ifstream ii("words.txt");
  int n;
  ii >> n;
  std::string s_;
  std::getline(ii, s_);
  while (n--) {
    std::string s;
    std::getline(ii, s);
    std::vector<std::string> vv{""};
    for (char i : s) {
      if (i == ':') {
        vv.push_back("");
        continue;
      }
      if (vv.back() != "" || i != ' ') {
        vv.back().push_back(i);
      }
    }
    words.push_back({vv[0], vv[1]});
  }
  std::shuffle(words.begin(), words.end(), rng);
  std::string corr;
  std::ofstream oo("correction.txt");
  int wrong = 0, cc = 0;
  for (auto [i, j] : words) {
    int col = getcol();
    std::cout << "\e[101m";
    int t = std::ceil(1.0 * wrong / words.size() * col);
    while (t--) {
      std::cout << ' ';
    }
    std::cout << "\e[102m";
    t = std::ceil(1.0 * cc / words.size() * col) - std::ceil(1.0 * wrong / words.size() * col);
    while (t--) {
      std::cout << ' ';
    }
    std::cout << "\e[47m";
    t = col - std::ceil(1.0 * cc / words.size() * col);
    while (t--) {
      std::cout << ' ';
    }
    std::cout << "\e[49m\n";
    if (i.find(' ') == i.npos) {
      std::cout << i[0] << "_______: ";
    }
    std::cout << j << std::endl;
    std::string ans;
    std::getline(std::cin, ans);
    if (ans != i && disle1(ans,i)) {
      std::cout << "\033[1A\e[33m" << ans << "\e[0m\n" << std::flush;
      std::getline(std::cin, ans);
    }
    if (ans != i) {
      ++wrong;
      std::cout << "\033[1A\e[31m" << ans << "\e[0m\n" << std::flush;
      std::cout << "\e[32m" << i << "\e[0m\n" << std::flush;
      corr += i + ": " + j + '\n';
    } else {
      std::cout << "\033[1A\e[32m" << i << "\e[0m\n" << std::flush;
    }
    ++cc;
  }
  oo << wrong << '\n' << corr;
  oo.close();
  int d = std::round(double(words.size() - wrong) / (words.size()) * 100);
  auto end = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "\nScore: ";
  if (0 <= d && d < 80) {
    std::cout << "\e[31m" << d << "\e[0m.\n";
  } else if (80 <= d && d < 90) {
    std::cout << "\e[33m" << d << "\e[0m.\n";
  } else if (90 <= d && d < 100) {
    std::cout << "\e[92m" << d << "\e[0m.\n";
  } else {
    std::cout << "\e[32m100\e[0m. Congratulations!\n";
  }
  std::cout << "Time: "
            << double(duration.count()) *
                   std::chrono::microseconds::period::num /
                   std::chrono::microseconds::period::den
            << "s\n";
  std::cout << "For details see correction.txt.\n";
  return 0;
}
