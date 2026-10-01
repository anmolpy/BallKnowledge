#include "../src/api_key.h"
#include <cassert>
void setKey(const char* value) {
#ifdef _WIN32
    _putenv_s("FOOTBALL_DATA_API_KEY", value ? value : "");
#else
    if (value) setenv("FOOTBALL_DATA_API_KEY", value, 1);
    else unsetenv("FOOTBALL_DATA_API_KEY");
#endif
}
int main() {
    setKey(nullptr);
    bool missing = false;
    try { footballApiKey(); } catch (const std::runtime_error&) { missing = true; }
    assert(missing);
    setKey("");
    missing = false;
    try { footballApiKey(); } catch (const std::runtime_error&) { missing = true; }
    assert(missing);
    setKey("synthetic-test-value");
    assert(footballApiKey() == "synthetic-test-value");
}
