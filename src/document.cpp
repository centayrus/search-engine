#include "document.h"

using namespace std;

    Document::Document(const int doc_id, const double rel, const int rat)
        : id(doc_id), relevance(rel), rating(rat) {}
    int id = 0;
    double relevance = 0.0;
    int rating = 0;

    std::ostream &operator<<(std::ostream &output, const Document &document) {
    output << "{ "s
           << "document_id = "s << document.id << ", "s
           << "relevance = "s << document.relevance << ", "s
           << "rating = "s << document.rating << " }"s;
    return output;
}