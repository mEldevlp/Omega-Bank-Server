#pragma once

#include <string>
#include <memory>
#include <vector>

enum class Level : uint8_t // each subsequent level owns the previous one, so we do bits move
{
	Default = 0,
	PRO = 1 << 0,
	PREMIUM = 1 << 1,
	ULTIMA = 1 << 2
};

constexpr Level operator|(Level a, Level b) 
{
	return static_cast<Level>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr bool hasLevel(Level levels, Level level) 
{
	return (static_cast<uint8_t>(levels) & static_cast<uint8_t>(level)) != 0U;
}

constexpr std::string levelToString(Level level)
{
	switch (level)
	{
		case Level::Default:	return "Default";
		case Level::PRO:		return "PRO";
		case Level::PREMIUM:	return "PREMIUM";
		case Level::ULTIMA:		return "ULTIMA";
		default:				return "Unknown";
	}
}

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
	std::string login;
	std::string password;
};

struct Card
{
	std::string name;	// can be changed but else not
	std::string card_number; // 16 digits
	std::string valid_thru;
	std::optional<std::string> CVV; // 3
	std::string account_number; // 20
	PaymentSystem payment_system;
	User user;
	int64_t balance;

	Card(std::string name, std::string card_number, std::string valid_thru,
		std::optional<std::string> CVV, std::string account_number, PaymentSystem payment_system,
		User user, int64_t balance);

	void clearCVV() { CVV.reset(); } // Delete CVV after validation
	void addMoney(int64_t amount) { balance += amount; }
};

class UserAccount
{
public:
	UserAccount() = delete;
	UserAccount(User&& user, Level user_level, std::vector<Card> cards) noexcept;

	~UserAccount() = default;

	bool depositMoney(const std::string& card_number, int64_t amount);
	int64_t getBalance(const std::string& card_number) const;

private:
	User user;
	Level user_level;
	std::vector<Card> cards;

private:
	void logTransaction(const Card& card, int64_t amount);
};