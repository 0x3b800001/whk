#include <sys/ioctl.h>
#include <unistd.h>

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

std::vector<std::string> split(std::string s) {
  std::vector<std::string> v;
  for (char i : s) {
    if (i == '[') {
      v.push_back("[");
    } else if (i == ']') {
      v.back().push_back(']');
      v.push_back("");
    } else {
      if (v.empty()) {
        v.push_back("");
      }
      v.back().push_back(i);
    }
  }
  std::vector<std::string> res;
  for (auto i : v) {
    if (i == " ") {
      res.back().push_back(' ');
      continue;
    }
    res.push_back(i);
  }
  return res;
}

std::vector<std::string> del(std::string s) {
  std::vector<std::string> res{""};
  auto q = split(s);
  bool ban = 0;
  for (auto i : q) {
    if (i[0] == '[' && !ban && (rng() % 3)) {
      res.push_back(i.substr(1, i.find(']') - 1));
      i = "[" +
          std::string(
              i.find(']') - 1 + std::uniform_int_distribution<>(4, 6)(rng),
              ' ') +
          "]" + i.substr(i.find(']') + 1);
      ban = 1;
    } else {
      std::string rr;
      for (char j : i) {
        if (j != '[' && j != ']') {
          rr.push_back(j);
        }
      }
      i = rr;
      ban = 0;
    }
    res[0] += i;
  }
  return res;
}

void gotox(int i) {
  int col = getcol();
  std::cout << "\033[" << i / col + 1 << ";" << i % col + 1 << "H"
            << std::flush;
}

int main() {
  auto start = std::chrono::system_clock::now();
  std::system("clear");
  std::string s;
  std::ifstream ii("passage.txt");
  std::getline(ii, s);
  auto q = del(s);
  if (q.size() == 1) {
    std::cout << "Error! Please try again.\n";
    return 0;
  }
  std::cout << q[0] << std::endl;
  int wrong = 0;
  for (int i = 1; i < int(q.size()); ++i) {
    gotox(q[0].find('[') + 3);
    std::string answer;
    std::getline(std::cin, answer);
    gotox(q[0].find('[') + 3);
    std::cout << q[0].substr(q[0].find('[') + 3) << std::flush;
    if (answer != q[i]) {
      gotox(q[0].find("[") + 3);
      std::cout << "\033[91m" << q[i] << "\033[0m";
      ++wrong;
    } else {
      gotox(q[0].find("[") + 3);
      std::cout << "\033[92m" << q[i] << "\033[0m";
    }
    q[0][q[0].find('[')] = ' ';
  }
  gotox(q[0].size());
  int d = std::round((q.size() - 1.0 - wrong) / (q.size() - 1) * 100);
  auto end = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "\nScore: ";
  if (0 <= d && d < 80) {
    std::cout << "\e[91m" << d <<"\e[0m.\n";
  } else if (80 <= d && d < 90) {
    std::cout << "\e[93m" << d <<"\e[0m.\n";
  } else if (90 <= d && d < 100) {
    std::cout << "\e[32m" << d <<"\e[0m.\n";
  } else {
    std::cout << "\e[92m100\e[0m. Congratulations!\n";
  }
  std::cout << "Time: "
            << double(duration.count()) *
                   std::chrono::microseconds::period::num /
                   std::chrono::microseconds::period::den
            << "s\n";
  return 0;
}