#include "raylib.h" //linking the external raylib library to our code

struct AnimData // declaring member variables for smarter and more efficient coding
{
    Rectangle rec; // rectangle for the image framing
    Vector2 pos; // vector frame for the positon
    int frame; // animation frame integer
    float updateTime; // update time float
    float runningTime; // running time float
};

// function for ground check
bool groundCheck(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height; // testing whether santa is on the ground by asking is santas vertical position more than the window bottom of the window (0 y position) deducated from santas heightreturning. This will return a boolean value, returning a true response if character is on the ground or false if in the air. This will increase efficiency of code later when I am adjusting the animation based on whether character is jumping or not
}

// function for game start screen
bool GameStarted = false; // defining a new boolean variable and initialising to zero, ready to use for a start screen option

// function for animation frame data update
AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime; // adding delta time variable to the running time each frame
    if (data.runningTime >= data.updateTime) // checking whether the running time is less than the update time member variable, if true then...
    {
        data.runningTime = 0.0; // if true, then reset running time back to zero
        // update animation frame
        data.rec.x = data.frame * data.rec.width; // setting the x value of the rectangle by assigning it the value of frame value multiplied by rectangle width 
        data.frame++; // update frame by incrementing by one
        if (data.frame > maxFrame) // checking whether we have hit the maximum number of frames on the sprite sheet (noting that we are using this same function with multiple different sized sprite sheets, we need to use the max frame variable which we will also define to enable greater flexibility and efficiently), and if so...
        {
            data.frame = 0; // if we have reached max number of frames on sprite sheet then we reset to zero and start cycle again
        }
    }
    return data; // return the output of the update anim data function which automates the increment and reset of the animation frame variables and cycle
}

