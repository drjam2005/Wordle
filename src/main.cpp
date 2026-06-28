#include <cstddef>
#include <math.h>
#include <vector>
#include <cstring>
#include <ctype.h>
#include "raylib.h"

#define WORD_LEN (5)
#define GUESS_COUNT (6)

struct Word {
	char word[WORD_LEN + 1];
	char guess[WORD_LEN + 1] = {0};
	std::vector<int> correctness;
	size_t index = 0;
	
	Word() {}

	void setWord(const char* word) {
		for(int i = 0; i < WORD_LEN; ++i){
			this->word[i] = toupper(word[i]);
		}
		this->word[WORD_LEN] = '\0';
	}

	void addChar(char givenChar){
		if(index >= WORD_LEN)
			return;
		char upperChar = toupper(givenChar);
		guess[index++] = upperChar;
	}

	void backspace(){
		if(index <= 0)
			return;
		guess[--index] = 0;
	}

	std::vector<int> checkCorrectness(){
		if(!this->correctness.empty()){
			return this->correctness;
		}

		std::vector<char> correctWord;
		for(int i = 0; i < WORD_LEN; ++i){
			correctWord.push_back(word[i]);
		}

		this->correctness.resize(WORD_LEN, 0);

		for(int i = 0; i < WORD_LEN; ++i){
			if(guess[i] == correctWord[i]){
				correctness[i] = 1;
				correctWord[i] = ' ';
			}
		}

		for(int i = 0; i < WORD_LEN; ++i){
			if(correctness[i] != 0) continue;
			for(int j = 0; j < WORD_LEN; ++j){
				if(guess[i] == correctWord[j]){
					correctness[i] = 2;
					correctWord[j] = ' ';
					break;
				}
			}
		}

		return correctness;
	}

	bool isCorrectGuess(){
		return (strcmp(word, guess) == 0);
	}
};

struct GameState {
	std::vector<Word> guesses;
	char word[WORD_LEN+1];
	size_t currentWord = 0;
	bool isDone = false;
	bool isWinner = false;

	GameState() {
		guesses.resize(GUESS_COUNT);
		setWord("tests");
	}
	void setWord(const char* givenWord){
		strcpy(this->word, givenWord);

		for(int i = 0; i < GUESS_COUNT; ++i){
			guesses[i].setWord(this->word);
		}
	}
};

class Updater {
	GameState& gameState;

public:
	Updater(GameState& gameState) : gameState(gameState) {}
	void Update(float dt) {
		if(gameState.isDone)
			return;

		Word& currentWord = gameState.guesses[gameState.currentWord];

		int currentChar = GetKeyPressed();

		if(IsKeyPressed(KEY_ENTER)){
			if(strlen(currentWord.guess) == WORD_LEN){
				currentWord.checkCorrectness();

				if(currentWord.isCorrectGuess()){
					gameState.isDone = true;
					gameState.isWinner = true;
					return;
				}

				if(gameState.currentWord >= GUESS_COUNT - 1){
					gameState.isDone = true;
					return;
				}

				gameState.currentWord++;
			}
		}else if(IsKeyPressed(KEY_BACKSPACE)){
			currentWord.backspace();
		}else if(currentChar){
			currentWord.addChar(currentChar);
		}
	}

};
class Renderer {
	GameState& gameState;

public:
	Renderer(GameState& gameState) : gameState(gameState) {}
	void Render(float dt) {
		int rowsToRender = gameState.currentWord + 1;

		// render the rectangles
		for(int i = 0; i < GUESS_COUNT; ++i){
			for(int j = 0; j < WORD_LEN; ++j){
				DrawRectangle(275-5+(50*j), 140-5+(50*i), 45, 45, WHITE);
			}
		}

		for(int i = 0; i < rowsToRender; ++i){
			bool shouldShowCorrectness =
				i < gameState.currentWord || gameState.isDone;

			if(shouldShowCorrectness && strlen(gameState.guesses[i].guess) == WORD_LEN){
				std::vector<int> correctness = gameState.guesses[i].checkCorrectness();

				for(int idx = 0; idx < correctness.size(); ++idx){
					size_t s = correctness[idx];

					if(s == 1){
						DrawRectangle(275-5 + (50*idx), 140-5+(50*i), 45, 45, GREEN);
					}else if(s == 2){
						DrawRectangle(275-5 + (50*idx), 140-5+(50*i), 45, 45, YELLOW);
					}else{
						DrawRectangle(275-5 + (50*idx), 140-5+(50*i), 45, 45, GRAY);
					}
				}
			}

			for(int ch = 0; ch < strlen(gameState.guesses[i].guess); ++ch){
				DrawText(
					TextFormat("%c", gameState.guesses[i].guess[ch]),
					275 + (50*ch), 140+(50*i), 45, i == gameState.currentWord && !gameState.isDone ? BLACK : WHITE
				);
			}
		}

		if(gameState.isDone && !gameState.isWinner){
			DrawText("You suck", 20, 20, 20, WHITE);
		}
	}

};

class Game {
	GameState gameState;
	Updater updater;
	Renderer renderer;

public:
	Game() : updater(gameState), renderer(gameState) {}

	void Loop(){
		float dt = GetFrameTime();

		updater.Update(dt);
		renderer.Render(dt);

	}

};

int main(){
	InitWindow(800, 600, "Wordle");
	SetTargetFPS(60);

	Game game;

	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(BLACK);

		game.Loop();

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
