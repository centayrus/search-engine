#include "search_server.h"

using namespace std;

// принимает string
SearchServer::SearchServer(const std::string &text) {
    if (!IsValidWord(text)) {
        throw std::invalid_argument("Строка содержит недопустимые символы");
    }
    for (const std::string &word : SplitIntoWords(text)) {
        stop_words_.insert(word);
    }
}
// Возвращаем номер документа по его порядковому номеру
int SearchServer::GetDocumentId(int index) const {
    int doc_id;
    try {
        doc_id = document_order_id_.at(index);
    } catch (...) {
        throw std::out_of_range("Документ c таким порядковым номером отсутствует");
    }
    return doc_id;
}

void SearchServer::AddDocument(int document_id, const std::string &document, DocumentStatus status,
                               const std::vector<int> &ratings) {
    // проверка на допустимость параметров функции
    if (document_id < 0 || documents_.count(document_id)) {
        throw std::invalid_argument("Ошибка при попытке добавления документа");
    }
    document_order_id_.push_back(document_id);

    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();
    for (const std::string &word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
}

// вызывается, когда в функцию второй параметр передается конкретный статус документа
std::vector<Document> SearchServer::FindTopDocuments(const std::string &raw_query, DocumentStatus doc_status) const {
    return FindTopDocuments(
        raw_query, [doc_status](DocumentStatus status) { return status == doc_status; });
}

// вызывется, когда в метод передается один параметр
std::vector<Document> SearchServer::FindTopDocuments(const std::string &raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

size_t SearchServer::GetDocumentCount() const {
    return documents_.size();
}

std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string &raw_query,
                                                                                 int document_id) const {
    // Объявление переменной снаружи оператора ветвления для доступности в return
    std::tuple<std::vector<std::string>, DocumentStatus> result;
    // Объявляем optional переменной внутри оператора
    if (const auto query = ParseQuery(raw_query); query.has_value()) {
        std::vector<std::string> matched_words;
        for (const std::string &word : query->plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const std::string &word : query->minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        result = {matched_words, documents_.at(document_id).status};
    }
    return result;
}

// private:
struct DocumentData {
    int rating;
    DocumentStatus status;
};

std::set<std::string> stop_words_;
std::map<std::string, std::map<int, double>> word_to_document_freqs_;
std::map<int, DocumentData> documents_;
std::vector<int> document_order_id_;

bool SearchServer::IsStopWord(const std::string &word) const {
    return stop_words_.count(word) > 0;
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string &text) const {
    std::vector<std::string> words;
    for (const std::string &word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Ошибка: в слове присутствуют недопустимые символы");
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int> &ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);

    return rating_sum / static_cast<int>(ratings.size());
}

// в задании не требовалось релизовать проверку,
SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    // Проверка допустимости входных параметров
    if (!IsValidWord(text) || DoubleMinusInWord(text) || !IsNullAfterMinus(text)) {
        throw std::invalid_argument("Невалидный текст запроса");
    }
    // проверка на допустимость параметров функции
    bool is_minus = false;
    // Word shouldn't be empty
    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }
    return {text, is_minus, IsStopWord(text)};
}

// здесь optional потому что такого было требование в задании, и я бы хотел оставить его
// optional, к тому же, привносит разнообразие
std::optional<SearchServer::Query> SearchServer::ParseQuery(const std::string &text) const {
    SearchServer::Query query;
    for (const std::string &word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            } else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

// Existence required
double SearchServer::ComputeWordInverseDocumentFreq(const std::string &word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}

bool SearchServer::IsValidWord(const std::string &word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) { return c >= '\0' && c < ' '; });
}
// Проверка на вхождение двойного минуса
bool SearchServer::DoubleMinusInWord(const std::string &query) {
    // проверка первых 2 символов
    if (query.substr(0, 2) == "--") {
        return true;
    }
    return false;
}
// Проверка на null или пробелы после минуса
bool SearchServer::IsNullAfterMinus(std::string text) {
    // Проверка последнего символа
    if (text[text.size() - 1] == '-') {
        return false;
    }
    return true;
}
