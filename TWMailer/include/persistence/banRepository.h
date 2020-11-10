#pragma once

#include <experimental/filesystem>
#include <fstream>

#include "libraries/json/json.hpp"
#include "persistence/entities/banTableEntry.h"

#define BANS_FILE "/bans.json"

#define BAN_TRIES 3        // Banned on 3. attempt
#define BAN_TIME 30 * 60   // 30 Minutes
#define TRY_TIMEOUT 5 * 60 // 5 Minutes

class BanRepository
{
private:
    std::filesystem::path bansFile;
    std::map<std::string, entities::BanTableEntry> bans;

    BanRepository(){};

    int ageOfTimepoint(std::chrono::system_clock::time_point time)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(time - std::chrono::system_clock::now()).count();
    }

    void save()
    {
        std::ofstream fileStream(bansFile);
        nlohmann::json bansJson = bans;
        fileStream << std::setw(4) << bansJson << std::endl;
    }

    void load()
    {
        if (std::filesystem::is_regular_file(bansFile))
        {
            nlohmann::json bansJson;
            std::ifstream fileStream(bansFile);
            fileStream >> bansJson;
            bans = bansJson.get<std::map<std::string, entities::BanTableEntry>>();
        }
    }

public:
    static BanRepository &instance()
    {
        static BanRepository INSTANCE;
        return INSTANCE;
    }

    void setStorageFolder(std::filesystem::path storageFolder)
    {
        bansFile = std::filesystem::absolute(storageFolder).string() + BANS_FILE;
        std::filesystem::create_directories(storageFolder);
        load();
    }

    void addFailed(std::string ip)
    {
        if (bans.find(ip) == bans.end())
        {
            bans[ip] = entities::BanTableEntry();
        }
        spdlog::debug("SAVE");

        if (isBanned(ip))
        {
            return;
        }
        spdlog::debug("SAVE");

        entities::BanTableEntry &ban = bans[ip];
        if (ageOfTimepoint(ban.getLastInteraction()) > TRY_TIMEOUT)
        {
            ban.setTries(0);
        }
        ban.setTries(ban.getTries() + 1);
        ban.updateLastInteraction();
        spdlog::debug("SAVE");

        save();
    }

    bool isBanned(std::string ip)
    {
        if (bans.find(ip) == bans.end())
        {
            return false;
        }

        entities::BanTableEntry &ban = bans[ip];
        return ban.getTries() >= BAN_TRIES && ageOfTimepoint(ban.getLastInteraction()) < BAN_TIME;
    }
};