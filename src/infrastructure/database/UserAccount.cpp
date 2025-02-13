#include "pch.h"

Card::Card(std::string name, std::string_view card_number, std::string_view valid_thru,
	std::string_view CVV, std::string_view account_number, PaymentSystem payment_system,
	User user, double balance)
	: name(std::move(name)),
	card_number(std::move(card_number)),
	valid_thru(std::move(valid_thru)),
	CVV(std::move(CVV)),
	account_number(std::move(account_number)),
	payment_system(payment_system),
	user(std::move(user)),
	balance(balance) {
}

UserAccount::UserAccount(User&& user, Level user_level, std::vector<std::unique_ptr<Card>> cards) noexcept
    : user(std::move(user)),
	user_level(user_level),
	cards(std::move(cards)) {}