#ifndef SERVER_INC_COMMAND_PROCESSOR_H
#define SERVER_INC_COMMAND_PROCESSOR_H

#include <functional>
#include <variant>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstddef>

namespace Server
{
    namespace Processing
    {
        namespace Interface
        {
            using CommandResult = std::variant<std::string, double>;

            enum class CommandID
            {
                STRING_OUT = 1,
                DOUBLE_OUT = 2,
            };

            class ICommandProcessor
            {
                public:
                virtual CommandResult ProcessCommand(const CommandID command_id, const std::vector<int8_t>& payload) = 0;
            };
        }

        class CommandProcessor : public Interface::ICommandProcessor
        {
            public:
            using CommandHandler = std::function<Interface::CommandResult(const std::vector<int8_t>&)>;

            CommandProcessor();

            Interface::CommandResult ProcessCommand(Interface::CommandID commandID, const std::vector<int8_t>& payload) override;

            private:
            void RegisterCommandHandlers();

            std::unordered_map<Interface::CommandID, CommandHandler> command_handlers;
        };
    }  // namespace Processing
} // namespace Server

#endif // SERVER_INC_COMMAND_PROCESSOR_H
