#ifndef GRUBARYBA_H
#define GRUBARYBA_H

#include <memory>

class Die;
class Human;

// Reprezentuje interfejs gry GrubaRyba.
// Konkretną implementacją jest MojaGrubaRyba.
// MojaGrubaRyba ma mieć konstruktor bezparametrowy.
class GrubaRyba {
public:
    virtual ~GrubaRyba() {}

    // Poziom gry komputera:
    // DUMB - kupuje co trzecie pole, na którym stanie i jest możliwe do kupienia;
    // SMARTASS - kupuje wszystkie pola, na których stanie i są możliwe do kupienia.
    enum class ComputerLevel {DUMB, SMARTASS};

    // Przekazuje prototypową kostkę do gry.
    // Jeżeli argumentem jest pusty wskaźnik, to nie wykonuje żadnej operacji (ale nie ma błędu).
    virtual void setDie(std::shared_ptr<Die> die) = 0;

    // Dodaje nowego gracza komputerowego, który gra na zdefiniowanym poziomie.
    // Nowy gracz komputerowy nazywa się Gracz<Numer>, gdzie <Numer> to numer pozycji,
    // na której został dodany (numeracja pozycji od 1).
    // Rzuca TooManyPlayersException, jeśli osiągnięto już maksymalną liczbę graczy.
    virtual void addComputerPlayer(ComputerLevel level) = 0;

    // Dodaje nowego gracza czlowieka.
    // Jeżeli argumentem jest pusty wskaźnik, to nie wykonuje żadnej operacji (ale nie ma błędu).
    // Rzuca TooManyPlayersException, jeśli osiągnięto już maksymalną liczbę graczy.
    virtual void addHumanPlayer(std::shared_ptr<Human> human) = 0;

    // Przeprowadza rozgrywkę co najwyżej podanej liczby rund (rozgrywka może
    // skończyć się wcześniej).
    // Jedna runda obejmuje po jednym ruchu każdego gracza.
    // Gracze ruszają się w kolejności, w której zostali dodani.
    // Na początku każdej rundy wypisywany jest numer rundy,
    // a na zakończenie informacje podsumowujące dla każdego gracza (format w przykładzie).
    // Rzuca NoDieException, jeśli nie przekazano kostki.
    // Rzuca TooFewPlayersException, jeśli liczba graczy nie pozwala na rozpoczęcie gry.
    virtual void play(unsigned int rounds) = 0;
};

// Reprezentuje interfejs kostki do gry.
// Konkretna implementacja zostanie dostarczona przy testach.
class Die {
public:
    virtual ~Die() {}

    // Zwraca wynik rzutu kostką.
    virtual unsigned short roll() const = 0;

    // Zwraca nową kostkę, która jest klonem danej kostki (wzorzec Prototype).
    // Może komuś się przyda.
    virtual std::shared_ptr<Die> clone() const = 0;
};

// Reprezentuje interfejs człowieka.
// Konkretna implementacja zostanie dostarczona przy testach.
// Obsługa zakupów w grze dla gracza człowieka powinna być następująca:
// (1) jeśli pole można kupić, to gracz człowiek jest odpytywany, czy chce kupić (wantBuy);
// (2) jeśli gracz człowiek chce kupić, ale nie ma wystarczających środków, to odpytywany jest
// o *wszystkie* posiadane pola, czy chce sprzedać (wantSell); sprzedawane są wszystkie, które
// gracz zadecyduje sprzedać;
// (3) jeśli gracz człowiek chce kupić i ma wystarczające środki, to kupowane jest dane pole.
class Human {
public:
    virtual ~Human() {}

    // Zwraca imię człowieka.
    virtual std::string const& getName() const = 0;

    // Zwraca true, jeśli człowiek chce kupić daną posiadłość.
    virtual bool wantBuy(std::string const& propertyName) = 0;

    // Zwraca true, jeśli człowiek chce sprzedać daną posiadłość.
    // Wywoływane w przypadku, gdy brakuje człowiekowi pieniędzy na zakup lub opłaty.
    virtual bool wantSell(std::string const& propertyName) = 0;

