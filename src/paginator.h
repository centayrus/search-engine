#pragma once

#include <cstdint>
#include <iostream>
#include <iterator>
#include <vector>

template <typename Iterator>
class IteratorRange {
   public:
    IteratorRange(Iterator it_begin, Iterator it_end)
        : begin_it(it_begin), end_it(it_end) {}

    Iterator begin() const {
        return begin_it;
    }
    Iterator end() const {
        return end_it;
    }
    size_t size() const {
        return distance(begin_it, end_it);
    }

   private:
    Iterator begin_it, end_it;
};

template <typename Iterator>
class Paginator {
   public:
    Paginator(Iterator page_begin, Iterator page_end, std::size_t p_size) {
       // Iterator new_iter;
        for (auto i = page_begin, new_iter = page_begin; i < page_end;) {
            std::size_t lengh = std::distance(i, page_end);
            lengh >= p_size ? std::advance(new_iter, p_size) : std::advance(new_iter, lengh);
            // Добавление элементов
            vector_pages_.push_back(IteratorRange(i, new_iter));
            i = new_iter;
        }
    }
    auto begin() const {
        return vector_pages_.begin();
    }
    auto end() const {
        return vector_pages_.end();
    }
    size_t size() const {
        return vector_pages_.size();
    }

   private:
    std::vector<IteratorRange<Iterator>> vector_pages_;
};

template <typename Iterator>
std::ostream &operator<<(std::ostream &output, const IteratorRange<Iterator> &res) {
    for (auto i = res.begin(); i != res.end(); ++i) {
        output << *i;
    }
    return output;
}

template <typename Container>
auto Paginate(const Container &c, std::size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}

