#ifndef OTTERGON_OTTERGON_H
#define OTTERGON_OTTERGON_H

#include <cstdint>
#include <cstdlib>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct string_view_t {
    const char* data;
    size_t size;
} string_view_t;

///enum level_t { trace, debug, info, warn, err, critical, off, level_nums };

struct config_log_t {
    string_view_t path;
    ////level_t level;
};

struct config_wal_t {
    string_view_t path;
    bool on;
    bool sync_to_disk;
};

struct config_disk_t {
    string_view_t path;
    bool on;
};

typedef struct config_t {
    config_log_t log;
    config_wal_t wal;
    config_disk_t disk;
} config_t;

typedef enum state_t {
    init,
    created,
    destroyed
} state_t;


typedef struct result_set_t {

} result_set_t;

typedef void* ottergon_ptr;

ottergon_ptr create(const config_t*);

void destroy(ottergon_ptr);

result_set_t* execute_sql(ottergon_ptr, string_view_t query);

#ifdef __cplusplus
}
#endif

#endif //OTTERGON_OTTERGON_H
