#ifndef LOG_H
#define LOG_H

class Log {
    public:
        explicit Log();
        virtual ~Log();

    public:
        static void print_log(const char *module, const int level, const char *format, ...);
        static void setDebugLevel(void);

    private:
        static int gLogLevel;
};

enum {
    LOG_DBG = 1<<0,
    LOG_INFO = 1<<1,
    LOG_ERR = 1<<2,
};

#define LogD(tag, format, args...) Log::print_log(tag, LOG_DBG, format, ##args)
#define LogI(tag, format, args...) Log::print_log(tag, LOG_INFO, format, ##args)
#define LogE(tag, format, args...) Log::print_log(tag, LOG_ERR, format, ##args)

#define FUNC_IN(tag, args...) Log::print_log(tag, LOG_DBG, "%s %d in", __func__, __LINE__, ##args);
#define FUNC_OUT(tag, args...) Log::print_log(tag, LOG_DBG, "%s %d out", __func__, __LINE__, ##args);
#define FUNC_PRINT(tag, args...) Log::print_log(tag, LOG_DBG, ##args);

#endif	// LOG_H