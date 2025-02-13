// curl is pretty straightforward, until the C kicks in
//
// .env -> must contain GEMINI_KEY
#include <curl/curl.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#define ENV unordered_map<string, string>

using namespace std;

enum Gemini { USER, AI };

class GeminiResponseParser {
 public:
  GeminiResponseParser() {}

  void push(enum Gemini user, string& prompt) {
    stringstream prompt_stream;
    string role = (user == Gemini::AI) ? "model" : "user";

    prompt_stream << "{ \"parts\":[{\"text\": \"" << prompt << "\"}]";
    prompt_stream << ", \"role\": \"" << role << "\" }";

    this->history.push_back(prompt_stream.str());
  }

  string str() {
    stringstream format;

    format << "{ \"contents\": [\n";
    int index = 0;
    for (auto& chat : history) {
      format << "\t" << chat;
      if (index++ != history.size() - 1)
        format << ",";
      format << "\n";
    }

    format << "] }";

    return format.str();
  }

  string_view parseResponse(string_view sv) {
    int text_loc = sv.find("\"text\":");
    if (text_loc == string::npos)
      return sv;
    sv.remove_prefix(text_loc + 9);

    text_loc = sv.find("\"role\": \"model\"");
    if (text_loc == string::npos)
      return sv;
    sv.remove_suffix(sv.length() - text_loc + 35);

    return sv;
  }

 private:
  vector<string> history;
};

void load_env(ENV& env) {
  fstream f(".env");
  if (!f) {
    cerr << "\033[31mError loading environ variables from file '.env'\033[0m"
         << endl;
    exit(1);
  }
  string env_key;
  string env_target;

  while (std::getline(f, env_key, '=')) {
    std::getline(f, env_target, '\n');
    env[env_key] = env_target;
  }

  f.close();
}

// Callback function to handle the received Chunk from server
// contents -> rec data
// size     -> size of one chunk
// nmemb    -> num of chunks
// userp    -> the pointer to user defined data: readBuffer
//
// return   -> number of bytes processed
static size_t WriteCallback(void* contents,
                            size_t size,
                            size_t nmemb,
                            void* userp) {
  ((string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

atomic<bool> displayWait = true;

void waiting() {
  // clang-format off
  vector<vector<string>> load = {
    { "╦", "═", "╦", "═", "╦", "═", "╒", "╕", },
    { "╩", "╦", "╝", "╔", "╝", "╔", "╞", "╡", },
    { "═", "╩", "═", "╩", "═", "╩", "╘", "╛", }
  };
  // clang-format on

  cout << "\033[0m" << flush;
  int i = 0;
  while (displayWait) {
    stringstream bffr;
    if (i++ > 30)
      i = 0;

    /*
     * ╒╦═╦═╦═╦═╦═╦═╦═╦═╦═╦═╦═╕
     * ╞╝╔╝╔╝╔╝╔╝╔╝╔╝╔╝╔╝╔╝╔╝╔╡
     * ╘═╩═╩═╩═╩═╩═╩═╩═╩═╩═╩═╩╛
     * ╒╦═╦═╦═╦═╦═╦═╦═╦═╦═╦═╦═╕
     * ╞╩╦╝╔╝╔╩╦╝╔╝╔╩╦╝╔╝╔╩╦╝╔╡
     * ╘═╩═╩═╩═╩═╩═╩═╩═╩═╩═╩═╩╛
     *
     * ╒ ╦═╦═╦═ ╕
     * ╞ ╩╦╝╔╝╔ ╡
     * ╘ ═╩═╩═╩ ╛
     *
     * ╒╦═╦═╦═╦═╦═╦═╦═╦═╦═╦═╦═╕
     * ╞╩╦╝╔∙l o a d i n╝╔╩╦╝╔╡
     * ╘═╩═╩═╩═╩═╩═╩═╩═╩═╩═╩═╩╛
     */

    for (int line = 0; line < 3; line++) {
      for (int j = 0; j <= i; j++) {
        if (j == 0) {
          bffr << load[line][6];
        } else if (j == i) {
          bffr << load[line][7];
        } else {
          bffr << load[line][j % 4];
        }
      }
      bffr << "\n";
    }

    cout << "\033[38;2;186;187;241m\r" << bffr.str() << flush;
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "\033[1A\033[2K\r\033[1A\033[2K\r\033[1A\033[2K\r" << flush;
  }
  cout << "\n\033[0m" << flush;
}

void initCurl(ENV& env, CURL* curl, struct curl_slist* headers) {
  if (env.find("GEMINI_KEY") == env.end()) {
    cerr << "please provide a GEMINI_KEY to proceed" << endl;
    exit(1);
  }
  cout << env["LEAKED_API_KEY"] << '\n';

  const string GEMINI_URL =
      "https://generativelanguage.googleapis.com/v1beta/models/"
      "gemini-1.5-flash:generateContent?key=" +
      env["LEAKED_API_KEY"];

  cout << GEMINI_URL << "\n";
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, GEMINI_URL.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
}

void clean(string& str) {
  str.erase(str.find_last_not_of(' ') + 1);
  str.erase(0, str.find_first_not_of(' '));
}

void peek_history(GeminiResponseParser& gemini) {
  fstream f("gemini_history.txt", ios::out);
  f << gemini.str();
  f.close();

  int fd = open("ge_run_history.sh", O_WRONLY | O_CREAT, 0777);

  string s;
  stringstream ss;
  ss << "#!bin/bash" << endl;
  ss << endl;
  ss << "less gemini_history.txt" << endl;
  ss << "rm -rf gemini_history.txt" << endl;
  ss << "rm -rf ge_run_history.sh";
  s = ss.str();
  write(fd, s.c_str(), s.length());
  close(fd);

  if (fork() == 0) {
    execl("/bin/bash", "bash", "ge_run_history.sh", NULL);
  }
  wait(NULL);
}

int main() {
  CURL* curl;
  CURLcode res;
  ENV env;
  GeminiResponseParser gemini;
  struct curl_slist* headers = nullptr;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  load_env(env);
  curl = curl_easy_init();
  if (!curl) {
    cerr << "Curly error";
    exit(1);
  }
  initCurl(env, curl, headers);

  while (1) {
    string prompt = "";
    string readBuffer;

    cout << "\033[0m";
    cout << "\033[90m\n \033[1m>\033[0m \033[90m";
    getline(cin, prompt);
    clean(prompt);

    if (prompt.empty())
      continue;
    if (prompt == "/clear") {
      cout << "\033[2J\033[H";
      continue;
    }
    if (prompt == "/history" || prompt == "/hist") {
      cout << "\033[0m" << flush;
      peek_history(gemini);
      continue;
    }

    gemini.push(Gemini::USER, prompt);

    string history = gemini.str();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, history.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    displayWait = true;
    thread wait(waiting);
    res = curl_easy_perform(curl);
    displayWait = false;
    wait.join();

    cout << "\033[0m\033[1m\033[38;2;186;187;241m";
    string parsedResponse =
        (string)gemini.parseResponse((string_view)readBuffer);
    gemini.push(Gemini::AI, parsedResponse);
    cout << parsedResponse << endl << endl;
  }
  curl_easy_cleanup(curl);
}
