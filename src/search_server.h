#pragma once
#include "string_processing.h"
#include "document.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

const int MAX_RESULT_DOCUMENT_COUNT = 5;

class SearchServer {
   public:

    SearchServer() = default;
    // принимает коллекцию: set или vector
    // template <typename StopWordlist>
    // SearchServer(const StopWordlist &text);
    template <typename StopWordlist>
SearchServer(const StopWordlist &text) {
    for (const auto &i : text) {
        if (i == "") {
            continue;
        }
        if (!IsValidWord(i)) {
            throw std::invalid_argument("Строка содержит недопустимые символы");
        }
        stop_words_.insert(i);
    }
}

    // принимает string
    SearchServer(const std::string &text);

    // Возвращаем номер документа по его порядковому номеру
    int GetDocumentId(int index) const;

    void AddDocument(int document_id, const std::string &document, DocumentStatus status,
                     const std::vector<int> &ratings);

    // шаблонная функция, в параметр которой передается функция-предикат для динамической фильтрации
    template <typename Predicate>
    std::vector<Document> FindTopDocuments(const std::string &raw_query,
                                      Predicate predicate) const;

    // вызывается, когда в функцию второй параметр передается конкретный статус документа
    std::vector<Document> FindTopDocuments(const std::string &raw_query, DocumentStatus doc_status) const;

    // вызывется, когда в метод передается один параметр
    std::vector<Document> FindTopDocuments(const std::string &raw_query) const;

    std::size_t GetDocumentCount() const;

    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string &raw_query,
                                                        int document_id) const;

   private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    std::set<std::string> stop_words_;
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;
    std::map<int, DocumentData> documents_;
    std::vector<int> document_order_id_;

    bool IsStopWord(const std::string &word) const;

    std::vector<std::string> SplitIntoWordsNoStop(const std::string &text) const;

    static int ComputeAverageRating(const std::vector<int> &ratings);
    
      struct QueryWord {
        std::string data; 
        bool is_minus; 
        bool is_stop; };

    QueryWord ParseQueryWord(std::string text) const;

        struct Query {
        std::set<std::string> plus_words;
        std::set<std::string> minus_words;
    };

    std::optional<Query> ParseQuery(const std::string &text) const;

    // Existence required
    double ComputeWordInverseDocumentFreq(const std::string &word) const;

    template <typename Predicate>
    std::vector<Document> FindAllDocuments(const Query &query, Predicate predicate) const;

    static bool IsValidWord(const std::string &word);

    // Проверка на вхождение двойного минуса
    static bool DoubleMinusInWord(const std::string &query);

    // Проверка на null или пробелы после минуса
    static bool IsNullAfterMinus(std::string text);
};


// шаблонная функция, в параметр которой передается функция-предикат для динамической фильтрации
template <typename Predicate>
std::vector<Document> SearchServer::FindTopDocuments(const std::string &raw_query,
                                                     Predicate predicate) const {
    const auto query = ParseQuery(raw_query);
    std::vector<Document> result;
    if (query.has_value()) {
        result = FindAllDocuments(query.value(), predicate);  // struct Document {int id; double relevance; int rating;};;
        sort(result.begin(), result.end(),
             [](const Document &lhs, const Document &rhs) {
                 if (std::abs(lhs.relevance - rhs.relevance) < std::numeric_limits<double>::epsilon()) {
                     return lhs.rating > rhs.rating;
                 }
                 return lhs.relevance > rhs.relevance;
             });
        if (result.size() > MAX_RESULT_DOCUMENT_COUNT) {
            result.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
    }
    return result;
}

template <typename Predicate>
std::vector<Document> SearchServer::FindAllDocuments(const Query &query, Predicate predicate) const {
    std::map<int, double> document_to_relevance;
    for (const std::string &word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
        for (const auto [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            if (predicate(documents_.at(document_id).status))

            {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }

    for (const std::string &word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        for (const auto [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    std::vector<Document> matched_documents;
    for (const auto [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back(
            {document_id, relevance, documents_.at(document_id).rating});
    }
    return matched_documents;
}

