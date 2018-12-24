/*
 * Bank.cpp
 *
 *  Created on: Dec 12, 2018
 *      Author: nadav
 */

#include "Bank.h"

Bank::Bank(Log& log) : bankBalance(0), bankLog(log){
	pthread_mutex_init(&printLock, NULL);
}

Bank::~Bank() {
	pthread_mutex_destroy(&printLock);
}

void Bank::ChargeCommissions(){
	return;
}

void Bank::PrintStatus(){
	pthread_mutex_lock(&printLock);
	printf("\033[2J");		// Clear screen
	printf("\033[1;1H");	// Initialize cursor
	cout << "Current Bank Status" << endl;

	for(list<Account>::iterator it = accounts.begin(); it != accounts.end(); ++it){
		cout << "Account " << it->GetAccountNumber() << ": ";
		cout << "Balance - " << it->GetBalance(it->GetPassword(), false) << " $ , ";
		cout << "Account Password - " << it->GetPassword() << endl;
	}

	cout << "The Bank has " << bankBalance << " $" << endl;
	pthread_mutex_unlock(&printLock);
}

Account& Bank::GetAccount(int accountNumber){
	for(list<Account>::iterator it = accounts.begin(); it != accounts.end(); ++it){
		if(it->GetAccountNumber() == accountNumber){
			return (*it);
		}
	}
	Account *defaultAccount = new Account();
	return *defaultAccount;
}

bool Bank::IsAccountExist(int accountNumber){
	if(0 == accountNumber){		// atoi failed, given illegal number
		return false;
	}
	Account& accountFound = GetAccount(accountNumber);
	// return (-1 != accountFound.GetAccountNumber());
	if(-1 == accountFound.GetAccountNumber()){
		delete &accountFound;	// made a default account, must delete it
		return false;
	}
	return true;
}

bool IsLegalPassword(string password){
	if(password.length() != PASSWORD_LENGTH){
		return false;
	}

	for(int i = 0; i < PASSWORD_LENGTH; ++i){
		if(password[i] < '0' || password[i] > '9'){
			return false;
		}
	}
	return true;
}

Result Bank::CreateAccount(int accountNumber, string password, int initialBalance){
	if(0 == accountNumber){		// atoi failed, given illegal account number
		return ACCOUNT_DOESNT_EXIST;
	}
	if(IsAccountExist(accountNumber)){
		return ACCOUNT_ALREADY_EXISTS;
	}
	if(!IsLegalPassword(password)){
		return PASSWORD_FAIL;
	}
	if(initialBalance < 0 ){
		return INITIAL_BALANCE_FAIL;
	}
	// All parameters are OK, insert new account to list (and keep it sorted)
	Account newAccount(accountNumber, password, initialBalance);
	accounts.push_back(newAccount);
	accounts.sort();
	sleep(1);

	return SUCCESS;
}

Result Bank::MakeVip(int accountNumber, string password){
	if(!IsAccountExist(accountNumber)){
		return ACCOUNT_DOESNT_EXIST;
	}
	return GetAccount(accountNumber).MakeVIP(password);
}

Result Bank::Deposit(int accountNumber, string password, int amount){
	if(!IsAccountExist(accountNumber)){
		return ACCOUNT_DOESNT_EXIST;
	}
	return GetAccount(accountNumber).Deposit(password, amount);
}

Result Bank::Withdraw(int accountNumber, string password, int amount){
	if(!IsAccountExist(accountNumber)){
		return ACCOUNT_DOESNT_EXIST;
	}
	return GetAccount(accountNumber).Withdraw(password, amount);
}

Result Bank::GetBalance(int accountNumber, string password){
	if(!IsAccountExist(accountNumber)){
		return ACCOUNT_DOESNT_EXIST;
	}
	return (GetAccount(accountNumber).GetBalance(password) == -1 ? PASSWORD_FAIL : SUCCESS);
}

Result Bank::BankTransfer(int srcAccountNum, string password, int dstAccountNum, int amount){
	if(!IsAccountExist(srcAccountNum)){
		return ACCOUNT_DOESNT_EXIST;
	}
	if(!IsAccountExist(dstAccountNum)){
		return ACCOUNT_DOESNT_EXIST;
	}
	Account& srcAccount = GetAccount(srcAccountNum);
	Account& dstAccount = GetAccount(dstAccountNum);
	return Transfer(password, dstAccount, srcAccount, amount);

}
