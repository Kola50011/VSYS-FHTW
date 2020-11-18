#pragma once

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ldap.h>
#include "libraries/spdlog/spdlog.h"

#define LDAP_URI "ldap://ldap.technikum-wien.at:389"
#define SEARCHBASE "dc=technikum-wien,dc=at"
#define SCOPE LDAP_SCOPE_SUBTREE
#define FILTER "(uid=if17b*)"
#define BIND_USER ""
#define BIND_PW ""

bool checkUserAndPassword(std::string user, std::string password) {

}

void test() {
    LDAP *ldap;
    LDAPMessage *result, *entry;
    BerElement *ber;
    char *attribute;
    BerValue **vals;

    BerValue *servercredp;
    BerValue cred;
    cred.bv_val = (char *) BIND_PW;
    cred.bv_len = strlen(BIND_PW);
    int rc = 0;

    int ldapversion = LDAP_VERSION3;

    if (ldap_initialize(&ldap, LDAP_URI) != LDAP_SUCCESS) {
        spdlog::error("ldap_init failed");
        return;
    }

    spdlog::info("connected to LDAP server  {}", LDAP_URI);

    if ((rc = ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &ldapversion)) != LDAP_SUCCESS) {
        spdlog::error("ldap_set_option(PROTOCOL_VERSION): {}", ldap_err2string(rc));
        ldap_unbind_ext_s(ldap, NULL, NULL);
        return;
    }

    if ((rc = ldap_start_tls_s(ldap, NULL, NULL)) != LDAP_SUCCESS) {
        spdlog::error("ldap_start_tls_s():: {}", ldap_err2string(rc));
        ldap_unbind_ext_s(ldap, NULL, NULL);
        return;
    }

    /* anonymous bind */
    rc = ldap_sasl_bind_s(ldap, BIND_USER, LDAP_SASL_SIMPLE, &cred, NULL, NULL, &servercredp);

    if (rc != LDAP_SUCCESS) {
        spdlog::error("LDAP bind error: {}", ldap_err2string(rc));
        ldap_unbind_ext_s(ldap, NULL, NULL);
        return;
    } else {
        spdlog::info("bind successful");
    }

    /* free memory used for result */
    ldap_msgfree(result);
    printf("LDAP search suceeded\n");

    ldap_unbind_ext_s(ldap, NULL, NULL);
    return;
}
