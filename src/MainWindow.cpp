#include "MainWindow.h"
#include "TransferDialog.h"
#include "AddClientDialog.h"
#include "AuthManager.h"
#include "DBManager.h"
#include "AddCardDialog.h"

#include <QtWidgets>
#include <QtSql>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
    loadAccountsModel();
    applyRolePermissions();
}

MainWindow::~MainWindow()
{
    if (db.isOpen()) db.close();
}

void MainWindow::setupUi()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    accountsView = new QTableView(this);
    transactionsView = new QTableView(this);

    balanceTable = new QTableWidget(this);
    balanceTable->setColumnCount(2);
    balanceTable->setHorizontalHeaderLabels({ tr("Currency"), tr("Balance") });
    balanceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    balanceTable->setSelectionMode(QAbstractItemView::NoSelection);

    cardBalancesTable = new QTableWidget(this);
    cardBalancesTable->setColumnCount(3);
    cardBalancesTable->setHorizontalHeaderLabels({ tr("Card ID"), tr("Last4"), tr("Balance") });
    cardBalancesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    cardsTable = new QTableWidget(this);
    cardsTable->setColumnCount(5);
    cardsTable->setHorizontalHeaderLabels({ tr("Card ID"), tr("Type"), tr("Last 4"), tr("Status"), tr("Expiry") });
    cardsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cardsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    refreshButton = new QPushButton(tr("Refresh"));
    transferButton = new QPushButton(tr("Transfer"));
    adminButton = new QPushButton(tr("Admin"));
    statusLabel = new QLabel(tr("Not connected"));

    QHBoxLayout* hTop = new QHBoxLayout;
    hTop->addWidget(refreshButton);
    hTop->addWidget(transferButton);
    hTop->addWidget(adminButton);
    hTop->addStretch();
    hTop->addWidget(statusLabel);

    QVBoxLayout* v = new QVBoxLayout(central);
    v->addLayout(hTop);
    v->addWidget(new QLabel(tr("Accounts:")));
    v->addWidget(accountsView, 3);

    QHBoxLayout* balanceCardsLayout = new QHBoxLayout;

    QGroupBox* balBox = new QGroupBox(tr("Balances (by currency)"));
    QVBoxLayout* balBoxLayout = new QVBoxLayout(balBox);
    balBoxLayout->addWidget(balanceTable);
    balanceCardsLayout->addWidget(balBox, 1);

    QGroupBox* cardBalBox = new QGroupBox(tr("Card Balances (by card)"));
    QVBoxLayout* cardBalBoxLayout = new QVBoxLayout(cardBalBox);
    cardBalBoxLayout->addWidget(cardBalancesTable);
    balanceCardsLayout->addWidget(cardBalBox, 1);

    QGroupBox* cardsBox = new QGroupBox(tr("Cards"));
    QVBoxLayout* cardsBoxLayout = new QVBoxLayout(cardsBox);
    cardsBoxLayout->addWidget(cardsTable);
    balanceCardsLayout->addWidget(cardsBox, 1);

    v->addLayout(balanceCardsLayout, 1);

    v->addWidget(new QLabel(tr("Transactions (selected account):")));
    v->addWidget(transactionsView, 2);

    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::onRefresh);
    connect(transferButton, &QPushButton::clicked, this, &MainWindow::onTransfer);
    connect(adminButton, &QPushButton::clicked, this, &MainWindow::onAdminPanel);

    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    logoutAction = new QAction(tr("Logout"), this);
    fileMenu->addAction(logoutAction);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::onLogout);

    setWindowTitle(tr("Omega Bank"));
    resize(1100, 900);
}

void MainWindow::loadAccountsModel()
{
    if (!QSqlDatabase::database().isOpen()) 
    {
        statusLabel->setText(tr("DB not connected"));
        return;
    }

    if (accountsModel) 
    {
        delete accountsModel;
        accountsModel = nullptr;
    }

    accountsModel = new QSqlTableModel(this, QSqlDatabase::database());
    accountsModel->setTable("accounts");
    accountsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    accountsModel->select();

    accountsModel->setHeaderData(accountsModel->fieldIndex("account_number"), Qt::Horizontal, tr("Account"));
    accountsModel->setHeaderData(accountsModel->fieldIndex("account_type"), Qt::Horizontal, tr("Type"));
    accountsModel->setHeaderData(accountsModel->fieldIndex("currency"), Qt::Horizontal, tr("Currency"));
    accountsModel->setHeaderData(accountsModel->fieldIndex("status"), Qt::Horizontal, tr("Status"));

    accountsView->setModel(accountsModel);
    accountsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    accountsView->setSelectionMode(QAbstractItemView::SingleSelection);
    accountsView->setColumnHidden(accountsModel->fieldIndex("id"), true);
    accountsView->resizeColumnsToContents();

    QItemSelectionModel* sel = accountsView->selectionModel();
    if (sel) 
    {
        connect(sel, &QItemSelectionModel::currentChanged, this, &MainWindow::onAccountSelectionChanged);
    }

    QString user = AuthManager::currentUser();
    QString role = AuthManager::currentRole();
    statusLabel->setText(tr("User: %1 (%2)").arg(user).arg(role));
}

