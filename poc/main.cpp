#include <iostream>
using namespace std;

using pubkey_t = uint32_t;

struct wallet_t {
   wallet_t(): balance(next_balance++) {
   }
   int balance;

   static int next_balance{0};
};

pubkey_t me{3};

//CHERI: pubkey i
  * cannot change others capabilities
  * cannot change its own capability
  * cannot access superior or sibling capabilities


struct wallets_t: unordered_map<pubkey_t, wallet_t*> {
  wallets_t() { //runs before main (super-capability)
    for (int i = 0; i < 10; ++i) {
      wallet_t* w = new wallet_t();
      //CHERI: tell CPU the pointer w can only be accessed by pubkey i or superior capability
      emplace(i, w);
      cout << "SUPERCAP> new user pubkey " << i << " balance " << w->balance << '\n';
    }
  }
  
};

wallets_t wallets;

wallet_t* lookup_wallet(pubkey_t& user) {
  auto i = wallets.find(user);
  if (i == wallets.end) {
    return nullptr;
  }
  return i->second;
}

int get_balance() {
  wallet_t* w = lookup_wallet(me);
  if (w == nullptr) {
    return 0;
  }
  return w->balance();
}



int main() { //explore wallet as given publickey
  me = atoi(argv[2]);

  //CHERI: tell CPU new permissions

  //...complex software

  cout << "I am public key: " << me << '\n';
  cout << "my balance is " << get_balance() << '\n';

  //...more complex software
  
  //malicious intention: I try to spy on other's wallets knowing that they are hosted here (in this process space)
  //software glich/code injection: I am able to disclose all wallets
  for (int n = 0, auto& i: wallets) {
    cout << "wallet " << ++n << ": balance " << i.second->balance << '\n';
  }
  return 0;
}
