#pragma once

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>

class QTableView;
class QPushButton;
class QLabel;
class QAction;
class QTableWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onRefresh();
    void onAccountSelectionChanged(const QModelIndex& current, const QModelIndex& previous);
    void onTransfer();
    void onLogout();
    void onAdminPanel();

private:
    void setupUi();
    void loadAccountsModel();
    void loadTransactionsForAccount(const QString& accountId);
    void applyRolePermissions();

    QSqlDatabase db;
    QSqlTableModel* accountsModel = nullptr;
    QTableView* accountsView = nullptr;
    QSqlQueryModel* transactionsModel = nullptr;
    QTableView* transactionsView = nullptr;

    QTableWidget* balanceTable = nullptr;       
    QTableWidget* cardBalancesTable = nullptr;  
    QTableWidget* cardsTable = nullptr;         

    QPushButton* refreshButton = nullptr;
    QPushButton* transferButton = nullptr;
    QPushButton* adminButton = nullptr;
    QLabel* statusLabel = nullptr;
    QAction* logoutAction = nullptr;
};