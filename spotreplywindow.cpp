#include "spotreplywindow.h"
#include "ui_spotreplywindow.h"
#include <QDirIterator>
#include <QListWidgetItem>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include "hashcash.h"
#include "spotlite.h"
#include "spotsignature.h"
#include "spotnntp.h"

SpotReplyWindow::SpotReplyWindow(int spotid, SpotLite *sl, const QByteArray &msgid, const QString &title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpotReplyWindow),
    _spotid(spotid),
    _hc(NULL),
    _sl(sl),
    _parentmsgid(msgid),
    _newsgroup("free.usenet"),
    _subjectPrefix("Re: "),
    _title(title)
{
    ui->setupUi(this);

    QDirIterator iter(":/smileys", QStringList("*.gif"));
    while (iter.hasNext())
    {
        QListWidgetItem *item = new QListWidgetItem(QIcon(iter.next()), "", ui->smileys);
        item->setData(Qt::UserRole, iter.fileName().left(iter.fileName().length()-4) );
    }
    setWindowTitle( QString("%1 - %2").arg(windowTitle(), _title));

    //_parentmsgid.chop(8);
    QList<QByteArray> mparts = msgid.split('@');
    _parentmsgid = mparts[0];
    if (mparts.count() > 1)
        _parentdomain = mparts[1];
    else
        _parentdomain = "@free.pt";
}

SpotReplyWindow::~SpotReplyWindow()
{
    delete ui;
}

void SpotReplyWindow::on_smileys_itemClicked(QListWidgetItem* item)
{
    QString smiley = "[img="+item->data(Qt::UserRole).toString()+"] ";
    ui->textEdit->insertPlainText(smiley);
    ui->textEdit->setFocus();
}

void SpotReplyWindow::accept()
{
    int msglen = ui->textEdit->toPlainText().length();
    if (msglen < 3)
    {
        QMessageBox::critical(this, tr("Reactie te kort!"), tr("Vul min. 3 tekens in"));
        return;
    }
    else if (msglen > 900)
    {
        QMessageBox::critical(this, tr("Reactie te lang!"), tr("Houd het kort, schrijf geen boekdelen"));
        return;
    }

    ui->buttonBox->setEnabled(false);

    /*
    QSet<int> nrs_already_inuse = _sl->getCommentMsgIDnrs(_parentmsgid);
    //qDebug() << "Al in gebruik" << nrs_already_inuse;
    QList<int> nrs_beschikbaar;

    for (int i=1; i<100; i++)
    {
        if (!nrs_already_inuse.contains(i))
            nrs_beschikbaar.append(i);
    }

    //qDebug() << "Beschikbaar" << nrs_beschikbaar;
    int nr;

    if ( nrs_beschikbaar.isEmpty() )
        nr = nrs_already_inuse.count() + 10;
    else
    {
        nr = nrs_beschikbaar[int( qrand() / (RAND_MAX + 1.0) * (nrs_beschikbaar.count() - 1) + 0)];
    }

    _msgid = _parentmsgid.replace('.',"")+"."+QByteArray::number(nr)+"@"+_parentdomain;
    */


    QByteArray localpart = SpotSignature::randomBytes(9);
    localpart = localpart.toBase64();

    _msgid = _parentmsgid.replace('.',"")+".0."+localpart+"@"+_parentdomain;


    _hc = new HashCash(this);
    connect(_hc, SIGNAL(computed()), this, SLOT(onComputed()), Qt::QueuedConnection);
    _timer.start();
    _hc->calculate(_msgid);
    qDebug() << "Bezig met berekenen van Hashcash voor '"+_msgid+"'";
}

inline QByteArray _specialStr(QByteArray in)
{
    return in.replace("/", "-s").replace("+", "-p").replace("=", "");
}

inline QString toRFC822Date(const QDateTime &localTime)
{
   QLocale locale( QLocale::c() );

   return locale.toString( localTime.date(), "ddd, dd MMM yyyy" ) + ' ' +
          locale.toString( localTime.time(), "hh:mm:ss" );
}

void SpotReplyWindow::onComputed()
{
    qDebug() << "Hashcash berekend in" << _timer.elapsed()/1000.0 << "secs";

    QSettings *s = _sl->settings();

    QByteArray pubkey = s->value("pubkey").toByteArray();
    QByteArray taggedmsgid = "<"+_msgid+">";
    _from = s->value("nick").toByteArray();

    SpotSignature sig(s->value("privkey").toByteArray() );

    QByteArray _body = ui->textEdit->toPlainText().toLatin1().trimmed().replace("\r", "").replace("\n", "\r\n");

    _msg = "From: "+_from+" <"+_from.replace(" ", "").toLower()+"@spot.net>\r\n"
            "Subject: "+_subjectPrefix+_title.toLatin1()+"\r\n"
            "Newsgroups: "+_newsgroup+"\r\n"
            "Message-ID: "+taggedmsgid+"\r\n"
            "References: <"+_parentmsgid+"@"+_parentdomain+">\r\n"
            "X-User-Signature: "+_specialStr(sig.sign(taggedmsgid /*+_body+"\r\n"+_from*/ ).toBase64() )+"\r\n"
            "X-User-Key: "+pubkey+"\r\n"+
            _hc->header()+
            "X-No-Archive: Yes\r\n"
            "Content-Type: text/plain; charset=ISO-8859-1\r\n"
            "Content-Transfer-Encoding: 8bit\r\n"
            "Date: "+toRFC822Date(QDateTime::currentDateTimeUtc() ).toLatin1()+" GMT\r\n"
            "Lines: "+QByteArray::number(_body.count('\n')+1)+"\r\n\r\n"+_body.replace("\n.", "\n..")+"\r\n.\r\n";
    qDebug() << "Uitgaand berichtje:\n" << _msg;


    _nntp = _sl->nntp();
    seperateConnection = !_nntp->idle();


    QString server  = s->value("server").toString();
    bool ssl   = s->value("ssl").toBool();

    if ( server.startsWith("newsreader") && server.endsWith("weka.nl") && !ssl)
    {
        server = "post.eweka.nl";
        seperateConnection = true;
    }

    if (seperateConnection)
    {
        QByteArray user = s->value("user").toByteArray();
        QByteArray pass = QByteArray::fromBase64(s->value("pass").toByteArray() );
        int port   = s->value("port").toInt();
        _nntp = new SpotNNTP(server, port, user, pass, ssl, false, this);
        connect(_nntp, SIGNAL(error(int,QString,QByteArray)), this, SLOT(onError(int,QString)));
        connect(_nntp, SIGNAL(loggedIn()), this, SLOT(onLoggedIn()));
        connect(_nntp, SIGNAL(postingComplete()), this, SLOT(onPostingComplete()));
        _nntp->login();
    }
    else
    {
        connect(_nntp, SIGNAL(error(int,QString,QByteArray)), this, SLOT(onError(int,QString)));
        connect(_nntp, SIGNAL(postingComplete()), this, SLOT(onPostingComplete()));
        onLoggedIn();
    }
}

void SpotReplyWindow::onError(int, const QString &msg)
{
    QMessageBox::critical(this, tr("Fout bij posten van reactie!"), msg);

    if (seperateConnection)
        _nntp->logout_and_delete();

    _nntp = NULL;
    ui->buttonBox->setEnabled(true);
}

void SpotReplyWindow::onLoggedIn()
{
    _nntp->post(_msg);
}

void SpotReplyWindow::onPostingComplete()
{
    if (seperateConnection)
        _nntp->logout_and_delete();

    _nntp = NULL;
    emit newComment(_msg);
    close();
}
