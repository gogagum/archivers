#include <ostream>
class LogStreamGet {
public:
    static std::ostream& getLogStream(const std::string& strParam);
private:
    static std::ostream nullStream_;  // NOLINT
};
