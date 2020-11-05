#include <iostream>
#include <stdexcept>

using namespace std;



class Move{
	public:
		Move(int sh, int sc, int th, int tc);

		int getSource() const;
		int getSourceCoins() const;
		int getTarget() const;
		int getTargetCoins() const;

		friend ostream &operator << (ostream &out, const Move &move);

	private:
		int source_heap, source_coins, target_heap, target_coins;
};

class State{
	public:
		State(int h, const int c[]);
		~State();

		void next(const Move &move) throw(logic_error);
		bool winning() const;

		int getHeaps() const;
		int getCoins(int h) const throw(logic_error);

		friend ostream &operator << (ostream &out, const State &state);

	private:
		int heaps, *heap_coins;
};

Move::Move(int sh, int sc, int th, int tc): source_heap(sh), source_coins(sc), target_heap(th), target_coins(tc){}

int Move::getSource() const{
	return source_heap;
}

int Move::getSourceCoins() const{
	return source_coins;
}

int Move::getTarget() const{
	return target_heap;
}

int Move::getTargetCoins() const{
	return target_coins;
}

ostream &operator << (ostream &out, const Move &move){
	if (move.getTargetCoins() != 0){
		out << "takes " << move.getSourceCoins() << " coins from heap " << move.getSource() << " and puts "
			<< move.getTargetCoins() << " coins to heap " << move.getTarget();
	}
	else{
		out << "takes " << move.getSourceCoins() << " coins from heap " << move.getSource()
			<< " and puts nothing";
	}
	return out;
}



State::State(int h, const int c[]): heaps(h), heap_coins(new int[h]){
	for (int i = 0; i < heaps; ++i) {
		heap_coins[i] = c[i];
	}
}

State::~State(){
	delete[](heap_coins);
}

void State::next(const Move &move) throw(logic_error){
	if(move.getSourceCoins() > getCoins(move.getSource()) || move.getTargetCoins() >= move.getSourceCoins()){
		throw logic_error("insufficient coins");
	}
	else{
		getCoins(move.getTarget());
		heap_coins[move.getTarget()] += move.getTargetCoins();
		heap_coins[move.getSource()] -= move.getSourceCoins();
	}
}

bool State::winning() const{
	for (int i = 0; i < heaps; ++i){
		if (heap_coins[i] != 0){
			return false;
		}
	}
	return true;
}

int State::getHeaps() const{
	return heaps;
}

int State::getCoins(int h) const throw (logic_error){
	if (h < 0 || h >= heaps) {
		throw logic_error("invalid heaps");
	}
	else{
		return heap_coins[h];
	}
}

ostream &operator << (ostream &out, const State &state){
	for (int i = 0; i < state.heaps ; ++i) {
		if (i == state.heaps-1){
			out << state.heap_coins[i];
		}
		else{
			out << state.heap_coins[i] << ", ";
		}
	}
	return out;
}

class Player{
	public:
		Player (const string &n);
		virtual ~Player();

		virtual const string &getType() const = 0;
		virtual Move play(const State &s) = 0;

		friend ostream &operator << (ostream &out, const Player &player);

	protected:
		string name;
};

Player::Player(const string &n): name(n){};

Player::~Player(){
	name.clear();
}

ostream &operator << (ostream &out, const Player &player){
	out << player.getType() << " player " << player.name;
	return out;
}

class GreedyPlayer: public Player{
	public:
		GreedyPlayer(const string &n): Player(n), type("Greedy"){}
		~GreedyPlayer() override{
			type.clear();
			name.clear();
		}

		const string &getType() const override{
			return type;
		}
		Move play(const State &s) override{
			int max = 0;
			int heap = 0;
			for(int i = 0; i < s.getHeaps(); ++i) {
				if (s.getCoins(i) > max) {
					max = s.getCoins(i);
					heap = i;
				}
			}
			Move greedy_move(heap, max, 0, 0);
			return greedy_move;
		}
	private:
		string type;
};

class SpartanPlayer: public Player{
	public:
		SpartanPlayer(const string &n): Player(n), type("Spartan"){}
		~SpartanPlayer() override{
			type.clear();
			name.clear();
		}

