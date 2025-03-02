#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer &search_server)
    : search_server_(search_server), empty_result_reqs_(0) {
}
// сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики

std::vector<Document> RequestQueue::AddFindRequest(const std::string &raw_query, DocumentStatus status) {
    const auto result = search_server_.FindTopDocuments(raw_query, status);
    EmptyResultReqsCounter(result.size());
    return result;
}
std::vector<Document> RequestQueue::AddFindRequest(const std::string &raw_query) {
    const auto result = search_server_.FindTopDocuments(raw_query);
    EmptyResultReqsCounter(result.size());
    return result;
}
int RequestQueue::GetNoResultRequests() const {
    return empty_result_reqs_;
}

struct QueryResult {
    int time;
    bool empty_yn;
};

void RequestQueue::EmptyResultReqsCounter(const std::uint64_t res_size) {
    int last_timestamp = 0;
    if (requests_.size() >= min_in_day_) {
        requests_.pop_front();
        // empty_yn может быть как true, так и false, зависит от того, был ли непустой результат у запроса
        if (requests_.front().empty_yn) {
            --empty_result_reqs_;
        };
    }

    if (!requests_.empty()) {
        last_timestamp = requests_.back().time;
    }

    if (res_size == 0) {
        ++empty_result_reqs_;
        requests_.push_back({++last_timestamp, true});
    } else {
        requests_.push_back({++last_timestamp, false});
    }
}