void MainWindow::onRefresh()
{
    if (accountsModel) accountsModel->select();
    QModelIndex idx = accountsView->currentIndex();
    if (idx.isValid()) 
    {
        QString accountId = accountsModel->data(accountsModel->index(idx.row(), accountsModel->fieldIndex("id"))).toString();
        loadTransactionsForAccount(accountId);
    }
}

void MainWindow::onAccountSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous)
        if (!current.isValid()) return;
    QString accountId = accountsModel->data(accountsModel->index(current.row(), accountsModel->fieldIndex("id"))).toString();
    loadTransactionsForAccount(accountId);
}

void MainWindow::loadTransactionsForAccount(const QString& accountId)
{
    if (!QSqlDatabase::database().isOpen()) return;
    if (!transactionsModel) 
    {
        transactionsModel = new QSqlQueryModel(this);
        transactionsView->setModel(transactionsModel);
    }

    QSqlQuery q(QSqlDatabase::database());
    q.prepare("SELECT id, type, amount, currency, created_at, description FROM transactions WHERE account_id = :aid ORDER BY created_at DESC LIMIT 200");
    q.bindValue(":aid", accountId);
    if (!q.exec()) 
    {
        QMessageBox::warning(this, tr("Query failed"), q.lastError().text());
        transactionsModel->setQuery(QSqlQuery());
        return;
    }

    transactionsModel->setQuery(q);
    transactionsView->resizeColumnsToContents();

    QSqlQuery qb(QSqlDatabase::database());
    qb.prepare(
        "SELECT currency, COALESCE(SUM( CASE WHEN type IN ('deposit','transfer_credit','interest') THEN amount WHEN type IN ('withdrawal','transfer_debit','fee') THEN -amount ELSE 0 END ),0) AS balance"
        " FROM transactions WHERE account_id = :aid GROUP BY currency ORDER BY currency"
    );

    qb.bindValue(":aid", accountId);
    if (!qb.exec()) {
        qWarning() << "Balance query failed:" << qb.lastError().text();
    }
    else 
    {
        balanceTable->setRowCount(0);
        int row = 0;
        while (qb.next()) 
        {
            balanceTable->insertRow(row);
            balanceTable->setItem(row, 0, new QTableWidgetItem(qb.value(0).toString()));
            balanceTable->setItem(row, 1, new QTableWidgetItem(qb.value(1).toString()));
            row++;
        }
        balanceTable->resizeColumnsToContents();
    }

    QSqlQuery qc(QSqlDatabase::database());
    qc.prepare("SELECT id, card_type, last_four, status, expiry_date FROM cards WHERE account_id = :aid ORDER BY issued_at DESC");
    qc.bindValue(":aid", accountId);
    if (!qc.exec()) {
        qWarning() << "Cards query failed:" << qc.lastError().text();
    }
    else 
    {
        cardsTable->setRowCount(0);
        int row = 0;
        while (qc.next()) 
        {
            cardsTable->insertRow(row);
            cardsTable->setItem(row, 0, new QTableWidgetItem(qc.value(0).toString()));
            cardsTable->setItem(row, 1, new QTableWidgetItem(qc.value(1).toString()));
            cardsTable->setItem(row, 2, new QTableWidgetItem(qc.value(2).toString()));
            cardsTable->setItem(row, 3, new QTableWidgetItem(qc.value(3).toString()));
            cardsTable->setItem(row, 4, new QTableWidgetItem(qc.value(4).toString()));
            row++;
        }
        cardsTable->resizeColumnsToContents();
    }

    QSqlQuery qcb(QSqlDatabase::database());
    qcb.prepare(
        "SELECT c.id as card_id, c.last_four, COALESCE(SUM( CASE WHEN t.type IN ('deposit','transfer_credit','interest') THEN t.amount WHEN t.type IN ('withdrawal','transfer_debit','fee') THEN -t.amount ELSE 0 END ),0) as balance"
        " FROM transactions t"
        " JOIN cards c ON (c.id = (t.metadata->> 'card_id')::uuid)"
        " WHERE t.account_id = :aid AND t.metadata ? 'card_id'"
        " GROUP BY c.id, c.last_four"
        " ORDER BY c.id"
    );

    qcb.bindValue(":aid", accountId);
    if (!qcb.exec()) 
    {
        qWarning() << "Card balances query failed (maybe metadata not used):" << qcb.lastError().text();
        cardBalancesTable->setRowCount(0);
    }
    else 
    {
        cardBalancesTable->setRowCount(0);
        int row = 0;
        while (qcb.next()) 
        {
            cardBalancesTable->insertRow(row);
            cardBalancesTable->setItem(row, 0, new QTableWidgetItem(qcb.value(0).toString()));
            cardBalancesTable->setItem(row, 1, new QTableWidgetItem(qcb.value(1).toString()));
            cardBalancesTable->setItem(row, 2, new QTableWidgetItem(qcb.value(2).toString()));
            row++;
        }
        cardBalancesTable->resizeColumnsToContents();
    }
}