		const string &getType() const override{
			return type;
		}
		Move play(const State &s) override{
			int max = 0;
			int heap = 0;
			for(int i = 0; i < s.getHeaps(); ++i) {
				if (s.getCoins(i) > max) {
					max = s.getCoins(i);
					heap = i;
				}
			}
			Move spartan_move(heap, 1, 0, 0);
			return spartan_move;
		}
	private:
		string type;
};

class SneakyPlayer: public Player{
	public:
		SneakyPlayer(const string &n): Player(n), type("Sneaky"){}
		~SneakyPlayer() override{
			type.clear();
			name.clear();
		}

		const string &getType() const override{
			return type;
		}
		Move play(const State &s) override{
			int heap = 0;
			while (s.getCoins(heap) == 0){
				heap++;
			}
			int min = s.getCoins(heap);
			for (int i = heap+1; i < s.getHeaps(); ++i) {
				if (s.getCoins(i) < min && s.getCoins(i) != 0) {
					min = s.getCoins(i);
					heap = i;
				}
			}
			Move sneaky_move(heap, min, 0, 0);
			return sneaky_move;
		}

	private:
		string type;
};

class RighteousPlayer: public Player {
	public:
		RighteousPlayer(const string &n): Player(n), type("Righteous") {}
		~RighteousPlayer() override {
			type.clear();
			name.clear();
		}

		const string &getType() const override {
			return type;
		}
		Move play(const State &s) override{
			int heap_min, heap_max, min, max;
			heap_min = heap_max = 0;
			min = max = s.getCoins(0);
			for (int i = 0; i < s.getHeaps(); ++i) {
				if (s.getCoins(i) < min) {
					min = s.getCoins(i);
					heap_min = i;
				}
				if (s.getCoins(i) > max){
					max = s.getCoins(i);
					heap_max = i;
				}
			}
			Move righteous_move(heap_max, (max+1)/2, heap_min, (max+1)/2-1);
			return righteous_move;
		}

	private:
		string type;
};

class Game {
	public:
		Game(int heaps, int players);
		~Game();

		void addHeap(int coins) throw(logic_error);
		void addPlayer(Player *player) throw(logic_error);
		void play(ostream &out) throw(logic_error);

	private:
		int heaps_this_game, players_this_game, heap_cnt, player_cnt, *heap_coins;
		Player **players_array;
};

Game::Game(int heaps, int players):heaps_this_game(heaps), players_this_game(players), heap_cnt(0), player_cnt(0),
	heap_coins(new int [heaps]), players_array(new Player *[players]) {}
	Game::~Game() {
		delete[](heap_coins);
		delete[](players_array);
	}

void Game::addHeap(int coins) throw(logic_error) {
	if (coins < 0) {
		throw logic_error("No point in inserting negative amount of coins...");
	}
	else if (heap_cnt == heaps_this_game) {
		throw logic_error("Number of heaps exceeded!");
	}
	else {
		heap_coins[heap_cnt++] = coins;
	}
}

void Game::addPlayer(Player *player) throw(logic_error) {
	if (player_cnt == players_this_game) {
		throw logic_error("Number of players exceeded!");
	}
	else {
		players_array[player_cnt++] = player;
	}
}

void Game::play(ostream &out) throw(logic_error) {
	if (heap_cnt != heaps_this_game) {
		throw logic_error("Invalid number of heaps!");
	}
	else if (player_cnt != players_this_game) {
		throw logic_error("Invalid number of players!");
	}
	else {
		State s(heaps_this_game, heap_coins);
		int win = 0;
		out << "State: " << s << endl;
		while (true) {
			for (int i = 0; i < players_this_game; ++i) {
				out << *players_array[i] << " " << players_array[i]->play(s) << endl;
				s.next(players_array[i]->play(s));
				out << "State: " << s << endl;
				win = s.winning();
				if (win) {
					out << *players_array[i] << " wins" << endl;
					break;
				}
			}
			if (win) {
				break;
			}
		}
	}
}

// Driver programm
int main(){

	Game specker(3, 4);
	specker.addHeap(10);
	specker.addHeap(20);
	specker.addHeap(17);
	specker.addPlayer(new SneakyPlayer("Tom"));
	specker.addPlayer(new SpartanPlayer("Mary"));
	specker.addPlayer(new GreedyPlayer("Alan"));
	specker.addPlayer(new RighteousPlayer("Robin"));
	specker.play(cout);

}
