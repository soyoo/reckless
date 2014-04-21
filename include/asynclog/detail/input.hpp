#ifndef ASYNCLOG_DETAIL_INPUT_HPP
#define ASYNCLOG_DETAIL_INPUT_HPP

#include "log_base.hpp"

namespace asynclog {
namespace detail {

class thread_input_buffer {
public:
    thread_input_buffer(detail::log_base* plog);
    ~thread_input_buffer();
    char* allocate_input_frame(std::size_t size);
    // returns pointer to following input frame
    char* discard_input_frame(std::size_t size);
    char* wraparound();
    char* input_start() const;
    char* input_end() const
    {
        return pinput_end_;
    }

private:
    static char* allocate_buffer();
    char* advance_frame_pointer(char* p, std::size_t distance);
    void wait_input_consumed();
    void signal_input_consumed();

    log_base* plog_;                    // Owner log instance
    spsc_event input_consumed_event_;

    char* const pbegin_;              // fixed value
    std::atomic<char*> pinput_start_; // moved forward by output thread, read by logger::write (to determine free space left)
    char* pinput_end_;                // moved forward by logger::write, never read by anyone else
    char* pcommit_end_;               // moved forward by commit(), read by wait_input_consumed (same thread so no race)
};

struct commit_extent {
    thread_input_buffer* pinput_buffer;
    char* pcommit_end;
};

}
}

#endif  // ASYNCLOG_DETAIL_INPUT_HPP
