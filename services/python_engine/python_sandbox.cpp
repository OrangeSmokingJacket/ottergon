#include <rocketjoe/services/python_engine/python_sandbox.hpp>

#include <boost/filesystem.hpp>
#include <pybind11/stl.h>

#include <rocketjoe/http/http.hpp>

#include <goblin-engineer/dynamic.hpp>

namespace rocketjoe { namespace services { namespace python_engine {

    using namespace pybind11::literals;

    enum class transport_type : unsigned char {
                http = 0x00,
                ws = 0x01,
    };

    auto python_context::run() -> void {
        auto ext = boost::filesystem::extension(path_script);

        if(ext == ".py") {
            exuctor = std::make_unique<std::thread>(
                    [this]() {
                      auto locals = py::dict("path"_a=path_script,
                                             "pyrocketjoe"_a=pyrocketjoe);

                        py::exec(R"(
                           import sys, os
                           from importlib import import_module

                           sys.modules['pyrocketjoe'] = pyrocketjoe
                           sys.path.insert(0, os.path.dirname(path))

                           module_name, _ = os.path.splitext(path)

                           import_module(os.path.basename(module_name))
                        )", py::globals(), locals);
                    }
            );
        }
    }

    auto python_context::push_job(http::query_context &&job) -> void {
        device_.push(std::move(job));
    }

    python_context::python_context(
            goblin_engineer::dynamic_config &configuration,
            actor_zeta::actor::actor_address ptr) :
            python{}, pyrocketjoe{"pyrocketjoe"}, address(std::move(ptr)) {

        std::cerr << "processing env python start " << std::endl;

        path_script = configuration.as_object().at("app").as_string();

        std::cerr << "processing env python finish " << std::endl;

        /// read
        pyrocketjoe.def(
                "jobs_wait",
                [this](std::unordered_map<std::size_t, decltype(device_)::id_t> &jobs) -> std::size_t {
                    return device_.pop_all(jobs);
                }
        );

        /// read
        pyrocketjoe.def(
                "job_type",
                [this](std::size_t id) -> uint {
                    if (device_.in(id)) {
                        auto &transport = device_.get_first(id);
                        return uint(transport_type::http);
                    }

                }
        );

        /// write
        pyrocketjoe.def(
                "job_close",
                [this](std::size_t id) {
                    if (device_.in(id)) {
                        device_.release(id);
                    }
                }
        );

        /// read
        pyrocketjoe.def(
                "http_header_read",
                [this](std::size_t id, std::string &name) -> std::string {
                    if (device_.in(id)) {
                        auto &http = device_.get_first(id);
                        return http[name].to_string();
                    }
                }
        );

        /// write
        pyrocketjoe.def(
                "http_header",
                [this](std::size_t id, std::string &name, std::string &value) -> void {
                    if (device_.in(id)) {
                        auto &http = device_.get_second(id);
                        http.set(name,value);
                    }

                }
        );
        /// read
        pyrocketjoe.def(
                "http_uri",
                [this](std::size_t id) -> std::string {
                    if (device_.in(id)) {
                        auto &http = device_.get_first(id);
                        return http.target().to_string();
                    }
                }
        );

        /// write
        pyrocketjoe.def(
                "http_body_write",
                [this](std::size_t id, std::string &body_) {

                    if (device_.in(id)) {
                        auto &http = device_.get_second(id);
                        return http.body()=body_;
                    }

                }
        );

        /// read
        pyrocketjoe.def(
                "http_body_read",
                [this](std::size_t id) -> std::string {
                    if (device_.in(id)) {
                        auto &http = device_.get_first(id);
                        return http.body();
                    }
                }
        );

        /// read
        pyrocketjoe.def(
                "http_method",
                [this](std::size_t id) -> std::string {
                    if (device_.in(id)) {
                        auto &http = device_.get_first(id);
                        return http.method_string().to_string();
                    }
                }
        );

        /// read
        pyrocketjoe.def(
                "http_status",
                [this](std::size_t id) -> std::size_t {
                    if (device_.in(id)) {
                        auto &http = device_.get_second(id);
                        http.result(http::http::status::ok);
                    }
                }
        );

        /// read
        pyrocketjoe.def(
                "file_read",
                [this](const std::string &path) -> std::map<std::size_t, std::string> {
                    std::ifstream file(path);
                    std::map<std::size_t, std::string> file_content;

                    if (!file)
                        return file_content;

                    for (std::size_t line_number = 0; !file.eof(); line_number++) {
                        std::string line;

                        std::getline(file, line);
                        file_content[line_number] = line;
                    }

                    return file_content;
                }
        );
    }

}}}