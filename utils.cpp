#include "utils.h"
#include <QString>
#include <random>

QString generateRandomSlug(QString fileName, QString extension)
{
    const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int length = 6;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, charset.length() - 1);

    QString result;
    for (int i = 0; i < length; ++i) {
        result += charset[distr(gen)];
    }
    return fileName + "-" + result + "." + extension;
}
