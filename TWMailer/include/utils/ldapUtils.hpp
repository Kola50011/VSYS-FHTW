#pragma once

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ldap.h>
#include "libraries/spdlog/spdlog.h"

#define LDAP_URI "ldap://ldap.technikum-wien.at:389"

namespace ldapUtils {
    bool checkUserAndPassword(std::string user, const std::string &password) {
        user = "uid=" + user + ",ou=People,dc=technikum-wien,dc=at";
        LDAP *ldap;

        // Initialize
        if (ldap_initialize(&ldap, LDAP_URI) != LDAP_SUCCESS) {
            spdlog::error("ldap_init failed");
            return false;
        }

        // Set version
        int ldapVersion = LDAP_VERSION3;
        int returnCode;
        if ((returnCode = ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &ldapVersion)) != LDAP_SUCCESS) {
            spdlog::error("ldap_set_option(PROTOCOL_VERSION): {}", ldap_err2string(returnCode));
            ldap_unbind_ext_s(ldap, nullptr, nullptr);
            return false;
        }

        // Initialize TLS
        if ((returnCode = ldap_start_tls_s(ldap, nullptr, nullptr)) != LDAP_SUCCESS) {
            spdlog::error("ldap_start_tls_s():: {}", ldap_err2string(returnCode));
            ldap_unbind_ext_s(ldap, nullptr, nullptr);
            return false;
        }

        // Bind
        BerValue credentials;
        credentials.bv_val = (char *) password.c_str();
        credentials.bv_len = strlen(password.c_str());

        returnCode = ldap_sasl_bind_s(ldap, user.c_str(), LDAP_SASL_SIMPLE, &credentials,
                                      nullptr, nullptr, nullptr);
        if (returnCode == LDAP_INVALID_CREDENTIALS) {
            spdlog::debug("User entered invalid credentials");
            ldap_unbind_ext_s(ldap, nullptr, nullptr);
            return false;
        }
        if (returnCode != LDAP_SUCCESS) {
            spdlog::error("ldap_sasl_bind_s(): {} {}", ldap_err2string(returnCode), returnCode);
            ldap_unbind_ext_s(ldap, nullptr, nullptr);
            return false;
        }

        // Connected successfully
        ldap_unbind_ext_s(ldap, nullptr, nullptr);
        return true;

    }
}
