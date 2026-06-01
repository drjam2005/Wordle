#include <cstddef>
#include <iostream>
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
	std::vector<Word> words;
	char word[WORD_LEN+1];
	size_t currentWord = 0;
	bool isDone = false;

	GameState() {
		words.resize(GUESS_COUNT);
		setWord("tests");
	}
	void setWord(const char* givenWord){
		strcpy(this->word, givenWord);

		for(int i = 0; i < GUESS_COUNT; ++i){
			words[i].setWord(this->word);
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

		Word& currentWord = gameState.words[gameState.currentWord];

		int currentChar = GetKeyPressed();

		if(IsKeyPressed(KEY_ENTER)){
			if(strlen(currentWord.guess) == WORD_LEN){
				currentWord.checkCorrectness();

				if(currentWord.isCorrectGuess()){
					gameState.isDone = true;
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

		for(int i = 0; i < rowsToRender; ++i){
			bool shouldShowCorrectness =
				i < gameState.currentWord || gameState.isDone;

			if(shouldShowCorrectness && strlen(gameState.words[i].guess) == WORD_LEN){
				std::vector<int> correctness = gameState.words[i].checkCorrectness();

				for(int idx = 0; idx < correctness.size(); ++idx){
					size_t s = correctness[idx];

					if(s == 1){
						DrawRectangle(275 + (50*idx), 140+(60*i), 50, 50, GREEN);
					}else if(s == 2){
						DrawRectangle(275 + (50*idx), 140+(60*i), 50, 50, YELLOW);
					}else{
						DrawRectangle(275 + (50*idx), 140+(60*i), 50, 50, GRAY);
					}
				}
			}

			for(int ch = 0; ch < strlen(gameState.words[i].guess); ++ch){
				DrawText(
					TextFormat("%c", gameState.words[i].guess[ch]),
					275 + (50*ch), 140+(60*i), 50, WHITE
				);
			}
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
