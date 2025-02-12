// curl is pretty straightforward, until the C kicks in
//
// .env -> must contain GEMINI_KEY
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#define ENV unordered_map<string, string>

using namespace std;

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
  string readBuffer;
  struct curl_slist* headers = nullptr;
  ENV env;

  load_env(env);
  if (env.find("GEMINI_KEY") == env.end()) {
    cerr << "please provide a GEMINI_KEY to proceed" << endl;
    exit(1);
  }
  cout << env["GEMINI_KEY"] << '\n';

  const string GEMINI_URL =
      "https://generativelanguage.googleapis.com/v1beta/models/"
      "gemini-1.5-flash:generateContent?key=" +
      env["GEMINI_KEY"];
  string data = R"(
  {
    "contents": [{
      "parts":[{"text": "Hello There :D, Mornin!"}]
    }]
  })";

  cout << GEMINI_URL << "\n";

  headers = curl_slist_append(headers, "Content-Type: application/json");

  curl = curl_easy_init();
  if (curl) {
    // curl_easy_setopt(curl, CURLOPT_URL, "https://horrifyingHorse.github.io");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, GEMINI_URL.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    // curl_easy_setopt(curl, CURLOPT_POST, param)
    res = curl_easy_perform(curl);
    cout << "\n" << res << "\n";
    if (res != CURLcode::CURLE_OK) {
      cerr << "Error";
    }
    cout << readBuffer << endl;
  } else {
    cerr << "Nah curl";
  }
}
