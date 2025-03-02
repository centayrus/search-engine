#pragma once

#include <iostream>

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED
};

struct Document {
    Document() = default;
    Document(const int doc_id, const double rel, const int rat);
    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

std::ostream &operator<<(std::ostream &output, const Document &document);