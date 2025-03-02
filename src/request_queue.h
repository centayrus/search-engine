#pragma once
#include <cmath>
#include <cstdint>
#include <deque>
#include <vector>

#include "document.h"
#include "search_server.h"

class RequestQueue {
   public:
    explicit RequestQueue(const SearchServer &search_server);

    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename Predicate>
    std::vector<Document> AddFindRequest(const std::string &raw_query, Predicate lambda);

    std::vector<Document> AddFindRequest(const std::string &raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string &raw_query);

    int GetNoResultRequests() const;

   private:
    struct QueryResult {
        int time;
        bool empty_yn;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer &search_server_;
    int empty_result_reqs_;

    void EmptyResultReqsCounter(const std::uint64_t res_size);
};

template <typename Predicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string &raw_query, Predicate predicate) {
    const auto result = search_server_.FindTopDocuments(raw_query, predicate);
    EmptyResultReqsCounter(result.size());
    return result;
}