void MainWindow::onTransfer()
{
    QModelIndex idx = accountsView->currentIndex();
    if (!idx.isValid()) 
    {
        QMessageBox::information(this, tr("No account"), tr("Please select source account in the Accounts table."));
        return;
    }

    QString fromAccountId = accountsModel->data(accountsModel->index(idx.row(), accountsModel->fieldIndex("id"))).toString();
    QString fromAccountNumber = accountsModel->data(accountsModel->index(idx.row(), accountsModel->fieldIndex("account_number"))).toString();

    TransferDialog dlg(this);
    dlg.setFromAccountId(fromAccountId);
    dlg.setFromAccountNumber(fromAccountNumber);

    if (dlg.exec() == QDialog::Accepted) 
    {
        QString toAccNum = dlg.toAccountNumber();
        double amount = dlg.amount();
        QString currency = dlg.currency();
        QString performedBy = dlg.performedByEmployeeId();

        QSqlQuery q(QSqlDatabase::database());
        q.prepare("SELECT id FROM accounts WHERE account_number = :accnum LIMIT 1");
        q.bindValue(":accnum", toAccNum);
        if (!q.exec() || !q.next()) 
        {
            QMessageBox::warning(this, tr("Account not found"), tr("Destination account not found: %1").arg(toAccNum));
            return;
        }
        QString toAccountId = q.value(0).toString();

        QSqlQuery q2(QSqlDatabase::database());
        q2.prepare("SELECT transfer_funds(:from_acc::uuid, :to_acc::uuid, :amt::numeric, :curr::bpchar, :performed_by::uuid)");
        q2.bindValue(":from_acc", fromAccountId);
        q2.bindValue(":to_acc", toAccountId);
        q2.bindValue(":amt", amount);
        q2.bindValue(":curr", currency);
        if (!performedBy.isEmpty()) q2.bindValue(":performed_by", performedBy); else q2.bindValue(":performed_by", QVariant());

        if (!q2.exec()) 
        {
            QMessageBox::critical(this, tr("Transfer failed"), q2.lastError().text());
            return;
        }

        QMessageBox::information(this, tr("Transfer sent"), tr("Transfer executed (server returned result)."));
        onRefresh();
    }
}

void MainWindow::applyRolePermissions()
{
    QString role = AuthManager::currentRole();

    adminButton->setVisible(false);
    transferButton->setEnabled(false);

    if (role == "admin") 
    {
        adminButton->setVisible(true);
        transferButton->setEnabled(true);
    }
    else if (role == "teller") 
    {
        transferButton->setEnabled(true);
    }
    else if (role == "auditor") 
    {
        transferButton->setEnabled(false);
    }
}

