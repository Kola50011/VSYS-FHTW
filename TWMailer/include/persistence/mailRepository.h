#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include "libraries/spdlog/spdlog.h"
#include "libraries/json/json.hpp"
#include "libraries/uuid/uuid.h"

#include "exceptions/fileNotFoundException.h"
#include "persistence/entities/mail.h"

#define MAILS_FOLDER "/mails"
#define USERS_FOLDER "/users"

class MailRepository
{
private:
    MailRepository(){};
    std::filesystem::path mailFolder;
    std::filesystem::path usersFolder;

    std::filesystem::path getMailPath(std::string id)
    {
        return mailFolder.string() + "/" + id + ".json";
    }

    std::filesystem::path getUserPath(std::string user)
    {
        return usersFolder.string() + "/" + user;
    }

    std::filesystem::path getUserMailPath(std::string user, std::string id)
    {
        return getUserPath(user).string() + "/" + id + ".json";
    }

    std::string saveMail(entities::Mail &mail)
    {
        auto uuid = uuid::generate_uuid_v4();
        mail.setId(uuid);

        std::ofstream fileStream(getMailPath(uuid));
        nlohmann::json mailjson = mail;
        fileStream << std::setw(4) << mailjson << std::endl;
        return uuid;
    }

    void saveMailForUser(std::string id, std::string user)
    {
        auto source = getUserMailPath(user, id);
        auto target = getMailPath(id);

        std::filesystem::create_directories(source.parent_path());
        std::error_code error;
        std::filesystem::create_symlink(target, source, error);
    }

public:
    static MailRepository &instance()
    {
        static MailRepository INSTANCE;
        return INSTANCE;
    }

    void setStorageFolder(std::filesystem::path storageFolder)
    {
        mailFolder = std::filesystem::absolute(storageFolder).string() + MAILS_FOLDER;
        std::filesystem::create_directories(mailFolder);

        usersFolder = std::filesystem::absolute(storageFolder).string() + USERS_FOLDER;
        std::filesystem::create_directories(usersFolder);
    }

    std::vector<entities::Mail> getMailsForUser(std::string name)
    {
        std::vector<entities::Mail> ret{};

        std::string userPath = getUserPath(name);
        if (!std::filesystem::is_directory(userPath))
        {
            return ret;
        }
        for (const auto &entry : std::filesystem::directory_iterator(userPath))
        {
            nlohmann::json mailJson;
            std::ifstream fileStream(entry);

            fileStream >> mailJson;
            ret.push_back(mailJson.get<entities::Mail>());
        }

        return ret;
    }

    void addMail(entities::Mail &mail)
    {
        auto uuid = saveMail(mail);
        saveMailForUser(uuid, mail.getSender());
        for (std::string user : mail.getReceivers())
        {
            saveMailForUser(uuid, user);
        }
    }

    void deleteMailForUser(std::string id, std::string user)
    {
        std::filesystem::remove(getUserMailPath(user, id));
    }

    bool mailExists(std::string id)
    {
        auto path = getMailPath(id);
        return std::filesystem::is_regular_file(path);
    }

    bool mailExistsForUser(std::string id, std::string user)
    {
        auto path = getUserMailPath(user, id);
        return std::filesystem::is_regular_file(path);
    }

    entities::Mail getMail(std::string id)
    {
        if (!mailExists(id))
        {
            throw FileNotFoundException();
        }

        nlohmann::json mailJson;
        std::ifstream fileStream(getMailPath(id));
        fileStream >> mailJson;
        auto ret = mailJson.get<entities::Mail>();
        return ret;
    }

    ~MailRepository(){};
};