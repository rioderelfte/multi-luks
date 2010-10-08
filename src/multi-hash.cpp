#include <cstdlib>

#include <iostream>
#include <string>

#include <openssl/evp.h>

/* this string is required to convert the hash to its hex representation */
static const char *hexDigits = "0123456789abcdef";

int main(int argc, char *argv[])
{
    /* check the argument count */
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <algorithm> <count> <salt>" << std::endl;
        return 1;
    }

    /* get the command line arguments */
    std::string algorithm(argv[1]);
    std::string salt(argv[3]);
    int count(std::atoi(argv[2]));

    /* check if count is positive */
    if (count <= 0) {
        std::cerr << "Please give a positive count" << std::endl;
        return 2;
    }

    /* read the password from stdin */
    std::string password;
    getline(std::cin, password, static_cast<char>(0));

    /* load the standard OpenSSL digest algorithms */
    OpenSSL_add_all_digests();

    /* get the requested digest algorithm from OpenSSL */
    const EVP_MD *digest = EVP_get_digestbyname(algorithm.c_str());

    /* check if the algorithm exists */
    if (!digest) {
        std::cerr << "Unknown hash algorithm '" << algorithm << '"' << std::endl;
        return 3;
    }

    /* create a hashing context */
    EVP_MD_CTX *context = EVP_MD_CTX_create();

    if (!context) {
        std::cerr << "Internal error" << std::endl;
        return 4;
    }

    /* hash the password count times */
    for (int i = 0; i < count; ++i) {
        /* initialise a new hashing */
        if (EVP_DigestInit_ex(context, digest, 0) != 1) {
            std::cerr << "Internal error" << std::endl;
            return 5;
        }

        /* first hash the salt */
        if (EVP_DigestUpdate(context, salt.c_str(), salt.size()) != 1) {
            std::cerr << "Internal error" << std::endl;
            return 6;
        }

        /* next hash the password */
        if (EVP_DigestUpdate(context, password.c_str(), password.size()) != 1) {
            std::cerr << "Internal error" << std::endl;
            return 7;
        }

        /* retrieve the calculated hash sum */
        unsigned char buf[EVP_MAX_MD_SIZE];
        unsigned length;
        if (EVP_DigestFinal_ex(context, buf, &length) != 1) {
            std::cerr << "Internal error" << std::endl;
            return 8;
        }

        /* overwrite the data in the password variable so it can not be stored
         * in the ram after the program exited */
        for (int i = 0; i < password.length(); ++i)
            password[i] = 0;

        /* convert the hash value to a hex representation and store it in the
         * password variable */
        password.resize(length * 2);

        for (int j = 0; j < length; ++j) {
            password[j * 2]     = hexDigits[buf[j] / 16];
            password[j * 2 + 1] = hexDigits[buf[j] % 16];
        }
    }

    /* destroy the hashing context and unload the OpenSSL digest algorithms */
    EVP_MD_CTX_destroy(context);
    EVP_cleanup();

    /* print the calculated hash to stdout */
    std::cout << password << std::endl;

    /* overwrite the password variable so it can not be stored in the ram after
     * the program exited */
    for (int i = 0; i < password.length(); ++i)
        password[i] = 0;
}
