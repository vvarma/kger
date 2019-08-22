//
// Created by nvr on 21/7/19.
//

#include "unique_id.h"

std::atomic<has_unique_id::uid> has_unique_id::next{1000};

has_unique_id::uid has_unique_id::unique_id() const {
    return m_id;
}

has_unique_id::has_unique_id(const has_unique_id &) : m_id(++next) {

}

has_unique_id::has_unique_id(has_unique_id &&) noexcept : m_id(++next) {

}

has_unique_id &has_unique_id::operator=(const has_unique_id &) noexcept {
    return *this;
}

has_unique_id::has_unique_id() : m_id(++next) {

}

has_unique_id::~has_unique_id() = default;
