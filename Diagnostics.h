#ifndef COMPILERV2_DIAGNOSTICS_H
#define COMPILERV2_DIAGNOSTICS_H

#include <string>
#include <vector>
#include "Token.h"

using std::string;
using std::vector;

enum class Severity {
    Error,
    Warning
};

struct Diagnostic {
    Severity severity;
    string message;
    Location location;

    bool operator==(const Diagnostic & diagnostic) const = default;
};

class Diagnostics {
    vector <Diagnostic> diagnostics;

public:

    void error(const string &message, const Location &location);
    void warning(const string &message, const Location &location);
    vector <Diagnostic> getAll();
};

#endif //COMPILERV2_DIAGNOSTICS_H
