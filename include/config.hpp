#pragma once

#include <string>

namespace viewer
{
    struct config
    {
        std::string java_path;
        std::string unluac_path;

      private:
        config();

      public:
        void save();

      public:
        static config &load();
    };
} // namespace viewer
