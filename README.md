# LonelyDriver
### Dev log
- 12/13/2023: Initiate project
- 12/14/2023: Created singleton Window class
- 12/15/2023: Render car model
- 12/19/2023: Add light source and road (plane)

<img src="./pics/progress_20231219.png" width="400" />

- 12/21/2023: Successfully run PhysX snippet (Hello world and Vehicle drive)

<img src="./pics/progress_20231221_1.png" width="200" /><img src="./pics/progress_20231221_2.png" width="200" />

- 12/27/2023: Added gravity to car model and light cube
- separated phisics functionalities into a singleton class

<img src="./pics/progress_20231227.gif" width="400" />

- 12/28/2023: Draw skybox using cubemap. Created `Skybox` class that derives `Drawable` for special draw calls.
- Toggle var `DrawWireframe` in `App` class to draw wireframe.  

<img src="./pics/progress_20231228.png" width="400" /><img src="./pics/progress_20231228_2.png" width="400" />

- 12/31/2023: Generated terrain using height map with Tesselation shader.

<img src="./pics/progress_20231230_CPUterrain.png" width="400" />

- 1/3/2024: changed car model (fixed missing textures) and use [semi-fixed time step](https://gafferongames.com/post/fix_your_timestep/) for render loop and physics step stability.

<img src="./pics/progress_20240103.gif" width="400" />

- 1/4/2024 Import and draw fbx model recursively \
This is for animating wheel direction when turing. I'm not sure why the transformation hierarchy works as I am applying scaling at the left-most of the `model` matrix (`M = T_drawOffset * S * T_2 * T_1 * R`), I thought it should be ` M = T_drawOffset * T_2 * T_1 * R * S`.

<img src="./pics/progress_20240104.gif" width="400" />

- 1/6/2024 Added physX vehicle simulation and attached with car model. \
The car is moving in pre-choreographed sequence.
<img src="./pics/progress_20240106.gif" width="400" />



### Next work
User input control acceleration and brake.

> [!WARNING]  
> I haven't tried to apply PhysX height feild, Not sure if it suits for collision with vehicle so I didn't try it yet. Now I am just going to let the vehicle run on a plane and add textures to the plane instead of using the tesselated terrain.

### TODOs
- [x] clean way to render wireframes (bounding box)
- [ ] vehicle physics and user control 
- [ ] terrain, compare CPU vs GPU generation peformance (speed, memory)
- [ ] add normal mapping in model shder
- [ ] change pointers to smart pointers
- [ ] should I move all dependencies (imgui, glfw, glm...) into solution folder?
