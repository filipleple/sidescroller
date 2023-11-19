#include <iostream>
#include <SDL2/SDL.h>
#include <bmp.h>
//#include <2dgame.h>


using namespace std;

int main(int, char**) {
    int numberOfTiles = 0; 
    int** mapa = mapParser(SCREEN_WIDTH, SCREEN_HEIGHT, "map.bmp", numberOfTiles);
    
    SDL_Window* window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    bool quit = false;
    SDL_Event e;

    SDL_Texture* bgTexture = IMG_toTexture(renderer, "clouds.bmp");
    SDL_Texture* brickTexture = IMG_toTexture(renderer, "brick.bmp");

    int collisionList[3];
    collisionList[0] = 0;
    collisionList[1] = 0;
    collisionList[2] = 0;
    int smashList[3] {0, 0, 0};   

    Walls walls(mapa, numberOfTiles, brickTexture);
    Camera camera;

    vector<Enemy> enemies = generateEnemies(walls);
    int maxEnemyCount  = enemies.size();

    Player player;
    SpriteSheet spritesheet(renderer);

    Bar healthbar(50, 50, 0);
    Bar scorebar(50, 100, 1);

    while(!quit && player.health > 0 && player.score < 0.1*maxEnemyCount && player.ypos < SCREEN_HEIGHT){
        
        while(SDL_PollEvent(&e) != 0){
            if( e.type == SDL_QUIT ){
                quit = true;
            }
            //camera.handleEvent(e);
            player.handleEvent(e);
        }
        
        //clear screen
        SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( renderer );
        SDL_SetRenderDrawColor( renderer, 0xEE, 0xEE, 0xFF, 0xFF );

        SDL_RenderCopy( renderer, bgTexture, NULL, NULL );

        
        if (collisionList[SIDE]){
            //player.xpos -= player.xvel;
            //player.xvel = 0;
            
        }

        detectCollision(player, walls, collisionList, camera);
        detectCollision(player, enemies, smashList, camera);
        
        player.move(collisionList); 

        walls.render(renderer, camera);
        player.render(renderer, camera, spritesheet);

        for (int i = 0; i < enemies.size(); i++){
            enemies[i].move(walls);
            enemies[i].render(renderer, camera);
        }

        healthbar.render(renderer, player.health);
        scorebar.render(renderer, player.score);

        camera.x = player.xpos;


        cout << "player at: " << player.xpos << ", " << player.ypos << endl;

        cout << "no of enemies: " << enemies.size() <<endl;

        SDL_Delay(5);
        //Update screen
        SDL_RenderPresent( renderer );

    }

    cout << "\n----------------" << endl;
    if(player.score >= 0.1*maxEnemyCount){
        cout << "YOU WIN" << endl;
    }
    else cout << "YOU LOSE" << endl;

    cout << "SCORE: " << player.score*10 << endl;

    cout << "----------------" << endl;

    return 0;

    


}
