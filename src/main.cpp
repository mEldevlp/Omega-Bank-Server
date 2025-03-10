#include "pch.h"

std::shared_ptr<spdlog::logger> Logger::logger;

void execute_query(PGconn* conn, const std::string& query) 
{
    PGresult* res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) 
    {
        std::cerr << "SQL error: " << PQerrorMessage(conn) << std::endl;
    }
    else 
    {
        std::cout << "Query executed successfully!\n";
    }

    PQclear(res);
}

int main(int argc, char** argv)
{
    std::system("cls");

    const char* conninfo = "host=localhost port=5432 dbname=bank user=postgres password=root";
    PGconn* conn = PQconnectdb(conninfo);

    Logger::init("Omega-Server");

    if (PQstatus(conn) != CONNECTION_OK) 
    {
		Logger::Error(std::string("Connection failed: ") + (PQerrorMessage(conn)));
        PQfinish(conn);
        return 1;
    }

    Logger::Info("Server is active!");
	Logger::Info("Connected to PostgreSQL on port 5432!");

    

    /*
    User user{ "Ivan", std::nullopt, "Petrov", "meldevlp", "12345"}; // Без отчества

    Card card1 {
        "Ivan's Main Card",
        "1234567812345678",
        "12/27",
        std::nullopt, // CVV не сохраняем
        "40817810099910001234",
        PaymentSystem::VISA,
        user,
        500000 // 5000.00 рублей (в копейках)
    };

    Card card2 {
    "Savings Card",
    "8765432187654321",
    "05/28",
    std::nullopt,
    "40817810123456789999",
    PaymentSystem::MASTERCARD,
    user,
    2500000 // 25000.00 рублей
    };

	std::vector<Card> cards;

	cards.push_back(card1);
	cards.push_back(card2);

	UserAccount test_user = UserAccount(std::move(user), Level::Default, cards);
    test_user.getBalance("1234567812345678");
    test_user.depositMoney("1234567812345678", 5000);
    test_user.getBalance("1234567812345678");
    */

    std::system("pause");
    PQfinish(conn);

	return 0;
}