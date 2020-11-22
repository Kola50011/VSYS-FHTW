#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include "libraries/spdlog/spdlog.h"
#include "libraries/json/json.hpp"
#include "libraries/uuid/uuid.h"

#include "exceptions/fileNotFoundException.hpp"
#include "persistence/entities/mail.hpp"

#define MAILS_FOLDER "/mails"
#define USERS_FOLDER "/users"
#define SENT_FOLDER "/sent"

class MailRepository {
private:
    MailRepository() = default;

    std::filesystem::path mailFolder;
    std::filesystem::path usersFolder;

    std::filesystem::path getMailPath(const std::string &id) {
        return mailFolder.string() + "/" + id + ".json";
    }

    std::filesystem::path getUserPath(const std::string &user) {
        return usersFolder.string() + "/" + user;
    }

    std::filesystem::path getUserSentPath(const std::string &user) {
        return getUserPath(user).string() + SENT_FOLDER;
    }

    std::filesystem::path getUserMailPath(const std::string &user, const std::string &id) {
        return getUserPath(user).string() + "/" + id + ".json";
    }

    std::filesystem::path getUserSentMailPath(const std::string &user, const std::string &id) {
        return getUserSentPath(user).string() + "/" + id + ".json";
    }

    std::string saveMail(entities::Mail &mail) {
        auto uuid = uuid::generate_uuid_v4();
        mail.setId(uuid);

        std::ofstream fileStream(getMailPath(uuid));
        nlohmann::json mailjson = mail;
        fileStream << std::setw(4) << mailjson << std::endl;
        return uuid;
    }

    void saveMailForSender(const std::string &id, const std::string &user) {
        auto source = getUserSentMailPath(user, id);
        auto target = getMailPath(id);

        std::filesystem::create_directories(source.parent_path());
        std::error_code error;
        std::filesystem::create_symlink(target, source, error);
    }

    void saveMailForReceiver(const std::string &id, const std::string &user) {
        auto source = getUserMailPath(user, id);
        auto target = getMailPath(id);

        std::filesystem::create_directories(source.parent_path());
        std::error_code error;
        std::filesystem::create_symlink(target, source, error);
    }

public:
    static MailRepository &instance() {
        static MailRepository INSTANCE;
        return INSTANCE;
    }

    void setStorageFolder(const std::filesystem::path &storageFolder) {
        mailFolder = std::filesystem::absolute(storageFolder).string() + MAILS_FOLDER;
        std::filesystem::create_directories(mailFolder);

        usersFolder = std::filesystem::absolute(storageFolder).string() + USERS_FOLDER;
        std::filesystem::create_directories(usersFolder);
    }

    std::vector<entities::Mail> getMailsForUser(const std::string &name) {
        std::vector<entities::Mail> ret{};

        std::string userPath = getUserPath(name);
        if (!std::filesystem::is_directory(userPath)) {
            return ret;
        }
        for (const auto &entry : std::filesystem::directory_iterator(userPath)) {
            if (entry.is_directory()) {
                continue;
            }
            nlohmann::json mailJson;
            std::ifstream fileStream(entry.path());

            fileStream >> mailJson;
            ret.push_back(mailJson.get<entities::Mail>());
        }

        return ret;
    }

    std::vector<entities::Mail> getSentMailsForUser(const std::string &name) {
        std::vector<entities::Mail> ret{};

        std::string userSentPath = getUserSentPath(name);
        if (!std::filesystem::is_directory(userSentPath)) {
            return ret;
        }
        for (const auto &entry : std::filesystem::directory_iterator(userSentPath)) {
            if (entry.is_directory()) {
                continue;
            }
            nlohmann::json mailJson;
            std::ifstream fileStream(entry.path());

            fileStream >> mailJson;
            ret.push_back(mailJson.get<entities::Mail>());
        }

        return ret;
    }

    void addMail(entities::Mail &mail) {
        auto uuid = saveMail(mail);
        saveMailForSender(uuid, mail.getSender());
        for (const std::string &user : mail.getReceivers()) {
            saveMailForReceiver(uuid, user);
        }
    }

    void deleteMailForUser(const std::string &id, const std::string &user) {
        std::filesystem::remove(getUserMailPath(user, id));
    }

    bool mailExists(const std::string& id) {
        auto path = getMailPath(id);
        return std::filesystem::is_regular_file(path);
    }

    bool mailExistsForUser(const std::string &id, const std::string &user) {
        auto path = getUserMailPath(user, id);
        return std::filesystem::is_regular_file(path);
    }

    entities::Mail getMail(const std::string& id) {
        if (!mailExists(id)) {
            throw FileNotFoundException();
        }

        nlohmann::json mailJson;
        std::ifstream fileStream(getMailPath(id));
        fileStream >> mailJson;
        auto ret = mailJson.get<entities::Mail>();
        return ret;
    }

    ~MailRepository() = default;
};