    // Klpnuje czlowieka.
    // Moze komus sie przyda.
    virtual std::shared_ptr<Human> clone() const = 0;
};

class NoDieException : public std::exception {
public:
    NoDieException() {}
    const char* what() const noexcept { return "No die set up to play a game"; }
};

class TooManyPlayersException : public std::exception {
    unsigned int max;
public:
    TooManyPlayersException(unsigned int max_): max(max_) {}
    const char* what() const noexcept { return "Max number of players exceeded"; }
    unsigned int getMax() const { return max; }
};

class TooFewPlayersException : public std::exception {
    unsigned int min;
public:
    TooFewPlayersException(unsigned int min_): min(min_) {}
    const char* what() const noexcept { return "Min number of players required"; }
    unsigned int getMin() const { return min; }
};

/* NASZE */

class MojaGrubaRyba : GrubaRyba {

public:
    void setDie(std::shared_ptr<Die> die);
    void addComputerPlayer(ComputerLevel level);
    void addHumanPlayer(std::shared_ptr<Human> human);
    void play(unsigned int rounds);
    bool isGameOn();
    int playerSteppedOnField(Player&, int pos);

private:
    std::shared_ptr<Die> die;
    std::vector<Player> players;
    Board board;
};

void MojaGrubaRyba::play(unsigned int rounds) {
    while (rounds--)
    {
        for(Player& player : players) {
            if (!isGameOn()) return;
            player.roll(die);
        }
    }
}

int MojaGrubaRyba::playerMoved(Player& player, int from, int howFar) {
    return board.playerSteppedOnField(player, from, howFar);
}

class Player {
public: 
    Player(int initialMoney, int initialFieldId);
    int pay(int money);
    void earn(int money);
    void roll(Die*);
    void moveForward(int);
    virtual bool wantBuy(const Property&) = 0;

protected:
    virtual bool wantSell(const Property&) = 0;
    int bankrupt();
    int cash;
    int pos;
    vector<Property*> properties;
};

class Property {
public:
    void takeOver(const Player&);
    void release();
    int getPrice();
    virtual int getCommission();
    const string& getName();
private:
    Player* owner;
    int price;
    string name;
};

class RealEstate : Property {
public:
    RealEstate(const string&, int);
    virtual int getCommission();
};

class PublicProperty : Property {
public:
    PublicProperty(const string&, int);
    virtual int getCommission();
};

class Field {
public:
    Field(const string& name);
    virtual void onStepOn(const Player&) = 0;
    virtual void onPassBy(const Player&) = 0;
    const string getName();
private:
    const Action& action;
    string name;
};

class DepositField : Field {
public:
    DepositField(const string& name, int depositFee);
    void onStepOn(const Player&);
    void onPassBy(const Player&);
private:
    const int depositFee;
    int cash;
};

class NoOpField {
public:
    NoOpField(const string& name);
    void onStepOn(const Player&) { }
    void onPassBy(const Player&) { }
};

class PropertyField {
public:
    PropertyField(const string& name, const Property& property);
    void onStepOn(const Player&) { }
    void onPassBy(const Player&) { }
private:
    const Property& property;
};

class RewardField {
public:
    RewardField(const string& name, int reward);
    void onStepOn(const Player&) { }
    void onPassBy(const Player&) { }
private:
    const int reward;
};

//teach me, master
class PunishmentField {
public:
    PunishmentField(const string& name, int fee);
    void onStepOn(const Player&) { }
    void onPassBy(const Player&) { }
private:
    const int fee;
};

class FieldFactory
{
public:
    virtual Vector<Field> createFields() const = 0;
};

class Board {
public:
    Board();
    ~Board();
    playerSteppedOnField(const Player& player, int from, int to);
private:
    Vector<std::shared_ptr<Field>> fields;
};

int Board::playerSteppedOnField(const Player& player, int from, int howFar) {
    int to = (from + howFar) % fields.size();
    for (int i = from + 1; i != to; i++) {
        fields[i]->onPassBy(player);
    }
    fields[to]->onStepOn(player);
    return to;
}

#endif