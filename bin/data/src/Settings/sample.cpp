class Player {
public:
  void Do() {}
};

class Base {};

class Game : public Base {
public:
  Game() {
    Init();
  }

  void Init() {}

  void Run() {
    player.Do();
  }

private:
  Player player;
};

Game* game;

int main() {
  game = new Game();

  game->Run();

  delete game;
  return 0;
}
