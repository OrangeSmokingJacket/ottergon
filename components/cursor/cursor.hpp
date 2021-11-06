#pragma once
#include <components/document/document_view.hpp>
#include <goblin-engineer/core.hpp>
#include <vector>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/unordered_set_hook.hpp>

namespace components::cursor {

    class data_cursor_t {
    public:
        data_cursor_t() = default;
        data_cursor_t(std::vector<components::document::document_view_t> data);
        std::size_t size() const;
    private:
        std::vector<document::document_view_t> data_;
    };

    class sub_cursor_t : public boost::intrusive::list_base_hook<> {
    public:
        sub_cursor_t() = default;
        sub_cursor_t(goblin_engineer::actor_address collection, data_cursor_t* data);
        goblin_engineer::actor_address& address();
        std::size_t size() const;

    private:
        goblin_engineer::actor_address collection_;
        std::vector<components::document::document_view_t>::const_iterator it;
        data_cursor_t* data_;
    };

    class cursor_t {
    public:
        cursor_t() = default;
        void push(sub_cursor_t* sub_cursor);
        std::size_t size() const;
        auto begin() -> std::list<std::unique_ptr<sub_cursor_t>>::iterator ;
        auto end() -> std::list<std::unique_ptr<sub_cursor_t>>::iterator ;
    private:
        uint64_t  size_{};
        std::list<sub_cursor_t>::const_iterator it;
        std::list<std::unique_ptr<sub_cursor_t>> sub_cursor_;
    };

} // namespace components::cursor