void MainWindow::onAdminPanel()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Admin Panel"));
    QVBoxLayout* l = new QVBoxLayout(&dlg);

    QHBoxLayout* topRow = new QHBoxLayout;
    QPushButton* addClientBtn = new QPushButton(tr("Add Client"));
    QPushButton* addCardBtn = new QPushButton(tr("Add Card"));
    topRow->addWidget(addClientBtn);
    topRow->addWidget(addCardBtn);

    topRow->addStretch();
    l->addLayout(topRow);

    QSqlQueryModel* auditModel = new QSqlQueryModel(&dlg);
    auditModel->setQuery(QSqlQuery("SELECT performed_at, entity_type, action, details FROM audit_log ORDER BY performed_at DESC LIMIT 200", QSqlDatabase::database()));
    QTableView* auditView = new QTableView(&dlg);
    auditView->setModel(auditModel);
    auditView->resizeColumnsToContents();

    QSqlTableModel* empModel = new QSqlTableModel(&dlg, QSqlDatabase::database());
    empModel->setTable("employee_accounts");
    empModel->select();
    QTableView* empView = new QTableView(&dlg);
    empView->setModel(empModel);
    empView->setColumnHidden(empModel->fieldIndex("password_hash"), true);
    empView->setColumnHidden(empModel->fieldIndex("salt"), true);
    empView->resizeColumnsToContents();

    QSqlTableModel* custModel = new QSqlTableModel(&dlg, QSqlDatabase::database());
    custModel->setTable("customers");
    custModel->select();
    QTableView* custView = new QTableView(&dlg);
    custView->setModel(custModel);
    custView->resizeColumnsToContents();

    l->addWidget(new QLabel(tr("Audit Log (recent):")));
    l->addWidget(auditView, 1);
    l->addWidget(new QLabel(tr("Employee accounts:")));
    l->addWidget(empView, 1);
    l->addWidget(new QLabel(tr("Customers:")));
    l->addWidget(custView, 1);

    connect(addClientBtn, &QPushButton::clicked, this, [this, custModel]() 
    {
        AddClientDialog* addDlg = new AddClientDialog(this);

        connect(addDlg, &AddClientDialog::clientCreated, this,
            [this, custModel](const QString& custId, const QString& accId, const QString& accNum) 
            {
                custModel->select();

                if (accountsModel) 
                {
                    accountsModel->select();
                }

                if (accountsView && accountsModel) 
                {
                    int col = accountsModel->fieldIndex("account_number");
                    if (col >= 0) {
                        for (int r = 0; r < accountsModel->rowCount(); ++r) 
                        {
                            QModelIndex idx = accountsModel->index(r, col);
                            if (accountsModel->data(idx).toString() == accNum) 
                            {
                                accountsView->selectRow(r);
                                accountsView->scrollTo(idx);
                                break;
                            }
                        }
                    }
                }
            });

        addDlg->setAttribute(Qt::WA_DeleteOnClose);
        addDlg->exec();
    });


    connect(addCardBtn, &QPushButton::clicked, this, [this, custModel, custView]() {
        QModelIndex cur = custView->currentIndex();
        if (!cur.isValid()) 
        {
            QMessageBox::information(this, tr("Select customer"), tr("Please select a customer first."));
            return;
        }

        int idColumn = custModel->fieldIndex("id");
        if (idColumn < 0) 
        {
            QMessageBox::warning(this, tr("Schema error"), tr("customers table has no 'id' column."));
            return;
        }
        QString custId = custModel->data(custModel->index(cur.row(), idColumn)).toString();
        if (custId.isEmpty()) 
        {
            QMessageBox::warning(this, tr("Select customer"), tr("Could not determine selected customer id."));
            return;
        }

        QSqlQuery q(QSqlDatabase::database());
        q.prepare(R"(
        SELECT a.id, a.account_number
        FROM accounts a
        JOIN account_owners ao ON ao.account_id = a.id
        WHERE ao.customer_id = :cid
        ORDER BY a.account_number
    )");
        q.bindValue(":cid", custId);

        QList<QPair<QString, QString>> accounts;
        if (!q.exec()) 
        {
            QMessageBox::critical(this, tr("DB error"), tr("Failed to fetch accounts: %1").arg(q.lastError().text()));
            return;
        }
        while (q.next()) 
        {
            accounts.append({ q.value(0).toString(), q.value(1).toString() });
        }

        if (accounts.isEmpty()) 
        {
            QMessageBox::information(this, tr("No accounts"), tr("This customer has no accounts to attach a card to."));
            return;
        }

        AddCardDialog dlg(accounts, this);
        if (dlg.exec() == QDialog::Accepted) 
        {
            custModel->select();
        }
    });

    dlg.resize(1000, 700);
    dlg.exec();
}


void MainWindow::onLogout()
{
    AuthManager::logout();
    DBManager::closeConnection();
    qApp->exit(0);
}