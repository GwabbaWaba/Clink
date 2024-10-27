#pragma once
#include <string>

using std::string;

namespace clink_exceptions {
    #define EXCEPTION_TYPE_BRANCHES \
        X(invalid_access)
    
    #define X(name) name,
    enum EventExceptionType {
        EXCEPTION_TYPE_BRANCHES
    };
    #undef X

    class EventException : public std::exception {
        private:
        #define X(name) #name,
        static constexpr const char* strExceptionType[] = {
            EXCEPTION_TYPE_BRANCHES
        };
        #undef X

        const string message;

        public:
        EventException(EventExceptionType type, string message):
            message(string(strExceptionType[type]) + message)
        {}
        const char* what() {
            return this->message.c_str();
        }

    };

    #undef EXCEPTION_TYPE_BRANCHES
}