int main() //main function body
{ 
    // array for window dimensions
    int windowDimensions[2]; // creating an array for window dimensions which are then easier to reference in the rest of code
        windowDimensions[0] = 512; // 1st array for window width of 512
        windowDimensions[1] = 380; // 2nd array for window height of 380

    // window dimensions
    InitWindow(windowDimensions[0], windowDimensions[1], "Santa Sprint"); // initialise window using constant values and exciting game name in header. Note that I have used the window dimension array variables here instead of entering fresh values again for efficiency
    // load audio player
    InitAudioDevice(); // intialise audio device so we can activate sounds and music

    // acceleration due to gravity (pixels per second per second)
    const int gravity{1'000}; // applying acceleration due to simulated impact of gravity using pixels per frame/ per frame

    // game audio
    Music music = LoadMusicStream("resources/music-comingtotown.mp3"); // loading background music file from resources folder
    PlayMusicStream(music);
    Sound soundJump = LoadSound("resources/sound-jump.mp3"); // loading jump sound file from resources folder
    Sound soundPass = LoadSound("resources/sound-pass.mp3"); // loading pass or win sound file from resources folder
    Sound soundFail = LoadSound("resources/sound-fail.mp3"); // loading fail or lose sound file from resources folder
    
    // santa character variables
        Texture2D santa = LoadTexture("resources/santa.png"); // initialising character by loading image in using the Raylib call function
        AnimData santaData; // creating new AnimData variable for character and initialising all its member variables ready for efficient reuse 
            santaData.rec.width = santa.width/6; // selecting only one of the 6 sprites on the sheet by dividing width by 6
            santaData.rec.height = santa.height; // using full height of sprite sheet
            santaData.rec.x = 0; // initialising character rectangle x position at zero to start
            santaData.rec.y = 0; // initialising character rectangle y position at zero to start
            santaData.pos.x = windowDimensions[0]/2 - santaData.rec.width/2; // centering my santa character's vertical position exactly in the centre noting that I need to take into account the width of the character for true center-ness 
            santaData.pos.y = windowDimensions[1] - santaData.rec.height; // placing the character on the ground but taking into account actual character height to ensure feet are on floor and not below
            santaData.frame = 0; // initalise at zero to support embedding in the character sprite sheet
            santaData.updateTime = 1.0/12.0; //setting an update time variable to 12th of second for the animation update time between frames leading to a gently santa movement. Note that this is constant and will not be changed across the program
            santaData.runningTime = 0.0; // declaring new running time veriable and initialising it to zero at start of sequence

    // hazard variables
    Texture2D coal = LoadTexture("resources/coal-sprite-sheet3.png"); // initialising hazards by loading image in using the Raylib call function

    const int hazardCount{10}; // constant variable for the amount of hazards that should be included, this will be used many times across the code in every for loop so important to set a variable to prevent unecessary repeat coding which is prone to error, especially by me!
    AnimData coals[hazardCount] {}; // creating array which recalls the amount of hazards defined in the above constant integer variable instead of seperate anim datas for efficiency, leaving this blank as I will define later within a for loop for efficiency

    for (int i = 0; i < hazardCount; i++) // setting up for loop for the hazards so I don't have to repeat code for each hazard then I will assign a starting value to each of the AnimData variables which can be reused without unecessary repeated coding. Note that here I have initialised the for loop with an integer at zero then set activation criteria that it has to be below 2, finally with the action of adding one to the i value if criteria is passed
    {
        // hazard positioning
        int verticalOffset = GetRandomValue(0.0, 1.5); // random values enable different heights of the hazards to be used in the later y position for more variety and complexity in game
        coals[i].rec.x = 0.0; // initialising rectangle vertical position
        coals[i].rec.y = 0.0; // initialising rectangle horizontal position
        coals[i].rec.width = coal.width/8; // rectangle width value noting the 8 value sprite sheet being used
        coals[i].rec.height = coal.height/8; // rectangle height value noting the 8 value sprite sheet being used
        coals[i].pos.x = windowDimensions[0] + (i*300);  // initialising x position for each hazard with a consistent gap of 300 between hazards
        coals[i].pos.y = windowDimensions[1] - coal.height/8 - verticalOffset * santaData.rec.height; // initialising y position value ready for the hazard to be floating off the ground. 
        coals[i].frame = 0; // initialising the frame at zero
        coals[i].runningTime = 0.0; // initialising the run time at zero
        coals[i].updateTime = 0.0; // initialising the update time at zero
    }

    // game finish line to pass for win
    float finishLine{coals[hazardCount - 1].pos.x}; // x position of an invisible finish line to activate completion of game, leading to a winning state for the player. The hazard array (coals) has been called where I have accessed the final element in the hazard array - identified by calling the hazardCount (total number of hazards) minus one (since we are starting at zero so I need to adjust the number being recalled for the actual finish line)

    // non consistent hazard variables outside the for loop
    coals[0].pos.x = windowDimensions[0]; // first hazard just off window
    coals[1].pos.x = windowDimensions[0] + 300; // phase 1 of hazards 300 pixels away from eachother
    coals[2].pos.x = windowDimensions[0] + 600; // phase 1 of hazards 300 pixels away from eachother
    coals[3].pos.x = windowDimensions[0] + 900; // phase 1 of hazards 300 pixels away from eachother
    coals[4].pos.x = windowDimensions[0] + 1100; // phase 2 of hazards 200 pixels away from eachother
    coals[5].pos.x = windowDimensions[0] + 1300; // phase 2 of hazards 200 pixels away from eachother
    coals[6].pos.x = windowDimensions[0] + 1500; // phase 2 of hazards 200 pixels away from eachother
    coals[7].pos.x = windowDimensions[0] + 1800; // phase 3 of hazards 150 pixels away from eachother
    coals[8].pos.x = windowDimensions[0] + 1950; // phase 3 of hazards 150 pixels away from eachother
    coals[9].pos.x = windowDimensions[0] + 2100; // phase 3 of hazards 150 pixels away from eachother

    // hazard x velocity (pixels/ second)
    int coalVel{-200}; // setting the hazard its own velocity, noting that this is initialised in the negative since it is moving backwards from right to left
        
    // air check
    bool isInAir = false; // checking if the rectangle is in the air and initialising at zero
   
    // jump velocity (pixels per second)
    const int jumpVel{-600}; // assigning a high negative value which can be added to every jump action at an increased power of the jump to so Santa can jump over the coals

    int velocity{0}; // enabling upwards movement by reducing the y coordinates

    // background textures
    Texture2D background = LoadTexture("resources/background-winter2x.png"); // loading in the third background ready for later animation
    float bgX{}; // initialising a x coordinate variable for the background to zero using a float since we are to define a Vector 2
    Texture2D midground = LoadTexture("resources/midground-moutains2x.png"); // loading in the second background ready for later animation
    float mgX{}; // initialising a x coordinate variable for the midground to zero using a float since we are to define a Vector 2
    Texture2D foreground = LoadTexture("resources/foreground-trees2x.png"); // loading in the third background first ready for later animation
    float fgX{}; // initialising a x coordinate variable for the foreground to zero using a float since we are to define a Vector 2
    Texture2D startground = LoadTexture("resources/start-screen.png"); // loading in the start screen background
    float ssX{}; // initialising a x coordinate variable for the foreground to zero using a float since we are to define a Vector 2

    bool collision = false; // creating boolian for a collision variable which will be later defined, this is initialised as false which enables a 1 life scenario (ie you lose if character collides with a single hazard)

    SetTargetFPS(60); //setting frames per second to 60 for efficiency
    while(!WindowShouldClose()) //allows user to exit upon escape button and end below while loop
        {	
            // delta time (time since last frame)
            const float dT = GetFrameTime(); // ensuring the program is frame rate interdependent as this can not be relied upon for reliability

            // start drawing
            BeginDrawing(); //start drawing command
            ClearBackground(WHITE); //set white background just as standard
            UpdateMusicStream(music); //update music buffer with new stream which will loop background music

            // start screen before gameplay activates
            if (!GameStarted) // if the Game Started variable is false then...
            {
                // draw startscreen
                DrawTexture(startground, 0.0, 0.0, WHITE); // background image
                DrawText("Press Space Key to Start", (windowDimensions[0]/4), (windowDimensions[1]*0.87), 20, BLACK); // text instruction in black
                DrawText("Don't hit the burning coals!", (windowDimensions[0]/4), (windowDimensions[1]*0.93), 20, RED); // additional text instruction in red
                
                if(IsKeyDown(KEY_SPACE)) // if user presses space bar then...
                {
                    GameStarted = true; // activate Game Start function and
                    PlaySound(soundPass); // play pass sound so user has audio prompt of action and start of gameplay
                }
            }
            else // if the if statement above is not true then do the following
            {

            // scroll and cycle the background
            bgX -= 20 * dT; // Updating background x position by 20 pixels per second and also multiplied by delta time so it is screen rate independent which means that possible system issues or lag will not impact game quality
            if (bgX <= -background.width) // if statement to check when we have reached end of 2nd copy of far background by checking if it is equal or less than the negtive value of the background images width (at a scale of 2). When activated, the first copy of the background texture's position will be reset and will return back to starting position ready for next background cycle
            {
                bgX = 0.0; // if true then return background x position to zero, ready for next background animation cycle
            }

            // scroll and cycle the midground
            mgX -= 40* dT; // Updating background x position by 40 pixels per second and also multiplied by delta time so it is screen rate independent which means that possible system issues or lag will not impact game quality
            if (mgX <= -midground.width) // if statement to check when we have reached end of 2nd copy of far background by checking if it is equal or less than the negtive value of the background images width (at a scale of 2). When activated, the first copy of the background texture's position will be reset and will return back to starting position ready for next background cycle
            {
                mgX = 0.0; // if true then return midground x position to zero, ready for next background animation cycle
            }

            // scroll and cycle the foreground
            fgX -= 80* dT; // Updating background x position by 60 pixels per second and also multiplied by delta time so it is screen rate independent which means that possible system issues or lag will not impact game quality
            if (fgX <= -foreground.width) // if statement to check when we have reached end of 2nd copy of far background by checking if it is equal or less than the negtive value of the background images width (at a scale of 2). When activated, the first copy of the background texture's position will be reset and will return back to starting position ready for next background cycle
            {
                fgX = 0.0; // if true then return foreground x position to zero, ready for next background animation cycle
            }

            // draw background
            Vector2 bg1Pos{bgX, 0.0}; // defining the vector 2 or position of the upcoming background image, this will fit the window resting at the zero point of the window. Note that the x position of the background will shift 20 pixels every second to simulate the movement
            DrawTextureEx(background, bg1Pos, 0.0, 1.0, WHITE); // similar to a texture 2d but with added variables for vector 2 and floats. Note that I am increasing scale of background image to 2 and without any tint or further adjustment like rotation
            Vector2 bg2Pos{bgX + background.width, 0.0}; //creating another vector2 for the subsquent backgrounds, also scaled by 2
            DrawTextureEx(background, bg2Pos, 0.0, 1.0, WHITE); // drawing the multiple background texture using same variables as first by with the second vector

            // draw midground
            Vector2 mg1Pos{mgX, 0.0}; // defining the vector 2 or position of the upcoming midground image, this will fit the window resting at the zero point of the window. Note that the x position of the background will shift 40 pixels every second to simulate the movement
            DrawTextureEx(midground, mg1Pos, 0.0, 1.0, WHITE); // similar to a texture 2d but with added variables for vector 2 and floats. Note that I am increasing scale of midground image to 2 and without any tint or further adjustment like rotation
            Vector2 mg2Pos{mgX + midground.width, 0.0}; //creating another vector2 for the subsquent midgrounds, also scaled by 2
            DrawTextureEx(midground, mg2Pos, 0.0, 1.0, WHITE); // drawing the multiple midground texture using same variables as first by with the second vector

            // draw foreground
            Vector2 fg1Pos{fgX, 0.0}; // defining the vector 2 or position of the upcoming foreground image, this will fit the window resting at the zero point of the window. Note that the x position of the foreground will shift 80 pixels every second to simulate the movement
            DrawTextureEx(foreground, fg1Pos, 0.0, 1.0, WHITE); // similar to a texture 2d but with added variables for vector 2 and floats. Note that I am increasing scale of background image to 2 and without any tint or further adjustment like rotation
            Vector2 fg2Pos{fgX + foreground.width, 0.0}; //creating another vector2 for the subsquent foregrounds, also scaled by 2
            DrawTextureEx(foreground, fg2Pos, 0.0, 1.0, WHITE); // drawing the multiple foreground texture using same variables as first by with the second vector

            // draw start screen
            Vector2 ss1Pos{ssX, 0.0}; // defining the vector 2 or position of the upcoming foreground image, this will fit the window resting at the zero point of the window. Note that the x position of the foreground will shift 80 pixels every second to simulate the movement
            DrawTextureEx(foreground, fg1Pos, 0.0, 1.0, WHITE); // similar to a texture 2d but with added variables for vector 2 and floats. Note that I am increasing scale of background image to 2 and without any tint or further adjustment like rotation

            // ground check
            if (groundCheck(santaData,windowDimensions[1])) // if statement that is calling in the ground check function using the anim data and character height, this returns a boolean result 
            {
                // rectangle is on the ground
                velocity = 0; // resetting velocity back to zero since we are on the ground
                isInAir = false; // logging that we are in air so we can activate jumping feature
            }
            else // when rectangle is in the air do the following (ie apply gravity to bring back down)
            {
                // air check
                velocity += gravity * dT; // apply gravity by adding gravity pixel everytime which brings character back down
                isInAir = true; // logging that we are not in air so we can deactivate jumping feature
            }

            // jump check
            if (IsKeyPressed(KEY_SPACE) && isInAir == false) // to detect and action against space bar being pressed but only if air detection is not identified which prevents double jumping
            {
                velocity += jumpVel; // reducing velocity by negative 10 moves upwards and simulates jump on space bar click
                PlaySound(soundJump);
            }

            // reduce rising when jump is released
            if (IsKeyReleased(KEY_SPACE) && velocity < 0) // when space bar is released and velocity variable is below zero then...
            {
                velocity = velocity/2; //adjust velocity varible by half
            }

            // update hazard position
            for (int i = 0; i < hazardCount; i++) // for loop, initialise variable i as zero, then when hazard count is less than i and one i
            {
               coals[i].pos.x += coalVel * dT;  // update x position every frame for each of the incoming hazards and making frame independent by multipying delta time which changes to pixels by frame
            }

            // update finish line
            finishLine += coalVel * dT; // scrolling finish line to left at the same speed as the hazards, achieved by using the hazards velocity multiplied by the delta time

            // update character position
            santaData.pos.y += velocity * dT; // update y position every frame for the movement and making frame independent by multipying delta time which changes to pixels by frame

            // update character update frame
            if (!isInAir) // using an if statement, only enabling animation through update of running time if the is in air check is not activated - this means that santa character will stop running when they are jumping for more realism to the character movement
            {
                // update character running time
                santaData = updateAnimData(santaData, dT,5); // call update anim data function and use its return value to update santa animation frame data
            }
            
            // update animation frame for all hazards 
            for (int i = 0; i < hazardCount; i++) // using a for loop which will loop over every element of the hazard coals array then update its animation frame in a single location which is a more efficient practice rather than coding the animation frame updates for each hazard 
            {
            // update hazard animation frame
                coals[i] = updateAnimData(coals[i], dT, 7); // call update anim data function and use its return value to update santa animation frame data
            }

            // collision rectangle to enable accurate collision detection between character and hazard
            for(AnimData coal : coals) // range based for loop to check for collision
            {
                float padding{60}; // in order for the collision rectangle to more accurately pick the hazard image and not its blank border in the sprite sheet, I am defining the border around the hazard images in the sprite sheet of 60 to later trim for better gameplay (i.e. collision is with actual hazard and not area around hazard)
                // hazard collision rectangle
                Rectangle coalRec //rectangle is used to locate the hazard on the sprite sheet
                    { 
                        coal.pos.x + padding, // x position of hazard image in the sprite sheet plus padding, so we're closer to the hazard image by 20, trimming its blank border
                        coal.pos.y + padding, // y position of hazard image in the sprite sheet plus padding, so we're closer to the hazard image by 20, trimming its blank border
                        coal.rec.width - 2*padding, // width of collision rectangle minus the padding around the hazard image in the sprite sheet
                        coal.rec.height - 2*padding // height of collision rectangle minus the padding around the hazard image in the sprite sheet
                    };
                // character collision rectangle
                Rectangle santaRec //rectangle is used to locate the character on the sprite sheet
                    { 
                        santaData.pos.x, // x position of hazard image in the sprite sheet
                        santaData.pos.y, // y position of hazard image in the sprite sheet
                        santaData.rec.width, // width of collision rectangle
                        santaData.rec.height // height of collision rectangle 
                    };
                if (CheckCollisionRecs(santaRec, coalRec)) // if statement to activate if the character rectangle collides with hazard rectangle, and if true...
                {
                    collision = true; // update the collision variable to true if collision occurs (note that this will activate a lose outcome)
                }
            }

            if (collision) // if statement for when collision variable is activated
            {
                // lose the game
                PlaySound(soundFail); // play fail sound
                DrawText("SAD SANTA!",windowDimensions[0]/5, 2*windowDimensions[1]/5,50,BLACK); // write text dark shadow in black
                DrawText("SAD SANTA!",windowDimensions[0]/5-4, 2*windowDimensions[1]/5-3,50,WHITE); // write text light shadow in white
                DrawText("SAD SANTA!",windowDimensions[0]/5-5, 2*windowDimensions[1]/5-4,50,RED); // write text front in green

                // restart the game
                if(IsKeyDown(KEY_SPACE)) // if space key is pressed then...
                {
                    collision = false; //reset collision variable to allow game restart
                    GameStarted = false; //reset game started variable to return to start screen
                }
            }
            else if (santaData.pos.x >= finishLine) // if above if statement is not true then...
            {
                // win the game
                PlaySound(soundPass); // play sound file called SoundPass
                DrawText("HO HO HOORAY!",windowDimensions[0]/7, 2*windowDimensions[1]/5,50,BLACK); // write text dark shadow in black
                DrawText("HO HO HOORAY!",windowDimensions[0]/7-4, 2*windowDimensions[1]/5-3,50,WHITE);  // write text light shadow in white
                DrawText("HO HO HOORAY!",windowDimensions[0]/7-5, 2*windowDimensions[1]/5-4,50,GREEN);  // write text front in green
            }
            else
            {
                // draw hazard for loop (only if collision has not occured)
                for (int i = 0; i < hazardCount; i++) // standard for loop, we are naming the integer variable i for index, initialising it at zero, setting the condition to be less than the hazard count constant variable then, if it passes the condition, adding the increment of 1
                {
                    DrawTextureRec(coal, coals[i].rec, coals[i].pos, WHITE); // calling upon the hazard variables for image, size and position without tint
                }

                // draw santa character (only if collision has not occured)
                DrawTextureRec(santa, santaData.rec, santaData.pos, WHITE); // calling upon the character variables for image, size and position without tint
            }
            }
            EndDrawing(); //stop all drawing activity
        }
    // unloading data processing upon program closure
    UnloadTexture(santa); //unloading the character texture and shutting things down properly for efficiency
    UnloadTexture(coal); //unloading the hazard texture and shutting things down properly for efficiency
    UnloadTexture(background); //unloading the background texture and shutting things down properly for efficiency
    UnloadTexture(midground); //unloading the background texture and shutting things down properly for efficiency
    UnloadTexture(foreground); //unloading the background texture and shutting things down properly for efficiency
    CloseAudioDevice(); //close audio function before program termination for efficiency
    CloseWindow(); //close window function before program termination for efficiency
}