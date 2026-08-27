#include "Diagnostics.h"
#include "iostream"

void Diagnostics::error(const string &message, const Location &location) {
    Diagnostic diagnostic;
    diagnostic.severity = Severity::Error;
    diagnostic.message = message;
    diagnostic.location = location;

    for (const auto &diag : diagnostics) {
        if (diag == diagnostic) {
            return;
        }
    }
    diagnostics.push_back(diagnostic);
}

void Diagnostics::warning(const string &message, const Location &location) {
    Diagnostic diagnostic;
    diagnostic.severity = Severity::Warning;
    diagnostic.message = message;
    diagnostic.location = location;
    diagnostics.push_back(diagnostic);
}

vector <Diagnostic> Diagnostics::getAll()  {
    return diagnostics;
}