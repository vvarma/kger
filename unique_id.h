//
// Created by nvr on 21/7/19.
//

#ifndef KGER_UNIQUE_ID_H
#define KGER_UNIQUE_ID_H


#include <atomic>

class has_unique_id {
public:
    using uid = uint32_t ;

    uid unique_id() const;

    // do not copy or assign unique ids
    has_unique_id(const has_unique_id &);

    has_unique_id(has_unique_id &&) noexcept;

    has_unique_id &operator=(const has_unique_id &) noexcept;

protected:
    has_unique_id();

private:
    uid m_id;
    static std::atomic<uid> next;

};


#endif //KGER_UNIQUE_ID_H
