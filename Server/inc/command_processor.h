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

            /**
             * @brief Enum representing the command identifiers that can be processed.
             * STRING_OUT - command that outputs a string result.
             * DOUBLE_OUT - command that outputs a floating-point number.
             */
            enum class CommandID
            {
                STRING_OUT = 1,
                DOUBLE_OUT = 2,
            };

            class ICommandProcessor
            {
                public:
                virtual ~ICommandProcessor() = default;

                /**
                 * @brief Processes a command based on the command ID and payload data.
                 *
                 * @param command_id The identifier of the command to be processed.
                 * @param payload The data to be processed, encoded as a vector of int8_t.
                 * @return CommandResult The result of processing the command, which is variant, check CommandResult.
                 */
                virtual CommandResult ProcessCommand(const CommandID command_id, const std::vector<int8_t>& payload) = 0;
            };
        }

        class CommandProcessor : public Interface::ICommandProcessor
        {
            public:
            using CommandHandler = std::function<Interface::CommandResult(const std::vector<int8_t>&)>;

            CommandProcessor();

            CommandProcessor(const CommandProcessor&) = delete;
            CommandProcessor& operator=(const CommandProcessor&) = delete;
            CommandProcessor(CommandProcessor&&) = delete;
            CommandProcessor& operator=(CommandProcessor&&) = delete;

            Interface::CommandResult ProcessCommand(Interface::CommandID commandID, const std::vector<int8_t>& payload) override;

            private:
            /**
             * @brief Registers the command handlers for each command ID.
             * This allows the processor to associate command IDs with specific handlers.
             */
            void RegisterCommandHandlers();

            std::unordered_map<Interface::CommandID, CommandHandler> command_handlers;
        };
    }  // namespace Processing
} // namespace Server

#endif // SERVER_INC_COMMAND_PROCESSOR_H
