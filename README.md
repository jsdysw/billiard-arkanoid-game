# billiard-arkanoid-game

<img src="https://user-images.githubusercontent.com/76895949/154515627-4978f832-26a0-44b3-84c0-58c1d05a7bdc.png" width="400px" height="400px"/><img src="https://user-images.githubusercontent.com/76895949/154515646-611fbe1d-d67f-4f1d-86a6-466f86586fa4.png" width="400px" height="400px"/>

- you can move the white ball with the mouse movement.
- space : start the game.
- esc : exit the program.
- You can watch the play video through demoVideo.mp4.

## Environment
- Downloaded DirectX.
- OS type : Windows
- Used Language : ISO C++14 Standard
- Compiler type/version : Microsoft (R) C/C++ Optimizing Compiler Version 19.28.29913 for x86

## How to Execute
- Make sure you downloaded DirectX first.
- Option 1. make a visual studio project -> set headerfile path -> copy source files from this repository -> click Debug/Start Without Debugging.
- Option 2. execute "VirtualLego.exe" at root folder

## Summary of Code
- const float spherePos[][] : it contains the coordinate of yellow balls which you have to hit and erase
- CSphere::hitBy(CSphere& ball) : check if two balls collide, and calculate the new reflected movement direction and speed.
- CSphere::ballUpdate() : update ball's state(Position, power, ..etc) at every frame(timeDiff). Additionally, make the ball safe not to go outside of the board
- CWall::hitBy(CSphere& ball) : check if the ball hits walls, and calculate the new reflected movement direction and speed.
- Setup() : This function is called once and setup all the initial states of the Map
- Display(float timeDelta) : This function is called every frame. Detect collide events and update all the states of the balls before drawing
- CALLBACK d3d::WndProc(..) : WM_MOUSEMOVE(move white ball), WM_KEYDWON, VK_SPACE(start game)
