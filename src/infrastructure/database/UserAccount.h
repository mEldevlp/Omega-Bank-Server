#pragma once

#include <string>
#include <memory>
#include <vector>

enum class Level // each subsequent level owns the previous one, so we do bits move
{
	PRO = 1 << 0,
	PREMIUM = 1 << 1,
	ULTIMA = 1 << 2
};

enum class PaymentSystem
{
	VISA = 0,
	MASTERCARD,
	AMERICAN_EXPRESS,
	MIR
};

struct User
{
	std::string first_name;
	std::optional<std::string> middle_name; // Someone can have not middle name
	std::string last_name;
};

struct Card
{
	std::string name;	// can be changed but else not
	std::string_view card_number; // 16 digits
	std::string_view valid_thru;
	std::string_view CVV; // 3
	std::string_view account_number; // 20
	PaymentSystem payment_system;
	User user;
	double balance;

	Card(std::string name, std::string_view card_number, std::string_view valid_thru,
		std::string_view CVV, std::string_view account_number, PaymentSystem payment_system,
		User user, double balance);
};

class UserAccount
{
public:
	UserAccount() = delete;
	UserAccount(User&& user, Level user_level, std::vector<std::unique_ptr<Card>> cards) noexcept;

	~UserAccount() = default;
private:
	User user;
	Level user_level;
	std::vector<std::unique_ptr<Card>> cards;
};