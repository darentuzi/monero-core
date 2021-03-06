#include "Wallet.h"
#include "PendingTransaction.h"
#include "TransactionHistory.h"
#include "wallet/wallet2_api.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QUrl>
#include <QTimer>

namespace {


}

class WalletListenerImpl : public  Bitmonero::WalletListener
{
public:
    WalletListenerImpl(Wallet * w)
        : m_wallet(w)
    {

    }

    virtual void moneySpent(const std::string &txId, uint64_t amount)
    {
        // TODO
        Q_UNUSED(txId)
        Q_UNUSED(amount)
        qDebug() << __FUNCTION__;
    }

    virtual void moneyReceived(const std::string &txId, uint64_t amount)
    {
        // TODO
        Q_UNUSED(txId)
        Q_UNUSED(amount)
        qDebug() << __FUNCTION__;
    }

    virtual void updated()
    {
        qDebug() << __FUNCTION__;
        emit m_wallet->updated();
    }

    // called when wallet refreshed by background thread or explicitly
    virtual void refreshed()
    {
        qDebug() << __FUNCTION__;
        emit m_wallet->refreshed();
    }

private:
    Wallet * m_wallet;
};



QString Wallet::getSeed() const
{
    return QString::fromStdString(m_walletImpl->seed());
}

QString Wallet::getSeedLanguage() const
{
    return QString::fromStdString(m_walletImpl->getSeedLanguage());
}

void Wallet::setSeedLanguage(const QString &lang)
{
    m_walletImpl->setSeedLanguage(lang.toStdString());
}

Wallet::Status Wallet::status() const
{
    return static_cast<Status>(m_walletImpl->status());
}

bool Wallet::connected() const
{
    return m_walletImpl->connected();
}

QString Wallet::errorString() const
{
    return QString::fromStdString(m_walletImpl->errorString());
}

bool Wallet::setPassword(const QString &password)
{
    return m_walletImpl->setPassword(password.toStdString());
}

QString Wallet::address() const
{
    return QString::fromStdString(m_walletImpl->address());
}

bool Wallet::store(const QString &path)
{
    return m_walletImpl->store(path.toStdString());
}

bool Wallet::init(const QString &daemonAddress, quint64 upperTransactionLimit)
{
    return m_walletImpl->init(daemonAddress.toStdString(), upperTransactionLimit);
}

void Wallet::initAsync(const QString &daemonAddress, quint64 upperTransactionLimit)
{
    m_walletImpl->initAsync(daemonAddress.toStdString(), upperTransactionLimit);
}

bool Wallet::connectToDaemon()
{
    return m_walletImpl->connectToDaemon();
}

void Wallet::setTrustedDaemon(bool arg)
{
    m_walletImpl->setTrustedDaemon(arg);
}

quint64 Wallet::balance() const
{
    return m_walletImpl->balance();
}

quint64 Wallet::unlockedBalance() const
{
    return m_walletImpl->unlockedBalance();
}

bool Wallet::refresh()
{
    bool result = m_walletImpl->refresh();
    if (result)
        emit updated();
    return result;
}

void Wallet::refreshAsync()
{
    m_walletImpl->refreshAsync();
}

PendingTransaction *Wallet::createTransaction(const QString &dst_addr, const QString &payment_id,
                                              quint64 amount, quint32 mixin_count,
                                              PendingTransaction::Priority priority)
{
    Bitmonero::PendingTransaction * ptImpl = m_walletImpl->createTransaction(
                dst_addr.toStdString(), payment_id.toStdString(), amount, mixin_count,
                static_cast<Bitmonero::PendingTransaction::Priority>(priority));
    PendingTransaction * result = new PendingTransaction(ptImpl, this);
    return result;
}

void Wallet::disposeTransaction(PendingTransaction *t)
{
    m_walletImpl->disposeTransaction(t->m_pimpl);
    delete t;
}

TransactionHistory *Wallet::history()
{
    if (!m_history) {
        Bitmonero::TransactionHistory * impl = m_walletImpl->history();
        m_history = new TransactionHistory(impl, this);
    }
    return m_history;
}


QString Wallet::generatePaymentId() const
{
    return QString::fromStdString(Bitmonero::Wallet::genPaymentId());
}

QString Wallet::integratedAddress(const QString &paymentId) const
{
    return QString::fromStdString(m_walletImpl->integratedAddress(paymentId.toStdString()));
}

QString Wallet::paymentId() const
{
    return m_paymentId;
}

void Wallet::setPaymentId(const QString &paymentId)
{
    m_paymentId = paymentId;
}


Wallet::Wallet(Bitmonero::Wallet *w, QObject *parent)
    : QObject(parent), m_walletImpl(w), m_history(nullptr)
{
    m_walletImpl->setListener(new WalletListenerImpl(this));
}

Wallet::~Wallet()
{
    Bitmonero::WalletManagerFactory::getWalletManager()->closeWallet(m_walletImpl);
}
