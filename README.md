# LonelyDriver
### Progress log
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

<img src="./pics/progress_20231228.png" width="200" /><img src="./pics/progress_20231228_2.png" width="200" />

### TODOs
- [x] clean way to render wireframes (bounding box)
- [ ] terrain
- [ ] vehicle physics and user control 
- [ ] change pointers to smart pointers
- [ ] should I move all dependencies (imgui, glfw, glm...) into solution folder?
