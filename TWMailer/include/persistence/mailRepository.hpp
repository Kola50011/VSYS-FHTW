#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

#include "libraries/spdlog/spdlog.h"
#include "libraries/json/json.hpp"
#include "libraries/uuid/uuid.h"
#include "libraries/plusaes/plusaes.hpp"

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

    bool doEncryption = false;
    std::vector<unsigned char> key;
    static constexpr unsigned char iv[16] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    };

    std::string encrypt(std::string decrypted) {
        std::vector<uint8_t> encrypted(plusaes::get_padded_encrypted_size(decrypted.size()));

        plusaes::encrypt_cbc((unsigned char *) decrypted.data(), decrypted.size(), &key[0], key.size(), &iv,
                             &encrypted[0], encrypted.size(), true);

        // Convert to savable string
        std::ostringstream oss;
        std::copy(encrypted.begin(), encrypted.end(), std::ostream_iterator<int>(oss, " "));
        return oss.str();
    }

    std::string decrypt(const std::string &encryptedString) {
        // Convert from saved string back to int array
        std::vector<uint8_t> encrypted;
        for (const auto &number :  stringUtils::split(encryptedString, " ")) {
            if (!number.empty() && number != "\n") {
                encrypted.push_back(std::stoi(number));
            }
        }

        unsigned long padded_size = 0;
        std::vector<unsigned char> decrypted(encryptedString.size());

        plusaes::decrypt_cbc(&encrypted[0], encrypted.size(), &key[0], key.size(), &iv, &decrypted[0], decrypted.size(),
                             &padded_size);

        return std::string(decrypted.begin(), decrypted.end());
    }

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
        // We are using an UUID to prevent id clashes
        auto uuid = uuid::generate_uuid_v4();
        mail.setId(uuid);

        std::stringstream jsonStringStream;
        nlohmann::json mailJson = mail;
        jsonStringStream << std::setw(4) << mailJson << std::endl;

        std::ofstream fileStream(getMailPath(uuid));

        if (doEncryption) {
            fileStream << encrypt(jsonStringStream.str()) << std::endl;
        } else {
            fileStream << jsonStringStream.str();
        }
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

    void setKey(const std::string &_key) {
        if (_key.size() != 16) {
            spdlog::error("Key must be exactly 16 characters long!");
            return;
        }
        char keyArray[17];
        std::strcpy(keyArray, _key.c_str());
        this->key = plusaes::key_from_string(&keyArray); // 16-char = 128-bit
        doEncryption = true;
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
            if (entry.is_directory()) { // Ignoring sent folder and other random folders in email directory
                continue;
            }
            try {
                auto mail = getMail(entry.path().stem());
                ret.push_back(mail);
            } catch (const FileNotFoundException& e) {
                spdlog::error("Tried to access non existing email at: " + entry.path().string());
            }
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
            if (entry.is_directory()) { // random folders in email directory
                continue;
            }
            try {
                auto mail = getMail(entry.path().stem());
                ret.push_back(mail);
            } catch (const FileNotFoundException& e) {
                spdlog::error("Tried to access non existing email at: " + entry.path().string());
            }
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

    bool mailExists(const std::string &id) {
        auto path = getMailPath(id);
        return std::filesystem::is_regular_file(path);
    }

    bool mailExistsForUser(const std::string &id, const std::string &user) {
        auto path = getUserMailPath(user, id);
        return std::filesystem::is_regular_file(path);
    }

    entities::Mail getMail(const std::string &id) {
        if (!mailExists(id)) {
            throw FileNotFoundException();
        }

        std::ifstream fileStream(getMailPath(id));
        std::stringstream fileTextStream;

        fileTextStream << fileStream.rdbuf();

        std::string fileText = fileTextStream.str();
        if (doEncryption) {
            fileText = decrypt(fileText);
        } else {
            if (fileText[0] != '{') {
                spdlog::error("Trying to decrypt encrypted message! Server will exit now!");
                std::quick_exit(1);
            }
        }
        spdlog::debug("Read message: " + fileText);

        auto mailJson = nlohmann::json::parse(fileText);
        auto ret = mailJson.get<entities::Mail>();
        return ret;
    }

    ~MailRepository() = default;
};