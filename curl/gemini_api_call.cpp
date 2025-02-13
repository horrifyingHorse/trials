// curl is pretty straightforward, until the C kicks in
//
// .env -> must contain GEMINI_KEY
#include <curl/curl.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
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

    format << "{ \"contents\": [";
    int index = 0;
    for (auto& chat : history) {
      format << chat;
      if (index++ != history.size() - 1)
        format << ",";
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
  string env_key;
  string env_target;

  if (!f) {
    cerr << "Error in opening file\n";
    return;
  }

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

int main() {
  CURL* curl;
  CURLcode res;
  struct curl_slist* headers = nullptr;
  ENV env;
  GeminiResponseParser gemini;

  load_env(env);
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
  string data = R"(
  {
    "contents": [{
      "parts":[{"text": "Hello There :D, Mornin!"}]
    }]
  })";

  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl = curl_easy_init();
  if (!curl) {
    cerr << "Nah curl";
    exit(1);
  }

  while (1) {
    string prompt = "";
    string readBuffer;
    cin.clear();
    cout << "\n > ";
    getline(cin, prompt);

    if (prompt.empty())
      continue;

    gemini.push(Gemini::USER, prompt);

    string data =
        "{ \"contents\": [{ \"parts\":[{\"text\": \"" + prompt + "\"}] }] }";
    // curl_easy_setopt(curl, CURLOPT_URL, "https://horrifyingHorse.github.io");
    string history = gemini.str();
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, history.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, GEMINI_URL.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    // curl_easy_setopt(curl, CURLOPT_POST, param)
    res = curl_easy_perform(curl);
    if (res != CURLcode::CURLE_OK) {
      cerr << "Error";
    }
    cout << gemini.parseResponse((string_view)readBuffer) << endl;
    // cout << readBuffer << endl;
  }
  curl_easy_cleanup(curl);
}
