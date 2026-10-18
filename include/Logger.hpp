namespace Ports {

    struct Logger {
        virtual ~Logger() = default;
        virtual void log(const std::string& message) = 0;
    };

}  // namespace Ports