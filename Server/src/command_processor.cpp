#include "../inc/command_processor.h"

namespace Server
{
    namespace Processing
    {
        CommandProcessor::CommandProcessor()
        {
            RegisterCommandHandlers();
        }

        Interface::CommandResult CommandProcessor::ProcessCommand(const Interface::CommandID command_id, const std::vector<int8_t>& payload)
        {
            auto it = command_handlers.find(command_id);
            if (it != command_handlers.end())
            {
                return it->second(payload);
            }
            else
            {
                throw std::out_of_range("Received command_id is out of range");
            }
        }

        bool CommandProcessor::CommandIDSupported(const Interface::CommandID command_id) const
        {
            switch (command_id)
            {
                case Interface::CommandID::STRING_OUT:
                    return true;
                    break;
                case Interface::CommandID::DOUBLE_OUT:
                    return true;
                    break;
                default:
                    return false;
            }
        }

        void CommandProcessor::RegisterCommandHandlers()
        {
            command_handlers[Interface::CommandID::STRING_OUT] = [](const std::vector<int8_t>& payload)
            {
                std::string str(reinterpret_cast<const char*>(payload.data()), payload.size());

                return str;
            };

            command_handlers[Interface::CommandID::DOUBLE_OUT] = [](const std::vector<int8_t>& payload)
            {
                if(payload.size() > sizeof(uint64_t))
                {
                    throw std::out_of_range("Payload is insufficient to be read as sizeof(uint64_t)");
                }

                const uint64_t value = *reinterpret_cast<const uint64_t*>(payload.data());

                const double result = value / 1000.0;

                return result;
            };
        }
    }  // namespace Processing
} // namespace Server
