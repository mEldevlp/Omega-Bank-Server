#include "pch.h"

Card::Card(std::string name, std::string card_number, std::string valid_thru,
	std::optional<std::string> CVV, std::string account_number, PaymentSystem payment_system,
	User user, int64_t balance)
	: name(std::move(name)),
	card_number(std::move(card_number)),
	valid_thru(std::move(valid_thru)),
	CVV(std::move(CVV)),
	account_number(std::move(account_number)),
	payment_system(payment_system),
	user(std::move(user)),
	balance(balance) 
{
	Logger::Info("Card " + this->name + " created!");
	Logger::Info("Card number: " + this->card_number);
	Logger::Info("Valid thru: " + this->valid_thru);
	Logger::Info("Account number: " + this->account_number);
	Logger::Info("Payment system: " + std::to_string(static_cast<int>(this->payment_system)));
	Logger::Info("User: " + this->user.first_name + " " + this->user.last_name);
	Logger::Info("Balance: " + std::to_string(this->balance / 100.0));
}

UserAccount::UserAccount(User&& user, Level user_level, std::vector<Card> cards) noexcept
    : user(std::move(user)),
	user_level(user_level),
	cards(std::move(cards))
{
	Logger::Info("User " + this->user.first_name + " " + this->user.last_name + " created!");
	Logger::Info("User level: " + levelToString(this->user_level));
	Logger::Info("Cards count: " + std::to_string(this->cards.size()));
}

bool UserAccount::depositMoney(const std::string & card_number, int64_t amount)
{
    if (amount <= 0) 
    {
		Logger::Error("Invalid deposit amount!");
        return false;
    }

    for (auto& card : cards) 
    {
        if (card.card_number == card_number) 
        {
            card.addMoney(amount);
            logTransaction(card, amount);
            return true;
        }
    }

	Logger::Error("Card not found!");
    return false;
}

int64_t UserAccount::getBalance(const std::string& card_number) const
{
	for (const auto& card : cards)
	{
		if (card.card_number == card_number)
		{
			Logger::Info("Balance of card " + card.card_number + " is " + std::to_string(card.balance / 100.0));
			return card.balance;
		}
	}

	return -1LL;
}

void UserAccount::logTransaction(const Card& card, int64_t amount)
{
	Logger::Info("Deposited " + std::to_string(amount / 100.0) + " to card " + card.card_number);
}
