#ifndef SPOTSIGNATURE_H
#define SPOTSIGNATURE_H

#include <QByteArray>
#include <openssl/rsa.h>

class SpotSignature
{
public:
    SpotSignature(int numbits = 512);
    SpotSignature(const QByteArray &key);
    ~SpotSignature();

    QByteArray publicKey();
    QByteArray privateKey();
    QByteArray sign(const QByteArray &data);

    static bool verify(const QByteArray &modulus, const QByteArray &exponent, const QByteArray &data, QByteArray &signature);
    static bool verifyHeaderHashCash(const QByteArray &str);
    static QByteArray randomBytes(int num);
    static QByteArray crc32base64(const QByteArray &inbase64);

protected:
    RSA *_rsa;
    static bool _initialized;
//    void _init();
};

#endif // SPOTSIGNATURE_H
