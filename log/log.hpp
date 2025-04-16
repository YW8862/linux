#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sys/stat.h>

// 日志级别枚举
enum LogLevel 
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class AsyncLogger 
{
public:
    // 单例模式实现
    static AsyncLogger& instance() 
    {
        static AsyncLogger logger;
        return logger;
    }

    // 初始化日志目录和后台线程
    void init(const std::string& dir = "./logs") 
    {
        _log_dir = dir;
        //创建日志存放文件夹
        create_directory(_log_dir);

        _worker = std::thread(&AsyncLogger::worker_thread, this);
    }

    // 添加日志条目到队列（线程安全）
    void log(LogLevel level, const std::string& file, 
             int line, const std::string& msg) 
    {
        auto now = std::chrono::system_clock::now();
        {
            std::lock_guard<std::mutex> lock(_queue_mutex);
            _queue.emplace(LogEntry{now, std::this_thread::get_id(), 
                                  level, file, line, msg});
        }
        _cond.notify_one();  // 通知工作线程
    }

    ~AsyncLogger() {
        _stop = true;
        _cond.notify_all();   // 唤醒所有等待线程
        if(_worker.joinable()) _worker.join();  // 等待后台线程结束
        if(file_stream_.is_open()) file_stream_.close();
    }

private:
    // 日志条目结构体
    struct LogEntry {
        std::chrono::system_clock::time_point timestamp;
        std::thread::id tid;
        LogLevel level;
        std::string file;
        int line;
        std::string message;
    };

    AsyncLogger() = default;  // 单例构造私有化

    // 创建目录（兼容C++11）
    void create_directory(const std::string& path) 
    {
        std::string cmd = "mkdir -p " + path;

        //调用命令创建文件夹
        if(system(cmd.c_str()) != 0) 
        {
            std::cerr << "Failed to create log directory" << std::endl;
        }
    }
    
    // 后台工作线程处理函数
    void worker_thread() {
        while(true) {
            std::unique_lock<std::mutex> lock(_queue_mutex);
            // 等待条件变量（自动释放锁）
            _cond.wait(lock, [&]{ return !_queue.empty() || _stop; });

            if(_stop && _queue.empty()) break;  // 退出条件

            auto entry = _queue.front();
            _queue.pop();
            lock.unlock();  // 及时释放锁

            write_to_file(entry);  // 实际写入文件
        }
    }

    // 写入日志文件
    void write_to_file(const LogEntry& entry) {
        std::string filename = get_filename(entry.timestamp);
        if(current_file_ != filename) {  // 日期变化时切换文件
            if(file_stream_.is_open()) file_stream_.close();
            current_file_ = filename;
            file_stream_.open(current_file_, std::ios::app);
        }

        if(file_stream_.is_open()) {
            file_stream_ << format_entry(entry) << std::endl;
        }
    }

    // 生成日期格式文件名
    std::string get_filename(const std::chrono::system_clock::time_point& tp) {
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm tm = *std::localtime(&t);
        char buffer[32];
        strftime(buffer, sizeof(buffer), "%Y%m%d", &tm);
        return _log_dir + "/log_" + buffer + ".txt";
    }

    // 格式化日志条目
    std::string format_entry(const LogEntry& entry) {
        std::time_t t = std::chrono::system_clock::to_time_t(entry.timestamp);
        char time_buf[32];
        strftime(time_buf, sizeof(time_buf), "%F %T", std::localtime(&t));
        
        std::ostringstream oss;
        oss << "[" << time_buf << "] "
            << "[tid:" << entry.tid << "] "
            << level_to_string(entry.level) << " "
            << entry.file << ":" << entry.line << " - "
            << entry.message;
        return oss.str();
    }

    // 日志级别转字符串
    std::string level_to_string(LogLevel level) {
        switch(level) {
            case LogLevel::DEBUG:   return "DEBUG";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::WARNING: return "WARN";
            case LogLevel::ERROR:   return "ERROR";
            default:                return "UNKNOWN";
        }
    }

    // 成员变量
    std::string _log_dir;                  // 日志目录
    std::queue<LogEntry> _queue;           // 日志队列
    std::mutex _queue_mutex;               // 队列互斥锁
    std::condition_variable cond;         // 条件变量
    std::thread _worker;                   // 后台工作线程
    bool _stop = false;                    // 停止标志
    std::ofstream file_stream_;            // 文件输出流
    std::string current_file_;             // 当前日志文件路径
};

// 日志消息包装类（RAII管理）
class LogMessage {
public:
    LogMessage(LogLevel level, const char* file, int line) 
        : level_(level), file_(file), line_(line) {}

    ~LogMessage() {
        if(level_ >= min_level_) {  // 级别过滤
            AsyncLogger::instance().log(level_, file_, line_, stream_.str());
        }
    }

    // 流式输出接口
    std::ostringstream& stream() { return stream_; }

    // 设置全局最小日志级别
    static void set_min_level(LogLevel level) { min_level_ = level; }

private:
    LogLevel level_;
    std::string file_;
    int line_;
    std::ostringstream stream_;
    static LogLevel min_level_;  // 静态成员记录最低日志级别
};

LogLevel LogMessage::min_level_ = LogLevel::INFO;

// 日志宏定义（自动捕获文件行号）
#define LOG(LEVEL) \
    if(LEVEL < LogMessage::min_level_) ; \
    else LogMessage(LEVEL, __FILE__, __LINE__).stream()