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
#include <thread>
#include <unordered_map>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#define ENV unordered_map<string, string>

using namespace std;

namespace ANSI {
const string DEFAULT = "\033[0m";
const string CURSOR_BEGIN = "\r";
const string BOLD = "\033[1m";

const string GRAY_FG = "\033[90m";
const string RED_FG = "\033[31m";
const string BABBF1 = "\033[38;2;186;187;241m";

const string MOVE1UP = "\033[1A";
const string MOVETOP = "\033[H";

const string CLEAR_SCREEN = "\033[2J";
const string CLEAR_LINE = "\033[2K\r";
}  // namespace ANSI

enum Gemini { USER, AI };

class GeminiResponseParser {
 public:
  GeminiResponseParser();
  void push(enum Gemini user, string& prompt);
  string str();
  string parseResponse(string& sv);

 private:
  vector<string> history;
  rapidjson::Document d;
};

void load_env(ENV&);
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
                            void* userp);
void waiting();
void initCurl(ENV&, CURL*, struct curl_slist*);
void clean(string&);
void peek_history(GeminiResponseParser&);

atomic<bool> displayWait = true;

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

    cout << ANSI::DEFAULT;
    cout << ANSI::GRAY_FG << ANSI::BOLD << "\n > " << ANSI::DEFAULT
         << ANSI::GRAY_FG;
    getline(cin, prompt);
    clean(prompt);

    if (prompt.empty())
      continue;
    if (prompt == "/clear") {
      cout << ANSI::CLEAR_SCREEN << ANSI::MOVETOP << flush;
      continue;
    }
    if (prompt == "/history" || prompt == "/hist") {
      cout << ANSI::DEFAULT << flush;
      peek_history(gemini);
      continue;
    }
    if (prompt == "/exit" || prompt == "/q") {
      cout << ANSI::DEFAULT << flush;
      return 0;
    }

    gemini.push(Gemini::USER, prompt);

    string history = gemini.str();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, history.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    displayWait = true;
    thread wait(waiting);
    res = curl_easy_perform(curl);
    string serializedResponse = (string)gemini.parseResponse(readBuffer);
    gemini.push(Gemini::AI, serializedResponse);
    displayWait = false;
    wait.join();

    cout << ANSI::DEFAULT << ANSI::BABBF1 << ANSI::BOLD;
    cout << readBuffer << endl;
  }
  curl_easy_cleanup(curl);
}

GeminiResponseParser::GeminiResponseParser() {}

void GeminiResponseParser::push(enum Gemini user, string& prompt) {
  string role = (user == Gemini::AI) ? "model" : "user";
  stringstream prompt_ss = (stringstream)prompt;

  d.SetObject();
  auto& allocator = d.GetAllocator();
  rapidjson::StringBuffer buffer;
  rapidjson::Value text_obj(rapidjson::kObjectType);
  rapidjson::Value text_arr_wrapper(rapidjson::kArrayType);
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  rapidjson::Value role_Value = rapidjson::Value(role.c_str(), allocator);
  rapidjson::Value prompt_Value = rapidjson::Value(prompt.c_str(), allocator);

  text_obj.AddMember("text", prompt_Value, allocator);
  text_arr_wrapper.PushBack(text_obj, allocator);
  d.AddMember("parts", std::move(text_arr_wrapper), allocator);
  d.AddMember("role", role_Value, allocator);

  d.Accept(writer);

  this->history.push_back(buffer.GetString());
}

string GeminiResponseParser::str() {
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

string GeminiResponseParser::parseResponse(string& sv) {
  string s = (string)sv;
  d.Parse(s.c_str());
  if (!d.HasMember("candidates")) {
    return sv;
  }

  rapidjson::Value& candidates =
      d["candidates"][0]["content"]["parts"][0]["text"];
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

  d["candidates"][0]["content"]["parts"][0]["text"].Accept(writer);

  sv = candidates.GetString();
  return sv;
}

void load_env(ENV& env) {
  fstream f(".env");
  if (!f) {
    cerr << ANSI::RED_FG << "Error loading environ variables from file '.env'"
         << ANSI::DEFAULT << endl;
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

static size_t WriteCallback(void* contents,
                            size_t size,
                            size_t nmemb,
                            void* userp) {
  ((string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

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

    cout << ANSI::BABBF1 << ANSI::CURSOR_BEGIN << bffr.str() << flush;
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << ANSI::MOVE1UP << ANSI::CLEAR_LINE << ANSI::MOVE1UP
         << ANSI::CLEAR_LINE << ANSI::MOVE1UP << ANSI::CLEAR_LINE << flush;
  }
  cout << "\n" << ANSI::DEFAULT << flush;
}

void initCurl(ENV& env, CURL* curl, struct curl_slist* headers) {
  if (env.find("GEMINI_KEY") == env.end()) {
    cerr << "please provide a GEMINI_KEY to proceed" << endl;
    exit(1);
  }

  const string GEMINI_URL =
      "https://generativelanguage.googleapis.com/v1beta/models/"
      "gemini-1.5-flash:generateContent?key=" +
      env["LEAKED_API_KEY"];

  cout << ANSI::BABBF1 << GEMINI_URL << "\n";
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
